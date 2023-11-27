use serde::{Deserialize, Serialize};
use std::collections::BTreeMap;

use super::types::Type;

#[derive(Debug, Deserialize, Serialize)]
pub struct Interface {
    pub description: String,
    #[serde(default)]
    pub cmds: BTreeMap<String, Command>,
    #[serde(default)]
    pub vars: BTreeMap<String, Type>,
    // TODO(ddo) We allow unknown fields for now to ignore the `error` interface
    // which is still under construction.
}

#[derive(Debug, Deserialize, Serialize)]
#[serde(deny_unknown_fields)]
pub struct Command {
    pub description: String,
    #[serde(default)]
    pub arguments: BTreeMap<String, Type>,
    pub result: Option<Type>,
}
