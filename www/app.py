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

if __name__ == '__main__':
	app.run(debug=True, host='0.0.0.0')
