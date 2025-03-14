// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');
const axios = require('axios');

async function fetch_tibber_api_data(mod) {
  evlog.info('Fetching update from Tibber');
  const graphQLQuery = `{
      viewer {
        homes {
          currentSubscription {
            priceInfo {
              current {
                total startsAt currency level
              }
              today {
                total startsAt currency level
              }
              tomorrow {
                total startsAt currency level
              }
            }
          }
        }
      }
    }`;
  let apiKeyField = `Bearer ${mod.config.impl.main.api_key}`;
  let requestConfig = {
    headers: {
      'authorization': apiKeyField,
      'Content-Type': 'application/json'
    }
  }
  let postData = {
    query: graphQLQuery
  }
  await axios.post('https://api.tibber.com/v1-beta/gql', postData, requestConfig).then((response) => {

    if (response === undefined || response.status === undefined || response.status !== 200) {
      evlog.error('Could not retrieve response data from Tibber API');
      return;
    }
    //evlog.error(response);
    let today;
    let tomorrow;

    // validate input from tibber
    if (Object.prototype.hasOwnProperty.call(response, 'data')
      && Object.prototype.hasOwnProperty.call(response.data, 'data')
      && Object.prototype.hasOwnProperty.call(response.data.data, 'viewer')
      && Object.prototype.hasOwnProperty.call(response.data.data.viewer, 'homes')
      && Array.isArray(response.data.data.viewer.homes)
      && !(response.data.data.viewer.homes[0] === 'undefined')
      && Object.prototype.hasOwnProperty.call(response.data.data.viewer.homes[0], 'currentSubscription')
      && Object.prototype.hasOwnProperty.call(response.data.data.viewer.homes[0].currentSubscription, 'priceInfo')
      && Object.prototype.hasOwnProperty.call(response.data.data.viewer.homes[0].currentSubscription.priceInfo, 'today')
      && Array.isArray(response.data.data.viewer.homes[0].currentSubscription.priceInfo.today)
      && Object.prototype.hasOwnProperty.call(response.data.data.viewer.homes[0].currentSubscription.priceInfo, 'tomorrow')
      && Array.isArray(response.data.data.viewer.homes[0].currentSubscription.priceInfo.tomorrow)) {
      today = response.data.data.viewer.homes[0].currentSubscription.priceInfo.today;
      tomorrow = response.data.data.viewer.homes[0].currentSubscription.priceInfo.tomorrow;
    } else {
      evlog.error('Received malformed reply from Tibber API');
      return;
    }

    let schedule = {
      schedule_import: []
    };

    for (const response_entry of today.concat(tomorrow)) {
      // validate input from tibber
      if (Object.prototype.hasOwnProperty.call(response_entry, 'startsAt')
        && Object.prototype.hasOwnProperty.call(response_entry, 'total')
        && Object.prototype.hasOwnProperty.call(response_entry, 'currency')
        && typeof response_entry.currency === 'string'
        && response_entry.currency.length === 3
        && typeof response_entry.total === 'number'
        && typeof response_entry.startsAt === 'string') {
        // create one entry for the schedule
        let entry = {};
        entry['timestamp'] = new Date(response_entry.startsAt).toISOString();
        entry['price_per_kwh'] = {
          // Tibber returns the total energy cost including taxes and fees.
          // Add constant offset if needed for other costs.
          value: response_entry.total + mod.config.impl.main.additional_cost_per_kwh,
          currency: response_entry.currency
        }
        // copy into schedule_import array
        schedule['schedule_import'].push(entry);
      } else {
        evlog.error('Received malformed entry in today from Tibber API');
      }
    }

    mod.provides.main.publish.energy_price_schedule(schedule);
  }).catch(() => {
    evlog.error('Exception occurred when contacting tibber API');
  }
  );
};

function start_api_loop(mod) {
  const update_interval_milliseconds = mod.config.impl.main.update_interval * 60 * 1000;
  fetch_tibber_api_data(mod);
  setInterval(fetch_tibber_api_data, update_interval_milliseconds, mod);
}

boot_module(async ({ setup, info, config }) => {
  evlog.info('Booting JsTibber!');
}).then((mod) => {
  // Call API for the first time and then set an interval to fetch the data regularly
  start_api_loop(mod);
});
