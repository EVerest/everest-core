const { evlog, boot_module } = require('everestjs');
const { setInterval } = require('timers');

boot_module(async ({ setup, info, config, mqtt }) => {

  // Subscribe external cmds
  mqtt.subscribe('/carsim/cmd/enable', (mod, en) => {enable(mod, {value: en})} );
  mqtt.subscribe('/carsim/cmd/execute_charging_session', (mod, str) => {execute_charging_session(mod, {value: str});});

  // register commands
  setup.provides.main.register.enable(enable);
  setup.provides.main.register.executeChargingSession(execute_charging_session);

  setup.uses.simulation_control.subscribe.simulation_feedback((mod, args)=>{mod.simulation_feedback = args;});

  // subscribe vars of used modules

}).then((mod) => {
  registerAllCmds(mod);
  mod.enabled = false;
  //enable(true);
  //execute_charging_session(mod, {value: "cp 12;sleep 2;cp 9;sleep 1;cp 6;sleep 10;cp 12"});
});

// Command enable
function enable(mod, raw_data) {
  if (mod === undefined || mod.provides === undefined) {
    evlog.warning('Already received data, but framework is not ready yet');
    return;
  }

  let en = raw_data.value==='true';

  // ignore if not changed
  if (mod.enabled==en) return;

  simdata_reset_defaults(mod);

  // Start/Stop execution timer
  if (en) {
    mod.enabled = true;
    mod.loopInterval = setInterval(simulation_loop, 250, mod);
  } else {
    mod.enabled = false;
   if (!(mod.loopInterval === undefined)) clearInterval(mod.loopInterval);
  }

  // Enable/Disable HIL
  mod.uses.simulation_control.call.enable({value:en});

  // Publish new simualtion enabled/disabled
  mod.provides.main.publish.enabled(en);
}


function simdata_reset_defaults(mod) {
  mod.simdata = {
    pp_resistor: 220.1,
    diode_fail: false,
    error_e: false,
    cp_voltage: 12.,
    rcd_current: 0.1,
    voltages: {L1: 230., L2: 230., L3: 230.},
    currents: {L1: 0., L2: 0., L3: 0., N: 0.},
    frequencies: {L1: 50., L2: 50., L3: 50.}
  };

  mod.simdata_setting = {
    cp_voltage: 12.,
    pp_resistor: 220.1,
    impedance: 500.,
    rcd_current: 0.1,
    voltages: {L1: 230., L2: 230., L3: 230.},
    currents: {L1: 0., L2: 0., L3: 0., N: 0.},
    frequencies: {L1: 50., L2: 50., L3: 50.}
  }

  mod.simCommands = [];
  mod.loopCurrentCommand = 0;
  mod.executionActive = false;

  mod.state = 'unplugged';

  mod.uses.simulation_control.call.setSimulationData({value: mod.simdata});
}


// Command execute_charging_session
function execute_charging_session(mod, args) {
  if (mod === undefined) {
    evlog.warning('Already received data, but framework is not ready yet');
    return;
  }

  if (!mod.enabled) {
    evlog.warning('Simulation disabled, cannot execute charging simulation.');
    return;
  }

  if (mod.executionActive === true) {
    evlog.warning('Execution of charging session simulation already running, cannot start new one');
    return;
  }

  let str = args.value;

  // start values
  simdata_reset_defaults(mod);
  addNoise(mod);

  mod.simCommands = parseSimCommands(mod,str);
  mod.loopCurrentCommand = -1;
  if (next_command(mod)) mod.executionActive = true;
}

// Prepare next command
function next_command(mod) {
  mod.loopCurrentCommand++;
  if (mod.loopCurrentCommand<mod.simCommands.length) {
    evlog.info(current_command(mod));
    return true;
  }
  return false;
}

function current_command(mod) {
  return mod.simCommands[mod.loopCurrentCommand];
}

