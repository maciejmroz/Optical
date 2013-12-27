/***********************************************************************************************
  OpenGL state manager
  copyright (c) 2000 Maciej Mroz
************************************************************************************************/

#include "glstate.h"
#include "debug_msg.h"
#include "texture.h"
#include <math.h>

void TMUDescriptor::clear_tex_gen_flags() {
	tex_gen_s_enabled=0;
	tex_gen_t_enabled=0;
}

TMUDescriptor::TMUDescriptor() {
	lod_bias=0.0f;
	client_state_enabled=0;
	client_state_enabled_copy=0;
	coord_pointer=NULL;
	coord_pointer_copy=NULL;
	enabled=0;
	enabled_copy=0;
	cube_map_enabled=0;
	cube_map_enabled_copy=0;
	int i;
	for(i=0;i<16;i++) {
		env_combine_p1[i]=GL_NONE;
		env_combine_p2[i]=GL_NONE;
	}
	num_env_combine_params=0;
	tex=NULL;
	tex_copy=NULL;
	texture_env=GL_NONE;
	texture_env_copy=GL_NONE;
	clear_tex_gen_flags();
	tex_offset[0]=tex_offset[1]=tex_offset[2]=0.0f;
	force_combine_update=0;
}

void TMUDescriptor::set_texture_env(GLenum env) {
	if(env==GL_COMBINE_EXT) num_env_combine_params=0;
	texture_env=env;
}

void TMUDescriptor::env_combine_param(GLenum p1,GLenum p2) {
	env_combine_p1[num_env_combine_params]=p1;
	env_combine_p2[num_env_combine_params++]=p2;
}

int TMUDescriptor::needs_change() {
	if(enabled!=enabled_copy) return 1;
	if(cube_map_enabled!=cube_map_enabled_copy) return 1;
	if(enabled && client_state_enabled
		&& (coord_pointer!=coord_pointer_copy)) return 1;
	if(cube_map_enabled && client_state_enabled
		&& (coord_pointer!=coord_pointer_copy)) return 1;
	if(enabled || cube_map_enabled) {
		if(tex!=tex_copy) return 1;
		if(tex_gen_s_enabled!=tex_gen_s_enabled_copy) return 1;
		if(tex_gen_s_enabled) {
			if(tex_gen_s_mode!=tex_gen_s_mode_copy) return 1;
		}
		if(tex_gen_t_enabled!=tex_gen_t_enabled_copy) return 1;
		if(tex_gen_t_enabled) {
			if(tex_gen_t_mode!=tex_gen_t_mode_copy) return 1;
		}
		if(texture_env!=texture_env_copy) return 1;
		if(force_combine_update) return 1;
		if((tex_offset[0]+tex_offset[1])!=0.0f) return 1;
	}
	return 0;
}

