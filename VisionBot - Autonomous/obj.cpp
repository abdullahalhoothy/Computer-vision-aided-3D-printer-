#include <cstdio>
#include <cstdlib>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <math.h>
// include this header file for basic image transfer functions
#include "image_transfer3.h"

// include this header file for computer vision functions
#include "vision.h"
//#include "obj.h"

using namespace std;

float GeometricRatio(image a, image label, float distance)
{
	double ic1, ic2, jc1, jc2;
	centroid(a, label, 1, ic1, jc1);
	centroid(a, label, 2, ic2, jc2);
	draw_point(a, (int)ic1, (int)jc1, 100);
	draw_point(a, (int)ic2, (int)jc2, 100);

	double Di = abs(ic2 - ic1);
	double Dj = abs(jc2 - jc1);
	double PixelDistance = sqrt((Di*Di) + (Dj*Dj));
	float ratio = PixelDistance / distance;




	ofstream myfile;
	myfile.open("GeometricRatio-100.txt");
	myfile << ratio << "\n";
	myfile.close();



	return ratio;
}


void calculate_HSVaverage(double &Hav, double &Sav, double &Vav, image &a, image &label, int nlabel, int ic, int jc)
{
	int max, min, n = 0;
	double Rn, Gn, Bn; // normalized R, G, B values
	double d, H;
	double hue = 0.0, sat = 0.0, value = 0.0;
	int R = 0, G = 0, B = 0;

	ibyte *pa;
	i2byte *pl;
	i4byte width, height;

	pa = a.pdata;
	pl = (i2byte *)label.pdata;

	int i, j, w = 2;
	// number of pixels
	width = a.width;
	height = a.height;


	for (i = -w; i <= w; i++) {
		for (j = -w; j <= w; j++) {
			if (pl[width*(jc + j) + (ic + i)] == nlabel) {
				n++;

				B = pa[width*(jc + j) + (ic + i)];
				G = pa[width*(jc + j) + (ic + i) + 1];
				R = pa[width*(jc + j) + (ic + i) + 2];



				max = min = R;

				if (G > max) max = G;
				if (B > max) max = B;

				if (G < min) min = G;
				if (B < min) min = B;

				value = max / 255.0; // normalize value so it varies from 0 to 1

				// note we need 255.0 to get floating point division (otherwise we get 0 with
				// integer division)

				if (max != 0) { // check for divide by zero in sat calculation
					sat = (max - min) / (double)max;  // varies from 0 to 1
					// use cast for denominator max to force floating point division -- same as above
				}
				else { // max == 0
					sat = 0; // no saturation if pixel is black 
				}

				if (max != min) { // check for / 0 for d

					d = max - min;
					Rn = (max - R) / d;
					Gn = (max - G) / d;
					Bn = (max - B) / d;

				}
				else { // d = 0 -> divide by zero for the above

					// if max == min the pixel is grey
					// -> set hue to some arbitrary negative value to indicate grey
					// ie no colour
					hue = 0.0;

					return; // don't calculate hue below
				}

				// calculate hue based on normalized R, G, B values

				if (R == max && G == min) {
					H = 5 + Bn;
				}
				else if (R == max && G != min) {
					H = 1 - Gn;
				}
				else if (G == max && B == min) {
					H = Rn + 1;
				}
				else if (G == max && B != min) {
					H = 3 - Bn;
				}
				else if (B == max && R == min) {
					H = 3 + Gn;
				}
				else {
					H = 5 - Rn;
				}

				hue = H * 60; // convert Hue to degrees (0 to 360)
				//cout << "\nH = " << hue;

				Hav += hue;
				Sav += sat;
				Vav += value;
			}
		}
	}
	Hav = Hav / n;
	Sav = Sav / n;
	Vav = Vav / n;

}


