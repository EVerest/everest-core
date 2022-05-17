import importlib
from pathlib import Path
from signal import SIGINT, signal
import sys
from threading import Event
from os import environ
import everestpy

log = everestpy.log

# make sure module path is available and import module
module_path = Path(environ.get("EV_PYTHON_MODULE"))
sys.path.append(module_path.parent.resolve().as_posix())
sys.path.append(module_path.parent.parent.resolve().as_posix())
module = importlib.import_module(f"{module_path.parent.name}.module")

module_adapter_ = None
setup = None
pub_cmds = None
wait_for_exit = Event()


def sigint_handler(_signum, _frame):
    wait_for_exit.set()


signal(SIGINT, sigint_handler)


def register_module_adapter(module_adapter):
    global module_adapter_
    module_adapter_ = module_adapter


def register_everest_register(_connections):
    vec = []
    if not pub_cmds:
        return vec

    for impl_id, cmds in pub_cmds.items():
        for cmd_name, _info in cmds.items():
            cmd = everestpy.EverestPyCmd()
            cmd.impl_id = impl_id
            cmd.cmd_name = cmd_name
            cmd.handler = lambda json_args, i=impl_id, c=cmd_name: getattr(
                module, f"{i}_{c}")(**json_args)

            vec.append(cmd)

    return vec


def wrapped_function(cmd_with_args):
    json_args = {}
    if cmd_with_args.arguments:
        for arg in cmd_with_args.arguments:
            json_args[arg] = None

    def kwarg_cmd(_first_arg, *args, **kwargs):
        if len(args) + len(kwargs) != len(json_args):
            raise ValueError("Incorrect number of arguments provided")
        json_args_it = iter(json_args)
        for arg in args:
            key = next(json_args_it)
            json_args[key] = arg
        for key, arg in kwargs.items():
            json_args[key] = arg
        return cmd_with_args.cmd(json_args)['retval']
    cmd = kwarg_cmd
    return cmd


def populate_vars(module_vars):
    module_interface = {}
    for k, v in module_vars.items():
        variables = {}
        for req_mod_id, rvv in v.items():
            variables[req_mod_id] = {}
            for kk, vv in rvv.items():
                variables[req_mod_id][f"subscribe_{kk}"] = vv
        module_interface[f"r_{k}"] = variables

    return module_interface


def populate_cmds(call_cmds, module_interface):
    for k, v in call_cmds.items():
        cmds = {}
        for req_mod_id, rvv in v.items():
            for kk, vv in rvv.items():
                cmds[f"call_{kk}"] = wrapped_function(vv)
            if f"r_{k}" in module_interface:
                module_interface[f"r_{k}"][req_mod_id] = {
                    **module_interface[f"r_{k}"][req_mod_id], **cmds}
            else:
                module_interface[f"r_{k}"][req_mod_id] = cmds
    return module_interface


def populate_module_setup(requirements, module_interface):
    module_setup = {}
    for k, v in module_interface.items():
        prefix = "r_"
        is_list = True
        if k.startswith(prefix):
            requirement_key = k[len(prefix):]
            min_connections = requirements[requirement_key].min_connections
            max_connections = requirements[requirement_key].max_connections
            if min_connections == 1 and min_connections == max_connections:
                is_list = False
        if not is_list:
            InternalType = type(f"{k}", (object, ), v[list(v.keys())[0]])
            module_setup[f"{k}"] = InternalType()
        else:
            InternalType = type(f"{k}", (dict, ), v)
            module_setup[f"{k}"] = InternalType()
            for kk, vv in v.items():
                InternalType = type(f"{kk}", (object, ), vv)
                module_setup[f"{k}"][f"{kk}"] = InternalType()
    return module_setup


def register_pre_init(reqs):
    global pub_cmds
    pub_cmds = reqs.pub_cmds
    module_interface = populate_vars(reqs.vars)
    module_interface = populate_cmds(reqs.call_cmds, module_interface)
    module_setup = populate_module_setup(reqs.requirements, module_interface)

    for k, v in reqs.pub_vars.items():
        variables = {}
        for kk, vv in v.items():
            variables[f"publish_{kk}"] = vv
        InternalType = type(f"r_{k}", (object, ), variables)
        module_setup[f"p_{k}"] = InternalType()

    if reqs.enable_external_mqtt:
        mqtt_functions = {
            "publish": module_adapter_.ext_mqtt_publish,
            "subscribe": module_adapter_.ext_mqtt_subscribe
        }
        MQTT = type("mqtt", (object, ), mqtt_functions)
        module_setup["mqtt"] = MQTT()

    Setup = type("Setup", (object, ), module_setup)
    global setup
    setup = Setup()


def register_init(module_configs, _module_info):
    module_config = None
    impl_configs = module_configs
    if "!module" in module_configs:
        module_config = module_configs["!module"]
        del impl_configs["!module"]

    module.pre_init(setup, module_config, impl_configs)
    module.init()


def register_ready():
    module.ready()


if __name__ == '__main__':
    EV_MODULE = environ.get('EV_MODULE')
    EV_PREFIX = environ.get('EV_PREFIX')
    EV_CONF_FILE = environ.get('EV_CONF_FILE')

    everestpy.register_module_adapter_callback(register_module_adapter)
    everestpy.register_everest_register_callback(register_everest_register)
    everestpy.register_init_callback(register_init)
    everestpy.register_pre_init_callback(register_pre_init)
    everestpy.register_ready_callback(register_ready)

    everestpy.init(EV_PREFIX, EV_CONF_FILE, EV_MODULE)

    wait_for_exit.wait()
