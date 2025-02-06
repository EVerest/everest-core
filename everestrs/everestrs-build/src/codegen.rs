use crate::schema::{
    self,
    interface::ErrorReference,
    manifest::{ConfigEntry, ConfigEnum},
    types::{DataTypes, ObjectOptions, StringOptions, Type, TypeBase, TypeEnum},
    ErrorList, Interface, Manifest,
};
use anyhow::{anyhow, bail, Context, Result};
use convert_case::{Case, Casing};
use minijinja::{Environment, UndefinedBehavior};
use serde::{de::DeserializeOwned, Serialize};
use std::collections::{BTreeMap, BTreeSet, HashMap, HashSet};
use std::fs;
use std::path::PathBuf;

// We include the JINJA templates into the binary. This has the disadvantage
// that every change to the templates requires a recompilation, but the
// advantage that the codegen library/binary is truly standalone and needs
// nothing shipped with it to work.
const CLIENT_JINJA: &str = include_str!("../jinja/client.jinja2");
const CONFIG_JINJA: &str = include_str!("../jinja/config.jinja2");
const ERRORS_JINJA: &str = include_str!("../jinja/errors.jinja2");
const MODULE_JINJA: &str = include_str!("../jinja/module.jinja2");
const SERVICE_JINJA: &str = include_str!("../jinja/service.jinja2");
const TYPES_JINJA: &str = include_str!("../jinja/types.jinja2");

fn is_reserved_keyword(s: &str) -> bool {
    // From https://doc.rust-lang.org/reference/keywords.html.
    matches!(
        s,
        "abstract"
            | "as"
            | "async"
            | "await"
            | "become"
            | "box"
            | "break"
            | "const"
            | "continue"
            | "crate"
            | "do"
            | "dyn"
            | "else"
            | "enum"
            | "extern"
            | "false"
            | "final"
            | "fn"
            | "for"
            | "if"
            | "impl"
            | "in"
            | "let"
            | "loop"
            | "macro"
            | "macro_rules"
            | "match"
            | "mod"
            | "move"
            | "mut"
            | "override"
            | "priv"
            | "pub"
            | "ref"
            | "return"
            | "self"
            | "static"
            | "struct"
            | "super"
            | "trait"
            | "true"
            | "try"
            | "type"
            | "typeof"
            | "union"
            | "unsafe"
            | "unsized"
            | "use"
            | "virtual"
            | "where"
            | "while"
            | "yield"
    )
}

fn lazy_load<'a, T: DeserializeOwned>(
    storage: &'a mut HashMap<String, T>,
    everest_core: &Vec<PathBuf>,
    prefix: &str,
    postfix: &str,
) -> Result<&'a mut T> {
    if storage.contains_key(postfix) {
        return Ok(storage.get_mut(postfix).unwrap());
    }

    let mut matches = everest_core
        .iter()
        .filter_map(|core| {
            let p = core.join(format!("{prefix}/{postfix}.yaml"));
            // If the file is missing we ignore the error since it may be
            // present in an different root.
            let Ok(blob) = fs::read_to_string(&p) else {
                return None;
            };
            let out = serde_yaml::from_str(&blob).with_context(|| format!("Failed to parse {p:?}"));
            match out {
                Err(err) => {
                    println!("{err:?}");
                    None
                }
                Ok(res) => Some(res),
            }
        })
        .collect::<Vec<_>>();

    assert!(
        matches.len() == 1,
        "The name `{prefix}/{postfix}` must be defined exactly once: Found {}",
        { matches.len() }
    );

    storage.insert(postfix.to_string(), matches.pop().unwrap());
    Ok(storage.get_mut(postfix).unwrap())
}

/// A lazy loader for YAML files. If the same file is requested twice, it will
/// not be re-parsed again.
#[derive(Default, Debug)]
struct YamlRepo {
    // This might be also a HashMap of "namespaces" and paths.
    everest_core: Vec<PathBuf>,
    interfaces: HashMap<String, Interface>,
    data_types: HashMap<String, DataTypes>,
    error_types: HashMap<String, ErrorList>,
}

impl YamlRepo {
    pub fn new(everest_core: Vec<PathBuf>) -> Self {
        Self {
            everest_core,
            ..Default::default()
        }
    }