void HSVtoRGB(int H, double S, double V, int &R, int &G, int &B) {

	double C = S * V;

	double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));

	double m = V - C;

	double Rs, Gs, Bs;



	if (H >= 0 && H < 60) {

		Rs = C;

		Gs = X;

		Bs = 0;

	}

	else if (H >= 60 && H < 120) {

		Rs = X;

		Gs = C;

		Bs = 0;

	}

	else if (H >= 120 && H < 180) {

		Rs = 0;

		Gs = C;

		Bs = X;

	}

	else if (H >= 180 && H < 240) {

		Rs = 0;

		Gs = X;

		Bs = C;

	}

	else if (H >= 240 && H < 300) {

		Rs = X;

		Gs = 0;

		Bs = C;

	}

	else {

		Rs = C;

		Gs = 0;

		Bs = X;

	}



	R = (Rs + m) * 255;

	G = (Gs + m) * 255;

	B = (Bs + m) * 255;

}

int RGBcentroid(image &rgb, double &ic, double &jc, string colorName){

	int q = 0;
	int k = 0;
	int n = 0;
	int Si = 0;
	int Sj = 0;
	ibyte r, g, b;
	ic = 0;
	jc = 0;
	// number of pixels in image
	int size = rgb.height * rgb.width * 3;
	for (k = 0; k < size; k += 3) {
		b = rgb.pdata[k];
		g = rgb.pdata[k + 1];
		r = rgb.pdata[k + 2];

		if (colorName == "red")
		{
			int difference = r - b;
			int orangeness = r - g;
			if (r > 90 && orangeness < 70 && difference > 25 && b < 110)  //orange detection 
			{
				n++;
				int i = q % 640;
				Si += i;
				Sj += (q - i) / 640;
			}
		}
		else if (colorName == "green")
		{
			int difference = g - r;
			int greenness = g - b;
			if (r < 65 && greenness > 30 && difference > 25 && g > 40 && b < 100)
			{
				n++;
				int i = q % 640;
				Si += i;
				Sj += (q - i) / 640;
			}

			//Cyan color
			if (r < 100 && greenness < 35 && difference > 45 && g > 50 && b < 140)
			{
				n++;
				int i = q % 640;
				Si += i;
				Sj += (q - i) / 640;
			}
		}
		else if (colorName == "blue")
		{
			int difference = g - r;
			int blueness = b - g;
			if (r < 5 && blueness < 40 && g < 5 && b >10)
			{
				n++;
				int i = q % 640;
				Si += i;
				Sj += (q - i) / 640;
			}
		}

		q++;
	}

	if (n > 0)
	{
		ic = Si / n;
		jc = Sj / n;
	}
	return 0;
}


void calculate_HSV(double &hue, double &sat, double &value, image &a, int ic, int jc)
{
	int R, G, B;
	int max, min;
	double Rn, Gn, Bn; // normalized R, G, B values
	double d, H;
	ibyte *pa;
	i4byte width, height;

	pa = a.pdata;
	width = a.width;
	height = a.height;
	if (ic < 0) ic = 0;
	if (jc < 0)jc = 0;

	B = pa[3 * (width*(jc)+(ic))];
	G = pa[3 * (width*(jc)+(ic)) + 1];
	R = pa[3 * (width*(jc)+(ic)) + 2];

	max = min = R;

	if (G > max) max = G;
	if (B > max) max = B;

	if (G < min) min = G;
	if (B < min) min = B;

	value = max / 255.0; // normalize value so it varies from 0 to 1

	// note we need 255.0 to get floating point division (otherwise we get 0 with
	// integer division)

	if (max != 0) { // check for divide by zero in sat calculation
		sat = (max - min) / (double)max;  // varies from 0 to 1
		// use cast for denominator max to force floating point division -- same as above
	}
	else { // max == 0
		sat = 0; // no saturation if pixel is black 
	}

	if (max != min) { // check for / 0 for d

		d = max - min;
		Rn = (max - R) / d;
		Gn = (max - G) / d;
		Bn = (max - B) / d;

	}
	else { // d = 0 -> divide by zero for the above

		// if max == min the pixel is grey
		// -> set hue to some arbitrary negative value to indicate grey
		// ie no colour
		hue = 0.0;

		return; // don't calculate hue below
	}

	// calculate hue based on normalized R, G, B values

	if (R == max && G == min) {
		H = 5 + Bn;
	}
	else if (R == max && G != min) {
		H = 1 - Gn;
	}
	else if (G == max && B == min) {
		H = Rn + 1;
	}
	else if (G == max && B != min) {
		H = 3 - Bn;
	}
	else if (B == max && R == min) {
		H = 3 + Gn;
	}
	else {
		H = 5 - Rn;
	}

	hue = H * 60; // convert Hue to degrees (0 to 360)

}