// state machine for the car simulator
function car_statemachine(mod) {
 let amps1 = 0.;
 let amps2 = 0.;
 let amps3 = 0.;
 let amps = 0;
 switch (mod.state) {
  case 'unplugged':
    drawPower(mod, 0,0,0,0);
    mod.simdata_setting.cp_voltage = 12.;
    mod.simdata.error_e = false;
    mod.simdata.diode_fail = false;
    break;
  case 'pluggedin':
    drawPower(mod, 0,0,0,0);
    mod.simdata_setting.cp_voltage = 9.; 
    break;
  case 'charging_regulated':
    amps = dutyCycleToAmps(mod.simulation_feedback.pwm_duty_cycle);
    if (amps>mod.maxCurrent) amps = mod.maxCurrent;

    // do not draw power if EVSE paused by stopping PWM
    if (amps > 5.9) {
      mod.simdata_setting.cp_voltage = 6.; 
      if (mod.simulation_feedback.relais_on>0 && mod.numPhases>0) amps1 = amps;
      else amps1 = 0;
      if (mod.simulation_feedback.relais_on>1 && mod.numPhases>1) amps2 = amps;
      else amps2 = 0;
      if (mod.simulation_feedback.relais_on>2 && mod.numPhases>2) amps3 = amps;
      else amps3 = 0;
  
      drawPower(mod,amps1,amps2,amps3,0.2);
    } else {
      mod.simdata_setting.cp_voltage = 9.;
      drawPower(mod,amps1,amps2,amps3,0.2);
    }
    break;

  case 'charging_fixed':
    // Also draw power if EVSE stopped PWM - this is a break the rules mode to test the charging implementation!
    mod.simdata_setting.cp_voltage = 6.; 

    amps = mod.maxCurrent;

    if (amps>mod.maxCurrent) amps = mod.maxCurrent;
    
    if (mod.simulation_feedback.relais_on>0 && mod.numPhases>0) amps1 = amps;
    else amps1 = 0;
    if (mod.simulation_feedback.relais_on>1 && mod.numPhases>1) amps2 = amps;
    else amps2 = 0;
    if (mod.simulation_feedback.relais_on>2 && mod.numPhases>2) amps3 = amps;
    else amps3 = 0;

    drawPower(mod,amps1,amps2,amps3,0.2);
    break;

  case 'error_e':
    drawPower(mod, 0,0,0,0);
    mod.simdata.error_e = true;
    break;
  case 'diode_fail':
    drawPower(mod, 0,0,0,0);
    mod.simdata.diode_fail = true;
    break; 
  default:
    mod.state = 'unplugged';
    break;
 }
}

// IEC61851 Table A.8
function dutyCycleToAmps(dc) {
  if (dc<8./100.) return 0;
  if (dc<85./100.) return dc*100.*0.6;
  if (dc<96./100.) return (dc*100.-64)*2.5;
  if (dc<97./100.) return 80;
  return 0;
}

function drawPower(mod, l1,l2,l3,n) {
  mod.simdata_setting.currents.L1 = l1;
  mod.simdata_setting.currents.L2 = l2;
  mod.simdata_setting.currents.L3 = l3;
  mod.simdata_setting.currents.N = n;
}

function simulation_loop(mod) {

  // Execute sim commands until a command blocks or we are finished
  while (mod.executionActive) {
    let c = current_command(mod);

    if(c.exec(mod, c)) {
      // command was non-blocking, run next command immediately
      if (!next_command(mod)) {
        evlog.info('Finished simulation.');
        simdata_reset_defaults(mod) 
        mod.executionActive = false;
        break;
      }
    } else break; // command blocked, wait for timer to run this function again
  }

  car_statemachine(mod);
  addNoise(mod);

  // send new sim data to HIL
  mod.uses.simulation_control.call.setSimulationData({value: mod.simdata});
}

function addNoise(mod) {
  const noise = (1+(Math.random()-0.5)*0.02);
  const lonoise = (1+(Math.random()-0.5)*0.005);
  const impedance = mod.simdata_setting.impedance / 1000.;

  mod.simdata.currents.L1 = mod.simdata_setting.currents.L1 * noise;
  mod.simdata.currents.L2 = mod.simdata_setting.currents.L2 * noise;
  mod.simdata.currents.L3 = mod.simdata_setting.currents.L3 * noise;
  mod.simdata.currents.N = mod.simdata_setting.currents.N * noise;

  mod.simdata.voltages.L1 = mod.simdata_setting.voltages.L1 * noise - impedance * mod.simdata.currents.L1;
  mod.simdata.voltages.L2 = mod.simdata_setting.voltages.L2 * noise - impedance * mod.simdata.currents.L2;
  mod.simdata.voltages.L3 = mod.simdata_setting.voltages.L3 * noise - impedance * mod.simdata.currents.L3;

  mod.simdata.frequencies.L1 = mod.simdata_setting.frequencies.L1 * lonoise;
  mod.simdata.frequencies.L2 = mod.simdata_setting.frequencies.L2 * lonoise;
  mod.simdata.frequencies.L3 = mod.simdata_setting.frequencies.L3 * lonoise;

  mod.simdata.cp_voltage = mod.simdata_setting.cp_voltage * noise;
  mod.simdata.rcd_current = mod.simdata_setting.rcd_current * noise;
  mod.simdata.pp_resistor = mod.simdata_setting.pp_resistor * noise;
}

