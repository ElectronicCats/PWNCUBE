from flask import Flask,url_for, redirect
from flask_socketio import SocketIO, emit, leave_room
from flask_sqlalchemy import SQLAlchemy
from flask_migrate import Migrate
from flask_login import LoginManager
from flask_cors import CORS
import os
import struct
import random
import datetime
import time
import numpy as np

from .orbitals import Orbital

migrate       = Migrate()
app           = Flask(__name__)
CORS(app, origins=["*", "http://10.190.0.125:80", "http://192.168.0.*:80", "http://localhost:80"])
login_manager = LoginManager()
# Constants
SERVERPATH         = os.getcwd()
ALLOWED_EXTENSIONS = set([ 'png', 'jpg', 'jpeg', 'gif','GIF','PNG','JPG','JPEG','csv'])
DB_HOST     = os.environ.get("DB_HOST", "127.0.0.1")
DB_USER     = os.environ.get("DB_USER", "pwnsat")
DB_PASSWORD = os.environ.get("DB_PASSWORD", "passwordpwnsat")
DB_NAME     = os.environ.get("DB_NAME", "pwnsat")

# APP CONFIG
app.config["SECRET_KEY"]                     = "n18o4XvcWy"
app.config["SQLALCHEMY_DATABASE_URI"]        = f"postgresql+psycopg2://{DB_USER}:{DB_PASSWORD}@{DB_HOST}:5432/{DB_NAME}"
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False
app.config['SOCK_SERVER_OPTIONS']            = {'ping_interval': 25}
app.config['ALLOWED_EXTENSIONS']             = ALLOWED_EXTENSIONS
app.config["SERVERPATH"]                     = SERVERPATH
app.config["STATIC"]                         = os.path.join(SERVERPATH, "static")
app.config["DOCSPATH"]                       = os.path.join(SERVERPATH, "documents")
app.config["UPLOAD"]                     = os.path.join(SERVERPATH, "uploads")

db       = SQLAlchemy(app)
socketio = SocketIO(app, cors_allowed_origins="*")

migrate.init_app(app, db)
login_manager.init_app(app)

orbital = Orbital()

class EulerControl:
    def __init__(self, new_time):
        self.tm = new_time
        self.alpha = 0.98
        self.roll  = 0.0
        self.pitch = 0.0
        self.yaw   = 0.0
    def get_time(self):
        return self.tm
    def set_time(self, new_time):
        self.tm = new_time


euler_control = EulerControl(time.time())

@login_manager.user_loader
def load_user(user_id):
    from .models import User
    return User.query.filter(User.id==user_id).first()

@login_manager.unauthorized_handler
def unauthorized():
    return redirect(url_for('auth.login'))

#Blueprints
@socketio.on('connect')
def on_connect():
    print("New client")

@socketio.on('disconnect')
def on_disconnect():
    emit('disconnect', broadcast=True)

@socketio.on('test')
def on_test(data):
    temperature = random.randrange(0, 30)
    pressure    = random.randrange(0, 30)
    altitude    = random.randrange(0, 30)
    humidity    = random.randrange(0, 30)
    payload = {
        "timestamp": datetime.datetime.now().strftime("%D/%mm/%Y %H:%M:%S"),
        "temperature": "%.2f" % temperature,
        "pressure"   : "%.2f" % pressure,
        "altitude"   : "%.2f" % altitude,
        "humidity"   : "%.2f" % humidity,
        "type": "weather"
    }
    emit('test', payload, broadcast=True)

@socketio.on('telemetry')
def on_telemetry(tm):
    telemetry = tm["telemetry"]
    (bmestatus, temperature, pressure, altitude, humidity, mpustatus, accex, accey, accez, gyrox, gyroy, gyroz, temp) = struct.unpack_from(">BffffBfffffff", telemetry[1:])
    current_time = time.time()
    dt = current_time - euler_control.get_time()
    euler_control.set_time(current_time)
    
    accel_roll = np.arctan2(accey, accez) * 180 / np.pi
    accel_pitch = np.arctan2(-accex, np.sqrt(accey**2 + accez**2)) * 180 / np.pi
    
    euler_control.roll = euler_control.alpha * (euler_control.roll + gyrox * dt) + (1 - euler_control.alpha) * accel_roll
    euler_control.pitch = euler_control.alpha * (euler_control.pitch + gyroy * dt) + (1 - euler_control.alpha) * accel_pitch
    
    euler_control.yaw += gyroz * dt * 180 / np.pi
    payload = {
        "timestamp": datetime.datetime.now().strftime("%D %H:%M:%S"),
        "bme_status"  : bmestatus,
        "temperature": "%.2f" % temperature,
        "pressure"   : "%.2f" % pressure,
        "altitude"   : "%.2f" % altitude,
        "humidity"   : "%.2f" % humidity,
        "accex"      : "%.4f" % accex,
        "accey"      : "%.4f" % accey,
        "accez"      : "%.4f" % accez,
        "gyrox"      : "%.4f" % gyrox,
        "gyroy"      : "%.4f" % gyroy,
        "gyroz"      : "%.4f" % gyroz,
        "mputemp"    : "%.4f" % temp,
        "mpu_status"  : mpustatus,
        "roll"       : float(euler_control.roll),
        "pitch"      : float(euler_control.pitch),
        "yaw"        : float(euler_control.yaw),
        "type": "sensors"
    }
    emit('tm', payload, broadcast=True)

def orbit_task():
    while True:
        TLE_LINE1 = "1 57179U 23091P   25104.15576030  .00008714  00000-0  51017-3 0  9990"
        TLE_LINE2 = "2 57179  97.5685 159.1045 0017189 162.4262 197.7566 15.11942696 98861"

        # Modify TLE
        new_tle_line1, new_tle_line2 = orbital.modify_tle_for_period(TLE_LINE1, TLE_LINE2)
        message = orbital.calculate_orbit(new_tle_line1, new_tle_line2)
        socketio.emit("orbit", message)
        time.sleep(1)

@socketio.on("orbit")
def on_message_orbit(message):
    TLE_LINE1 = "1 57179U 23091P   25104.15576030  .00008714  00000-0  51017-3 0  9990"
    TLE_LINE2 = "2 57179  97.5685 159.1045 0017189 162.4262 197.7566 15.11942696 98861"

    # Modify TLE
    new_tle_line1, new_tle_line2 = orbital.modify_tle_for_period(TLE_LINE1, TLE_LINE2)
    message = orbital.calculate_orbit(new_tle_line1, new_tle_line2)
    emit("orbit", message, broadcast=True)

@socketio.on('telecommand')
def on_telecommand(tc):
    print(f"TC: {tc}")
    emit('tc', tc, broadcast=True)

@socketio.on('image')
def on_telecommand(img):
    print(f"TC: {img}")
    emit('img', img, broadcast=True)

@socketio.on('send_tc')
def on_send_tc(data):
    print(f"TC: {data}")
    emit('send_tc_connector', data, broadcast=True)

@socketio.on('leave')
def on_leave(data):
    room = data['room']
    leave_room(room)
    emit('status', {'message': f'Leve the room {room}.'}, room=room)

from .api import api as api_blueprint
from .auth import auth as auth_blueprint

app.register_blueprint(api_blueprint)
app.register_blueprint(auth_blueprint)
