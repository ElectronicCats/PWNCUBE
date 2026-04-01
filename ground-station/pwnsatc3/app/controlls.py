import socket, hashlib, base64, json
from passlib.hash import sha256_crypt

class Utilidades:
    def __init__(self):
        pass

    def getHostname(self):
        hostname = socket.gethostname()
        return hostname

    def getIpAddress(self):
        hostname   = self.getHostname()
        ip_address = socket.gethostbyname(hostname)
        return ip_address

    def setupDomainname(self, domain):
        return domain
    
    def encryptPassword(self, password):
        return sha256_crypt.encrypt(password)

    def checkPassword(self, password, password_hash):
        return sha256_crypt.verify(password, password_hash)
    
    def generateHashMD5(self, core):
        return hashlib.md5(core.encode()).hexdigest()

    def convertToBase64(self, core):
        return base64.b64encode(core.encode()).decode()
    
    def convertFromBase64(self, core):
        return base64.b64decode(core.encode()).decode()
    
    def commandParser(self, filepath, type="TELEMETRY"):
        with open(filepath, "r") as f:
            lines = f.readlines()
        
        result = {type: {}}
        current_name = None
        for line in lines:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            if line.startswith("TELEMETRY"):
                parts = line.split()
                _, connector, name, endian, description = parts[0], parts[1], parts[2], parts[3], " ". join(parts[4:])
                current_name = name
                result["TELEMETRY"][name] = {
                    "connector": connector,
                    "endian": endian,
                    "description": description.strip('"'),
                    "fields": []
                }
            elif line.startswith("COMMAND"):
                parts = line.split()
                _, connector, name, endian, description = parts[0], parts[1], parts[2], parts[3], " ".join(parts[4:])
                current_name = name
                result["COMMANDS"][name] = {
                    "name": name,
                    "connector": connector,
                    "endian": endian,
                    "description": description.strip('"'),
                    "parameters": [],
                    "response": None
                }
            elif line.startswith("APPEND_ITEM") or line.startswith("APPEND_ID_ITEM"):
                parts = line.split()
                _, field_name, bit_size, ftype, *rest = parts
                description = rest[-1].strip('"')
                entry = {
                    "name": field_name,
                    "bits": int(bit_size),
                    "type": ftype,
                    "description": description.strip('"')
                }
                result["TELEMETRY"][current_name]["fields"].append(entry)
            elif line.startswith("ID_PARAMETER"):
                parts = line.split()
                ftype, name, _, _, intype, min, max, value, description = parts[0], parts[1], parts[2], parts[3], parts[4], parts[5], parts[6], parts[7], " ".join(parts[8:])
                entry = {
                    "type": ftype,
                    "name": name,
                    "intype": intype,
                    "min": min,
                    "max": max,
                    "value": value,
                    "description": description.strip('"'),
                    "raw": " ".join(parts[1:])}
                result["COMMANDS"][current_name]["parameters"].append(entry)
            elif line.startswith("APPEND_PARAMETER"):
                parts = line.split()
                print(parts)
                ftype, name, size, inttype, min, value, max, description = parts[0], parts[1], parts[2], parts[3], parts[4], parts[5], parts[6], " ".join(parts[7:])
                if inttype == "STRING":
                    ftype, name, size, inttype, value, description = parts[0], parts[1], parts[2], parts[3], parts[4], " ".join(parts[5:])
                entry = {
                    "type": ftype,
                    "name": name,
                    "size": size,
                    "inttype": inttype,
                    "value": value,
                    "description": description.strip('"'),
                    "raw": " ".join(parts[1:])}
                result["COMMANDS"][current_name]["parameters"].append(entry)
            elif line.startswith("RESPONSE"):
                parts = line.split()
                _, connector, response = parts
                result["COMMANDS"][current_name]["response"] = response
        return result
