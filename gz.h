/*
 * Gz.h - include file for the cs580 rendering library
 */

/*
 * universal constants
 */
#define GZ_SUCCESS      0
#define GZ_FAILURE      1

/*
 * display classes
 */

#define GZ_RGBAZ_DISPLAY        1

/*
 * rendering classes
 */
#define GZ_Z_BUFFER_RENDER      1

/*
 * name list tokens
 */
#define GZ_NULL_TOKEN           0	/* triangle vert attributes */
#define GZ_POSITION             1
#define GZ_NORMAL               2
#define GZ_TEXTURE_INDEX        3

/* renderer-state default pixel color */
#define GZ_RGB_COLOR            99	

#define GZ_INTERPOLATE			95	/* define interpolation mode */
#define GZ_SHADER				96	/* define the shade mode */

#define GZ_DIRECTIONAL_LIGHT		79	/* directional light */
#define GZ_AMBIENT_LIGHT		78	/* ambient light type */

#define GZ_AMBIENT_COEFFICIENT		1001	/* Ka material property */
#define GZ_DIFFUSE_COEFFICIENT		1002	/* Kd material property */
#define GZ_SPECULAR_COEFFICIENT		1003	/* Ks material property */
#define GZ_DISTRIBUTION_COEFFICIENT	1004	/* specular power of material */

#define	GZ_TEXTURE_MAP			1010	/* pointer to texture routine */

/*
 * value-list attributes
 */

/* shade mode flags combine the bit fields below */
#define	GZ_NONE			0	/* flat shading only */
#define	GZ_AMBIENT			1	/* can be selected or not */
#define	GZ_DIFFUSE			2	/* can be selected or not */
#define GZ_SPECULAR			4	/* can be selected or not *

/* select interpolation mode of the shader */
#define GZ_FLAT			0	/* do flat shading with GZ_RBG_COLOR */
#define	GZ_COLOR			1	/* interpolate vertex color */
#define	GZ_NORMALS			2	/* interpolate normals */

typedef int     GzRenderClass;
typedef int     GzDisplayClass;
typedef int     GzToken;
typedef void    *GzPointer;
typedef float   GzColor[3];
typedef short   GzIntensity;	/* 0-4095 in lower 12-bits for RGBA */
typedef float   GzCoord[3];
typedef float   GzTextureIndex[2];
typedef float   GzMatrix[4][4];
typedef int	    GzDepth;		/* signed z for clipping */

typedef	int	(*GzTexture)(float u, float v, GzColor color);	/* pointer to texture sampling method */
/* u,v parameters [0,1] are defined tex_fun(float u, float v, GzColor color) */

/*
 * Gz camera definition
 */
#ifndef GZCAMERA
#define GZCAMERA
typedef struct  GzCamera
{
        GzMatrix        Xiw;  		/* xform from world to image space */
        GzMatrix        Xpi;            /* perspective projection xform */
        GzCoord         position;       /* position of image plane origin */
        GzCoord         lookat;         /* position of look-at-point */
        GzCoord         worldup;        /* world up-vector (almost screen up) */
        float           FOV;            /* horizontal field of view */
} GzCamera;
#endif

#ifndef GZLIGHT
#define GZLIGHT
typedef struct  GzLight
{
        GzCoord        direction; 	/* vector from surface to light */
        GzColor        color;		/* light color intensity */
} GzLight;
#endif

#ifndef GZINPUT
#define GZINPUT
typedef struct  GzInput
{

        GzCoord         rotation;       /* object rotation */
		GzCoord			translation;	/* object translation */
		GzCoord			scale;			/* object scaling */
		GzCamera		camera;			/* camera */
} GzInput;

#endif

#define RED     0               /* array indicies for color vector */
#define GREEN   1
#define BLUE    2

#define X       0               /* array indicies for position vector */
#define Y       1
#define Z       2

#define U       0               /* array indicies for texture coords */
#define V       1

short ctoi(float);
float substitute(int x, int y, float A, float B, float C);
//void calculatePlaneEquation(GzEdge e1, GzEdge e2, float *A, float *B, float *C, float *D); 
void calculateLineEquation(GzCoord pt1, GzCoord pt2, float *A, float *B, float *C);
int sort(GzCoord *cord[3]);