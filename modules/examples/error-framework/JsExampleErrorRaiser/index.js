// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');

const conditions_lists = [
  {
    name: 'Only A is active',
    conditions: [
      {
        type: 'example/ExampleErrorA',
        sub_type: 'some sub type',
        active: true,
      },
      {
        type: 'example/ExampleErrorB',
        sub_type: 'some sub type',
        active: false,
      },
      {
        type: 'example/ExampleErrorC',
        sub_type: 'some sub type',
        active: false,
      },
      {
        type: 'example/ExampleErrorD',
        sub_type: 'some sub type',
        active: false,
      },
    ],
  },
  {
    name: 'Only B is active',
    conditions: [
      {
        type: 'example/ExampleErrorA',
        sub_type: 'some sub type',
        active: false,
      },
      {
        type: 'example/ExampleErrorB',
        sub_type: 'some sub type',
        active: true,
      },
      {
        type: 'example/ExampleErrorC',
        sub_type: 'some sub type',
        active: false,
      },
      {
        type: 'example/ExampleErrorD',
        sub_type: 'some sub type',
        active: false,
      },
    ],
  },
  {
    name: 'Only C & D are active',
    conditions: [
      {
        type: 'example/ExampleErrorA',
        sub_type: 'some sub type',
        active: false,
      },
      {
        type: 'example/ExampleErrorB',
        sub_type: 'some sub type',
        active: false,
      },
      {
        type: 'example/ExampleErrorC',
        sub_type: 'some sub type',
        active: true,
      },
      {
        type: 'example/ExampleErrorD',
        sub_type: 'some sub type',
        active: true,
      },
    ],
  },
  {
    name: 'No Error is active',
    conditions: [
      {
        type: 'example/ExampleErrorA',
        sub_type: 'some sub type',
        active: false,
      },
      {
        type: 'example/ExampleErrorB',
        sub_type: 'some sub type',
        active: false,
      },
      {
        type: 'example/ExampleErrorC',
        sub_type: 'some sub type',
        active: false,
      },
      {
        type: 'example/ExampleErrorD',
        sub_type: 'some sub type',
        active: false,
      },
    ],
  },
];

function sleep(ms) {
  return new Promise((resolve) => { setTimeout(resolve, ms); });
}

function check_conditions(mod) {
  evlog.info('');
  evlog.info('Check Conditions:');
  conditions_lists.forEach((element) => {
    const res = mod.provides.example_raiser.error_state_monitor.is_condition_satisfied(element.conditions);
    if (res) {
      evlog.info(`Condition '${element.name}' is satisfied`);
    } else {
      evlog.info(`Condition '${element.name}' is not satisfied`);
    }
  });
  evlog.info('');
  evlog.info('');
}

boot_module(async ({
  setup, info, config,
}) => {
  evlog.info('Nothing to set up');
}).then(async (mod) => {
  evlog.info('JsExample started');
  let error = mod.provides.example_raiser.error_factory.create_error(
    'example/ExampleErrorA',
    'some sub type',
    'This is an example message'
  );
  mod.provides.example_raiser.raise_error(error);
  check_conditions(mod);
  await sleep(1000);
  mod.provides.example_raiser.clear_error(error.type, error.sub_type);
  check_conditions(mod);
  await sleep(1000);
  error = mod.provides.example_raiser.error_factory.create_error(
    'example/ExampleErrorB',
    'some sub type',
    'This is also an example message'
  );
  mod.provides.example_raiser.raise_error(error);
  check_conditions(mod);
  await sleep(1000);
  mod.provides.example_raiser.clear_error(error.type, error.sub_type);
  check_conditions(mod);
  await sleep(1000);
  error = mod.provides.example_raiser.error_factory.create_error(
    'example/ExampleErrorC',
    'some sub type',
    'This is also an example message'
  );
  mod.provides.example_raiser.raise_error(error);
  check_conditions(mod);
  await sleep(1000);
  error = mod.provides.example_raiser.error_factory.create_error(
    'example/ExampleErrorD',
    'some sub type',
    'This is also an example message'
  );
  mod.provides.example_raiser.raise_error(error);
  check_conditions(mod);
  await sleep(1000);
  mod.provides.example_raiser.clear_all_errors_of_impl();
  check_conditions(mod);
  await sleep(1000);
});
