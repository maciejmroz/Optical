/*****************************************************************************************
 Life Harmony heghtfield effect
*****************************************************************************************/

#ifndef _LIFE_HARMONY_HEIGHTFIELD_H_
#define _LIFE_HARMONY_HEIGHTFIELD_H_

#include "engine.h"
#include "effect.h"

class LH_Heightfield : public ProceduralObject {
	Texture *tex;
	Texture *envmap;
	float field_size;			//size of the heghtfield
	int div_num;				//number of field divisions
	Image* hmap;				//heightmap;
	float hmap_force_factor;	//how strong the heightmap is
	float gravity;				//constant force to make it look cool
	float spring_factor;		//spring force factor
	float *force_buffer;
	float *velocity_buffer;
	float *vertex_buffer;
	float *texcoord_buffer;
	float *normal_buffer;
	int *index_buffer;
	float last_frame;
	//experimentals
	float hmap_blend_factor;     //[0;1.0f]
	float hmap_strength;
public:
	LH_Heightfield(Texture *texture,Texture *env,Image *heightmap, int divisions,
		float size,float heightmap_force, float gravity_force,
		float spring_force);
	~LH_Heightfield();
	void update(float frame);
	void set_hmap(Image* heightmap);
	void set_hmap_blend(float blend_factor);
	void set_hmap_strength(float strength);
	void render();
};

class LH_UVDistort : public ProceduralObject {
	Texture *tex;
	int div_num;
	float *vertex_buffer;
	float *texcoord_buffer;
	int *index_buffer;
public:
	LH_UVDistort(Texture *texture,int divisions);
	~LH_UVDistort();
	void update(float frame);
	void render();
};

class LH_HeightfieldEffect : public DMEffect {
	Scene3D *scn;
	LH_Heightfield *field;
	LH_UVDistort *uvdistort;
	Image *hmap;
public:
	LH_HeightfieldEffect();
	~LH_HeightfieldEffect();
	void render(int time);
};

class LH_FlyingTexEffect : public DMEffect {  //might be used easier this way
	Texture *napis;
	float s_x,s_y;
	float v_x,v_y;
	float siz_x,siz_y;
	int numsamples;
public:
	LH_FlyingTexEffect(char *filename,int num_samples,
		float start_x,float start_y,float vel_x,float vel_y,
		float size_x,float size_y);
	~LH_FlyingTexEffect();
	void render(int time);
};

class LH_HeightfieldScene : public DMScene {
public:
	LH_HeightfieldScene();
};

#endif