    pub fn get_interface<'a>(&'a mut self, name: &str) -> Result<&'a mut Interface> {
        lazy_load(&mut self.interfaces, &self.everest_core, "interfaces", name)
    }

    pub fn get_data_types<'a>(&'a mut self, name: &str) -> Result<&'a mut DataTypes> {
        lazy_load(&mut self.data_types, &self.everest_core, "types", name)
    }

    pub fn get_errors<'a>(&'a mut self, prefix: &str, name: &str) -> Result<&'a mut ErrorList> {
        lazy_load(&mut self.error_types, &self.everest_core, prefix, name)
    }
}

// We just pull out of ObjectOptions what we really need for codegen.
#[derive(Clone, Hash, PartialOrd, Ord, PartialEq, Eq)]
struct TypeRef {
    /// The same as the file name under everest-core/types.
    module_path: Vec<String>,
    type_name: String,
}

impl TypeRef {
    fn from_object(args: &ObjectOptions) -> Result<Self> {
        assert!(args.object_reference.is_some());
        assert!(
            args.properties.is_empty(),
            "Found an object with $ref, but also with properties. Cannot handle that case."
        );
        Self::from_reference(args.object_reference.as_ref().unwrap())
    }

    fn from_string(args: &StringOptions) -> Result<Self> {
        assert!(args.object_reference.is_some());
        Self::from_reference(args.object_reference.as_ref().unwrap())
    }

    fn from_reference(r: &str) -> Result<Self> {
        let parts: Vec<_> = r.trim_start_matches('/').split("#/").collect();
        if parts.len() != 2 {
            bail!("Unexpected type reference: {}", r);
        }
        let module_name = parts[0].to_string();
        let module_path = module_name.split('/').map(|s| s.to_string()).collect();
        let type_name = parts[1].to_string();
        Ok(Self {
            module_path,
            type_name,
        })
    }

    pub fn module_name(&self) -> String {
        format!("crate::generated::types::{}", self.module_path.join("::"),)
    }

    pub fn absolute_type_path(&self) -> String {
        format!("{}::{}", self.module_name(), self.type_name)
    }
}

impl std::fmt::Debug for TypeRef {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "TypeRef /{}#/{}",
            self.module_path.join("/"),
            self.type_name
        )
    }
}

fn as_typename(arg: &TypeBase, type_refs: &mut BTreeSet<TypeRef>) -> Result<String> {
    use TypeBase::*;
    use TypeEnum::*;
    Ok(match arg {
        Single(Null) => "()".to_string(),
        Single(Boolean(_)) => "bool".to_string(),
        Single(String(args)) => {
            if args.object_reference.is_none() {
                "String".to_string()
            } else {
                let t = TypeRef::from_string(args)?;
                let name = t.absolute_type_path();
                type_refs.insert(t);
                name
            }
        }
        Single(Number(_)) => "f64".to_string(),
        Single(Integer(_)) => "i64".to_string(),
        Single(Object(args)) => {
            if args.object_reference.is_none() {
                "__serde_json::Value".to_string()
            } else {
                let t = TypeRef::from_object(args)?;
                let name = t.absolute_type_path();
                type_refs.insert(t);
                name
            }
        }
        Single(Array(args)) => match args.items {
            None => "Vec<__serde_json::Value>".to_string(),
            Some(ref v) => {
                let item_type = as_typename(&v.arg, type_refs)?;
                format!("Vec<{item_type}>")
            }
        },
        Multiple(_) => "__serde_json::Value".to_string(),
    })
}

#[derive(Debug, Clone, Serialize)]
struct DataTypeContext {
    name: String,
    extra_serde_annotations: Vec<String>,
}

#[derive(Debug, Clone, Serialize)]
struct ArgumentContext {
    name: String,
    description: Option<String>,
    data_type: DataTypeContext,
}

impl ArgumentContext {
    pub fn from_schema(
        name: String,
        var: &Type,
        type_refs: &mut BTreeSet<TypeRef>,
    ) -> Result<Self> {
        Ok(ArgumentContext {
            name,
            description: var.description.clone(),
            data_type: DataTypeContext {
                name: as_typename(&var.arg, type_refs)?,
                extra_serde_annotations: Vec::new(),
            },
        })
    }
}

#[derive(Debug, Clone, Serialize)]
struct CommandContext {
    name: String,
    description: String,
    result: Option<ArgumentContext>,
    arguments: Vec<ArgumentContext>,
}

