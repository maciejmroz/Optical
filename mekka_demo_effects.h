/************************************************************************
 Header file with effect classes for Addict Mekka Symposium 2001 demo
 copyleft 2001 Maciej Mroz (Frost/Addict)
 ************************************************************************/

#ifndef _MS2001_EFFECTS_
#define _MS2001_EFFECTS_

#include "effect.h"
#include "field.h"
#include "texture.h"
#include "engine.h"
#include "st_mesh.h"

class Mekka_Blobs : public ScField {
	float *quad_vertex_buffer;
	unsigned int num_quads;
	float *quad_color_buffer;
	unsigned int quad_table_size;
	Texture *tex;
public:
	Mekka_Blobs(float mini_x,float maxi_x,float mini_y,float maxi_y,
		float mini_z,float maxi_z,int res=16,Texture *tx=NULL);
	~Mekka_Blobs();
	void update(float frame);
	void render();
};

class Mekka_BlobsEffect1:public DMEffect {
	Mekka_Blobs* blob;
	Texture *tex,*tex2;
	Scene3D *scn;
public:
	Mekka_BlobsEffect1();
	~Mekka_BlobsEffect1();
	void render(int time);
};

class Mekka_HelixWall {
	float *vert_coords;
	float *norm_coords;
	float *agp_vert_coords;
	float *agp_norm_coords;
	unsigned short* indices;
	int num_verts;
	float r;
	float h;
	float twists;
	int is_optimized;
public:
	Mekka_HelixWall(float rad=0.5f,int num_twists=5,float height=1.0f);
	~Mekka_HelixWall();
	void render(int time=0);
};

class Mekka_Scenka1 : public DMEffect {
	Scene3D *scn;
	Scene3D *napisy;
	STMeshMorph *mesh;
	STSphere *sphere;
	STTorus *torus;
	Mekka_HelixWall* heliksy;
public:
	Mekka_Scenka1();
	~Mekka_Scenka1();
	void render(int time);
};

class Mekka_Scene_BlurOut : public DMEffect {
	DMEffect *ef;
	int offset;
	int blur_length;
public:
	Mekka_Scene_BlurOut(DMEffect *blur_target,int ef_offset,int ef_blur_length);
	~Mekka_Scene_BlurOut();
	void render(int time);
};

class Mekka_AudioHrzBlur : public DMEffect {
	DMEffect *ef;
	int offset;
public:
	Mekka_AudioHrzBlur(DMEffect *blur_target,int ef_offset);
	~Mekka_AudioHrzBlur();
	void render(int time);
};

class Mekka_AudioImgFlash : public DMEffect {
	float len;
	Texture *tex;
public:
	Mekka_AudioImgFlash(char* filename,float length);
	virtual void render(int time);
};

class Mekka_SineImgFlash : public DMEffect {
	Texture *tex;
	float len;
	float offset;
	float scale;
public:
	Mekka_SineImgFlash(char *filename,float length,float ofs,float scl);
	virtual void render(int time);
};

class Mekka_CreditsImg : public DMEffect {
	Texture *tex;
public:
	Mekka_CreditsImg();
	virtual void render(int time);
};


class Mekka_MultiSceneBlur : public DMEffect {
	VarTable<DMEffect*,1> effects;
	VarTable<int,1> lengths;
	VarTable<int,1> start_times;
	VarTable<int,1> end_times;
	int last_render;
	Texture *tex;
	Texture *tex_prev;
	Texture *additionalRT;
public:
	void add_effect(DMEffect* ef,int start_tm,int end_tm,int len);
	Mekka_MultiSceneBlur();
	~Mekka_MultiSceneBlur();
	void render(int time);
};

class Mekka_SceneShift : public DMEffect {
	DMEffect *ef;
	int normal_period;
	int shifted_period;
	float shift_amount;
public:
	Mekka_SceneShift(DMEffect *effect,int normal_period_p,int shifted_period_p,
		float shift_amount_p);
	void render(int time);
};

class Mekka_SceneGlow : public DMEffect {
	Scene3D *glow_target;
	Scene3D *background;
	Texture *render_target;
public:
	Mekka_SceneGlow();
	~Mekka_SceneGlow();
	void render(int time);
};

class Mekka_SineDistort : public DMEffect {
	Texture *tex;
	float *vert_coords;
	float *tex_coords;
	float *colors;
	unsigned short *indices;
	int size;
	//nv_vertex_array_range stuff
	float *agp_vert_coords;
	float *agp_tex_coords;
	float *agp_colors;
	int is_optimized;
public:
	Mekka_SineDistort(char *texture,int subdivisions=30);
	~Mekka_SineDistort();
	void render(int time);
};

class Mekka_SineDistort2 : public DMEffect {
	Texture *tex;
	float *vert_coords;
	float *tex_coords;
	float *colors;
	unsigned short *indices;
	int size;
	//nv_vertex_array_range stuff
	float *agp_vert_coords;
	float *agp_tex_coords;
	float *agp_colors;
	int is_optimized;
public:
	Mekka_SineDistort2(char *texture,int subdivisions=30);
	~Mekka_SineDistort2();
	void render(int time);
};


class Mekka_UVDistort : public ProceduralObject {
	Texture *tex;
	int div_num;
	float *vertex_buffer;
	float *texcoord_buffer;
	unsigned short *index_buffer;

	//nv_vertex_array_range stuff
	int is_optimized;
	float *agp_vertex_buffer;
	float *agp_texcoord_buffer;
public:
	Mekka_UVDistort(Texture *texture,int divisions);
	~Mekka_UVDistort();
	void update(float frame);
	void render();
};


class Mekka_2DEffect1 : public DMEffect {
	Mekka_SineDistort *uvd1;
	Mekka_SineDistort *uvd2;
	Mekka_SineDistort *uvd3;
	Mekka_SineDistort *uvd4;
	Mekka_UVDistort *rings;
public:
	Mekka_2DEffect1();
	~Mekka_2DEffect1();
	void render(int time);
};

class Mekka_FinalScene : public DMEffect {
	STHeightMap *hmap;
	STXYQuad *background;
	Image *cool_writings[3];
	Scene3D *scn;
public:
	Mekka_FinalScene();
	~Mekka_FinalScene();
	void render(int time);
};


//unused

class Mekka_BlurredTorus : public DMEffect {
	Scene3D *scn;
	Scene3D *background;
public:
	Mekka_BlurredTorus();
	~Mekka_BlurredTorus();
	void render(int time);
};

#endif