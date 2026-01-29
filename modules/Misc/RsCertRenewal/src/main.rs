//! Certificate renewal module for EVerest.
//!
//! This module monitors certificates in a configured directory for expiration and
//! automatically fetches new certificates from a configured endpoint when they are
//! about to expire.
//!
//! ## Configuration
//!
//! - `endpoint`: URL to fetch new certificates from
//! - `cert_dir`: Directory containing certificates to monitor
//! - `check_interval_seconds`: How often to check certificates (default: 3600)
//! - `expiry_threshold_days`: Days before expiration to trigger renewal (default: 30)
//!
//! ## Example usage
//!
//! ```yaml
//! active_modules:
//!   cert_renewal:
//!     module: RsCertRenewal
//!     config_module:
//!       endpoint: "https://example.com/certs/renew"
//!       cert_dir: "/etc/everest/certs"
//!       check_interval_seconds: 3600
//!       expiry_threshold_days: 30
//! ```
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use anyhow::{Context, Result};
use chrono::{DateTime, Utc};
use generated::{get_config, Module, ModulePublisher};
use std::fs;
use std::path::Path;
use std::sync::Arc;
use std::thread;
use std::time::Duration;

/// Configuration extracted from the generated config
#[derive(Clone)]
struct Config {
    endpoint: String,
    cert_dir: String,
    check_interval_seconds: i64,
    expiry_threshold_days: i64,
}

/// Represents a certificate file with its metadata
struct CertificateInfo {
    path: String,
    expiry: DateTime<Utc>,
    subject: String,
}

/// Parses a PEM-encoded certificate file and extracts expiry information
fn parse_certificate(path: &Path) -> Result<CertificateInfo> {
    let content = fs::read_to_string(path)
        .with_context(|| format!("Failed to read certificate file: {}", path.display()))?;

    // Use x509-parser's PEM parsing
    let (_, pem) = x509_parser::pem::parse_x509_pem(content.as_bytes())
        .map_err(|e| anyhow::anyhow!("Failed to parse PEM from {}: {:?}", path.display(), e))?;

    let cert = pem.parse_x509()
        .with_context(|| format!("Failed to parse X509 certificate: {}", path.display()))?;

    let not_after = cert.validity().not_after;
    let expiry = DateTime::from_timestamp(not_after.timestamp(), 0)
        .ok_or_else(|| anyhow::anyhow!("Invalid timestamp in certificate"))?;

    let subject = cert.subject().to_string();

    Ok(CertificateInfo {
        path: path.display().to_string(),
        expiry,
        subject,
    })
}

/// Scans a directory for certificate files (.pem, .crt, .cer)
fn scan_certificates(cert_dir: &str) -> Result<Vec<CertificateInfo>> {
    let mut certs = Vec::new();
    let dir_path = Path::new(cert_dir);

    if !dir_path.exists() {
        log::warn!("Certificate directory does not exist: {}", cert_dir);
        return Ok(certs);
    }

    let entries = fs::read_dir(dir_path)
        .with_context(|| format!("Failed to read certificate directory: {}", cert_dir))?;

    for entry in entries {
        let entry = entry?;
        let path = entry.path();

        if path.is_file() {
            if let Some(ext) = path.extension() {
                let ext = ext.to_string_lossy().to_lowercase();
                if ext == "pem" || ext == "crt" || ext == "cer" {
                    match parse_certificate(&path) {
                        Ok(cert_info) => {
                            log::debug!(
                                "Found certificate: {} (expires: {})",
                                cert_info.subject,
                                cert_info.expiry
                            );
                            certs.push(cert_info);
                        }
                        Err(e) => {
                            log::warn!("Failed to parse certificate {}: {}", path.display(), e);
                        }
                    }
                }
            }
        }
    }

    Ok(certs)
}

/// Checks if a certificate is expiring soon
fn is_expiring_soon(cert: &CertificateInfo, threshold_days: i64) -> bool {
    let now = Utc::now();
    let days_until_expiry = (cert.expiry - now).num_days();
    days_until_expiry <= threshold_days
}

