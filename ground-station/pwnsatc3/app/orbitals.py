import requests
from skyfield.api import EarthSatellite, load
from skyfield.api import wgs84
from datetime import datetime, timezone
import math

DEF_LAT = 19.4326
DEF_LON = -99.1332
DEF_ALT = 2.3

EARTH_RADIUS = 6371.0  # km

LEO_ORBIT = 9 #
LEO_COMM_WINDOW = 4
SCALE_FACTOR = 10

class Orbital:
  def __init__(self, orbit_period=LEO_ORBIT, comm_window=LEO_COMM_WINDOW, radius_km=3000):
    self.latitude = DEF_LAT
    self.longitude = DEF_LON
    self.altitude = DEF_ALT
    self.orbit_period = orbit_period
    self.comm_window = comm_window
    self.radius_km = radius_km
  
  def set_coords(self, lat, lon):
    self.latitude = lat
    self.longitude = lon

  def position_set(self):
    return self.fetch_position
  
  def get_coordinates(self):
    return (self.latitude, self.longitude, self.altitude)

  def request_coordinates(self):
    try:
      req_ip = requests.get("https://api.ipify.org?format=json")
      if req_ip.status_code != 200:
        print(f"Error status code: {req_ip.status_code}")
        return self.get_coordinates()
      
      ip_addr = req_ip.json()["ip"]
      req_coords = requests.get(f"http://ip-api.com/json/{ip_addr}")
      if req_coords.status_code != 200:
        print(f"Error status code: {req_coords.status_code}")
        return self.get_coordinates()
      coords = req_coords.json()
      
      self.latitude = coords["lat"]
      self.longitude = coords["lon"]
      return self.get_coordinates()
    except requests.exceptions.ConnectionError:
      return (DEF_LAT, DEF_LON, DEF_ALT)
  
  def haversine_distance(self, lat1, lon1, lat2, lon2):
    phi1 = math.radians(lat1)
    phi2 = math.radians(lat2)
    delta_phi = math.radians(lat2 - lat1)
    delta_lambda = math.radians(lon2 - lon1)

    a = math.sin(delta_phi / 2)**2 + math.cos(phi1) * math.cos(phi2) * math.sin(delta_lambda / 2)**2
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
    distance = EARTH_RADIUS * c

    return distance
  
  def check_current_position(self, satellite, observer, start_time):
    try:
      geo_current = satellite.at(start_time).subpoint()
      sat_lat = float(geo_current.latitude.degrees)
      sat_lon = float(geo_current.longitude.degrees)
      obs_lat = float(observer.latitude.degrees)
      obs_lon = float(observer.longitude.degrees)

      distance = self.haversine_distance(obs_lat, obs_lon, sat_lat, sat_lon)
      position_info = {
        "current_lat": sat_lat,
        "current_lon": sat_lon,
        "distance": distance,
        "within_radius": distance <= self.radius_km,
        "comm_window": None
      }

      if position_info["within_radius"]:
        half_window = (self.comm_window / 2) / 1440
        comm_start = start_time - half_window
        comm_end = start_time + half_window

        geo_start = satellite.at(comm_start).subpoint()
        geo_end = satellite.at(comm_end).subpoint()
        position_info["comm_window"] = {
          "start_time": comm_start.utc_iso(),
          "end_time": comm_end.utc_iso(),
          "start_lat": float(geo_start.latitude.degrees),
          "start_lon": float(geo_start.longitude.degrees),
          "end_lat": float(geo_end.latitude.degrees),
          "end_lon": float(geo_end.longitude.degrees),
          "distance_at_center": distance
        }
      return position_info
    except Exception:
      pass

  def calculate_orbit(self, tle_line1, tle_line2):
    lat, lon, alt = self.get_coordinates()
    ts              = load.timescale()
    satellite       = EarthSatellite(tle_line1, tle_line2, "Satellite", ts)
    now             = datetime.now(timezone.utc)
    observer        = wgs84.latlon(lat, lon, alt)
    t_now           = ts.utc(now)
    geo_now         = satellite.at(t_now).subpoint()
    sat_now_lat     = geo_now.latitude.degrees
    sat_now_lon     = geo_now.longitude.degrees

    delta_minutes   = 30
    num_points = int((6 * 60) / delta_minutes)
    times = ts.utc(now.year, now.month, now.day,
                [now.hour + (i * delta_minutes / 60) for i in range(num_points)])
  
    geocentric_positions = satellite.at(times)
    subpoints = geocentric_positions.subpoint()
    sat_latitudes = [float(lat) for lat in subpoints.latitude.degrees]
    sat_longitudes = [float(lon) for lon in subpoints.longitude.degrees]
   
    pass_info = self.check_current_position(satellite, observer, t_now)
    if pass_info["comm_window"] is not None:
      print(pass_info)
    
    message = {
      "groundtracks": {
        "sat_latitudes": sat_latitudes,
        "sat_longitudes": sat_longitudes,
      },
      "satnow": [sat_now_lat, sat_now_lon],
      "ground": [lat, lon, alt],
      "position": {
        "latitude": sat_now_lat,
        "longitude": sat_now_lon,
      },
      "next_pass": pass_info
    }
    return message

  def modify_tle_for_period(self, tle_line1, tle_line2):
    target_period_seconds = self.orbit_period * 60
    mean_motion = 86400 / target_period_seconds
    
    _ = EarthSatellite(tle_line1, tle_line2, "TEST_SAT")
    
    mean_motion_str = f"{mean_motion:011.8f}"
    new_line2 = tle_line2[:52] + mean_motion_str + tle_line2[63:]
    
    new_line2 = new_line2[:-1] + self.calculate_checksum(new_line2[:-1])
    
    return tle_line1, new_line2

  def calculate_checksum(self, line):
    total = 0
    for char in line:
        if char.isdigit():
            total += int(char)
        elif char == '-':
            total += 1
    return str(total % 10)