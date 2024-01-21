import math
import serial  # pip install pyserial
import numpy as np
from time import sleep
import time
import json
import keyboard  # pip install keyboard

hSerial = serial.Serial('COM3', 115200, timeout=1, parity=serial.PARITY_NONE)
sleep(0.5)
timestr = time.strftime("%Y%m%d-%H%M%S")
hFile = open("data_two_position_controller_%s.txt" % (timestr), "a")

hSerial.reset_input_buffer()
hSerial.flush()
run = True



while run:
    text = hSerial.readline()
    textString = text.decode("utf-8")
    print(textString)



    try:
        sample = json.loads(text)
        temperatureAnalog = sample["RPM"]
        hFile.write("%s" % textString)
    except ValueError:
        print("Bad JSON")
        print("%s\r" % {text})
        hSerial.flush()
        hSerial.reset_input_buffer()


    if keyboard.is_pressed("q"):
        run = False  # finishing the loop
        break

hSerial.close()
hFile.close()