/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
const { evlog, boot_module } = require('everestjs');

let sim_value = 5000;
let update_interval = 1000;
let current_charging_power = 0;

function on_update(mod) {
    mod.provides.main.publish.powermeter(
        {
            timestamp: Date.now()/1000,
            power_W: {
              total: sim_value - current_charging_power
            }
        }        
    );
}

function on_yetipowermeter(mod, val) {
    current_charging_power = val.power_W.total;
}

boot_module(async ({ setup, info, config, mqtt }) => {
    evlog.info("Booting JsSimPowermeter..");
    // Load config
    sim_value = config.impl.main.sim_value;
    update_interval = config.impl.main.update_interval;
    setup.uses.yetipowermeter.subscribe.powermeter(on_yetipowermeter);
}).then((mod) => {
    // Start output loop
    setInterval(on_update, update_interval, mod);
});
