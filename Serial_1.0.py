# -*- coding: utf-8 -*-
"""
Created on Thu Apr 29 15:59:29 2021

@author: Marwen
"""

import serial
import serial.tools.list_ports
import numpy as np
import warnings
import pyautogui

import time




def write_data(x, index):

    value = "{}{}f".format(int(x),index) # we attach here the value of the advance time with 'a' the index coresponding 
    print(value)                                 # in the arduino code
    ser.write(bytes(value, 'utf-8'))  # here we will send the message to the arduino 
    #time.sleep(0.7)
    #if(ser.in_waiting):
    ser_bytes = ser.readline()[:-2].decode("utf-8")
    print(ser_bytes)



def connect_to_arduino_if_exist():
    print("Connection to Arduino")
    arduino_ports = [
        p.device
        for p in serial.tools.list_ports.comports()
        if 'Arduino' in p.description  # may need tweaking to match new arduinos
    ]
    try:
        if not arduino_ports:
            raise IOError("No Arduino found")
        if len(arduino_ports) > 1:
            warnings.warn('Multiple Arduinos found - using the first')
        ser = serial.Serial(port =arduino_ports[0], baudrate ="9600");
        #print("Arduino is connected")
    except:
        #print("Arduino is not connected")
        ser = []
    return ser

def key(i):
        switcher={              # Need to be adapted to the differentkeys chosen to be set
                0:'q',
                1:'return',
                2:'up',
                3:'w',
                4:'left',
                5:'right',
                6:'down',
                7:'x',
                8:'q'
             }
        return switcher.get(i,"Invalid key")


    
    
    
ser = connect_to_arduino_if_exist()
if ser == []:
    print("Using the bluetooth connection")
    ser = serial.Serial('COM8', 9600) # The communication ports is to be defined 
    

## This part will be useful to adapt the different values of our arduino:


set_values = input("Do you want to change the parameters: ") # Taking input from user
ser.write(bytes(set_values, 'utf-8'))
if(set_values == '1'):
    advance_time = input("Insert the advance time: ")
    write_data(advance_time, 'b')            # This part is used to adjust the time advance                                
    time_step = input("Insert the speed of each step: ")
    write_data(time_step,'c')                # Same for sweeping the grid 3
    activ_time = input("Set the time before each activation: ")
    write_data(activ_time, 'd')        #  we also adjust the time before activating each sensor

    # Adapting the threshold can be optional if we have higher sensitivity of the capacitive sensors
    """
    threshold = input("Insert the threshold for the sensors: ")
    value = "{}af".format(int())
    """
else:
    print("The arduino is working with the initial set values")


while True:
    # Ce code fonctionne pour voir la valuer du bouton touched et released 
    ser_bytes = ser.readline().decode('ascii')
    #a, b = [int(s) for s in ser_bytes] 
    a = int(ser_bytes)
    print("The pushed button is {} corresponding to the key: {}  ".format(a, key(a) ))
    
    # upper button is referring to 2
    # right button is referring to 5
    # down button is referring to 6
    # Left button is referring to 4
    # the return button is referring to 0
    
    # using a qwertz keyboard 
    if a != 0 :
         pyautogui.press(key(a))

    
    # We know store the ints from the arduino we only need to access to grid 
    # A solution is to use Pyautogui