void StateManager::prerender_setup() {
	unsigned int i;
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("PRERENDER SETUP:\n");
	DebugFrameMessage("\tCurrent texture is %d\n",current_texture);
	DebugFrameMessage("\tCurrent client active texture is %d\n",current_client);
	for(i=0;i<max_texture_units;i++) {
		DebugFrameMessage("\tTexture%d.enabled %d,cached %d\n",i,
			textures[i].enabled,textures[i].enabled_copy);
		DebugFrameMessage("\tTexture%d.client_state_enabled %d, cached %d\n",i,
			textures[i].client_state_enabled,textures[i].client_state_enabled_copy);
		DebugFrameMessage("\tTexcoord pointer: 0x%x, cached 0x%x\n",
			textures[i].coord_pointer,textures[i].coord_pointer_copy);
		DebugFrameMessage("\tTexture%d.tex 0x%x, cached 0x%x\n",i,textures[i].tex,textures[i].tex_copy);
		switch(textures[i].texture_env) {
		case GL_ADD:
			DebugFrameMessage("\tTexture environment function is GL_ADD, cached ");
			break;
		case GL_REPLACE:
			DebugFrameMessage("\tTexture environment function is GL_REPLACE, cached ");
			break;
		case GL_MODULATE:
			DebugFrameMessage("\tTexture environment function is GL_MODULATE, cached ");
			break;
		default:
			DebugFrameMessage("\tTexture environment function is unknown, cached ");
			break;
		}
		switch(textures[i].texture_env_copy) {
		case GL_ADD:
			DebugFrameMessage("GL_ADD\n");
			break;
		case GL_REPLACE:
			DebugFrameMessage("GL_REPLACE\n");
			break;
		case GL_MODULATE:
			DebugFrameMessage("GL_MODULATE\n");
			break;
		default:
			DebugFrameMessage("unknown\n");
			break;
		}
		DebugFrameMessage("\tTexgen: s - %d, cached %d; t- %d, cached %d\n",
			textures[i].tex_gen_s_enabled,textures[i].tex_gen_s_enabled_copy,
			textures[i].tex_gen_t_enabled,textures[i].tex_gen_t_enabled_copy);
	}
#endif
	if(textures[current_texture].enabled!=textures[current_texture].enabled_copy) {
		textures[current_texture].enabled_copy=textures[current_texture].enabled;
		if(textures[current_texture].enabled) {
#ifdef _LOG_EVERYTHING
			DebugFrameMessage("glEnable(GL_TEXTURE_2D);\n");
#endif
			glEnable(GL_TEXTURE_2D);
			//force BindTexture
			if(textures[current_texture].tex) textures[current_texture].tex_copy=NULL;  
		}
		else {
#ifdef _LOG_EVERYTHING
			DebugFrameMessage("glDisable(GL_TEXTURE_2D);\n");
#endif
			glDisable(GL_TEXTURE_2D);
		}
	}
	if(textures[current_texture].cube_map_enabled!=
		textures[current_texture].cube_map_enabled_copy) {
		textures[current_texture].cube_map_enabled_copy=
			textures[current_texture].cube_map_enabled;
		if(textures[current_texture].cube_map_enabled) {
#ifdef _LOG_EVERYTHING
			DebugFrameMessage("glEnable(GL_TEXTURE_CUBE_MAP_ARB);\n");
#endif
			glEnable(GL_TEXTURE_CUBE_MAP_ARB);
			//force BindTexture
			if(textures[current_texture].tex) textures[current_texture].tex_copy=NULL;  
		}
		else {
#ifdef _LOG_EVERYTHING
			DebugFrameMessage("glDisable(GL_TEXTURE_CUBE_MAP_ARB);\n");
#endif
			glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		}
	}
	if(textures[current_client].client_state_enabled!=
		textures[current_client].client_state_enabled_copy) {
		textures[current_client].client_state_enabled_copy=
			textures[current_client].client_state_enabled;
		if(textures[current_client].client_state_enabled) {
#ifdef _LOG_EVERYTHING
			DebugFrameMessage("glEnableClientState(GL_TEXTURE_COORD_ARRAY);\n");
#endif
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		else {
#ifdef _LOG_EVERYTHING
			DebugFrameMessage("glDisableClientState(GL_TEXTURE_COORD_ARRAY);\n");
#endif
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
	if(textures[current_client].enabled) {
		if(textures[current_client].client_state_enabled) {
			if(textures[current_client].coord_pointer!=
				textures[current_client].coord_pointer_copy) {
				textures[current_client].coord_pointer_copy=
					textures[current_client].coord_pointer;
#ifdef _LOG_EVERYTHING
				DebugFrameMessage("glTexCoordPointer(current client=%d);\n",current_client);
#endif
				glTexCoordPointer(2,GL_FLOAT,0,textures[current_client].coord_pointer);
			}
		}
	}
	if(textures[current_client].cube_map_enabled) {
		if(textures[current_client].client_state_enabled) {
			if(textures[current_client].coord_pointer!=
				textures[current_client].coord_pointer_copy) {
				textures[current_client].coord_pointer_copy=
					textures[current_client].coord_pointer;
#ifdef _LOG_EVERYTHING
				DebugFrameMessage("glTexCoordPointer(current client=%d - cube map);\n",
					current_client);
#endif
				glTexCoordPointer(3,GL_FLOAT,0,textures[current_client].coord_pointer);
			}
		}
	}
	if(textures[current_texture].client_state_enabled!=
		textures[current_texture].client_state_enabled_copy) {
		textures[current_texture].client_state_enabled_copy=
			textures[current_texture].client_state_enabled;
		current_client=current_texture;
#ifdef _LOG_EVERYTHING
		DebugFrameMessage("glClientActiveTextureARB(%d);\n",current_client);
#endif
		glClientActiveTextureARB(GL_TEXTURE0_ARB+current_client);
		if(textures[current_texture].client_state_enabled) {
#ifdef _LOG_EVERYTHING
			DebugFrameMessage("glEnableClientState(GL_TEXTURE_COORD_ARRAY);\n");
#endif
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		else {
#ifdef _LOG_EVERYTHING
			DebugFrameMessage("glDisableClientState(GL_TEXTURE_COORD_ARRAY);\n");
#endif
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
	if(textures[current_texture].enabled || textures[current_texture].cube_map_enabled) {
		if(textures[current_texture].tex!=textures[current_texture].tex_copy) {
			textures[current_texture].tex_copy=textures[current_texture].tex;
			if(textures[current_texture].tex) textures[current_texture].tex->activate();
		}
		if(textures[current_texture].client_state_enabled 
			&& textures[current_texture].enabled) {
			if(textures[current_texture].coord_pointer!=
				textures[current_texture].coord_pointer_copy) {
				textures[current_texture].coord_pointer_copy=
					textures[current_texture].coord_pointer;
				if(current_client!=current_texture) {
					current_client=current_texture;
#ifdef _LOG_EVERYTHING
					DebugFrameMessage("glClientActiveTextureARB(%d);\n",current_client);
#endif
					glClientActiveTextureARB(GL_TEXTURE0_ARB+current_client);
				}
#ifdef _LOG_EVERYTHING
				DebugFrameMessage("glTexCoordPointer(current client=%d);\n",current_client);
#endif
				glTexCoordPointer(2,GL_FLOAT,0,textures[current_texture].coord_pointer);
			}
		}
		if(textures[current_client].client_state_enabled &&
			textures[current_client].cube_map_enabled) {
			if(textures[current_client].coord_pointer!=
				textures[current_client].coord_pointer_copy) {
				textures[current_client].coord_pointer_copy=
					textures[current_client].coord_pointer;
				if(current_client!=current_texture) {
					current_client=current_texture;
#ifdef _LOG_EVERYTHING
					DebugFrameMessage("glClientActiveTextureARB(%d);\n",current_client);
#endif
					glClientActiveTextureARB(GL_TEXTURE0_ARB+current_client);
				}
#ifdef _LOG_EVERYTHING
				DebugFrameMessage("glTexCoordPointer(current client=%d - cube map);\n",
					current_client);
#endif
				glTexCoordPointer(3,GL_FLOAT,0,textures[current_client].coord_pointer);
			}
		}
		if(textures[current_texture].tex_gen_s_enabled!=
			textures[current_texture].tex_gen_s_enabled_copy) {
			textures[current_texture].tex_gen_s_enabled_copy=
				textures[current_texture].tex_gen_s_enabled;
			if(textures[current_texture].tex_gen_s_enabled) {
#ifdef _LOG_EVERYTHING
				DebugFrameMessage("glEnable(GL_TEXTURE_GEN_S);\n");
#endif
				glEnable(GL_TEXTURE_GEN_S);
			}
			else {
#ifdef _LOG_EVERYTHING
				DebugFrameMessage("glDisable(GL_TEXTURE_GEN_S);\n");
#endif
				glDisable(GL_TEXTURE_GEN_S);
			}
		}
		if(textures[current_texture].tex_gen_s_enabled) {
			if(textures[current_texture].tex_gen_s_mode!=GL_NONE) {
#ifdef _LOG_EVERYTHING
				DebugFrameMessage("glTexGeni(GL_S,current_rexture=%d);\n",current_texture);
#endif
				glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,textures[current_texture].tex_gen_s_mode);
			}
		}
		if(textures[current_texture].tex_gen_t_enabled!=
			textures[current_texture].tex_gen_t_enabled_copy) {
			textures[current_texture].tex_gen_t_enabled_copy=
				textures[current_texture].tex_gen_t_enabled;
			if(textures[current_texture].tex_gen_t_enabled) {
#ifdef _LOG_EVERYTHING
				DebugFrameMessage("glEnable(GL_TEXTURE_GEN_T);\n");
#endif
				glEnable(GL_TEXTURE_GEN_T);
			}
			else {
#ifdef _LOG_EVERYTHING
				DebugFrameMessage("glDisable(GL_TEXTURE_GEN_T);\n");
#endif
				glDisable(GL_TEXTURE_GEN_T);
			}
		}
		if(textures[current_texture].tex_gen_t_enabled) {
			if(textures[current_texture].tex_gen_t_mode!=GL_NONE) {
#ifdef _LOG_EVERYTHING
				DebugFrameMessage("glTexGeni(GL_T,current_rexture=%d);\n",current_texture);
#endif
				glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,textures[current_texture].tex_gen_t_mode);
			}
		}
		if(textures[current_texture].texture_env!=textures[current_texture].texture_env_copy) {
			textures[current_texture].texture_env_copy=
				textures[current_texture].texture_env;
				glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,
					textures[current_texture].texture_env);
				if(textures[current_texture].texture_env==GL_COMBINE_EXT) {
					int i;
					for(i=0;i<textures[current_texture].num_env_combine_params;i++) {
						glTexEnvi(GL_TEXTURE_ENV,
							textures[current_texture].env_combine_p1[i],
							textures[current_texture].env_combine_p2[i]);
					}
					textures[current_texture].force_combine_update=0;
				}
		}
		if((textures[current_texture].texture_env==GL_COMBINE_EXT)
			&& textures[current_texture].force_combine_update) {
			int i;
			for(i=0;i<textures[current_texture].num_env_combine_params;i++) {
				glTexEnvi(GL_TEXTURE_ENV,
					textures[current_texture].env_combine_p1[i],
					textures[current_texture].env_combine_p2[i]);
			}
			textures[current_texture].force_combine_update=0;
		}
		if((textures[current_texture].tex_offset[0]+
			textures[current_texture].tex_offset[1])!=0.0f) {
			if(!matrix_mode_changed) {
				glMatrixMode(GL_TEXTURE);
				matrix_mode_changed=1;
			}
			glLoadIdentity();
			glTranslatef((float)fmod((double)textures[current_texture].tex_offset[0],1.0),
				(float)fmod((double)textures[current_texture].tex_offset[1],1.0),0.0f);
		}
	}
	unsigned int old_current;
	old_current=current_texture;
	for(i=0;i<8;i++) {
		if((i!=old_current) && (textures[i].needs_change())) {
			current_texture=i;
#ifdef _LOG_EVERYTHING
			DebugFrameMessage("glActiveTextureARB(%d)\n",current_texture);
#endif
			glActiveTextureARB(GL_TEXTURE0_ARB+current_texture);
			if(textures[current_texture].enabled!=textures[current_texture].enabled_copy) {
				textures[current_texture].enabled_copy=textures[current_texture].enabled;
				if(textures[current_texture].enabled) {
#ifdef _LOG_EVERYTHING
					DebugFrameMessage("glEnable(GL_TEXTURE_2D)\n");
#endif
					//force BindTexture
					if(textures[current_texture].tex) textures[current_texture].tex_copy=NULL;  
					glEnable(GL_TEXTURE_2D);
				}
				else {
#ifdef _LOG_EVERYTHING
					DebugFrameMessage("glDisable(GL_TEXTURE_2D)\n");
#endif
					glDisable(GL_TEXTURE_2D);
				}
			}
			if(textures[current_texture].cube_map_enabled!=
				textures[current_texture].cube_map_enabled_copy) {
				textures[current_texture].cube_map_enabled_copy=
					textures[current_texture].cube_map_enabled;
				if(textures[current_texture].cube_map_enabled) {
#ifdef _LOG_EVERYTHING
					DebugFrameMessage("glEnable(GL_TEXTURE_CUBE_MAP_ARB);\n");
#endif
					glEnable(GL_TEXTURE_CUBE_MAP_ARB);
				}
				else {
#ifdef _LOG_EVERYTHING
					DebugFrameMessage("glDisable(GL_TEXTURE_CUBE_MAP_ARB);\n");
#endif
					glDisable(GL_TEXTURE_CUBE_MAP_ARB);
				}
			}
			if(textures[current_client].client_state_enabled!=
				textures[current_client].client_state_enabled_copy) {
				textures[current_client].client_state_enabled_copy=
					textures[current_client].client_state_enabled;
				if(textures[current_client].client_state_enabled)
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				else glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			}
			if(textures[current_client].enabled) {
				if(textures[current_client].client_state_enabled) {
					if(textures[current_client].coord_pointer!=
						textures[current_client].coord_pointer_copy) {
						textures[current_client].coord_pointer_copy=
							textures[current_client].coord_pointer;
						glTexCoordPointer(2,GL_FLOAT,0,textures[current_client].coord_pointer);
					}
				}
			}
			if(textures[current_client].client_state_enabled &&
				textures[current_client].cube_map_enabled) {
				if(textures[current_client].coord_pointer!=
					textures[current_client].coord_pointer_copy) {
					textures[current_client].coord_pointer_copy=
						textures[current_client].coord_pointer;
#ifdef _LOG_EVERYTHING
					DebugFrameMessage("glTexCoordPointer(current client=%d - cube map);\n",
						current_client);
#endif
					glTexCoordPointer(3,GL_FLOAT,0,textures[current_client].coord_pointer);
				}
			}
			if(textures[current_texture].client_state_enabled!=
				textures[current_texture].client_state_enabled_copy) {
				textures[current_texture].client_state_enabled_copy=
					textures[current_texture].client_state_enabled;
				current_client=current_texture;
#ifdef _LOG_EVERYTHING
				DebugFrameMessage("glClientActiveTextureARB(%d);\n",current_client);
#endif
				glClientActiveTextureARB(GL_TEXTURE0_ARB+current_client);
				if(textures[current_texture].client_state_enabled) {
#ifdef _LOG_EVERYTHING
					DebugFrameMessage("glEnableClientState(GL_TEXTURE_COORD_ARRAY);\n");
#endif
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				}
				else {
#ifdef _LOG_EVERYTHING
					DebugFrameMessage("glDisableClientState(GL_TEXTURE_COORD_ARRAY);\n");
#endif
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}
			}
			if(textures[current_texture].enabled ||
				textures[current_texture].cube_map_enabled) {
				if(textures[current_texture].tex!=textures[current_texture].tex_copy) {
					textures[current_texture].tex_copy=textures[current_texture].tex;
					if(textures[current_texture].tex) textures[current_texture].tex->activate();
				}
				if(textures[current_texture].client_state_enabled &&
					textures[current_texture].enabled) {
					if(textures[current_texture].coord_pointer!=
						textures[current_texture].coord_pointer_copy) {
						textures[current_texture].coord_pointer_copy=
							textures[current_texture].coord_pointer;
						if(current_client!=current_texture) {
							current_client=current_texture;
#ifdef _LOG_EVERYTHING
							DebugFrameMessage("glClientActiveTextureARB(%d);\n",current_client);
#endif
							glClientActiveTextureARB(GL_TEXTURE0_ARB+current_client);
						}
#ifdef _LOG_EVERYTHING
						DebugFrameMessage("glTexCoordPointer(current client=%d);\n",
							current_client);
#endif
						glTexCoordPointer(2,GL_FLOAT,0,textures[current_texture].coord_pointer);
					}
				}
				if(textures[current_texture].client_state_enabled &&
					textures[current_texture].cube_map_enabled) {
					if(textures[current_texture].coord_pointer!=
						textures[current_texture].coord_pointer_copy) {
						textures[current_texture].coord_pointer_copy=
							textures[current_texture].coord_pointer;
						if(current_client!=current_texture) {
							current_client=current_texture;
#ifdef _LOG_EVERYTHING
							DebugFrameMessage("glClientActiveTextureARB(%d);\n",current_client);
#endif
							glClientActiveTextureARB(GL_TEXTURE0_ARB+current_client);
						}
#ifdef _LOG_EVERYTHING
						DebugFrameMessage("glTexCoordPointer(current client=%d - cube map);\n",
							current_client);
#endif
						glTexCoordPointer(3,GL_FLOAT,0,textures[current_texture].coord_pointer);
					}
				}
				if(textures[current_texture].tex_gen_s_enabled!=
					textures[current_texture].tex_gen_s_enabled_copy) {
					textures[current_texture].tex_gen_s_enabled_copy=
						textures[current_texture].tex_gen_s_enabled;
					if(textures[current_texture].tex_gen_s_enabled) glEnable(GL_TEXTURE_GEN_S);
					else glDisable(GL_TEXTURE_GEN_S);
				}
				if(textures[current_texture].tex_gen_s_enabled) {
					if(textures[current_texture].tex_gen_s_mode!=GL_NONE) {
						glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,textures[current_texture].tex_gen_s_mode);
					}
				}
				if(textures[current_texture].tex_gen_t_enabled!=
					textures[current_texture].tex_gen_t_enabled_copy) {
					textures[current_texture].tex_gen_t_enabled_copy=
						textures[current_texture].tex_gen_t_enabled;
					if(textures[current_texture].tex_gen_t_enabled) glEnable(GL_TEXTURE_GEN_T);
					else glDisable(GL_TEXTURE_GEN_T);
				}
				if(textures[current_texture].tex_gen_t_enabled) {
					if(textures[current_texture].tex_gen_t_mode!=GL_NONE) {
						glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,textures[current_texture].tex_gen_t_mode);
					}
				}
				if(textures[current_texture].texture_env!=textures[current_texture].texture_env_copy) {
					textures[current_texture].texture_env_copy=
						textures[current_texture].texture_env;
					glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,
						textures[current_texture].texture_env);
					if(textures[current_texture].texture_env==GL_COMBINE_EXT) {
						int i;
						for(i=0;i<textures[current_texture].num_env_combine_params;i++) {
							glTexEnvi(GL_TEXTURE_ENV,
								textures[current_texture].env_combine_p1[i],
								textures[current_texture].env_combine_p2[i]);
						}
						textures[current_texture].force_combine_update=0;
					}
				}
				if((textures[current_texture].texture_env==GL_COMBINE_EXT)
					&& textures[current_texture].force_combine_update) {
					int i;
					for(i=0;i<textures[current_texture].num_env_combine_params;i++) {
						glTexEnvi(GL_TEXTURE_ENV,
							textures[current_texture].env_combine_p1[i],
							textures[current_texture].env_combine_p2[i]);
					}
					textures[current_texture].force_combine_update=0;
				}
				if((textures[current_texture].tex_offset[0]+
					textures[current_texture].tex_offset[1])!=0.0f) {
					if(!matrix_mode_changed) {
						glMatrixMode(GL_TEXTURE);
						matrix_mode_changed=1;
					}
					glLoadIdentity();
					glTranslatef((float)fmod((double)textures[current_texture].tex_offset[0],1.0),
						(float)fmod((double)textures[current_texture].tex_offset[1],1.0),0.0f);
				}
			}
		}
	}
	for(i=0;i<8;i++) {  //check client state changes that might have been missed above
		if(textures[i].client_state_enabled_copy!=
			textures[i].client_state_enabled) {
			textures[i].client_state_enabled_copy=
				textures[i].client_state_enabled;
			if(current_client!=i) {
				current_client=i;
				glClientActiveTextureARB(GL_TEXTURE0_ARB+current_client);
			}
			if(textures[i].client_state_enabled) glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			else glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
}

void StateManager::postrender_setup() {
	//reset matrix mode to modelview etc
	if(matrix_mode_changed) {
		unsigned int i;
		for(i=0;i<8;i++) {
			if((textures[i].tex_offset[0]+
				textures[i].tex_offset[1])!=0.0f) {
				if(current_texture!=i) {
					current_texture=i;
					glActiveTextureARB(GL_TEXTURE0_ARB+i);
				}
				glLoadIdentity();
				textures[i].tex_offset[0]=0.0f;
				textures[i].tex_offset[1]=0.0f;
			}
		}
		matrix_mode_changed=0;
		glMatrixMode(GL_MODELVIEW);
	}
}

StateManager::StateManager() {
	int i;
	for(i=0;i<NUM_STATES;i++) {
		states[i]=0;
		new_states[i]=0;
		is_locked[i]=0;
	}
	operating_mode=0;
	matrix_mode_changed=0;
	current_texture=0;
	current_client=0;
}

void StateManager::set_cached_mode() {
	operating_mode=1;
	int i;
	for(i=0;i<NUM_STATES;i++) {
		new_states[i]=states[i];
	}
}

void StateManager::set_immediate_mode() {
	operating_mode=0;
	apply_state_vector();
}

void StateManager::apply_state_vector() {
	if(new_states[SM_LIGHTING]!=states[SM_LIGHTING]) {
		states[SM_LIGHTING]=new_states[SM_LIGHTING];
		if(states[SM_LIGHTING]) {
			glEnable(GL_LIGHTING);
		}
		else {
			glDisable(GL_LIGHTING);
		}
	}
	if(new_states[SM_DEPTH_TEST]!=states[SM_DEPTH_TEST]) {
		states[SM_DEPTH_TEST]=new_states[SM_DEPTH_TEST];
		if(states[SM_DEPTH_TEST]) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}
	}
	if(new_states[SM_STENCIL_TEST]!=states[SM_STENCIL_TEST]) {
		states[SM_STENCIL_TEST]=new_states[SM_STENCIL_TEST];
		if(states[SM_STENCIL_TEST]) {
			glEnable(GL_STENCIL_TEST);
		}
		else {
			glDisable(GL_STENCIL_TEST);
		}
	}
	if(new_states[SM_ALPHA_TEST]!=states[SM_ALPHA_TEST]) {
		states[SM_ALPHA_TEST]=new_states[SM_ALPHA_TEST];
		if(states[SM_ALPHA_TEST]) {
			glEnable(GL_ALPHA_TEST);
		}
		else {
			glDisable(GL_ALPHA_TEST);
		}
	}
	if(new_states[SM_FOG]!=states[SM_FOG]) {
		states[SM_FOG]=new_states[SM_FOG];
		if(states[SM_FOG]) {
			glEnable(GL_FOG);
		}
		else {
			glDisable(GL_FOG);
		}
	}
	if(new_states[SM_LIGHT0]!=states[SM_LIGHT0]) {
		states[SM_LIGHT0]=new_states[SM_LIGHT0];
		if(states[SM_LIGHT0]) {
			glEnable(GL_LIGHT0);
		}
		else {
			glDisable(GL_LIGHT0);
		}
	}
	if(new_states[SM_LIGHT1]!=states[SM_LIGHT1]) {
		states[SM_LIGHT1]=new_states[SM_LIGHT1];
		if(states[SM_LIGHT1]) {
			glEnable(GL_LIGHT1);
		}
		else {
			glDisable(GL_LIGHT1);
		}
	}
	if(new_states[SM_LIGHT2]!=states[SM_LIGHT2]) {
		states[SM_LIGHT2]=new_states[SM_LIGHT2];
		if(states[SM_LIGHT2]) {
			glEnable(GL_LIGHT2);
		}
		else {
			glDisable(GL_LIGHT2);
		}
	}
	if(new_states[SM_LIGHT3]!=states[SM_LIGHT3]) {
		states[SM_LIGHT3]=new_states[SM_LIGHT3];
		if(states[SM_LIGHT3]) {
			glEnable(GL_LIGHT3);
		}
		else {
			glDisable(GL_LIGHT3);
		}
	}
	if(new_states[SM_LIGHT4]!=states[SM_LIGHT4]) {
		states[SM_LIGHT4]=new_states[SM_LIGHT4];
		if(states[SM_LIGHT4]) {
			glEnable(GL_LIGHT4);
		}
		else {
			glDisable(GL_LIGHT4);
		}
	}
	if(new_states[SM_LIGHT5]!=states[SM_LIGHT5]) {
		states[SM_LIGHT5]=new_states[SM_LIGHT5];
		if(states[SM_LIGHT5]) {
			glEnable(GL_LIGHT5);
		}
		else {
			glDisable(GL_LIGHT5);
		}
	}
	if(new_states[SM_LIGHT6]!=states[SM_LIGHT6]) {
		states[SM_LIGHT6]=new_states[SM_LIGHT6];
		if(states[SM_LIGHT6]) {
			glEnable(GL_LIGHT6);
		}
		else {
			glDisable(GL_LIGHT6);
		}
	}
	if(new_states[SM_LIGHT7]!=states[SM_LIGHT7]) {
		states[SM_LIGHT7]=new_states[SM_LIGHT7];
		if(states[SM_LIGHT7]) {
			glEnable(GL_LIGHT7);
		}
		else {
			glDisable(GL_LIGHT7);
		}
	}
	if(new_states[SM_CULL_FACE]!=states[SM_CULL_FACE]) {
		states[SM_CULL_FACE]=new_states[SM_CULL_FACE];
		if(states[SM_CULL_FACE]) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}
	}
	if(new_states[SM_BLEND]!=states[SM_BLEND]) {
		states[SM_BLEND]=new_states[SM_BLEND];
		if(states[SM_BLEND]) {
			glEnable(GL_BLEND);
		}
		else {
			glDisable(GL_BLEND);
		}
	}
}