impl CommandContext {
    pub fn from_schema(
        name: String,
        cmd: &crate::schema::interface::Command,
        type_refs: &mut BTreeSet<TypeRef>,
    ) -> Result<Self> {
        let mut arguments = Vec::new();
        for (name, arg) in &cmd.arguments {
            arguments.push(ArgumentContext::from_schema(name.clone(), arg, type_refs)?);
        }
        Ok(CommandContext {
            name,
            description: cmd.description.clone(),
            result: match &cmd.result {
                None => None,
                Some(arg) => Some(ArgumentContext::from_schema(
                    "return_value".to_string(),
                    arg,
                    type_refs,
                )?),
            },
            arguments,
        })
    }
}

/// The error group maps to one error yaml file.
#[derive(Debug, Clone, Serialize)]
struct ErrorGroupContext {
    /// The name is basically the yaml file in which the errors are defined.
    name: String,

    /// The list of errors
    error_list: schema::error::ErrorList,
}

mod impl_error {
    #[derive(Hash, Eq, PartialEq)]
    pub struct ErrorPath<'a> {
        /// The prefix where the error files are.
        pub prefix: &'a str,

        /// The error file itself.
        pub file: &'a str,
    }

    pub struct ErrorDefinition<'a> {
        /// The path of the error.
        pub path: ErrorPath<'a>,

        /// The type which is optional. If the type is not defined we accept
        /// all errors in the path.
        pub error_type: Option<&'a str>,
    }

    impl<'a> ErrorDefinition<'a> {
        /// Try to construct an error definition from the string.
        pub fn try_new(value: &'a str) -> anyhow::Result<Self> {
            let mut splits = value.split("#/");
            let path = splits.next().ok_or(anyhow::anyhow!("No path defined"))?;

            // Split the path and remove the empty parts.
            // (The first element might be empty if we have a leading `/`).
            let paths = path
                .split("/")
                .filter(|path| !path.is_empty())
                .collect::<Vec<_>>();

            anyhow::ensure!(paths.len() == 2, "Expecting exactly two paths");
            anyhow::ensure!(
                paths.iter().all(|path| !path.is_empty()),
                "Empty paths not allowed"
            );

            let path = ErrorPath {
                prefix: paths[0],
                file: paths[1],
            };

            let error_type = splits.next();
            if let Some(inner) = error_type {
                anyhow::ensure!(!inner.is_empty(), "Type must not be empty");
            }
            Ok(Self { path, error_type })
        }
    }
}

impl ErrorGroupContext {
    /// Generates the [ErrorGroupContext] from the `error_reference`.
    ///
    /// The error_reference can have two forms:
    /// - /errors/example
    /// - /errors/example#/ExampleErrorA
    ///
    /// The first type is straight forward. For the second type however, we want
    /// to group them by their file name.
    fn from_yaml(yaml_repo: &mut YamlRepo, errors: &[ErrorReference]) -> Vec<Self> {
        // The errors may be defined multiple times. If we find a definition
        // which would use all, we use all. Otherwise we use the specific
        // defintions.
        enum ErrorOption {
            /// Use all errors in a file.
            All,

            /// Use only specific errors in a file.
            Some(HashSet<String>),
        }

        // Find all the error options defined.
        let mut error_definitions = HashMap::new();
        for error_ref in errors {
            let new_error = impl_error::ErrorDefinition::try_new(&error_ref.reference)
                .expect("Failed to parse {error_ref}");

            let mut error_definition = error_definitions
                .entry(new_error.path)
                .or_insert(ErrorOption::Some(HashSet::new()));
            // We don't "downgrade" `All` to `Some`.
            if let ErrorOption::Some(options) = &mut error_definition {
                if let Some(new_option) = new_error.error_type {
                    options.insert(new_option.to_string());
                } else {
                    *error_definition = ErrorOption::All;
                }
            }
        }

        let mut output = Vec::new();
        // Load the error yaml form the disk.
        for (error_path, error_option) in error_definitions {
            let error_list = yaml_repo
                .get_errors(error_path.prefix, error_path.file)
                .unwrap();

            let mut error_group_context = ErrorGroupContext {
                name: error_path.file.to_string(),
                error_list: error_list.clone(),
            };

            // Remove unused options.
            if let ErrorOption::Some(options) = error_option {
                error_group_context
                    .error_list
                    .errors
                    .retain(|e| options.contains(&e.name));
            }

            // The yaml file might have no errors defined at all. This would
            // still comply with the EVerest schema but the user can't do
            // anything with it.
            if !error_group_context.error_list.errors.is_empty() {
                output.push(error_group_context);
            }
        }

        output
    }
}

