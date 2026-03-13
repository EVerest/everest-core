#![allow(non_snake_case)]

#[everestrs::test(config = "config_probe.yaml", module = "example_1", harness = true)]
fn test_harness_generates_counterpart(module: &Module) {
    use generated::*;
    use std::sync::Arc;

    struct Dummy;

    impl OnReadySubscriber for Dummy {
        fn on_ready(&self, _pub_impl: &ModulePublisher) {}
    }

    impl ExampleServiceSubscriber for Dummy {
        fn uses_something(&self, _context: &Context, _key: String) -> everestrs::Result<bool> {
            Ok(false)
        }
    }

    impl ExampleClientSubscriber for Dummy {
        fn on_max_current(&self, _context: &Context, _value: f64) {}
        fn on_error_raised(
            &self,
            _context: &Context,
            _error: everestrs::ErrorType<errors::example::Error>,
        ) {
        }
        fn on_error_cleared(
            &self,
            _context: &Context,
            _error: everestrs::ErrorType<errors::example::Error>,
        ) {
        }
    }

    let dummy = Arc::new(Dummy);
    let _pub = module.start(dummy.clone(), dummy.clone(), dummy.clone());
}

#[everestrs::harness(config = "config_probe.yaml", module = "example_1")]
mod some_module {
    use generated::*;
    use std::sync::Arc;

    struct Dummy;

    impl OnReadySubscriber for Dummy {
        fn on_ready(&self, _pub_impl: &ModulePublisher) {}
    }

    impl ExampleServiceSubscriber for Dummy {
        fn uses_something(&self, _context: &Context, _key: String) -> everestrs::Result<bool> {
            Ok(false)
        }
    }

    impl ExampleClientSubscriber for Dummy {
        fn on_max_current(&self, _context: &Context, _value: f64) {}
        fn on_error_raised(
            &self,
            _context: &Context,
            _error: everestrs::ErrorType<errors::example::Error>,
        ) {
        }
        fn on_error_cleared(
            &self,
            _context: &Context,
            _error: everestrs::ErrorType<errors::example::Error>,
        ) {
        }
    }

    #[everestrs::test(config = "config_probe.yaml", module = "example_1")]
    fn test_harness_in_module(module: &Module) {
        let dummy = Arc::new(Dummy);
        let _pub = module.start(dummy.clone(), dummy.clone(), dummy.clone());
    }
}
