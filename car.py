from pyPS4Controller.controller import Controller
from serial import Serial
import datetime
import threading
import time
from multiprocessing import Process, Value
import ctypes

class MyController(Controller):

    def __init__(self, **kwargs):
        Controller.__init__(self, **kwargs)
        self.map = self.interpolater(0, 32768, 0, 255)
        self.rdir = 2 #True is forward
        self.ldir = 2
        self.lspeed = 0
        self.rspeed = 0
        self.port = Serial("/dev/ttyS0", baudrate=115200, timeout=1)
        self.port.flush()
        self.start = datetime.datetime.now()

    def on_L3_up(self, value):
        value = self.map(-value)
        if self.ldir == 1:
            self.ldir = 2
        self.lspeed = value
        self.task()

    def on_L3_down(self, value):
        value = self.map(value)
        if self.ldir == 2:
            self.ldir = 1
        self.lspeed = value
        self.task()

    def on_L3_y_at_rest(self):
        self.lspeed = 0
        self.send_data()

    def on_R3_up(self, value):
        value = self.map(-value)
        if self.rdir == 1:
            self.rdir = 2
        self.rspeed = value
        self.task()

    def on_R3_down(self, value):
        value = self.map(value)
        if self.rdir == 2:
            self.rdir = 1
        self.rspeed = value
        self.task()

    def on_R3_y_at_rest(self):
        self.rspeed = 0
        self.send_data()

    def interpolater(self, left_min, left_max, right_min, right_max):
        # Figure out how 'wide' each range is
        leftSpan = left_max - left_min
        rightSpan = right_max - right_min

        # Compute the scale factor between left and right values
        scaleFactor = rightSpan / leftSpan

        # create interpolation function using pre-calculated scaleFactor
        def interp_fn(value):
            return right_min + int((value-left_min)*scaleFactor)

        return interp_fn


    def task(self):
        if 0.025 < (datetime.datetime.now() - self.start).total_seconds():
            self.start = datetime.datetime.now()
            self.send_data()

    def send_data(self):
        dir = (self.rdir << 2) + self.ldir
        msg = bytes([self.rspeed, self.lspeed, dir])
        #, ord('\n')])
        self.port.write(msg)
        print('Vr =', self.rspeed, 'Vl =', self.lspeed, 'rdir =', self.rdir, 'ldir =', self.ldir, 'rldir =', dir)



controller = MyController(interface="/dev/input/js0", connecting_using_ds4drv=False)
controller.listen()