int threshold_new(image &a, image &b, int maxtvalue, int mintvalue)
// binary threshold operation
// a - greyscale image
// b - binary image
// tvalue - threshold value
{
	i4byte size, i;
	ibyte *pa, *pb;

	// initialize pointers
	pa = a.pdata;
	pb = b.pdata;

	// check for compatibility of a, b
	if (a.height != b.height || a.width != b.width) {
		printf("\nerror in threshold: sizes of a, b are not the same!");
		return 1;
	}

	if (a.type != GREY_IMAGE || b.type != GREY_IMAGE) {
		printf("\nerror in threshold: input types are not valid!");
		return 1;
	}

	// number of bytes
	size = (i4byte)a.width * a.height;

	// threshold operation
	for (i = 0; i < size; i++) {
		if (pa[i] < maxtvalue && pa[i]> mintvalue) pb[i] = 0;
		else pb[i] = 255;
	}

	return 0;
}

int draw_point_RGB(image &rgb, int ip, int jp, int R, int G, int B)
{
	ibyte *p;
	int i, j, w = 2, pixel;

	// initialize pointer
	p = rgb.pdata;

	if (rgb.type != RGB_IMAGE) {
		printf("\nerror in draw_point_RGB: input type not valid!");
		return 1;
	}

	// limit out of range values
	if (ip < w) ip = w;
	if (ip > rgb.width - w - 1) ip = rgb.width - w - 1;
	if (jp < w) jp = w;
	if (jp > rgb.height - w - 1) jp = rgb.height - w - 1;

	for (i = -w; i <= w; i++) {
		for (j = -w; j <= w; j++) {
			pixel = rgb.width*(jp + j) + (ip + i);
			p[3 * pixel] = B;
			p[3 * pixel + 1] = G;
			p[3 * pixel + 2] = R;
		}
	}

	return 0;
}



void checkColor(double Hue, string &colorName)
{
	if (Hue <= 60 && Hue >= 0)
	{
		//cout << "\nColor is red ...";
		colorName = "red";
		//getch();
	}
	else if (Hue <= 120 && Hue >= 60)
	{
		//cout << "\nColor is yellow ...";
		colorName = "green";
		//getch();
	}
	else if (Hue <= 180 && Hue >= 120)
	{
		//cout << "\nColor is green ...";
		colorName = "green";
		//getch();
	}
	else if (Hue <= 240 && Hue >= 181)
	{
		//cout << "\nColor is cyan ...";
		colorName = "blue";
		//getch();
	}
	else if (Hue <= 300 && Hue >= 241)
	{
		//cout << "\nColor is blue ...";
		colorName = "blue";
		//getch();
	}
	else if (Hue <= 360 && Hue >= 301)
	{
		//cout << "\nColor is magenta ...";
		colorName = "red";
		//getch();
	}
}

