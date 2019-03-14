
// this loop back example is based on serial example #2

#include <Arduino.h>

void setup()
{
	// *make sure to change this to the same as the windows program
	// -- in this case it has been set to the 3D printer baud rate
	// 115200 bps
 	Serial.begin(115200);
	
	// send some initial message in order to mimic 
	// the initial message send by the 3D printer
	Serial.print("3D printer is waiting for G-code commands\n");
}


void loop()
{
    const int NMAX = 64;
    static char buffer[NMAX];
    int i = 0, n, len;
    char ch;

    // wait until new data is available
    while( Serial.available() == 0 ) i++;

    // read the incoming characters and send them back imediately	
    len = Serial.available();
    n = Serial.readBytes(buffer,len);
	
	// check for time-out in case the message is interrupted
	// ie due to a communication fault / error
    if( n < len ) {
        Serial.print("error: not enough data, n = ");
        Serial.println(n);
    }

	for(i=0;i<n;i++) Serial.print(buffer[i]);
	// don't print a '\n' since the G code message
	// should include these
 
}
