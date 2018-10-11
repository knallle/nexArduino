#NexArduino#

## Acknowledgements ##
* [Sebastian Nilsson][] - my initial source of inspiration. 
* [NexaCtrl][] by [calle-gunnarsson][] - the library I use for the basic control of 433 MHz devices. Originally authored by [James Taylor][], but extended with support for absolute dimming of supported 433 MHz receivers. The dimming support was the reason I chose to use an Arduino instead of only a Raspberry Pi (besides, of course, from the challenge in combining the two).

##Introduction##
This guide describes how a simple system for home automation can be constructed using:

* An Arduino UNO development board
* A Raspberry Pi computer
* A pair of 433 MHz receiver/transmitter modules
* 433 MHz remote controlled devices, for instance the [Nexa][] units.
* A [NEYCR-705][] remote controller for 433 MHz devices.
* A pair of [433 MHz transmitter/receiver modules][433_banggood] for Arduino.
* For extended range: [an antenna for 433 Mhz][antenna_banggood].

The finished system is capable of controlling 20 different devices from practically any unit with an internet connection. The system even enables scheduling.

##The general idea##
The system consists of an Arduino connected to a Raspberry Pi via USB. The Arduino handles communication with the 433 MHz transmitter. The Raspberry Pi acts as a web server and opens the system to the user.

The user sends a HTML request to the Raspberry Pi server running either Python/Flask or Ruby/Sinatra. the message is processed by the server which sends a control message to the Arduino. The Arduino receives the message and transmitts the appropriate control sequence over 433 MHz.

## Arduino side ##
In order to communicate over 433 MHz, the Arduino needs to be extended with a 433 MHz transmitter module and a 433 MHz receiver module. The receiver module is used only for identification of the ID number of the remote controller. 
### Connecting the 433 MHz modules ###
Both the receiving and the transmitting modules run on 5VDC, så connect the _Vcc_ and _GND_ pins to _5V_ and _GND_ respectively on the Arduino. Then connect the _DATA_ pin on the receiver to _Pin8_ on the Arduino and connect the _DATA_ pin on the transmitter to _Pin13_ on the Arduino. The connections can be made with a [bread board](), with [jumper cables]() or by soldering.

### Sniffing the remote controller ###
Each remote controller uses its own unique[^1] ID number to identify itself to the receiveing units. In order to control the units, our system has to mimic the remote control that is controlling the units. That is, it has to know the ID of the remote controller. The ID is found by running the _sniffing_ script __sniffing.ino__ which can be found on [GitHub](). The script listens after signals sent from a 433 MHz remote controller and prints the ID of the remote.

### Listening to serial communication and sending control signals ###
Now to the main script which runs constantly on the Arduino and listens for incoming commands via the Serial port. The complete code can be accessed on [GitHub]() and is piecewise described below.

	#include "NexaCtrl.h"
		
	#define TX_PIN 13
	#define RX_PIN 8
		
	NexaCtrl nexaCtrl(TX_PIN, RX_PIN);

We start by including the external file _NexaCtrl.h_ which handles the low level 433 MHz communication and then define the pins used for receiving/transmitting signals with the 433 MHz units. Then we create a NexaCtrl object which will handle the communication.

	// Define constants
	const static unsigned long controller_id = 11222333;
	const uint8_t default_value = 99;
	const uint8_t num_of_input_characters = 3;
	
Next, we define three constants to use in the code. Note that _controller\_id_ is the id of the controller which we identified by sniffing. 


	// Array for incoming numbers: [device_id, mode, dim_level]
	int c_incoming_num[num_of_input_characters];
	
	// Initialize variables used in loop
	unsigned int device, dim_level, index;
	bool sequence_read;
	char received;
	String inData;
	
	//ASCII-codes
	const char ascii_eol = 0x25;        // '%'
	const char ascii_delimiter = 0x2F;  // '/'
	const char ascii_zero = 0x30;       // '0'
	const char ascii_nine = 0x39;       // '9'

When data is sent over the Serial port, it is built up as: _'device\_id/state/dim\_level%'_, that is; three integers spearated with the delimiter '_/_' and ended by the EOL charachter '_%_'. Since tha data is received as the data type _char_ we define the ASCII values for the delimiter and the EOL so that we can later compare the incoming data to them. We also create a array in which the incoming data will be saved.

	void setup() {
	    Serial.begin(9600);
	    index = 0;
	    sequence_read = false;
	    for(int i=0; i<num_of_input_characters; i++) {
	        c_incoming_num[i] = default_value;
	    }
	}
	
