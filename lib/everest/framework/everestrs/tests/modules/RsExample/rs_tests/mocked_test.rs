#![allow(non_snake_case)]

#[everestrs::test(config = "config_probe.yaml", module = "example_1", harness = true)]
fn test_mocked_generates_counterpart(module: &Module) {
    use generated::*;
    use std::sync::Arc;

    let mock_service = Arc::new(MockExampleServiceSubscriber::new());

    let (tx, rx) = std::sync::mpsc::channel();
    let mut mock_client = MockExampleClientSubscriber::new();
    mock_client
        .expect_on_max_current()
        .withf(|_, value| *value == 123.0)
        .times(1)
        .return_once(move |_, _| {
            tx.send(()).unwrap();
        });
    let mock_client = Arc::new(mock_client);

    let mut mock_on_ready = MockOnReadySubscriber::new();
    mock_on_ready.expect_on_ready().times(1).return_once(|_| ());

    let _pub = module.start(Arc::new(mock_on_ready), mock_service, mock_client);

    // Wait for RsExample's on_ready to publish max_current(123.0).
    rx.recv_timeout(std::time::Duration::from_secs(5))
        .expect("Timed out waiting for on_max_current");
}

#[everestrs::harness(config = "config_probe.yaml", module = "example_1")]
mod some_module {
    use generated::*;
    use std::sync::Arc;

    #[everestrs::test(config = "config_probe.yaml", module = "example_1")]
    fn test_mocked_in_module(module: &Module) {
        let mock_service = Arc::new(MockExampleServiceSubscriber::new());

        let (tx, rx) = std::sync::mpsc::channel();
        let mut mock_client = MockExampleClientSubscriber::new();
        mock_client
            .expect_on_max_current()
            .withf(|_, value| *value == 123.0)
            .times(1)
            .return_once(move |_, _| {
                tx.send(()).unwrap();
            });
        let mock_client = Arc::new(mock_client);

        let mut mock_on_ready = MockOnReadySubscriber::new();
        mock_on_ready.expect_on_ready().times(1).return_once(|_| ());

        let _pub = module.start(Arc::new(mock_on_ready), mock_service, mock_client);

        // Wait for RsExample's on_ready to publish max_current(123.0).
        rx.recv_timeout(std::time::Duration::from_secs(5))
            .expect("Timed out waiting for on_max_current");
    }

    #[everestrs::test(config = "config_probe.yaml", module = "example_1")]
    #[should_panic]
    fn test_mocked_with_panic_handler(module: &Module) {
        let mock_service = Arc::new(MockExampleServiceSubscriber::new());

        let (tx, rx) = std::sync::mpsc::channel();
        let mut mock_client = MockExampleClientSubscriber::new();
        mock_client
            .expect_on_max_current()
            .withf(|_, value| *value != 123.0)
            .times(1)
            .return_once(move |_, _| {
                tx.send(()).unwrap();
            });
        let mock_client = Arc::new(mock_client);

        let mut mock_on_ready = MockOnReadySubscriber::new();
        mock_on_ready.expect_on_ready().times(1).return_once(|_| ());

        let _pub = module.start(Arc::new(mock_on_ready), mock_service, mock_client);

        // Wait for RsExample's on_ready to publish max_current(123.0).
        rx.recv_timeout(std::time::Duration::from_secs(5))
            .expect("Timed out waiting for on_max_current");
    }

    #[everestrs::test(config = "config_probe.yaml", module = "example_1")]
    #[should_panic]
    fn test_mocked_with_panic_mocks(module: &Module) {
        let mock_service = Arc::new(MockExampleServiceSubscriber::new());

        let mut mock_client = MockExampleClientSubscriber::new();
        mock_client
            .expect_on_max_current()
            .times(2..) // No one will call us twice.
            .return_const(());
        let mock_client = Arc::new(mock_client);

        let mut mock_on_ready = MockOnReadySubscriber::new();
        mock_on_ready.expect_on_ready().times(1).return_once(|_| ());

        let _pub = module.start(Arc::new(mock_on_ready), mock_service, mock_client);
    }
}
