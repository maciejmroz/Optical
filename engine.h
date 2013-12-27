/*********************************************************************************************
  engine.h
  copyright (c) 2000-2001 Maciej Mroz
*********************************************************************************************/


#ifndef _FROST_3D_ENGINE_V1_H_
#define _FROST_3D_ENGINE_V1_H_

#include "glstuff.h"
#include "glstate.h"
#include "mathutils.h"
#include "texture.h"
#include <stdio.h>
#include "../gucio2/vfs/vfs.h"
#include "debug_msg.h"

//engine globals
extern "C++" int width;  //screen width in pixels
extern "C++" int height;  //screen height in pixels
extern "C++" unsigned int max_lights;  //maximum number of lights allowed
extern "C++" float scn_default_size;
extern "C++" int use_display_lists;
extern "C++" int immediate_mode;

extern "C++" LPVFS vfs; //filesystem interface

//misc utility functions
void DrawQuad(Texture *tex=NULL,float screen_x1=0.0f,float screen_y1=0.0f,
			  float screen_x2=1.0f,float screen_y2=1.0f,
			  float tex_x1=0.0f,float tex_y1=0.0f,float tex_x2=1.0f,float tex_y2=1.0f,
			  int blend=0,GLenum src_blend=GL_SRC_ALPHA,GLenum dst_blend=GL_ONE_MINUS_SRC_ALPHA,
			  int change_col=1);
//if((change_col==1) && (tex!=NULL)) col=(1.0,1.0,1.0,1.0)
unsigned random_int();
unsigned next_int(unsigned int ref);
float random_float();   //<0,1> range
float next_float(float ref);
extern "C++" unsigned int rand_seed;

//TBD: remove_element,clear etc
template<class T,int step> class VarTable {
	T* tbl;
	int size;
public:
	int length;
	VarTable();
	~VarTable();
	void add_element(T elem);
	void remove_element();
	void clear();
	T operator[](int ind);
};

template<class T, int step> VarTable<T,step>::VarTable() {
	length=0;
	tbl=NULL;
	size=0;
}

template<class T,int step> VarTable<T,step>::~VarTable() {
	if(tbl) delete tbl;
}

template<class T,int step> void VarTable<T,step>::add_element(T elem) {
	if(length==size) {
		if(tbl==NULL) {
			tbl=new T[step];
			size=step;
			tbl[length++]=elem;
		}
		else {
			T* tmp;
			tmp=new T[size+step];
			int i;
			for(i=0;i<length;i++) {
				tmp[i]=tbl[i];
			}
			size+=step;
			delete tbl;
			tbl=tmp;
			tbl[length++]=elem;
		}
	}
	else {
		tbl[length++]=elem;
	}
}

template<class T,int step> T VarTable<T,step>::operator[](int ind) {
	if((ind<0) || (ind>=length)) {
		PrintfBox("Array index out of bouds!!\n");
		return (T)0;
	}
	return tbl[ind];
}

template<class T, int step> void VarTable<T,step>::remove_element() {
	if(length>0) length--;
}


template<class T, int step> void VarTable<T,step>::clear() {
	length=0;
}

class Surface;
class Object3D;
class Scene3D;
class ObjectManager;
class ProceduralObject;
class MovingObject3D;

extern ObjectManager obj_manager;
extern Scene3D* current_scene;				//global to access scene being rendered
											//oc for ProceduralObject methods this one is
											//::current_scene
extern MovingObject3D* current_object;		//needed to access transform data from
											//Object3D		



/**********************
 Keyframing&Motion
 LightWave3D-style motion structures
 some are based on Newtek's motion code
 some are just inspired by that code
 LW compatible anyway
**********************/

#define NUM_CHAN	 9
typedef float		 ChanVec[NUM_CHAN];

//acces to KeyFrame is limited to Motion class
class KeyFrame {
	ChanVec		 cv;
	double		 tens, cont, bias;
	int		 linear;
	int		 step;
	void operator=(const KeyFrame& key);  //very limited =
	friend class Motion;
};

