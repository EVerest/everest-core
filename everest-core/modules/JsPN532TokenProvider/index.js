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