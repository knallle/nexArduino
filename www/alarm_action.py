from datetime import datetime
import urllib2

def minute_approx_equal(_min, _min_xx):
	min = int(_min)
	min_xx = int(_min_xx)
	tolerance = 5
#	print( abs(min-min_xx) <= tolerance ) 
	return abs(min-min_xx) <= tolerance 

def web_getter(device, state, dim_level):
	url = "http://192.168.1.249:5000/" + str(device) + "/" + str(state) + "/" + str(dim_level)	
	urllib2.urlopen(url)

time = datetime.now().time()
hour = str( time.hour )
minute = str( time.minute )


f = open('/var/www/.alarm', 'r')
line = f.readline().strip('\n')
device, state, dim_level = line.split("/")
line = f.readline().strip('\n')
hour_on, minute_on = line.split("/")
line = f.readline().strip('\n')
hour_off, minute_off = line.split("/")
f.close()

#print(hour)
#print(hour_off)
#print(minute)
#print(minute_off)

if hour == hour_on and minute_approx_equal(minute, minute_on):
	# Turn light on
	state = 1
#	print(state)
	web_getter(device, state, dim_level)
elif hour == hour_off and minute_approx_equal(minute, minute_off):
	# Turn light off
	state = 0
#	print(state)
	web_getter(device, state, dim_level)
