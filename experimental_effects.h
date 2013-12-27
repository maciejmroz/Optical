

#ifndef _EXPERIMENTAL_EFFECTS_H_
#define _EXPERIMENTAL_EFFECTS_H_

#include "engine.h"
#include "effect.h"

class ExpSineEffectDistort : public DMEffect {
	Texture *render_target;
	DMEffect *ef;
	float *vert_coords;
	float *tex_coords;
	float *colors;
	unsigned short *indices;
	int size;
	//nv_vertex_array_range stuff
	float *agp_vert_coords;
	float *agp_tex_coords;
	int is_optimized;

public:
	ExpSineEffectDistort();
	~ExpSineEffectDistort();
	virtual void render(int time);
};

#endif