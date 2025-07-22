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

/// The Iskra firmware has a bug where we must remove leading zeros from the r
/// and s segments if the first non-zero byte is smaller than 0x80.
///
/// If we remove any zeros we have to correct the lengths of the entire signature
/// and the affected r/s components.
///
/// The signature looks like
/// 0x30, 0x44, 0x02, 0x20 ... 0x02, 0x20, ...
/// |     |     |     |        |     | length of the r/s component
/// |     |     |     |        | start of the r/s component
/// |     |     |     | length of the r/s component.
/// |     |     | start of r/s component.
/// |     | signature length
/// | start
pub fn correct_signature(input: Vec<u16>) -> Vec<u16> {
    // Check if we have the first two bytes.
    if input.len() < 2 {
        log::warn!("Signature too short, aborting");
        return input;
    }

    // Check the integrity of the signature
    if input.len() != input[1] as usize + 2 {
        log::warn!(
            "Signature length mismatch: Expected {} received {}, aborting",
            input.len(),
            input[1] as usize + 2
        );
        return input;
    }

    let mut output = Vec::new();
    output.reserve(input.len());
    output.extend_from_slice(&input[0..2]);

    let mut current_pos = 2;

    // Process sub-components
    while current_pos < input.len() {
        if input[current_pos] != 2 {
            log::warn!(
                "Sub-component starts with {} instead of 2, aborting",
                input[current_pos]
            );
            return input;
        }
        output.push(2);
        current_pos += 1;

        if current_pos >= input.len() {
            log::warn!("No length element, aborting");
            return input;
        }

        // Get the length of the current sub-component
        let sub_component_length = input[current_pos] as usize;
        current_pos += 1;

        // Check if we have enough u16 values for the sub-component
        if current_pos + sub_component_length > input.len() {
            log::warn!("Sub-component length exceeds available u16 values, aborting");
            return input;
        }

        // Extract the sub-component
        let mut sub_component = &input[current_pos..current_pos + sub_component_length];

        // Process sub-component. Find the first non-zero value.
        let non_zero_idx = sub_component
            .iter()
            .position(|&value| value != 0)
            .unwrap_or(sub_component.len() - 1);
        let non_zero_value = sub_component[non_zero_idx];
        // If there is a non-zero value, check if it is less than 0x80.
        if non_zero_value < 0x80 {
            log::info!("Removed {} leading zeros from sub-component", non_zero_idx);
            sub_component = &sub_component[non_zero_idx..];
        }

        output.push(sub_component.len() as u16);
        output.extend_from_slice(&sub_component);

        current_pos += sub_component_length;
    }

    // Update the signature length (2nd u16) if any zeros were removed
    output[1] = (output.len() - 2) as u16;
    output
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

    #[test]
    fn test__correct_signature() {
        fn hex_string_to_vec(hex: &str) -> Vec<u16> {
            hex.as_bytes()
                .chunks(2)
                .map(|chunk| {
                    let hex_str = std::str::from_utf8(chunk).unwrap();
                    u16::from_str_radix(hex_str, 16).unwrap()
                })
                .collect()
        }
        // Test case with specific hex strings
        for (input_hex, expected_hex) in [
            // In the first segment 1 byte with zero.
            ("30440220005d226c5f20ebebfd9f0d1da2aa696dd4f77f3708eecde72b4a346ae03b8ce402206a9b5ad6da8a3234e5df67fbddd2dd98b454c74625f9340a2e82fc81c4b41d49", 
            "3043021f5d226c5f20ebebfd9f0d1da2aa696dd4f77f3708eecde72b4a346ae03b8ce402206a9b5ad6da8a3234e5df67fbddd2dd98b454c74625f9340a2e82fc81c4b41d49"), 
            // In the first segment 2 bytes with zero.
            ("304402200000226c5f20ebebfd9f0d1da2aa696dd4f77f3708eecde72b4a346ae03b8ce402206a9b5ad6da8a3234e5df67fbddd2dd98b454c74625f9340a2e82fc81c4b41d49", 
            "3042021e226c5f20ebebfd9f0d1da2aa696dd4f77f3708eecde72b4a346ae03b8ce402206a9b5ad6da8a3234e5df67fbddd2dd98b454c74625f9340a2e82fc81c4b41d49"), 
            // In the first segment 1 bytes with zero but second element is valid
            ("304402200080226c5f20ebebfd9f0d1da2aa696dd4f77f3708eecde72b4a346ae03b8ce402206a9b5ad6da8a3234e5df67fbddd2dd98b454c74625f9340a2e82fc81c4b41d49",
            "304402200080226c5f20ebebfd9f0d1da2aa696dd4f77f3708eecde72b4a346ae03b8ce402206a9b5ad6da8a3234e5df67fbddd2dd98b454c74625f9340a2e82fc81c4b41d49"),
            // In the second segment 3 bytes with zero.
            ("304402200080226c5f20ebebfd9f0d1da2aa696dd4f77f3708eecde72b4a346ae03b8ce4022000000016da8a3234e5df67fbddd2dd98b454c74625f9340a2e82fc81c4b41d49",
            "304102200080226c5f20ebebfd9f0d1da2aa696dd4f77f3708eecde72b4a346ae03b8ce4021d16da8a3234e5df67fbddd2dd98b454c74625f9340a2e82fc81c4b41d49"),
            // In the first segment all zeros
            ("30440220000000000000000000000000000000000000000000000000000000000000000002206a9b5ad6da8a3234e5df67fbddd2dd98b454c74625f9340a2e82fc81c4b41d49", 
             "302502010002206a9b5ad6da8a3234e5df67fbddd2dd98b454c74625f9340a2e82fc81c4b41d49"),
            // In both are zeros.
            ("30440220005d226c5f20ebebfd9f0d1da2aa696dd4f77f3708eecde72b4a346ae03b8ce40220007b5ad6da8a3234e5df67fbddd2dd98b454c74625f9340a2e82fc81c4b41d49", 
            "3042021f5d226c5f20ebebfd9f0d1da2aa696dd4f77f3708eecde72b4a346ae03b8ce4021f7b5ad6da8a3234e5df67fbddd2dd98b454c74625f9340a2e82fc81c4b41d49"),
            // No second sub-component
            ("30220220005d226c5f20ebebfd9f0d1da2aa696dd4f77f3708eecde72b4a346ae03b8ce4", 
            "3021021f5d226c5f20ebebfd9f0d1da2aa696dd4f77f3708eecde72b4a346ae03b8ce4"),            
            ] {
            // Convert hex strings to Vec<u16>
            let input = hex_string_to_vec(input_hex);
            let expected = hex_string_to_vec(expected_hex);

            assert_eq!(correct_signature(input), expected);
        }

        // Test cases with invalid input
        for input in [
            "304402",     // Not the right length overall. 44 is Wrong.
            "30020300",   // Sub-component not started by 2. 03 is Wrong.
            "030102",     // No length element.
            "0303020201", // Length too short in the sub-component. 02 is Wrong.
            "01",         // Too short.
            "",           // Also too short.
        ] {
            let expected = hex_string_to_vec(input);
            assert_eq!(correct_signature(expected.clone()), expected);
        }
    }
}