The setup-loop sets up a Serial connection with baudrate 9600. When using a Serial monitor to communicate with the Arduino, the baudrate must be set to 9600. Also, all values in the array for the incoming values are set to the _default\_value_.
	
	void loop() {
	    /*
	    * Listen for incoming data
	    */
	    if (Serial.available() > 0) {
	        
	        /*
	        * Read data over Serial
	        */
	        while (Serial.available() > 0) {
	            received = Serial.read(); // One charachter at a time
	  
We start the main loop by checking if there is incoming data to read. If so, we read the data piece by piece until there is no more data to read.
	            
	            if ( received != ascii_eol ) { // EOL not reached
	                if ( received == ascii_delimiter ) { // Got delimiter between input data
	                    c_incoming_num[index] = inData.toInt();
	                    inData = "";
	                    index++;
	                } else {
	                    if ( received >= ascii_zero && received <= ascii_nine ) {    // Accept only integers
	                        inData += received;
	                    }
	                }
	            } else { // EOL reached
	                if ( inData != "" ) {
	                    c_incoming_num[index] = inData.toInt();
	                    inData = "";
	                }
	                index = 0;
	                sequence_read = true;
	                break;
	            }
	        }
	                
If the read char is neither the EOL charachter nor the delimiter, the read char is appended to the string which stores the indata. If the read char is the delimiter or EOL, the indata string is typecasted to an integer and added to the data array. When EOL is reached, the flag _sequence\_read_ is set to true, which indicates that a full set of data has been received.
	
	        /*
	        * Send control command over 433 MHz
	        */
	        if (sequence_read) {
	            device = c_incoming_num[0]-1;   // Nexa device 1 uses device_id 0 etc.
	            dim_level = c_incoming_num[2];

To send the signal, the integers belonging to the device and dim_level are extracted from the array to make the code easier to understand.
     
	            if (dim_level <= 15) { // A dim level was specified. Dim level in [0..15]
	                nexaCtrl.DeviceDim(controller_id, device, dim_level);
	            } else {
	                switch (c_incoming_num[1]) {  // Check specified mode
	                    case 0:
	                        nexaCtrl.DeviceOff(controller_id, device);
	                        break;
	                    case 1:
	                        nexaCtrl.DeviceOn(controller_id, device);
	                        break;
	                    default:
	                        break;
	                }
	            }
	            
Depending on the value of _dim\_level_, the NexaCtrl object is used to dim the device or turn the device on/off_. For Nexa devices, the maximum dim level is 15, wherefore a dim_level above 15 means the device is dimmable.
	            
	            // Reset and get ready for next transfer
	            for(int i=0; i<num_of_input_characters; i++) {c_incoming_num[i] = 99;}
	            sequence_read = false;
	        }
	    }
	}
	
Finally, variables are reset to default values and the program is ready for the next Serial command.

## Raspberry Pi side ##
### Using Python or Ruby? ###
### Setting up a Flask/Sinatra server ###
Setting up a Sinatra server is done with the following code:

	# myapp.rb
	require 'sinatra'
	require 'serialport'
	
	set :port, 5000
	ser = SerialPort.new("/dev/ttyACM0", 9600)
	
	get '/' do
	  'Hello world!'
	end
	
	get '/:device/:mode/:dim_level' do
	  "Device: #{params[:device]}, Mode: #{params[:mode]}, Dim level #{params[:dim_level]}"
	  ["#{params[:device]}", "/", "#{params[:mode]}", "/", "#{params[:dim_level]}", "%"].each do |m|
	    ser.write m
	    sleep 0.01  
	  end
	end
	
### Serial communication to Arduino ###

## User interface suggestions ##
### Using any web browser ###
### iOS: using Workflow.app ###
### macOS: using Keyboard Maestro ###
### Scheduling with cron ###
### Using something else ###

Links
[Sebastian Nilsson]: http://sebastiannilsson.com/en/projekt/arduino/433-mhz-rf-nexa-saendare-och-mottagare-med-arduino/ "Sebastian's website."
[NexaCtrl]: https://github.com/calle-gunnarsson/NexaCtrl "NexaCtrl on GitHub."
[calle-gunnarsson]: https://github.com/calle-gunnarsson "calle-gunnarsson on GitHub."
[James Taylor]: http://jtlog.wordpress.com/ "James Taylor's website."
[Nexa]: https://www.nexa.se "Nexa's website."
[NEYCR-705]: https://www.nexa.se/vara-produkter/system-nexa/sandare/lyct-705 "NEYCR-705 product page."
[this Instructable]: http://www.instructables.com/id/RF-315433-MHz-Transmitter-receiver-Module-and-Ardu/step2/schematics/ "Schematics for connecting the 433 MHz modules."
[433_banggood]: http://www.banggood.com/433Mhz-RF-Transmitter-With-Receiver-Kit-For-Arduino-ARM-MCU-Wireless-p-74102.html?rmmds=search "433 MHz transmitter/receiver modules on Banggood."
[antenna_banggood]: http://www.banggood.com/10-Pcs-433MHZ-Spiral-Spring-Helical-Antenna-5MM-p-932781.html?rmmds=search "433 MHz antenna on Banggood."
[bread board]: https://www.google.se/search?client=safari&rls=en&q=bread+board&ie=UTF-8&oe=UTF-8&gfe_rd=cr&ei=UIN3WKPjK8Sq8wfk4ZWIBA
[jumper cables]: https://www.google.se/search?q=jumper+cables+arduino&client=safari&rls=en&biw=960&bih=835&source=lnms&tbm=isch&sa=X&ved=0ahUKEwiGndCi2rzRAhVMWCwKHazFAQEQ_AUIBigB
[my GitHub]: https://github.com/knallle/nexIno

Footnotes
[^1]: The ID is not entirely unique, but the number of different IDs is so great that the probability of two remotes located close to each other geographically to have the same IDs is practically zero.