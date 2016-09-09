from flask import Flask, render_template
import serial
import time

app = Flask(__name__)

ser=serial.Serial('/dev/ttyACM0',9600)

@app.route('/')
def index():
	return render_template('lamps.html')

@app.route('/<device>/<state>')
def update_lamp(device, state):
	if device.isdigit() and state.isdigit():
		ser.write(device)
		ser.write(state)
#		time.sleep(1)
#		ser.write(device)
#		ser.write(state)
		time.sleep(0.3)

	return render_template('lamps.html')

if __name__ == '__main__':
	app.run(debug=True, host='0.0.0.0')
