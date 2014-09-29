/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#define PI 3.141592653;

int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
	if(mat == NULL) {
		return GZ_FAILURE;
	}
	float rad = ( degree * (3.141592653/180));

	GzMatrix rot = {
		1, 0, 0, 0,
		0, cos(rad), -sin(rad), 0,
		0, sin(rad), cos(rad), 0,
		0, 0, 0, 1
	};

	memcpy(mat, rot, sizeof(GzMatrix));

	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
	if(mat == NULL) {
		return GZ_FAILURE;
	}
	float rad = ( degree * (3.141592653/180));

	GzMatrix rot = { 
		cos(rad), 0, sin(rad), 0,
		0, 1, 0, 0,
		-sin(rad), 0, cos(rad), 0,
		0, 0, 0, 1 
	};
	memcpy(mat, rot, sizeof(GzMatrix));
	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
	if(mat == NULL) {
		return GZ_FAILURE;
	}
	float rad = ( degree * (3.141592653/180));

	GzMatrix rot = { 
		cos(rad), -sin(rad), 0, 0,
		sin(rad), cos(rad), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 
	};
	
	memcpy(mat, rot, sizeof(GzMatrix));
	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
	if(mat == NULL) {
		return GZ_FAILURE;
	}

	GzMatrix trans = { 1, 0, 0 , translate[X],
		0, 1, 0, translate[Y],
		0, 0, 1, translate[Z],
		0, 0 ,0, 1
	};
	
	memcpy(mat, trans, sizeof(GzMatrix));
	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
	if(mat == NULL) {
		return GZ_FAILURE;
	}

	GzMatrix scaleMat = { 
		scale[X], 0, 0 , 0,
		0, scale[Y], 0, 0,
		0, 0, scale[Z], 0,
		0, 0 ,0, 1
	};
	
	memcpy(mat, scaleMat, sizeof(GzMatrix));
	
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

	if(renderClass == GZ_Z_BUFFER_RENDER && display != NULL) {
		*render = new GzRender();
		if(*render == NULL) {
			return GZ_FAILURE;
		}

		(*render)->renderClass = renderClass;
		(*render)->open = 0;
		(*render)->matlevel = -1;
		(*render)->display = display;

		//form Xsp
		float Xres = display->xres;
		float Yres = display->yres;

		/*float d = 1/tan(DEFAULT_FOV/2);

		GzMatrix Xsp = {
			Xres/2, 0, 0, Xres/2,
			0, -Yres/2, 0, Yres/2,
			0, 0, INT_MAX/d, 0,
			0, 0, 0, 1
		};

		for(int i=0; i<4; i++) {
			for(int j=0; j<4; j++) {
				(*render)->Xsp[i][j] = Xsp[i][j];
			}
		}*/

		//init camera defaults
		(*render)->camera.lookat[X] = 0;
		(*render)->camera.lookat[Y] = 0;
		(*render)->camera.lookat[Z] = 0;
		(*render)->camera.FOV = DEFAULT_FOV;
		(*render)->camera.worldup[X] = 0;
		(*render)->camera.worldup[Y] = 1;
		(*render)->camera.worldup[Z] = 0;
		(*render)->camera.position[X] = DEFAULT_IM_X;
		(*render)->camera.position[Y] = DEFAULT_IM_Y;
		(*render)->camera.position[Z] = DEFAULT_IM_Z;

		return GZ_SUCCESS;;
	}
	return GZ_FAILURE;
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
	if(render == NULL) {
		return GZ_FAILURE;
	}

	int size = (render->display->xres) * (render->display->yres);
	render->display->fbuf = new GzPixel[size];

	for (int i = 0; i < size; i++)
	{
		render->display->fbuf[i].red = 2000;
		render->display->fbuf[i].green = 2000;
		render->display->fbuf[i].blue = 2000;
		render->display->fbuf[i].z = INT_MAX;
		render->display->fbuf[i].alpha = 0;
	}

	render->open = 1;
	render->flatcolor[0] = 4095;
	render->flatcolor[1] = 4095;
	render->flatcolor[2] = 4095;
	//GzInitDisplay(render->display);

	//d = 1/tan(fov/2)
	float rad = (float)( render->camera.FOV * (3.141592653/180));
	float d = 1/tan(rad/2);
	float Xres = render->display->xres;
	float Yres = render->display->yres;

	GzMatrix Xsp = {
			Xres/2, 0, 0, Xres/2,
			0, -Yres/2, 0, Yres/2,
			0, 0, INT_MAX/d, 0,
			0, 0, 0, 1
	};

	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			(render)->Xsp[i][j] = Xsp[i][j];
		}
	}

	GzMatrix Xpi = {
		1, 0, 0 ,0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 1/d, 1
	};

	GzCoord lookatVector = {
		render->camera.lookat[X],
		render->camera.lookat[Y],
		render->camera.lookat[Z]
	};

	GzCoord cameraVector = {
		render->camera.position[X],
		render->camera.position[Y],
		render->camera.position[Z]
	};

	GzCoord clVector  = {
		lookatVector[X] - cameraVector[X],
		lookatVector[Y] - cameraVector[Y],
		lookatVector[Z] - cameraVector[Z]
	};

	//mag = X^2 + Y^2 + Z^2
	float magCLVector = sqrt((clVector[X]*clVector[X]) + (clVector[Y]*clVector[Y]) + (clVector[Z]*clVector[Z]));

	GzCoord cameraZ = {
		clVector[X] / magCLVector,
		clVector[Y] / magCLVector,
		clVector[Z] / magCLVector
	};

	float upDotProductCameraZ = (render->camera.worldup[X]*cameraZ[X]) + (render->camera.worldup[Y]*cameraZ[Y]) + (render->camera.worldup[Z]*cameraZ[Z]);

	GzCoord upDash = {
		render->camera.worldup[X] - (upDotProductCameraZ*cameraZ[X]),
		render->camera.worldup[Y] - (upDotProductCameraZ*cameraZ[Y]),
		render->camera.worldup[Z] - (upDotProductCameraZ*cameraZ[Z])
	};

	float magUpDash = sqrt((upDash[X] * upDash[X]) + (upDash[Y] * upDash[Y]) + (upDash[Z] * upDash[Z]));

	GzCoord cameraY = {
		upDash[X] / magUpDash,
		upDash[Y] / magUpDash,
		upDash[Z] / magUpDash
	};

	GzCoord cameraX = {
		(cameraY[Y] * cameraZ[Z]) - (cameraY[Z] * cameraZ[Y]),
		(cameraY[Z] * cameraZ[X]) - (cameraY[X] * cameraZ[Z]),
		(cameraY[X] * cameraZ[Y]) - (cameraY[Y] * cameraZ[X])
	};

	float cameraXDotCameraVector = (cameraX[X]*cameraVector[X]) + (cameraX[Y]*cameraVector[Y]) + (cameraX[Z]*cameraVector[Z]);
	float cameraYDotCameraVector = (cameraY[X]*cameraVector[X]) + (cameraY[Y]*cameraVector[Y]) + (cameraY[Z]*cameraVector[Z]);
	float cameraZDotCameraVector = (cameraZ[X]*cameraVector[X]) + (cameraZ[Y]*cameraVector[Y]) + (cameraZ[Z]*cameraVector[Z]);

	GzMatrix Xiw = {
		cameraX[X], cameraX[Y], cameraX[Z], -cameraXDotCameraVector,
		cameraY[X], cameraY[Y], cameraY[Z], -cameraYDotCameraVector,
		cameraZ[X], cameraZ[Y], cameraZ[Z], -cameraZDotCameraVector,
		0, 0, 0, 1
	};

	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			render->camera.Xpi[i][j] = Xpi[i][j];
			render->camera.Xiw[i][j] = Xiw[i][j];
		}
	}

	GzPushMatrix(render, render->Xsp);
	GzPushMatrix(render, Xpi);
	GzPushMatrix(render, Xiw);
	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	if(render == NULL && camera == NULL) {
		return GZ_FAILURE;
	}
	
	render->camera.FOV = camera->FOV;
	render->camera.position[X] = camera->position[X];
	render->camera.position[Y] = camera->position[Y];
	render->camera.position[Z] = camera->position[Z];
	render->camera.lookat[X] = camera->lookat[X];
	render->camera.lookat[Y] = camera->lookat[Y];
	render->camera.lookat[Z] = camera->lookat[Z];
	render->camera.worldup[X] = camera->worldup[X];
	render->camera.worldup[Y] = camera->worldup[Y];
	render->camera.worldup[Z] = camera->worldup[Z];

	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			render->camera.Xiw[i][j] = camera->Xiw[i][j];
			render->camera.Xpi[i][j] = camera->Xpi[i][j];
		}
	}

	return GZ_SUCCESS;	
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	if(render == NULL || matrix == NULL) {
		return GZ_FAILURE;
	}

	int stackTop = ++render->matlevel;
	if(stackTop == 0) {
		for(int i=0; i<4; i++) {
			for(int j=0; j<4; j++) {
				render->Ximage[stackTop][i][j] = matrix[i][j];
			}
		}
	}
	else {
		multiplyMatrix(render->Ximage[render->matlevel-1], matrix, render->Ximage[render->matlevel]);	
	}
	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if(render->matlevel == -1)
		return GZ_FAILURE;
	render->matlevel--;
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
	if(render == NULL || numParts == 0) {
		return GZ_FAILURE;
	}

	//calculate Xsm
	//GzMatrix *Xsm = (GzMatrix*)malloc(sizeof(GzMatrix));
	GzMatrix intermediate, Xsm;
	//if(render->matlevel >= 0) {
		memcpy(Xsm, render->Ximage[render->matlevel], sizeof(GzMatrix));
	/*	GzPopMatrix(render);
		while(render->matlevel >= 0) {
			memcpy(intermediate, Xsm, sizeof(GzMatrix));
			//Xsm = (multiplyMatrix(intermediate, render->Ximage[render->matlevel]));
			for (int i=0 ; i<4 ; i++){
				for (int j=0 ; j<4 ; j++){
					Xsm[i][j]=0;
					for (int k=0 ; k<4 ; k++){
						Xsm[i][j] += (intermediate[i][k] * render->Ximage[render->matlevel][k][j]);
					}
				}
			}
			GzPopMatrix(render);
		}
		GzPushMatrix(render, Xsm);
	}*/
	int i;
	for(i = 0; i < numParts ; i++) {
		if(nameList[i] == GZ_POSITION) {
			//GzCoord *cord[3] = (GzCoord*)malloc(sizeof(float) * 9);
			GzCoord cordOrig[3];
			cordOrig[0][0] = *((float*)valueList[i]);
			cordOrig[0][1] = *(((float*)valueList[i])+1);
			cordOrig[0][2] = *(((float*)valueList[i])+2);
			cordOrig[1][0] = *(((float*)valueList[i])+3);
			cordOrig[1][1] = *(((float*)valueList[i])+4);
			cordOrig[1][2] = *(((float*)valueList[i])+5);
			cordOrig[2][0] = *(((float*)valueList[i])+6);
			cordOrig[2][1] = *(((float*)valueList[i])+7);
			cordOrig[2][2] = *(((float*)valueList[i])+8);

			GzCoord cord[3];
			float w;
			//transform
			for(int index=0 ; index<3 ; index++) {
				cord[index][X] = render->Ximage[render->matlevel][0][0]*cordOrig[index][X] +
					render->Ximage[render->matlevel][0][1]*cordOrig[index][Y] +
					render->Ximage[render->matlevel][0][2]*cordOrig[index][Z] +
					render->Ximage[render->matlevel][0][3]*1.0;
				cord[index][Y] = render->Ximage[render->matlevel][1][0]*cordOrig[index][X] +
					render->Ximage[render->matlevel][1][1]*cordOrig[index][Y] +
					render->Ximage[render->matlevel][1][2]*cordOrig[index][Z] +
					render->Ximage[render->matlevel][1][3]*1.0;
				cord[index][Z] = render->Ximage[render->matlevel][2][0]*cordOrig[index][X] +
					render->Ximage[render->matlevel][2][1]*cordOrig[index][Y] +
					render->Ximage[render->matlevel][2][2]*cordOrig[index][Z] +
					render->Ximage[render->matlevel][2][3]*1.0;
				w = render->Ximage[render->matlevel][3][0]*cordOrig[index][X] +
					render->Ximage[render->matlevel][3][1]*cordOrig[index][Y] +
					render->Ximage[render->matlevel][3][2]*cordOrig[index][Z] +
					render->Ximage[render->matlevel][3][3]*1.0;
				
				cord[index][X] /= w;
				cord[index][Y] /= w;
				cord[index][Z] /= w;
			}

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

void multiplyMatrix(GzMatrix a, GzMatrix b, GzMatrix r) {
	for (int i=0 ; i<4 ; i++){
		for (int j=0 ; j<4 ; j++){
			r[i][j]=0;
			for (int k=0 ; k<4 ; k++){
                r[i][j] += (a[i][k] * b[k][j]);
            }
        }
    }
}
