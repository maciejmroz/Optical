/**********************************************************************************************

  Volume fog done because I didn't want to learn for my classes and I had do to
  something

***********************************************************************************************/


#ifndef _LH_VOLUME_FOG_H_
#define _LH_VOLUME_FOG_H_

#include "effect.h"
#include "texture.h"
#include "engine.h"

class VolumeParticles : public ProceduralObject {
	Texture *tex;
	float *positions;
	//render buffers
	float *vertex_buffer;
	float *color_buffer;
	float *texcoord_buffer;
	float size;
	int num_particles;
public:
	VolumeParticles(Texture *p_tex,int numparticles,float p_size=0.5f,
		float min_x=-4.0f,float max_x=4.0f,
		float min_y=-4.0f,float max_y=4.0f,
		float min_z=0.0f,float max_z=0.1f);
	~VolumeParticles();
	void update(float frame=0.0f);
	void render();
};

class LH_VolumeFogEffect : public DMEffect {
	Scene3D *scn;
	VolumeParticles* p_system1;
public:
	LH_VolumeFogEffect();
	~LH_VolumeFogEffect();
	void render(int time);
};

#endif