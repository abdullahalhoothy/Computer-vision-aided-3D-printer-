

#include <cstdio>
#include <cstdlib>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>  
#include "serial_com.h"
#include <windows.h>
using namespace std;

// include this header file for basic image transfer functions
#include "image_transfer3.h"

// include this header file for computer vision functions
#include "vision.h"

//class of 3D-Printer
#include "Printer.h"

//My-function 
#include "obj.h"

//global variables
HANDLE h2;
char buffer[64];
char s1[1000];
char ch2;
int n, s_index = 0;


#define KEY(c) ( GetAsyncKeyState((int)(c)) & (SHORT)0x8000 )

//function prototypes
void applyfilters(image &rgb, image &a, image &label);
void closeGripper(HANDLE &h2);
void openGripper(HANDLE &h2);

int main(int argc, char* argv[])
{
	int j;
	double hist[255], hmin, hmax, x, ic, jc;
	FILE *fp;
	image a, rgb, rgb0 ,rgb00; // declare some image structures
	image label;
	int cam_number, height, width;

	char ch;

	// set camera number (normally 0 or 1)
	cam_number = 1;
	width = 640;
	height = 480;

	//declaire instance of 3D-printer
	Printer printer("COM5");
	Sleep(1500);





	activate_camera(cam_number, height, width);	// activate camera


	printf("\nset home...");
	//getch();

	//set to home location
	printer.setHome();
	Sleep(100);

	//move printer to cover bed
	printf("\nmove printer to cover bed...");

	//getch();
	printer.move(50, 110, 200);
	Sleep(100);


	



	rgb.type = RGB_IMAGE;
	rgb.width = width;
	rgb.height = height;

	rgb0.type = RGB_IMAGE;
	rgb0.width = width;
	rgb0.height = height;

	rgb00.type = RGB_IMAGE;
	rgb00.width = width;
	rgb00.height = height;

	// set the type and size of the images
	a.type = GREY_IMAGE;
	a.width = width;
	a.height = height;

	label.type = LABEL_IMAGE;
	label.width = width;
	label.height = height;

	// allocate memory for the images
	allocate_image(a);

	allocate_image(label);
	allocate_image(rgb);
	allocate_image(rgb0);
	allocate_image(rgb00);

	printf("\npress x to get an image");


	Sleep(100);

	//Get first image
	while (1)
	{
	acquire_image(rgb, cam_number);

	if (kbhit()) {
	ch = getch();
	if (ch == 'x') break;
	}

	view_rgb_image(rgb);
	save_rgb_image("shot1.bmp", rgb);
	}

	// copy the original rgb image to use later
	copy(rgb, rgb0);

	printf("\ninput image rgb");
	//getch();

	//filters for binary image
	applyfilters(rgb, a, label);

	//Callibration value in two elevation  //h1=200  h2=100
	float gratio200 = 0.0;
	float gratio100 = 0.0;


	printf("\ncentroid of labeled image");
	//getch();

	// compute the centroid of the label image
	centroid(a, label, 1, ic, jc);

	//draw centroid in binary image
	draw_point_RGB(rgb, (int)ic, (int)jc, 255, 0, 0);

	cout << "\nic = " << ic << "\njc = " << jc;
	view_rgb_image(rgb);
	Sleep(200);

	printf("\ncalculate HSV");
	//getch();

	//HSV image values
	double H = 0, S = 0, V = 0;

	//Calculate Hue Saturate and value of HSV image to detect Color
	calculate_HSV(H, S, V, rgb0, ic, jc);
	cout << "\nH = " << H << "\nS = " << S << "\nV = " << V;

	printf("\ncolor Detection");
	//getch();
	Sleep(200);
	string colorName = "";

	//Color detection function
	checkColor(H, colorName);
	cout << "\nColor is " << colorName << "...";
	//getch();
	Sleep(1000);

	printf("\nRGBcentroid");
	//getch();

	//calculate centroid im RGB image
	RGBcentroid(rgb0, ic, jc, colorName);

	//Draw centroid of RGB image
	draw_point_RGB(rgb0, (int)ic, (int)jc, 0, 0, 255);
	cout << "\nic = " << ic << "\njc = " << jc;
	Sleep(200);
	view_rgb_image(rgb0);


	//Read callibration ratio from txt files
	ifstream file("GeometricRatio-200.txt");
	string temp;
	getline(file, temp);
	gratio200 = strtof((temp).c_str(), 0);
	cout << fixed;
	cout << "\nGeometricRatio-200 = " << setprecision(5) << gratio200;
	cout << "\nSet the Centroid of object...";

	ifstream file2("GeometricRatio-100.txt");
	string temp2;
	getline(file2, temp2);
	gratio100 = strtof((temp2).c_str(), 0);
	cout << fixed;
	cout << "\nGeometricRatio-100 = " << setprecision(5) << gratio100;
	cout << "\nSet the Centroid of object...";

	Sleep(200);

	//Calculate real distance of Head to object
	double Xdistance = ic - (640 / 2);
	Xdistance = Xdistance / gratio200;

	double Ydistance = jc - (480 / 2);
	Ydistance = Ydistance / gratio200;

	cout << "\nXdistance = " << Xdistance;
	cout << "\nYdistance = " << Ydistance;
	Sleep(200);


	//move printer to top of object
	cout << "\nMove Printer...";
	printer.move(Xdistance + printer.xP, Ydistance + printer.yP, 100);
	gratio100 = gratio200 * 2;
	Sleep(15000);



	//re-calculate centroid of RGB image

	//Get second image of the object
	acquire_image(rgb, cam_number);
	save_rgb_image("shot2.bmp", rgb);
	Sleep(200);


	copy(rgb, rgb0);

	//Calculate centroid of RGB image again
	printf("\nRGBcentroid");
	RGBcentroid(rgb, ic, jc, colorName);

	//Draw centroid and center of screen to visual evaluation
	draw_point_RGB(rgb, (int)ic, (int)jc, 0, 0, 255);
	draw_point_RGB(rgb, (640 / 2), (480 / 2), 120, 120, 120);
	cout << "\nic = " << ic << "\njc = " << jc;
	view_rgb_image(rgb);
	Sleep(200);

	//Calculate real distance of centroid to head and move printer
	cout << "\nSet the Centroid of object...";
	Xdistance = ic - (640 / 2);
	Xdistance = Xdistance / gratio100;


	Ydistance = jc - (480 / 2);
	Ydistance = Ydistance / gratio100;

	cout << "\nXdistance = " << Xdistance;
	cout << "\nYdistance = " << Ydistance;
	Sleep(200);
	cout << "\nMove Printer...";
	printer.move(Xdistance + printer.xP, Ydistance + printer.yP, printer.zP);
	Sleep(1000);

	
	//copy RGB image
	copy(rgb0, rgb00);

	double PerimeterDistance[18][2];

	//Shape detection function
	Shape_object_theta_RGB(ic, jc, colorName, rgb00);

	//Edge detection function and draw points on perimeter
	perimeter(rgb0, ic, jc, colorName, gratio100, PerimeterDistance);
	draw_point_RGB(rgb0, (int)ic, (int)jc, 0, 0, 255);
	view_rgb_image(rgb0);

	Sleep(200);

	//touch the centroid
	printf("\ntouch the centroid...");

	//Offset of pen with the camera x=15 y=2
	printer.move( - 15 + printer.xP, printer.yP - 2, 30);

	//put location of centroid for future steps
	double Xcentroid = printer.xP;
	double Ycentroid = printer.yP;

	Sleep(10000);

	//move up
	printf("\nMove up...");
	printer.move( printer.xP,  printer.yP, 40);
	Sleep(1000);

	//touch touch perimeter...
	printf("\ntouch perimeter...");
	printer.move(PerimeterDistance[0][0] + Xcentroid, PerimeterDistance[0][1] + Ycentroid, 30);
	
	for (int i = 1; i < 18; i++)
	{
		printer.move(PerimeterDistance[i][0] + Xcentroid, PerimeterDistance[i][1] + Ycentroid, 30);
		Sleep(50);

	}



	//gripper steps
	Sleep(100);
	open_serial("COM3", h2);
	Sleep(100); // need to wait for data to come

	while (serial_available(h2) > 0) {
		serial_recv_char(ch2, h2);
		cout << ch2;
	}

	Sleep(1000);

	//gripper open
	cout << "\nopen gripper...";
	 openGripper(h2); //open gripper function
	

    // move head to catch the object by the gripper

	printf("\nMove gripper...");
	printer.move(printer.xP, printer.yP, 50);
	Sleep(100);
	printer.move(Xcentroid + 39.5, Ycentroid + 42, 50);
	printer.move(Xcentroid + 39.5, Ycentroid + 42, 30);
	Sleep(10000);

	
    //close gripper
	cout << "\nclose gripper...";
	 closeGripper(h2); //close gripper function



	//move head with catched object

	 printf("\nMove gripper...");
	printer.move(Xcentroid + 39, Ycentroid + 42, 50);
	printer.setHome();
	Sleep(16000);

	
	//open gripper to release object
	cout << "\nopen gripper...";
	 openGripper(h2);
	printf("\nFinish...");
	getch();



	// free the image memory before the program completes
	free_image(a);
	free_image(label);
	free_image(rgb);
	free_image(rgb0);

	deactivate_cameras();

	printf("\n\ndone.\n");
	getch();

	return 0;
}


