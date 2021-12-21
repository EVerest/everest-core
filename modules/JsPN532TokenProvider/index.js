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
const pn532 = require('pn532');
const SerialPort = require('serialport');
const ndef = require('ndef');

var reader;
boot_module(async ({ setup, config }) => {
    evlog.info(`Opening serial port at '${config.impl.main.serial_port}' at baud rate ${config.impl.main.baud_rate}...`);
    reader = new pn532.PN532(new SerialPort(config.impl.main.serial_port, { baudRate: config.impl.main.baud_rate }));
    // wait for device to get ready (sadly this is an event instead of a promise --> we have to promisify it)
    await new Promise((resolve, reject) => reader.on('ready', () => resolve()));
    evlog.info("PN532 firmware version: ", await reader.getFirmwareVersion());
}).then((mod) => {
    reader.on("tag", async (tag) => {
        if(tag) {
            evlog.info("Got raw rfid/nfc tag: ", tag);
            
            // read tag data for debug/logging purposes
            var tagData = await reader.readNdefData();
            if(tagData) {
                evlog.debug("Raw tag ndef data: ", tagData);
                var records = ndef.decodeMessage(Array.from(tagData));
                evlog.debug("Tag records: ", records);
            }
            
            // emit new token into the everest system
            const data = {
                token: tag.uid,
                type: "rfid",
                timeout: mod.config.impl.main.timeout,
            };
            evlog.info('Publishing new rfid/nfc token: ', data);
            mod.provides.main.publish.token(data);
        }
    });
});