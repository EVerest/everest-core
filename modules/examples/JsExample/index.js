// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');

function sleep(ms) {
  return new Promise((resolve) => { setTimeout(resolve, ms); });
}

boot_module(async ({
  setup, info, config,
}) => {
  evlog.info('Nothing to set up');
}).then(async (mod) => {
  evlog.info('JsExample started');
  const handle = mod.provides.example.raise.example_ExampleErrorA('Example error', 'Low');
  evlog.info(`Raised error example_ExampleErrorA: ${JSON.stringify(handle, null, '  ')}`);
  await sleep(2000);
  mod.provides.example.raise.example_ExampleErrorA('Example error', 'Medium');
  await sleep(2000);
  mod.provides.example.raise.example_ExampleErrorA('Example error', 'High');
  await sleep(2000);
  mod.provides.example.request_clear_uuid(handle);
  await sleep(2000);
  mod.provides.example.request_clear_all_of_type.example_ExampleErrorA();
  await sleep(2000);
  mod.provides.example.raise.example_ExampleErrorB('Example error', 'Low');
  mod.provides.example.raise.example_ExampleErrorC('Example error', 'Medium');
  await sleep(2000);
  mod.provides.example.request_clear_all();
});
