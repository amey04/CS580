/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"
#include "math.h"
#include <complex>
#include <map>

GzColor	*image;
int xs, ys;
int reset = 1;

int POS(GzCoord C) {
	return C[X]+(C[Y]*xs);
}

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
    fd = fopen ("texture", "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* bounds-test u,v to make sure nothing will overflow image array bounds */
    if (u < 0) {
		u = 0;
	} else if (u > 1) {
		u = 1;
	}
	if (v < 0) {
		v = 0;
	} else if (v > 1) {
		v = 1;
	}
/* determine texture cell corner values and perform bilinear interpolation */
	float u1 = (u * (xs-1));
	float v1 = (v * (ys-1));

	//find 4 adj points;
	GzCoord A, B, C, D;
	A[X] = floor(u1);
	A[Y] = floor(v1);

	B[X] = ceil(u1);
	B[Y] = floor(v1);

	C[X] = ceil(u1);
	C[Y] = ceil(v1);

	D[X] = floor(u1);
	D[Y] = ceil(v1);

/* set color to interpolated GzColor value and return */
	color[RED] = u*v*image[POS(C)][RED] + (1-u)*v*image[POS(D)][RED] + u*(1-v)*image[POS(B)][RED] + (1-u)*(1-v)*image[POS(A)][RED];
	color[GREEN] = u*v*image[POS(C)][GREEN] + (1-u)*v*image[POS(D)][GREEN] + u*(1-v)*image[POS(B)][GREEN] + (1-u)*(1-v)*image[POS(A)][GREEN];
	color[BLUE] = u*v*image[POS(C)][BLUE] + (1-u)*v*image[POS(D)][BLUE] + u*(1-v)*image[POS(B)][BLUE] + (1-u)*(1-v)*image[POS(A)][BLUE];

	return GZ_SUCCESS;
}

