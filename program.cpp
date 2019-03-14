
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <strstream> // for string streams

#include <conio.h>

#include <windows.h>

#include "serial_com.h"

// include this header file for basic image transfer functions
#include "image_transfer2.h"

#include "timer.h"

using namespace std;

#define KEY(c) ( GetAsyncKeyState((int)(c)) & (SHORT)0x8000 )

int main()
{  
	HANDLE h1;
	const int NMAX = 64, SMAX = 1000;
	char buffer[NMAX];
	char s1[SMAX];
	char example_data[] = "X:120.00 Y:0.00 Z:0.00 E:0.00 Count X:12000 Y:0 Z:0\nok";
	char ch;
	int n,s_index=0;
	int x=100,y=100,z=10;
	double xr = 0.0;

	// declare a string stream to convert a mixture
	// of numbers and text to a string to send to the Arduino
	// or 3D printer

	// declare an output string stream of max NMAX charaters
	ostrstream sout(buffer,NMAX);

	// declare an input string stream of max SMAX charaters
	istrstream sin(s1,SMAX);

	// note: the serial port driver should be set to the same data (baud) rate
	// in the device manager com port settings.
	// the uploader should also use the same rate.

	// make sure bps setting in serial_com.cpp file
	// is compatible with the printer -- normally 115200 bps

	// should have the same com port as Arduino here
	open_serial("\\\\.\\COM4",h1); // COM9 for my 3D printer

	// should wait briefly to complete the send and/or
	// to receive data from Arduino
	Sleep(100); // need to wait for data to come
	// back from arduino or the next loop will fall through

	// print out what the Arduino sends back
	// this is like a mini serial monitor program
	while( serial_available(h1) > 0 ) {
		serial_recv_char(ch,h1);
		cout << ch;
	}

	cout << "\ninitial 3D printer data transmit is complete\n";

	Sleep(1000);

	// start home ///////////////////

	// we should home printer before sending move commands
	// -- otherwise it will not know where it is to start.
	sout << "G28";
	sout << "\n";

	sout << '\0'; // terminate the string so strlen can function
	// note the C++ compiler seems to hate "\0"

	n = strlen(buffer); // number of bytes to send (excludes \0)

	// for debugging
	cout << "\nn = " << n;
	cout << "\nbuffer = " << buffer;

	// send the command(s) to the 3D printer / Arduino
	serial_send(buffer,n,h1);

	Sleep(10);

	cout << "\nincoming G-code commands:\n\n";

	// print out what the Arduino sends back
	// this is like a mini serial monitor program
	while( serial_available(h1) > 0 ) {
		serial_recv_char(ch,h1);
		cout << ch;
		Sleep(1); // wait for more characters
	}

	// end home ////////////////

	while(1) {	

		if( KEY('X') ) break; // press x to exit loop

		if( KEY(VK_UP) || KEY(VK_DOWN) ) {

			if( KEY(VK_UP) ) x += 5;
			if( KEY(VK_DOWN) ) x -= 5;

			// set sout position to beginning so we can use it again
			sout.seekp(0);

			sout << "G0"; // move without extrusion
			sout << " X" << x;
			sout << " Y" << y;
			sout << " Z" << z;
			sout << "\n";

			sout << '\0'; // terminate the string so strlen can function
			// note the C++ compiler seems to hate "\0"

			n = strlen(buffer); // number of bytes to send (excludes \0)

			// for debugging
			cout << "\nn = " << n;
			cout << "\nbuffer = " << buffer;
	
			// send the move command to the 3D printer
			serial_send(buffer,n,h1);

			// wait 100 ms for a message to be sent back
			// why wait -> give some time for the move to finish
			// also time for the anet arduino board to process
			// the command --> ie we don't want to fill up
			// its incoming message mailbox
			// note the approximate time to move = dist / speed
			// -> maybe use vision system to see when it stops
			// or gets there
			// -- note there is also a G-code command
			// which sends back the current position
			Sleep(100); 
			// might have to be tuned in order to get all characters in following message for s1

			cout << "\nincoming G-code commands:\n\n";

			// determine the number of bytes available to read
			// in order to prevent blocking in the following 
			// receive function
			n = serial_available(h1);
			
			// take all 3D printer output and put it in string s1
			serial_recv(s1,n,h1);
			s1[n] = '\0'; // add termination character to string

			// this next part with example_data should be removed
			// later

			// put some example data into s1 -- remove this for 3D printer
			n = strlen(example_data); 
			strcpy(s1,example_data); // copy example data into s1

			// output received data for testing / debugging purposes
			cout << s1;

			// convert string to numbers using sin
			sin >> ch;
			sin >> ch;
			sin >> xr;
			cout << "\nxr = " << xr << "\n";

			// if the 3d printer is within 5 mm
			// of desired position then stop
			// and move on to the next task.
			if( abs(x-xr) < 5 ) break;


			// reset sin to the beginning of the string s1 for next time
			sin.seekg(sin.beg);

		} // end if

	} // end while(1)	

	cout << "\npress a key to complete.";
	getch();

	cout << "\ndone.\n\n";	
	
	close_serial(h1);

	// note when the program ends the 3D printer resets

	return 0; 
}