void StateManager::lock_state(GLenum s) {
	switch(s) {
		case GL_LIGHTING:
			is_locked[SM_LIGHTING]=1;
		break;
		case GL_DEPTH_TEST:
			is_locked[SM_DEPTH_TEST]=1;
		break;
		case GL_STENCIL_TEST:
			is_locked[SM_STENCIL_TEST]=1;
		break;
		case GL_ALPHA_TEST:
			is_locked[SM_ALPHA_TEST]=1;
		break;
		case GL_FOG:
			is_locked[SM_FOG]=1;
		break;
		case GL_LIGHT0:
			is_locked[SM_LIGHT0]=1;
		break;
		case GL_LIGHT1:
			is_locked[SM_LIGHT1]=1;
		break;
		case GL_LIGHT2:
			is_locked[SM_LIGHT2]=1;
		break;
		case GL_LIGHT3:
			is_locked[SM_LIGHT3]=1;
		break;
		case GL_LIGHT4:
			is_locked[SM_LIGHT4]=1;
		break;
		case GL_LIGHT5:
			is_locked[SM_LIGHT5]=1;
		break;
		case GL_LIGHT6:
			is_locked[SM_LIGHT6]=1;
		break;
		case GL_LIGHT7:
			is_locked[SM_LIGHT7]=1;
		break;
		case GL_CULL_FACE:
			is_locked[SM_CULL_FACE]=1;
		break;
		case GL_BLEND:
			is_locked[SM_BLEND]=1;
		break;
	}
}

