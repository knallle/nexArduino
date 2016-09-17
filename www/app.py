from flask import Flask, render_template
import serial
import time

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
	

@app.route('/')
def index():
	return render_template('lamps.html')

@app.route('/<device>/<state>/<dim_level>')
def update_lamp(device, state, dim_level):
	if device.isdigit() and state.isdigit() and dim_level.isdigit():
		delimiter = ','
		msg = str(device) + delimiter + str(state) + delimiter + str(dim_level)
		
		ser.write(msg)
		time.sleep(0.3)

	return render_template('lamps.html')

@app.route('/<device>/<state>/<dim_level>/<schedule_h_on>/<schedule_m_on>/<schedule_h_off>/<schedule_m_off>')
def schedule_lamp(device, state, dim_level, schedule_h_on, schedule_m_on, schedule_h_off, schedule_m_off):
	s_h_on = int(schedule_h_on)
	s_m_on = int(schedule_h_on)
	s_h_off = int(schedule_h_off)
	s_m_off = int(schedule_h_off)
	
	if s_h_on>=0 and s_m_on>=0 and s_h_off>=0 and s_m_off>=0:
		command = "/".join( (device, state, dim_level) ) + "\n"
		f = open('.alarm', 'w')
		f.write(command)
		f.close

		if s_h_on <= 24 and s_m_on <= 60:
			# Set timer to start lamp
			command = "/".join( (schedule_h_on, schedule_m_on) ) + "\n"
			f = open('.alarm', 'a')
			f.write(command)
			f.close
		else:
			write_dummy_alarm()	

		if s_h_off <= 24 and s_m_off <= 60:
			# Set timer to shut down lamp
			command = "/".join( (schedule_h_off, schedule_m_off) ) + "\n"
			f = open('.alarm', 'a')
			f.write(command)
			f.close
		else:
			write_dummy_alarm()	

	return render_template('lamps.html')	

if __name__ == '__main__':
	app.run(debug=True, host='0.0.0.0')