//filters of binary image
void applyfilters(image &rgb, image &a, image &label)
{
	int nlabels;
	int height, width, nhist;
	width = 640;
	height = 480;
	double hist[255], hmin, hmax;
	image  b;
	b.type = GREY_IMAGE;
	b.width = width;
	b.height = height;
	allocate_image(b);
	// convert RGB image to a greyscale image
	copy(rgb, a);

	copy(a, rgb);    // convert to RGB image format
	view_rgb_image(rgb);
	printf("\ninput image converted to greyscale");

	Sleep(100);

	// scale the image to enhance contrast
	scale(a, b);

	copy(b, a); // put result back into image a
	Sleep(100);

	copy(a, rgb);    // convert to RGB image format
	view_rgb_image(rgb);
	printf("\nimage scale function is applied");
	//getch();
	Sleep(100);

	copy(a, rgb);    // convert to RGB image format
	view_rgb_image(rgb);
	printf("\nimage after filter function is applied");
	//getch();
	Sleep(100);
	// make a histogram
	nhist = 60; // make 60 bins -- each bin is 255/60 range of intensity
	// eg bin1 = 0-3 
	// bin2 = 4-8,
	// etc.
	histogram(a, hist, nhist, hmin, hmax);


	// use threshold function to make a binary image (0,255)
	threshold_new(a, b, 150, 60);
	copy(b, a);

	copy(a, rgb);    // convert to RGB image format
	view_rgb_image(rgb);
	printf("\nimage after threshold function is applied");

	Sleep(100);
	// invert the image
	invert(a, b);
	copy(b, a);

	copy(a, rgb);    // convert to RGB image format
	view_rgb_image(rgb);
	printf("\nimage after invert function is applied");
	//getch();
	Sleep(100);
	// perform an erosion function to remove noise (small objects)
	erode(a, b);
	copy(b, a);

	copy(a, rgb);    // convert to RGB image format
	view_rgb_image(rgb);
	printf("\nimage after erosion function is applied");
	//getch();
	Sleep(100);
	// perform a dialation function to fill in 
	// and grow the objects
	dialate(a, b);
	copy(b, a);

	dialate(a, b);
	copy(b, a);

	copy(a, rgb);    // convert to RGB image format
	view_rgb_image(rgb);

	// label the objects in a binary image
	// labels go from 1 to nlabels
	label_image(a, label, nlabels);
}


