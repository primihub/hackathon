import json
import sys

s = sys.stdin.read()
d = json.loads(s)
lat = int(d["latitude"] * 1e5)
lng = int(d["longitude"] * 1e5)
#lat = str(d["latitude"]).split('.')
#lng = str(d["longitude"]).split('.')
#print(lat, lng)
#lat = float(lat[0]) + float('.' + lat[1]) / 60.
#lng = float(lng[0]) + float('.' + lng[1]) / 60.
print(f"-x {lat} -y {lng}")
