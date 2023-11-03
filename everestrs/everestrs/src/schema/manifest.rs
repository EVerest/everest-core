use serde::Deserialize;
use std::collections::BTreeMap;

#[derive(Debug, Deserialize)]
pub struct Manifest {
    pub description: String,
    pub provides: BTreeMap<String, ProvidesEntry>,
    pub requires: BTreeMap<String, RequiresEntry>,
    pub metadata: Metadata,
}

#[derive(Debug, Deserialize)]
pub struct YamlData {
    pub description: String,
}

#[derive(Debug, Deserialize)]
pub struct ProvidesEntry {
    pub interface: String,
    pub description: String,
}

#[derive(Debug, Deserialize)]
pub struct RequiresEntry {
    pub interface: String,
    pub max_connections: i32,
    pub min_connections: i32,
}

#[derive(Debug, Deserialize)]
pub struct Metadata {
    pub license: String,
    pub authors: Vec<String>,
}
