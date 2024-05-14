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

function check_conditions(setup) {
  evlog.info('');
  evlog.info('Check Conditions:');
  conditions_lists.forEach((element) => {
    const res = setup.uses.example_raiser.error_state_monitor.is_condition_satisfied(
      element.conditions
    );
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
  // setup.uses.example_raiser.subscribe_error(
  //   'example/ExampleErrorA',
  //   (mod, error) => {
  //     evlog.info('Received error example_ExampleErrorA: ' + JSON.stringify(error, null, '  '));
  //   },
  //   (mod, error) => {
  //     evlog.info('Received error cleared example_ExampleErrorA: ' + JSON.stringify(error, null, '  '));
  //   }
  // );
  setup.uses.example_raiser.subscribe_all_errors(
    (error) => {
      evlog.info(`Received error: ${error.type}`);
      check_conditions(setup);
    },
    (error) => {
      evlog.info(`Received error cleared: ${error.type}`);
      check_conditions(setup);
    }
  );
}).then((mod) => {
  evlog.info('JsExampleErrorSubscriber started');
});