void StateManager::unlock_state(GLenum s) {
	switch(s) {
		case GL_LIGHTING:
			is_locked[SM_LIGHTING]=0;
		break;
		case GL_DEPTH_TEST:
			is_locked[SM_DEPTH_TEST]=0;
		break;
		case GL_STENCIL_TEST:
			is_locked[SM_STENCIL_TEST]=0;
		break;
		case GL_ALPHA_TEST:
			is_locked[SM_ALPHA_TEST]=0;
		break;
		case GL_FOG:
			is_locked[SM_FOG]=0;
		break;
		case GL_LIGHT0:
			is_locked[SM_LIGHT0]=0;
		break;
		case GL_LIGHT1:
			is_locked[SM_LIGHT0]=0;
		break;
		case GL_LIGHT2:
			is_locked[SM_LIGHT2]=0;
		break;
		case GL_LIGHT3:
			is_locked[SM_LIGHT3]=0;
		break;
		case GL_LIGHT4:
			is_locked[SM_LIGHT4]=0;
		break;
		case GL_LIGHT5:
			is_locked[SM_LIGHT5]=0;
		break;
		case GL_LIGHT6:
			is_locked[SM_LIGHT6]=0;
		break;
		case GL_LIGHT7:
			is_locked[SM_LIGHT7]=0;
		break;
		case GL_CULL_FACE:
			is_locked[SM_CULL_FACE]=0;
		break;
		case GL_BLEND:
			is_locked[SM_BLEND]=0;
		break;
	}
}

