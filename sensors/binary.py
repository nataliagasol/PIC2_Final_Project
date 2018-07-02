from .main import Sensor

class BinarySensor(Sensor):
    """BinarySensor"""
    def __init__(self, name):
        super(BinarySensor, self).__init__(name)
        self.state = False

    def get_data(self):
        return self.state

    def setup(self):
        pass
