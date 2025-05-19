use serde::{Deserialize, Serialize};
use std::collections::BTreeMap;

use super::types::Type;

#[derive(Debug, Deserialize, Serialize)]
#[serde(deny_unknown_fields)]
pub struct Interface {
    pub description: String,
    #[serde(default)]
    pub cmds: BTreeMap<String, Command>,
    #[serde(default)]
    pub vars: BTreeMap<String, Type>,
    /// The error reference represents the entry in the manifest were
    /// we reference an error file.
    #[serde(default)]
    pub errors: Vec<ErrorReference>,
}

/// The same as the one above but the cpp runtime returns the errors as a map
/// contrary to the definition inside the yaml file...
#[derive(Debug, Deserialize, Serialize)]
pub struct InterfaceFromEverest {
    // Note: EVerest config over mqtt does not return descriptions even so
    // they should be necessary.
    #[serde(default)]
    pub description: String,
    #[serde(default)]
    pub cmds: BTreeMap<String, Command>,
    #[serde(default)]
    pub vars: BTreeMap<String, Type>,
}

#[derive(Debug, Deserialize, Serialize)]
#[serde(deny_unknown_fields)]
pub struct Command {
    // Note: EVerest config over mqtt does not return descriptions even so
    // they should be necessary.
    #[serde(default)]
    pub description: String,
    #[serde(default)]
    pub arguments: BTreeMap<String, Type>,
    pub result: Option<Type>,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct ErrorReference {
    pub reference: String,
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_deserialization() {
        serde_yaml::from_str::<Interface>(
            r#"
description: >-
  This is an example interface used for the error framework example modules.
errors:
  - reference: /errors/example#/ExampleErrorA
  - reference: /errors/example#/ExampleErrorB
  - reference: /errors/example#/ExampleErrorC
  - reference: /errors/example#/ExampleErrorD

"#,
        )
        .unwrap();

        serde_yaml::from_str::<Interface>(
            r#"
description: Nothing here.
"#,
        )
        .unwrap();
    }
}
