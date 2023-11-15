// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');

boot_module(async ({
  setup, info, config,
}) => {
  // setup.uses.example.subscribe_error.example_ExampleErrorA(
  //   (mod, error) => {
  //     evlog.info('Received error example_ExampleErrorA: ' + JSON.stringify(error, null, '  '));
  //   },
  //   (mod, error) => {
  //     evlog.info('Received error cleared example_ExampleErrorA: ' + JSON.stringify(error, null, '  '));
  //   }
  // );
  setup.uses.example.subscribe_all_errors(
    (mod, error) => {
      evlog.info(`Received error: ${JSON.stringify(error, null, '  ')}`);
    },
    (mod, error) => {
      evlog.info(`Received error cleared: ${JSON.stringify(error, null, '  ')}`);
    }
  );
}).then((mod) => {
  evlog.info('JsExampleUser started');
});
