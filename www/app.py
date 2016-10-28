from flask import Flask, render_template
import serial
import time
import json

app = Flask(__name__)

try:
	ser=serial.Serial('/dev/ttyACM0',9600)
except serial.serialutil.SerialException:
	try:
		ser=serial.Serial('/dev/ttyACM1',9600)
	except serial.serialutil.SerialException:
		print("Could not connect to Arduino on ACM0 or ACM1")

def write_dummy_alarm():
	command = "25/61\n"
	f = open('.alarm', 'a')
	f.write(command)
	f.close
	
def set_alarm(device, state, dim_level, min_on, hour_on, min_off, hour_off):
	f = open('/var/www/alarm_json','r')
	json_str = f.read()
	f.close()
	units = json.loads(json_str)
	units[device]['min_on'] = min_on	
	units[device]['hour_on'] = hour_on	
	units[device]['min_off'] = min_off	
	units[device]['hour_off'] = hour_off	
	units[device]['state'] = state
	units[device]['dim_level'] = dim_level
	f = open('/var/www/alarm_json','w')
	f.write( json.dumps(units) )
	f.close()

@app.route('/')
def index():
	return render_template('lamps_empty.html')

@app.route('/<device>/<state>/<dim_level>')
def update_lamp(device, state, dim_level):
	if device.isdigit() and state.isdigit() and dim_level.isdigit():
		
		delimiter = "/"
		eol = "%"

		msg = str(device) + delimiter + str(state) + delimiter + str(dim_level) + eol

		for m in msg:
			ser.write(m)
			time.sleep(0.005)
		
	return render_template('lamps_empty.html')

@app.route('/<device>/<state>/<dim_level>/<schedule_h_on>/<schedule_m_on>/<schedule_h_off>/<schedule_m_off>')
def schedule_lamp(device, state, dim_level, schedule_h_on, schedule_m_on, schedule_h_off, schedule_m_off):
	hour_on = int(schedule_h_on)
	min_on = int(schedule_m_on)
	hour_off = int(schedule_h_off)
	min_off = int(schedule_m_off)

	set_alarm(device, state, dim_level, min_on, hour_on, min_off, hour_off)

	return render_template('lamps_empty.html')	

if __name__ == '__main__':
	app.run(debug=True, host='0.0.0.0', threaded=True)