#[derive(Debug, Clone, Serialize)]
struct InterfaceContext {
    name: String,
    description: String,
    cmds: Vec<CommandContext>,
    vars: Vec<ArgumentContext>,
    /// The errors of an interface.
    errors: Vec<ErrorGroupContext>,
}

impl InterfaceContext {
    pub fn from_yaml(
        yaml_repo: &mut YamlRepo,
        name: &str,
        type_refs: &mut BTreeSet<TypeRef>,
    ) -> Result<Self> {
        let interface_yaml = yaml_repo.get_interface(name)?;
        let mut vars = Vec::new();
        for (name, var) in &interface_yaml.vars {
            vars.push(ArgumentContext::from_schema(name.clone(), var, type_refs)?);
        }
        let mut cmds = Vec::new();
        for (name, cmd) in &interface_yaml.cmds {
            cmds.push(CommandContext::from_schema(name.clone(), cmd, type_refs)?);
        }

        // We can only borrow the yaml_repo once. It's actually not necessary so
        // we should refactor this.
        let description = interface_yaml.description.clone();
        let errors = interface_yaml.errors.clone();
        let errors = ErrorGroupContext::from_yaml(yaml_repo, &errors);

        Ok(InterfaceContext {
            name: name.to_string(),
            description,
            vars,
            cmds,
            errors,
        })
    }
}

#[derive(Debug, Clone, Serialize, Default)]
struct TypeModuleContext {
    children: BTreeMap<String, TypeModuleContext>,
    objects: Vec<ObjectTypeContext>,
    enums: Vec<EnumTypeContext>,
}

#[derive(Debug, Clone, Serialize)]
struct ObjectTypeContext {
    name: String,
    properties: Vec<ArgumentContext>,
}

#[derive(Debug, Clone, Serialize)]
struct EnumTypeContext {
    name: String,
    items: Vec<String>,
}

#[derive(Debug, Clone)]
enum TypeContext {
    Object(ObjectTypeContext),
    Enum(EnumTypeContext),
}

fn type_context_from_ref(
    r: &TypeRef,
    yaml_repo: &mut YamlRepo,
    type_refs: &mut BTreeSet<TypeRef>,
) -> Result<TypeContext> {
    use TypeBase::*;
    use TypeEnum::*;

    let module_path = r.module_path.join("/");
    let data_types_yaml = yaml_repo.get_data_types(&module_path)?;

    let type_descr = data_types_yaml
        .types
        .get_mut(&r.type_name)
        .ok_or_else(|| anyhow!("Unable to find data type {:?}. Is it defined?", r))?;

    let mut new_types: BTreeMap<std::string::String, Type> = BTreeMap::new();

    let res = match &mut type_descr.arg {
        Single(Object(args)) => {
            let mut properties = Vec::new();
            for (name, var) in &mut args.properties {
                let mut extra_serde_annotations = Vec::new();
                let data_type = {
                    // This is some "trick" - if we have enums which are defined
                    // inplace, we create a new entry.
                    if let Single(String(enum_args)) = &mut var.arg {
                        match &enum_args.enum_items {
                            Some(items) => {
                                let new_type = Type {
                                    description: Some("An inlined type".to_string()),
                                    arg: Single(String(StringOptions {
                                        pattern: None,
                                        format: None,
                                        max_length: None,
                                        min_length: None,
                                        enum_items: Some(items.clone()),
                                        default: None,
                                        object_reference: None,
                                    })),

                                    qos: None,
                                };
                                let new_name = format!(
                                    "{}AutoGen{}",
                                    r.type_name.to_case(Case::Pascal),
                                    name.to_case(Case::Pascal)
                                );
                                enum_args.object_reference =
                                    Some(format!("/{}#/{}", module_path, new_name));
                                new_types.insert(new_name, new_type);
                            }
                            _ => {}
                        }
                    }
                    let d = as_typename(&var.arg, type_refs)?;
                    if !args.required.contains(name) {
                        extra_serde_annotations
                            .push("skip_serializing_if = \"Option::is_none\"".to_string());
                        format!("Option<{}>", d)
                    } else {
                        d
                    }
                };
                properties.push(ArgumentContext {
                    name: name.clone(),
                    description: var.description.clone(),
                    data_type: DataTypeContext {
                        name: data_type,
                        extra_serde_annotations,
                    },
                });
            }
            Ok(TypeContext::Object(ObjectTypeContext {
                name: r.type_name.clone(),
                properties,
            }))
        }
        Single(String(args)) => {
            assert!(
                args.enum_items.is_some(),
                "Expected a named string type to be an enum, but {} was not.",
                r.type_name
            );

            Ok(TypeContext::Enum(EnumTypeContext {
                name: r.type_name.clone(),
                items: args.enum_items.clone().unwrap(),
            }))
        }
        other => unreachable!("Does not support $ref for {other:?}"),
    };

    data_types_yaml.types.extend(new_types);
    return res;
}