void StateManager::want_enabled(GLenum s) {
	if(operating_mode) {
		switch(s) {
		case GL_LIGHTING:
			new_states[SM_LIGHTING]=1;
		break;
		case GL_DEPTH_TEST:
			new_states[SM_DEPTH_TEST]=1;
		break;
		case GL_STENCIL_TEST:
			new_states[SM_STENCIL_TEST]=1;
		break;
		case GL_ALPHA_TEST:
			new_states[SM_ALPHA_TEST]=1;
		break;
		case GL_FOG:
			new_states[SM_FOG]=1;
		break;
		case GL_LIGHT0:
			new_states[SM_LIGHT0]=1;
		break;
		case GL_LIGHT1:
			new_states[SM_LIGHT1]=1;
		break;
		case GL_LIGHT2:
			new_states[SM_LIGHT2]=1;
		break;
		case GL_LIGHT3:
			new_states[SM_LIGHT3]=1;
		break;
		case GL_LIGHT4:
			new_states[SM_LIGHT4]=1;
		break;
		case GL_LIGHT5:
			new_states[SM_LIGHT5]=1;
		break;
		case GL_LIGHT6:
			new_states[SM_LIGHT6]=1;
		break;
		case GL_LIGHT7:
			new_states[SM_LIGHT7]=1;
		break;
		case GL_TEXTURE_2D:
			textures[current_texture].enabled=1;
			prerender_setup();                         //DYSKUSYJNE!!!!
			return;
		break;
		case GL_CULL_FACE:
			new_states[SM_CULL_FACE]=1;
		break;
		case GL_BLEND:
			new_states[SM_BLEND]=1;
		break;
		}
	}
	else {
		switch(s) {
		case GL_LIGHTING:
			if(states[SM_LIGHTING] || (is_locked[SM_LIGHTING])) return;
			states[SM_LIGHTING]=1;
		break;
		case GL_DEPTH_TEST:
			if(states[SM_DEPTH_TEST] || (is_locked[SM_DEPTH_TEST])) return;
			states[SM_DEPTH_TEST]=1;
		break;
		case GL_STENCIL_TEST:
			if(states[SM_STENCIL_TEST] || (is_locked[SM_STENCIL_TEST])) return;
			states[SM_STENCIL_TEST]=1;
		break;
		case GL_ALPHA_TEST:
			if(states[SM_ALPHA_TEST] || (is_locked[SM_ALPHA_TEST])) return;
			states[SM_ALPHA_TEST]=1;
		break;
		case GL_FOG:
			if(states[SM_FOG] || (is_locked[SM_FOG])) return;
			states[SM_FOG]=1;
		break;
		case GL_LIGHT0:
			if(states[SM_LIGHT0] || (is_locked[SM_LIGHT0])) return;
			states[SM_LIGHT0]=1;
		break;
		case GL_LIGHT1:
			if(states[SM_LIGHT1] || (is_locked[SM_LIGHT1])) return;
			states[SM_LIGHT1]=1;
		break;
		case GL_LIGHT2:
			if(states[SM_LIGHT2] || (is_locked[SM_LIGHT2])) return;
			states[SM_LIGHT2]=1;
		break;
		case GL_LIGHT3:
			if(states[SM_LIGHT3] || (is_locked[SM_LIGHT3])) return;
			states[SM_LIGHT3]=1;
		break;
		case GL_LIGHT4:
			if(states[SM_LIGHT4] || (is_locked[SM_LIGHT4])) return;
			states[SM_LIGHT4]=1;
		break;
		case GL_LIGHT5:
			if(states[SM_LIGHT5] || (is_locked[SM_LIGHT5])) return;
			states[SM_LIGHT5]=1;
		break;
		case GL_LIGHT6:
			if(states[SM_LIGHT6] || (is_locked[SM_LIGHT6])) return;
			states[SM_LIGHT6]=1;
		break;
		case GL_LIGHT7:
			if(states[SM_LIGHT7] || (is_locked[SM_LIGHT7])) return;
			states[SM_LIGHT7]=1;
		break;
		case GL_TEXTURE_2D:
			textures[current_texture].enabled=1;
			prerender_setup();
			return;
		break;
		case GL_CULL_FACE:
			if(states[SM_CULL_FACE] || (is_locked[SM_CULL_FACE])) return;
			states[SM_CULL_FACE]=1;
		break;
		case GL_BLEND:
			if(states[SM_BLEND] || (is_locked[SM_BLEND])) return;
			states[SM_BLEND]=1;
		break;
		}
#ifdef _LOG_EVERYTHING
		DebugFrameMessage("Calling glEnable\n");
#endif
	glEnable(s);
	}
}

