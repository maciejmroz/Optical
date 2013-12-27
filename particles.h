/***********************************************************************************************
 Header file for particle systems
***********************************************************************************************/

#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "engine.h"

class ParticleSystem : public ProceduralObject {
	float appear_time;
	float disappear_time;
	float life_time;
	float emission_intensity;  //particles/sec - should be Envelope????
	Envelope max_velocity;
	Envelope min_velocity;
	Envelope min_size;
	Envelope max_size;
	Envelope spread;  // (spread,spread,spread) vector added to velocity - random!
	float gravity[3];
	float color[3];
	void alloc_mem(int size);
	int num_particles;

	Motion emitter_mot;
	// (max_particles=emission*life_time)
	float last_update;
	float last_emission;
	float last_rand;
	float *sizes;
	float *velocities;
	float *colors;           //needed for appear/disappear
	float *ages;
	float *positions;
	Texture *tex;
	//render buffers
	float *vertex_buffer;
	float *color_buffer;
	float *texcoord_buffer;
	int current_size;
protected:
	float e_x,e_y,e_z;
	virtual void calc_motion(float frame);
public:
	ParticleSystem(Motion mot,Texture *p_tex);
	~ParticleSystem();
	void set_gravity(float gx,float gy, float gz);
	void set_velocities(Envelope min,Envelope max);
	void set_sizes(Envelope min,Envelope max);
	void set_times(float appear,float disappear, float life);
	void set_spread(Envelope spd);
	void set_emission(float emission);
	void set_color(float r,float g,float b);
	virtual void render();
	virtual void update(float frame);
};

#endif