/***********************************************************************************************
  OpenGL state manager
  copyright (c) 2000 Maciej Mroz
************************************************************************************************/

#ifndef _DM_STATE_H_
#define _DM_STATE_H_

#include "glstuff.h"
//#include "texture.h"

enum {
	SM_LIGHTING,
	SM_DEPTH_TEST,
	SM_STENCIL_TEST,
	SM_ALPHA_TEST,
	SM_FOG,
	SM_LIGHT0,
	SM_LIGHT1,
	SM_LIGHT2,
	SM_LIGHT3,
	SM_LIGHT4,
	SM_LIGHT5,
	SM_LIGHT6,
	SM_LIGHT7,
	SM_CULL_FACE,
	SM_BLEND
};

#define NUM_STATES 15

class Texture;

class TMUDescriptor {
	GLenum texture_env;
	GLenum texture_env_copy;
	GLenum env_combine_p1[16];
	GLenum env_combine_p2[16];
	int num_env_combine_params;
	int enabled_copy;
	int cube_map_enabled_copy;
	int client_state_enabled_copy;
	float *coord_pointer_copy;
	Texture *tex_copy;
	GLenum tex_gen_s_mode_copy;
	GLenum tex_gen_t_mode_copy;
	int tex_gen_s_enabled_copy;
	int tex_gen_t_enabled_copy;
	float lod_bias;
	int needs_change();
	friend class StateManager;
	friend class Texture;
public:
	int enabled;
	int cube_map_enabled;
	int client_state_enabled;
	float *coord_pointer;
	Texture *tex;
	float tex_offset[3];
	GLenum tex_gen_s_mode;
	GLenum tex_gen_t_mode;
	int tex_gen_s_enabled;
	int tex_gen_t_enabled;
	int force_combine_update;  //update EXT_texture_env_combine parameters
	//it is important to set it to 1 before prerender_setup() when using this extension
	void set_texture_env(GLenum env);
	void env_combine_param(GLenum p1,GLenum p2);
	void clear_tex_gen_flags();
	TMUDescriptor();
};

class StateManager {
	int states[NUM_STATES];
	int new_states[NUM_STATES];
	int is_locked[NUM_STATES];
	int operating_mode; //immediate=0,cached=1
	//want_active_texture_ARB(..) and want_client_active_texture_ARB(..) are never cached
	//and operate in immediate mode
	//current texture unit and client active texture unit are undefined after call
	//to apply_state_vector()
	unsigned int current_texture;
	unsigned int current_client;
	int matrix_mode_changed;  //indicate whether matrix mode was changed to GL_TEXTURE
	friend class Texture;
public:
	TMUDescriptor textures[8];
	void want_enabled(GLenum s);
	void want_disabled(GLenum s);
	void want_active_texture_ARB(GLenum tmu_id);
	void want_active_client_texture_ARB(GLenum tmu_id);
	void want_client_state_enabled(GLenum tmu_id);
	void want_client_state_disabled(GLenum tmu_id);
	void want_tmu_disabled(GLenum tmu_id);
	void lock_state(GLenum s);
	void unlock_state(GLenum s);
	void set_cached_mode();
	void set_immediate_mode();
	void apply_state_vector();
	void prerender_setup();
	void postrender_setup();
	void synchronize();		//force state reset when context switching
	void dump_state_to_framelog();
	StateManager();
};

extern "C++" StateManager state;
extern "C++" StateManager alt_state;
extern "C++" StateManager* c_state;

#endif