void StateManager::want_disabled(GLenum s) {
	if(operating_mode) {
		switch(s) {
		case GL_LIGHTING:
			new_states[SM_LIGHTING]=0;
		break;
		case GL_DEPTH_TEST:
			new_states[SM_DEPTH_TEST]=0;
		break;
		case GL_STENCIL_TEST:
			new_states[SM_STENCIL_TEST]=0;
		break;
		case GL_ALPHA_TEST:
			new_states[SM_ALPHA_TEST]=0;
		break;
		case GL_FOG:
			new_states[SM_FOG]=0;
		break;
		case GL_LIGHT0:
			new_states[SM_LIGHT0]=0;
		break;
		case GL_LIGHT1:
			new_states[SM_LIGHT1]=0;
		break;
		case GL_LIGHT2:
			new_states[SM_LIGHT2]=0;
		break;
		case GL_LIGHT3:
			new_states[SM_LIGHT3]=0;
		break;
		case GL_LIGHT4:
			new_states[SM_LIGHT4]=0;
		break;
		case GL_LIGHT5:
			new_states[SM_LIGHT5]=0;
		break;
		case GL_LIGHT6:
			new_states[SM_LIGHT6]=0;
		break;
		case GL_LIGHT7:
			new_states[SM_LIGHT7]=0;
		break;
		case GL_TEXTURE_2D:
			textures[current_texture].enabled=0;
			prerender_setup();                         //DYSKUSYJNE!!!!
			return;
		break;
		case GL_CULL_FACE:
			new_states[SM_CULL_FACE]=0;
		break;
		case GL_BLEND:
			new_states[SM_BLEND]=0;
		break;
		}
	}
	else {
		switch(s) {
		case GL_LIGHTING:
			if(!states[SM_LIGHTING] || (is_locked[SM_LIGHTING])) return;
			states[SM_LIGHTING]=0;
			break;
		case GL_DEPTH_TEST:
			if(!states[SM_DEPTH_TEST] || (is_locked[SM_DEPTH_TEST])) return;
			states[SM_DEPTH_TEST]=0;
			break;
		case GL_STENCIL_TEST:
			if(!states[SM_STENCIL_TEST] || (is_locked[SM_STENCIL_TEST])) return;
			states[SM_STENCIL_TEST]=0;
			break;
		case GL_ALPHA_TEST:
			if(!states[SM_ALPHA_TEST] || (is_locked[SM_ALPHA_TEST])) return;
			states[SM_ALPHA_TEST]=0;
			break;
		case GL_FOG:
			if(!states[SM_FOG] || (is_locked[SM_FOG])) return;
			states[SM_FOG]=0;
			break;
		case GL_LIGHT0:
			if(!states[SM_LIGHT0] || (is_locked[SM_LIGHT0])) return;
			states[SM_LIGHT0]=0;
			break;
		case GL_LIGHT1:
			if(!states[SM_LIGHT1] || (is_locked[SM_LIGHT1])) return;
			states[SM_LIGHT1]=0;
			break;
		case GL_LIGHT2:
			if(!states[SM_LIGHT2] || (is_locked[SM_LIGHT2])) return;
			states[SM_LIGHT2]=0;
			break;
		case GL_LIGHT3:
			if(!states[SM_LIGHT3] || (is_locked[SM_LIGHT3])) return;
			states[SM_LIGHT3]=0;
			break;
		case GL_LIGHT4:
			if(!states[SM_LIGHT4] || (is_locked[SM_LIGHT4])) return;
			states[SM_LIGHT4]=0;
			break;
		case GL_LIGHT5:
			if(!states[SM_LIGHT5] || (is_locked[SM_LIGHT5])) return;
			states[SM_LIGHT5]=0;
			break;
		case GL_LIGHT6:
			if(!states[SM_LIGHT6] || (is_locked[SM_LIGHT6])) return;
			states[SM_LIGHT6]=0;
			break;
		case GL_LIGHT7:
			if(!states[SM_LIGHT7] || (is_locked[SM_LIGHT7])) return;
			states[SM_LIGHT7]=0;
			break;
		case GL_TEXTURE_2D:
			textures[current_texture].enabled=0;
			prerender_setup();
			return;
		break;
		case GL_CULL_FACE:
			if(!states[SM_CULL_FACE] || (is_locked[SM_CULL_FACE])) return;
			states[SM_CULL_FACE]=0;
		break;
		case GL_BLEND:
			if(!states[SM_BLEND] || (is_locked[SM_BLEND])) return;
			states[SM_BLEND]=0;
		break;
		}
		glDisable(s);
	}
}