double object_radiusRGB5points(double ic, double jc, double beta, string colorName, image &rgb)
{
	double r = 0.0, dr, r_max, rTemp;
	int i, j, k, width, height, q, pn = 0;
	ibyte *pa;

	width = rgb.width;
	height = rgb.height;
	pa = rgb.pdata;

	dr = 0.5; // use 0.5 pixels just to be sure

	r_max = 180; // limit the max object radius size to something reasonable

	for (r = dr; r < r_max; r += dr) {
		i = ic + r*cos(beta);
		j = jc + r*sin(beta);

		//cout << "\n I = " << i << "\nJ = " << j;


		// limit i and j in case it gets out of bounds -> wild pointer
		if (i < 0) i = 0;
		if (i >= width) i = width;
		if (j < 0) j = 0;
		if (j >= height) j = height;

		// convert i,j to image coord k

		int B = pa[3 * (width*(int)(j)+(int)(i))];
		int G = pa[3 * (width*(int)(j)+(int)(i)) + 1];
		int R = pa[3 * (width*(int)(j)+(int)(i)) + 2];

		if (colorName == "red")
		{
			int difference = R - B;
			int orangeness = R - G;
			if (R > 90 && orangeness < 70 && difference > 25 && B < 110)
			{
				rTemp = r;
				pn = 0;
			}
			else
			{
				if (pn > 5)
				{
					r = rTemp;
					break;
				}
				else
				{
					pn++;
				}
			}
		}
		else if (colorName == "green")
		{
			int difference = G - R;
			int greenness = G - B;
			//R < 45 && greenness > 35 && difference > 25 && G > 50 && B < 100
			if (R < 65 && greenness > 30 && difference > 25 && G > 40 && B < 100)
			{
				rTemp = r;
				pn = 0;
			}
			else if (R < 100 && greenness < 35 && difference > 45 && G > 50 && B < 140) //Cyan color
			{
				rTemp = r;
				pn = 0;
			}
			else
			{
				if (pn > 5)
				{
					r = rTemp;
					break;
				}
				else
				{
					pn++;
				}
			}
		}
		else if (colorName == "blue")
		{
			int difference = G - R;
			int blueness = B - G;
			if (R < 5 && blueness < 45 && G < 5 && B >10)
			{
				rTemp = r;
				pn = 0;
			}
			else
			{
				if (pn > 5)
				{
					r = rTemp;
					break;
				}
				else
				{
					pn++;
				}
			}
		}



		//if (colorName != Color2) break;
	}



	// mark point in rgb image for testing / debugging
	draw_point_RGB(rgb, i, j, 0, 255, 0);

	return r - 15;
}

int RGBcentroid_limitedArea(image &rgb, double &ic, double &jc, string colorName){

	int w, h;
	int k = 0;
	int n = 0;
	int Si = 0;
	int Sj = 0;
	ibyte r, g, b;
	ic = 640 / 2;
	jc = 480 / 2;
	w = 150;
	h = 100;


	for (int j = -h; j <= h; j++) {
		for (int i = -w; i <= w; i++) {
			k = rgb.width*(jc + j) + (ic + i);
			k = 3 * k;

			b = rgb.pdata[k];
			g = rgb.pdata[k + 1];
			r = rgb.pdata[k + 2];
			if (colorName == "red")
			{
				if (r > 100 && g < 100 && b < 100)
				{
					n++;
					Si += ic + i;
					Sj += jc + j;
				}
			}
			else if (colorName == "green")
			{
				if (r < 100 && g > 100 && b < 100)
				{
					n++;
					Si += ic + i;
					Sj += jc + j;
				}
			}
			else if (colorName == "blue")
			{
				if (r < 100 && g < 100 && b > 100)
				{
					n++;
					Si += ic + i;
					Sj += jc + j;
				}
			}
		}
	}
	if (n > 0)
	{
		ic = Si / n;
		jc = Sj / n;
	}
	return 0;
}

void perimeter(image &rgb, double &ic, double &jc, string colorName, float GeometricRatio, double PerimeterDistance[36][2])
{
	double X0distance = 0.0;
	double Y0distance = 0.0;
	double X1distance = 0.0;
	double Y1distance = 0.0;
	//double r1 = object_radius(ic, jc, 0, 1, label, rgb0);
	double r0 = object_radiusRGB5points(ic, jc, 0, colorName, rgb);
	//cout << "\nradius 1=" << r1 / GeometricRatio;

	// x and y direction actual distance from centroid

	PerimeterDistance[0][0] = r0*cos(3.14159 * 0 / 180) / GeometricRatio;  // r1 in this case since angle is zero
	PerimeterDistance[0][1] = r0*sin(3.14159 * 0 / 180) / GeometricRatio;  // 0 in this case since angle is zero
	X0distance = PerimeterDistance[0][0];
	Y0distance = PerimeterDistance[0][1];
	int n = 0;
	for (int i = 10; i < 351; i += 20)
	{
		n++;
		double r1 = object_radiusRGB5points(ic, jc, 3.14159 * i / 180, colorName, rgb);
		X1distance = r1*cos(3.14159 * i / 180) / GeometricRatio;
		Y1distance = r1*sin(3.14159 * i / 180) / GeometricRatio;

		PerimeterDistance[n][0] = X1distance;
		PerimeterDistance[n][1] = Y1distance;


	}

}

