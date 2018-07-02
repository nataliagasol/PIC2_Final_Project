from sensorslibrary import Sensorslibrary
from .numeric import NumericSensor


class NFCSensor(NumericSensor):
    """NFCSensor"""

    def __init__(self, name):
        super(NFCSensor, self).__init__(name)
        self.value = None

    def setup(self):
        try:
            id = Sensorslibrary.nfc()
            self.value = id
        except:
            print "Error!"

    def get_acumulative(self):
        return "Error"


class FlowSensor(NumericSensor):
    """FlowSensor"""

    def __init__(self, name):
        super(FlowSensor, self).__init__(name)
        self.flow_acumulative = 0.0

    def setup(self):
        self.value = Sensorslibrary.flow()
        self.flow_acumulative += self.value

    def get_acumulative(self):
        return self.flow_acumulative

    def reset_cumulative(self):
        self.flow_acumulative = 0.0
