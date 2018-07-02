#!/usr/bin/env python
# -*-coding:utf_8-*-

import random
import time


# Class created to get the mock values.
class Sensorslibrary(object):
    """Sensorslibrary"""

    def __init__(self):
        super(Sensorslibrary, self).__init__()

    @staticmethod
    def nfc():
        time.sleep(1)
        if random.randint(1, 4) == 3:
            return random.choice(['ABCD136468', 'BCDE789514', 'CDEF663247'])
        return None

    @staticmethod
    def flow():
        time.sleep(1)
        flow = 0.0
        a = 15
        while a > 0:
            flow += random.random()
            a -= 1
            time.sleep(1)
        return flow
