import serial
import serial.tools.list_ports as port_list
import time


ports = list(port_list.comports())
print([x.device for x in ports])

port = ports[4].device
baudrate = 115200
serialPort = serial.Serial(port=port, baudrate=baudrate,
                                bytesize=8, timeout=1, stopbits=serial.STOPBITS_ONE)
serialString = ""
# serialPort.write(bytes.fromhex("A551F6"))
serialPort.write(b'AT\r\n')
time.sleep(1)
serialString = serialPort.read(100)
print(serialString)

serialPort.write(b'AT+CMGF=1\r\n')
time.sleep(1)
serialString = serialPort.read(100)
print(serialString)

print("here")

serialPort.write(b'AT+CMGS="+16509467854"\r\n')

time.sleep(1)
serialString = serialPort.read(100)
print(serialString)

serialPort.write(b'hello world\r\n')
time.sleep(1)
serialString = serialPort.read(100)
print(serialString)

while True:
    try:
        # serialPort.reset_input_buffer()
        # serialPort.reset_output_buffer()
        # serialString = serialPort.read(10).hex()
        serialString = serialPort.read()
        print(serialString)
    except KeyboardInterrupt:
        break

serialPort.close()