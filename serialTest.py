import Adafruit_BBIO.UART as UART
import serial
import sys

UART.setup("UART2") #Need to use sudo python <program nam> for this
UART.setup("UART1") #Need for the other arduino for multicore

ser2 = serial.Serial(port = "/dev/ttyO2", baudrate=9600)
ser1 = serial.Serial(port = "/dev/ttyO1", baudrate=9600)
ser2.close()
ser1.close()
ser2.open()
ser1.open()
if ser2.isOpen() and ser1.isOpen():
	print "Serial is open!"
	speed_one = int(sys.argv[1])
	speed_two = int(sys.argv[2])
	speed_three = int(sys.argv[3])
	speed_four = int(sys.argv[4])
	right_side_status = str(sys.argv[5])
	left_side_status = str(sys.argv[6])
	x_coordinate = int(sys.argv[7])
	y_coordinate = int(sys.argv[8])
	fire_gun = int(sys.argv[9])
	final_string = str(speed_one) + "k" + str(speed_two) +"k" + str(speed_three) + "k" + str(speed_four) + "k" + str(right_side_status) + str(left_side_status) + str(x_coordinate) + "k" + str(y_coordinate) + "k" + str(fire_gun) 
	ser2.write(final_string)
	ser1.write(final_string)
	ser2.close()
	ser1.close()
