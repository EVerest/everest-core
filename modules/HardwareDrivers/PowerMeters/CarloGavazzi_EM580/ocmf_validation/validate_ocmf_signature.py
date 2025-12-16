#!/usr/bin/env python3
"""
OCMF Signature Validation Script

Validates ECDSA signatures using brainpoolP384r1 curve and SHA256 hash algorithm.
This script can be used to verify the authenticity of OCMF (Open Charge Metering Format) data.

Usage:
    python3 validate_ocmf_signature.py --public-key <hex_key> --text <message> --signature <hex_signature>
    python3 validate_ocmf_signature.py --public-key <hex_key> --file <json_file> --signature <hex_signature>
    python3 validate_ocmf_signature.py --public-key <hex_key> --ocmf-json <ocmf_json_string>

The signature should be in DER format (hex encoded).
The public key should be in uncompressed format (hex encoded, 97 bytes = 194 hex chars for P384).
"""

import argparse
import json
import sys
from hashlib import sha256

try:
    from cryptography.hazmat.primitives import hashes, serialization
    from cryptography.hazmat.primitives.asymmetric import ec
    from cryptography.hazmat.backends import default_backend
    from cryptography.hazmat.primitives.asymmetric.utils import encode_dss_signature, decode_dss_signature
    from cryptography.hazmat.primitives.asymmetric.utils import Prehashed
except ImportError:
    print("Error: cryptography library is required. Install it with: pip install cryptography")
    sys.exit(1)


def parse_ocmf_json(ocmf_json_str):
    """
    Parse OCMF JSON string and extract the data-to-be-signed and signature.
    
    OCMF format structure:
    {
        "SD": "data-to-be-signed",
        "SA": "signature-algorithm",
        "SI": "signature"
    }
    """
    try:
        ocmf_data = json.loads(ocmf_json_str)
        if "SD" not in ocmf_data or "SI" not in ocmf_data:
            raise ValueError("OCMF JSON must contain 'SD' (data) and 'SI' (signature) fields")
        return ocmf_data["SD"], ocmf_data["SI"]
    except json.JSONDecodeError as e:
        raise ValueError(f"Invalid JSON format: {e}")


def parse_ocmf_string(ocmf_str):
    """
    Parse OCMF pipe-separated string format: OCMF|<data_json>|<signature_json>
    
    The signature_json should contain "SD" field with the signature hex.
    """
    parts = ocmf_str.split("|", 2)
    if len(parts) != 3 or parts[0] != "OCMF":
        raise ValueError("Invalid OCMF string format. Expected: OCMF|<data_json>|<signature_json>")
    
    data_json = parts[1]
    signature_json_str = parts[2]
    
    # Parse signature JSON to get SD field
    try:
        signature_json = json.loads(signature_json_str)
        signature_hex = signature_json.get("SD", "")
        if not signature_hex:
            raise ValueError("Signature JSON must contain 'SD' field")
        return data_json, signature_hex
    except json.JSONDecodeError as e:
        raise ValueError(f"Invalid signature JSON format: {e}")


def hex_to_bytes(hex_str):
    """Convert hex string to bytes, handling both with and without 0x prefix."""
    hex_str = hex_str.strip()
    if hex_str.startswith("0x") or hex_str.startswith("0X"):
        hex_str = hex_str[2:]
    # Remove any whitespace or separators
    hex_str = hex_str.replace(" ", "").replace(":", "").replace("-", "")
    try:
        return bytes.fromhex(hex_str)
    except ValueError as e:
        raise ValueError(f"Invalid hex string: {e}")


