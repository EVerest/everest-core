use proc_macro::TokenStream;
use quote::quote;
use std::path::PathBuf;
use syn::{parse_macro_input, spanned::Spanned, ItemFn, ItemMod, Type};

/// Attribute macro that wraps an EVerest module's `main` function to control
/// module lifecycle. The user function receives a borrowed `&Module`, ensuring
/// it cannot be dropped prematurely. After the user function returns, the
/// `Module` is dropped deterministically before process exit.
///
/// # Example
///
/// ```ignore
/// #[everestrs::main]
/// fn main(module: &Module) {
///     let class = Arc::new(MyModule {});
///     let _publishers = module.start(class.clone(), class.clone());
///     loop { std::thread::sleep(std::time::Duration::from_secs(1)); }
/// }
/// ```
#[proc_macro_attribute]
pub fn main(attr: TokenStream, item: TokenStream) -> TokenStream {
    if !attr.is_empty() {
        let attr2: proc_macro2::TokenStream = attr.into();
        return syn::Error::new_spanned(attr2, "#[everestrs::main] takes no arguments")
            .to_compile_error()
            .into();
    }

    let input = parse_macro_input!(item as ItemFn);

    if let Err(e) = main_validate(&input) {
        return e.to_compile_error().into();
    }

    let sig = &input.sig;
    let param = &sig.inputs[0];
    let body = &input.block;
    let ret = &sig.output;

    // Extract the parameter name and inner type from `name: &Type`.
    let (param_name, inner_ty) = match param {
        syn::FnArg::Typed(pat_type) => {
            let name = &pat_type.pat;
            match pat_type.ty.as_ref() {
                Type::Reference(type_ref) => (name, &type_ref.elem),
                _ => unreachable!("validated above"),
            }
        }
        _ => unreachable!("validated above"),
    };

    let expanded = quote! {
        fn main() #ret {
            let #param_name = #inner_ty::new();
            let __everest_result = {
                fn __everest_main(#param_name: &#inner_ty) #ret
                    #body
                __everest_main(&#param_name)
            };
            drop(#param_name);
            __everest_result
        }
    };

    expanded.into()
}

fn main_validate(input: &ItemFn) -> Result<(), syn::Error> {
    if input.sig.ident != "main" {
        return Err(syn::Error::new(
            input.sig.ident.span(),
            "#[everestrs::main] can only be applied to `fn main`",
        ));
    }

    if input.sig.asyncness.is_some() {
        return Err(syn::Error::new(
            input.sig.asyncness.span(),
            "#[everestrs::main] does not support async functions",
        ));
    }

    if !input.sig.generics.params.is_empty() {
        return Err(syn::Error::new(
            input.sig.generics.span(),
            "#[everestrs::main] does not support generic functions",
        ));
    }

    if input.sig.inputs.len() != 1 {
        return Err(syn::Error::new(
            input.sig.inputs.span(),
            "#[everestrs::main] function must have exactly one parameter: `module: &Module`",
        ));
    }

    let param = &input.sig.inputs[0];
    match param {
        syn::FnArg::Receiver(_) => {
            return Err(syn::Error::new(
                param.span(),
                "#[everestrs::main] function must not take `self`",
            ));
        }
        syn::FnArg::Typed(pat_type) => match pat_type.ty.as_ref() {
            Type::Reference(type_ref) => {
                if type_ref.mutability.is_some() {
                    return Err(syn::Error::new(
                        type_ref.mutability.span(),
                        "#[everestrs::main] parameter must be a shared reference (`&Module`), not `&mut`",
                    ));
                }
            }
            _ => {
                return Err(syn::Error::new(
                    pat_type.ty.span(),
                    "#[everestrs::main] parameter must be a reference (e.g. `module: &Module`)",
                ));
            }
        },
    }

    Ok(())
}

struct TestAttr {
    config_path: Option<syn::LitStr>,
    module_name: syn::LitStr,
    harness: bool,
}

impl syn::parse::Parse for TestAttr {
    fn parse(input: syn::parse::ParseStream) -> syn::Result<Self> {
        let mut config_path = None;
        let mut module_name = None;
        let mut harness = false;

        while !input.is_empty() {
            let ident: syn::Ident = input.parse()?;
            let _: syn::Token![=] = input.parse()?;
            match ident.to_string().as_str() {
                "config" => {
                    config_path = Some(input.parse::<syn::LitStr>()?);
                }
                "module" => {
                    module_name = Some(input.parse::<syn::LitStr>()?);
                }
                "harness" => {
                    harness = input.parse::<syn::LitBool>()?.value;
                }
                other => {
                    return Err(syn::Error::new(
                        ident.span(),
                        format!(
                            "unknown attribute `{other}`, expected `config`, `module`, or `harness`"
                        ),
                    ));
                }
            }
            if !input.is_empty() {
                let _: syn::Token![,] = input.parse()?;
            }
        }

        let module_name = module_name
            .ok_or_else(|| syn::Error::new(input.span(), "missing `module` attribute"))?;

        Ok(Self {
            config_path,
            module_name,
            harness,
        })
    }
}

#[proc_macro_attribute]
pub fn test(attr: TokenStream, item: TokenStream) -> TokenStream {
    let test_attr = parse_macro_input!(attr as TestAttr);
    let input = parse_macro_input!(item as ItemFn);

    // If config is provided, generate harness + test in a wrapping module.
    // If not, just emit the test function (assumes harness is on an enclosing mod).
    match test_impl(&test_attr, input) {
        Ok(tokens) => tokens.into(),
        Err(e) => e.to_compile_error().into(),
    }
}

fn test_validate(input: &ItemFn) -> Result<(), syn::Error> {
    if input.sig.asyncness.is_some() {
        return Err(syn::Error::new(
            input.sig.asyncness.span(),
            "#[everestrs::test] does not support async functions",
        ));
    }

    if !input.sig.generics.params.is_empty() {
        return Err(syn::Error::new(
            input.sig.generics.span(),
            "#[everestrs::test] does not support generic functions",
        ));
    }

    if input.sig.inputs.len() != 1 {
        return Err(syn::Error::new(
            input.sig.inputs.span(),
            "#[everestrs::test] function must have exactly one parameter",
        ));
    }

    let param = &input.sig.inputs[0];
    match param {
        syn::FnArg::Receiver(_) => {
            return Err(syn::Error::new(
                param.span(),
                "#[everestrs::test] function must not take `self`",
            ));
        }
        syn::FnArg::Typed(pat_type) => match pat_type.ty.as_ref() {
            Type::Reference(type_ref) => {
                if type_ref.mutability.is_some() {
                    return Err(syn::Error::new(
                        type_ref.mutability.span(),
                        "#[everestrs::test] parameter must be a shared reference, not `&mut`",
                    ));
                }
            }
            _ => {
                return Err(syn::Error::new(
                    pat_type.ty.span(),
                    "#[everestrs::test] parameter must be a reference (e.g. `module: &Module`)",
                ));
            }
        },
    }

    Ok(())
}

fn test_impl(attr: &TestAttr, item_fn: ItemFn) -> Result<proc_macro2::TokenStream, syn::Error> {
    // Sanity checks.
    test_validate(&item_fn)?;

    // Forward all attributes from the user function to the generated #[test] fn
    // (e.g. #[should_panic], #[ignore], #[allow(...)]).
    let forwarded_attrs = &item_fn.attrs;

    let sig = &item_fn.sig;
    let ident = &sig.ident;
    let param = &sig.inputs[0];
    let ret = &sig.output;
    let body = &item_fn.block;
    let module = &attr.module_name;

    // Extract the parameter name and inner type from `name: &Type`.
    let (param_name, inner_ty) = match param {
        syn::FnArg::Typed(pat_type) => {
            let name = &pat_type.pat;
            match pat_type.ty.as_ref() {
                Type::Reference(type_ref) => (name, &type_ref.elem),
                _ => unreachable!("validated above"),
            }
        }
        _ => unreachable!("validated above"),
    };

    // Derive the config filename from the attribute. The bazel rule symlinks
    // the config file to etc/everest/<basename>.
    let config_basename = attr
        .config_path
        .as_ref()
        .map(|lit| {
            std::path::Path::new(&lit.value())
                .file_name()
                .unwrap()
                .to_string_lossy()
                .into_owned()
        })
        .unwrap_or_else(|| "config-sil.yaml".to_string());

    // The function we would like to emit
    let test_fn = quote! {
        #[test]
        #(#forwarded_attrs)*
        fn #ident() #ret {
            let prefix = std::env::current_dir().expect("Failed to get current directory");
            let config = prefix.join(format!("etc/everest/{}", #config_basename));

            // Each test gets a unique MQTT prefix so tests can run in parallel
            // without topic collisions.
            let __mqtt_prefix = format!(
                "everest_test_{:?}_{:?}/",
                std::process::id(),
                std::thread::current().id(),
            );

            // Start the manager, telling it not to spawn the module under test.
            // Blocks until the manager signals readiness via --status-fifo.
            let _manager = ::everestrs::manager::Manager::start(
                &prefix, &config, &[#module], Some(&__mqtt_prefix),
            ).expect("Failed to start manager");
            let args = everestrs::Args {
                prefix: prefix.clone(),
                module: #module.to_string(),
                log_config: prefix.join("etc/everest/default_logging.cfg"),
                mqtt_broker_socket_path: None,
                mqtt_broker_host: "localhost".to_string(),
                mqtt_broker_port: 1883,
                mqtt_everest_prefix: __mqtt_prefix,
                mqtt_external_prefix: "".to_string(),
            };

            let #param_name = #inner_ty::new_with_args(args);
            let __everest_result = {
                fn __everest_test(#param_name: &#inner_ty) #ret
                    #body
                __everest_test(&#param_name)
            };
            drop(#param_name);
            __everest_result
        }
    };

    if attr.harness {
        let generated_tokens = generate_harness_tokens(attr)?;
        let mod_ident = &item_fn.sig.ident;
        Ok(quote! {
            mod #mod_ident {
                #generated_tokens
                #[allow(unused_imports)]
                use generated::Module;
                #test_fn
            }
        })
    } else {
        Ok(test_fn)
    }
}

fn resolve_everest_core_roots() -> Vec<PathBuf> {
    if let Ok(val) = std::env::var("EVEREST_CORE_ROOT") {
        val.split(':').map(PathBuf::from).collect()
    } else {
        // Fallback: try CARGO_MANIFEST_DIR and walk up to find the repo root.
        if let Ok(manifest_dir) = std::env::var("CARGO_MANIFEST_DIR") {
            let mut dir = PathBuf::from(&manifest_dir);
            // Walk up looking for a directory that contains an "interfaces" subdirectory.
            loop {
                if dir.join("interfaces").is_dir() {
                    return vec![dir];
                }
                if !dir.pop() {
                    break;
                }
            }
        }
        Vec::new()
    }
}

/// Core logic: resolves config, builds synthetic manifest, runs codegen.
/// Returns the generated tokens.
fn generate_harness_tokens(attr: &TestAttr) -> Result<proc_macro2::TokenStream, syn::Error> {
    let config_lit = attr.config_path.as_ref().expect("`config` attribute must be set");
    let manifest_dir = std::env::var("CARGO_MANIFEST_DIR")
        .map_err(|_| syn::Error::new(config_lit.span(), "CARGO_MANIFEST_DIR not set"))?;

    let config_path = PathBuf::from(&manifest_dir).join(config_lit.value());
    let module_instance = attr.module_name.value();
    let everest_core_roots = resolve_everest_core_roots();

    if everest_core_roots.is_empty() {
        return Err(syn::Error::new(
            config_lit.span(),
            "Could not determine everest-core root. Set EVEREST_CORE_ROOT environment variable.",
        ));
    }

    let (manifest, _tracked_files) =
        everestrs_build::build_test_manifest(&config_path, &module_instance, &everest_core_roots)
            .map_err(|e| {
            syn::Error::new(
                attr.module_name.span(),
                format!("Failed to build test manifest: {e}"),
            )
        })?;

    let generated_code = everestrs_build::codegen::emit_manifest(manifest, everest_core_roots)
        .map_err(|e| {
            syn::Error::new(
                attr.module_name.span(),
                format!("Code generation failed: {e}"),
            )
        })?;

    syn::parse_str(&generated_code).map_err(|e| {
        syn::Error::new(
            attr.module_name.span(),
            format!("Failed to parse generated code: {e}"),
        )
    })
}

/// Attribute macro that auto-generates a test counterpart module by reading
/// a config.yaml and deducing interfaces from connected modules' manifests.
///
/// Applied to a `mod` to share generated types across multiple tests and
/// fixtures. For single test functions, use
/// `#[everestrs::test(config = "...", module = "...")]` instead.
///
/// # Example
///
/// ```ignore
/// #[everestrs::harness(config = "config.yaml", module = "example_1")]
/// mod my_tests {
///     #[everestrs::test(module = "example_1")]
///     fn test_a(module: &Module) { ... }
///
///     #[everestrs::test(module = "example_1")]
///     fn test_b(module: &Module) { ... }
/// }
/// ```
#[proc_macro_attribute]
pub fn harness(attr: TokenStream, item: TokenStream) -> TokenStream {
    let test_attr = parse_macro_input!(attr as TestAttr);
    let item_mod = parse_macro_input!(item as ItemMod);

    match harness_impl(&test_attr, item_mod) {
        Ok(tokens) => tokens.into(),
        Err(e) => e.to_compile_error().into(),
    }
}

/// Generates harness code and injects it into a module body.
fn harness_impl(
    attr: &TestAttr,
    item_mod: syn::ItemMod,
) -> Result<proc_macro2::TokenStream, syn::Error> {
    let generated_tokens = generate_harness_tokens(attr)?;
    if let Some((_brace, items)) = item_mod.content {
        let vis = &item_mod.vis;
        let ident = &item_mod.ident;
        let attrs = &item_mod.attrs;
        Ok(quote! {
            #(#attrs)*
            #vis mod #ident {
                #generated_tokens
                #[allow(unused_imports)]
                use generated::Module;
                #(#items)*
            }
        })
    } else {
        Err(syn::Error::new_spanned(
            &item_mod,
            "#[everestrs::harness] requires a module with a body (not just a declaration)",
        ))
    }
}