void StateManager::want_active_texture_ARB(GLenum tmu_id) {
	if(((current_texture+GL_TEXTURE0_ARB)!=tmu_id) && hasMultitexture) {
		current_texture=tmu_id-GL_TEXTURE0_ARB;
		glActiveTextureARB(current_texture+GL_TEXTURE0_ARB);
	}
	prerender_setup();
}

void StateManager::want_active_client_texture_ARB(GLenum tmu_id) {
	if(((current_client+GL_TEXTURE0_ARB)!=tmu_id) && hasMultitexture) {
		current_client=tmu_id-GL_TEXTURE0_ARB;
		glClientActiveTextureARB(current_client+GL_TEXTURE0_ARB);
	}
	prerender_setup();
}

void StateManager::synchronize() {
	int j;
	for(j=0;j<8;j++) {
		if(textures[j].client_state_enabled) {
			textures[j].client_state_enabled_copy=0;
		}
		else {
			textures[j].client_state_enabled_copy=1;
		}
		textures[j].client_state_enabled_copy=0;
		textures[j].coord_pointer_copy=NULL;
		if(textures[j].enabled) {
			textures[j].enabled_copy=0;
		}
		else {
			textures[j].enabled_copy=1;
		}
		textures[j].tex_copy=NULL;
		textures[j].texture_env_copy=GL_NONE;
	}
	if(hasMultitexture) {
		glActiveTextureARB(GL_TEXTURE0_ARB+current_texture);
		glClientActiveTextureARB(GL_TEXTURE0_ARB+current_client);
	}
	c_state->prerender_setup();
	for(j=0;j<NUM_STATES;j++) {
		switch(states[j]) {
		case SM_LIGHTING:
			if(states[j]) glEnable(GL_LIGHTING);
			else glDisable(GL_LIGHTING);
		break;
		case SM_DEPTH_TEST:
			if(states[j]) glEnable(GL_DEPTH_TEST);
			else glDisable(GL_DEPTH_TEST);
		break;
		case SM_STENCIL_TEST:
			if(states[j]) glEnable(GL_STENCIL_TEST);
			else glDisable(GL_STENCIL_TEST);
		break;
		case SM_ALPHA_TEST:
			if(states[j]) glEnable(GL_ALPHA_TEST);
			else glDisable(GL_ALPHA_TEST);
		break;
		case SM_FOG:
			if(states[j]) glEnable(GL_FOG);
			else glDisable(GL_FOG);
		break;
		case SM_LIGHT0:
			if(states[j]) glEnable(GL_LIGHT0);
			else glDisable(GL_LIGHT0);
		break;
		case SM_LIGHT1:
			if(states[j]) glEnable(GL_LIGHT1);
			else glDisable(GL_LIGHT1);
		break;
		case SM_LIGHT2:
			if(states[j]) glEnable(GL_LIGHT2);
			else glDisable(GL_LIGHT2);
		break;
		case SM_LIGHT3:
			if(states[j]) glEnable(GL_LIGHT3);
			else glDisable(GL_LIGHT3);
		break;
		case SM_LIGHT4:
			if(states[j]) glEnable(GL_LIGHT4);
			else glDisable(GL_LIGHT4);
		break;
		case SM_LIGHT5:
			if(states[j]) glEnable(GL_LIGHT5);
			else glDisable(GL_LIGHT5);
		break;
		case SM_LIGHT6:
			if(states[j]) glEnable(GL_LIGHT6);
			else glDisable(GL_LIGHT6);
		break;
		case SM_LIGHT7:
			if(states[j]) glEnable(GL_LIGHT7);
			else glDisable(GL_LIGHT7);
		break;
		case SM_CULL_FACE:
			if(states[j]) glEnable(GL_CULL_FACE);
			else glDisable(GL_CULL_FACE);
		break;
		case SM_BLEND:
			if(states[j]) glEnable(GL_BLEND);
			else glDisable(GL_BLEND);
		break;
		}
	}
	
}