/// Fetches a renewed certificate from the configured endpoint
fn fetch_renewed_certificate(endpoint: &str, cert_info: &CertificateInfo) -> Result<String> {
    log::info!(
        "Fetching renewed certificate for: {} from {}",
        cert_info.subject,
        endpoint
    );

    let request_body = format!(
        r#"{{"subject":"{}","current_expiry":"{}","certificate_path":"{}"}}"#,
        cert_info.subject,
        cert_info.expiry.to_rfc3339(),
        cert_info.path
    );

    let response = ureq::post(endpoint)
        .set("Content-Type", "application/json")
        .send_string(&request_body)
        .with_context(|| format!("Failed to contact renewal endpoint: {}", endpoint))?;

    if response.status() != 200 {
        anyhow::bail!(
            "Renewal endpoint returned error status: {}",
            response.status()
        );
    }

    let new_cert = response
        .into_string()
        .with_context(|| "Failed to read response body")?;

    Ok(new_cert)
}

/// Saves a renewed certificate to disk
fn save_certificate(path: &str, content: &str) -> Result<()> {
    fs::write(path, content).with_context(|| format!("Failed to write certificate to: {}", path))?;

    log::info!("Saved renewed certificate to: {}", path);
    Ok(())
}

/// Performs a certificate check and renewal cycle
fn check_and_renew_certificates(config: &Config) -> Result<()> {
    if config.cert_dir.is_empty() {
        log::debug!("No certificate directory configured, skipping check");
        return Ok(());
    }

    log::info!("Checking certificates in: {}", config.cert_dir);

    let certificates = scan_certificates(&config.cert_dir)?;
    log::info!("Found {} certificates to monitor", certificates.len());

    for cert in &certificates {
        let days_until_expiry = (cert.expiry - Utc::now()).num_days();
        log::info!(
            "Certificate '{}' expires in {} days ({})",
            cert.subject,
            days_until_expiry,
            cert.expiry
        );

        if is_expiring_soon(cert, config.expiry_threshold_days) {
            log::warn!(
                "Certificate '{}' is expiring soon ({} days), attempting renewal",
                cert.subject,
                days_until_expiry
            );

            if config.endpoint.is_empty() {
                log::error!(
                    "No renewal endpoint configured, cannot renew certificate: {}",
                    cert.subject
                );
                continue;
            }

            match fetch_renewed_certificate(&config.endpoint, cert) {
                Ok(new_cert) => {
                    if let Err(e) = save_certificate(&cert.path, &new_cert) {
                        log::error!("Failed to save renewed certificate: {}", e);
                    }
                }
                Err(e) => {
                    log::error!("Failed to fetch renewed certificate: {}", e);
                }
            }
        }
    }

    Ok(())
}

/// Main module struct
struct CertRenewalModule {
    config: Config,
}

impl generated::OnReadySubscriber for CertRenewalModule {
    fn on_ready(&self, _publishers: &ModulePublisher) {
        log::info!("RsCertRenewal module is ready");
        log::info!("Configuration:");
        log::info!("  cert_dir: {}", self.config.cert_dir);
        log::info!("  endpoint: {}", self.config.endpoint);
        log::info!(
            "  check_interval_seconds: {}",
            self.config.check_interval_seconds
        );
        log::info!(
            "  expiry_threshold_days: {}",
            self.config.expiry_threshold_days
        );

        // Perform initial check
        if let Err(e) = check_and_renew_certificates(&self.config) {
            log::error!("Initial certificate check failed: {}", e);
        }
    }
}

impl generated::EmptyServiceSubscriber for CertRenewalModule {}

fn main() {
    let module_config = get_config();
    log::info!("Starting RsCertRenewal module");

    // Extract config into our own struct that implements Clone
    let config = Config {
        endpoint: module_config.endpoint.clone(),
        cert_dir: module_config.cert_dir.clone(),
        check_interval_seconds: module_config.check_interval_seconds,
        expiry_threshold_days: module_config.expiry_threshold_days,
    };

    let check_interval = Duration::from_secs(config.check_interval_seconds as u64);
    let config_for_loop = config.clone();

    let module = Arc::new(CertRenewalModule { config });

    let _everest_module = Module::new(module.clone(), module.clone());

    // Main loop for periodic certificate checks
    loop {
        thread::sleep(check_interval);

        if let Err(e) = check_and_renew_certificates(&config_for_loop) {
            log::error!("Certificate check failed: {}", e);
        }
    }
}