class Motion {
	KeyFrame	*keylist;
	int	keys; //number of keys
	int steps;
	int num_allocated_frames;
	int end_behavior;
public:
	ChanVec cv;
	Motion();
	~Motion();
	void clear();
	void add_key_frame(ChanVec c_vec,double tens,double cont,double bias,
		int linear,int step);
	void alloc_keyframe_mem(int numframes);
	void calc(double step);
	void set_end_behavior(int eb);
	Motion& operator=(const Motion &mot);
	Motion(const Motion&);
	Motion(const char* file);  //from .mot file
};

//access to EnvFrame is limited to Envelope class
class EnvFrame {
	double tens,cont,bias;
	int linear;
	float value;
	int step;
	void operator=(const EnvFrame& efr);  //very limited =
	friend class Envelope;
public:
	~EnvFrame() {};
};

class Envelope {
	EnvFrame** keylist;
	int keys,steps;
	int end_behavior;
	int num_allocated_frames;
public:
	float res;    //envelope interpolation result
	Envelope();
	~Envelope();
	Envelope(float val);
	void add_key_frame(float value,double tens,double cont,double bias,
		int linear,int step);
	void operator=(const Envelope& env);   //quite limited = (self modifying)
	void alloc_keyframe_mem(int numframes);
	void calc(double step);
	void set_end_behavior(int eb);
	void clear();
	void dump();
};

/*************
 Mesh, Object3D and helper classes
*************/

#define NUM_TEXTURES 8

class BBox {
public:
	Point3 p1,p2,p3,p4,p5,p6,
		p7,p8,p_min,p_max;
};

// different textures
enum {
	BASE,			//aka color
	SPECULAR,		//additive spherical
	LUMINOSITY,		//additive
	DIFFUSE,		//detail
	BUMP,
	REFLECTION, 
	GLOSS,
	LIGHTMAP  
};

enum MESH_RENDER_MODE {
	RM_BASE_TEXTURE_BIT=0x1,
	RM_LIGHTMAP_TEXTURE_BIT=0x2,
	RM_SPECULAR_TEXTURE_BIT=0x4,
	RM_LUMINOSITY_TEXTURE_BIT=0x8,
	RM_DIFFUSE_TEXTURE_BIT=0x10,
	RM_BUMP_TEXTURE_BIT=0x20,
	RM_REFLECTION_TEXTURE_BIT=0x40,
	RM_GLOSS_TEXTURE_BIT=0x80,
	RM_DOUBLE_SIDED_BIT=0x100,
	RM_TRANSPARENT_BIT=0x200,
	RM_ALPHA_TEST_BIT=0x400,
	RM_ADDITIVE_BIT=0x800
};

//LightWave 3D texture paremeters
class tex_parameters {
public:
	char* type;
	char* timg;
	float tsiz[3],tctr[3],tfal[3],tvel[3];
	unsigned short int flags;
	float widthTiling,heightTiling;
	unsigned short width_wrap,height_wrap;
	tex_parameters();
	~tex_parameters();
};

//Vertex buffer class - could be struct, but .....
class VertexBuffer {
public:
	float *vert_coords;
	unsigned int num_verts;
	float* norm_coords;
	float* tex_coords[NUM_TEXTURES];
	unsigned short* indices;		//Geforce needs it to be short
									//TBD: #typedef index type?
									//will probably be uint on future hardware
	unsigned int min_ind,max_ind;	//for LockArraysEXT()
	unsigned int num_tris;
	//NV_vertex_array_renge stuff 
//	int are_vertices_external;      //prohibits delete calls on AGP mem pointers
	int is_external;
//	int are_normals_external;
//	int are_tcoords_external;
	//and some bonus functionality to ease coding
	void optimize();				//push to AGP mem if possible
	void init();
	void clean();
	VertexBuffer() { init();};
	~VertexBuffer() { clean();};
};

enum VS_MODIFY_FLAGS {         //some constants for vertex shader stack
	VS_VERTICES=0x1,
	VS_NORMALS=0x2,
	VS_BASE_TEXCOORS=0x4,
	VS_LUMINOSITY_TEXCOORDS=0x8,
	VS_LIGHTMAP_TEXCOORDS=0x10,
	VS_SPECULAR_TEXCOORDS=0x20,
	VS_DIFFUSE_TEXCOORDS=0x40,
	VS_BUMP_TEXCOORDS=0x80,
	VS_REFLECTION_TEXCOORDS=0x100,
	VS_GLOSS_TEXCOORDS=0x200
};