def load_public_key_from_hex(public_key_hex):
    """
    Load ECDSA public key from hex string.
    
    For brainpoolP384r1:
    - Uncompressed format: 0x04 || X || Y (97 bytes = 194 hex chars)
    - X and Y are each 48 bytes (96 hex chars)
    """
    public_key_bytes = hex_to_bytes(public_key_hex)
    
    # Accept DER-encoded SubjectPublicKeyInfo (starts with 0x30) as well.
    # This is the format typically returned by devices in a DER key block.
    if len(public_key_bytes) >= 2 and public_key_bytes[0] == 0x30:
        try:
            key = serialization.load_der_public_key(public_key_bytes, backend=default_backend())
        except Exception as e:
            raise ValueError(f"Failed to load DER public key: {e}")
        if not isinstance(key, ec.EllipticCurvePublicKey):
            raise ValueError("DER public key is not an EC public key")
        if not isinstance(key.curve, ec.BrainpoolP384R1):
            raise ValueError(f"DER public key curve mismatch: expected brainpoolP384r1, got {type(key.curve).__name__}")
        return key

    # For P384, uncompressed key should be 97 bytes (0x04 + 48 bytes X + 48 bytes Y)
    if len(public_key_bytes) != 97:
        raise ValueError(f"Expected 97 bytes for uncompressed P384 public key, got {len(public_key_bytes)} bytes")
    
    if public_key_bytes[0] != 0x04:
        raise ValueError("Uncompressed public key must start with 0x04")
    
    # Extract X and Y coordinates (each 48 bytes)
    x = public_key_bytes[1:49]
    y = public_key_bytes[49:97]
    
    # Create public key using brainpoolP384r1 curve
    public_numbers = ec.EllipticCurvePublicNumbers(
        int.from_bytes(x, byteorder='big'),
        int.from_bytes(y, byteorder='big'),
        ec.BrainpoolP384R1()
    )
    
    return public_numbers.public_key(default_backend())


def decode_signature(signature_hex):
    """
    Decode signature from hex string.
    
    The signature can be in two formats:
    1. DER encoded (ASN.1 format) - standard for ECDSA
    2. Raw format: r || s (each 48 bytes for P384)
    """
    signature_bytes = hex_to_bytes(signature_hex)
    
    # Try DER format first (most common)
    try:
        # For P384, DER signature is typically around 104-110 bytes
        # Try to decode as DER
        from cryptography.hazmat.primitives.asymmetric.utils import decode_dss_signature
        r, s = decode_dss_signature(signature_bytes)
        return r, s
    except Exception:
        # If DER fails, try raw format: r || s (each 48 bytes = 96 bytes total for P384)
        if len(signature_bytes) == 96:
            r = int.from_bytes(signature_bytes[:48], byteorder='big')
            s = int.from_bytes(signature_bytes[48:], byteorder='big')
            return r, s
        else:
            raise ValueError(f"Signature format not recognized. Expected DER or 96-byte raw format, got {len(signature_bytes)} bytes")


def verify_signature(public_key, message, signature_hex):
    """
    Verify ECDSA signature using brainpoolP384r1 and SHA256.
    
    Args:
        public_key: ECDSA public key object
        message: The message/text to verify (string or bytes)
        signature_hex: Signature in hex format (DER or raw)
    
    Returns:
        bool: True if signature is valid, False otherwise
    """
    # Convert message to bytes if it's a string
    if isinstance(message, str):
        message_bytes = message.encode('utf-8')
    else:
        message_bytes = message
    
    # Decode signature
    r, s = decode_signature(signature_hex)
    
    # Verify signature
    try:
        public_key.verify(
            encode_dss_signature(r, s),
            message_bytes,
            ec.ECDSA(hashes.SHA256())
        )
        return True
    except Exception as e:
        print(f"Signature verification failed: {e}")
        return False


def verify_signature_prehashed(public_key, message, signature_hex):
    """
    Verify signature where the device signs SHA256(message) directly (pre-hashed ECDSA).
    """
    # Convert message to bytes if it's a string
    if isinstance(message, str):
        message_bytes = message.encode('utf-8')
    else:
        message_bytes = message

    digest = sha256(message_bytes).digest()
    r, s = decode_signature(signature_hex)

    try:
        public_key.verify(
            encode_dss_signature(r, s),
            digest,
            ec.ECDSA(Prehashed(hashes.SHA256()))
        )
        return True
    except Exception as e:
        print(f"Prehashed signature verification failed: {e}")
        return False


