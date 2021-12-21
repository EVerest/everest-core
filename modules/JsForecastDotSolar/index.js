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
const axios = require('axios');

async function fetch_solar_api_data(mod) {
    // We pass mod as arg for future param config
    evlog.info("Making HTTP request...");
    let response = await axios.get("https://api.forecast.solar/estimate/watthours/52/12/37/0/5.67");
    let date = new Date();
    if (response.status != 200) {
        evlog.error(`Could not retrieve response data from solar forecast API at ${date}`);
        return;
    }
    let forecast_data = {utc_timestamp: date.toISOString(), forecast: response.data};
    mod.provides.main.publish.forecast(forecast_data);
};

function start_api_loop(mod) {
    const update_interval_milliseconds = mod.config.impl.main.update_interval * 60 * 1000;
    fetch_solar_api_data(mod, update_interval_milliseconds);
    setInterval(fetch_solar_api_data, update_interval_milliseconds, mod);
}

boot_module(async ({ setup, info, config, mqtt }) => {
    evlog.info("Booting JsForecastSolar!");
}).then((mod) => {
    // Call API for the first time and then set an interval to fetch the data regularly
    start_api_loop(mod);
});
