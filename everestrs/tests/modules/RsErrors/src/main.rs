#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

fn main() {}

#[cfg(test)]
mod tests {
    /// In this test we check that we can deserialize all four errors.
    #[test]
    fn test_duplicate() {
        use crate::generated::errors::errors_duplicate::Error;
        use crate::generated::errors::errors_duplicate::ExampleErrorsError;
        for (error_str, error_enum) in [
            (
                "example_errors/ExampleErrorA",
                ExampleErrorsError::ExampleErrorA,
            ),
            (
                "example_errors/ExampleErrorB",
                ExampleErrorsError::ExampleErrorB,
            ),
            (
                "example_errors/ExampleErrorC",
                ExampleErrorsError::ExampleErrorC,
            ),
            (
                "example_errors/ExampleErrorD",
                ExampleErrorsError::ExampleErrorD,
            ),
        ] {
            assert_eq!(
                serde_yaml::from_str::<Error>(error_str).unwrap(),
                Error::ExampleErrors(error_enum)
            );
        }
    }

    /// In this test we check that we can only deserialize the selected two
    /// errors.
    #[test]
    fn test_selected() {
        use crate::generated::errors::errors_selected::Error;
        use crate::generated::errors::errors_selected::ExampleErrorsError;
        for (error_str, error_enum) in [
            (
                "example_errors/ExampleErrorA",
                ExampleErrorsError::ExampleErrorA,
            ),
            (
                "example_errors/ExampleErrorB",
                ExampleErrorsError::ExampleErrorB,
            ),
        ] {
            assert_eq!(
                serde_yaml::from_str::<Error>(error_str).unwrap(),
                Error::ExampleErrors(error_enum)
            );
        }

        for error_str in [
            "example_errors/ExampleErrorC",
            "example_errors/ExampleErrorD",
        ] {
            assert!(serde_yaml::from_str::<Error>(error_str).is_err());
        }
    }

    /// This test should just compile. The deserialization is tested above.
    #[test]
    fn test_multiple() {
        {
            use crate::generated::errors::errors_multiple::ExampleErrorsError;
            let _ = ExampleErrorsError::ExampleErrorA;
            let _ = ExampleErrorsError::ExampleErrorB;
            let _ = ExampleErrorsError::ExampleErrorC;
            let _ = ExampleErrorsError::ExampleErrorD;
        }

        {
            use crate::generated::errors::errors_multiple::MoreErrorsError;
            let _ = MoreErrorsError::ExampleErrorA;
            let _ = MoreErrorsError::MoreError;
            let _ = MoreErrorsError::SnakeCaseError;
        }
    }

}
