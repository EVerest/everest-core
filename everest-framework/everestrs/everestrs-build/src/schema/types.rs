use serde::{Deserialize, Deserializer, Serialize};
use std::collections::{BTreeMap, HashSet};

/// Implements the schema defined under `type.yaml`. Every type has a `type`
/// and a `description` field.
#[derive(Debug, Deserialize, Serialize)]
pub struct Type {
    // TODO(ddo) The schema says that this field is required, but multiple
    // type definitions do not obey this rule.
    pub description: Option<String>,

    #[serde(flatten)]
    pub arg: TypeBase,

    /// This is part of the Variable definition.
    pub qos: Option<i64>,
}

/// The type may be either represented by a string or by an array of strings.
/// In the case of an array of strings.
#[derive(Debug, Serialize)]
pub enum TypeBase {
    Single(TypeEnum),
    Multiple(Vec<TypeEnum>),
}

impl<'de> Deserialize<'de> for TypeBase {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        let serde_yaml::Value::Mapping(map) = Deserialize::deserialize(deserializer)? else {
            return Err(serde::de::Error::custom("Variable must be a mapping"));
        };

        let arg_type = map
            .get("type")
            .ok_or("The `type` tag is missing")
            .map_err(|e| serde::de::Error::custom(e.to_string()))?;

        let arg = match arg_type {
            serde_yaml::Value::String(_) => {
                let t: TypeEnum = serde_yaml::from_value(serde_yaml::Value::Mapping(map))
                    .map_err(|e| serde::de::Error::custom(e.to_string()))?;
                TypeBase::Single(t)
            }
            serde_yaml::Value::Sequence(s) => {
                let mut types = Vec::with_capacity(s.len());
                for t in s.into_iter() {
                    let mut mapping = serde_yaml::Mapping::new();
                    mapping.insert(serde_yaml::Value::String("type".to_string()), t.clone());
                    let t: TypeEnum = serde_yaml::from_value(serde_yaml::Value::Mapping(mapping))
                        .map_err(|e| serde::de::Error::custom(e.to_string()))?;
                    types.push(t);
                }
                TypeBase::Multiple(types)
            }
            _ => {
                return Err(serde::de::Error::custom(
                    "'type' must be a sequence or a string.",
                ))
            }
        };

        Ok(arg)
    }
}

#[derive(Clone, Debug, Deserialize, Serialize)]
#[serde(deny_unknown_fields)]
pub struct BooleanOptions {
    pub default: Option<bool>,
}

#[derive(Clone, Debug, Deserialize, Serialize)]
#[serde(deny_unknown_fields)]
pub struct NumberOptions {
    pub minimum: Option<f64>,
    pub maximum: Option<f64>,
    pub default: Option<f64>,
}

#[derive(Clone, Debug, Deserialize, Serialize)]
#[serde(deny_unknown_fields)]
pub struct IntegerOptions {
    pub minimum: Option<i64>,
    pub maximum: Option<i64>,
    pub default: Option<i64>,
}

#[derive(Debug, Deserialize, Serialize)]
#[serde(rename_all = "camelCase", deny_unknown_fields)]
pub struct ArrayOptions {
    pub min_items: Option<usize>,
    pub max_items: Option<usize>,
    pub items: Option<Box<Type>>,
}

#[derive(Debug, Deserialize, Serialize)]
#[serde(rename_all = "camelCase", deny_unknown_fields)]
pub struct ObjectOptions {
    #[serde(default)]
    pub properties: BTreeMap<String, Type>,

    #[serde(default)]
    pub required: HashSet<String>,

    #[serde(default)]
    pub additional_properties: bool,

    #[serde(rename = "$ref")]
    pub object_reference: Option<String>,
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub enum StringFormat {
    #[serde(rename = "date-time")]
    DateTime,
}

#[derive(Clone, Debug, Deserialize, Serialize)]
#[serde(rename_all = "camelCase", deny_unknown_fields)]
pub struct StringOptions {
    pub pattern: Option<String>,
    pub format: Option<StringFormat>,
    pub max_length: Option<usize>,
    pub min_length: Option<usize>,

    #[serde(rename = "enum")]
    pub enum_items: Option<Vec<String>>,

    pub default: Option<String>,

    #[serde(rename = "$ref")]
    pub object_reference: Option<String>,
}

#[derive(Debug, Deserialize, Serialize)]
#[serde(rename_all = "camelCase", tag = "type", deny_unknown_fields)]
pub enum TypeEnum {
    Null,
    Boolean(BooleanOptions),
    String(StringOptions),
    Number(NumberOptions),
    Integer(IntegerOptions),
    Array(ArrayOptions),
    Object(ObjectOptions),
}

#[derive(Debug, Deserialize)]
#[serde(deny_unknown_fields)]
pub struct DataTypes {
    pub description: String,
    pub types: BTreeMap<String, Type>,
}
