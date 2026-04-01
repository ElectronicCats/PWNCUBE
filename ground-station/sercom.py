import time
import serial
import socket
import os
import argparse
import threading
class SerialConnector:
  def __init__(self, serial_path="/dev/tty.usbmodem212201", connector_ip="0.0.0.0", connector_port=5005, sercom_port=5006):
    self.sock           = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self.sock_recv      = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self.ser            = serial.Serial(serial_path, 115200)
    self.connector_ip   = connector_ip
    self.connector_port = connector_port
    self.sercom_port = sercom_port
    self.th_recv_task = threading.Thread()

  def recv_task(self):
    while True:
      data, addrs = self.sock_recv.recvfrom(1024)
      print(f"[{addrs} {data}]")
      self.ser.write(data)
      time.sleep(0.05)
  
  def sender_task(self):
    while True:
      try:
        if self.ser.in_waiting > 0:
          data = self.ser.read(self.ser.in_waiting)
          print(f"[SER] {data}")
          self.sock.sendto(data, (self.connector_ip, self.connector_port))

        time.sleep(0.05)
      except KeyboardInterrupt:
        self.ser.close()
        self.sock.close()
        self.sock_recv.close()
        break

  def run(self):
    self.sock_recv.bind(("0.0.0.0", self.sercom_port))
    self.th_recv_task = threading.Thread(target=self.recv_task, daemon=True)
    self.th_recv_task.start()
    self.sender_task()
    

if __name__ == '__main__':
  parser = argparse.ArgumentParser("SerialInterace")
  parser.add_argument("-p", "-port", help="Serial port", default="/dev/tty.usbmodem212201")
  args = parser.parse_args()
  try:
    connector = SerialConnector(serial_path=args.p)
    connector.run()
  except serial.serialutil.SerialException:
    print("[ERROR] Connect the serial device or check the port.")