/***********************************************************************************************
  misc utility functions
  (c) 2000 Maciej Mroz
***********************************************************************************************/

#include "engine.h"

void DrawQuad(Texture *tex,float screen_x1,float screen_y1,
			  float screen_x2,float screen_y2,
			  float tex_x1,float tex_y1,float tex_x2,float tex_y2,
			  int blend,GLenum src_blend,GLenum dst_blend,
			  int change_col) {
	unsigned int i;
	const float x_size=0.266f;
	const float y_size=0.2f;
	float v_coords[12];
	v_coords[0]=-x_size+screen_x1*x_size*2.0f;
	v_coords[1]=y_size-screen_y1*y_size*2.0f;
	v_coords[2]=-0.2f;
	v_coords[3]=-x_size+screen_x1*x_size*2.0f;
	v_coords[4]=y_size-screen_y2*y_size*2.0f;
	v_coords[5]=-0.2f;
	v_coords[6]=-x_size+screen_x2*x_size*2.0f;
	v_coords[7]=y_size-screen_y2*y_size*2.0f;
	v_coords[8]=-0.2f;
	v_coords[9]=-x_size+screen_x2*x_size*2.0f;
	v_coords[10]=y_size-screen_y1*y_size*2.0f;
	v_coords[11]=-0.2f;
	float t_coords[8];
	t_coords[0]=tex_x1;
	t_coords[1]=tex_y1;
	t_coords[2]=tex_x1;
	t_coords[3]=tex_y2;
	t_coords[4]=tex_x2;
	t_coords[5]=tex_y2;
	t_coords[6]=tex_x2;
	t_coords[7]=tex_y1;

	glVertexPointer(3,GL_FLOAT,0,v_coords);
	
	for(i=1;i<max_texture_units;i++) {
		c_state->textures[i].clear_tex_gen_flags();
		c_state->textures[i].enabled=0;
		c_state->textures[i].cube_map_enabled=0;
		c_state->textures[i].client_state_enabled=0;
	}
	if(tex) {
		c_state->textures[0].enabled=1;
		c_state->textures[0].tex=tex;
		c_state->textures[0].set_texture_env(GL_MODULATE);
		c_state->textures[0].clear_tex_gen_flags();
		//nowy stuff
		c_state->textures[0].coord_pointer=t_coords;
		c_state->textures[0].client_state_enabled=1;
	}
	else {
		c_state->textures[0].enabled=0;
		c_state->textures[0].client_state_enabled=0;
	}
	c_state->want_disabled(GL_LIGHTING);
	c_state->want_disabled(GL_FOG);
	c_state->want_disabled(GL_DEPTH_TEST);
	c_state->want_disabled(GL_ALPHA_TEST);
	if(blend) {
		c_state->want_enabled(GL_BLEND);
		glBlendFunc(src_blend,dst_blend);
	}
	else {
		c_state->want_disabled(GL_BLEND);
	}
	if(tex && change_col) glColor4f(1.0f,1.0f,1.0f,1.0f);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glFrustum(-0.133,0.133,-0.1,0.1,0.1,2.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	c_state->prerender_setup();
	c_state->want_disabled(GL_CULL_FACE);
	c_state->want_disabled(GL_DEPTH_TEST);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDepthMask(0);
	if(hasVertexArrayRange) {
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	glDrawArrays(GL_QUADS,0,4);
	if(hasVertexArrayRange) {
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	glDepthMask(1);
	glEnableClientState(GL_NORMAL_ARRAY);
	c_state->want_enabled(GL_DEPTH_TEST);
	c_state->want_enabled(GL_CULL_FACE);
	c_state->postrender_setup();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

unsigned int rand_seed=0;

unsigned int next_int(unsigned int ref) {
	rand_seed=69069*ref+1;
	return rand_seed;
}

unsigned int random_int() {
	rand_seed=69069*rand_seed+1;
	return rand_seed;
}

float next_float(float ref) {
	rand_seed=69069*((unsigned int)(ref*(float)pow(2.0,32.0)))+1;
	return (float)rand_seed/(float)pow(2.0,32.0)+0.5f;    // ????????????????????????????
}

float random_float() {
	rand_seed=69069*rand_seed+1;
	return (float)rand_seed/(float)pow(2.0,32.0);
}

