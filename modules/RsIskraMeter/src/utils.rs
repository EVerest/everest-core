use anyhow::Result;
use rand::Rng;
use std::sync::Mutex;

pub fn to_8_string(input: &[u16]) -> Result<String> {
    let u8_slice = input.iter().flat_map(|&x| u16::to_be_bytes(x)).collect();
    Ok(String::from_utf8(u8_slice)?.trim().to_string())
}

pub fn counter(regs: [u16; 2], exp: u16) -> f64 {
    let measurement = (regs[0] as i32) << 16 | ((regs[1] as i32) & 0xFFFF);
    measurement as f64 * 1.0e1_f64.powi(exp as i32)
}

/// Unsigned Measurement (32 bit)
/// Decade Exponent (Signed 8 bit)
/// Binary Signed value (24 bit)
pub fn from_t5_format(regs: [u16; 2]) -> f64 {
    let exp = ((regs[0] >> 8) & 0xFF) as i8;
    let value = (regs[0] & 0xFF) as u8;
    let measurement = ((value as u32) << 16) | ((regs[1] as u32) & 0xFFFF);
    measurement as f64 * 1.0e1_f64.powi(exp as i32)
}

/// Signed Measurement (32 bit)
/// Decade Exponent (Signed 8 bit)
/// Binary Signed value (24 bit)
pub fn from_t6_format(regs: [u16; 2]) -> f64 {
    let exp = ((regs[0] >> 8) & 0xFF) as i8;
    let value = (regs[0] & 0xFF) as i8;
    let measurement = ((value as i32) << 16) | ((regs[1] as i32) & 0xFFFF);
    measurement as f64 * 1.0e1_f64.powi(exp as i32)
}

pub fn string_to_vec(input: &str) -> Vec<u16> {
    input
        .as_bytes()
        .chunks(2)
        .map(|chunk| {
            let mut value = (chunk[0] as u16) << 8;
            if chunk.len() > 1 {
                value |= chunk[1] as u16 & 0xFF;
            }
            value
        })
        .collect()
}

pub fn create_random_meter_session_id() -> String {
    let start = format!("{:06X}", rand::thread_rng().gen_range(0..=0xFFFFFF));
    let hex_time = format!(
        "{:X}",
        std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_secs()
    );
    let end = format!("{:06X}", rand::thread_rng().gen_range(0..=0xFFFFFF));

    start + &hex_time + &end
}

pub fn to_hex_string(input: Vec<u16>) -> String {
    input
        .into_iter()
        .flat_map(|value| value.to_be_bytes())
        .map(|value| format!("{value:02X}"))
        .collect::<Vec<_>>()
        .concat()
}

pub fn create_ocmf(signed_meter_values: String, signature: String) -> String {
    format!("OCMF|{}|{{\"SD\":\"{}\"}}", signed_meter_values, signature)
}

pub struct ErrorReporter<F, G>
where
    F: Fn() + Send + Sync + 'static,
    G: Fn() + Send + Sync + 'static,
{
    error_count: Mutex<usize>,
    error_reported: Mutex<bool>,
    report_error: F,
    clear_error: G,
    threshold: usize,
}

