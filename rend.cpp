/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"


int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value

	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value

	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value

	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value

	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value

	return GZ_SUCCESS;
}


//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- keep closed until all inits are done 
- setup Xsp and anything only done once 
- span interpolator needs pointer to display 
- check for legal class GZ_Z_BUFFER_RENDER 
- init default camera 
*/ 

	*render = new GzRender();
	(*render)->renderClass = renderClass;
	(*render)->display = display;
	(*render)->open = 0;
	return GZ_SUCCESS;

}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	delete render;
	return GZ_SUCCESS;
}


int GzBeginRender(GzRender *render)
{
/*  
- set up for start of each frame - clear frame buffer 
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms if it want to. 
*/ 
	render->open = 1;
	render->flatcolor[0] = 4095;
	render->flatcolor[1] = 4095;
	render->flatcolor[2] = 4095;
	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	return GZ_SUCCESS;	
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	int i;
	for(i = 0; i < numAttributes ; i++) {
		if(nameList[i] == GZ_RGB_COLOR) {
			float *list = (float*)valueList[i];
			render->flatcolor[0] = (list)[0] < 0 ? 0 : list[0] > 1 ? 1 : list[0];
			render->flatcolor[1] = (list)[1] < 0 ? 0 : list[1] > 1 ? 1 : list[1];
			render->flatcolor[2] = (list)[2] < 0 ? 0 : list[2] > 1 ? 1 : list[2];
		}
	}

	if(i == 0) {
		return GZ_FAILURE;
	}
	return GZ_SUCCESS;
}

int GzPutTriangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList) 
/* numParts - how many names and values */
{
/* 
- pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
- Invoke the scan converter and return an error code
*/
	int i;
	for(i = 0; i < numParts ; i++) {
		if(nameList[i] == GZ_POSITION) {
			//GzCoord *cord[3] = (GzCoord*)malloc(sizeof(float) * 9);
			GzCoord cord[3];
			cord[0][0] = *((float*)valueList[i]);
			cord[0][1] = *(((float*)valueList[i])+1);
			cord[0][2] = *(((float*)valueList[i])+2);
			cord[1][0] = *(((float*)valueList[i])+3);
			cord[1][1] = *(((float*)valueList[i])+4);
			cord[1][2] = *(((float*)valueList[i])+5);
			cord[2][0] = *(((float*)valueList[i])+6);
			cord[2][1] = *(((float*)valueList[i])+7);
			cord[2][2] = *(((float*)valueList[i])+8);

			//find min, max X & Y
			float maxX=-1, maxY=-1, minX=MAXXRES+1, minY=MAXYRES+1;
			for(int i=0 ; i<3 ; i++) {
				if(cord[i][0] < minX)
					minX = cord[i][0];
				if(cord[i][0] > maxX)
					maxX = cord[i][0];
				if(cord[i][1] < minY)
					minY = cord[i][1];
				if(cord[i][1] > maxY)
					maxY = cord[i][1];
			}
			
			if(maxX > 255) 
				maxX = 256;
			if(maxY > 255)
				maxY = 256;
			if(maxX < 0)
				maxX = 0;
			if(minY < 0)
				minY = 0;
			if(minX < 0)
				minX = 0;
			if(minX > 255)
				minX = 256;
			if(minY < 0)
				minY = 0;
			if(minY > 255)
				minY = 256;
			//sort(&cord);

			for(int i=0 ; i<3 ; i++) {
				for(int j=0; j<3 ; j++) {
					if(cord[i][1] < cord[j][1]) {
						float tempX = cord[i][0];
						float tempY = cord[i][1];
						float tempZ = cord[i][2];
						cord[i][0] = cord[j][0];
						cord[i][1] = cord[j][1];
						cord[i][2] = cord[j][2];
						cord[j][0] = tempX;
						cord[j][1] = tempY;
						cord[j][2] = tempZ;
					}
					else if(cord[i][1] == cord[j][1] && cord[i][0] < cord[j][0]) {
						float tempX = cord[i][0];
						float tempY = cord[i][1];
						float tempZ = cord[i][2];
						cord[i][0] = cord[j][0];
						cord[i][1] = cord[j][1];
						cord[i][2] = cord[j][2];
						cord[j][0] = tempX;
						cord[j][1] = tempY;
						cord[j][2] = tempZ;
					}
				}
			}

			// calculate line equations
			float A[3], B[3], C[3], D[3];

			calculateLineEquation(cord[2], cord[0], &A[2], &B[2], &C[2]);
			//Ax + By + C = 0
			//x = (-C - By)/A
			int leftEdge = 0;
			float x1 = (-C[2] - B[2]*cord[1][1])/A[2];
			if(cord[1][0] <= x1) {
				// need to switch points
				float tempX = cord[1][0];
				float tempY = cord[1][1];
				float tempZ = cord[1][2];
				cord[1][0] = cord[2][0];
				cord[1][1] = cord[2][1];
				cord[1][2] = cord[2][2];
				cord[2][0] = tempX;
				cord[2][1] = tempY;
				cord[2][2] = tempZ;
				leftEdge++;
			}
			calculateLineEquation(cord[0], cord[1], &A[0], &B[0], &C[0]);
			calculateLineEquation(cord[1], cord[2], &A[1], &B[1], &C[1]);
			calculateLineEquation(cord[2], cord[0], &A[2], &B[2], &C[2]);

			GzDisplay *display = render->display;
			GzEdge e1, e2;
			e1.pt1[0] = cord[0][0];
								e1.pt1[1] = cord[0][1];
								e1.pt1[2] = cord[0][2];
								e1.pt2[0] = cord[1][0];
								e1.pt2[1] = cord[1][1];
								e1.pt2[2] = cord[1][2];
								e2.pt1[0] = cord[1][0];
								e2.pt1[1] = cord[1][1];
								e2.pt1[2] = cord[1][2];
								e2.pt2[0] = cord[2][0];
								e2.pt2[1] = cord[2][1];
								e2.pt2[2] = cord[2][2];
			// check all pixles between (minX, minY) to (maxX, maxY)
			int cnt=0;
			GzDepth z=0;
			for(int i = floor(minX) ; i < ceil(maxX) ; i++) {
				for(int j = floor(minY) ; j < ceil(maxY); j++) {
					if(substitute(i, j, A[0], B[0], C[0]) <= 0 && 
						substitute(i, j, A[1], B[1], C[1]) <= 0 &&
						substitute(i, j, A[2], B[2], C[2]) <= 0) {
							z = (interpolateZ(e1, e2, i, j));
							if(display->fbuf[ARRAY(i, j)].z == 0 || (z) < (display->fbuf[ARRAY(i, j)].z)) {
								/*if(leftEdge == 1 && (int)substitute(i, j, A[1], B[1], C[1]) < 0 && 
									(int)substitute(i, j, A[2], B[2], C[2]) <= 0 && 
									(int)substitute(i, j, A[0], B[0], C[0]) < 0) {
									
									display->fbuf[ARRAY(i, j)].red = ctoi(render->flatcolor[0]);
									display->fbuf[ARRAY(i, j)].green = ctoi(render->flatcolor[1]);
									display->fbuf[ARRAY(i, j)].blue = ctoi(render->flatcolor[2]);
									display->fbuf[ARRAY(i, j)].z = (z);
									//GzPutDisplay(display, i, j, ctoi(render->flatcolor[0]), ctoi(render->flatcolor[1]), ctoi(render->flatcolor[2]), z, 1);
								}
								else if(leftEdge == 0 && (int)substitute(i, j, A[0], B[0], C[0]) <= 0 && 
									(int)substitute(i, j, A[1], B[1], C[1]) < 0 && 
									(int)substitute(i, j, A[2], B[2], C[2]) <= 0) {
									
									display->fbuf[ARRAY(i, j)].red = ctoi(render->flatcolor[0]);
									display->fbuf[ARRAY(i, j)].green = ctoi(render->flatcolor[1]);
									display->fbuf[ARRAY(i, j)].blue = ctoi(render->flatcolor[2]);
									display->fbuf[ARRAY(i, j)].z = (z);
								}
								else {*/
									display->fbuf[ARRAY(i, j)].red = ctoi(render->flatcolor[0]);
									display->fbuf[ARRAY(i, j)].green = ctoi(render->flatcolor[1]);
									display->fbuf[ARRAY(i, j)].blue = ctoi(render->flatcolor[2]);
									display->fbuf[ARRAY(i, j)].z = (z);
								//}
							}
							cnt++;
					}
				}
			}
			/*if(cnt==0) {
				GzEdge e1, e2;
				e1.pt1[0] = cord[0][0];
				e1.pt1[1] = cord[0][1];
				e1.pt1[2] = cord[0][2];
				e1.pt2[0] = cord[1][0];
				e1.pt2[1] = cord[1][1];
				e1.pt2[2] = cord[1][2];
				e2.pt1[0] = cord[1][0];
				e2.pt1[1] = cord[1][1];
				e2.pt1[2] = cord[1][2];
				e2.pt2[0] = cord[2][0];
				e2.pt2[1] = cord[2][1];
				e2.pt2[2] = cord[2][2];
				if(cord[0][0] == cord[1][0] && cord[1][0] == cord[2][0]) {
					cord[0][0] = ceil(cord[0][0]);
					for(int j = (minY) ; j < ceil(maxY); j++) {
						z = interpolateZ(e1, e2, cord[0][0], j);
						if(display->fbuf[ARRAY((int)(cord[0][0]), j)].z == 0 || (z) <= (display->fbuf[ARRAY((int)cord[0][0], j)].z)) {
							display->fbuf[ARRAY((int)cord[0][0], j)].red = ctoi(render->flatcolor[0]);
							display->fbuf[ARRAY((int)cord[0][0], j)].green = ctoi(render->flatcolor[1]);
							display->fbuf[ARRAY((int)cord[0][0], j)].blue = ctoi(render->flatcolor[2]);
							display->fbuf[ARRAY((int)cord[0][0], j)].z = ceil(z);
						}
					}
				}
				if((cord[0][1] == cord[1][1]) && (cord[1][1] == cord[2][1])) {
					cord[1][1] = ceil(cord[1][1]);;
					for(int i = (minX) ; i < ceil(maxX) ; i++) {
						z = ceil(interpolateZ(e1, e2, i, cord[1][1]));
						if(display->fbuf[ARRAY(i, (int)cord[1][1])].z == 0 || (int)z <= display->fbuf[ARRAY(i, (int)cord[1][1])].z) {
							display->fbuf[ARRAY(i,(int)cord[1][1])].red = ctoi(render->flatcolor[0]);
							display->fbuf[ARRAY(i,(int)cord[1][1])].green = ctoi(render->flatcolor[1]);
							display->fbuf[ARRAY(i,(int)cord[1][1])].blue = ctoi(render->flatcolor[2]);
							display->fbuf[ARRAY(i,(int)cord[1][1])].z = ceil(z);
						}
					}
				}
			}*/
		}
	}
	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */
float substitute(int x, int y, float A, float B, float C) {
	// AX + BY + C = 0
	return (A*x + B*y + C);
}

int interpolateZ(GzEdge e1, GzEdge e2, int i, int j) {
	float A1, B1, C1, D1;
	calculatePlaneEquation(e1, e2, &A1, &B1, &C1, &D1);
	return (-((A1)*i + (B1)*j + D1)/(C1));
}

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

void calculateLineEquation(GzCoord pt1, GzCoord pt2, float *A, float *B, float *C) {
	//AX + BY + C = 0
	//A = dY, B = -dX
	//C = dXY - dYX
	*A = pt2[1] - pt1[1];
	*B = -(pt2[0] - pt1[0]);
	*C = (pt2[0] - pt1[0])*pt1[1] - (pt2[1] - pt1[1])*pt1[0];
}

void calculatePlaneEquation(GzEdge e1, GzEdge e2, float *A, float *B, float *C, float *D) {

	/**A = (e1.pt1[1]-e1.pt2[1])*(e2.pt1[2]-e2.pt2[2]) - (e1.pt1[2]-e1.pt1[2])*(e2.pt1[1]-e2.pt2[1]);
	*B = (e1.pt1[0]-e1.pt2[0])*(e2.pt1[2]-e2.pt2[2]) - (e1.pt1[2]-e1.pt2[2])*(e2.pt1[0]-e2.pt2[0]);
	*C = (e1.pt1[0]-e1.pt2[0])*(e2.pt1[1]-e2.pt2[1]) - (e1.pt1[1]-e1.pt2[1])*(e2.pt1[0]-e2.pt2[0]);

	//AX + BY + CZ + D = 0
	//D = -AX - BY - CZ
	*D = -(*A * e1.pt1[0]) - (*B * e1.pt1[1]) - (*C * e1.pt1[2]);*/

	// calculate vectors for 2 edges
	float V1[3], V2[3];
	V1[0] = e1.pt2[0] - e1.pt1[0];
	V1[1] = e1.pt2[1] - e1.pt1[1];
	V1[2] = e1.pt2[2] - e1.pt1[2];

	V2[0] = e2.pt2[0] - e2.pt1[0];
	V2[1] = e2.pt2[1] - e2.pt1[1];
	V2[2] = e2.pt2[2] - e2.pt1[2];

	// Dot product of 2 vectors
	// A = V1[y] * V1[z] - V2[y] * V1[z]
	// B = V1[z] * V2[x] - V2[z] * V1[x]
	// C = V1[x] * V2[y] - V1[x] * V2[y]

	*A = (V1[1]*V2[2]) - (V2[1]*V1[2]);
	*B = (V1[2]*V2[0]) - (V2[2]*V1[0]);
	*C = (V1[0]*V2[1]) - (V2[0]*V1[1]);
	*D = -(*A)*e1.pt2[0] - (*B)*e1.pt2[1] - (*C)*e1.pt2[2];
}
int sort(float **cord) {
	for(int i=0 ; i<3 ; i++) {
		for(int j=0; j<3 ; j++) {
			if(cord[i][1] < cord[j][1]) {
				float tempX = cord[i][0];
				float tempY = cord[i][1];
				float tempZ = cord[i][2];
				cord[i][0] = cord[j][0];
				cord[i][1] = cord[j][1];
				cord[i][2] = cord[j][2];
				cord[j][0] = tempX;
				cord[j][1] = tempY;
				cord[j][2] = tempZ;
			}
			else if(cord[i][1] == cord[j][1] && cord[i][0] < cord[i][0]) {
				float tempX = cord[i][0];
				float tempY = cord[i][1];
				float tempZ = cord[i][2];
				cord[i][0] = cord[j][0];
				cord[i][1] = cord[j][1];
				cord[i][2] = cord[j][2];
				cord[j][0] = tempX;
				cord[j][1] = tempY;
				cord[j][2] = tempZ;
			}
		}
	}
	return 1;
}


