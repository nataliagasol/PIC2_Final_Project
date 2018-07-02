from .main import Sensor

class NumericSensor(Sensor):
    """NumericSensor"""
    def __init__(self, name):
        super(NumericSensor, self).__init__(name)
        self.value = 0.0

    def get_data(self):
        return self.value

    def setup(self):
        pass