void openGripper(HANDLE &h2)
{
	
	// declare an output string stream of max NMAX charaters
	ostrstream sout(buffer, 64);

	// declare an input string stream of max SMAX charaters
	istrstream sin(s1, 1000);

	// set sout position to beginning so we can use it again
	sout.seekp(0);

	sout << "1"; // move without extrusion


	sout << '\0'; // terminate the string so strlen can function
	// note the C++ compiler seems to hate "\0"

	n = strlen(buffer); // number of bytes to send (excludes \0)

	// for debugging
	cout << "\nn = " << n;
	cout << "\nbuffer = " << buffer;

	// send the opencommand to the gripper
	serial_send(buffer, n, h2);
	Sleep(100);
}

void closeGripper(HANDLE &h2)
{
	char buffer[64];
	char s1[1000];
	// declare an output string stream of max NMAX charaters
	ostrstream sout2(buffer, 64);

	// declare an input string stream of max SMAX charaters
	istrstream sin2(s1, 1000);

	// set sout position to beginning so we can use it again
	sout2.seekp(0);

	sout2 << "2"; // move without extrusion


	sout2 << '\0'; // terminate the string so strlen can function
	// note the C++ compiler seems to hate "\0"

	n = strlen(buffer); // number of bytes to send (excludes \0)

	// for debugging
	cout << "\nn = " << n;
	cout << "\nbuffer = " << buffer;

	// send the close command to the gripper
	serial_send(buffer, n, h2);
	Sleep(100);
}