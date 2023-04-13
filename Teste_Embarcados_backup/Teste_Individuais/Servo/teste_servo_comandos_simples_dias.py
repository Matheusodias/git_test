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

#while 1:
print("\nComandos seriais! \n\nComandos enviados:")
cmd = ("RW\r")
#cmd = ("MV A=80 V=32212 G\r")
#cmd = ("CMD UCP UDM\r")
#cmd = ("X\r")
#cmd = ("Z\r")
ser.write(cmd.encode())
print(cmd)
sleep(2)

print("\nDados recebidos:")
data = ser.read(35)
print(data)





