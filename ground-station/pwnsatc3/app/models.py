from sqlalchemy.exc import SQLAlchemyError
from flask_login import UserMixin
from app import db
from datetime import datetime
        
class Configuracion(db.Model):
    __tablename__ = 'configuracion'
    id     = db.Column(db.Integer, primary_key=True)
    nombre = db.Column(db.String(100),nullable=False)
    valor  = db.Column(db.String(100),nullable=False)

    def __init__(self,nombre, valor):
        self.nombre = nombre
        self.valor  = valor

    def add(self,config):
        db.session.add(config)
        session_commit()
        return config
    
    def update(self):
        return session_commit()
    
    def delete(self,config):
        db.session.delete(config)
        return session_commit()
    
    def serialize(self):
        return {
            'config_id': self.id,
            'nombre'   : self.nombre,
            'valor'    : self.valor
        }

# ** Logs
class Logs(db.Model):
    __tablename__ = 'logs'
    id            = db.Column(db.Integer, primary_key=True)
    log_json      = db.Column(db.String(2048),nullable=True)
    date         = db.Column(db.DateTime,nullable=False)

    def __init__(self,log_json):
        self.log_json = log_json
        self.date     = datetime.now()

    def add(self,log):
        db.session.add(log)
        session_commit()
        return log
    
    def update(self):
        return session_commit()
    
    def delete(self,log):
        db.session.delete(log)
        return session_commit()
    
    def serialize(self):
        return {
            'log_id'   : self.id,
            'log_json' : self.log_json,
            'date'    : self.date
        }


class RadioInterface(db.Model):
    __tablename__ = 'radiointerface'
    id            = db.Column(db.Integer, primary_key=True)
    frequency     = db.Column(db.String(1024),nullable=True)
    bandwidth     = db.Column(db.String(1024),nullable=True)
    coding_rate   = db.Column(db.String(1024),nullable=True)

    def __init__(self, frequency, bandwidth, coding_rate):
        self.frequency   = frequency
        self.bandwidth   = bandwidth
        self.coding_rate = coding_rate
    
    def add(self,pesaje):
        db.session.add(pesaje)
        session_commit()
        return pesaje
    
    def update(self):
        return session_commit()
    
    def delete(self,pesaje):
        db.session.delete(pesaje)
        return session_commit()

    def get_socket_name(self):
        if self.socket:
            return "TCP"
        return "UDP"
    
    def serialize(self):
        return {
            "id"         : self.id,
            "frequency"  : self.frequency,
            "bandwidth"  : self.bandwidth,
            "coding_rate": self.coding_rate
        }

class EnetInterface(db.Model):
    __tablename__ = 'enetinterface'
    id            = db.Column(db.Integer, primary_key=True)
    ip            = db.Column(db.String(100),nullable=True)
    port          = db.Column(db.Integer,nullable=False)
    socket        = db.Column(db.Boolean, default=False) # False = UDP - TCP

    def __init__(self, ip, port, socket):
        self.ip     = ip
        self.port   = port
        self.socket = socket
    
    def add(self,pesaje):
        db.session.add(pesaje)
        session_commit()
        return pesaje
    
    def update(self):
        return session_commit()
    
    def delete(self,pesaje):
        db.session.delete(pesaje)
        return session_commit()

    def get_socket_name(self):
        if self.socket:
            return "TCP"
        return "UDP"
    
    def serialize(self):
        return {
            "id"    : self.id,
            "ip"    : self.ip,
            "port"  : self.port,
            "socket": self.socket
        }

class User(UserMixin, db.Model):
    __tablename__ = 'user'
    id            = db.Column(db.Integer, primary_key=True)
    password      = db.Column(db.String(1024),nullable=False)

    def __init__(self, password):
        self.password  = password
    
    def add(self,user):
        db.session.add(user)
        session_commit()
        return user
    
    def update(self):
        return session_commit()
    
    def delete(self,user):
        db.session.delete(user)
        return session_commit()
    
    def is_authenticated(self):
        return True

    def is_active(self):
        return self.activo

    def is_admin(self):
        return self.isAdmin
    
    def is_anonymous(self):
        return False
    
    def get_id(self):
        return str(self.id)

def session_commit():
    try:
        db.session.commit()
    except SQLAlchemyError as e:
        print("Session rollback: {}".format(e))
        db.session.rollback()
        reason=str(e)
