
#include "veclib.h"

typedef struct Colour {
  float r,g,b;
} Colour;

typedef struct {
  float c,m,y;
} Colour2;

typedef struct LinePrim {	/* Line Segment primitive */
  int type;			/* linetype (solid, dashed,..) */
  Colour col;			/* rgb colour */
  Point3 from;
  Point3 to;
} LinePrim;

typedef struct TextPrim {	/* Text string Primitive */
  float size;			/* size of lettering */
  Colour col;			/* rgb colour */
  char *s;			/* Pointer to the string */
  Point3 where;			/* Location of the string */
} TextPrim;

typedef struct PolyPrim {	/* Polygon primitive */
  int nvert;			/* number of vertices */
  Colour col;			/* rgb colour */
  union {
    Vector3 vec;		/* polygon normal */
    double abcd[4];		/* 4 coefficients */
  } normal;
  Point3 centre;		/* Centre of polygon */
  Point3 *data;			/* variable array of vertices */
} PolyPrim;

typedef struct Primitive {	/* General Primitive structure */
  struct Primitive *next;	/* pointer to next primitive */
  float zmin;			/* front part of primitive */
  int type;			/* what type of primitive? */
  void *data;			/* pointer to the data */
} Primitive;


typedef struct ObjViewList {	/* General Object view parameter structure */
  struct ObjViewList *next;	/* pointer to next primitive */
  int type;			/* what type of primitive? */
  union {
    Point3 vrp;			/* 1. View Reference Point */
    Vector3 vpn;		/* 2. View plane normal */ 
    Vector3 dop;		/* 3. Direction of projection (parallel) */
    Point3 cop;			/* 4. Center of projection (perspective) */
    Vector3 vup;		/* 5. View up direction */
    Box2 window;		/* 6. Corners of the view plane window */
    Box3 viewport;	 	/* 7. Corners of the 3D viewport */
    Box3 device;		/* 8. Window in physical device coordinates. */
    int ViewMode;		/* 9. Viewing mode (parallel or perspective) */
    int ClipFlag;		/* 10. Clip flag */
    Point2 Clip;		/* 11. Front and back clipping limits */
  } data;
} ObjViewList;

typedef struct {
  Point3 vrp;			/* View Reference Point */
  Vector3 vpn;			/* View plane normal  */
  Vector3 dop;			/* direction of projection (parallel) */
  Point3 cop;			/* center of projection (perspective) */
  Vector3 vup;			/* view up direction */
  Box2 window;			/* corners of the view plane window */
  Box3 viewport;		/* corners of the 3D viewport */
  Box3 device;			/* Window in physical device coordinates */
  int ViewMode;			/* viewing mode (parallel or perspective) */
  int ClipFlag;			/* Clip flag */
  Vector2 Clip;			/* Front and back clipping limits */
  double PerZmin;		/* Z values of front clipping plane */
} ViewPar;

typedef struct {
  Matrix4 Np;			/* Final perspective transformation */
  Matrix4 Ip;			/* Unit viewing volume to NDC transf. */
  Matrix4 N2D;			/* NDC to physical device transformation */
  int ClipFlag;			/* Clip flag */
  double PerZmin;		/* Z values of front clipping plane */
} View;

typedef struct Object {		/* General object structure */
  struct Object *next;		/* pointer to next object */
  char *name;			/* name of object */
  char visible;			/* is object visible? */
  int type;			/* what type of object? */
  struct GraphicsContext *GC;	/* The current graphics context */
  ViewPar viewpar;		/* The current viewing parameters */
  ObjViewList *viewlist;	/* view transformation to apply to coords */
  struct Primitive *prim_head;	/* pointer to first primitive */
  struct Primitive *prim_tail;	/* pointer to last primitive */
  struct Instance *inst_head;	/* pointer to list of instances */
  struct Instance *inst_tail;	/* pointer to last instance in list */
} Object;

typedef struct Instance {	/* Instance transformation structure */
  struct Instance *next;	/* next in list */
  Matrix4 *trf;			/* transformation for this object*/
  struct GraphicsContext *GC;	/* The current graphics context */
  ViewPar viewpar;		/* The current viewing parameters */
  Object *obj;			/* pointer to the object */
} Instance;

typedef struct {
  double ambient;		/* Ambient coefficient */
  double diffuse;		/* Diffuse coefficient */
  double specular;		/* Specular coefficient */
  double pow;			/* Phong shading power. */
} Surface;

typedef struct {
  int type;			/* Type of light (amb, dir, pos, spot)*/
  Colour col;			/* Colour of light */
  Vector3 position;		/* Position of light */
  float intensity;
  float atten[2];		/* attenuation coefficients */
  float exp, angle;		/* concentration, spread */
} LightSource;

typedef struct GraphicsContext { /* Define a type to keep the graphics env. */
  int linetype;			/* linetype (1=solid, etc.) */
  float linewidth;		/* linewidth in points */
  float charsize;		/* size of characters */
  Colour colour;		/* the current colour */
  int zthick;			/* Z-thickening */
  Surface surf;			/* Current surface */
} GraphicsContext;

#define LINESEGMENT 1
#define TEXTSEGMENT 2
#define POLYGON 3
#define SPHERE 4
#define DIRECTIONAL 1
#define POSITIONAL 2
#define SPOT 3

/*
  Local variables:
  mode: font-lock
  End:
*/

