#![allow(non_snake_case)]

use std::sync::Arc;
use tokio::sync::oneshot;

#[tokio::test]
#[everestrs::test(config = "config.yaml", module = "example_1", harness = true)]
async fn test_tokio_everest(module: &Module) {
    use super::*;
    use generated::*;

    let mock_service = Arc::new(MockExampleServiceSubscriber::new());

    let (tx, rx) = oneshot::channel();
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
    rx.await.expect("Timed out waiting for on_max_current");
}

#[everestrs::test(config = "config.yaml", module = "example_1", harness = true)]
#[tokio::test]
async fn test_everest_tokio(module: &Module) {
    use super::*;
    use generated::*;

    let mock_service = Arc::new(MockExampleServiceSubscriber::new());

    let (tx, rx) = oneshot::channel();
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
    rx.await.expect("Timed out waiting for on_max_current");
}