/* 
 Parses simcmds string into simulation commands object

 Command functions return true if they are done or false if they need to 
 be called again on the next sim loop (to implement blocking commands such as sleep)

*/
function parseSimCommands(mod, str) {
  // compile aliases here as well
  // and convert waiting times
  let simcmds = str.toLowerCase().replace(/\n/g,";").split(';');
  let compiledCommands = [];

  for (cmd of simcmds) {
    cmd = cmd.replace(/,/g,' ').split(' ');
    let cmdname = cmd.shift();
    let args = [];
    for (a of cmd) {args.push(parseFloat(a));}
    let c = mod.registeredCmds[cmdname];
    if (c===undefined || args.length != c.numargs) {
      // Ignoreing unknown command / command with wrong parameter count
      evlog.warning ("Ignoring command "+cmdname+" with "+args.length+" arguments");
    } else {
      compiledCommands.push({cmd: c.cmd, args: args, exec: c.exec});
    }
  }
  return compiledCommands;
}


// Register all available sim commands
function registerAllCmds(mod) {
  mod.registeredCmds = [];

  registerCmd (mod, 'cp', 1, (mod, c) => {
    mod.simdata_setting.cp_voltage = c.args[0]; 
    return true;
   });

   registerCmd (mod, 'sleep', 1, (mod, c) => {
    if (c.timeLeft === undefined) c.timeLeft = c.args[0]*4+1;
    return (!(c.timeLeft-->0)); 
   });

   registerCmd (mod, 'iec_wait_pwr_ready', 0, (mod, c) => {
    mod.state = 'pluggedin';
    if (mod.simulation_feedback===undefined) return false;
    if (mod.simulation_feedback.evse_pwm_running && dutyCycleToAmps(mod.simulation_feedback.pwm_duty_cycle)>0) return true;
    else return false;
   });

   registerCmd (mod, 'iso_wait_pwr_ready', 0, (mod, c) => {
    mod.state = 'pluggedin';
    if (mod.simulation_feedback===undefined) return false;
    if (mod.simulation_feedback.evse_pwm_running && mod.simulation_feedback.pwm_duty_cycle>0.04 && mod.simulation_feedback.pwm_duty_cycle<0.06) return true;
    else return false;
   });

   registerCmd (mod, 'draw_power_regulated', 2, (mod, c) => {
    mod.maxCurrent = c.args[0];
    mod.numPhases = c.args[1];
    mod.state = 'charging_regulated';
    return true;
   });

   registerCmd (mod, 'draw_power_fixed', 2, (mod, c) => {
    mod.maxCurrent = c.args[0];
    mod.numPhases = c.args[1];
    mod.state = 'charging_fixed';
    return true;
   });

   registerCmd (mod, 'pause', 0, (mod, c) => {
    mod.state = 'pluggedin';
    return true;
   });

   registerCmd (mod, 'unplug', 0, (mod, c) => {
    mod.state = 'unplugged';
    return true;
   });

   registerCmd (mod, 'error_e', 0, (mod, c) => {
    mod.state = 'error_e';
    return true;
   });

   registerCmd (mod, 'diode_fail', 0, (mod, c) => {
    mod.state = 'diode_fail';
    return true;
   });

   registerCmd (mod, 'rcd_current', 1, (mod, c) => {
    mod.simdata_setting.rcd_current = c.args[0];
    return true;
   });

   registerCmd (mod, 'pp_resistor', 1, (mod, c) => {
    mod.simdata_setting.pp_resistor = c.args[0];
    return true;
   });
}

function registerCmd(mod, name, numargs, execfunction) {
 mod.registeredCmds[name] = {cmd: name, numargs: numargs, exec: execfunction};
}

/*
 command reference:

 cp N - set cp to N volts - don't use except for bogus car simulation. Use iec_xxx commands for normal sessions
 sleep N - sleep N seconds
 iec_wait_pwr_ready - wait for power ready IEC mode only (i.e. a valid 10-95% PWM or so state B2)
 iso_wait_pwr_ready - wait for ISO power ready (B2 5% and ISO15118 EVCC successful connection, with fallback to IEC)
 draw_power_regulated|draw_power_fixed, A,N - switch to C and draw A amps from the N phases once relais turns on. 
                    Auto 0 if phases are not provided by EVSE. if fixed, draw power independent of PWM DC or ISO.
                    If regulated, follow ISO or PWM current limits.
 pause - sets CP level to 9V and car state machine stops drawing power. use draw_power_xxx to resume
 
 unplug - alias to cp 12V

 error_e - signal Error E (CP-PE short)
 diode_fail - fail diode

 rcd_current N - set rcd_current to N mA
 pp_resistor N - apply N Ohms to PP
 
 In case EVSE reports error F state machine cancels execution and resets cp to 12V


 TODO:

 - implement remaining commands
 - only draw power on phases that are switched on (add #phases in feedback packet) -> implemented in C++, recompile!, change in js code!

*/
