from flask import Blueprint, jsonify, request
from sqlalchemy import  or_,func
from functools import wraps
from .controlls import Utilidades
from app import app

utilidades = Utilidades()


api = Blueprint("api", __name__)