//VertexShader class - much more powerful than DX8 VS but software only
class VertexShader {
protected:
	VertexBuffer *in,*out;
	VertexShader *next;
public:
	VertexShader(VertexBuffer *in_buf,VertexBuffer *out_buf);
	~VertexShader() { delete next;};			//should be virtual?????
	void attach_vshader(VertexShader *vshader);	//all shaders should use the same in/out
												//buffers but it is not forced in any way
	virtual unsigned int vshade(unsigned int flags,float frame=0.0f)=0;
};

class MorphShader : public VertexShader {
	Envelope am;
	VertexBuffer *target;
public:
	MorphShader(VertexBuffer *in_buf,VertexBuffer *out_buf,
		Envelope* amount,VertexBuffer *morph_target);
	unsigned int vshade(unsigned int flags,float frame=0.0f);
};

/*****************************************************
 simple skeletal deformation system
 no IK (yet)
*****************************************************/


class Bone {
	float *weights;           //vertex weight table
	int num_weights;          //seems neccesary
	char* name;
	int active;
	Point3 rest_position;
	float rest_direction[3];  //heading,pitch,bank
	Matrix rest_matrix;
	Matrix transform_mtx;     //parent_transform*own_transform
	float rest_length;
	int scale_strength;
	float strength;
	Motion motion;
	int parent_bone;
	friend class Skeleton;
	friend class Scene3D;
public:
	Bone();
	Bone(const Bone& bn);
	void operator=(const Bone& bn);
	~Bone();
};

class Skeleton : public VertexShader {
	VarTable<Bone*,1> bone_tbl;
	float *vertex_accum;
public:
	Skeleton(VertexBuffer *in_buf,VertexBuffer *out_buf,VarTable<Bone*,1> *bone_set);
	~Skeleton();
	unsigned int vshade(unsigned int flags,float frame=0.0f);
};

/****************************************************
 Surface class - simplest renderable primitive
 exact reproduction of LW surfaces
****************************************************/

class Surface {
	void copy_attributes(Surface* dest,const Surface* src);
protected:
	VertexShader* mod_stack;
	VertexBuffer *vb,*mod_vb;
//	int is_optimized;
//	GLuint call_list;

	//for LW particle systems
	int is_particle_system;
	float *particle_coords;					//these are regenerated from vert coords
	float particle_size;

	Texture* textures[NUM_TEXTURES];
	float tex_velocities[3*NUM_TEXTURES];	//3D textures should also be supported
	float amb[4];							//material
	float dif[4];
	float spec[4];
	float col[4];
	float emission[4];
	float reflectivity;
	float alpha_ref;
	unsigned int render_flags;
	Object3D *parent;
	friend class Object3D;
	void render_particles();
public:
	Surface** split(int &out_num);
	void optimize();
	Surface(Object3D *p);
	~Surface();
	void add_texture(Texture* texture,int texture_type=BASE);
	void attach_mod(VertexShader *m);
	VertexBuffer* get_vb() { return vb;};
	VertexBuffer* get_mod_vb() { return mod_vb;};
	void draw_very_simple();				// :))) - just a little hack
	void draw_simple();
	void render(float frame=0.0f);
	void generate_normals();
	int generate_tex_coords(const tex_parameters* t_desc,int texture_type=BASE);
	void dump(const char *filename="debug.log");
	void dump_to_framelog();				//shorter version with DebugFrameMessage
};

//object file types - for now LWO 5.x only
enum {
	LWO_OBJECT
};

class Object3D {
	int process_POLS(int f,int chunk_length,float *vert_coords);
	unsigned int num_meshes;
	Surface** meshes;  //dynamic pointer table -  TBD: template
	BBox bbox;
	friend class Surface;
public:
	char *name;
	Object3D();
	Object3D(const char* filename,int type=LWO_OBJECT);
	~Object3D();
	void load_LWO_file(const char* filename);
	void align_to_box(float size);
	int has_uniform_flags();
	int has_transparent_surfaces();
	int has_additive_surfaces();
	unsigned int get_render_flags();
	void render(float frame=0.0f);
	void render_solid_surfaces(float frame=0.0f);
	void render_additive_surfaces(float frame=0.0f);
	void render_transparent_surfaces(float frame=0.0f);
	void dump(const char *filename="debug.log");
	void split_sufaces();	//should be called BEFORE optimize() (if called at all)
	void optimize();
	unsigned int get_num_surfaces() {return num_meshes;};
	Surface* get_surface(unsigned int i)
	{ if((i<num_meshes) && (i>=0)) return meshes[i]; else return NULL;};
	BBox get_bbox() { return bbox; };
};

