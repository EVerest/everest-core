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
const fs = require('fs')

function log_data(mod, log_type, str_data) {
    var file_path = mod.config.impl.main.file_path_prefix;
    file_path += '_';
    file_path += log_type;
    file_path += '.log';

    var log_line = Date.now().toString();
    log_line += ',';
    log_line += str_data;
    log_line += '\r\n';

    fs.appendFile(file_path, log_line, err => {
        if (err) {
            evlog.error(err);
            return;
        }
    })
}

function on_msg(mod, data) {
    for (const [key, value] of Object.entries(data)) {
        log_data(mod, `${key}`, `${value}`);
    }
}

function on_pm_grid(mod, val) {
    log_data(mod, 'pm_grid_w_total', `${val.power_W.total}`);
}

function on_pm_ev(mod, val) {
    log_data(mod, 'pm_ev_w_total', `${val.power_W.total}`);
}

boot_module(async ({ setup, info, config, mqtt }) => {
    evlog.info("Booting JsEmgrLogger..");

    // Subscribe to energy_manager logging
    setup.uses.emgr.subscribe.logging(on_msg);
    setup.uses.pm_grid.subscribe.powermeter(on_pm_grid);
    setup.uses.pm_ev.subscribe.powermeter(on_pm_ev);
}).then((mod) => {

});
