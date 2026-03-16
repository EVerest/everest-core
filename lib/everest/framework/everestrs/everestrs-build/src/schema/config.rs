use serde::Deserialize;
use std::collections::BTreeMap;

#[derive(Debug, Deserialize)]
pub struct Config {
    pub active_modules: BTreeMap<String, ActiveModule>,
}

#[derive(Debug, Deserialize)]
pub struct ActiveModule {
    pub module: String,
    #[serde(default)]
    pub connections: BTreeMap<String, Vec<Connection>>,
}

#[derive(Debug, Deserialize)]
pub struct Connection {
    pub module_id: String,
    pub implementation_id: String,
}
