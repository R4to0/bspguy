#pragma once

#include "util.h"
#include "bsplimits.h"

#define MAX_SINGLEMAP ((MAX_SURFACE_EXTENT+1)*(MAX_SURFACE_EXTENT+1))
#define MAX_SURFACE_EXTENT  16 // if lightmap extent exceeds 16, the map will not be able to load in 'Software' renderer and HLDS.
#define TEXTURE_STEP        16 // this constant was previously defined in lightmap.cpp. --vluzacn
#define TEX_SPECIAL     1    // sky or slime or null, no lightmap or 256 subdivision

#define assume(exp, message) {if (!(exp)) {printf("\n***** ERROR *****\nAssume '%s' failed\n at %s:%d\n %s\n\n", #exp, __FILE__, __LINE__, message); }}
#define hlassume(exp, message) {if (!(exp)) {printf("\n***** ERROR *****\nAssume '%s' failed\n at %s:%d\n %s\n\n", #exp, __FILE__, __LINE__, #message); }}

#define qmax(a,b)            (((a) > (b)) ? (a) : (b)) // changed 'max' to 'qmax'. --vluzacn
#define qmin(a,b)            (((a) < (b)) ? (a) : (b)) // changed 'min' to 'qmin'. --vluzacn

// HLCSG_HLBSP_DOUBLEPLANE: We could use smaller epsilon for hlcsg and hlbsp (hlcsg and hlbsp use double as vec_t), which will totally eliminate all epsilon errors. But we choose this big epsilon to tolerate the imprecision caused by Hammer. Basically, this is a balance between precision and flexibility.
#define NORMAL_EPSILON   0.00001
#define ON_EPSILON       0.04 // we should ensure that (float)BOGUS_RANGE < (float)(BOGUA_RANGE + 0.2 * ON_EPSILON)

//
// Vector Math
//
#define DotProduct(x,y) ( (x)[0] * (y)[0] + (x)[1] * (y)[1]  +  (x)[2] * (y)[2])
#define CrossProduct(a, b, dest) \
{ \
    (dest)[0] = (a)[1] * (b)[2] - (a)[2] * (b)[1]; \
    (dest)[1] = (a)[2] * (b)[0] - (a)[0] * (b)[2]; \
    (dest)[2] = (a)[0] * (b)[1] - (a)[1] * (b)[0]; \
}
#define VectorSubtract(a,b,c)    { (c)[0]=(a)[0]-(b)[0]; (c)[1]=(a)[1]-(b)[1]; (c)[2]=(a)[2]-(b)[2]; }
#define VectorAdd(a,b,c)         { (c)[0]=(a)[0]+(b)[0]; (c)[1]=(a)[1]+(b)[1]; (c)[2]=(a)[2]+(b)[2]; }
#define VectorScale(a,b,c)       { (c)[0]=(a)[0]*(b);(c)[1]=(a)[1]*(b);(c)[2]=(a)[2]*(b); }
#define VectorCopy(a,b) { (b)[0]=(a)[0]; (b)[1]=(a)[1]; (b)[2]=(a)[2]; }
#define VectorMA(a, scale, b, dest) \
{ \
    (dest)[0] = (a)[0] + (scale) * (b)[0]; \
    (dest)[1] = (a)[1] + (scale) * (b)[1]; \
    (dest)[2] = (a)[2] + (scale) * (b)[2]; \
}

typedef float vec_t;
typedef vec_t  vec3_t[3]; // x,y,z

typedef struct
{
	vec_t v[4][3];
}
matrix_t;

class Winding;

typedef enum
{
	LightOutside,                                          // Not lit
	LightShifted,                                          // used HuntForWorld on 100% dark face
	LightShiftedInside,                                    // moved to neighbhor on 2nd cleanup pass
	LightNormal,                                           // Normally lit with no movement
	LightPulledInside,                                     // Pulled inside by bleed code adjustments
	LightSimpleNudge,                                      // A simple nudge 1/3 or 2/3 towards center along S or T axist
}
light_flag_t;

typedef enum
{
	plane_x = 0,
	plane_y,
	plane_z,
	plane_anyx,
	plane_anyy,
	plane_anyz
}
planetypes;

typedef struct
{
	float           normal[3];
	float           dist;
	planetypes      type;                                  // PLANE_X - PLANE_ANYZ ?remove? trivial to regenerate
}
dplane_t;

typedef struct
{
	float           point[3];
}
dvertex_t;

typedef struct
{
	unsigned short  v[2];                                  // vertex numbers
}
dedge_t;

typedef struct texinfo_s
{
	float           vecs[2][4];                            // [s/t][xyz offset]
	int             miptex;
	int             flags;
}
texinfo_t;

typedef struct
{
	unsigned short	planenum;
	short           side;

	int             firstedge;                             // we must support > 64k edges
	short           numedges;
	short           texinfo;

	// lighting info
	byte            styles[MAXLIGHTMAPS];
	int             lightofs;                              // start of [numstyles*surfsize] samples
}
dface_t;

typedef struct
{
	int             texmins[2], texsize[2];
	int             surfnum;
	dface_t* face;
}
lightinfo_t;

typedef struct
{
	dplane_t planes[4];
}
samplefragrect_t;

typedef struct samplefrag_s
{
	int facenum; // facenum
	samplefragrect_t rect; // original rectangle that forms the boundary
	Winding* mywinding; // relative to the texture coordinate on that face
}
samplefrag_t;

typedef struct
{
	int w, h;
	light_flag_t luxelFlags[MAX_SINGLEMAP];
} lightmap_flags_t;

extern dface_t g_dfaces[MAX_MAP_FACES];
extern vec3_t g_face_offset[MAX_MAP_FACES];
extern dplane_t backplanes[MAX_MAP_PLANES];
extern dplane_t g_dplanes[MAX_MAP_PLANES];
extern texinfo_t g_texinfo[MAX_MAP_TEXINFOS];
extern int g_dsurfedges[MAX_MAP_SURFEDGES];
extern dedge_t g_dedges[MAX_MAP_EDGES];
extern dvertex_t g_dvertexes[MAX_MAP_VERTS];

extern const vec3_t vec3_origin;

class Bsp;

void qrad_init_globals(Bsp* bsp);
lightmap_flags_t qrad_get_lightmap_flags(Bsp* bsp, int faceIdx);

const dplane_t* getPlaneFromFace(const dface_t* const face);

void CalcFaceExtents(lightinfo_t* l);
void CalcPoints(lightinfo_t* l, light_flag_t* LuxelFlags);