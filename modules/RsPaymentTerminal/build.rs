use everestrs_build::Builder;

pub fn main() {
    Builder::new(
        "manifest.yaml",
        vec![std::env::var("EVEREST_CORE_ROOT").unwrap()],
    )
    .generate()
    .unwrap();

    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed=manifest.yaml");
}
