use serde::{Deserialize, Serialize};

/// Implements the schema defined under `error-declaration.yaml`. Every type has
/// mandatory `name` and `description` fields.
#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct Error {
    /// The description of the error.
    pub description: String,

    /// The name of the error.
    pub name: String,

    /// The namespace of the error.
    pub namespace: Option<String>,
}

/// Implements the list of errors.
#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct ErrorList {
    /// The description of all errors in the file.
    pub description: String,

    /// The list of errors.
    /// We add default to allow make the `errors` field optional.
    #[serde(default)]
    pub errors: Vec<Error>,
}

#[cfg(test)]
mod tests {
    use super::*;
    use serde_yaml;

    #[test]
    fn test_deserialization() {
        // Test with the list.
        let _ = serde_yaml::from_str::<ErrorList>(
            r#"
            description: this is a description
            errors:
                - name: foo
                  description: bar
                - name: this
                  description: that
            "#,
        )
        .unwrap();

        // Test without the list
        let _ = serde_yaml::from_str::<ErrorList>(
            r#"
            description: just a description without errors
            "#,
        )
        .unwrap();
    }
}
