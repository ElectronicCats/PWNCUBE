import threading
from app import app, socketio, orbit_task

if __name__ == "__main__":
    try:
        threading.Thread(target=orbit_task, daemon=True).start()
        socketio.run(app,debug=True, host="0.0.0.0", port=80, allow_unsafe_werkzeug=True)
    except OSError as err:
        print("OS error: {0}".format(err))
        app.run(debug=True, host="0.0.0.0", port=5555)