def main():
    parser = argparse.ArgumentParser(
        description="Validate ECDSA-brainpoolP384r1-SHA256 signatures for OCMF data",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Validate with separate components
  python3 validate_ocmf_signature.py \\
      --public-key "04<194_hex_chars>" \\
      --text "data-to-be-signed" \\
      --signature "<signature_hex>"
  
  # Validate from OCMF pipe-separated string
  python3 validate_ocmf_signature.py \\
      --public-key "04<194_hex_chars>" \\
      --ocmf-string 'OCMF|{"data":"..."}|{"SD":"signature","SA":"ECDSA-brainpoolP384r1-SHA256"}'
  
  # Validate from OCMF JSON string
  python3 validate_ocmf_signature.py \\
      --public-key "04<194_hex_chars>" \\
      --ocmf-json '{"SD":"data","SA":"ECDSA-brainpoolP384r1-SHA256","SI":"signature"}'
  
  # Validate from file
  python3 validate_ocmf_signature.py \\
      --public-key "04<194_hex_chars>" \\
      --file ocmf_data.json \\
      --signature "<signature_hex>"
        """
    )
    
    parser.add_argument(
        '--public-key',
        required=True,
        help='Public key in hex format (uncompressed, 194 hex chars for P384)'
    )
    
    parser.add_argument(
        '--text',
        help='The text/message to verify (data-to-be-signed)'
    )
    
    parser.add_argument(
        '--signature',
        help='Signature in hex format (DER or raw r||s format)'
    )
    
    parser.add_argument(
        '--file',
        help='Read text from file (UTF-8)'
    )
    
    parser.add_argument(
        '--ocmf-json',
        help='OCMF JSON string containing SD (data) and SI (signature) fields'
    )
    
    parser.add_argument(
        '--ocmf-string',
        help='OCMF pipe-separated string format: OCMF|<data_json>|<signature_json>'
    )

    parser.add_argument(
        '--dump-candidates',
        action='store_true',
        help='Dump the exact message candidates that are attempted for verification (also written to /tmp).'
    )
    
    args = parser.parse_args()
    
    # Load public key
    try:
        print("Loading public key...")
        public_key = load_public_key_from_hex(args.public_key)
        print(f"✓ Public key loaded (brainpoolP384r1)")
    except Exception as e:
        print(f"✗ Error loading public key: {e}")
        sys.exit(1)
    
    # Determine message and signature
    message = None
    signature = None
    
    if args.ocmf_string:
        # Parse OCMF pipe-separated string
        try:
            message, signature = parse_ocmf_string(args.ocmf_string)
            print(f"✓ Parsed OCMF string format")
            print(f"  Data length: {len(message)} characters")
            print(f"  Signature length: {len(signature)} hex characters")
        except Exception as e:
            print(f"✗ Error parsing OCMF string: {e}")
            sys.exit(1)
    elif args.ocmf_json:
        # Parse OCMF JSON
        try:
            message, signature = parse_ocmf_json(args.ocmf_json)
            print(f"✓ Parsed OCMF JSON")
            print(f"  Data length: {len(message)} characters")
            print(f"  Signature length: {len(signature)} hex characters")
        except Exception as e:
            print(f"✗ Error parsing OCMF JSON: {e}")
            sys.exit(1)
    elif args.file:
        # Read from file
        if not args.signature:
            print("✗ Error: --signature is required when using --file")
            sys.exit(1)
        try:
            with open(args.file, 'r', encoding='utf-8') as f:
                message = f.read()
            signature = args.signature
            print(f"✓ Read message from file: {args.file}")
            print(f"  Message length: {len(message)} characters")
        except Exception as e:
            print(f"✗ Error reading file: {e}")
            sys.exit(1)
    elif args.text and args.signature:
        # Direct text and signature
        message = args.text
        signature = args.signature
        print(f"✓ Using provided text and signature")
        print(f"  Message length: {len(message)} characters")
    else:
        print("✗ Error: Must provide either --ocmf-string, --ocmf-json, or (--text and --signature), or (--file and --signature)")
        sys.exit(1)
    
    # Normalize JSON for OCMF (compact format, no spaces)
    # NOTE: Do NOT normalize / compact JSON. We verify exactly the extracted JSON bytes.
    # If the device signs compact JSON, the device output must already be compact.
    
    # Verify signature
    print("\nVerifying signature...")
    print(f"  Algorithm: ECDSA-brainpoolP384r1-SHA256")
    
    # Double-check what we're about to hash
    message_bytes = message.encode('utf-8') if isinstance(message, str) else message
    final_hash = sha256(message_bytes).hexdigest()
    print(f"  Message length: {len(message)} characters ({len(message_bytes)} bytes)")
    print(f"  Message hash (SHA256): {final_hash}")
    print(f"  Message preview (first 100 chars): {message[:100]}...")
    
    try:
        # Verify exactly what we received as <data_json> from the OCMF pipe string.
        candidates = [("json_exact", message)]
        if isinstance(message, str):
            candidates.extend(
                [
                    ("ocmf_prefix_json_exact", "OCMF|" + message),
                    ("json_exact_nullterm", message + "\x00"),
                    ("ocmf_prefix_json_exact_nullterm", "OCMF|" + message + "\x00"),
                ]
            )

        def dump_candidate(label, candidate_value):
            if not args.dump_candidates:
                return
            if isinstance(candidate_value, str):
                b = candidate_value.encode("utf-8")
            else:
                b = candidate_value
            print(f"\n--- candidate:{label} ---")
            print(f"len(chars)={len(candidate_value) if isinstance(candidate_value, str) else 'n/a'} len(bytes)={len(b)}")
            print("sha256(bytes)=", sha256(b).hexdigest())
            # Show a safe representation so NUL and other non-printables are visible.
            preview = candidate_value if isinstance(candidate_value, str) else b.decode("utf-8", errors="replace")
            print("repr=", repr(preview))
            out_path = f"/tmp/ocmf_message_candidate_{label}.txt"
            with open(out_path, "wb") as f:
                f.write(b)
            print("written=", out_path)

        for name, candidate in candidates:
            dump_candidate(name, candidate)
            print(f"\nAttempt: {name} (standard ECDSA over message bytes)")
            if verify_signature(public_key, candidate, signature):
                print("\n✓ SIGNATURE VALID - The message is authentic!")
                sys.exit(0)

            print(f"Attempt: {name} (prehashed ECDSA over SHA256(message))")
            if verify_signature_prehashed(public_key, candidate, signature):
                print("\n✓ SIGNATURE VALID - The message is authentic!")
                sys.exit(0)

            # Some devices sign the ASCII hex digest rather than the raw message (rare, but cheap to test).
            if isinstance(candidate, str):
                digest_hex = sha256(candidate.encode("utf-8")).hexdigest()
                dump_candidate(f"{name}_sha256hex", digest_hex)
                print(f"Attempt: {name} (standard ECDSA over sha256(message).hexdigest() bytes)")
                if verify_signature(public_key, digest_hex, signature):
                    print("\n✓ SIGNATURE VALID - The message is authentic!")
                    sys.exit(0)
                print(f"Attempt: {name} (prehashed ECDSA over SHA256(sha256(message).hexdigest()))")
                if verify_signature_prehashed(public_key, digest_hex, signature):
                    print("\n✓ SIGNATURE VALID - The message is authentic!")
                    sys.exit(0)

        print("\n✗ SIGNATURE INVALID - none of the tried variants matched.")
        sys.exit(1)
    except Exception as e:
        print(f"\n✗ Error during verification: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()

