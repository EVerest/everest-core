use std::env;
use std::path::{Path, PathBuf};

struct Libraries {
    everestrs_sys: PathBuf,
    framework: PathBuf,
}

fn find_everest_workspace_root() -> PathBuf {
    if let Ok(everest_framework_dir) = env::var("EVEREST_RS_FRAMEWORK_SOURCE_LOCATION") {
        return PathBuf::from(everest_framework_dir);
    }

    let mut cur_dir =
        PathBuf::from(env::var("CARGO_MANIFEST_DIR").expect("always set in build.rs execution"));

    // A poor heuristic: We traverse the directories upwards until we find a directory called
    // everest-framework and hope that this is the EVerest workspace.
    while cur_dir.parent().is_some() {
        cur_dir = cur_dir.parent().unwrap().to_path_buf();
        if cur_dir.join("everest-framework").is_dir() {
            return cur_dir;
        }
    }
    panic!("everstrs is not build within an EVerest workspace.");
}

/// Returns the Libraries path if this is a standalone build of everest-framework or None if it is
/// not.
fn find_libs_in_everest_framework(root: &Path) -> Option<Libraries> {
    let (everestrs_sys, framework) =
        if let Ok(everest_framework_dir) = env::var("EVEREST_RS_FRAMEWORK_BINARY_LOCATION") {
            let everest_framework_path = PathBuf::from(everest_framework_dir);
            (
                everest_framework_path.join("everestrs/libeverestrs_sys.a"),
                everest_framework_path.join("lib/libframework.so"),
            )
        } else {
            (
                root.join("everest-framework/build/everestrs/libeverestrs_sys.a"),
                root.join("everest-framework/build/lib/libframework.so"),
            )
        };
    if everestrs_sys.exists() && framework.exists() {
        Some(Libraries {
            everestrs_sys,
            framework,
        })
    } else {
        None
    }
}

/// Returns the Libraries path if this is an EVerest workspace where make install was run in
/// everest-core/build or None if not.
fn find_libs_in_everest_core_build_dist(root: &Path) -> Option<Libraries> {
    let everestrs_sys = root.join("everest-core/build/dist/lib/libeverestrs_sys.a");
    let framework = root.join("everest-core/build/dist/lib/libframework.so");
    if everestrs_sys.exists() && framework.exists() {
        Some(Libraries {
            everestrs_sys,
            framework,
        })
    } else {
        None
    }
}

/// Takes a path to a library like `libframework.so` and returns the name for the linker, aka
/// `framework`
fn libname_from_path(p: &Path) -> String {
    p.file_stem()
        .and_then(|os_str| os_str.to_str())
        .expect("'p' must be valid UTF-8 and have a .so extension.")
        .strip_prefix("lib")
        .expect("'p' should start with `lib`")
        .to_string()
}

fn print_link_options(p: &Path) {
    println!(
        "cargo:rustc-link-search=native={}",
        p.parent().unwrap().to_string_lossy()
    );
    println!("cargo:rustc-link-lib={}", libname_from_path(p));
}

fn find_libs(root: &Path) -> Libraries {
    let libs = find_libs_in_everest_core_build_dist(&root);
    if libs.is_some() {
        return libs.unwrap();
    }
    find_libs_in_everest_framework(&root)
        .expect("everestrs is not build in a EVerest workspace that already ran cmake build")
}

fn main() {
    let root = find_everest_workspace_root();
    let libs = find_libs(&root);

    print_link_options(&libs.everestrs_sys);
    print_link_options(&libs.framework);
}
