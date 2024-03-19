use anyhow::Result;
use rand::Rng;

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

#[cfg(test)]
mod tests {
    use super::*;

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