//object management class
//all construction/destruction is hidden here and should not take
//place anywhere else
class ObjectManager {
	VarTable<Object3D*,1> static_objects;
	VarTable<Object3D*,1> dynamic_objects;
public:
	ObjectManager();
	~ObjectManager();
	Object3D* get_object_dynamic(const char *filename); //always return new object
	Object3D* get_object_static(const char *filename);  //return existing object if possible
};

/**************
 Light class
**************/

class Light {
protected:
	GLenum light_id;
	ChanVec res;					//position,rotation and scale as in LW 3D
	char* name;
	float color[4];
	int light_type;
	Light();
	void set_light_id(GLenum id);
public:
	Light(GLenum id);
	void set_color(float col_r,float col_g,float col_b);
	void set_name(const char *s);
	void set_type(int t);			//spot, omni etc
	void set_position(float x,float y,float z);
	void set_rotation(float rx,float ry,float rz);
	void draw(float frame=0.0f);
};


/*****************
 Camera class
*****************/

class Camera {
protected:
double frustum_l,frustum_r,frustum_t,frustum_b;
double near_clip,far_clip;
ChanVec res;						//position,rotation and scale
public:
Camera();
~Camera();
void set_frustum(double l,double r,double b,double t,
                 double n,double f);
void set_origin(Point3 o);
void set_origin(float xp,float yp, float zp);
void set_rotatex(float ang);
void set_rotatey(float ang);
void set_rotatez(float ang);
void update_frustum();
void apply_transform(void);
void get_origin(Point3 pos);
void get_origin(float *xp,float *yp,float *zp);
void draw(float frame=0);
};


/*********************
 Scene3D class with helpers:
 MovingObject, MovingCamera,Moving Light etc
*********************/

class MovingObject3D {
	Object3D* obj;
	Motion motion;
	ChanVec res;					//motion interp. result
	Point3 pivot;
	int target_object;
	int parent_object;
	Envelope metamorph;
	int morph_target;
	int morph_surfaces;
	int is_dynamic;					//morphing/morph target/skeletal deform??????
	char* name;
	Matrix transform_mtx;
	MovingObject3D** obj_table;		//needed for parent/target stuff
	void load_object();				//accessible to Scene3D only
	friend class Scene3D;
public:
	MovingObject3D();
	MovingObject3D(const char* filename,int type=LWO_OBJECT);
	~MovingObject3D();
	void set_pivot_point(Point3 pv);
	void get_pivot_point(Point3 pv);
	void calc_transform(float frame=0.0f);
	void get_transform(Matrix mx);
	void apply_transform();
	void get_position(Point3 pos);
	void get_transformed_bbox(BBox *bx);
	void get_trn_bbox_center(Point3 pt);
	void render(float frame=0.0f);
	void render_solid_surfaces(float frame=0.0f);
	void render_additive_surfaces(float frame=0.0f);
	void render_transparent_surfaces(float frame=0.0f);
	void attach_object(Object3D* object);
	void setup_morphing();
};

class MovingCamera : public Camera {
	Motion motion;
	int target_object;								//target object index, 0=no target
	int parent_object;
	Envelope zoom_factor;
	Matrix transform_mtx;
	MovingObject3D** obj_table;						//needed for parent/target stuff
	void calc_transform(float frame=0);
	void apply_transform();
	void get_transform(Matrix mx);
	void get_camera_matrix(Matrix mx);
	void get_frustum_matrix(Matrix mx);
	void get_transformed_frustum(float left[4],
		float right[4], float top[4], float bottom[4],
		float near_p[4], float far_p[4]);			//plane equations
	friend class Scene3D;
public:
	MovingCamera();
	void draw(float frame=0);
	void get_position(Point3 pos);
	void get_direction(Vector3 dir);
	void get_up_vector(Vector3 up);
	float get_near_clip();
	float get_far_clip();
};

