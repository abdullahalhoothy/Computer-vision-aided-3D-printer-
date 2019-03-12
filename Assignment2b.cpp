//Assignment1

#include <iostream>
#include <cmath>
#include <conio.h>

using namespace std;

// include this header file for basic image transfer functions
#include "image_transfer2.h"

// include this header file for computer vision functions
#include "vision.h"

#include "timer.h"

int main(int argc, char* argv[])
{


	image rgb1, a;
	int camera;
	int k = 0, size = 0, q = 0, n = 0, Si = 0, Sj = 0;

	ibyte R, G, B; // 0 - 255 
	//-- if you add 50 to 245 it will roll
	char ch;

	//ibyte *pa, *pb;

	// set camera (normally 0 or 1)
	camera = 0;

	// initialize vision library
	activate_vision(camera);

	printf("\npress a key to acquire image\n");
	getch();



	rgb1.width = 640;
	rgb1.height = 480;
	rgb1.type = RGB_IMAGE;

	a.width = 640;
	a.height = 480;
	a.type = GREY_IMAGE;


	// allocate dynamic memory for the image
	// one big 1D dynamic array
	allocate_image(rgb1);
	allocate_image(a);

	printf("\npress x key to end program");

	// number of pixels in image
	size = 640 * 480 * 3;

	while (1) {
		// get a rgb image with 3 bytes per pixel
		acquire_image(rgb1);

		q = 0;
		n = 0;
		Si = 0;
		Sj = 0;
		for (k = 0; k < size; k += 3) {
			// components are stored in the order B-G-R
			// B0 G0 R0 B1 G1 R1 .... Bsize-1 Gsize-1 Rsize-1
			B = rgb1.pdata[k];
			G = rgb1.pdata[k + 1];
			R = rgb1.pdata[k + 2];

			if (B > 150 && G < 100 && R < 100)
			{
				n++;
				int i = q % 640;
				Si += i;
				Sj += (q - i) / 640;
			}
			q++;
		}
		// press 'x' key to end program
		if (kbhit()) {
			ch = getch();
			if (ch == 'x') break;
		}
		if (n > 0)
		{
			int I = Si / n;
			int J = Sj / n;
			int K = I + (640 * J);
			copy(rgb1, a);

			for (int i = 0; i < 10; i++)
			{

				a.pdata[K + i] = (ibyte)0;


				for (int f = 1; f < 10; f++)
				{
					a.pdata[K + (f * 640) + i] = (ibyte)0;
				}
			}

		}
		copy(a, rgb1);
		view_image(rgb1);
	}

	save_image("mypic.bmp", rgb1);
	// delete the dynamic memory
	free_image(rgb1);
	free_image(a);
	// this is important for the eye 3
	deactivate_vision();

	printf("\n\ndone.\n");
	getch();

	return 0; // no errors
}