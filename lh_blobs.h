/*****************************************************************************************
 Life Harmony blobs effect
*****************************************************************************************/

#ifndef _LIFE_HARMONY_BLOB_EFFECT_
#define _LIFE_HARMONY_BLOB_EFFECT_

#include "effect.h"
#include "field.h"
#include "texture.h"

class LH_Blob1 : public ScField {
	float *quad_vertex_buffer;
	unsigned int num_quads;
	float *quad_color_buffer;
	//float *quad_texcoord_buffer; ????????????????????????
	unsigned int quad_table_size;
	Texture *tex;
public:
	LH_Blob1(float mini_x,float maxi_x,float mini_y,float maxi_y,
		float mini_z,float maxi_z,int res=16,Texture *tx=NULL);
	~LH_Blob1();
	void update(float frame);
	void render();
};

class LH_BlobEffect1:public DMEffect {
	LH_Blob1* blob;
	Texture *tex,*tex2;
public:
	LH_BlobEffect1();
	~LH_BlobEffect1();
	void render(int time);
};

#endif