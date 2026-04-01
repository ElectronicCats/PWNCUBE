import socketio
from socketio.exceptions import BadNamespaceError
import threading
import socket
import os
import time

SERVERPATH         = os.getcwd()
class SocketConnector:
  # Use server_url=pwnsatc3 with docker
  def __init__(self, server_url=f"http://127.0.0.1:80", sock_ip="127.0.0.1", sock_port=5005, sender_port=5006):
    self.sio         = socketio.Client(reconnection_attempts=100, reconnection_delay=50, reconnection_delay_max=10)
    self.sock        = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self.sock_sender = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self.server_url = server_url
    self.sock_ip     = sock_ip
    self.sock_port   = int(sock_port)
    self.sender_port = int(sender_port)
    self.recv_buffer = bytearray()

    self.sock.setblocking(False)
    self.sock.settimeout(0.5)
    
    self.sio.on("connect", self.on_connect)
    self.sio.on("disconnect", self.on_disconnect)
    self.sio.on("send_tc_connector", self.on_send_tc)

    self.file_write = None
    self.file_name_set = False
    self.packet_count = 0
    self.image_save_path = ""
    self.image_name = ""
    self.tlm_map = {
      "PING"  : "send_ping",
      "STATUS": "get_status",
      "GET_TEMP": "get_temp",
      "GET_GYRO": "get_gyro",
      "GET_TM": "get_tm",
      "SEND_MESSAGE": "send",
      "ERROR" : 0x4320
    }

  def send_connector_data(self, cmd, data=None):
    if data is not None:
      ser_cmd = f"{self.tlm_map[cmd]} {data}\r\n"
    else:
      ser_cmd = f"{self.tlm_map[cmd]}\r\n"
    print(f"Sending command: {ser_cmd}")
    self.sock_sender.sendto(ser_cmd.encode(), (self.sock_ip, self.sender_port))


  def extract_packets(self, marker_start: bytes, marker_end: bytes):
    while True:
      start_idx = self.recv_buffer.find(marker_start)
      end_idx = self.recv_buffer.find(marker_end)
      if start_idx != -1 and end_idx != -1 and end_idx > start_idx:
          packet = self.recv_buffer[start_idx + len(marker_start):end_idx]
          self.recv_buffer = self.recv_buffer[end_idx + len(marker_end):]
          yield packet
      else:
        break

  def handle_s_packet(self, packet: bytes):
    if len(packet) < 1:
      return
    segment = packet[0]
    if segment == 0:
      if not self.file_name_set:
        self.file_write = open("bin.bin", "wb")
      if self.file_write:
        self.file_write.write(packet[3:])
    elif segment == 1:
      filename = packet[3:-1].decode(errors="ignore")
      self.file_write = open(filename, "wb")
      self.file_name_set = True
      self.sio.emit("telecommand", data={"room": "telecommand", "telecommand": "START"})
    elif segment == 2:
      if self.file_write:
        self.file_write.close()
        self.file_write = None
        self.file_name_set = False
        self.sio.emit("telecommand", data={"room": "telecommand", "telecommand": "DONE"})

  def handle_i_packet(self, packet: bytes):
    if len(packet) < 1:
        return
    segment = packet[0]
    print(f"\n[{self.packet_count}] Packet - Segment: {segment}===============================")

    if segment == 0:
      if not self.file_name_set:
        self.file_write = open("output.webp", "wb")
      if self.file_write:
        self.file_write.write(packet[1:])
        print(packet[1:])
        self.packet_count += 1

    elif segment == 1:
      self.image_name = packet[1:-1].decode(errors="ignore")
      self.image_save_path = os.path.join(SERVERPATH, "app", "static", "uploads", self.image_name)
      self.file_write = open(self.image_save_path, "wb")
      self.file_name_set = True
      self.sio.emit("image", data={"room": "image", "status": "START"})

    elif segment == 2:  # End
      print(packet)
      if self.file_write:
        self.file_write.close()
        self.file_write = None
        self.file_name_set = False
        self.packet_count = 0
        self.sio.emit("image", data={"room": "image", "status": "DONE", "image": 
                                     os.path.join("static", "uploads", self.image_name)})
  

  def handle_t_packet(self, packet: bytes):
    """Telemetry T@ type"""
    self.sio.emit("telemetry", data={"room": "telemetry", "telemetry": packet})

  def on_send_tc(self, data):
    self.send_connector_data(data["name"], data["param"])

  def on_connect(self):
    print("Connected to server")
    self.sio.emit("join", {"room": "telemetry"})
    
  def on_disconnect(self):
    print("Disconnected from server")
  
  def send_gs_tc_response(self, name, data):
    pass

  def recv_tak(self):
    while True:
      try:
        data, _ = self.sock.recvfrom(1024)
        if b"[SYS]" in data:
          return
        apid = data.split(b"@;")[0].replace(b"\r\n", b"")
        data = data.split(b"@;")[1].replace(b"\r\n", b"")
        print(f"[{apid}] Data>{data}")
        if int(apid) == 201:
            self.send_gs_tc_response("PING", f"PING:{data.decode()}".encode())
        elif int(apid) == 450:
            self.send_gs_tc_response("STATUS", f"STATUS:{data.decode()}".encode())
        elif int(apid) == 451:  
            self.send_gs_tc_response("TEMP", data)
        elif int(apid) == 452:
            self.send_gs_tc_response("GYRO", data)
        elif int(apid) == 453 or int(apid) == 100:
            self.send_gs_tc_response("TM", data)
            self.sio.emit("telemetry", data={"room": "telemetry", "telemetry": data})
        elif int(apid) == 100:
            print(data)
            self.sio.emit("telemetry", data={"room": "telemetry", "telemetry": data})
        elif int(apid) == 1023:
            self.send_gs_tc_response("IDLE", data)
      except KeyboardInterrupt:
        break
      except IndexError:
        continue
      except BadNamespaceError:
        continue
      except TimeoutError:
        continue
  
  def run(self):
    self.sock.bind((self.sock_ip, self.sock_port))
    
    th_recv = threading.Thread(target=self.recv_tak, daemon=True)
    th_recv.start()
    
    self.sio.connect(self.server_url)
    while True:
      time.sleep(0.1)
    
    

if __name__ == '__main__':
  connector = SocketConnector()
  connector.run()