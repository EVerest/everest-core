const { spawn } = require('child_process');
const { evlog, boot_module } = require('everestjs');

function JavaStartedDeferred(mqtt_base_path, module_name) {
  const self = this;
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
      '-cp', 'rise-v2g-secc-1.2.6.jar', 'com.v2gclarity.risev2g.secc.main.StartSECC',
      mqttServerAddress, mqttServerPort, mqtt_base_path,
    ];

    evlog.info(`Starting java subprocess: '${cmd}' with args ${args} in cwd '${cwd}'`);
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
      evlog.error(`Java child process exited with code '${code}' and signal '${signal}', terminating proxy module ${module_name}`);
      process.exit(1);
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

const mqtt_paths = {
  state: 'state',
  cmds: 'cmd',
  vars: 'var',
  ready: 'ready',
};

boot_module(async ({
  setup, info, config, mqtt,
}) => {
  // setup correct mqtt paths
  Object.keys(mqtt_paths).forEach((key) => {
    mqtt_paths[key] = `${config.impl.main.mqtt_base_path}/${mqtt_paths[key]}`;
  });

  // start java process
  const java_started = new JavaStartedDeferred(config.impl.main.mqtt_base_path, info.printable_identifier);

  mqtt.subscribe(mqtt_paths.vars, (mod, raw_data) => {
    if (mod === undefined) {
      evlog.warning('Already received data, but framework is not ready yet');
      return;
    }
    evlog.debug(`raw data: ${raw_data}`);
    const data = JSON.parse(raw_data);
    evlog.debug(`stringified data: ${data}`);
    if (data.impl_id === 'ac_charger') mod.provides.ac_charger.publish[data.var](data.val);
    else if (data.impl_id === 'dc_charger') mod.provides.dc_charger.publish[data.var](data.val);
    else {
      evlog.error(`Java RiseV2G tried to access unknown implementation with id ${data.impl_id}, ignoring!`);
    }
  });

  Object.keys(setup.provides.ac_charger.register).forEach((key) => {
    evlog.debug(`Providing cmd '${key}' on 'ac_charger' implementation ...`);
    setup.provides.ac_charger.register[key]((mod, arg) => {
      mod.mqtt.publish(mqtt_paths.cmds, JSON.stringify({
        impl_id: 'ac_charger',
        cmd: key,
        args: arg,
      }));
    });
  });

  Object.keys(setup.provides.dc_charger.register).forEach((key) => {
    evlog.debug(`Providing cmd '${key}' on 'dc_charger' implementation ...`);
    setup.provides.dc_charger.register[key]((mod, arg) => {
      mod.mqtt.publish(mqtt_paths.cmds, JSON.stringify({
        impl_id: 'dc_charger',
        cmd: key,
        args: arg,
      }));
    });
  });

  // resolve/reject promise when child process was started or could not be started
  mqtt.subscribe(mqtt_paths.state, (mod, raw_data) => {
    const data = JSON.parse(raw_data);
    if (data.ready === true) java_started.resolve();
  });

  java_started.promise.then(() => {
    evlog.info(`proxy module ${info.printable_identifier} now started...`);
  });

  return java_started.promise;
}).then((mod) => {
  mod.mqtt.publish(mqtt_paths.ready, JSON.stringify(true));
});
