import pytest
import pytest_asyncio
import json
import asyncio
from typing import Callable, Dict
import paho.mqtt.client as mqtt
from everest.testing.core_utils.probe_module import ProbeModule
from everest.testing.core_utils.everest_core import EverestCore
from everest.testing.core_utils.fixtures import *


class AsyncApiMqttHandler:
    def __init__(self, broker: str, port: int = 1883, everest_prefix: str = ""):
        self.broker = broker
        self.port = port
        self.everest_prefix = everest_prefix
        self.handlers: Dict[str, Callable] = {}
        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self.client.on_connect = self._on_connect
        self.client.on_message = self._on_message
        self.loop = None
        self._connected = None
        
    def register_handler(self, topic: str, handler: Callable):
        """Register a command handler for a topic"""
        self.handlers[topic] = handler
        self.client.subscribe(topic)
        
    async def publish(self, topic: str, payload: str):
        """Publish data to a topic"""
        if self._connected:
            await self._connected.wait()
        self.client.publish(topic, payload)
        
    def _on_connect(self, client, userdata, flags, reason_code, properties):
        """Subscribe to registered topics on connect"""
        for topic in self.handlers:
            client.subscribe(topic)
        if self._connected:
            self.loop.call_soon_threadsafe(self._connected.set)
            
    def _on_message(self, client, userdata, msg):
        """Dispatch message to handler in asyncio loop"""
        if msg.topic in self.handlers:
            asyncio.run_coroutine_threadsafe(
                self.handlers[msg.topic](msg.payload.decode()),
                self.loop
            )
            
    async def start(self):
        """Start the MQTT client"""
        self.loop = asyncio.get_event_loop()
        self._connected = asyncio.Event()
        self.client.connect(self.broker, self.port)
        self.client.loop_start()
        
        try:
            await asyncio.wait_for(self._connected.wait(), timeout=5.0)
        except asyncio.TimeoutError:
            raise TimeoutError("MQTT connection timeout")
        
    async def stop(self):
        """Stop the MQTT client"""
        self.client.loop_stop()
        self.client.disconnect()

@pytest_asyncio.fixture
async def async_api_mqtt_handler(everest_core: EverestCore) -> AsyncApiMqttHandler:
    handler = AsyncApiMqttHandler("localhost", 1883, everest_core.mqtt_external_prefix)
    await handler.start()
    yield handler
    await handler.stop()

@pytest.fixture
def probe_module(everest_core: EverestCore) -> ProbeModule:
    everest_core.start(standalone_module='probe')
    probe_module = ProbeModule(everest_core.get_runtime_session())
    probe_module.start()
    return probe_module

@pytest.mark.asyncio
@pytest.mark.everest_core_config('probe-system.yaml')
async def test_api_cmds(everest_core: EverestCore, async_api_mqtt_handler: AsyncApiMqttHandler, probe_module: ProbeModule):

    mqtt_prefix = everest_core.mqtt_external_prefix

    # register mqtt handler for get_boot_reason
    async def on_get_boot_reason(payload: str):
        request = json.loads(payload)
        response_topic = request['headers']['replyTo']
        response_payload = "ApplicationReset"
        await async_api_mqtt_handler.publish(f"{mqtt_prefix}{response_topic}", json.dumps(response_payload))

    # register mqtt handler for set_system_time
    async def on_set_system_time(payload: str):
        request = json.loads(payload)
        response_topic = request['headers']['replyTo']
        response_payload = True
        await async_api_mqtt_handler.publish(f"{mqtt_prefix}{response_topic}", json.dumps(response_payload))

    # register mqtt handler for update_firmware
    async def on_update_firmware(payload: str):
        request = json.loads(payload)
        response_topic = request['headers']['replyTo']
        response_payload = "Accepted"
        await async_api_mqtt_handler.publish(f"{mqtt_prefix}{response_topic}", json.dumps(response_payload))

    # register mqtt handler for upload_logs
    async def on_upload_logs(payload: str):
        request = json.loads(payload)
        response_topic = request['headers']['replyTo']
        response_payload = {
            "file_name": "test_log.txt",
            "upload_logs_status": "Accepted"
        }
        await async_api_mqtt_handler.publish(f"{mqtt_prefix}{response_topic}", json.dumps(response_payload))

    # register mqtt handler for is_reset_allowed
    async def on_is_reset_allowed(payload: str):
        request = json.loads(payload)
        response_topic = request['headers']['replyTo']
        response_payload = True
        await async_api_mqtt_handler.publish(f"{mqtt_prefix}{response_topic}", json.dumps(response_payload))

    async_api_mqtt_handler.register_handler(f"{everest_core.mqtt_external_prefix}everest_api/1/system/system_api/e2m/get_boot_reason", on_get_boot_reason)
    async_api_mqtt_handler.register_handler(f"{everest_core.mqtt_external_prefix}everest_api/1/system/system_api/e2m/set_system_time", on_set_system_time)
    async_api_mqtt_handler.register_handler(f"{everest_core.mqtt_external_prefix}everest_api/1/system/system_api/e2m/update_firmware", on_update_firmware)
    async_api_mqtt_handler.register_handler(f"{everest_core.mqtt_external_prefix}everest_api/1/system/system_api/e2m/upload_logs", on_upload_logs)
    async_api_mqtt_handler.register_handler(f"{everest_core.mqtt_external_prefix}everest_api/1/system/system_api/e2m/is_reset_allowed", on_is_reset_allowed)

    result = await probe_module.call_command(
        'system',
        'get_boot_reason',
        {}
    )
    assert result == "ApplicationReset"

    result = await probe_module.call_command(
        'system',
        'set_system_time',
        {'timestamp': "2024-01-01T00:00:00Z"}
    )
    assert result == True

    result = await probe_module.call_command(
        'system',
        'update_firmware',
        {
            'firmware_update_request': {
                'location': 'http://example.com/firmware.bin',
                'request_id': 1
            }
        }
    )
    assert result == "Accepted"

    result = await probe_module.call_command(
        'system',
        'upload_logs',
        {
            'upload_logs_request': {
                'location': 'http://example.com/logs'
            }
        }
    )
    assert result == {"file_name": "test_log.txt", "upload_logs_status": "Accepted"}

    result = await probe_module.call_command(
        'system',
        'is_reset_allowed',
        {'type': 'Soft'}
    )
    assert result == True
