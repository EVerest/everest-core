use std::fs::File;
use std::io::BufRead;
use std::path::{Path, PathBuf};
use std::{env, io};

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

fn find_libs_in_dir(lib_dir: &Path) -> Option<Libraries> {
    let everestrs_sys = lib_dir.join("libeverestrs_sys.a");
    let framework = lib_dir.join("libframework.so");
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
    find_libs_in_dir(&root.join("everest-core/build/dist/lib"))
}

/// Takes a path to a library like `libframework.so` and returns the name for the linker, aka
/// `framework`
fn libname_from_path(p: &Path) -> String {
    let base = p
        .file_stem()
        .and_then(|os_str| os_str.to_str())
        .expect("'p' must be valid UTF-8 and have an extension.")
        .strip_prefix("lib")
        .expect("'p' should start with `lib`");

    // foo.so.1.2.3 -> foo
    base.split('.').next().unwrap_or(base).to_string()
}

fn print_link_options(p: &Path) {
    println!(
        "cargo:rustc-link-search=native={}",
        p.parent().unwrap().to_string_lossy()
    );
    println!("cargo:rustc-link-lib={}", libname_from_path(p));
}

fn find_libs_in_everest_workspace() -> Option<Libraries> {
    let root = find_everest_workspace_root();
    let libs = find_libs_in_everest_core_build_dist(&root);
    if libs.is_some() {
        return libs;
    }
    find_libs_in_everest_framework(&root)
}

/// Registers the libraries specified in the `EVEREST_RS_LINK_DEPENDENCIES` environment variable.
/// Expected to be a path to a text file that contains one object file path per line.
fn register_everest_link_deps(link_deps_path: &str) -> io::Result<()> {
    let link_deps = File::open(link_deps_path).map_err(|e| {
        io::Error::new(
            e.kind(),
            format!("Could not open EVEREST_RS_LINK_DEPENDENCIES file '{link_deps_path}': {e}"),
        )
    })?;

    let mut found_any = false;
    for line in io::BufReader::new(link_deps).lines() {
        let line = line?;
        let path = Path::new(&line);
        if !path.is_file() {
            return Err(io::Error::new(io::ErrorKind::NotFound, format!(
                "Cannot find library path '{line}' specified in EVEREST_RS_LINK_DEPENDENCIES ({link_deps_path}).",
            )));
        }

        print_link_options(path);
        found_any = true;
    }

    if found_any {
        Ok(())
    } else {
        Err(io::Error::new(
            io::ErrorKind::NotFound,
            format!("No library paths found in EVEREST_RS_LINK_DEPENDENCIES ({link_deps_path})."),
        ))
    }
}

fn main() {
    // See https://doc.rust-lang.org/cargo/reference/features.html#build-scripts
    // for details.
    if env::var("CARGO_FEATURE_BUILD_BAZEL").is_ok() {
        println!("Skipping due to bazel");
        return;
    }

    match env::var("EVEREST_RS_LINK_DEPENDENCIES") {
        Ok(p) => {
            register_everest_link_deps(&p)
                .expect("Failed to register libraries specified in EVEREST_RS_LINK_DEPENDENCIES");
        }
        Err(_) => {
            let libs = match env::var("EVEREST_LIB_DIR") {
                Ok(p) => find_libs_in_dir(Path::new(&p)),
                Err(_) => find_libs_in_everest_workspace(),
            };

            let libs = libs
        .expect("Could not find libframework.so and libeverestrs_sys. There are a few ways to solve this:
- Set EVEREST_LIB_DIR to a path that contains them.
- Set EVEREST_RS_LINK_DEPENDENCIES to the build/everestrs-link-dependencies.txt file generated by CMake (preferable).
- Or run the build again with everestrs being inside an everest workspace.");

            print_link_options(&libs.everestrs_sys);
            print_link_options(&libs.framework);
        }
    }

    println!("cargo:rustc-link-lib=boost_log");
    println!("cargo:rustc-link-lib=boost_log_setup");
}