void mapColor(float length, GzColor c)
{
	/*GzColor c1 = {176,23,31};
	GzColor c2 = {0,255,137};
	GzColor c3 = {202,255,112};
	GzColor c4 = {255,165,0};
	GzColor c5 = {142,0,142};
	GzColor c6 = {225,228,225};
	GzColor c7 = {0,139,139};
	GzColor c8 = {0,0,255};
	GzColor c9 = {255,0,0};
	GzColor c10 = {255,0,255};
	GzColor c11 = {139,105,20};*/	

	GzColor c1 = {1,0,0};
	GzColor c2 = {0,1,0};
	GzColor c3 = {1,1,0.2};
	GzColor c4 = {0,0,1};
	GzColor c5 = {0.6,0,0.4};
	GzColor c6 ={0,0.4,0};
	GzColor c7 = {1,0.6,0};
	GzColor c8 = {0,0,0.4};
	GzColor c9 = {0,0.5,0.5};
	GzColor c10 = {1,0,0.6};
	GzColor c11 = {0,0.4,0.3};

	float a, b;

	if(length >= 0.0 && length <= 0.2) {
		a = (0.2 - length)/(0.2);
		b = (length)/(0.2);
		c[RED] = a*c1[RED] + b*c2[RED];
		c[GREEN] = a*c1[GREEN] + b*c2[GREEN];
		c[BLUE] = a*c1[BLUE] + b*c2[BLUE];
	}
	else if(length > 0.2 && length <= 0.4) {
		a = (0.4 - length)/(0.2);
		b = (length - 0.2)/(0.2);
		c[RED] = a*c2[RED] + b*c3[RED];
		c[GREEN] = a*c2[GREEN] + b*c3[GREEN];
		c[BLUE] = a*c2[BLUE] + b*c3[BLUE];
	}
	else if(length > 0.4 && length <= 0.6) {
		a = (0.6 - length)/(0.2);
		b = (length - 0.4)/(0.2);
		c[RED] = a*c3[RED] + b*c4[RED];
		c[GREEN] = a*c3[GREEN] + b*c4[GREEN];
		c[BLUE] = a*c3[BLUE] + b*c4[BLUE];
	}
	else if(length > 0.6 && length <= 0.8) {\
		a = (0.8 - length)/(0.2);
		b = (length - 0.6)/(0.2);
		c[RED] = a*c4[RED] + b*c5[RED];
		c[GREEN] = a*c4[GREEN] + b*c5[GREEN];
		c[BLUE] = a*c4[BLUE] + b*c5[BLUE];
	}
	else if(length > 0.8 && length <= 1.0) {
		a = (1.0 - length)/(0.2);
		b = (length - 0.8)/(0.2);
		c[RED] = a*c5[RED] + b*c6[RED];
		c[GREEN] = a*c5[GREEN] + b*c6[GREEN];
		c[BLUE] = a*c5[BLUE] + b*c6[BLUE];
	}
	else if(length > 1.0 && length <= 1.2) {
		a = (1.2 - length)/(0.2);
		b = (length - 1.0)/(0.2);
		c[RED] = a*c6[RED] + b*c7[RED];
		c[GREEN] = a*c6[GREEN] + b*c7[GREEN];
		c[BLUE] = a*c6[BLUE] + b*c7[BLUE];
	}
	else if(length > 1.2 && length <= 1.4) {
		a = (1.4 - length)/(0.2);
		b = (length - 1.2)/(0.2);
		c[RED] = a*c7[RED] + b*c8[RED];
		c[GREEN] = a*c7[GREEN] + b*c8[GREEN];
		c[BLUE] = a*c7[BLUE] + b*c8[BLUE];
	}
	else if(length > 1.4 && length <= 1.6) {
		a = (1.6 - length)/(0.2);
		b = (length - 1.4)/(0.2);
		c[RED] = a*c8[RED] + b*c9[RED];
		c[GREEN] = a*c8[GREEN] + b*c9[GREEN];
		c[BLUE] = a*c8[BLUE] + b*c9[BLUE];
	}
	else if(length > 1.4 && length <= 1.8) {
		a = (1.8 - length)/(0.2);
		b = (length - 1.6)/(0.2);
		c[RED] = a*c9[RED] + b*c10[RED];
		c[GREEN] = a*c9[GREEN] + b*c10[GREEN];
		c[BLUE] = a*c9[BLUE] + b*c10[BLUE];
	}
	else if(length > 1.8 && length <= 2.0) {
		a = (1.8 - length)/(0.2);
		b = (length - 1.8)/(0.2);
		c[RED] = a*c10[RED] + b*c11[RED];
		c[GREEN] = a*c10[GREEN] + b*c11[GREEN];
		c[BLUE] = a*c10[BLUE] + b*c11[BLUE];
	}
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{	
	int maxiter = 500;

	if (u < 0) {
		u = 0;
	} else if (u > 1) {
		u = 1;
	}
	if (v < 0) {
		v = 0;
	} else if (v > 1) {
		v = 1;
	}

	using namespace std;
	complex<float> X1(u, v);
	double Cr = 0.12375;
	double Ci = 0.56805;
	complex<float> C1(0.12375, 0.56805);
	complex<float> R(0,0);
	int n=0;

	while (n < maxiter) {
		R = (X1*X1) + C1;
		if((R.real()*R.real() + R.imag()*R.imag()) > 2)
			break;
		X1 = R;
		n++;
	}
	float real = R.real();
	float imag = R.imag();
	float length = sqrt(real*real + imag*imag);
	if(length < 0)
		length = 0;
	if(length > 2)
		length = 2;

	mapColor(length, color);
	/*if(n==maxiter) {
		color[RED] = 0.2;
		color[GREEN] = 0.2;
		color[BLUE] = 0.2;
	}
	else if(n > 200){
		color[RED] = 0;
		color[GREEN] = 0;
		color[BLUE] = 0;
	}
	else if(n > 150){
		color[RED] = 0.45;
		color[GREEN] = 0;
		color[BLUE] = 0.45;
	}
	else if(n > 100){
		color[RED] = 0.8;
		color[GREEN] = 0.7;
		color[BLUE] = 0.3;
	}
	else if(n > 50){
		color[RED] = 0.12;
		color[GREEN] = 0.32;
		color[BLUE] = 0.26;
	}

	color[RED] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
	color[GREEN] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
	color[BLUE] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));*/
	return GZ_SUCCESS;
}