impl<F, G> ErrorReporter<F, G>
where
    F: Fn() + Send + Sync + 'static,
    G: Fn() + Send + Sync + 'static,
{
    pub fn new(report_error: F, clear_error: G, threshold: usize) -> Self {
        Self {
            error_count: Mutex::new(0),
            error_reported: Mutex::new(false),
            report_error,
            clear_error,
            threshold,
        }
    }

    pub fn record_error(&self) {
        let mut reported = self.error_reported.lock().unwrap();

        if !*reported {
            let mut count = self.error_count.lock().unwrap();
            *count += 1;

            if *count >= self.threshold {
                (self.report_error)();
                *reported = true;
            }
        }
    }

    pub fn clear_errors(&self) {
        let mut reported = self.error_reported.lock().unwrap();

        if *reported {
            let mut count = self.error_count.lock().unwrap();
            (self.clear_error)();
            *count = 0;
            *reported = false;
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::atomic::{AtomicBool, Ordering};
    use std::sync::Arc;

    #[test]
    fn test_report_error_called_when_threshold_reached() {
        let report_called = Arc::new(AtomicBool::new(false));
        let clear_called = Arc::new(AtomicBool::new(false));

        let report_error = {
            let report_called = Arc::clone(&report_called);
            move || {
                report_called.store(true, Ordering::SeqCst);
            }
        };

        let clear_error = {
            let clear_called = Arc::clone(&clear_called);
            move || {
                clear_called.store(true, Ordering::SeqCst);
            }
        };

        let threshold = 3;
        let error_reporter = ErrorReporter::new(report_error, clear_error, threshold);

        // Simulate errors
        error_reporter.record_error();
        error_reporter.record_error();
        error_reporter.record_error(); // should trigger report_error

        // Verify the closure was called when threshold was reached
        assert!(
            report_called.load(Ordering::SeqCst),
            "Report error was not called!"
        );
        assert!(
            !clear_called.load(Ordering::SeqCst),
            "Clear error was incorrectly called!"
        );
    }

    #[test]
    fn test_clear_error_called_after_successful_read() {
        let report_called = Arc::new(AtomicBool::new(false));
        let clear_called = Arc::new(AtomicBool::new(false));

        let report_error = {
            let report_called = Arc::clone(&report_called);
            move || {
                report_called.store(true, Ordering::SeqCst);
            }
        };

        let clear_error = {
            let clear_called = Arc::clone(&clear_called);
            move || {
                clear_called.store(true, Ordering::SeqCst);
            }
        };

        let threshold = 3;
        let error_reporter = ErrorReporter::new(report_error, clear_error, threshold);

        // Simulate errors and then a successful read
        error_reporter.record_error();
        error_reporter.record_error();
        error_reporter.record_error(); // should trigger report_error
        error_reporter.clear_errors(); // should trigger clear_error

        // Verify the closures were called in the correct order
        assert!(
            report_called.load(Ordering::SeqCst),
            "Report error was not called!"
        );
        assert!(
            clear_called.load(Ordering::SeqCst),
            "Clear error was not called!"
        );
    }

    #[test]
    fn test_report_error_not_called_before_threshold() {
        let report_called = Arc::new(AtomicBool::new(false));
        let clear_called = Arc::new(AtomicBool::new(false));

        let report_error = {
            let report_called = Arc::clone(&report_called);
            move || {
                report_called.store(true, Ordering::SeqCst);
            }
        };

        let clear_error = {
            let clear_called = Arc::clone(&clear_called);
            move || {
                clear_called.store(true, Ordering::SeqCst);
            }
        };

        let threshold = 3;
        let error_reporter = ErrorReporter::new(report_error, clear_error, threshold);

        // Simulate errors, but not enough to reach the threshold
        error_reporter.record_error(); // Count: 1
        error_reporter.record_error(); // Count: 2

        // Verify that report_error was not called before threshold is reached
        assert!(
            !report_called.load(Ordering::SeqCst),
            "Report error was incorrectly called!"
        );
        assert!(
            !clear_called.load(Ordering::SeqCst),
            "Clear error was incorrectly called!"
        );
    }

    #[test]
    fn test_error_reported_only_once_after_reaching_threshold_multiple_times() {
        let report_call_count = Arc::new(Mutex::new(0));
        let clear_call_count = Arc::new(Mutex::new(0));

        let report_error = {
            let report_call_count = Arc::clone(&report_call_count);
            move || {
                let mut count = report_call_count.lock().unwrap();
                *count += 1; // Increment report_error call count
            }
        };

        let clear_error = {
            let clear_call_count = Arc::clone(&clear_call_count);
            move || {
                let mut count = clear_call_count.lock().unwrap();
                *count += 1; // Increment clear_error call count
            }
        };

        let threshold = 3;
        let error_reporter = ErrorReporter::new(report_error, clear_error, threshold);

        // Simulate errors and trigger the error reporting
        error_reporter.record_error();
        error_reporter.record_error();
        error_reporter.record_error(); // should trigger report_error
        error_reporter.record_error(); // should NOT trigger report_error again
        error_reporter.record_error(); // should NOT trigger report_error again
        error_reporter.record_error(); // should NOT trigger report_error again
        error_reporter.record_error(); // should NOT trigger report_error again

        // Verify the closure was called only once, even if threshold is reached multiple times
        assert!(
            *report_call_count.lock().unwrap() == 1,
            "Report error was called more than once!"
        );
        assert!(*clear_call_count.lock().unwrap() == 0, "Clear was called!");
    }

    #[test]
    /// Tests for the `to_8_string` conversion.
    fn test__to_8_string() {
        let parameter = [
            (vec![], ""),
            (
                vec![
                    u16::from_be_bytes([b' ', b'\r']),
                    u16::from_be_bytes([b'h', b'e']),
                    u16::from_be_bytes([b'l', b'l']),
                    u16::from_be_bytes([b'o', b' ']),
                ],
                "hello",
            ),
            (vec![u16::from_be_bytes([b' ', b'a']); 5], "a a a a a"),
            (vec![u16::from_be_bytes([b' ', b' ']); 5], ""),
        ];

        for (input, expected) in parameter {
            let output = to_8_string(&input).unwrap();
            assert_eq!(&output, expected);
        }
    }

    #[test]
    /// Tests the `counter` conversion.
    fn test__counter() {
        let parameters = [
            ([0, 0], 1, 0.0),
            ([0, 1234], 0, 1234.0),
            ([0, 1234], 1, 12340.0),
            ([16, 0], 0, (16 << 16) as f64),
        ];
        for (input_reg, input_exp, expected) in parameters {
            assert_eq!(counter(input_reg, input_exp), expected);
        }
    }

    #[test]
    /// Tests the `from_t5_format` and `from_t6_format` conversionss.
    fn test__from_tx_format() {
        let parameters = [
            ([0, 0], 0.0),
            ([0, 1234], 1234.0),
            ([u16::from_be_bytes([3, 2]), 0], (2 << 16) as f64 * 1000.0),
        ];

        for (input, expected) in parameters {
            assert_eq!(from_t5_format(input), expected);
            assert_eq!(from_t6_format(input), expected);
        }
    }

    #[test]
    fn test__string_to_vec() {
        let parameters = [
            ("", vec![]),
            (
                "hello",
                vec![
                    u16::from_be_bytes([b'h', b'e']),
                    u16::from_be_bytes([b'l', b'l']),
                    (b'o' as u16) << 8,
                ],
            ),
            (
                "test",
                vec![
                    u16::from_be_bytes([b't', b'e']),
                    u16::from_be_bytes([b's', b't']),
                ],
            ),
        ];

        for (input, expected) in parameters {
            assert_eq!(string_to_vec(input), expected);
        }
    }

    #[test]
    fn test__to_hex_string() {
        let parameters = [(vec![], ""), (vec![0xdead, 0xbeef], "DEADBEEF")];

        for (input, expected) in parameters {
            assert_eq!(to_hex_string(input), expected);
        }
    }
}
