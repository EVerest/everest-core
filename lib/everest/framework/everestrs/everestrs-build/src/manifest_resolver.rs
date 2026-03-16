use crate::schema;
use crate::schema::manifest::{Manifest, ProvidesEntry, RequiresEntry};
use anyhow::{bail, Context, Result};
use std::collections::BTreeMap;
use std::path::{Path, PathBuf};

mod inner {
    use super::*;
    /// A cache that lazily loads and stores module manifests by module type name.
    pub(super) struct ManifestCache<'a> {
        everest_core: &'a [PathBuf],
        entries: BTreeMap<String, (PathBuf, Manifest)>,
    }

    impl<'a> ManifestCache<'a> {
        pub(super) fn new(everest_core: &'a [PathBuf]) -> Self {
            Self {
                everest_core,
                entries: BTreeMap::new(),
            }
        }

        /// Returns the manifest for `module_type`, loading it on first access.
        pub(super) fn get(&mut self, module_type: &str) -> Result<&Manifest> {
            if !self.entries.contains_key(module_type) {
                let (path, manifest) = find_manifest(module_type, self.everest_core)?;
                self.entries
                    .insert(module_type.to_string(), (path, manifest));
            }
            Ok(&self.entries[module_type].1)
        }

        /// Returns the paths of all manifests that were loaded.
        pub(super) fn into_paths(self) -> impl Iterator<Item = PathBuf> {
            self.entries.into_values().map(|(path, _)| path)
        }
    }

    /// Recursively searches `dir` for a directory named `module_type` containing
    /// a `manifest.yaml`. Returns the path to the manifest on first match.
    /// Symlinks are skipped to avoid circular traversal.
    fn find_manifest_in(dir: &Path, module_type: &str) -> Option<PathBuf> {
        let entries = std::fs::read_dir(dir).ok()?;
        for entry in entries.flatten() {
            let ft = match entry.file_type() {
                Ok(ft) => ft,
                Err(_) => continue,
            };
            if ft.is_symlink() {
                continue;
            }
            let path = entry.path();
            if path.file_name().map_or(false, |n| n == module_type) {
                let manifest = path.join("manifest.yaml");
                if manifest.is_file() {
                    return Some(manifest);
                }
            }
            if ft.is_dir() {
                if let Some(found) = find_manifest_in(&path, module_type) {
                    return Some(found);
                }
            }
        }
        None
    }

    /// Finds a module manifest by searching for `{ModuleName}/manifest.yaml`
    /// anywhere under each everest_core root. Modules can be nested arbitrarily
    /// deep (e.g. `modules/Examples/RustExamples/RsExample/manifest.yaml`).
    /// Symlinks are skipped to avoid circular traversal.
    fn find_manifest(module_type: &str, everest_core: &[PathBuf]) -> Result<(PathBuf, Manifest)> {
        for root in everest_core {
            if let Some(path) = find_manifest_in(root, module_type) {
                let blob = std::fs::read_to_string(&path)
                    .with_context(|| format!("Failed to read {path:?}"))?;
                let manifest: Manifest = serde_yaml::from_str(&blob)
                    .with_context(|| format!("Failed to parse {path:?}"))?;
                return Ok((path, manifest));
            }
        }
        bail!("Could not find manifest for module type '{module_type}' in any everest_core root");
    }
}

/// Reads config.yaml, finds the target module instance, deduces its
/// interfaces from connected modules' manifests, returns a synthetic
/// Manifest and the list of files read (for dependency tracking).
pub fn build_test_manifest(
    config_path: &Path,
    module_instance: &str,
    everest_core: &[PathBuf],
) -> Result<(Manifest, Vec<PathBuf>)> {
    let blob = std::fs::read_to_string(config_path)
        .with_context(|| format!("While reading config {config_path:?}"))?;
    let config: schema::Config =
        serde_yaml::from_str(&blob).with_context(|| format!("While parsing {config_path:?}"))?;

    let target = config.active_modules.get(module_instance).ok_or_else(|| {
        anyhow::anyhow!("Module instance '{module_instance}' not found in {config_path:?}")
    })?;

    let mut cache = inner::ManifestCache::new(everest_core);

    // Step 1: Resolve outgoing connections (what the target module requires).
    // For each connection slot in the target module, find what interface the
    // connected module provides at that implementation_id.
    let mut requires = BTreeMap::new();
    for (slot_name, connections) in &target.connections {
        for conn in connections {
            let connected_module = config.active_modules.get(&conn.module_id).ok_or_else(|| {
                anyhow::anyhow!("Connected module '{}' not found in config", conn.module_id)
            })?;

            let connected_manifest = cache.get(&connected_module.module)?;

            let provides_entry = connected_manifest
                .provides
                .get(&conn.implementation_id)
                .ok_or_else(|| {
                    anyhow::anyhow!(
                        "Module type '{}' does not provide '{}'",
                        connected_module.module,
                        conn.implementation_id
                    )
                })?;

            requires.insert(
                slot_name.clone(),
                RequiresEntry {
                    interface: provides_entry.interface.clone(),
                    min_connections: None,
                    max_connections: None,
                    ignore: Default::default(),
                },
            );
        }
    }

    // Step 2: Resolve incoming connections (what the target module must provide).
    // Scan all other modules' connections to find ones pointing at our target.
    let mut provides = BTreeMap::new();
    for (other_id, other_module) in &config.active_modules {
        if other_id == module_instance {
            continue;
        }
        for (other_slot, connections) in &other_module.connections {
            for conn in connections {
                if conn.module_id != module_instance {
                    continue;
                }
                // other_module requires interface via other_slot,
                // connected to our target's conn.implementation_id.
                let other_manifest = cache.get(&other_module.module)?;

                let requires_entry = other_manifest.requires.get(other_slot).ok_or_else(|| {
                    anyhow::anyhow!(
                        "Module type '{}' does not require '{}'",
                        other_module.module,
                        other_slot
                    )
                })?;

                provides.insert(
                    conn.implementation_id.clone(),
                    ProvidesEntry {
                        interface: requires_entry.interface.clone(),
                        description: format!(
                            "Auto-generated from {}.{} connection",
                            other_id, other_slot
                        ),
                        config: BTreeMap::new(),
                    },
                );
            }
        }
    }

    let manifest = Manifest {
        description: format!("Synthetic test manifest for {module_instance}"),
        metadata: None,
        provides,
        requires,
        enable_telemetry: false,
        enable_external_mqtt: false,
        config: BTreeMap::new(),
        capabilities: Vec::new(),
        enable_global_errors: false,
    };

    let mut tracked_files = vec![config_path.to_path_buf()];
    tracked_files.extend(cache.into_paths());

    Ok((manifest, tracked_files))
}