#[derive(Debug, Clone, Serialize)]
struct SlotContext {
    implementation_id: String,
    interface: String,
    min_connections: i64,
    max_connections: i64,
}

#[derive(Debug, Clone, Serialize)]
struct ConfigContext {
    name: String,
    config: Vec<ArgumentContext>,
}

#[derive(Debug, Clone, Serialize)]
struct RenderContext {
    /// The interfaces the user will need to fill in.
    provided_interfaces: Vec<InterfaceContext>,
    /// The interfaces we are requiring.
    required_interfaces: Vec<InterfaceContext>,
    /// All errors involved - those we can raise and those we can receive.
    involved_errors: HashMap<String, Vec<ErrorGroupContext>>,
    provides: Vec<SlotContext>,
    requires: Vec<SlotContext>,
    requires_with_generics: bool,
    types: TypeModuleContext,
    module_config: Vec<ArgumentContext>,
    provided_config: Vec<ConfigContext>,
}

fn title_case(arg: String) -> String {
    arg.to_case(Case::Pascal)
}

fn snake_case(arg: String) -> String {
    arg.to_case(Case::Snake)
}

/// Like `snake_case`, but can deal with reserved names (and will then use raw identifiers).
fn identifier_case(arg: String) -> String {
    let arg = snake_case(arg);
    if is_reserved_keyword(&arg) {
        format!("r#{arg}")
    } else {
        arg
    }
}

/// Converts the config data read from yaml and generates the context for Jinja.
///
/// The config data contains the config name (key) and the config data (value).
/// We use the value to derive the type and the (optional) description.
fn emit_config(config: BTreeMap<String, ConfigEntry>) -> Vec<ArgumentContext> {
    config
        .into_iter()
        .map(|(k, v)| match v.value {
            ConfigEnum::Boolean(_) => ArgumentContext {
                name: k,
                description: v.description,
                data_type: DataTypeContext {
                    name: "bool".to_string(),
                    extra_serde_annotations: Vec::new(),
                },
            },
            ConfigEnum::Integer(_) => ArgumentContext {
                name: k,
                description: v.description,
                data_type: DataTypeContext {
                    name: "i64".to_string(),
                    extra_serde_annotations: Vec::new(),
                },
            },
            ConfigEnum::Number(_) => ArgumentContext {
                name: k,
                description: v.description,
                data_type: DataTypeContext {
                    name: "f64".to_string(),
                    extra_serde_annotations: Vec::new(),
                },
            },
            ConfigEnum::String(_) => ArgumentContext {
                name: k,
                description: v.description,
                data_type: DataTypeContext {
                    name: "String".to_string(),
                    extra_serde_annotations: Vec::new(),
                },
            },
        })
        .collect::<Vec<_>>()
}