void StateManager::dump_state_to_framelog() {
	DebugFrameMessage("Dumping OpenGL state ...\n");
	DebugFrameMessage("GL_LIGHTING - %d\n",states[SM_LIGHTING]);
	DebugFrameMessage("GL_DEPTH_TEST - %d\n",states[SM_DEPTH_TEST]);
	DebugFrameMessage("GL_STENCIL_TEST - %d\n",states[SM_STENCIL_TEST]);
	DebugFrameMessage("GL_ALPHA_TEST - %d\n",states[SM_ALPHA_TEST]);
	DebugFrameMessage("GL_FOG - %d\n",states[SM_FOG]);
	DebugFrameMessage("GL_LIGHT0 - %d\n",states[SM_LIGHT0]);
	DebugFrameMessage("GL_LIGHT1 - %d\n",states[SM_LIGHT1]);
	DebugFrameMessage("GL_LIGHT2 - %d\n",states[SM_LIGHT2]);
	DebugFrameMessage("GL_LIGHT3 - %d\n",states[SM_LIGHT3]);
	DebugFrameMessage("GL_LIGHT4 - %d\n",states[SM_LIGHT4]);
	DebugFrameMessage("GL_LIGHT5 - %d\n",states[SM_LIGHT5]);
	DebugFrameMessage("GL_LIGHT6 - %d\n",states[SM_LIGHT6]);
	DebugFrameMessage("GL_LIGHT7 - %d\n",states[SM_LIGHT7]);
	DebugFrameMessage("GL_BLEND - %d\n",states[SM_BLEND]);
	DebugFrameMessage("Textures:\n");
	DebugFrameMessage("Current texture is %d\n",current_texture);
	unsigned int i;
	for(i=0;i<max_texture_units;i++) {
		DebugFrameMessage("Texture unit %d:\n",i);
		DebugFrameMessage("Enabled - %d\n",textures[i].enabled);
		DebugFrameMessage("Enabled_copy - %d\n",textures[i].enabled_copy);
		DebugFrameMessage("Pointer - 0x%x\n",textures[i].tex);
		DebugFrameMessage("Pointer copy - 0x%x\n",textures[i].tex_copy);
		DebugFrameMessage("Texture environment function is 0x%x\n",textures[i].texture_env);
		DebugFrameMessage("Texture environment function copy is 0x%x\n",
			textures[i].texture_env_copy);
		DebugFrameMessage("(GL_REPLACE=0x%x, GL_MODULATE=0x%x, GL_ADD=0x%x)\n",
			GL_REPLACE,GL_MODULATE,GL_ADD);
	}
}

//DEPRECATED!!!
void StateManager::want_client_state_enabled(GLenum tmu_id) {
	textures[tmu_id-GL_TEXTURE0_ARB].client_state_enabled=1;
	prerender_setup();
}

//DEPRECATED!!!
void StateManager::want_client_state_disabled(GLenum tmu_id) {
	textures[tmu_id-GL_TEXTURE0_ARB].client_state_enabled=0;
	prerender_setup();
}


//DEPRECATED!!!
void StateManager::want_tmu_disabled(GLenum tmu_id) {
	textures[tmu_id-GL_TEXTURE0_ARB].client_state_enabled=0;
	textures[tmu_id-GL_TEXTURE0_ARB].enabled=0;
	prerender_setup();
}

StateManager state;
StateManager alt_state;
StateManager* c_state;