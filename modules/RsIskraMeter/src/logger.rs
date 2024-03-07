//! Logger with journald compliant output.
use env_logger::{Builder, Env};
use everestrs::serde as __serde;
use std::io::Write;

#[derive(__serde::Serialize)]
#[serde(crate = "__serde", rename_all = "UPPERCASE")]
struct Entry<'a> {
    code_file: &'a str,
    code_line: u32,
    message: &'a str,
}

/// Initializes the logging.
///
/// # Arguments
/// * `env_var` - The environment variable defining the minimal logging
/// severity. Must be one of `trace`, `debug`, `info`, `warn` or `error`. If
/// the environment variable is not set, the default severity will be `info`.
pub fn init_logger(env_var: &str) {
    let env = Env::default().filter_or(env_var, "info");

    Builder::from_env(env)
        .format(|buf, record| {
            let message = &record.args().to_string();

            let entry = Entry {
                code_file: record.file().unwrap_or("unknown"),
                code_line: record.line().unwrap_or(0),
                message,
            };

            writeln!(
                buf,
                "<{}>{}",
                match record.level() {
                    log::Level::Error => 3,
                    log::Level::Warn => 4,
                    log::Level::Info => 6,
                    log::Level::Debug => 7,
                    log::Level::Trace => 7,
                },
                ::everestrs::serde_json::to_string(&entry)
                    .unwrap_or("Failed to format the log message".to_string()),
            )
        })
        .init();
}