pub fn emit(manifest_path: PathBuf, everest_core: Vec<PathBuf>) -> Result<String> {
    let mut yaml_repo = YamlRepo::new(everest_core);
    let blob = fs::read_to_string(&manifest_path).context("While reading manifest file")?;
    let manifest: Manifest = serde_yaml::from_str(&blob).context("While parsing manifest")?;

    let mut env = Environment::new();
    env.set_undefined_behavior(UndefinedBehavior::Strict);
    env.add_filter("title", title_case);
    env.add_filter("snake", snake_case);
    env.add_filter("identifier", identifier_case);
    env.add_template("client", CLIENT_JINJA)?;
    env.add_template("config", CONFIG_JINJA)?;
    env.add_template("errors", ERRORS_JINJA)?;
    env.add_template("module", MODULE_JINJA)?;
    env.add_template("service", SERVICE_JINJA)?;
    env.add_template("types", TYPES_JINJA)?;

    let provided_config = manifest
        .provides
        .iter()
        .filter(|(_, data)| !data.config.is_empty())
        .map(|(name, data)| ConfigContext {
            name: name.clone(),
            config: emit_config(data.config.clone()),
        })
        .collect::<Vec<_>>();

    let mut type_refs = BTreeSet::new();
    let mut provided_interfaces = HashMap::with_capacity(manifest.provides.len());
    let mut provides = Vec::with_capacity(manifest.provides.len());
    for (implementation_id, imp) in manifest.provides {
        if !provided_interfaces.contains_key(&imp.interface) {
            let interface_context =
                InterfaceContext::from_yaml(&mut yaml_repo, &imp.interface, &mut type_refs)?;
            provided_interfaces.insert(imp.interface.clone(), interface_context);
        }
        provides.push(SlotContext {
            implementation_id,
            interface: imp.interface.clone(),
            min_connections: 1,
            max_connections: 1,
        })
    }

    let mut required_interfaces = HashMap::with_capacity(manifest.requires.len());
    let mut requires = Vec::with_capacity(manifest.requires.len());
    for (implementation_id, imp) in manifest.requires {
        if !required_interfaces.contains_key(&imp.interface) {
            let interface_context =
                InterfaceContext::from_yaml(&mut yaml_repo, &imp.interface, &mut type_refs)?;
            required_interfaces.insert(imp.interface.clone(), interface_context);
        }

        requires.push(SlotContext {
            implementation_id,
            interface: imp.interface.clone(),
            min_connections: imp.min_connections.unwrap_or(1),
            max_connections: imp.max_connections.unwrap_or(1),
        })
    }

    let mut type_module_root = TypeModuleContext::default();

    let mut done: BTreeSet<TypeRef> = BTreeSet::new();
    while done.len() != type_refs.len() {
        let mut new = BTreeSet::new();
        for t in &type_refs {
            if done.contains(t) {
                continue;
            }
            let mut module = &mut type_module_root;
            for p in &t.module_path {
                module = module.children.entry(p.clone()).or_default();
            }
            match type_context_from_ref(t, &mut yaml_repo, &mut new)? {
                TypeContext::Object(item) => module.objects.push(item),
                TypeContext::Enum(item) => module.enums.push(item),
            }
            done.insert(t.clone());
        }
        type_refs.extend(new.into_iter());
    }

    let module_config = emit_config(manifest.config);
    let requires_with_generics = requires
        .iter()
        .any(|elem| elem.min_connections != 1 || elem.max_connections != 1);

    let involved_errors = provided_interfaces
        .iter()
        .chain(required_interfaces.iter())
        .filter(|(_key, value)| !value.errors.is_empty())
        .map(|(key, value)| (key.clone(), value.errors.clone()))
        .collect::<HashMap<_, _>>();

    let context = RenderContext {
        provided_interfaces: provided_interfaces.values().cloned().collect(),
        required_interfaces: required_interfaces.values().cloned().collect(),
        involved_errors,
        provides,
        requires,
        requires_with_generics,
        types: type_module_root,
        module_config,
        provided_config,
    };
    let tmpl = env.get_template("module").unwrap();
    Ok(tmpl.render(context).unwrap())
}

#[cfg(test)]
mod tests {
    #[test]
    fn test_split_paths_invalid() {
        use super::impl_error::*;
        let invalid_input = [
            "/foo/bar/baz", // too many
            "/foo",         // too few,
            "/foo/",        // no type
            "//foo",        // no path,
            "",             // just empty
        ];

        for input in invalid_input {
            assert!(ErrorDefinition::try_new(input).is_err());
        }
    }

    #[test]
    fn test_split_paths() {
        use super::impl_error::*;
        let res = ErrorDefinition::try_new("/foo/bar#/baz").unwrap();
        assert_eq!(res.path.prefix, "foo");
        assert_eq!(res.path.file, "bar");
        assert!(matches!(res.error_type, Some("baz")));

        let res = ErrorDefinition::try_new("/foo/bar").unwrap();
        assert_eq!(res.path.prefix, "foo");
        assert_eq!(res.path.file, "bar");
        assert!(res.error_type.is_none());

        let res = ErrorDefinition::try_new("foo/bar#/baz").unwrap();
        assert_eq!(res.path.prefix, "foo");
        assert_eq!(res.path.file, "bar");
        assert!(matches!(res.error_type, Some("baz")));

        let res = ErrorDefinition::try_new("foo/bar").unwrap();
        assert_eq!(res.path.prefix, "foo");
        assert_eq!(res.path.file, "bar");
        assert!(res.error_type.is_none());
    }
}
