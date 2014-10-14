/*   CS580 HW   */
#include    "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"

#include <stdio.h>
#include <string>

int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* create a framebuffer:
 -- allocate memory for framebuffer : (sizeof)GzPixel x width x height
 -- pass back pointer 
*/
	*framebuffer = NULL;
	*framebuffer = (char*)malloc(sizeof(GzPixel) * width * height);
	if(framebuffer == NULL) {
		return GZ_FAILURE;
	}
	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, GzDisplayClass dispClass, int xRes, int yRes)
{

/* create a display:
  -- allocate memory for indicated class and resolution
  -- pass back pointer to GzDisplay object in display
*/
	*display = new GzDisplay();
	if(*display == NULL) {
		return GZ_FAILURE;
	}

	(*display)->xres = xRes;
	(*display)->yres = yRes;
	(*display)->dispClass = dispClass;
	(*display)->fbuf = (GzPixel*) malloc((sizeof(GzPixel) * xRes * yRes));
	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
/* clean up, free memory */
	delete display->fbuf;
	delete display;
	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass)
{
/* pass back values for an open display */

	*xRes = display->xres;
	*yRes = display->yres;
	*dispClass = display->dispClass;
	return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display)
{
/* set everything to some default values - start a new frame */
	//GzNewDisplay(&display, GZ_RGBAZ_DISPLAY, MAXXRES, MAXYRES);
	for(unsigned short yRes = 0; yRes < display->yres ; yRes++) {
		for(unsigned short xRes = 0 ; xRes < display->xres ; xRes ++) {
			display->fbuf[ARRAY(xRes, yRes)].red = 4095;
			display->fbuf[ARRAY(xRes, yRes)].blue = 4095;
			display->fbuf[ARRAY(xRes, yRes)].green = 4095;
			display->fbuf[ARRAY(xRes, yRes)].alpha = 1;
			display->fbuf[ARRAY(xRes, yRes)].z = 0;
		}
	}
	display->open = 1;
	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* write pixel values into the display */
	if(i < 0 || i > display->xres || j < 0 || j > display->yres) {
		return GZ_FAILURE;
	}
	if(r < 0 ) {
		r = 0;
	}
	if(r > 4095) {
		r = 4095;
	}
	if(g < 0) {
		g = 0;
	}
	if(g > 4095) {
		g = 4095;
	}
	if(b < 0) {
		b = 0;
	}
	if(b > 4095) {
		b = 4095;
	}
/*	if(a < 0 || a > 1 || z < 0 || z > 1) {
		return GZ_FAILURE;
	}*/

	display->fbuf[ARRAY(i,j)].red = r;
	display->fbuf[ARRAY(i,j)].green = g;
	display->fbuf[ARRAY(i,j)].blue = b;
	display->fbuf[ARRAY(i,j)].z = z;
	display->fbuf[ARRAY(i,j)].alpha = a;
	return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
	/* pass back pixel value in the display */
	/* check display class to see what vars are valid */
	if(i < 0 || i > display->xres || j < 0 || j > display->yres) {
		return GZ_FAILURE;
	}
	*r = display->fbuf[ARRAY(i,j)].red;
	*g = display->fbuf[ARRAY(i,j)].green;
	*b = display->fbuf[ARRAY(i,j)].blue;
	*z = display->fbuf[ARRAY(i,j)].z;
	*a = display->fbuf[ARRAY(i,j)].alpha;
	return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

	/* write pixels to ppm file based on display class -- "P6 %d %d 255\r" */
	if(outfile == NULL) {
		return GZ_FAILURE;
	}
	//write header
	fprintf(outfile, "P6 %d %d 255\r", display->xres, display->yres);

	for(int i=0; i<display->xres; i++) {
		for(int j=0; j<display->yres; j++) {
			fprintf(outfile, "%c%c%c", display->fbuf[ARRAY(j,i)].red >> 4, display->fbuf[ARRAY(j,i)].green >> 4, display->fbuf[ARRAY(j,i)].blue >> 4);
		}
	}
	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

	/* write pixels to framebuffer: 
		- Put the pixels into the frame buffer
		- Caution: store the pixel to the frame buffer as the order of blue, green, and red 
		- Not red, green, and blue !!!
	*/
	if(framebuffer == NULL) {
		return GZ_FAILURE;
	}

	for(int i = 0 ; i < display->xres ; i++) {
		for(int j = 0 ; j < display->yres ; j++) {
			*framebuffer++ = (char)((display->fbuf[ARRAY(j,i)].blue) >> 4);
			*framebuffer++ = (char)((display->fbuf[ARRAY(j,i)].green) >> 4);
			*framebuffer++ = (char)((display->fbuf[ARRAY(j,i)].red) >> 4);
		}
	}
	return GZ_SUCCESS;
}
