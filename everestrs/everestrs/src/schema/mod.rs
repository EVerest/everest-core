pub mod interface;
pub mod manifest;

// We ignore unknown fields everywhere, because the linting and verification of the YAMLs is done
// by libframework.so. So we only mention the fields we actually care about.

pub use interface::Interface;
pub use manifest::Manifest;
use serde::Deserialize;
use std::collections::BTreeMap;

#[derive(Debug, Deserialize)]
pub struct DataTypes {
    pub description: String,
    pub types: BTreeMap<String, interface::Variable>,
}
