import time

import RPi.GPIO as GPIO
import SimpleMFRC522

from .numeric import NumericSensor


class NFCSensor(NumericSensor):
    """NFCSensor class """

    def __init__(self, name):
        super(NFCSensor, self).__init__(name)
        self.value = None

    # Update the value of NFC sensor.
    def setup(self):
        reader = SimpleMFRC522.SimpleMFRC522()

        try:
            id, text = reader.read()
            self.value = id
        finally:
            GPIO.cleanup()

    def get_acumulative(self):
        return "Error"


class FlowSensor(NumericSensor):
    """FlowSensor class"""

    def __init__(self, name, pin):
        super(FlowSensor, self).__init__(name)
        self.value = 0.0
        self.flow_acumulative = 0.0
        self.pin = pin
        self.lastvalue = 0.0
        self.state = True

    # Add the values get from flow sensor converting the pulses given in liters with a conversion factor.
    def countPulse(self, channel):
        self.value = self.value + (0.5 / 1765)

    def setup(self):
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

        GPIO.add_event_detect(self.pin, GPIO.RISING, callback=self.countPulse)

        while True:
            time.sleep(3)
            if (self.lastvalue == self.value):
                self.flow_acumulative += self.value
                break
            self.lastvalue = self.value

        GPIO.cleanup()

    # Return the value of cumulative flow.
    def get_acumulative(self):
        return self.flow_acumulative

    def reset_cumulative(self):
        self.flow_acumulative = 0.0




class FlowSensorESP(NumericSensor):
    """FlowSensorESP class"""
    def __init__(self, name, logger):
        super(FlowSensorESP, self).__init__(name)
        self.logger = logger
        self.data_flow = data_flow


    # The callback for when the client receives a CONNACK response from the server.
    def on_connect(self,client,userdata,flags,rc):
        if rc == 0:
            self.logger.debug('Well connected')
        else:
            self.logger.debug('Bad connection. Returned code: ',rc)
        # Subscribing here mean that if we lose the connection and reconnect
        # then subscriptions will be renewed.
        client.subscribe("/sensor/FlowSensor")
        #data_difference = client.subscribe("/sensor/FlowDifference")

    # The callback for when a PUBLISH message is received from the server.
    def on_message(self,client,userdata,message):
        self.logger.debug('Message received')
        self.data_flow = message.payload

    def connect_mqtt(self):
        client = mqtt.Client()
        client.on_connect = self.on_connect
        client.on_message = self.on_message
        client.connect("192.168.43.110")
        # In order not to block call that porcesses network traffic, we use loop_start
        # that starts a new thread, that calls the loop method at regulars intervals for us.
        # It also handles re-connects automatically.
        client.loop_start()

    def get_data(self):
        data = self.data_flow
        return data
