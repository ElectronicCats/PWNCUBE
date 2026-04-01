import json
from flask import render_template, request, url_for, flash, redirect, Blueprint, g, session, abort, make_response, jsonify
from functools import wraps
from sqlalchemy import  desc,or_,and_,func,event,extract,literal_column,case,Integer,distinct,cast,not_,asc
from werkzeug.security import generate_password_hash, check_password_hash
from flask_login import login_user, logout_user, login_required, current_user
from .controlls import Utilidades
from .models import Logs, Configuracion, User
import subprocess
import os

from app import app

SERVERPATH         = os.getcwd()
utilidades = Utilidades()

auth = Blueprint("auth", __name__)

#404 page
@auth.errorhandler(404)
def page_not_found(e):
    return render_template("404.html"), 404

@auth.route("/", methods=["GET"])
@login_required
def dashboard():
    context = {}
    return render_template("dashboard.html", context=context)


@auth.route("/login", methods=["GET", "POST"])
def login():
    if request.method == "POST":
        password = request.form["password"]

        user = User.query.first()
        if user is None:
            user = User(utilidades.encryptPassword("toor"))
            user.add(user)
        if not utilidades.checkPassword(password, user.password):
            context_log = {
                "type"    : "error",
                "message" : "Password Incorrect"
            }
            generar_log = Logs(json.dumps(context_log))
            generar_log.add(generar_log)

            flash("Password Incorrect", "danger")
            return redirect(url_for("auth.login"))

        login_user(user, remember=False)
        return redirect(url_for("auth.dashboard"))
    return render_template("login.html")

@auth.route("/logout", methods=["GET"])
@login_required
def logout():
    logout_user()
    return redirect(url_for("auth.login"))

@auth.route("/command", methods=["GET"])
@login_required
def command_view():
    # telemetry_cmd = utilidades.commandParser(os.path.join(SERVERPATH, "pwnsatc3/app/static/assets/tlm.txt"))
    # cmd_cmd = utilidades.commandParser(os.path.join(SERVERPATH, "pwnsatc3/app/static/assets/cmds.txt"), "COMMANDS")
    telemetry_cmd = utilidades.commandParser(os.path.join(SERVERPATH, "app/static/assets/tlm.txt"))
    cmd_cmd = utilidades.commandParser(os.path.join(SERVERPATH, "app/static/assets/cmds.txt"), "COMMANDS")
    print(cmd_cmd)
    context = {
        "tc": cmd_cmd,
        "tm": telemetry_cmd
    }
    return render_template("commands.html", context=context)

@auth.route("/create_app", methods=["GET"])
def usuarios_add():

    # Create the migration
    subprocess.run(["flask", "db", "init"])
    subprocess.run(["flask", "db", "migrate"])
    subprocess.run(["flask", "db", "upgrade"])

    new_user = User(utilidades.encryptPassword("toor"))
    new_user.add(new_user)
    
    return redirect(url_for("auth.login"))