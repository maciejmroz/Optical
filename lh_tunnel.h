/*****************************************************************************************
 Life Harmony tunnel effect
*****************************************************************************************/

#ifndef _LIFE_HARMONY_TUNNEL_H_
#define _LIFE_HARMONY_TUNNEL_H_

#include "effect.h"
#include "particles.h"

/*class Tunnel : public ProceduralObject {
	Texture *tex;
	float radius;
	float length;
	float wave_length;
	float wave_period;
	int radial_segments,length_segments;
	float *vertex_buffer;
	float *texcoord_buffer;
	float *normal_buffer;
	int radial_tiles;
	int length_tiles;
	unsigned int *index_buffer;
public:
	Tunnel(Texture *tx,float rad,float len,
		float wav_len,float wav_period,int radial_seg,int len_seg,
		int tx_radial_tiles=4,int tx_len_tiles=8);
	~Tunnel();
	void set_texture(Texture *tx) {tex=tx;};
	void update(float frame);
	void render();
};*/

class HelixString : public ProceduralObject {
	Texture *tex1;
	float min_rad;
	float max_rad;
	float max_twist;    //liczba zwojow na calej dlugosci
	float twist_period;
	float wave_length;
	float wave_period;
	float length;
	float diam;
	int segments;
	float *vertex_buffer;
	float *texcoord_buffer;
	float *normal_buffer;
public:
	HelixString(Texture *tex,float len,float diameter,float min_radius,float max_radius,
		float wav_len,float wav_period, float maximum_twist,
		float twist_time,int num_segments);
	~HelixString();
	void update(float frame);
	void render();
};

class HelixParticles : public ParticleSystem {
	virtual void calc_motion(float frame);
	float radius;
	float speed;
	float start;
	float angle_offset;
public:
	HelixParticles(Motion mot,Texture *p_tex);
	~HelixParticles() {};
	void set_motion_parameters(float rad,float spd,float start_point,float ang_offset);
};

class LH_TunnelEffect : public DMEffect {
	Scene3D *scn;
	HelixString *tunnel1;
	HelixString *tunnel2;
	Texture *tex,*p_tex,*back_tex,*render_target;
	HelixParticles* p_system1;
	HelixParticles* p_system2;
	HelixParticles* p_system3;
public:
	LH_TunnelEffect();
	~LH_TunnelEffect();
	void render(int time);
};

#endif