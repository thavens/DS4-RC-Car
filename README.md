# DS4-RC-Car
Uses PyPS4Controller library
  - controller.py contains library hotfix for program


# How it Works
Raspberry pi pairs to ps4 controller and input is taken from controller and converted to serial data that is sent over serial port (wheel direction and speed) to an Arduino that controls the i2c bus on the motor drivers of the rc car.

Original RC car was the called: SEEED rover
  - replaced intel edison with my own raspi and arduino
 
# Note:
  - i2c on grove motor driver has conflicts with the raspi. It only works the first time you turn it on and then just stops working.
  - similar issue here: https://forum.dexterindustries.com/t/grove-i2c-motor-driver-with-grovepi/744/24
