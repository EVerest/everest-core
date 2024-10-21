# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

from types import SimpleNamespace
import paho.mqtt.client as mqtt

class _MQTTHelper:

  def __init__(self, client: mqtt.Client, topics):
    self._client = client
    self._client.on_message = self._on_message
    self._topics = topics
    self._callbacks = {}

    for topic in self._topics:
      self._client.subscribe(topic)
      self._callbacks[topic[:-2] if topic.endswith("#") else topic] = None

  def _on_message(self, client, userdata, message):
    for key in self._callbacks.keys():
      if key in message.topic:
        if self._callbacks[key] is not None:
          self._callbacks[key](message.topic, message.payload)
        return

  def register_callback(self, topic, callback):
    self._callbacks[topic] = callback

class _ReceiveOperationFunctions:
  "This class is used to receive messages from the MQTT broker"

  def __init__(self, mqtt_helper: _MQTTHelper, topic):
    self._mqtt_helper = mqtt_helper
    self._topic = topic

  def register_callback(self, callback):
    "Registers a callback function to be called when a message is received on the topic"
    self._mqtt_helper.register_callback(self._topic, callback)

class SpyClient:

  def __init__(self, connected_mqtt_client: mqtt.Client, topic: str):

    self._client = connected_mqtt_client
    if topic != "":
      self._mqtt_prefix = topic
    else:
      self._mqtt_prefix = 'everest/api/1.0'
    topics = {
      f'{ self._mqtt_prefix }/#',
    }
    self._mqtt_helper = _MQTTHelper(self._client, topics)

    self.receive = SimpleNamespace()
    self.receive.spy = _ReceiveOperationFunctions(self._mqtt_helper, f'{ self._mqtt_prefix }')
