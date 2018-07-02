"""
Main sensor
"""
class Sensor(object):
    """Main Sensor class"""
    def __init__(self, name):
        super(Sensor, self).__init__()
        self.name = name

    def setup(self):
        pass

    def get_data(self):
        pass

    def get_cumulative(self):
        pass

    def reset_cumulative(self):
        pass