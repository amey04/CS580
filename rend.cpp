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

		(*render)->numlights = 0;
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

	//Push to Xnorm
	//Calculate unitory rotation matrix
	GzMatrix temp;
	matrix[0][3] = 0;
	matrix[1][3] = 0;
	matrix[2][3] = 0;
	matrix[3][3] = 1;
	if(render->matlevel > 1) {
		multiplyMatrix(render->Xnorm[render->matlevel-1], matrix, render->Xnorm[render->matlevel]);	
		float factor = 1/(sqrt(render->Xnorm[render->matlevel][0][0]*render->Xnorm[render->matlevel][0][0] + 
			render->Xnorm[render->matlevel][0][1]*render->Xnorm[render->matlevel][0][1] +
			render->Xnorm[render->matlevel][0][2]*render->Xnorm[render->matlevel][0][2] ));

		for(int i=0 ; i<3 ; i++) {
			for(int j=0 ; j<3 ; j++) {
				render->Xnorm[render->matlevel][i][j] *= factor;	
	//			temp[i][j] = matrix[i][j] * factor;	
			}
		}

	}
	else {
		for(int i=0 ; i<4 ; i++) {
			for(int j=0; j<4 ; j++) {
				if(i == j) 
					temp[i][j] = 1;
				else
					temp[i][j] = 0;
			}
		}
		memcpy(render->Xnorm[render->matlevel], temp, sizeof(GzMatrix));
	}
	//memcpy(render->Xnorm[render->matlevel], temp, sizeof(GzMatrix));

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
		switch(nameList[i]) {

			case GZ_RGB_COLOR:
			{
				float *list = (float*)valueList[i];
				render->flatcolor[0] = (list)[0] < 0 ? 0 : list[0] > 1 ? 1 : list[0];
				render->flatcolor[1] = (list)[1] < 0 ? 0 : list[1] > 1 ? 1 : list[1];
				render->flatcolor[2] = (list)[2] < 0 ? 0 : list[2] > 1 ? 1 : list[2];
				break;
			}
			case GZ_INTERPOLATE:
				{
				render->interp_mode = *((int*)valueList[i]);
				break;
				}
			case GZ_DIRECTIONAL_LIGHT:
				{
				if(render->numlights < MAX_LIGHTS) {				
					GzLight *light = (GzLight*)valueList[i];
					render->lights[render->numlights].color[0] = (float)light->color[0];
					render->lights[render->numlights].color[1] = (float)light->color[1];
					render->lights[render->numlights].color[2] = (float)light->color[2];
					render->lights[render->numlights].direction[X] = light->direction[X];
					render->lights[render->numlights].direction[Y] = light->direction[Y];
					render->lights[render->numlights++].direction[Z] = light->direction[Z];
				}
				else {
					return GZ_FAILURE;
				}
				break;
				}
			case GZ_AMBIENT_LIGHT:
				{
				GzLight *light = (GzLight*)valueList[i];
				render->ambientlight.color[0] = (float)light->color[0];
				render->ambientlight.color[1] = (float)light->color[1];
				render->ambientlight.color[2] = (float)light->color[2];
				render->ambientlight.direction[X] = light->direction[X];
				render->ambientlight.direction[Y] = light->direction[Y];
				render->ambientlight.direction[Z] = light->direction[Z];
				break;
				}
			case GZ_AMBIENT_COEFFICIENT:
			{
				float *list = (float*)valueList[i];
				render->Ka[0] = (list)[0] < 0 ? 0 : list[0] > 1 ? 1 : list[0];
				render->Ka[1] = (list)[1] < 0 ? 0 : list[1] > 1 ? 1 : list[1];
				render->Ka[2] = (list)[2] < 0 ? 0 : list[2] > 1 ? 1 : list[2];
				break;
			}
			case GZ_DIFFUSE_COEFFICIENT:
				{
				float *list = (float*)valueList[i];
				render->Kd[0] = (list)[0] < 0 ? 0 : list[0] > 1 ? 1 : list[0];
				render->Kd[1] = (list)[1] < 0 ? 0 : list[1] > 1 ? 1 : list[1];
				render->Kd[2] = (list)[2] < 0 ? 0 : list[2] > 1 ? 1 : list[2];
				break;
				}
			case GZ_SPECULAR_COEFFICIENT:
				{
				float *list = (float*)valueList[i];
				render->Ks[0] = (list)[0] < 0 ? 0 : list[0] > 1 ? 1 : list[0];
				render->Ks[1] = (list)[1] < 0 ? 0 : list[1] > 1 ? 1 : list[1];
				render->Ks[2] = (list)[2] < 0 ? 0 : list[2] > 1 ? 1 : list[2];
				break;
				}
			case GZ_DISTRIBUTION_COEFFICIENT:
				{
				render->spec = *((float*)valueList[i]);
				break;
				}
		}
	}

	if(i == 0) {
		return GZ_FAILURE;
	}
	return GZ_SUCCESS;
}

int GzPuttotalAreangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList) 
/* numParts - how many names and values */
{
/* 
- pass in a totalAreangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
- Invoke the scan converter and return an error code
*/
	if(render == NULL || numParts == 0) {
		return GZ_FAILURE;
	}

	//calculate Xsm
	GzMatrix intermediate, Xsm, Xn;
	memcpy(Xsm, render->Ximage[render->matlevel], sizeof(GzMatrix));
	GzMatrix temp;
	memcpy(temp, render->Xnorm[0], sizeof(GzMatrix));
	GzMatrix result;
	for(int k=1 ; k<render->matlevel; k++) {
		multiplyMatrix(temp, render->Xnorm[k], result);
		memcpy(temp, result, sizeof(GzMatrix));
	}
	memcpy(Xn, render->Xnorm[render->matlevel], sizeof(GzMatrix));

	int i;
	GzCoord cord[3];
	GzCoord xformNorm[3];

	for(i = 0; i < numParts ; i++) {
		if(nameList[i] == GZ_POSITION) {
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
		}
		else if(nameList[i] == GZ_NORMAL) {
			GzCoord normals[3];
			normals[0][0] = *((float*)valueList[i]);
			normals[0][1] = *(((float*)valueList[i])+1);
			normals[0][2] = *(((float*)valueList[i])+2);
			normals[1][0] = *(((float*)valueList[i])+3);
			normals[1][1] = *(((float*)valueList[i])+4);
			normals[1][2] = *(((float*)valueList[i])+5);
			normals[2][0] = *(((float*)valueList[i])+6);
			normals[2][1] = *(((float*)valueList[i])+7);
			normals[2][2] = *(((float*)valueList[i])+8);
			
			for(int j=0 ; j<3 ; j++) {
				xformNorm[j][X] = Xn[0][0]*normals[j][X] + Xn[0][1]*normals[j][Y] + Xn[0][2]*normals[j][Z] ;
				xformNorm[j][Y] = Xn[1][0]*normals[j][X] + Xn[1][1]*normals[j][Y] + Xn[1][2]*normals[j][Z] ;
				xformNorm[j][Z] = Xn[2][0]*normals[j][X] + Xn[2][1]*normals[j][Y] + Xn[2][2]*normals[j][Z] ;
				normalizeVector(xformNorm[j]);
			}
		}
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
			
	for(int i=0 ; i<3 ; i++) {
		for(int j=0; j<3 ; j++) {
			if(cord[i][1] < cord[j][1] || (cord[i][1] == cord[j][1] && cord[i][0] < cord[j][0])) {
				float tempX = cord[i][0];
				float tempY = cord[i][1];
				float tempZ = cord[i][2];
				float tempNX = xformNorm[i][X];
				float tempNY = xformNorm[i][Y];
				float tempNZ = xformNorm[i][Z];
				cord[i][0] = cord[j][0];
				cord[i][1] = cord[j][1];
				cord[i][2] = cord[j][2];
				xformNorm[i][X] = xformNorm[j][X];
				xformNorm[i][Y] = xformNorm[j][Y];
				xformNorm[i][Z] = xformNorm[j][Z];
				cord[j][0] = tempX;
				cord[j][1] = tempY;
				cord[j][2] = tempZ;
				xformNorm[j][X] = tempNX;
				xformNorm[j][Y] = tempNY;
				xformNorm[j][Z] = tempNZ;
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
		float tempNX = xformNorm[1][X];
		float tempNY = xformNorm[1][Y];
		float tempNZ = xformNorm[1][Z];
		cord[1][0] = cord[2][0];
		cord[1][1] = cord[2][1];
		cord[1][2] = cord[2][2];
		xformNorm[1][X] = xformNorm[2][X];
		xformNorm[1][Y] = xformNorm[2][Y];
		xformNorm[1][Z] = xformNorm[2][Z];
		cord[2][0] = tempX;
		cord[2][1] = tempY;
		cord[2][2] = tempZ;
		xformNorm[2][X] = tempNX;
		xformNorm[2][Y] = tempNY;
		xformNorm[2][Z] = tempNZ;
		leftEdge++;
	}
	calculateLineEquation(cord[0], cord[1], &A[0], &B[0], &C[0]);
	calculateLineEquation(cord[1], cord[2], &A[1], &B[1], &C[1]);
	calculateLineEquation(cord[2], cord[0], &A[2], &B[2], &C[2]);

	//solve shading equation to get colors for 3 vertices of totalAreangle 
	GzColor col1, col2, col3;
	calculateShadingEquation(render, xformNorm[0], col1);
	calculateShadingEquation(render, xformNorm[1], col2);
	calculateShadingEquation(render, xformNorm[2], col3);

	float totalArea = calculateTraingleArea(cord[0], cord[1], cord[2]);

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
	float r,g,b;
	for(int i = floor(minX) ; i < ceil(maxX) ; i++) {
		for(int j = floor(minY) ; j < ceil(maxY); j++) {
			if((substitute(i, j, A[0], B[0], C[0]) <= 0 && 
				substitute(i, j, A[1], B[1], C[1]) <= 0 &&
				substitute(i, j, A[2], B[2], C[2]) <= 0)){
					z = (interpolateZ(e1, e2, i, j));
					GzCoord position = {i, j, 1};
					if(display->fbuf[ARRAY(i, j)].z == 0 || (z) < (display->fbuf[ARRAY(i, j)].z)) {
						//caclculate relative area of traingle
						float area1 = calculateTraingleArea(cord[1], position, cord[2]);
						float area2 = calculateTraingleArea(cord[0], position, cord[2]);
						float area3 = calculateTraingleArea(cord[0], position, cord[1]);
						if(render->interp_mode == GZ_COLOR) {	
							r = (area1*col1[0] + area2*col2[0] + area3*col3[0]) / totalArea;
						    g = (area1*col1[1] + area2*col2[1] + area3*col3[1]) / totalArea;
							b = (area1*col1[2] + area2*col2[2] + area3*col3[2]) / totalArea;

							if (r > 1.0) r = 1.0;
							if(r < 0) r = 0;
							if (g > 1.0) g = 1.0;
							if(g < 0) g = 0;
							if (b > 1.0) b = 1.0;
							if(b < 0) b = 0;
						}
						else if(render->interp_mode == GZ_NORMAL) {
							GzCoord normal;
							//calculate normal to this pixle
							normal[X] = area1*xformNorm[0][X] + area2*xformNorm[1][X] + area3*xformNorm[2][X];
							normal[Y] = area1*xformNorm[0][Y] + area2*xformNorm[1][Y] + area3*xformNorm[2][Y];
							normal[Z] = area1*xformNorm[0][Z] + area2*xformNorm[1][Z] + area3*xformNorm[2][Z];
							normalizeVector(normal);

							GzColor c;
							calculateShadingEquation(render, normal, c);
							if (c[0] > 1.0) c[0] = 1.0;
							if(c[0] < 0) c[0] = 0;
							if (c[1] > 1.0) c[1] = 1.0;
							if(c[1] < 0) c[1] = 0;
							if (c[2] > 1.0) c[2] = 1.0;
							if(c[2] < 0) c[2] = 0;

							r = c[0];
							g = c[1];
							b = c[2];
						}
						else {
							r = render->flatcolor[0];
							g = render->flatcolor[1];
							b = render->flatcolor[2];
						}
						GzPutDisplay(display, i, j, ctoi(r), ctoi(g), ctoi(b), 1, z);
				}
			cnt++;
			}
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

void normalizeVector(GzCoord v) {
	float factor = sqrt(v[X]*v[X] + v[Y]*v[Y] + v[Z]*v[Z]);
	v[X] /= factor;
	v[Y] /= factor;
	v[Z] /= factor;
}

void calculateShadingEquation(GzRender *render, GzCoord N, GzColor color) {

	/**
	 * shading equation 
	 * C = (Ks sum[le ( R.E)s]) + (Kd sum[le (N.L)] + (Ka la)
	 * C, le, Ks, Kd, Ka, la -- RGB vectors
	 * Reflected ray direction vector (each light source)
	 * E eye ray direction vec
	 * N surface normal
	 * L light ray direction vec
	 **/

	//E- eye vector constant
	GzCoord E;
	E[X] = 0;
	E[Y] = 0;
	E[Z] = -1;

	// calculate N.L and N.E
	float NdotE = calculateDotProduct(N, E);
	GzCoord R[MAX_LIGHTS];

	GzCoord Nin;
	Nin[X] = -N[X];
	Nin[Y] = -N[Y];
	Nin[Z] = -N[Z];

	GzColor termA, termB;
	termA[0] = termA[1] = termA[2] = 0;
	termB[0] = termB[1] = termB[2] = 0;

	for(int i=0 ; i<render->numlights ; i++) {
		float NdotL = calculateDotProduct(N, render->lights[i].direction);
		
		// check if NdotE and NdotL are both of same sign otherwise ingnore
		// calculate R as 2(N.L)N-L
		if(NdotL > 0 && NdotE > 0) {
			R[i][X] = (2*NdotL) * N[X] - render->lights[i].direction[X];
			R[i][Y] = (2*NdotL) * N[Y] - render->lights[i].direction[Y];
			R[i][Z] = (2*NdotL) * N[Z] - render->lights[i].direction[Z];
			normalizeVector(R[i]);

			float RdotE = calculateDotProduct(R[i], E);
			if(RdotE > 1.0)
				RdotE = 1.0;
			else if(RdotE < 0.0)
				RdotE = 0.0;
			termA[0] += render->lights[i].color[0] * pow(RdotE, render->spec);
			termA[1] += render->lights[i].color[1] * pow(RdotE, render->spec);
			termA[2] += render->lights[i].color[2] * pow(RdotE, render->spec);

			//term b
			termB[0] += render->lights[i].color[0] * calculateDotProduct(N, render->lights[i].direction);
			termB[1] += render->lights[i].color[1] * calculateDotProduct(N, render->lights[i].direction);
			termB[2] += render->lights[i].color[2] * calculateDotProduct(N, render->lights[i].direction);

		}
		else if(NdotE < 0 && NdotL < 0) {//negate normal vector
			R[i][X] = (2*NdotL) * (-N[X]) - render->lights[i].direction[X];
			R[i][Y] = (2*NdotL) * (-N[Y]) - render->lights[i].direction[Y];
			R[i][Z] = (2*NdotL) * (-N[Z]) - render->lights[i].direction[Z];
			normalizeVector(R[i]);

			float RdotE = calculateDotProduct(R[i], E);
			if(RdotE > 1.0)
				RdotE = 1.0;
			else if(RdotE < 0.0)
				RdotE = 0.0;
			termA[0] += render->lights[i].color[0] * pow(RdotE, render->spec);
			termA[1] += render->lights[i].color[1] * pow(RdotE, render->spec);
			termA[2] += render->lights[i].color[2] * pow(RdotE, render->spec);

			//term B
			termB[0] += render->lights[i].color[0] * calculateDotProduct(Nin, render->lights[i].direction);
			termB[1] += render->lights[i].color[1] * calculateDotProduct(Nin, render->lights[i].direction);
			termB[2] += render->lights[i].color[2] * calculateDotProduct(Nin, render->lights[i].direction);
		}
	}

	//termA - multiply by Ks
	termA[0] *= render->Ks[0];
	termA[1] *= render->Ks[1];
	termA[2] *= render->Ks[2];

	//termB - multiply by Kd
	termB[0] *= render->Kd[0];
	termB[1] *= render->Kd[1];
	termB[2] *= render->Kd[2];

	//termC -- (Ka la)
	GzColor termC;
	termC[0] = render->Ka[0] * render->ambientlight.color[0];
	termC[1] = render->Ka[1] * render->ambientlight.color[1];
	termC[2] = render->Ka[2] * render->ambientlight.color[2];

	//return color
	color[0] = termA[0] + termB[0] + termC[0];
	color[1] = termA[1] + termB[1] + termC[1];
	color[2] = termA[2] + termB[2] + termC[2];
}

float calculateTraingleArea(GzCoord v0, GzCoord v1, GzCoord v2) {
	return abs(.5 * (v0[X]*v1[Y] + v0[Y]*v2[X] + v1[X]*v2[Y] - v1[Y]*v2[X] - v0[Y]*v1[X] - v0[X]*v2[Y]));
}

float calculateDotProduct(GzCoord v1, GzCoord v2) {
	return v1[X]*v2[X] + v1[Y]*v2[Y] + v1[Z]*v2[Z];
}