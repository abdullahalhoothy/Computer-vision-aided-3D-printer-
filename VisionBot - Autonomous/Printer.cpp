#include "Printer.h"


Printer::Printer(char *PortName)
{

	open_serial(PortName, h1);

	// should wait briefly to complete the send and/or
	Sleep(100); // need to wait for data to come

	while (serial_available(h1) > 0) {
		serial_recv_char(ch, h1);
		cout << ch;
	}

	cout << "\ninitial 3D printer data transmit is complete\n";
	isReady = true;
	Sleep(1000);
}

void Printer::setHome()
{
	
	// declare an output string stream of max NMAX charaters
	ostrstream sout(buffer, 64);

	// declare an input string stream of max SMAX charaters
	istrstream sin(s1, 1000);

	// start home ///////////////////

	sout << "G28";
	sout << "\n";
	/*sout << "G1 Z0";
	sout << "\n";*/

	sout << '\0'; // terminate the string so strlen can function


	n = strlen(buffer); // number of bytes to send (excludes \0)

	// for debugging
	cout << "\nn = " << n;
	cout << "\nbuffer = " << buffer;

	// send the command(s) to the 3D printer
	serial_send(buffer, n, h1);

	Sleep(10);

	cout << "\nincoming G-code commands:\n\n";
	xP = 0.0;
	yP = 0.0;
	zP = 0.0;

	while (serial_available(h1) > 0) {
		serial_recv_char(ch, h1);
		cout << ch;
		Sleep(1); // wait for more characters
	}
	//close_serial(h1);
}

void Printer::move(int x, int y, int z)
{

	// declare an output string stream of max NMAX charaters
	ostrstream sout(buffer, 64);

	// declare an input string stream of max SMAX charaters
	istrstream sin(s1, 1000);

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
	serial_send(buffer, n, h1);
	Sleep(100);
	// might have to be tuned in order to get all characters in following message for s1

	cout << "\nincoming G-code commands:\n\n";

	// determine the number of bytes available to read
	// in order to prevent blocking in the following 
	// receive function
	n = serial_available(h1);

	// take all 3D printer output and put it in string s1
	serial_recv(s1, n, h1);
	s1[n] = '\0'; // add termination character to string
	xP = x;
	yP = y;
	zP = z;

	// reset sin to the beginning of the string s1 for next time
	sin.seekg(sin.beg);

	//close_serial(h1);
}

void Printer::currentPosition(string &Position)
{
	// declare an output string stream of max NMAX charaters
	ostrstream sout(buffer, 64);

	// declare an input string stream of max SMAX charaters
	istrstream sin(s1, 1000);

	// set sout position to beginning so we can use it again
	sout.seekp(0);

	sout << "M114"; // move without extrusion
	sout << "\n";

	sout << '\0'; // terminate the string so strlen can function
	// note the C++ compiler seems to hate "\0"

	n = strlen(buffer); // number of bytes to send (excludes \0)

	// for debugging
	cout << "\nn = " << n;
	cout << "\nbuffer = " << buffer;

	// send the move command to the 3D printer
	serial_send(buffer, n, h1);
	Sleep(100);

	// receive function
	n = serial_available(h1);

	// take all 3D printer output and put it in string s1
	serial_recv(s1, n, h1);
	s1[n] = '\0'; // add termination character to string

	Position = s1;
	// reset sin to the beginning of the string s1 for next time
	sin.seekg(sin.beg);


}
Printer::~Printer()
{
	close_serial(h1);
}
