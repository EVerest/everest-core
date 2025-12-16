# OCMF Signature Validation

This directory contains tools for validating OCMF (Open Charge Metering Format) signatures from the Carlo Gavazzi EM580 powermeter.

## Overview

The EM580 device signs OCMF transaction data using **ECDSA-brainpoolP384r1-SHA256**. This validation tool verifies the authenticity of OCMF data by checking the digital signature against the device's public key.

## Prerequisites

### Python Dependencies

Install the required Python library:

```bash
pip install cryptography
```

Or if using Nix:

```bash
nix-shell -p "python3.withPackages (ps: with ps; [ cryptography ])"
```

## Files

- **`validate_ocmf_signature.py`** - Main validation script
- **`test_validation.sh`** - Convenience script for quick testing
- **`README.md`** - This file

## Usage

### Method 1: Using the convenience script

Edit `test_validation.sh` to set your public key and OCMF data, then run:

```bash
./test_validation.sh
```

### Method 2: Using the validation script directly

#### Validate OCMF pipe-separated string format

The EM580 device outputs OCMF data in the format: `OCMF|<data_json>|<signature_json>`

```bash
python3 validate_ocmf_signature.py \
    --public-key "04521C09090AB6A2826A613D36483A71F789F6C0D900F9A9106415EA8BE3F6AFEB5926B39E264CB3727647DA49B153370221F18048B343AC0318203F7043F840CD8BB5C9C6734C0DB46B19711AD94A0DB8F1FA854E2D60D25B33D7DDE145F61E6C" \
    --ocmf-string 'OCMF|{"FV":"1.2",...}|{"SD":"signature_hex","SA":"ECDSA-brainpoolP384r1-SHA256"}'
```

Or read from a file:

```bash
python3 validate_ocmf_signature.py \
    --public-key "04<194_hex_chars>" \
    --ocmf-string "$(cat ocmf_data.txt)"
```

#### Validate with separate components

```bash
python3 validate_ocmf_signature.py \
    --public-key "04<194_hex_chars>" \
    --text "data-to-be-signed" \
    --signature "<signature_hex>"
```

#### Validate from file

```bash
python3 validate_ocmf_signature.py \
    --public-key "04<194_hex_chars>" \
    --file data.json \
    --signature "<signature_hex>"
```

## Public Key Format

The public key must be in **uncompressed format**:
- Starts with `0x04`
- Followed by X coordinate (48 bytes = 96 hex chars)
- Followed by Y coordinate (48 bytes = 96 hex chars)
- **Total: 97 bytes = 194 hex characters** for P384

The public key can be read from the EM580 device at Modbus register **309473** (address 2500h). For a 384-bit key, read 49 words (98 bytes), but the last byte is unused, so use only the first 97 bytes.

## Signature Format

The signature can be in two formats:
1. **DER format** (ASN.1 encoded) - most common, typically 102-110 bytes
2. **Raw format**: r || s (each 48 bytes for P384, total 96 bytes = 192 hex chars)

The script automatically detects the format.

## OCMF Data Format

The EM580 device outputs OCMF data in a pipe-separated format:

```
OCMF|<data_json>|<signature_json>
```

Where:
- `<data_json>` - JSON object containing all meter data (FV, GI, GS, RD, etc.)
- `<signature_json>` - JSON object with:
  - `SD`: The signature in hex format
  - `SA`: The signature algorithm (e.g., "ECDSA-brainpoolP384r1-SHA256")

## JSON Normalization

**Important**: OCMF requires signatures to be computed over **compact JSON** (no spaces). The validation script automatically normalizes JSON to compact format before verification.

Example:
- Original: `{"LI": 99,"LR": 0}`
- Compact: `{"LI":99,"LR":0}`

The script handles this normalization automatically.

## Example Output

```
Loading public key...
✓ Public key loaded (brainpoolP384r1)

✓ Parsed OCMF string format
  Data length: 828 characters
  Signature length: 204 hex characters

⚠ JSON normalization: Original had 828 chars, compact has 825 chars
  Using compact JSON format for signature verification (OCMF requirement)
  Original hash: acafca116bd433ed0a8ad1200de600adf977d9bdef966bdecb3ec1c3cda2fdcc
  Compact hash:  fa3020425aaf1d03f8e2bce13f76e60cb098b3bff1664d1d45503b0d9c6b351b

Verifying signature...
  Algorithm: ECDSA-brainpoolP384r1-SHA256
  Message length: 825 characters (825 bytes)
  Message hash (SHA256): fa3020425aaf1d03f8e2bce13f76e60cb098b3bff1664d1d45503b0d9c6b351b
  Message preview (first 100 chars): {"FV":"1.2","GI":"Carlo Gavazzi Controls-EM580DINAV23XS3DET","GS":"KZ1660104001D"...

✓ SIGNATURE VALID - The message is authentic!
```

## Troubleshooting

### Signature verification fails

If signature verification fails, check:

1. **Public key**: Ensure it matches the device's current public key (read from register 309473)
2. **Signature**: Ensure it's from the same transaction as the data
3. **Data format**: The script automatically normalizes JSON, but verify the data hasn't been modified
4. **Key/Signature pair**: The public key and signature must be from the same device and transaction

### Common errors

- **"Expected 97 bytes for uncompressed P384 public key"**: The public key format is incorrect. Ensure it's 194 hex characters (97 bytes) starting with `04`.
- **"Invalid hex string"**: Check that the public key and signature contain only valid hexadecimal characters (0-9, A-F).
- **"Signature format not recognized"**: The signature should be either DER format (starts with 0x30) or raw format (96 bytes).

## Technical Details

### Algorithm
- **Curve**: brainpoolP384r1 (Brainpool P-384)
- **Hash**: SHA-256
- **Signature**: ECDSA

### Data-to-be-signed
The device signs the **compact JSON representation** of the OCMF data (the `<data_json>` part, without the "OCMF|" prefix or signature JSON).

### Byte Order
- Public key: Uncompressed format (0x04 || X || Y), big-endian
- Signature: DER format (ASN.1) or raw (r || s), big-endian

## References

- [OCMF Specification](https://github.com/SAFE-eV/OCMF-Open-Charge-Metering-Format)
- EM580 Modbus Communication Protocol document (Table 4.19, 4.21)