class MovingLight : public Light {
	Motion motion;
	int parent_object;
	int target_object;
	Matrix transform_mtx;
	MovingObject3D** obj_table;			//needed for parent/target stuff
	void calc_transform(float frame=0);
	void apply_transform();
	void get_transform(Matrix mx);
	Envelope light_intensity;
	Envelope falloff;
	Envelope cone_angle;
	Envelope edge_angle;
	int lens_flare;
	Envelope flare_intensity;
	float appear_intensity;				//for smooth appear/disappear of flares
										//own invention inspired by Unreal engine
	float last_vis_check;
	int flare_options;
	friend class Scene3D;
public:
	MovingLight(GLenum id);
	void draw(float frame=0);
	void get_position(Point3 pos);
	void get_color(float* col_r,float* col_g,float* col_b,float* col_a);
	void set_cone_angle(Envelope env);
	void set_edge_angle(Envelope env);
	void set_falloff(Envelope env);
	void set_light_intensity(Envelope env);
	void set_flare_type(int t);
	void set_flare_options(int t);
	void set_flare_intensity(Envelope env);
};

class SceneRenderFX {
public:
	int shake_camera;
	Envelope shake_amount;
	int fuzzy_camera;		//0=disabled,1=vertical,2=horizontal
	int blinding_flares;	//0=disabled,1=additive,2=(src,1-src)
	SceneRenderFX();
};

//LWS wrapper (for now)
class Scene3D {
	int first_frame;
	int last_frame;
	float frames_per_second;
	float ambient[4];				//alpha is also needed
	Envelope ambient_intensity;
	Envelope global_flare_intensity;
	float flare_falloff_time;
	int enable_lens_flare;
	MovingObject3D** obj_table;
	ProceduralObject** solid_procedurals;
	ProceduralObject** transparent_procedurals;
	unsigned int num_objects;
	unsigned int num_solid_procedurals;
	unsigned int num_transparent_procedurals;
	float scene_size_factor;
	MovingCamera* cam;
	//typically there can be only 8 lights in the scene, but we might want to use extra lights for
	//other purposes - and we are prepared for future GL implementations
	MovingLight* lights[32];  
	unsigned int num_lights;
	int fog_type;
	Envelope fog_start;
	Envelope fog_end;
	float fog_color[4];
	float backdrop_color[4];
	int backdrop_fog;
	Texture *flare_tex[5];
	float frame;					//current frame
	void render_flares();
	int num_transparent_objects;
	int num_additive_objects;
	float *bbox_centers;
	int *is_transparent;
	int *is_additive;
	int *transparent_order;
	SceneRenderFX *rfx;
	friend int cmp_centers(const void* c1,const void* c2);
	void optimize();				//convert to display lists whatever is possible
	friend class ProceduralObject;
public:
	Scene3D(float size=1.0f);
	Scene3D(const char* filename,float size=1.0f);
	~Scene3D();
	void set_global_flare_intensity(Envelope env);
	void set_ambient_color(float amb_r,float amb_g,float amb_b);
	void set_ambient_intensity(Envelope env);
	void enable_flare(void);
	void disable_flare(void);
	void load_LWS_file(const char* filename);
	void add_object(MovingObject3D* obj);
	void attach_procedural_object(ProceduralObject* obj,int stage=0);
	void set_camera(MovingCamera* camera);
	void set_render_fx(SceneRenderFX *render_fx);
	void render(int time=0);
	MovingCamera* get_camera() {return cam;};
	unsigned int get_num_objects() {return num_objects;};
	MovingObject3D* get_object(unsigned int i) 
	{ if((i<num_objects) && (i>=0)) return obj_table[i]; else return NULL;};
	float get_size_factor() {return scene_size_factor;};
	float get_frame() {return frame;};
};

/*******************
 ProceduralObject class
*******************/
class ProceduralObject {
protected:
	Scene3D* current_scene;
	MovingCamera* get_scene_camera() { return current_scene->cam;};
public:
	ProceduralObject() {current_scene=NULL;};
	virtual void update(float frame) {};
	virtual void render() {};
	virtual void get_bounding_box(BBox* bx);
	void set_current_scene(Scene3D* scn) {current_scene=scn;};
	virtual void render(float frame) {update(frame);render();};
};

#endif