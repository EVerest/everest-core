use clap::Parser;

/// Validates the EVerest config.
#[derive(Parser, Debug)]
struct Args {
    /// The output file to touch. Bazel will look for this one.
    #[arg(long)]
    output: String,

    /// The input file containing the config. We will parse this file.
    #[arg(long)]
    config: String,

    /// The list of expected modules.
    #[arg(required = true)]
    modules: Vec<String>,
}

/// The relevant sub-portion of EVerest's config.
mod config {

    #[derive(Debug, serde::Deserialize)]
    pub struct Module {
        pub module: String,
    }

    #[derive(Debug, serde::Deserialize)]
    pub struct EverestConfig {
        pub active_modules: std::collections::HashMap<String, Module>,
    }
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::parse();
    let config = std::fs::read_to_string(args.config)?;
    let config: config::EverestConfig = serde_yaml::from_str(&config)?;

    let config_modules: std::collections::HashSet<_> = config
        .active_modules
        .into_values()
        .map(|m| m.module)
        .collect();

    let given_modules: std::collections::HashSet<_> = args.modules.into_iter().collect();
    assert!(
        given_modules == config_modules,
        "given_modules != config_modules.\ngiven_modules: {:?}\nconfig_modules: {:?}",
        given_modules,
        config_modules
    );

    std::fs::OpenOptions::new()
        .create(true)
        .truncate(true)
        .write(true)
        .open(args.output)?;
    Ok(())
}
