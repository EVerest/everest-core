const { evlog, boot_module } = require('everestjs');
const axios = require('axios');

async function fetch_tibber_api_data(mod) {
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
            "authorization": apiKeyField,
            "Content-Type": "application/json"
        }
    }
    let postData = {
        query: graphQLQuery
    }
    let response = await axios.post("https://api.tibber.com/v1-beta/gql", postData, requestConfig);
    if (response.status != 200) {
        evlog.error(`Could not retrieve response data from Tibber API at ${date} - ${response.data}`);
        return;
    }
    let response_data = response.data.data.viewer.homes[0].currentSubscription.priceInfo;
    let date = new Date();
    let forecast = {utc_timestamp: date.toISOString()};
    forecast.prices = [];
    forecast.prices.push(...response_data.today);
    forecast.prices.push(...response_data.tomorrow);
    for (let i = 0 ; i < forecast.prices.length ; i++) {
        let current_price = forecast.prices[i];
        let new_date_string = new Date(current_price.startsAt).toISOString();
        current_price.startsAt = new_date_string;
    }
    mod.provides.main.publish.forecast(forecast);
};

function start_api_loop(mod) {
    const update_interval_milliseconds = mod.config.impl.main.update_interval * 60 * 1000;
    fetch_tibber_api_data(mod, update_interval_milliseconds);
    setInterval(fetch_tibber_api_data, update_interval_milliseconds, mod);
}

boot_module(async ({ setup, info, config, mqtt }) => {
    evlog.info("Booting JsForecastSolar!");
}).then((mod) => {
    // Call API for the first time and then set an interval to fetch the data regularly
    start_api_loop(mod);
});
