use super::types::{BooleanOptions, IntegerOptions, NumberOptions, StringOptions};
use serde::Deserialize;
use std::collections::BTreeMap;

#[derive(Debug, Deserialize)]
#[serde(deny_unknown_fields)]
pub struct Manifest {
    pub description: String,
    pub metadata: Metadata,
    pub provides: BTreeMap<String, ProvidesEntry>,
    #[serde(default)]
    pub requires: BTreeMap<String, RequiresEntry>,
    #[serde(default)]
    pub enable_telemetry: bool,
    // This is just here, so that we do not crash for deny_unknown_fields,
    // this is never used in Rust code.
    #[allow(dead_code)]
    enable_external_mqtt: bool,

    #[serde(default)]
    pub config: BTreeMap<String, ConfigEntry>,

    #[serde(default)]
    pub capabilities: Vec<String>,

    #[serde(default)]
    pub enable_global_errors: bool,
}

#[derive(Debug, Deserialize)]
#[serde(deny_unknown_fields)]
pub struct ProvidesEntry {
    pub interface: String,
    pub description: String,
    #[serde(default)]
    pub config: BTreeMap<String, ConfigEntry>,
}

#[derive(Debug, Deserialize)]
#[serde(deny_unknown_fields)]
pub struct RequiresEntry {
    pub interface: String,
    pub min_connections: Option<i64>,
    pub max_connections: Option<i64>,
}

#[derive(Debug, Deserialize)]
#[serde(deny_unknown_fields)]
pub struct Metadata {
    pub license: String,
    pub authors: Vec<String>,
}

#[derive(Debug, Clone, Deserialize)]
pub struct ConfigEntry {
    pub description: Option<String>,
    #[serde(flatten)]
    pub value: ConfigEnum,
}

#[derive(Debug, Clone, Deserialize)]
#[serde(rename_all = "camelCase", tag = "type", deny_unknown_fields)]
pub enum ConfigEnum {
    Boolean(BooleanOptions),
    String(StringOptions),
    Integer(IntegerOptions),
    Number(NumberOptions),
}
