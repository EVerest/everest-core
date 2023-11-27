use anyhow::Result;
use argh::FromArgs;
use everestrs_build::Builder;
use std::path::PathBuf;

#[derive(FromArgs)]
/// Codegen for EVerest-rs
struct Args {
    /// path to everest-core.
    #[argh(option)]
    pub everest_core: Vec<PathBuf>,

    /// manifest to generate code for
    #[argh(option)]
    pub manifest: PathBuf,

    /// output directory to put the generated code to.
    #[argh(option)]
    pub out_dir: PathBuf,
}

pub fn main() -> Result<()> {
    let args: Args = argh::from_env();

    Builder::new(args.manifest, args.everest_core)
        .out_dir(args.out_dir)
        .generate()?;

    Ok(())
}