double Shape_object_theta_RGB(double ic, double jc, string colorName, image &rgb)
{
	double th, dth, th_max = 0.0, r_max = 0.0, r;
	int n = 45, i, j, shape_detected = 0;;
	double point_counter = 0, r_45, r_90, r_135, r_180, r_225, r_270, r_315, r_120, r_240;
	dth = 3.14159 / n;

	for (th = 0.0; th < 2 * 3.141591; th += dth) {

		r = object_radiusRGB5points(ic, jc, th, colorName, rgb);
		point_counter += 1;

		if (r > r_max) {
			r_max = r;
			th_max = th;
		}
	}

	//printf("\nRadius at beta: r_max = %lf ", r_max);
	r_45 = object_radiusRGB5points(ic, jc, th_max + 0.78535, colorName, rgb);
	//printf("\nRadius at beta: r_45 = %lf ", r_45);
	r_90 = object_radiusRGB5points(ic, jc, th_max + 1.5707, colorName, rgb);
	//printf("\nRadius at beta: r_90 = %lf ", r_90);
	r_120 = object_radiusRGB5points(ic, jc, th_max + 2.0942, colorName, rgb);
	//printf("\nRadius at beta: r_120 = %lf ", r_120);
	r_135 = object_radiusRGB5points(ic, jc, th_max + 2.35605, colorName, rgb);
	//printf("\nRadius at beta: r_135 = %lf ", r_135);
	r_180 = object_radiusRGB5points(ic, jc, th_max + 3.1414, colorName, rgb);
	//printf("\nRadius at beta: r_180 = %lf ", r_180);
	r_225 = object_radiusRGB5points(ic, jc, th_max + 3.92675, colorName, rgb);
	//printf("\nRadius at beta: r_225 = %lf ", r_225);
	r_240 = object_radiusRGB5points(ic, jc, th_max + 4.18853, colorName, rgb);
	//printf("\nRadius at beta: r_240 = %lf ", r_240);
	r_270 = object_radiusRGB5points(ic, jc, th_max + 4.7121, colorName, rgb);
	//printf("\nRadius at beta: r_270 = %lf ", r_270);
	r_315 = object_radiusRGB5points(ic, jc, th_max + 5.49745, colorName, rgb);
	//printf("\nRadius at beta: r_315 = %lf ", r_315);


	// triangle shape detection
	if (r_max*.90 < r_120  &&  r_120 < r_max*1.10 && shape_detected == 0)
	{
		if (r_max*.90 < r_240  &&  r_240 < r_max*1.10)
		{
			// circle shape detection
			if (r_max*.90 < r_45  &&  r_45 < r_max*1.10 && shape_detected == 0)
			{
				if (r_max*.90 < r_90  &&  r_90 < r_max*1.10)
				{
					if (r_max*.90 < r_180  &&  r_180 < r_max*1.10)
					{
						if (r_max*.90 < r_225  &&  r_225 < r_max*1.10)
						{
							if (r_max*.90 < r_270  &&  r_270 < r_max*1.10)
							{
								if (r_max*.90 < r_315  &&  r_315 < r_max*1.10)
								{
									printf("\nshape is circle");
									shape_detected = 1;
								}
							}
						}

					}
				}
			}



			if (shape_detected == 0)
			{
				printf("\nshape is triangle");
				shape_detected = 1;
			}
		}
	}


	// square shape detection
	if (r_max*.90 < r_90  &&  r_90 < r_max*1.10 && shape_detected == 0)
	{
		if (r_max*.90 < r_180  &&  r_180 < r_max*1.10)
		{
			if (r_max*.90 < r_270  &&  r_270 < r_max*1.10)
			{
				printf("\nshape is square");
				shape_detected = 1;
			}
		}
	}



	if (shape_detected == 0)
	{
		printf("\n unknown shape");
	}

	return point_counter;
}
