# -*- coding: utf-8 -*-
import serial
from time import sleep

# ----- Setup the Serial COM Port ------

ser = serial.Serial(
port='/dev/ttyS0',
baudrate = 9600,
parity=serial.PARITY_NONE,
stopbits=serial.STOPBITS_ONE,
bytesize=serial.EIGHTBITS,
timeout=1,
)

# ----- Read the Shear Load -----

while 1:
	
	cmd = input("\n\nDigite o comando: ")
	cmd = cmd+"\r"
	ser.write(cmd.encode())
	print(cmd)
	sleep(0.1)

	print("\nDados recebidos:")
	data = ser.read(35)
	print(data)






