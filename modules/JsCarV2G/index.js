// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
const { spawn } = require('child_process');
const { evlog, boot_module } = require('everestjs');
const os = require('os');

let network_interface;

function JavaStartedDeferred(mqtt_base_path, module_name, mod) {
  self = this;
  this.promise = new Promise((resolve, reject) => {
    self.resolve = resolve;
    const mqttServerAddress = process.env.MQTT_SERVER_ADDRESS || 'localhost';
    const mqttServerPort = process.env.MQTT_SERVER_PORT || '1883';
    const cmd = 'java';

    // FIXME: the path hierarchy should be well defined,
    //        i.e where to find 3rd party things
    const cwd = `${process.cwd()}/rise_v2g`;
    const args = [
      '-Djava.net.preferIPv4Stack=false', '-Djava.net.prefecom.v2gclarity.rrIPv6Addresses=true',
      '-cp', 'rise-v2g-evcc-1.2.6.jar', 'com.v2gclarity.risev2g.evcc.main.StartEVCC',
      mqttServerAddress, mqttServerPort, mqtt_base_path, mod.payment, mod.energymode, network_interface,
    ];

    evlog.debug(`Starting java subprocess: '${cmd}' with args ${args} in cwd '${cwd}'`);
    // Workaround for IPv6 unavailable in java child when spawned from nodejs:
    // https://stackoverflow.com/questions/34108124/ip-addresses-of-network-interfaces-in-a-java-process-spawned-by-nodejs/52573455
    // STDIN must be disabled because otherwise it is connected to fd 0 which disables ipv6 in jave (wtf)
    const java = spawn(cmd, args, {
      cwd,
      stdio: ['ignore', 'pipe', 'pipe'],
    });

    // handle child stdio and normal child exit
    java.stdout.on('data', (data) => {
      evlog.debug(`java stdout: ${data}`);
    });

    java.stderr.on('data', (data) => {
      evlog.warning(`java stderr: ${data}`);
    });

    java.on('close', (code, signal) => {
      if (code === 0) {
        evlog.info(`Java child process exited with code '${code}' and signal '${signal}'`);
        mod.provides.ev.publish.V2G_Session_Finished();
      } else {
        evlog.error(`Java child process exited with code '${code}' and signal '${signal}'`
                    + `, terminating proxy module ${module_name}`);
        process.exit(1);
      }
    });

    java.on('error', (err) => {
      evlog.error(`Failed to start java subprocess: ${err}`);
      reject(err);
    });

    // kill java subprocess on sigterm
    process.on('SIGTERM', () => {
      evlog.warning('SIGTERM received: terminating java subprocess');
      if (!java.kill('SIGTERM')) java.kill('SIGKILL');
      process.exit(0);
    });
  });
}

function check_network_interface(network_iface) {
  let networkfound = false;
  const net_init = os.networkInterfaces();

  for (const key of Object.keys(net_init)) {
    if (key === network_iface) {
      networkfound = true;
    }
  }

  return networkfound;
}

const mqtt_paths = {
  state: 'state',
  cmds: 'cmd',
  vars: 'var',
  ready: 'ready',
};

boot_module(async ({
  setup, info, config, mqtt,
}) => {

  network_interface = config.impl.main.device;
  if (check_network_interface(network_interface) === false) {
    evlog.warning(`The network interface ${network_interface} was not found!`);
  }

  // setup correct mqtt paths
  Object.keys(mqtt_paths).forEach((key) => {
    mqtt_paths[key] = `${config.impl.main.mqtt_base_path}/${mqtt_paths[key]}`;
  });

  mqtt.subscribe(mqtt_paths.vars, (mod, raw_data) => {
    if (mod === undefined) {
      evlog.warning('Already received data, but framework is not ready yet');
      return;
    }
    evlog.debug(`raw data: ${raw_data}`);
    const data = JSON.parse(raw_data);
    evlog.debug(`stringified data: ${data}`);
    if (data.impl_id === 'ev') mod.provides.ev.publish[data.var](data.val);
    else {
      evlog.error(`Java RiseV2G tried to access unknown implementation with id ${data.impl_id}, ignoring!`);
    }
  });

  Object.keys(setup.provides.ev.register).filter((key) => key !== 'start_charging').forEach((key) => {
    evlog.debug(`Providing cmd '${key}' on 'ev' implementation ...`);
    setup.provides.ev.register[key]((mod, arg) => {
      mod.mqtt.publish(mqtt_paths.cmds, JSON.stringify({
        impl_id: 'ev',
        cmd: key,
        args: arg,
      }));
    });
  });

  // resolve/reject promise when child process was started or could not be started
  mqtt.subscribe(mqtt_paths.state, (mod, raw_data) => {
    const data = JSON.parse(raw_data);
    if (data.ready === true) mod.java_started.resolve();
  });

  setup.provides.ev.register.start_charging(start_v2g);
}).then((mod) => {
  mod.mqtt.publish(mqtt_paths.ready, JSON.stringify(true));
});

function start_v2g(mod, args) {
  let started = false;
  mod.payment = args.PaymentOption;
  mod.energymode = args.EnergyTransferMode;

  switch (mod.config.impl.main.stack_implementation) {
    case 'Josev':
      // Todo
      break;
    case 'OpenV2G':
      // Todo
      break;
    case 'RISE-V2G':
    default:
      // start java process
      mod.java_started = new JavaStartedDeferred(
        mod.config.impl.main.mqtt_base_path,
        mod.info.printable_identifier,
        mod
      );

      mod.java_started.promise.then(() => {
        evlog.info(`proxy module ${mod.info.printable_identifier} now started...`);
      });

      started = true;
      break;
  }

  return started;
}
