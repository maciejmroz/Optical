/********************************************************************************************
  Header file for STMesh class and its derivatives - represents parametric meshes
  that can be expressed in terms of two variables (S,T) 0.0<S,T<1.0f
  The class provides easy acces to vertex, normal, tangent and binormal data
  and allows user to evaluate any of these values for arbitrary S,T values
  Handles different render modes than Surface class, just a few predefined ones
  (texture,sphere map,texture+dot3 with specular or without + maybe some more modes -
  see below for details)

  (c) 2001 Maciej Mroz (Frost/Addict)
********************************************************************************************/

#ifndef _ST_MESH_H_
#define	_ST_MESH_H_

#include "glstuff.h"
#include "texture.h"
#include "engine.h"
#include "mathutils.h"

enum ST_MESH_RENDER_MODE {
	ST_RM_MATERIAL_ONLY,
	ST_RM_BASE_TEXTURE,
	ST_RM_SPHERE_MAP,
	ST_RM_DOT3,
	ST_RM_DOT3_SPECULAR,
	ST_RM_DOT3_ARB
};

class STMesh {
	ST_MESH_RENDER_MODE render_mode;
	float *tex_coords;			//base texture coordinates
	float *diffuse_bump_coords;	//cube map coordinates for diffuse
								//term bump mapping pass 
								//(tangent space light vector)
	float *specular_bump_coords;//cube map coordinates for specular
								//term bump mapping pass 
								//(tangent space half angle vector)
	float *bump_coords;
	unsigned short *indices;	//Always triangle strips 
								//(degenerated triangles are used to connect them)
	//NV_vertex_array range stuff
	int is_optimized;
	float *agp_vert_coords;
	float *agp_norm_coords;
	float *agp_tex_coords;
	float *agp_bump_coords;
	float *agp_dif_bump_coords;
	float *agp_spec_bump_coords;
	//textures
	Texture *base_tex;
	Texture *bump_map;			//it is stored as normal map but conversion
								//from height field is performed at load time
	Texture *cube_normalizer;	//per-pixel normalization
								//cube map
	float base_wrap_s,base_wrap_t;
	float bump_wrap_s,bump_wrap_t;
	//other stuff
	int valid_time;				//to avoid (probably costly) unnecessary updates
								//represents last time value used by update
								//(-1 at the beginning)
	int are_tangents_valid;		//It would be silly to recalc vertices when
								//only light position changes
								//so this one is set to 0 by set_light_pos,
								//set_light_pos_indirect,set_eye_pos
								//and set_eye_pos_indirect
	float light_x,light_y,light_z;
	float eye_x,eye_y,eye_z;
	float diffuse_col[4];

	GLuint fragment_shader;

//	virtual void update_binormals();	
	virtual void update_normals();	//it is easier for derived classes
								//to calculate tangent and binormal (simple derivatives)
								//than to bother with normal - and here it is just cross product
								//of the two
	void update_cube_map_coords();
	void base_tex_bump_pass();
	void diffuse_bump_pass();
	void specular_bump_pass();
	void diffuse_bump_pass_arb();
	void dot3_bump_fragment_shader();
protected:
	int size;					//subdivision level
	float *vert_coords;
	float *tangent;
	float *binormal;
	float *norm_coords;
	virtual void update_vertices(int time)=0;
	virtual void update_tangents(int time)=0;
//	virtual void update_normals(int time)=0;
	virtual void update_binormals(int time)=0;
	friend class STMeshMorph;
public:
	STMesh(ST_MESH_RENDER_MODE rm,int subdivisions=50);
	~STMesh();
	virtual void update(int time);
	void set_base_texture(char* filename,float wrap_s=1.0f,float wrap_t=1.0f);
	void set_bump_map(char* filename,float wrap_s=1.0f,float wrap_t=1.0f);
	virtual void render(int time=0);
	void render_base_only();
	void render_diffuse_only();
	void set_light_pos(float x,float y,float z);	//this is light position
								//in object space! It is  up to user to set it up correctly
								//and do proper transform
	void set_light_pos_indirect(Matrix obj_transform,float x,float y,float z);
								//this lets you specify light position in world
								//coordinates
	void set_eye_pos(float x,float y,float z);		//same as above
	void set_eye_pos_indirect(Matrix obj_transform,float x,float y,float z);
	void set_diffuse_color(float r,float g,float b);
};

//derived classes

//just a test class to play with
class STSphere : public STMesh {
	void update_vertices(int time);
	void update_tangents(int time);
	void update_binormals(int time);
	int s_waves,t_waves;
	float ampl;
public:
	void set_num_waves(int s,int t);
	void set_wave_amplitude(float a); //something in (0,1) is reasonable :)))
	STSphere(ST_MESH_RENDER_MODE rm,int subdivisions=50);
	~STSphere() {};
};


class STTorus : public STMesh {
	void update_vertices(int time);
	void update_tangents(int time);
	void update_binormals(int time);
	int r_waves,R_waves;
	float R_ampl,r_ampl;
public:
	void set_num_waves(int R,int r);
	void set_R_wave_amplitude(float R);
	void set_r_wave_amplitude(float r);
	STTorus(ST_MESH_RENDER_MODE rm,int subdivisions=50);
	~STTorus() {};
};

class STMeshMorph : public STMesh {
	void update_vertices(int time);
	void update_tangents(int time);
	void update_binormals(int time);
	Envelope amount;
	VarTable<STMesh*,1> meshes;
public:
	void add_mesh(STMesh *m);
	void set_morph_env(Envelope &env);
	STMeshMorph(ST_MESH_RENDER_MODE rm,int subdivisions=50);
	~STMeshMorph();
};

class STXYQuad : public STMesh {
	void update_vertices(int time) {};
	void update_binormals(int time) {};
	void update_tangents(int time) {};
public:
	STXYQuad(float xsize=1.0f,float ysize=1.0f,
		ST_MESH_RENDER_MODE rm=ST_RM_BASE_TEXTURE,int subdivisions=10);
	~STXYQuad() {};
};

class STHeightMap : public STMesh {
	void update_vertices(int time);
	void update_binormals(int time);
	void update_tangents(int time);
	Image *hmap;
	float xsiz;
	float ysiz;
	float strength;
public:
	void set_hmap(Image *height_map);
	void set_hmap_strength(float s);
	STHeightMap(Image *height_map,float xsize=1.0f,float ysize=1.0f,
		ST_MESH_RENDER_MODE rm=ST_RM_BASE_TEXTURE,int subdivisions=50);
	~STHeightMap() {};
};

#endif