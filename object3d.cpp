/***********************************************************************************************
                   object3d.cpp - implementacja Surface, Object3D itp
				   copyright (c) 2000 Maciej Mroz
***********************************************************************************************/

#include "engine.h"
#include <math.h>
#include "mathutils.h"
#include "glstate.h"
#include "debug_msg.h"

#pragma warning( disable : 4305 ) //const double->float 
#pragma warning( disable : 4244 ) //double->float and int->float

#define PI 3.14159265f
#define TWOPI 6.28318530f
#define HALFPI 1.57079632f

/**********
 VertexBuffer class
***********/

void VertexBuffer::init() {
	norm_coords=NULL;
	num_verts=0;
	int i;
	for(i=0;i<NUM_TEXTURES;i++) {
		tex_coords[i]=NULL;
	}
	vert_coords=NULL;
	indices=NULL;
	num_tris=0;
	max_ind=0;
	min_ind=0;
	is_external=0;
}

void VertexBuffer::clean() {
	if(!is_external) {
		if(norm_coords) delete norm_coords;
		if(vert_coords) delete vert_coords;
		int i;
		for(i=0;i<NUM_TEXTURES;i++) {
			if(tex_coords[i]) delete tex_coords[i];
		}
	}
	if(indices) delete indices;
	init();
}

void VertexBuffer::optimize() {
	float* tmp_verts;
	float* tmp_normals;
	unsigned int i;
	tmp_verts=(float*)agp_alloc->get_agp_block(3*num_verts*sizeof(float));
	if(!tmp_verts) return;
	//normals
	if(norm_coords) {
		tmp_normals=(float*)agp_alloc->get_agp_block(3*num_verts*sizeof(float));
		if(!tmp_normals) return;
	}
	//texture coordinates
	int j;
	float* new_tex_coords[NUM_TEXTURES];
	for(j=0;j<NUM_TEXTURES;j++) {
		new_tex_coords[j]=NULL;
		if(tex_coords[j]) {
			new_tex_coords[j]=(float*)agp_alloc->get_agp_block(2*num_verts*sizeof(float));
			if(!new_tex_coords[j]) return;
		}
	}

	for(i=0;i<num_verts;i++) {
		tmp_verts[3*i]=vert_coords[3*i];
		tmp_verts[3*i+1]=vert_coords[3*i+1];
		tmp_verts[3*i+2]=vert_coords[3*i+2];
	}
	delete vert_coords;
	vert_coords=tmp_verts;
	if(norm_coords) {
		for(i=0;i<num_verts;i++) {
			tmp_normals[3*i]=norm_coords[3*i];
			tmp_normals[3*i+1]=norm_coords[3*i+1];
			tmp_normals[3*i+2]=norm_coords[3*i+2];
		}
		delete norm_coords;
		norm_coords=tmp_normals;
	}
	for(j=0;j<NUM_TEXTURES;j++) {
		if(tex_coords[j]) {
			for(i=0;i<num_verts;i++) {
				new_tex_coords[j][2*i]=tex_coords[j][2*i];
				new_tex_coords[j][2*i+1]=tex_coords[j][2*i+1];
			}
			delete tex_coords[j];
			tex_coords[j]=new_tex_coords[j];
		}
	}
	is_external=1;
}

/**********
 Surface class implementation
***********/

Surface::Surface(Object3D *p) {
	mod_stack=NULL;
	vb=new VertexBuffer();
	mod_vb=new VertexBuffer();

	parent=p;
	reflectivity=0.0f;
	spec[0]=spec[1]=spec[2]=spec[3]=0.0f;
	amb[0]=amb[1]=amb[2]=amb[3]=1.0f;
	col[0]=col[1]=col[2]=col[3]=1.0f;
	dif[0]=dif[1]=dif[2]=dif[3]=1.0f;
	emission[0]=emission[1]=emission[2]=emission[3]=0.0f;
	render_flags=0;
	int i;
	for(i=0;i<NUM_TEXTURES;i++) {
		textures[i]=NULL;
	}
	for(i=0;i<3*NUM_TEXTURES;i++) {
		tex_velocities[i]=0.0f;
	}
	
	is_particle_system=0;
	particle_coords=NULL;
	particle_size=0.01f;
	alpha_ref=1.0f;
}

Surface::~Surface() {
	delete vb;
	delete mod_vb;
}

void Surface::attach_mod(VertexShader *m) {
	if(mod_stack==NULL) {
		mod_stack=m;
	}
	else {
		mod_stack->attach_vshader(m);
	}
}

void Surface::add_texture(Texture* texture,int texture_type) {
	textures[texture_type]=texture;
}

void Surface::render_particles() {
	if(textures[BASE]==NULL) return;
	Vector3 view_dir,cam_up,view_up,view_side;
	current_scene->get_camera()->get_direction(view_dir);
	current_scene->get_camera()->get_up_vector(cam_up);
	VCrossProduct3(cam_up,view_dir,view_side);
	VCrossProduct3(view_dir,view_side,view_up);
	VNormalize3(view_side);
	VNormalize3(view_up);
	Vector3 sx,sy;
	unsigned int i;
	Matrix mx,mx2;
	current_object->get_transform(mx);               //GET RID OF IT!!!!!!!
	//kill translation (we only need rotation part of matrix)
	mx[3]=0.0f;
	mx[7]=0.0f;
	mx[11]=0.0f;
	mx[12]=0.0f;
	mx[13]=0.0f;
	mx[14]=0.0f;
	//get the matrix to convert camera direction vectors
	MInvert(mx,mx2);


	Vector3 tmp_v;
	MVMultiply3(mx2,view_up,tmp_v);
	VCopy3(tmp_v,view_up);

	MVMultiply3(mx2,view_side,tmp_v);
	VCopy3(tmp_v,view_side);

/*#ifdef _LOG_EVERYTHING
	DebugFileMessage("particles.log","Matrix:\n");
	DebugFileMessage("particles.log","%f %f %f %f\n",mx[0],mx[1],mx[2],mx[3]);
	DebugFileMessage("particles.log","%f %f %f %f\n",mx[4],mx[5],mx[6],mx[7]);
	DebugFileMessage("particles.log","%f %f %f %f\n",mx[8],mx[9],mx[10],mx[11]);
	DebugFileMessage("particles.log","%f %f %f %f\n",mx[12],mx[13],mx[14],mx[15]);
#endif*/

	float* vert_coords=vb->vert_coords;
	for(i=0;i<vb->num_verts;i++) {
		sx[0]=view_side[0];
		sx[1]=view_side[1];
		sx[2]=view_side[2];

		sy[0]=view_up[0];
		sy[1]=view_up[1];
		sy[2]=view_up[2];

		VSMultiply3(sx,particle_size);
		VSMultiply3(sy,particle_size);
		

		particle_coords[4*3*i]=vert_coords[3*i]-sx[0]+sy[0];
		particle_coords[4*3*i+1]=vert_coords[3*i+1]-sx[1]+sy[1];
		particle_coords[4*3*i+2]=vert_coords[3*i+2]-sx[2]+sy[2];

		particle_coords[4*3*i+3]=vert_coords[3*i]+sx[0]+sy[0];
		particle_coords[4*3*i+4]=vert_coords[3*i+1]+sx[1]+sy[1];
		particle_coords[4*3*i+5]=vert_coords[3*i+2]+sx[2]+sy[2];

		particle_coords[4*3*i+6]=vert_coords[3*i]+sx[0]-sy[0];
		particle_coords[4*3*i+7]=vert_coords[3*i+1]+sx[1]-sy[1];
		particle_coords[4*3*i+8]=vert_coords[3*i+2]+sx[2]-sy[2];

		particle_coords[4*3*i+9]=vert_coords[3*i]-sx[0]-sy[0];
		particle_coords[4*3*i+10]=vert_coords[3*i+1]-sx[1]-sy[1];
		particle_coords[4*3*i+11]=vert_coords[3*i+2]-sx[2]-sy[2];
	}
	//render them now
	glColor4fv(col);

	for(i=0;i<max_texture_units;i++) {
		c_state->textures[i].client_state_enabled=0;
		c_state->textures[i].clear_tex_gen_flags();
		c_state->textures[i].enabled=0;                 //!!!!!!!!!!!!!!!!!!!!!!!
	}
	c_state->textures[0].client_state_enabled=1;
	c_state->textures[0].coord_pointer=vb->tex_coords[BASE];
	c_state->textures[0].tex=textures[BASE];
	c_state->textures[0].enabled=1;
	c_state->textures[0].set_texture_env(GL_MODULATE);

	glDisableClientState(GL_NORMAL_ARRAY);
	c_state->prerender_setup();
	glDepthMask(GL_FALSE);
	c_state->want_enabled(GL_BLEND);
	c_state->want_disabled(GL_LIGHTING);
	c_state->want_disabled(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glVertexPointer(3,GL_FLOAT,0,particle_coords);

	if(hasVertexArrayRange) {
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	glDrawArrays(GL_QUADS,0,4*vb->num_verts);
	if(hasVertexArrayRange) {
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}

	glDepthMask(GL_TRUE);
	c_state->postrender_setup();


	c_state->want_enabled(GL_LIGHTING);
	glEnableClientState(GL_NORMAL_ARRAY);
	
}

void Surface::draw_very_simple() {
	unsigned int i,j;
	glBegin(GL_TRIANGLES);
	for(i=0;i<3*vb->num_tris;i++) {
		if(hasMultitexture) {
			for(j=0;j<((max_texture_units>8) ? 8 : max_texture_units);j++) {
				if((c_state->textures[j].enabled) &&
					(c_state->textures[j].client_state_enabled)) {
					glMultiTexCoord2fvARB(GL_TEXTURE0_ARB+j,
						&c_state->textures[j].coord_pointer[2*vb->indices[i]]);
				}
			}
		}
		else {
			if((c_state->textures[0].enabled) &&
				(c_state->textures[0].client_state_enabled)) {
				glTexCoord2fv(&c_state->textures[0].coord_pointer[2*vb->indices[i]]);
			}
		}
		glNormal3fv(&vb->norm_coords[3*vb->indices[i]]);
		glVertex3fv(&vb->vert_coords[3*vb->indices[i]]);
	}
	glEnd();
}

void Surface::draw_simple() {
	if(render_flags & RM_DOUBLE_SIDED_BIT) {
		c_state->want_disabled(GL_CULL_FACE);
	}
	else {
		c_state->want_enabled(GL_CULL_FACE);
	}
	if(immediate_mode) {
		unsigned int i,j;
		glBegin(GL_TRIANGLES);
		for(i=0;i<3*vb->num_tris;i++) {
			if(hasMultitexture) {
				for(j=0;j<((max_texture_units>8) ? 8 : max_texture_units);j++) {
					if((c_state->textures[j].enabled) &&
						(c_state->textures[j].client_state_enabled)) {
						glMultiTexCoord2fvARB(GL_TEXTURE0_ARB+j,
							&c_state->textures[j].coord_pointer[2*vb->indices[i]]);
					}
				}
			}
			else {
				if((c_state->textures[0].enabled) &&
					(c_state->textures[0].client_state_enabled)) {
					glTexCoord2fv(&c_state->textures[0].coord_pointer[2*vb->indices[i]]);
				}
			}
			glNormal3fv(&vb->norm_coords[3*vb->indices[i]]);
			glVertex3fv(&vb->vert_coords[3*vb->indices[i]]);
		}
		glEnd();
		return;
	}
#ifdef _LOG_EVERYTHING
			DebugFrameMessage("Calling DrawElements(GL_TRIANGLES,%d,GL_UNISIGNED_SHORT,0x%d)\n",
				3*vb->num_tris,vb->indices);
#endif
	if(hasCompiledVertexArray) {
		glLockArraysEXT(vb->min_ind,vb->max_ind-vb->min_ind+1);
	}
	if((!vb->is_external) &&     //this should never happen :-)
		hasVertexArrayRange) {
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	glDrawElements(GL_TRIANGLES,3*vb->num_tris,GL_UNSIGNED_SHORT,vb->indices);
	if((!vb->is_external) &&
		hasVertexArrayRange) {
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	if(hasCompiledVertexArray) {
		glUnlockArraysEXT();
	}
}

//deprecated
static void setup_single_texturing() {
	if(hasMultitexture) {
		unsigned int i;
		for(i=1;i<max_texture_units;i++) {
			c_state->want_tmu_disabled(GL_TEXTURE0_ARB+i);
		}
		c_state->want_active_client_texture_ARB(GL_TEXTURE0_ARB);
		c_state->want_active_texture_ARB(GL_TEXTURE0_ARB);
	}
	c_state->want_enabled(GL_TEXTURE_2D);
}

void Surface::render(float frame) {
	unsigned int modify_flags=0;
	if(mod_stack!=NULL) {
		modify_flags=mod_stack->vshade(0,frame);
	}
	if(is_particle_system) {
		render_particles();             //check morphing !!!!!!!!
		return;
	}
#ifdef _LOG_EVERYTHING
	dump_to_framelog();
#endif
	float whiteness[4];
	whiteness[0]=whiteness[1]=whiteness[2]=1.0f;
	whiteness[3]=dif[3];
	float blackness[4];
	blackness[0]=blackness[1]=blackness[2]=0.0f;
	blackness[3]=dif[3];
	float tmp_spec[4];
	tmp_spec[0]=spec[0]*0.1f;
	tmp_spec[1]=spec[1]*0.1f;
	tmp_spec[2]=spec[2]*0.1f;
	tmp_spec[3]=spec[3]*0.1f;
	unsigned int i;
	glColor4fv(col);
	if(render_flags & RM_BASE_TEXTURE_BIT) {
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,whiteness);
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,whiteness);
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,emission);
//		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,blackness);
	}
	else {
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,col);	
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,dif);
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,emission);
//		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,tmp_spec);
	}
	if(render_flags & RM_ALPHA_TEST_BIT) {
		glAlphaFunc(GL_LEQUAL,alpha_ref);
		c_state->want_enabled(GL_ALPHA_TEST);
	}
	else {
		c_state->want_disabled(GL_ALPHA_TEST);
	}
	if((render_flags & RM_TRANSPARENT_BIT) ||
		(render_flags & RM_ADDITIVE_BIT)) {
		c_state->want_enabled(GL_BLEND);
	}
	else {
		c_state->want_disabled(GL_BLEND);
	}
	if(render_flags & RM_ADDITIVE_BIT) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else if(render_flags & RM_TRANSPARENT_BIT) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

	if(modify_flags & VS_VERTICES) {
		glVertexPointer(3,GL_FLOAT,0,mod_vb->vert_coords);
	}
	else {
		glVertexPointer(3,GL_FLOAT,0,vb->vert_coords);
	}
	glNormalPointer(GL_FLOAT,0,vb->norm_coords);

	unsigned int texture_render_flags;
	texture_render_flags=render_flags & 0xff;
	switch(texture_render_flags) {
	case RM_BASE_TEXTURE_BIT | RM_LUMINOSITY_TEXTURE_BIT:
		if(render_flags & RM_TRANSPARENT_BIT) {
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			if(0/*hasMultitexture && hasTextureEnvCombine*/) {
				c_state->textures[0].clear_tex_gen_flags();
				c_state->textures[0].client_state_enabled=1;
				c_state->textures[0].coord_pointer=vb->tex_coords[BASE];
				c_state->textures[0].enabled=1;
				c_state->textures[0].tex=textures[BASE];
				if((tex_velocities[3*BASE]+tex_velocities[3*BASE+1])!=0.0f) {
					c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*BASE];
					c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*BASE+1];
				}
				c_state->textures[0].set_texture_env(GL_MODULATE);

				c_state->textures[1].clear_tex_gen_flags();
				c_state->textures[1].enabled=1;
				c_state->textures[1].client_state_enabled=1;
				c_state->textures[1].tex=textures[LUMINOSITY];
				c_state->textures[1].coord_pointer=vb->tex_coords[LUMINOSITY];
				if((tex_velocities[3*LUMINOSITY]+tex_velocities[3*LUMINOSITY+1])!=0.0f) {
					c_state->textures[1].tex_offset[0]=0.0333f*frame*tex_velocities[3*LUMINOSITY];
					c_state->textures[1].tex_offset[1]=0.0333f*frame*tex_velocities[3*LUMINOSITY+1];
				}
				c_state->textures[1].set_texture_env(GL_COMBINE_EXT);
				//we need separate alpha processing
				//by default EXT_texture_env_add modulates alpha component
				//so we have to use EXT_texture_env_combine instead
				float env_col[4]={0.0f,0.0f,0.0f,0.0f};
				glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,env_col);

				c_state->textures[1].env_combine_param(GL_COMBINE_RGB_EXT,GL_ADD);
				c_state->textures[1].env_combine_param(GL_COMBINE_ALPHA_EXT,GL_REPLACE);
				c_state->textures[1].env_combine_param(GL_SOURCE0_RGB_EXT,GL_PREVIOUS_EXT);
				c_state->textures[1].env_combine_param(GL_SOURCE1_RGB_EXT,GL_TEXTURE);
				c_state->textures[1].env_combine_param(GL_SOURCE0_ALPHA_EXT,GL_PREVIOUS_EXT);
				c_state->textures[1].env_combine_param(GL_OPERAND0_RGB_EXT,GL_SRC_COLOR);
				c_state->textures[1].env_combine_param(GL_OPERAND1_RGB_EXT,GL_SRC_COLOR);
				c_state->textures[1].env_combine_param(GL_OPERAND0_ALPHA_EXT,GL_SRC_ALPHA);
				c_state->textures[1].force_combine_update=1;
				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();
			}
			else {  //multipass
				for(i=0;i<max_texture_units;i++) {
					c_state->textures[i].clear_tex_gen_flags();
					c_state->textures[i].enabled=0;
					c_state->textures[i].client_state_enabled=0;
				}
				c_state->textures[0].enabled=1;
				c_state->textures[0].client_state_enabled=1;
				c_state->textures[0].coord_pointer=vb->tex_coords[BASE];
				c_state->textures[0].tex=textures[BASE];
				if((tex_velocities[3*BASE]+tex_velocities[3*BASE+1])!=0.0f) {
					c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*BASE];
					c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*BASE+1];
				}
				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();
				c_state->textures[0].tex=textures[LUMINOSITY];
				if((tex_velocities[3*LUMINOSITY]+tex_velocities[3*LUMINOSITY+1])!=0.0f) {
					c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*LUMINOSITY];
					c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*LUMINOSITY+1];
				}
				c_state->textures[0].coord_pointer=vb->tex_coords[LUMINOSITY];
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
				c_state->want_enabled(GL_BLEND);
				c_state->textures[0].set_texture_env(GL_REPLACE);
				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();
				c_state->textures[0].set_texture_env(GL_MODULATE);
			}
		}
		else {
			if(hasMultitexture && hasTextureEnvAdd) {
				c_state->textures[0].clear_tex_gen_flags();
				c_state->textures[0].enabled=1;
				c_state->textures[0].client_state_enabled=1;
				c_state->textures[0].tex=textures[BASE];
				c_state->textures[0].coord_pointer=vb->tex_coords[BASE];
				if((tex_velocities[3*BASE]+tex_velocities[3*BASE+1])!=0.0f) {
					c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*BASE];
					c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*BASE+1];
				}
				c_state->textures[0].set_texture_env(GL_MODULATE);

				c_state->textures[1].clear_tex_gen_flags();
				c_state->textures[1].enabled=1;
				c_state->textures[1].client_state_enabled=1;
				c_state->textures[1].tex=textures[LUMINOSITY];
				c_state->textures[1].coord_pointer=vb->tex_coords[LUMINOSITY];
				if((tex_velocities[3*LUMINOSITY]+tex_velocities[3*LUMINOSITY+1])!=0.0f) {
					c_state->textures[1].tex_offset[1]=0.0333f*frame*tex_velocities[3*LUMINOSITY];
					c_state->textures[1].tex_offset[1]=0.0333f*frame*tex_velocities[3*LUMINOSITY+1];
				}
				c_state->textures[1].set_texture_env(GL_ADD);
				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();
			}
			else {  //multipass version
				for(i=0;i<max_texture_units;i++) {
					c_state->textures[i].clear_tex_gen_flags();
					c_state->textures[i].enabled=0;
					c_state->textures[i].client_state_enabled=0;
				}
				c_state->textures[0].enabled=1;
				c_state->textures[0].client_state_enabled=1;
				c_state->textures[0].coord_pointer=vb->tex_coords[BASE];
				c_state->textures[0].tex=textures[BASE];
				if((tex_velocities[3*BASE]+tex_velocities[3*BASE+1])!=0.0f) {
					c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*BASE];
					c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*BASE+1];
				}
				c_state->textures[0].set_texture_env(GL_MODULATE);

				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();
				c_state->textures[0].coord_pointer=vb->tex_coords[LUMINOSITY];
				c_state->textures[0].tex=textures[LUMINOSITY];
				if((tex_velocities[3*LUMINOSITY]+tex_velocities[3*LUMINOSITY+1])!=0.0f) {
					c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*LUMINOSITY];
					c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*LUMINOSITY+1];
				}
				c_state->textures[0].set_texture_env(GL_REPLACE);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
				c_state->want_enabled(GL_BLEND);
				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();
			}
		}
	break;
	case RM_BASE_TEXTURE_BIT | RM_SPECULAR_TEXTURE_BIT:
		if(render_flags & RM_TRANSPARENT_BIT) {
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			if(0 /*hasMultitexture && hasTextureEnvCombine*/) {
				c_state->textures[0].clear_tex_gen_flags();
				c_state->textures[0].client_state_enabled=1;
				c_state->textures[0].coord_pointer=vb->tex_coords[BASE];
				c_state->textures[0].enabled=1;
				c_state->textures[0].tex=textures[BASE];
				if((tex_velocities[3*BASE]+tex_velocities[3*BASE+1])!=0.0f) {
					c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*BASE];
					c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*BASE+1];
				}
				c_state->textures[0].set_texture_env(GL_MODULATE);

				c_state->textures[1].enabled=1;
				c_state->textures[1].client_state_enabled=0;
				c_state->textures[1].tex=textures[SPECULAR];
				if((tex_velocities[3*SPECULAR]+tex_velocities[3*SPECULAR+1])!=0.0f) {
					c_state->textures[1].tex_offset[0]=0.0333f*frame*tex_velocities[3*SPECULAR];
					c_state->textures[1].tex_offset[1]=0.0333f*frame*tex_velocities[3*SPECULAR+1];
				}
				c_state->textures[1].tex_gen_s_enabled=1;
				c_state->textures[1].tex_gen_s_mode=GL_SPHERE_MAP;
				c_state->textures[1].tex_gen_t_enabled=1;
				c_state->textures[1].tex_gen_t_mode=GL_SPHERE_MAP;
				c_state->textures[1].set_texture_env(GL_COMBINE_EXT);
				float env_col[4]={0.0f,0.0f,0.0f,0.0f};
				glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,env_col);
				c_state->textures[1].env_combine_param(GL_COMBINE_RGB_EXT,GL_ADD);
				c_state->textures[1].env_combine_param(GL_COMBINE_ALPHA_EXT,GL_REPLACE);
				c_state->textures[1].env_combine_param(GL_SOURCE0_RGB_EXT,GL_PREVIOUS_EXT);
				c_state->textures[1].env_combine_param(GL_SOURCE1_RGB_EXT,GL_TEXTURE);
				c_state->textures[1].env_combine_param(GL_SOURCE0_ALPHA_EXT,GL_PREVIOUS_EXT);
				c_state->textures[1].env_combine_param(GL_OPERAND0_RGB_EXT,GL_SRC_COLOR);
				c_state->textures[1].env_combine_param(GL_OPERAND1_RGB_EXT,GL_SRC_COLOR);
				c_state->textures[1].env_combine_param(GL_OPERAND0_ALPHA_EXT,GL_SRC_ALPHA);
				c_state->textures[1].force_combine_update=1;

				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();
			}
			else {
				for(i=0;i<max_texture_units;i++) {
					c_state->textures[i].clear_tex_gen_flags();
					c_state->textures[i].enabled=0;
					c_state->textures[i].client_state_enabled=0;
				}
				c_state->textures[0].enabled=1;
				c_state->textures[0].client_state_enabled=1;
				c_state->textures[0].coord_pointer=vb->tex_coords[BASE];
				c_state->textures[0].tex=textures[BASE];
				if((tex_velocities[3*BASE]+tex_velocities[3*BASE+1])!=0.0f) {
					c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*BASE];
					c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*BASE+1];
				}
				c_state->textures[0].set_texture_env(GL_MODULATE);

				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();

				c_state->textures[0].tex=textures[SPECULAR];
				c_state->textures[0].client_state_enabled=0;
				c_state->textures[0].tex_gen_s_enabled=1;
				c_state->textures[0].tex_gen_s_mode=GL_SPHERE_MAP;
				c_state->textures[0].tex_gen_t_enabled=1;
				c_state->textures[0].tex_gen_t_mode=GL_SPHERE_MAP;
				c_state->textures[0].set_texture_env(GL_REPLACE);

				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();
			}
		}
		else {
			if(hasMultitexture && hasTextureEnvAdd) {
				c_state->textures[0].enabled=1;
				c_state->textures[0].client_state_enabled=1;
				c_state->textures[0].coord_pointer=vb->tex_coords[BASE];
				c_state->textures[0].tex=textures[BASE];
				c_state->textures[0].clear_tex_gen_flags();
				if((tex_velocities[3*BASE]+tex_velocities[3*BASE+1])!=0.0f) {
					c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*BASE];
					c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*BASE+1];
				}
				c_state->textures[0].set_texture_env(GL_MODULATE);

				c_state->textures[1].enabled=1;
				c_state->textures[1].client_state_enabled=0;
				c_state->textures[1].tex=textures[SPECULAR];
				if((tex_velocities[3*SPECULAR]+tex_velocities[3*SPECULAR+1])!=0.0f) {
					c_state->textures[1].tex_offset[0]=0.0333f*frame*tex_velocities[3*SPECULAR];
					c_state->textures[1].tex_offset[1]=0.0333f*frame*tex_velocities[3*SPECULAR+1];
				}
				c_state->textures[1].set_texture_env(GL_ADD);
				c_state->textures[1].tex_gen_s_enabled=1;
				c_state->textures[1].tex_gen_s_mode=GL_SPHERE_MAP;
				c_state->textures[1].tex_gen_t_enabled=1;
				c_state->textures[1].tex_gen_t_mode=GL_SPHERE_MAP;

				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();
			}
			else {  //multipass version
				for(i=0;i<max_texture_units;i++) {
					c_state->textures[i].clear_tex_gen_flags();
					c_state->textures[i].enabled=0;
					c_state->textures[i].client_state_enabled=0;
				}
				c_state->textures[0].enabled=1;
				c_state->textures[0].client_state_enabled=1;
				c_state->textures[0].coord_pointer=vb->tex_coords[BASE];
				c_state->textures[0].tex=textures[BASE];
				if((tex_velocities[3*BASE]+tex_velocities[3*BASE+1])!=0.0f) {
					c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*BASE];
					c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*BASE+1];
				}
				c_state->textures[0].set_texture_env(GL_MODULATE);

				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();

				c_state->textures[0].tex=textures[SPECULAR];
				c_state->textures[0].client_state_enabled=0;
				c_state->textures[0].tex_gen_s_enabled=1;
				c_state->textures[0].tex_gen_s_mode=GL_SPHERE_MAP;
				c_state->textures[0].tex_gen_t_enabled=1;
				c_state->textures[0].tex_gen_t_mode=GL_SPHERE_MAP;
				c_state->textures[0].set_texture_env(GL_REPLACE);

				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();
			}
		}
	break;
	default:   //try to do the best
		//render base texture and/or diffuse texture
		if(render_flags & RM_BASE_TEXTURE_BIT) {
			if(render_flags & RM_DIFFUSE_TEXTURE_BIT) {
				if(hasMultitexture) {
					c_state->textures[0].enabled=1;
					c_state->textures[0].client_state_enabled=1;
					c_state->textures[0].coord_pointer=vb->tex_coords[BASE];
					c_state->textures[0].tex=textures[BASE];
					c_state->textures[0].clear_tex_gen_flags();
					if((tex_velocities[3*BASE]+tex_velocities[3*BASE+1])!=0.0f) {
						c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*BASE];
						c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*BASE+1];
					}
					c_state->textures[0].set_texture_env(GL_MODULATE);
					
					c_state->textures[1].enabled=1;
					c_state->textures[1].client_state_enabled=1;
					c_state->textures[1].tex=textures[DIFFUSE];
					c_state->textures[1].clear_tex_gen_flags();
					c_state->textures[1].coord_pointer=vb->tex_coords[DIFFUSE];
					if((tex_velocities[3*DIFFUSE]+tex_velocities[3*DIFFUSE+1])!=0.0f) {
						c_state->textures[1].tex_offset[0]=0.0333f*frame*tex_velocities[3*DIFFUSE];
						c_state->textures[1].tex_offset[1]=0.0333f*frame*tex_velocities[3*DIFFUSE+1];
					}
					c_state->textures[1].set_texture_env(GL_MODULATE);

					c_state->prerender_setup();
					draw_simple();
					c_state->postrender_setup();
				}
				else {  //base texture only
					for(i=0;i<max_texture_units;i++) {
						c_state->textures[i].clear_tex_gen_flags();
						c_state->textures[i].enabled=0;
						c_state->textures[i].client_state_enabled=0;
					}
					c_state->textures[0].enabled=1;
					c_state->textures[0].client_state_enabled=1;
					c_state->textures[0].coord_pointer=vb->tex_coords[BASE];
					c_state->textures[0].tex=textures[BASE];
					if((tex_velocities[3*BASE]+tex_velocities[3*BASE+1])!=0.0f) {
						c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*BASE];
						c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*BASE+1];
					}
					c_state->textures[0].set_texture_env(GL_MODULATE);
					
					c_state->prerender_setup();
					draw_simple();
					c_state->postrender_setup();
				}
			}
			else { //base texture only
				for(i=0;i<max_texture_units;i++) {
					c_state->textures[i].clear_tex_gen_flags();
					c_state->textures[i].enabled=0;
					c_state->textures[i].client_state_enabled=0;
				}
				c_state->textures[0].enabled=1;
				c_state->textures[0].client_state_enabled=1;
				c_state->textures[0].coord_pointer=vb->tex_coords[BASE];
				c_state->textures[0].tex=textures[BASE];
				if((tex_velocities[3*BASE]+tex_velocities[3*BASE+1])!=0.0f) {
					c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*BASE];
					c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*BASE+1];
				}
				c_state->textures[0].set_texture_env(GL_MODULATE);

				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();
			}
		}
		else if(render_flags & RM_DIFFUSE_TEXTURE_BIT) {
			for(i=0;i<max_texture_units;i++) {
				c_state->textures[i].clear_tex_gen_flags();
				c_state->textures[i].enabled=0;
				c_state->textures[i].client_state_enabled=0;
			}
			c_state->textures[0].enabled=1;
			c_state->textures[0].client_state_enabled=1;
			c_state->textures[0].coord_pointer=vb->tex_coords[DIFFUSE];
			c_state->textures[0].tex=textures[DIFFUSE];
			if((tex_velocities[3*DIFFUSE]+tex_velocities[3*DIFFUSE+1])!=0.0f) {
				c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*DIFFUSE];
				c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*DIFFUSE+1];
			}
			c_state->textures[0].set_texture_env(GL_MODULATE);
			
			c_state->prerender_setup();
			draw_simple();
			c_state->postrender_setup();
		}
		else { //color only
			if(!(render_flags & RM_LUMINOSITY_TEXTURE_BIT)) {
				//we usually don't want color layer 
				//when luminosity texture is present              - it's a speed hack!!!!
				for(i=0;i<max_texture_units;i++) {
					c_state->textures[i].clear_tex_gen_flags();
					c_state->textures[i].enabled=0;
					c_state->textures[i].client_state_enabled=0;
				}
				c_state->prerender_setup();
				draw_simple();
			}
		}

		//render the rest (specular, luminosity etc)
		c_state->want_enabled(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);

		if((render_flags & RM_GLOSS_TEXTURE_BIT) &&
			(render_flags & RM_SPECULAR_TEXTURE_BIT)) {
			if(hasMultitexture) {
				c_state->textures[0].clear_tex_gen_flags();
				c_state->textures[0].enabled=1;
				c_state->textures[0].client_state_enabled=1;
				c_state->textures[0].coord_pointer=vb->tex_coords[GLOSS];
				c_state->textures[0].tex=textures[GLOSS];
				c_state->textures[0].set_texture_env(GL_MODULATE);
				if((tex_velocities[3*GLOSS]+tex_velocities[3*GLOSS+1])!=0.0f) {
					c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*GLOSS];
					c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*GLOSS+1];
				}

				c_state->textures[1].enabled=1;
				c_state->textures[1].client_state_enabled=0;
				c_state->textures[1].tex=textures[SPECULAR];
				c_state->textures[1].set_texture_env(GL_MODULATE);
				c_state->textures[1].tex_gen_s_enabled=1;
				c_state->textures[1].tex_gen_s_mode=GL_SPHERE_MAP;
				c_state->textures[1].tex_gen_t_enabled=1;
				c_state->textures[1].tex_gen_t_mode=GL_SPHERE_MAP;

				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();
			}
			else { //specular only
				for(i=0;i<max_texture_units;i++) {
					c_state->textures[i].clear_tex_gen_flags();
					c_state->textures[i].enabled=0;
					c_state->textures[i].client_state_enabled=0;
				}
				c_state->textures[0].enabled=1;
				c_state->textures[0].client_state_enabled=0;
				c_state->textures[0].tex_gen_s_enabled=1;
				c_state->textures[0].tex_gen_s_mode=GL_SPHERE_MAP;
				c_state->textures[0].tex_gen_t_enabled=1;
				c_state->textures[0].tex_gen_t_mode=GL_SPHERE_MAP;
				c_state->textures[0].tex=textures[SPECULAR];
				c_state->textures[0].set_texture_env(GL_REPLACE);
				c_state->textures[0].tex=textures[SPECULAR];
				c_state->prerender_setup();
				draw_simple();
				c_state->postrender_setup();
			}
		}
		else if(render_flags & RM_SPECULAR_TEXTURE_BIT) {
			for(i=0;i<max_texture_units;i++) {
				c_state->textures[i].clear_tex_gen_flags();
				c_state->textures[i].enabled=0;
				c_state->textures[i].client_state_enabled=0;
			}
			c_state->textures[0].enabled=1;
			c_state->textures[0].tex=textures[SPECULAR];
			c_state->textures[0].tex_gen_s_enabled=1;
			c_state->textures[0].tex_gen_s_mode=GL_SPHERE_MAP;
			c_state->textures[0].tex_gen_t_enabled=1;
			c_state->textures[0].tex_gen_t_mode=GL_SPHERE_MAP;
			c_state->textures[0].set_texture_env(GL_REPLACE);
			
			c_state->prerender_setup();
			draw_simple();
			c_state->postrender_setup();
		}
		if(render_flags & RM_LUMINOSITY_TEXTURE_BIT) {
			for(i=0;i<max_texture_units;i++) {
				c_state->textures[i].clear_tex_gen_flags();
				c_state->textures[i].enabled=0;
				c_state->textures[i].client_state_enabled=0;
			}
			c_state->textures[0].enabled=1;
			c_state->textures[0].client_state_enabled=1;
			c_state->textures[0].coord_pointer=vb->tex_coords[LUMINOSITY];
			c_state->textures[0].tex=textures[LUMINOSITY];
			if((tex_velocities[3*LUMINOSITY]+tex_velocities[3*LUMINOSITY+1])!=0.0f) {
				c_state->textures[0].tex_offset[0]=0.0333f*frame*tex_velocities[3*LUMINOSITY];
				c_state->textures[0].tex_offset[1]=0.0333f*frame*tex_velocities[3*LUMINOSITY+1];
			}
			c_state->textures[0].set_texture_env(GL_REPLACE);
			
			c_state->prerender_setup();
			draw_simple();
			c_state->postrender_setup();
		}
		if(render_flags & RM_REFLECTION_TEXTURE_BIT) {
			float tmp_col[4];
			tmp_col[0]=dif[0];
			tmp_col[1]=dif[1];
			tmp_col[2]=dif[2];
			tmp_col[3]=reflectivity;
			glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,tmp_col);
			glColor4fv(tmp_col);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			for(i=0;i<max_texture_units;i++) {
				c_state->textures[i].clear_tex_gen_flags();
				c_state->textures[i].enabled=0;
				c_state->textures[i].client_state_enabled=0;
			}
			c_state->textures[0].enabled=1;
			c_state->textures[0].tex=textures[REFLECTION];
			c_state->textures[0].tex_gen_s_enabled=1;
			c_state->textures[0].tex_gen_s_mode=GL_SPHERE_MAP;
			c_state->textures[0].tex_gen_t_enabled=1;
			c_state->textures[0].tex_gen_t_mode=GL_SPHERE_MAP;
			c_state->textures[0].set_texture_env(GL_MODULATE);
			c_state->want_disabled(GL_LIGHTING);
			c_state->prerender_setup();
			draw_simple();
			c_state->postrender_setup();
			c_state->want_enabled(GL_LIGHTING);
		}
	break;   
	}
	glColor4f(1.0f,1.0f,1.0f,1.0f);
}

void Surface::generate_normals() {
	Vector3 v0,v1,v2;
	unsigned int i;
	vb->norm_coords=new float[3*vb->num_verts];
	float *norm_coords=vb->norm_coords;
	float *vert_coords=vb->vert_coords;
	unsigned short *indices=vb->indices;
	for(i=0;i<3*vb->num_verts;i++) norm_coords[i]=0.0f;
	for(i=0;i<vb->num_tris;i++) {
		PDiff3(&vert_coords[3*indices[3*i]],
			&vert_coords[3*indices[3*i+1]],v0);
		PDiff3(&vert_coords[3*indices[3*i]],
			&vert_coords[3*indices[3*i+2]],v1);
		VCrossProduct3(v0,v1,v2);
		VAdd3(&norm_coords[3*indices[3*i]],
			v2,&norm_coords[3*indices[3*i]]);
		VAdd3(&norm_coords[3*indices[3*i+1]],
			v2,&norm_coords[3*indices[3*i+1]]);
		VAdd3(&norm_coords[3*indices[3*i+2]],
			v2,&norm_coords[3*indices[3*i+2]]);
	}
	for(i=0;i<vb->num_verts;i++) VNormalize3(&norm_coords[3*i]);
}

//funkcje pomocnicze generatora wspolrzednych UV
//thx to Newtek
static void xyztoh(float x,float y,float z,float *h)
{
    if ((x==0.0f) && (z==0.0f))
        *h = 0.0f;
    else {
        if (z==0.0f)
            *h = (x < 0.0f) ? HALFPI : -HALFPI;
        else if (z < 0.0f)
            *h = -atan(x / z) + PI;
        else
            *h = -atan(x / z);
    }
}

static void xyztohp(float x,float y,float z,float *h,float *p)
{
    if (x == 0.0f && z == 0.0f) {
        *h = 0.0f;
        if (y != 0.0f)
            *p = (y < 0.0f) ? -HALFPI : HALFPI;
        else
            *p = 0.0f;
    }
    else {
        if (z == 0.0f)
            *h = (x < 0.0f) ? HALFPI : -HALFPI;
        else if (z < 0.0f)
            *h = -atan(x / z) + PI;
        else
            *h = -atan(x / z);
        x = sqrt(x * x + z * z);
        if (x == 0.0f)
            *p = (y < 0.0f) ? -HALFPI : HALFPI;
        else
            *p = atan(y / x);
    }
}


int Surface::generate_tex_coords(const tex_parameters *t_desc,int texture_type) {
	if(vb->indices==NULL) return 0;

#ifdef _LOG_EVERYTHING
	DebugMessage("Mesh::generate_tex_coords - tex_parameters dump:\n");
	DebugMessage("\tTexture type - %s\n",t_desc->type);
	DebugMessage("\tTexture image - %s\n",t_desc->timg);
	DebugMessage("\tFlags - %d\n",t_desc->flags);
	if((t_desc->flags) & 1) DebugMessage("\tX axis alignment\n");
	if((t_desc->flags) & 2) DebugMessage("\tY axis alignment\n");
	if((t_desc->flags) & 4) DebugMessage("\tZ axis alignment\n");
	DebugMessage("\tTexture center - %f %f %f\n",t_desc->tctr[0],t_desc->tctr[1],t_desc->tctr[2]);
	DebugMessage("\tTexture size - %f %f %f\n",t_desc->tsiz[0],t_desc->tsiz[1],t_desc->tsiz[2]);
	int j;
	float min_x,max_x,min_y,max_y,min_z,max_z;
	min_x=max_x=vb->vert_coords[vb->indices[0]];
	min_y=max_y=vb->vert_coords[vb->indices[0]+1];
	min_z=max_z=vb->vert_coords[vb->indices[0]+2];
	for(j=1;j<3*vb->num_tris;j++) {
		if(min_x>vb->vert_coords[3*vb->indices[j]]) min_x=vb->vert_coords[3*vb->indices[j]];
		if(max_x<vb->vert_coords[3*vb->indices[j]]) max_x=vb->vert_coords[3*vb->indices[j]];

		if(min_y>vb->vert_coords[3*vb->indices[j]+1]) min_y=vb->vert_coords[3*vb->indices[j]+1];
		if(max_y<vb->vert_coords[3*vb->indices[j]+1]) max_y=vb->vert_coords[3*vb->indices[j]+1];

		if(min_z>vb->vert_coords[3*vb->indices[j]+2]) min_z=vb->vert_coords[3*vb->indices[j]+2];
		if(max_z<vb->vert_coords[3*vb->indices[j]+2]) max_z=vb->vert_coords[3*vb->indices[j]+2];
	}
	DebugMessage("\tmin_x: %f \tmax_x: %f\n",min_x,max_x);
	DebugMessage("\tmin_y: %f \tmax_y: %f\n",min_y,max_y);
	DebugMessage("\tmin_z: %f \tmax_z: %f\n",min_z,max_z);
#endif

	float x,y,z,lon,lat;
	unsigned int i;
	int texgen_done=0;
#ifdef _LOG_EVERYTHING
	DebugMessage("num_tris %d\n",vb->num_tris);
	DebugMessage("num_verts %d\n",vb->num_verts);
	for(i=0;i<3*vb->num_tris;i++) {
		if(vb->indices[i]>vb->num_verts) DebugMessage("OUT OF RANGE!!!!!!!!!!!!!!\n");
	}
#endif
	unsigned int num_verts=vb->num_verts;
	float *vert_coords=vb->vert_coords;
	float **tex_coords=vb->tex_coords;
	if(strcmp(t_desc->type,"Planar Image Map")==0) {
		tex_coords[texture_type]=new float[2*num_verts];
		for(i=0;i<num_verts;i++) {
			x=vert_coords[3*i]-t_desc->tctr[0];
			y=vert_coords[3*i+1]-t_desc->tctr[1];
			z=-vert_coords[3*i+2]-t_desc->tctr[2];
			if((t_desc->flags) & 1) {
				tex_coords[texture_type][2*i]=z/t_desc->tsiz[2]+0.5f;
			}
			else {
				tex_coords[texture_type][2*i]=x/t_desc->tsiz[0]+0.5f;
			}
			if((t_desc->flags) & 2) {
				tex_coords[texture_type][2*i+1]=-z/t_desc->tsiz[2]+0.5f;
			}
			else {
				tex_coords[texture_type][2*i+1]=-y/t_desc->tsiz[1]+0.5f;
			}
		}
		texgen_done=1;
	}
	else if(strcmp(t_desc->type,"Cylindrical Image Map")==0) {
		tex_coords[texture_type]=new float[2*num_verts];
		for(i=0;i<num_verts;i++) {
			x=vert_coords[3*i]-t_desc->tctr[0];
			y=vert_coords[3*i+1]-t_desc->tctr[1];
			z=-vert_coords[3*i+2]-t_desc->tctr[2];
			if ((t_desc->flags) & 1) {
				xyztoh(z,x,-y,&lon);
				tex_coords[texture_type][2*i+1] = -x / t_desc->tsiz[0] + .5;
			}
			else if ((t_desc->flags) & 2) {
				xyztoh(-x,y,z,&lon);
				tex_coords[texture_type][2*i+1] = -y / t_desc->tsiz[1] + .5;
			}
			else {
				xyztoh(-x,z,-y,&lon);
				tex_coords[texture_type][2*i+1] = -z / t_desc->tsiz[2] + .5;
			}
			lon = 1.0 - lon / TWOPI;
			if (t_desc->widthTiling != 1.0)
				lon = lon * t_desc->widthTiling;
			tex_coords[texture_type][2*i]=lon;
		}
		texgen_done=1;
	}
	else if(strcmp(t_desc->type,"Spherical Image Map")==0) {
		tex_coords[texture_type]=new float[2*num_verts];
		for(i=0;i<num_verts;i++) {
			x=vert_coords[3*i]-t_desc->tctr[0];
			y=vert_coords[3*i+1]-t_desc->tctr[1];
			z=-vert_coords[3*i+2]-t_desc->tctr[2];
			if ((t_desc->flags) & (1))
				xyztohp(z,x,-y,&lon,&lat);
			else if ((t_desc->flags) & (1<<1))
				xyztohp(-x,y,z,&lon,&lat);
			else xyztohp(-x,z,-y,&lon,&lat);
			lon = 1.0 - lon / TWOPI;
			lat = .5 - lat / PI;
			if (t_desc->widthTiling != 1.0)
				lon = lon * t_desc->widthTiling;
			if (t_desc->heightTiling != 1.0)
				lat = lat * t_desc->heightTiling;
			tex_coords[texture_type][2*i]=lon;
			tex_coords[texture_type][2*i+1]=lat;
		}
		texgen_done=1;
	}
	if(texgen_done) {

#ifdef _LOG_EVERYTHING
		DebugMessage("Mesh::generate_tex_coords - texture coordinates:\n");
		float min_s,max_s,min_t,max_t;
		min_s=max_s=tex_coords[texture_type][0];
		min_t=max_t=tex_coords[texture_type][1];
		for(j=1;j<num_verts;j++) {
			if(min_s>tex_coords[texture_type][2*j]) 
				min_s=tex_coords[texture_type][2*j];
			if(max_s<tex_coords[texture_type][2*j]) 
				max_s=tex_coords[texture_type][2*j];
	
			if(min_t>tex_coords[texture_type][2*j+1]) 
				min_t=tex_coords[texture_type][2*j+1];
			if(max_t<tex_coords[texture_type][2*j+1]) 
				max_t=tex_coords[texture_type][2*j+1];
		}
		DebugMessage("\tmin_s: %f \tmax_s: %f\n",min_s,max_s);
		DebugMessage("\tmin_t: %f \tmax_t: %f\n",min_t,max_t);
#endif

		if(t_desc->timg!=NULL) {
			switch(texture_type) {
			case BASE:
				render_flags|=RM_BASE_TEXTURE_BIT;
			break;
			case LIGHTMAP:
				render_flags|=RM_LIGHTMAP_TEXTURE_BIT;
			break;
			case LUMINOSITY:
				render_flags|=RM_LUMINOSITY_TEXTURE_BIT;
			break;
			case SPECULAR:
				render_flags|=RM_SPECULAR_TEXTURE_BIT;
			break;
			case DIFFUSE:
				render_flags|=RM_DIFFUSE_TEXTURE_BIT;
			break;
			case BUMP:
				render_flags|=RM_BUMP_TEXTURE_BIT;
			break;
			case REFLECTION:
				render_flags|=RM_REFLECTION_TEXTURE_BIT;
			break;
			case GLOSS:
				render_flags|=RM_GLOSS_TEXTURE_BIT;
			break;
			}
		}
	}
	return texgen_done;
}

void Surface::dump(const char* filename) {
	DebugFileMessage(filename,"************************************************************\n");
	DebugFileMessage(filename,"\t\t\tMesh content dump\n");
	DebugFileMessage(filename,"************************************************************\n");
	DebugFileMessage(filename,"Geometry data:\n");
	DebugFileMessage(filename,"Mesh has %d vertices forming %d triangles\n",
		vb->num_verts,vb->num_tris);
	DebugFileMessage(filename,"Vertex pointer 0x%x\n",vb->vert_coords);
	DebugFileMessage(filename,"Normal pointer 0x%x\n",vb->norm_coords);
	DebugFileMessage(filename,"Texture coordinate pointer 0x%x\n",vb->tex_coords);
	int i;
	DebugFileMessage(filename,"Checking coordinate sets ...\n");
	for(i=0;i<NUM_TEXTURES;i++) {
		DebugFileMessage(filename,"Coordinate set %d pointer is 0x%x\n",i,vb->tex_coords[i]);
	}
/*	DebugFileMessage(filename,"Dumping tables ... see geometry.log for details\n");
	for(i=0;i<num_verts;i++) {
		DebugFileMessage("geometry.log","Vertex %d:\nx:%f\ty:%f\tz:%f\nnx:%f\tny:%f\tnz:%f\n",i,
			vert_coords[3*i],vert_coords[3*i+1],vert_coords[3*i+2],
			norm_coords[3*i],norm_coords[3*i+1],norm_coords[3*i+2]);
		for(j=0;j<NUM_TEXTURES;j++) {
			if(tex_coords[j]) {
				DebugFileMessage("geometry.log","Texture %d:\tu:%f\tv:%f\n",j,tex_coords[j][2*i],
					tex_coords[j][2*i+1]);
			}
		}
	}*/
	DebugFileMessage(filename,"Vertex indices pointer 0x%x\n",vb->indices);
/*	DebugFileMessage(filename,"Dumping indices ... see geometry.log for details\n");
	for(i=0;i<num_tris;i++) {
		DebugFileMessage("geometry.log","Triangle %d:\tv:%d\tv:%d\tv:%d\n",
			i,indices[3*i],indices[3*i+1],indices[3*i+2]);
	}*/
	DebugFileMessage(filename,"Surface attributes:\n");
	DebugFileMessage(filename,"Render flags %d\n",render_flags);
	DebugFileMessage(filename,"Base color (used only when lighting is disabled):\n%f\t%f\t%f\t%f\n",
		col[0],col[1],col[2],col[3]);
	DebugFileMessage(filename,"Ambient color (not used):%f\t%f\t%f\t%f\n",
		amb[0],amb[1],amb[2],amb[3]);
	DebugFileMessage(filename,"Diffuse color:%f\t%f\t%f\t%f\n",
		dif[0],dif[1],dif[2],dif[3]);
	DebugFileMessage(filename,"Specular color:%f\t%f\t%f\t%f\n",
		spec[0],spec[1],spec[2],spec[3]);
	DebugFileMessage(filename,"Reflectivity: %f\n",reflectivity);
	DebugFileMessage(filename,"Texture velocities:\n");
	for(i=0;i<NUM_TEXTURES;i++) {
		DebugFileMessage(filename,"Texture %d:vs:%f\tvt:%f\n",i,tex_velocities[3*i],
			tex_velocities[3*i+1]);
	}
	DebugFileMessage(filename,"Texture pointers:\n");
	for(i=0;i<NUM_TEXTURES;i++) {
		DebugFileMessage(filename,"Texture %d pointer is 0x%x\n",i,textures[i]);
	}
	DebugFileMessage(filename,"Vertex modifier stack pointer is 0x%x\n",mod_stack);
}

void Surface::dump_to_framelog() {
	DebugFrameMessage("************************************************************\n");
	DebugFrameMessage("\t\t\tSurface content dump\n");
	DebugFrameMessage("************************************************************\n");
	DebugFrameMessage("Geometry data:\n");
	DebugFrameMessage("Mesh has %d vertices forming %d triangles\n",vb->num_verts,vb->num_tris);
	DebugFrameMessage("Vertex pointer 0x%x\n",vb->vert_coords);
	DebugFrameMessage("Normal pointer 0x%x\n",vb->norm_coords);
	DebugFrameMessage("Texture coordinate pointer 0x%x\n",vb->tex_coords);
	int i;
	DebugFrameMessage("Checking coordinate sets ...\n");
	for(i=0;i<NUM_TEXTURES;i++) {
		DebugFrameMessage("Coordinate set %d pointer is 0x%x\n",i,vb->tex_coords[i]);
	}
	DebugFrameMessage("Vertex indices pointer 0x%x\n",vb->indices);
	DebugFrameMessage("Surface attributes:\n");
	DebugFrameMessage("Render flags %d\n",render_flags);
	DebugFrameMessage("Base color (used only when lighting is disabled):\n%f\t%f\t%f\t%f\n",
		col[0],col[1],col[2],col[3]);
	DebugFrameMessage("Ambient color (not used):%f\t%f\t%f\t%f\n",
		amb[0],amb[1],amb[2],amb[3]);
	DebugFrameMessage("Diffuse color:%f\t%f\t%f\t%f\n",
		dif[0],dif[1],dif[2],dif[3]);
	DebugFrameMessage("Specular color:%f\t%f\t%f\t%f\n",
		spec[0],spec[1],spec[2],spec[3]);
	DebugFrameMessage("Reflectivity: %f\n",reflectivity);
	DebugFrameMessage("Texture velocities:\n");
	for(i=0;i<NUM_TEXTURES;i++) {
		DebugFrameMessage("Texture %d:vs:%f\tvt:%f\n",i,tex_velocities[3*i],
			tex_velocities[3*i+1]);
	}
	DebugFrameMessage("Texture pointers:\n");
	for(i=0;i<NUM_TEXTURES;i++) {
		DebugFrameMessage("Texture %d pointer is 0x%x\n",i,textures[i]);
	}
	DebugFrameMessage("Vertex modifier stack pointer is 0x%x\n",mod_stack);
}


void Surface::copy_attributes(Surface* dest,const Surface* src) {
		dest->alpha_ref=src->alpha_ref;
		dest->amb[0]=src->amb[0];
		dest->amb[1]=src->amb[1];
		dest->amb[2]=src->amb[2];
		dest->amb[3]=src->amb[3];
		dest->col[0]=src->col[0];
		dest->col[1]=src->col[1];
		dest->col[2]=src->col[2];
		dest->col[3]=src->col[3];
		dest->dif[0]=src->dif[0];
		dest->dif[1]=src->dif[1];
		dest->dif[2]=src->dif[2];
		dest->dif[3]=src->dif[3];
		dest->emission[0]=src->emission[0];
		dest->emission[1]=src->emission[1];
		dest->emission[2]=src->emission[2];
		dest->emission[3]=src->emission[3];
		dest->is_particle_system=src->is_particle_system;
		dest->parent=src->parent;
		dest->particle_size=particle_size;
		dest->reflectivity=src->reflectivity;
		dest->render_flags=src->render_flags;
		dest->spec[0]=src->spec[0];
		dest->spec[1]=src->spec[1];
		dest->spec[2]=src->spec[2];
		dest->spec[3]=src->spec[3];
}

#define MAX_VB_SIZE 2000

Surface** Surface::split(int &out_num) {
	Surface** return_table=NULL;
	unsigned int i,tri_index;
	if(is_particle_system || vb->num_verts<(MAX_VB_SIZE)) {
		return_table=new Surface*[1];
		return_table[0]=new Surface(parent);

		//clone surface
		copy_attributes(return_table[0],this);

/*		return_table[0]->alpha_ref=alpha_ref;
		return_table[0]->amb[0]=amb[0];
		return_table[0]->amb[1]=amb[1];
		return_table[0]->amb[2]=amb[2];
		return_table[0]->amb[3]=amb[3];
		return_table[0]->call_list=call_list;
		return_table[0]->col[0]=col[0];
		return_table[0]->col[1]=col[1];
		return_table[0]->col[2]=col[2];
		return_table[0]->col[3]=col[3];
		return_table[0]->dif[0]=dif[0];
		return_table[0]->dif[1]=dif[1];
		return_table[0]->dif[2]=dif[2];
		return_table[0]->dif[3]=dif[3];
		return_table[0]->emission[0]=emission[0];
		return_table[0]->emission[1]=emission[1];
		return_table[0]->emission[2]=emission[2];
		return_table[0]->emission[3]=emission[3];
		return_table[0]->is_optimized=is_optimized;
		return_table[0]->is_particle_system=is_particle_system;
*/
		return_table[0]->mod_stack=NULL;
		return_table[0]->mod_vb=new VertexBuffer();

//		return_table[0]->parent=parent;

		if(particle_coords) {
			return_table[0]->particle_coords=new float[4*3*vb->num_verts];
		}
		else {
			return_table[0]->particle_coords=NULL;
		}
/*
		return_table[0]->particle_size=particle_size;
		return_table[0]->reflectivity=reflectivity;
		return_table[0]->render_flags=render_flags;
		return_table[0]->spec[0]=spec[0];
		return_table[0]->spec[1]=spec[1];
		return_table[0]->spec[2]=spec[2];
		return_table[0]->spec[3]=spec[3];
*/
		for(i=0;i<3*NUM_TEXTURES;i++) {
			return_table[0]->tex_velocities[i]=tex_velocities[i];
		}
		for(i=0;i<NUM_TEXTURES;i++) {
			return_table[0]->textures[i]=textures[i];
		}
		return_table[0]->vb=new VertexBuffer();
		if(vb->vert_coords) {
			return_table[0]->vb->vert_coords=new float[3*vb->num_verts];
			memcpy((void*)return_table[0]->vb->vert_coords,
				(const void*)vb->vert_coords,
				4*3*vb->num_verts);
		}
		if(vb->norm_coords) {
			return_table[0]->vb->norm_coords=new float[3*vb->num_verts];
			memcpy((void*)return_table[0]->vb->norm_coords,
				(const void*)vb->norm_coords,
				4*3*vb->num_verts);
		}
		if(vb->indices) {
			return_table[0]->vb->indices=new unsigned short[3*vb->num_tris];
			memcpy((void*)return_table[0]->vb->indices,(const void*)vb->indices,
				2*3*vb->num_tris);
		}
		for(i=0;i<NUM_TEXTURES;i++) {
			if(vb->tex_coords[i]) {
				return_table[0]->vb->tex_coords[i]=new float[2*vb->num_verts];
				memcpy((void*)return_table[0]->vb->tex_coords[i],
					(const void*)vb->tex_coords[i],
					4*2*vb->num_verts);
			}
		}
		return_table[0]->vb->is_external=0;
		return_table[0]->vb->max_ind=vb->max_ind;
		return_table[0]->vb->min_ind=vb->min_ind;
		return_table[0]->vb->num_tris=vb->num_tris;
		return_table[0]->vb->num_verts=vb->num_verts;
		out_num=1;
		return return_table;
	}

	VarTable<Surface*,1> out_tbl;
	Surface* current_surf;
	VertexBuffer* cur_vb;
	int* used=new int[vb->num_verts];
	unsigned short *index_map;
	index_map=new unsigned short[vb->num_verts];
	VarTable<unsigned short,300> tri_index_tbl;
	unsigned short reverse_index_map[MAX_VB_SIZE+3];



	current_surf=new Surface(parent);
	out_tbl.add_element(current_surf);
//	cur_vb=current_surf->vb;
	//clone current_surface parameters (but no VertexBuffer)

	copy_attributes(current_surf,this);
	current_surf->mod_stack=NULL;
	current_surf->mod_vb=new VertexBuffer();
	if(particle_coords) {
		current_surf->particle_coords=new float[4*3*vb->num_verts];
	}
	else {
		current_surf->particle_coords=NULL;
	}
	for(i=0;i<3*NUM_TEXTURES;i++) {
		current_surf->tex_velocities[i]=tex_velocities[i];
	}
	for(i=0;i<NUM_TEXTURES;i++) {
		current_surf->textures[i]=textures[i];
	}
	cur_vb=new VertexBuffer();
	current_surf->vb=cur_vb;
	cur_vb->is_external=0;

	for(i=0;i<vb->num_verts;i++) {
		used[i]=0;
		index_map[i]=0;
	}
	for(i=0;i<(MAX_VB_SIZE+3);i++) {
		reverse_index_map[i]=0;
	}
	current_surf->vb->num_verts=0;
	current_surf->vb->num_tris=0;

//	PrintfBox("starting triangle loop");
	//loop through all the tringles in VB
	int last_surf_processed=0;
	for(tri_index=0;tri_index<vb->num_tris;tri_index++) {
		for(i=0;i<3;i++) {    //for each vertex in triangle
			if(used[vb->indices[3*tri_index+i]]) {
				tri_index_tbl.add_element(index_map[vb->indices[3*tri_index+i]]);
			}
			else {
				index_map[vb->indices[3*tri_index+i]]=cur_vb->num_verts;
				reverse_index_map[current_surf->vb->num_verts]=vb->indices[3*tri_index+i];
				cur_vb->num_verts++;
				used[vb->indices[3*tri_index+i]]=1;
				tri_index_tbl.add_element(index_map[vb->indices[3*tri_index+i]]);
			}
		}
		current_surf->vb->num_tris++;
		//check size excess here
		if(cur_vb->num_verts>MAX_VB_SIZE) {   //start new surface
			//fill the vb data
			cur_vb->vert_coords=new float[3*cur_vb->num_verts];
			if(vb->norm_coords) {
				cur_vb->norm_coords=new float[3*cur_vb->num_verts];
			}
			for(i=0;i<NUM_TEXTURES;i++) {
				if(vb->tex_coords[i]) {
					cur_vb->tex_coords[i]=new float[2*cur_vb->num_verts];
				}
			}
			for(i=0;i<cur_vb->num_verts;i++) {
				cur_vb->vert_coords[3*i]=
					vb->vert_coords[3*reverse_index_map[i]];
				cur_vb->vert_coords[3*i+1]=
					vb->vert_coords[3*reverse_index_map[i]+1];
				cur_vb->vert_coords[3*i+2]=
					vb->vert_coords[3*reverse_index_map[i]+2];
				if(vb->norm_coords) {
					cur_vb->norm_coords[3*i]=
						vb->norm_coords[3*reverse_index_map[i]];
					cur_vb->norm_coords[3*i+1]=
						vb->norm_coords[3*reverse_index_map[i]+1];
					cur_vb->norm_coords[3*i+2]=
						vb->norm_coords[3*reverse_index_map[i]+2];
				}
				int j;
				for(j=0;j<NUM_TEXTURES;j++) {
					if(vb->tex_coords[j]) {
						cur_vb->tex_coords[j][2*i]=
							vb->tex_coords[j][2*reverse_index_map[i]];
						cur_vb->tex_coords[j][2*i+1]=
							vb->tex_coords[j][2*reverse_index_map[i]+1];
					}
				}
			}
			cur_vb->min_ind=0;
			cur_vb->max_ind=cur_vb->num_verts-1;
			cur_vb->indices=new unsigned short[3*cur_vb->num_tris];
			for(i=0;i<3*cur_vb->num_tris;i++) {
				cur_vb->indices[i]=tri_index_tbl[i];
			}
			tri_index_tbl.clear();


			//create new Surface and continue
			if(tri_index!=(vb->num_tris-1)) {  //prevent unnecessary memory leak
				current_surf=new Surface(parent);
				out_tbl.add_element(current_surf);
				//clone current_surface parameters (but no VertexBuffer)
				copy_attributes(current_surf,this);
				current_surf->mod_stack=NULL;
				current_surf->mod_vb=new VertexBuffer();
				if(particle_coords) {
					current_surf->particle_coords=new float[4*3*vb->num_verts];
				}
				else {
					current_surf->particle_coords=NULL;
				}
				for(i=0;i<3*NUM_TEXTURES;i++) {
					current_surf->tex_velocities[i]=tex_velocities[i];
				}
				for(i=0;i<NUM_TEXTURES;i++) {
					current_surf->textures[i]=textures[i];
				}
				cur_vb=new VertexBuffer();
				current_surf->vb=cur_vb;
				cur_vb->is_external=0;

				for(i=0;i<vb->num_verts;i++) {
					used[i]=0;
					index_map[i]=0;
				}
				for(i=0;i<(MAX_VB_SIZE+3);i++) {
					reverse_index_map[i]=0;
				}
				cur_vb->num_verts=0;
				cur_vb->num_tris=0;
			}
			else {
				last_surf_processed=1;
			}
		}

	}
	//end the last surface
	if(!last_surf_processed) {
		cur_vb->vert_coords=new float[3*cur_vb->num_verts];
		if(vb->norm_coords) {
			cur_vb->norm_coords=new float[3*cur_vb->num_verts];
		}
		for(i=0;i<NUM_TEXTURES;i++) {
			if(vb->tex_coords[i]) {
				cur_vb->tex_coords[i]=new float[2*cur_vb->num_verts];
			}
		}
		for(i=0;i<cur_vb->num_verts;i++) {
			cur_vb->vert_coords[3*i]=
				vb->vert_coords[3*reverse_index_map[i]];
			cur_vb->vert_coords[3*i+1]=
				vb->vert_coords[3*reverse_index_map[i]+1];
			cur_vb->vert_coords[3*i+2]=
				vb->vert_coords[3*reverse_index_map[i]+2];
			if(vb->norm_coords) {
				cur_vb->norm_coords[3*i]=
					vb->norm_coords[3*reverse_index_map[i]];
				cur_vb->norm_coords[3*i+1]=
					vb->norm_coords[3*reverse_index_map[i]+1];
				cur_vb->norm_coords[3*i+2]=
					vb->norm_coords[3*reverse_index_map[i]+2];
			}
			int j;
			for(j=0;j<NUM_TEXTURES;j++) {
				if(vb->tex_coords[j]) {
					cur_vb->tex_coords[j][2*i]=
						vb->tex_coords[j][2*reverse_index_map[i]];
					cur_vb->tex_coords[j][2*i+1]=
						vb->tex_coords[j][2*reverse_index_map[i]+1];
				}
			}
		}
		cur_vb->min_ind=0;
		cur_vb->max_ind=cur_vb->num_verts-1;
		cur_vb->indices=new unsigned short[3*cur_vb->num_tris];
		for(i=0;i<3*cur_vb->num_tris;i++) {
			cur_vb->indices[i]=tri_index_tbl[i];
		}
		tri_index_tbl.clear();
	}

	return_table=new Surface*[out_tbl.length];
	for(i=0;i<(unsigned int)(out_tbl.length);i++) {
		return_table[i]=out_tbl[i];
	}
	out_num=out_tbl.length;
	delete index_map;
	delete used;
	return return_table;
}

void Surface::optimize() {
	if(mod_stack) return;
	if(is_particle_system) return;
	if(!vb->is_external) {
		if(vb) {
			vb->optimize();
		}
	}
	else {
#ifdef _LOG_EVERYTHING
		DebugFileMessage("warnings.log","Optimize called for already optimized object!\n");
#endif
	}
}
//koniec implementacji Surface

/**********
 Implementacja klasy Object3D:
 Kontener dla kilku obiektow klasy Surface -
 jest pelnosprawnym obiektem dla LightWave 3D poniewaz gromadzi
 wiele powierzchni o roznych atrybutach
***********/

Object3D::Object3D() {
	meshes=NULL;
	num_meshes=0;
	name=NULL;
}

Object3D::Object3D(const char* filename,int type) {
	meshes=NULL;
	name=NULL;
	num_meshes=0;
	switch(type) {
	case LWO_OBJECT:
		load_LWO_file(filename);
	break;
	default:

#ifdef _LOG_EVERYTHING
		DebugFileMessage("warnings.log","Object3D::Object3D(filename,type)");
		DebugFileMessage("warnings.log"," - unknown file type\n");
#endif

	break;
	}
}

Object3D::~Object3D() {
	unsigned int i;
	if(name) delete name;
	for(i=0;i<num_meshes;i++) {
		if(meshes[i]!=NULL) delete meshes[i];
	}
}


void Object3D::split_sufaces() {
	if(!num_meshes) return;
	unsigned int new_surf_num,i,j;
	Surface** tmp;
	VarTable<Surface*,1> new_surf_table;
	for(i=0;i<num_meshes;i++) {
//		PrintfBox("Calling split()");
		int tmp2=0;
		tmp=meshes[i]->split(tmp2);
		new_surf_num=(unsigned int)tmp2;
//		PrintfBox("new surf num %d",new_surf_num);
		for(j=0;j<new_surf_num;j++) {
			new_surf_table.add_element(tmp[j]);
		}
		delete tmp;
	}
	for(i=0;i<num_meshes;i++) {
		delete meshes[i];
	}
	num_meshes=new_surf_table.length;
	delete meshes;
	meshes=new Surface*[num_meshes];
	for(i=0;i<num_meshes;i++) {
		meshes[i]=new_surf_table[i];
	}
}

void Object3D::render(float frame) {
	unsigned int i;
	for(i=0;i<num_meshes;i++) meshes[i]->render(frame);
}

void Object3D::render_solid_surfaces(float frame) {
	unsigned int i;
	for(i=0;i<num_meshes;i++) {
		if(!((meshes[i]->render_flags & RM_ADDITIVE_BIT) ||
			(meshes[i]->render_flags & RM_TRANSPARENT_BIT))) 
			meshes[i]->render(frame);
	}
}

void Object3D::render_additive_surfaces(float frame) {
	unsigned int i;
	for(i=0;i<num_meshes;i++) {
		if(meshes[i]->render_flags & RM_ADDITIVE_BIT) {
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			meshes[i]->render(frame);
		}
	}
}

void Object3D::render_transparent_surfaces(float frame) {
	unsigned int i;
	for(i=0;i<num_meshes;i++) {
		if(meshes[i]->render_flags & RM_TRANSPARENT_BIT) {
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			meshes[i]->render(frame);
		}
	}
}


void Object3D::optimize() {
	unsigned int i;
	if(num_meshes==0) return;
//	PrintfBox("calling split_surfaces()");
//	split_sufaces();
//	PrintfBox("returned from split_surfaces()");
	for(i=0;i<num_meshes;i++) meshes[i]->optimize();
}

void Object3D::align_to_box(float size) {
	if(num_meshes==0) return;

	float min_x,min_y,min_z;
	float max_x,max_y,max_z;
	float dx,dy,dz,dmax;
	float t_x,t_y,t_z;
	float scale_factor;
	unsigned int i,j;

	min_x=max_x=meshes[0]->vb->vert_coords[0];
	min_y=max_y=meshes[0]->vb->vert_coords[1];
	min_z=max_z=meshes[0]->vb->vert_coords[2];

	for(j=0;j<num_meshes;j++) {
		if(meshes[j]->vb->vert_coords==NULL) return;
		for(i=0;i<meshes[j]->vb->num_verts;i++) {
			if(min_x>meshes[j]->vb->vert_coords[3*i]) min_x=meshes[j]->vb->vert_coords[3*i];
			if(max_x<meshes[j]->vb->vert_coords[3*i]) max_x=meshes[j]->vb->vert_coords[3*i];
			
			if(min_y>meshes[j]->vb->vert_coords[3*i+1]) min_y=meshes[j]->vb->vert_coords[3*i+1];
			if(max_y<meshes[j]->vb->vert_coords[3*i+1]) max_y=meshes[j]->vb->vert_coords[3*i+1];
			
			if(min_z>meshes[j]->vb->vert_coords[3*i+2]) min_z=meshes[j]->vb->vert_coords[3*i+2];
			if(max_z<meshes[j]->vb->vert_coords[3*i+2]) max_z=meshes[j]->vb->vert_coords[3*i+2];
		}
	}
	dx=max_x-min_x;
	dy=max_y-min_y;
	dz=max_z-min_z;
	dmax=dx;
	if(dy>dmax) dmax=dy;
	if(dz>dmax) dmax=dz;
	scale_factor=size/dmax;
	t_x=-0.5*(max_x+min_x);
	t_y=-0.5*(max_y+min_y);
	t_z=-0.5*(max_z+min_z);
	for(j=0;j<num_meshes;j++) {
		for(i=0;i<meshes[j]->vb->num_verts;i++) {
			meshes[j]->vb->vert_coords[3*i]+=t_x;
			meshes[j]->vb->vert_coords[3*i]*=scale_factor;
			meshes[j]->vb->vert_coords[3*i+1]+=t_y;
			meshes[j]->vb->vert_coords[3*i+1]*=scale_factor;
			meshes[j]->vb->vert_coords[3*i+2]+=t_z;
			meshes[j]->vb->vert_coords[3*i+2]*=scale_factor;
		}
	}
}

unsigned int Object3D::get_render_flags() {
	if(num_meshes>0) return meshes[0]->render_flags;
	else return 0;
}

int Object3D::has_uniform_flags() {
	if(num_meshes>0) {
		if(num_meshes==1) return 1;
		unsigned int flags=meshes[0]->render_flags;
		unsigned int i;
		for(i=1;i<num_meshes;i++) if(meshes[i]->render_flags!=flags) return 0;
		return 1;
	}
	return 0;
}

int Object3D::has_transparent_surfaces() {
	if(num_meshes>0) {
		unsigned int i;
		for(i=0;i<num_meshes;i++) if((meshes[i]->render_flags & RM_TRANSPARENT_BIT)
			/*|| (meshes[i]->render_flags & RM_ALPHA_TEST_BIT)*/) return 1;
	}
	return 0;
}

int Object3D::has_additive_surfaces() {
	if(num_meshes>0) {
		unsigned int i;
		for(i=0;i<num_meshes;i++) 
			if(meshes[i]->render_flags & RM_ADDITIVE_BIT) return 1;
	}
	return 0;
}

void Object3D::dump(const char* filename) {
	unsigned int i;
	if(num_meshes>0) {
		for(i=0;i<num_meshes;i++) {
			if(meshes[i]) meshes[i]->dump(filename);
		}
	}
}

/***
loader LWO wraz z funkcjami pomocniczymi 
*****/

//big endian->low endian
static unsigned int read_4_bytes(int f,void* out) {
	char buf[4];
	char* o;
//	fread(buf,1,4,f);
	vfs->fread(f,4,buf);
	o=(char*)out;
	o[0]=buf[3];
	o[1]=buf[2];
	o[2]=buf[1];
	o[3]=buf[0];
	return 4;
}

static unsigned int read_2_bytes(int f,void* out) {
	char buf[2];
	char* o;
//	fread(buf,1,2,f);
	vfs->fread(f,2,buf);
	o=(char*)out;
	o[0]=buf[1];
	o[1]=buf[0];
	return 2;
}

static void process_2bval_block(void* block,int size) {
	char *pos=(char*)block;
	int i;
	char tmp;
	for(i=0;i<size;i++) {
		tmp=pos[0];
		pos[0]=pos[1];
		pos[1]=tmp;
		pos+=2;
	}
}

static void process_4bval_block(void* block,int size) {
	char *pos=(char*)block;
	int i;
	char tmp;
	for(i=0;i<size;i++) {
		tmp=pos[0];
		pos[0]=pos[3];
		pos[3]=tmp;
		tmp=pos[1];
		pos[1]=pos[2];
		pos[2]=tmp;
		pos+=4;
	}
}

//odczyt stringu z pliku z przydzialem pamieci
//i uwglednieniem specyfiki zapisu stringow przez LightWave
static unsigned int read_S(int f, char** str_buf) {
	if(!*str_buf) delete *str_buf;
	int size=100;
	*str_buf=new char[size];
	int pos=0;
	char c_buf;
	while(vfs->fread(f,1,&c_buf),c_buf!='\0') {
		if(pos<size) (*str_buf)[pos++]=c_buf;
		else {
			char* tmp_ps;
			int i;
			tmp_ps=new char[size+100];
			for(i=0;i<size;i++) tmp_ps[i]=(*str_buf)[i];
			size+=100;
			delete *str_buf;
			*str_buf=tmp_ps;
			(*str_buf)[pos++]=c_buf;
		}
	}
	if(pos<size) (*str_buf)[pos++]=c_buf;
	else {
		char* tmp_ps;
		int i;
		tmp_ps=new char[size+100];
		for(i=0;i<size;i++) tmp_ps[i]=(*str_buf)[i];
		size+=100;
		delete *str_buf;
		*str_buf=tmp_ps;
		(*str_buf)[pos++]=c_buf;
	}

	if(pos%2) {  // odd/even string
		pos++;
		vfs->fseek(f,1,SEEK_CUR);
	}
	return pos;
}

enum {
	CTEX,
	STEX,
	LTEX,
	DTEX,
	BTEX,
	RIMG,
	RTEX,
	TTEX
};

#define NUM_LWO_TEX_TYPES 8

//konstruktor/destruktor pomocniczej struktury tex_parameters
tex_parameters::tex_parameters() {
	type=NULL;
	timg=NULL;
	tctr[0]=0.0f;
	tctr[1]=0.0f;
	tctr[2]=0.0f;
	widthTiling=1.0f;
	heightTiling=1.0f;
	width_wrap=height_wrap=2;
}

tex_parameters::~tex_parameters() {
	if(type!=NULL) delete type;
	if(timg!=NULL) delete timg;
}


//wlasciwy loader LWO
int Object3D::process_POLS(int f,int chunk_length,float *vert_coords) {
	meshes=new Surface*[num_meshes];
	unsigned int i;
	for(i=0;i<num_meshes;i++) meshes[i]=new Surface(this);
	int chunk_pos=0;
	unsigned short int numvert;
	int vert_table[2000];
	unsigned int table_pos;
	short int tmp_index;
	short int first_index;
	short int recent_index;
	short int surf_id;
	int particle_system=0;
	int* ind_size;

	ind_size=new int[num_meshes];
	for(i=0;i<num_meshes;i++) ind_size[i]=0;
	
	unsigned short *tmp_pols_buf;
	tmp_pols_buf=new unsigned short[chunk_length/2];
	vfs->fread(f,chunk_length,tmp_pols_buf);
	process_2bval_block(tmp_pols_buf,chunk_length/2);
	
	while(chunk_pos<chunk_length) {
		numvert=tmp_pols_buf[chunk_pos/2];
		chunk_pos+=2;
		table_pos=0;
		if(numvert<3) {
			if(numvert==1) particle_system=1;
			chunk_pos+=2*numvert+2;
		}
		else {
			first_index=tmp_pols_buf[chunk_pos/2];
			chunk_pos+=2;
			recent_index=tmp_pols_buf[chunk_pos/2];
			chunk_pos+=2;
			for(i=0;i<(unsigned int)(numvert-2);i++) {
				tmp_index=tmp_pols_buf[chunk_pos/2];
				chunk_pos+=2;
				//maly reorder aby zgodzic sie z domyslnym cullingiem OpenGL
				vert_table[table_pos++]=first_index;
				vert_table[table_pos++]=tmp_index;
				vert_table[table_pos++]=recent_index;
				recent_index=tmp_index;
			}
			surf_id=tmp_pols_buf[chunk_pos/2];
			chunk_pos+=2;
			if(surf_id<0) {			//ignore detail polygons
#ifdef _LOG_EVERYTHING
//				DebugFileMessage("warnings.log","Detail polygon was found in file %s\n",filename);
#endif
				unsigned short int how_many;
				unsigned short int detail_verts;
				how_many=tmp_pols_buf[chunk_pos/2];
				chunk_pos+=2;
				for(i=0;i<how_many;i++) {
					detail_verts=tmp_pols_buf[chunk_pos/2];
					chunk_pos+=2;
					chunk_pos+=2*detail_verts+2;
				}
				surf_id=-surf_id;
			}
			surf_id--;
			
			int tmp_val1;
			if(ind_size[surf_id]==0) {
				if(meshes[surf_id]->vb->indices!=NULL) delete meshes[i]->vb->indices;
				meshes[surf_id]->vb->indices=new unsigned short[2048];
				for(i=0;i<table_pos;i++)
					meshes[surf_id]->vb->indices[i]=vert_table[i];
				meshes[surf_id]->vb->num_tris=numvert-2;
				ind_size[surf_id]=1;
			}
			else {
				if((table_pos+3*meshes[surf_id]->vb->num_tris)>(unsigned int)(2048*ind_size[surf_id])) {
					unsigned short *tmp;
					ind_size[surf_id]++;
					tmp=new unsigned short[2048*ind_size[surf_id]];
					tmp_val1=3*meshes[surf_id]->vb->num_tris;
					for(i=0;i<(unsigned int)tmp_val1;i++)
						tmp[i]=meshes[surf_id]->vb->indices[i];
					delete meshes[surf_id]->vb->indices;
					meshes[surf_id]->vb->indices=tmp;
				}
				tmp_val1=3*meshes[surf_id]->vb->num_tris;
				for(i=0;i<table_pos;i++) 
					meshes[surf_id]->vb->indices[i+tmp_val1]=vert_table[i];
				meshes[surf_id]->vb->num_tris+=numvert-2;
			}
			

/*

			if(meshes[surf_id]->indices==NULL) {
				meshes[surf_id]->indices=new unsigned int[table_pos];
				for(i=0;i<table_pos;i++)
					meshes[surf_id]->indices[i]=vert_table[i];
				meshes[surf_id]->num_tris=numvert-2;
			}
			else {
				unsigned int *tmp;
				tmp=new unsigned int[table_pos+3*meshes[surf_id]->num_tris];
				int tmp_val1=3*meshes[surf_id]->num_tris;
				for(i=0;i<tmp_val1;i++)
					tmp[i]=meshes[surf_id]->indices[i];
				for(i=0;i<table_pos;i++) 
					tmp[i+tmp_val1]=vert_table[i];
				delete meshes[surf_id]->indices;
				meshes[surf_id]->indices=tmp;
				meshes[surf_id]->num_tris+=numvert-2;
			}
*/			
		}
	}
	//min/max vertex index
	for(i=0;i<num_meshes;i++) {
		unsigned int min_i,max_i,j;
		unsigned int tmp_val1,tmp_val2;
		unsigned short* tmp_int_p;
		if(meshes[i]->vb->num_tris>0) {
			min_i=meshes[i]->vb->indices[0];
			max_i=meshes[i]->vb->indices[0];
			tmp_val1=3*meshes[i]->vb->num_tris;
			tmp_int_p=meshes[i]->vb->indices;
			for(j=1;j<tmp_val1;j++) {
				if(tmp_int_p[j]>max_i) max_i=tmp_int_p[j];
				if(tmp_int_p[j]<min_i) min_i=tmp_int_p[j];
			}
		}
		else {
			min_i=max_i=0;
		}
		meshes[i]->vb->min_ind=min_i;
		meshes[i]->vb->max_ind=max_i;
		meshes[i]->vb->num_verts=meshes[i]->vb->max_ind-meshes[i]->vb->min_ind+1;
		meshes[i]->vb->vert_coords=new float[3*meshes[i]->vb->num_verts];
		tmp_val1=3*meshes[i]->vb->min_ind;
		tmp_val2=3*(meshes[i]->vb->max_ind+1);
		for(j=tmp_val1;j<tmp_val2;j++) {
			meshes[i]->vb->vert_coords[j-tmp_val1]=vert_coords[j];
		}
		tmp_val1=3*meshes[i]->vb->num_tris;
		for(j=0;j<tmp_val1;j++) meshes[i]->vb->indices[j]-=meshes[i]->vb->min_ind;
		meshes[i]->vb->max_ind-=meshes[i]->vb->min_ind;
		meshes[i]->vb->min_ind=0;
	}
	delete tmp_pols_buf;
	delete ind_size;
	return particle_system;
}

void Object3D::load_LWO_file(const char* filename) {

#ifdef _LOG_EVERYTHING
		DebugMessage("filename %s\n",filename);
#endif
	name=new char[strlen(filename)+1];
	strcpy(name,filename);
	int f;
	unsigned int i;
	int file_id;
	unsigned int file_pos;
	unsigned int file_length;
	int chunk_id;
	unsigned int chunk_pos;
	unsigned int chunk_length;
	int subchunk_id;
	unsigned short int subchunk_length;
	int LWO_version=0;
	char** surf_names;
	int particle_system=0;
	float particle_size=0.0f;

	//temporaries
	unsigned int num_verts;
	float* vert_coords;

	//texture stuff
	TexLoadDesc tdesc;
	tex_parameters** tex_params[NUM_LWO_TEX_TYPES];
	unsigned int stack_sizes[NUM_LWO_TEX_TYPES];
	int current_stack;
	current_stack=0;
	for(i=0;i<NUM_LWO_TEX_TYPES;i++) {
		tex_params[i]=NULL;
		stack_sizes[i]=0;
	}

	vfs->openFile(filename,f);

	read_4_bytes(f,&file_id);
#ifdef _LOG_EVERYTHING
	if(file_id!='FORM') {
		DebugFileMessage("warning.log","Object3D::load_LWO_file - %s is not LWO\n",filename);
		vfs->closeFile(f);
		return;
	}
#else
	if(file_id!='FORM') {
		vfs->closeFile(f);
		return;
	}
#endif

	read_4_bytes(f,&file_length);
	file_pos=read_4_bytes(f,&LWO_version);
#ifdef _LOG_EVERYTHING
	if(LWO_version!='LWOB') {
		if(LWO_version=='LWO2') {
			DebugFileMessage("warnings.log","Object3D::load_LWO_file - LightWave 3D 6.0 file - skipping\n");
		}
		else {
			DebugFileMessage("warnings.log","Object3D::load_LWO_file - %s is not LWO\n",filename);
		}
		vfs->closeFile(f);
		return;
	}
#else
	if(LWO_version!='LWOB') {
		vfs->closeFile(f);
		return;
	}
#endif

	while(file_pos<file_length) {
		file_pos+=read_4_bytes(f,&chunk_id);
		file_pos+=read_4_bytes(f,&chunk_length);
		file_pos+=chunk_length;
		switch(chunk_id) {
		case 'PNTS':
			num_verts=chunk_length/12;
			vert_coords=new float[chunk_length/4];
			vfs->fread(f,chunk_length,vert_coords);
			process_4bval_block(vert_coords,chunk_length/4);
			for(i=0;i<(chunk_length/12);i++) {
				//To ponizej zdaje sie naprawieac problemy wynikajace z odmiennych
				//ukladow wspolrzednych LW i OpenGL
				vert_coords[3*i+2]=-vert_coords[3*i+2];
				if(i==0) {
					bbox.p_min[0]=vert_coords[0];
					bbox.p_min[1]=vert_coords[1];
					bbox.p_min[2]=vert_coords[2];
					bbox.p_max[0]=vert_coords[0];
					bbox.p_max[1]=vert_coords[1];
					bbox.p_max[2]=vert_coords[2];
				}
				else {
					if(vert_coords[3*i]<bbox.p_min[0]) bbox.p_min[0]=vert_coords[3*i];
					if(vert_coords[3*i+1]<bbox.p_min[1]) bbox.p_min[1]=vert_coords[3*i+1];
					if(vert_coords[3*i+2]<bbox.p_min[2]) bbox.p_min[2]=vert_coords[3*i+2];
					if(vert_coords[3*i]>bbox.p_max[0]) bbox.p_max[0]=vert_coords[3*i];
					if(vert_coords[3*i+1]>bbox.p_max[1]) bbox.p_max[1]=vert_coords[3*i+1];
					if(vert_coords[3*i+2]>bbox.p_max[2]) bbox.p_max[2]=vert_coords[3*i+2];
				}
			}
			bbox.p1[0]=bbox.p_min[0];
			bbox.p1[1]=bbox.p_min[1];
			bbox.p1[2]=bbox.p_min[2];

			bbox.p2[0]=bbox.p_min[0];
			bbox.p2[1]=bbox.p_min[1];
			bbox.p2[2]=bbox.p_max[2];

			bbox.p3[0]=bbox.p_min[0];
			bbox.p3[1]=bbox.p_max[1];
			bbox.p3[2]=bbox.p_min[2];

			bbox.p4[0]=bbox.p_min[0];
			bbox.p4[1]=bbox.p_max[1];
			bbox.p4[2]=bbox.p_max[2];

			bbox.p5[0]=bbox.p_max[0];
			bbox.p5[1]=bbox.p_min[1];
			bbox.p5[2]=bbox.p_min[2];

			bbox.p6[0]=bbox.p_max[0];
			bbox.p6[1]=bbox.p_min[1];
			bbox.p6[2]=bbox.p_max[2];

			bbox.p7[0]=bbox.p_max[0];
			bbox.p7[1]=bbox.p_max[1];
			bbox.p7[2]=bbox.p_min[2];

			bbox.p8[0]=bbox.p_max[0];
			bbox.p8[1]=bbox.p_max[1];
			bbox.p8[2]=bbox.p_max[2];
		break;
		case 'SRFS':
			num_meshes=0;
			chunk_pos=0;
			while(chunk_pos<chunk_length) {
				if(num_meshes==0) {
					surf_names=new char*[num_meshes+1];
					chunk_pos+=read_S(f,&surf_names[num_meshes++]);
				}
				else {
					char** tmp;
					tmp=new char*[num_meshes+1];
					for(i=0;i<num_meshes;i++) tmp[i]=surf_names[i];
					chunk_pos+=read_S(f,&tmp[num_meshes++]);
					delete surf_names;
					surf_names=tmp;
				}
			}
#ifdef _LOG_EVERYTHING
		DebugMessage("File %s surfaces:\n",filename);
		for(i=0;i<num_meshes;i++) DebugMessage("\t%s\n",surf_names[i]);
#endif
		break;
		case 'POLS':
			short int surf_id;
			particle_system=process_POLS(f,chunk_length,vert_coords);
/*			meshes=new Surface*[num_meshes];
			for(i=0;i<num_meshes;i++) meshes[i]=new Surface(this);
			chunk_pos=0;
			unsigned short int numvert;
			int vert_table[2000];
			unsigned int table_pos;
			short int tmp_index;
			short int first_index;
			short int recent_index;
			short int surf_id;

			unsigned short *tmp_pols_buf;
			tmp_pols_buf=new unsigned short[chunk_length/2];
			fread(tmp_pols_buf,1,chunk_length,f);
			process_2bval_block(tmp_pols_buf,chunk_length/2);

			while(chunk_pos<chunk_length) {
				numvert=tmp_pols_buf[chunk_pos/2];
				chunk_pos+=2;
				table_pos=0;
				if(numvert<3) {
					if(numvert==1) particle_system=1;
					chunk_pos+=2*numvert+2;
				}
				else {
					first_index=tmp_pols_buf[chunk_pos/2];
					chunk_pos+=2;
					recent_index=tmp_pols_buf[chunk_pos/2];
					chunk_pos+=2;
					for(i=0;i<(unsigned int)(numvert-2);i++) {
						tmp_index=tmp_pols_buf[chunk_pos/2];
						chunk_pos+=2;
						//maly reorder aby zgodzic sie z domyslnym cullingiem OpenGL
						vert_table[table_pos++]=first_index;
						vert_table[table_pos++]=tmp_index;
						vert_table[table_pos++]=recent_index;
						recent_index=tmp_index;
					}
					surf_id=tmp_pols_buf[chunk_pos/2];
					chunk_pos+=2;
					if(surf_id<0) {			//ignore detail polygons
#ifdef _LOG_EVERYTHING
						DebugFileMessage("warnings.log","Detail polygon was found in file %s\n",filename);
#endif
						unsigned short int how_many;
						unsigned short int detail_verts;
						how_many=tmp_pols_buf[chunk_pos/2];
						chunk_pos+=2;
						for(i=0;i<how_many;i++) {
							detail_verts=tmp_pols_buf[chunk_pos/2];
							chunk_pos+=2;
							chunk_pos+=2*detail_verts+2;
						}
						surf_id=-surf_id;
					}
					surf_id--;
					if(meshes[surf_id]->indices==NULL) {
						meshes[surf_id]->indices=new unsigned int[table_pos];
						for(i=0;i<table_pos;i++)
						meshes[surf_id]->indices[i]=vert_table[i];
						meshes[surf_id]->num_tris=numvert-2;
					}
					else {
						unsigned int *tmp;
						tmp=new unsigned int[table_pos+3*meshes[surf_id]->num_tris];
						int tmp_val1=3*meshes[surf_id]->num_tris;
						for(i=0;i<tmp_val1;i++)
							tmp[i]=meshes[surf_id]->indices[i];
						for(i=0;i<table_pos;i++) 
							tmp[i+tmp_val1]=vert_table[i];
						delete meshes[surf_id]->indices;
						meshes[surf_id]->indices=tmp;
						meshes[surf_id]->num_tris+=numvert-2;
					}
				}
			}
			//min/max vertex index
			for(i=0;i<num_meshes;i++) {
				unsigned int min_i,max_i,j;
				int tmp_val1,tmp_val2;
				unsigned int* tmp_int_p;
				if(meshes[i]->num_tris>0) {
					min_i=meshes[i]->indices[0];
					max_i=meshes[i]->indices[0];
					tmp_val1=3*meshes[i]->num_tris;
					tmp_int_p=meshes[i]->indices;
					for(j=1;j<tmp_val1;j++) {
						if(tmp_int_p[j]>max_i) max_i=tmp_int_p[j];
						if(tmp_int_p[j]<min_i) min_i=tmp_int_p[j];
					}
				}
				else {
					min_i=max_i=0;
				}
				meshes[i]->min_ind=min_i;
				meshes[i]->max_ind=max_i;
				meshes[i]->num_verts=meshes[i]->max_ind-meshes[i]->min_ind+1;
				meshes[i]->vert_coords=new float[3*meshes[i]->num_verts];
				tmp_val1=3*meshes[i]->min_ind;
				tmp_val2=3*(meshes[i]->max_ind+1);
				for(j=tmp_val1;j<tmp_val2;j++) {
					meshes[i]->vert_coords[j-tmp_val1]=vert_coords[j];
				}
				tmp_val1=3*meshes[i]->num_tris;
				for(j=0;j<tmp_val1;j++) meshes[i]->indices[j]-=meshes[i]->min_ind;
				meshes[i]->max_ind-=meshes[i]->min_ind;
				meshes[i]->min_ind=0;
			}
			delete tmp_pols_buf;
*/
//			delete vert_coords;		//we may need that for particle systems so 
									//we delete it later in the code
		break;
		case 'SURF':
			char* tmp_name;
			float transparency;
			float luminosity;
			transparency=1.0f;
			luminosity=0.0f;
			chunk_pos=read_S(f,&tmp_name);
			surf_id=0;
			for(i=0;i<num_meshes;i++) if(strcmp(tmp_name,surf_names[i])==0) surf_id=i;
#ifdef _LOG_EVERYTHING
			DebugMessage("Entered SURF reader:\n");
			DebugMessage("\tsurf_id=%d\n",surf_id);
			DebugMessage("\tsurf_name=\"%s\"\n",tmp_name);
#endif
			while(chunk_pos<chunk_length) {
				chunk_pos+=read_4_bytes(f,&subchunk_id);
				chunk_pos+=read_2_bytes(f,&subchunk_length);
				chunk_pos+=subchunk_length;

				switch(subchunk_id) {
				case 'COLR':
					unsigned char tmp_col[4];
					vfs->fread(f,4,tmp_col);
					meshes[surf_id]->col[0]=tmp_col[0]/255.0f;
					meshes[surf_id]->col[1]=tmp_col[1]/255.0f;
					meshes[surf_id]->col[2]=tmp_col[2]/255.0f;
				break;
				case 'VDIF':
					float dif;
					read_4_bytes(f,&dif);
					meshes[surf_id]->dif[0]=meshes[surf_id]->col[0]*dif;
					meshes[surf_id]->dif[1]=meshes[surf_id]->col[1]*dif;
					meshes[surf_id]->dif[2]=meshes[surf_id]->col[2]*dif;
				break;
				case 'VSPC':
					float spec;
					read_4_bytes(f,&spec);
					meshes[surf_id]->spec[0]=meshes[surf_id]->col[0]*spec;
					meshes[surf_id]->spec[1]=meshes[surf_id]->col[1]*spec;
					meshes[surf_id]->spec[2]=meshes[surf_id]->col[2]*spec;
				break;
				case 'VTRN':
					read_4_bytes(f,&transparency);
					transparency=1.0f-transparency;
				break;
				case 'VRFL':
					read_4_bytes(f,&meshes[surf_id]->reflectivity);
				break;
				case 'VLUM':
					read_4_bytes(f,&luminosity);
					meshes[surf_id]->emission[0]=meshes[surf_id]->col[0]*luminosity;
					meshes[surf_id]->emission[1]=meshes[surf_id]->col[1]*luminosity;
					meshes[surf_id]->emission[2]=meshes[surf_id]->col[2]*luminosity;
				break;
				case 'FLAG':
					unsigned short surf_flags;
					read_2_bytes(f,&surf_flags);
					if(surf_flags & 0x100) meshes[surf_id]->render_flags|=RM_DOUBLE_SIDED_BIT;
					if(surf_flags & 0x200) meshes[surf_id]->render_flags|=RM_ADDITIVE_BIT;
				break;
				case 'CTEX':
					current_stack=CTEX;
					if(stack_sizes[current_stack]==0) {
						tex_params[current_stack]=new tex_parameters*[1];
						tex_params[current_stack][stack_sizes[current_stack]]=new tex_parameters;
						stack_sizes[current_stack]++;
						read_S(f,&tex_params[current_stack][stack_sizes[current_stack]-1]->type);
					}
					else {
						tex_parameters** tmp;
						tmp=new tex_parameters*[stack_sizes[current_stack]+1];
						for(i=0;i<stack_sizes[current_stack];i++) 
							tmp[i]=tex_params[current_stack][i];
						stack_sizes[current_stack]++;
						tmp[stack_sizes[current_stack]-1]=new tex_parameters();
						read_S(f,&tmp[stack_sizes[current_stack]-1]->type);
						delete tex_params[current_stack];
						tex_params[current_stack]=tmp;
					}
#ifdef _LOG_EVERYTHING
					DebugMessage("\tCTEX - %s\n",
						tex_params[current_stack][stack_sizes[current_stack]-1]->type);
#endif
				break;
				case 'DTEX':
					current_stack=DTEX;
					if(stack_sizes[current_stack]==0) {
						tex_params[current_stack]=new tex_parameters*[1];
						tex_params[current_stack][stack_sizes[current_stack]]=new tex_parameters;
						stack_sizes[current_stack]++;
						read_S(f,&tex_params[current_stack][stack_sizes[current_stack]-1]->type);
					}
					else {
						tex_parameters** tmp;
						tmp=new tex_parameters*[stack_sizes[current_stack]+1];
						for(i=0;i<stack_sizes[current_stack];i++) 
							tmp[i]=tex_params[current_stack][i];
						stack_sizes[current_stack]++;
						tmp[stack_sizes[current_stack]-1]=new tex_parameters();
						read_S(f,&tmp[stack_sizes[current_stack]-1]->type);
						delete tex_params[current_stack];
						tex_params[current_stack]=tmp;
					}
#ifdef _LOG_EVERYTHING
					DebugMessage("\tDTEX - %s\n",
						tex_params[current_stack][stack_sizes[current_stack]-1]->type);
#endif
				break;
				case 'STEX':
					current_stack=STEX;
					if(stack_sizes[current_stack]==0) {
						tex_params[current_stack]=new tex_parameters*[1];
						tex_params[current_stack][stack_sizes[current_stack]]=new tex_parameters;
						stack_sizes[current_stack]++;
						read_S(f,&tex_params[current_stack][stack_sizes[current_stack]-1]->type);
					}
					else {
						tex_parameters** tmp;
						tmp=new tex_parameters*[stack_sizes[current_stack]+1];
						for(i=0;i<stack_sizes[current_stack];i++) 
							tmp[i]=tex_params[current_stack][i];
						stack_sizes[current_stack]++;
						tmp[stack_sizes[current_stack]-1]=new tex_parameters();
						read_S(f,&tmp[stack_sizes[current_stack]-1]->type);
						delete tex_params[current_stack];
						tex_params[current_stack]=tmp;
					}
#ifdef _LOG_EVERYTHING
					DebugMessage("\tSTEX - %s\n",
						tex_params[current_stack][stack_sizes[current_stack]-1]->type);
#endif
				break;
				case 'RTEX':
					current_stack=RTEX;
					if(stack_sizes[current_stack]==0) {
						tex_params[current_stack]=new tex_parameters*[1];
						tex_params[current_stack][stack_sizes[current_stack]]=new tex_parameters;
						stack_sizes[current_stack]++;
						read_S(f,&tex_params[current_stack][stack_sizes[current_stack]-1]->type);
					}
					else {
						tex_parameters** tmp;
						tmp=new tex_parameters*[stack_sizes[current_stack]+1];
						for(i=0;i<stack_sizes[current_stack];i++) 
							tmp[i]=tex_params[current_stack][i];
						stack_sizes[current_stack]++;
						tmp[stack_sizes[current_stack]-1]=new tex_parameters();
						read_S(f,&tmp[stack_sizes[current_stack]-1]->type);
						delete tex_params[current_stack];
						tex_params[current_stack]=tmp;
					}
#ifdef _LOG_EVERYTHING
					DebugMessage("\tRTEX - %s\n",
						tex_params[current_stack][stack_sizes[current_stack]-1]->type);
#endif
				break;
				case 'RIMG':
					current_stack=RIMG;
					if(stack_sizes[current_stack]==0) {
						tex_params[current_stack]=new tex_parameters*[1];
						tex_params[current_stack][stack_sizes[current_stack]]=new tex_parameters;
						stack_sizes[current_stack]++;
						read_S(f,&tex_params[current_stack][stack_sizes[current_stack]-1]->timg);
					}
					else {
						tex_parameters** tmp;
						tmp=new tex_parameters*[stack_sizes[current_stack]+1];
						for(i=0;i<stack_sizes[current_stack];i++) 
							tmp[i]=tex_params[current_stack][i];
						stack_sizes[current_stack]++;
						tmp[stack_sizes[current_stack]-1]=new tex_parameters();
						read_S(f,&tmp[stack_sizes[current_stack]-1]->timg);
						delete tex_params[current_stack];
						tex_params[current_stack]=tmp;
					}

#ifdef _LOG_EVERYTHING
					DebugMessage("\tRIMG - %s\n",
						tex_params[current_stack][stack_sizes[current_stack]-1]->type);
#endif
				break;
				case 'TTEX':
					current_stack=TTEX;
					if(stack_sizes[current_stack]==0) {
						tex_params[current_stack]=new tex_parameters*[1];
						tex_params[current_stack][stack_sizes[current_stack]]=new tex_parameters;
						stack_sizes[current_stack]++;
						read_S(f,&tex_params[current_stack][stack_sizes[current_stack]-1]->type);
					}
					else {
						tex_parameters** tmp;
						tmp=new tex_parameters*[stack_sizes[current_stack]+1];
						for(i=0;i<stack_sizes[current_stack];i++) 
							tmp[i]=tex_params[current_stack][i];
						stack_sizes[current_stack]++;
						tmp[stack_sizes[current_stack]-1]=new tex_parameters();
						read_S(f,&tmp[stack_sizes[current_stack]-1]->type);
						delete tex_params[current_stack];
						tex_params[current_stack]=tmp;
					}
#ifdef _LOG_EVERYTHING
					DebugMessage("\tTTEX - %s\n",
						tex_params[current_stack][stack_sizes[current_stack]-1]->type);
#endif
				break;
				case 'LTEX':
					current_stack=LTEX;
					if(stack_sizes[current_stack]==0) {
						tex_params[current_stack]=new tex_parameters*[1];
						tex_params[current_stack][stack_sizes[current_stack]]=new tex_parameters;
						stack_sizes[current_stack]++;
						read_S(f,&tex_params[current_stack][stack_sizes[current_stack]-1]->type);
					}
					else {
						tex_parameters** tmp;
						tmp=new tex_parameters*[stack_sizes[current_stack]+1];
						for(i=0;i<stack_sizes[current_stack];i++) 
							tmp[i]=tex_params[current_stack][i];
						stack_sizes[current_stack]++;
						tmp[stack_sizes[current_stack]-1]=new tex_parameters();
						read_S(f,&tmp[stack_sizes[current_stack]-1]->type);
						delete tex_params[current_stack];
						tex_params[current_stack]=tmp;
					}
#ifdef _LOG_EVERYTHING
					DebugMessage("\tLTEX - %s\n",
						tex_params[current_stack][stack_sizes[current_stack]-1]->type);
#endif
				break;
				case 'BTEX':
					current_stack=BTEX;
					if(stack_sizes[current_stack]==0) {
						tex_params[current_stack]=new tex_parameters*[1];
						tex_params[current_stack][stack_sizes[current_stack]]=new tex_parameters;
						stack_sizes[current_stack]++;
						read_S(f,&tex_params[current_stack][stack_sizes[current_stack]-1]->type);
					}
					else {
						tex_parameters** tmp;
						tmp=new tex_parameters*[stack_sizes[current_stack]+1];
						for(i=0;i<stack_sizes[current_stack];i++) 
							tmp[i]=tex_params[current_stack][i];
						stack_sizes[current_stack]++;
						tmp[stack_sizes[current_stack]-1]=new tex_parameters();
						read_S(f,&tmp[stack_sizes[current_stack]-1]->type);
						delete tex_params[current_stack];
						tex_params[current_stack]=tmp;
					}
#ifdef _LOG_EVERYTHING
					DebugMessage("\tBTEX - %s\n",
						tex_params[current_stack][stack_sizes[current_stack]-1]->type);
#endif
				break;
				case 'TFP0':
					float tfp0;
					read_4_bytes(f,&tfp0);
					if(tex_params[current_stack]!=NULL) {
						if(stack_sizes[current_stack]) {
							if((strcmp(tex_params[current_stack]
								[stack_sizes[current_stack]-1]->type,
								"Cylindrycal Image Map")==0) ||
								(strcmp(tex_params[current_stack]
								[stack_sizes[current_stack]-1]->type,
								"Spherical Image Map")==0)) {
								tex_params[current_stack]
									[stack_sizes[current_stack]-1]->widthTiling=tfp0;
							}
						}
					}
				break;
				case 'TFP1':
					float tfp1;
					read_4_bytes(f,&tfp1);
					if(tex_params[current_stack]!=NULL) {
						if(stack_sizes[current_stack]) {
							if((strcmp(tex_params[current_stack]
								[stack_sizes[current_stack]-1]->type,
								"Spherical Image Map")==0)) {
								tex_params[current_stack]
									[stack_sizes[current_stack]-1]->heightTiling=tfp1;
							}
						}
					}
				break;
				case 'TFLG':
					read_2_bytes(f,&tex_params[current_stack][stack_sizes[current_stack]-1]->flags);
				break;
				case 'TSIZ':
					read_4_bytes(f,&tex_params[current_stack]
						[stack_sizes[current_stack]-1]->tsiz[0]);
					read_4_bytes(f,&tex_params[current_stack]
						[stack_sizes[current_stack]-1]->tsiz[1]);
					read_4_bytes(f,&tex_params[current_stack]
						[stack_sizes[current_stack]-1]->tsiz[2]);
					if((current_stack==CTEX) && particle_system) {
						particle_size=
							tex_params[current_stack][stack_sizes[current_stack]-1]->tsiz[0];
					}
				break;
				case 'TCTR':
					read_4_bytes(f,&tex_params[current_stack]
						[stack_sizes[current_stack]-1]->tctr[0]);
					read_4_bytes(f,&tex_params[current_stack]
						[stack_sizes[current_stack]-1]->tctr[1]);
					read_4_bytes(f,&tex_params[current_stack]
						[stack_sizes[current_stack]-1]->tctr[2]);
				break;
				case 'TFAL':
					read_4_bytes(f,&tex_params[current_stack]
						[stack_sizes[current_stack]-1]->tfal[0]);
					read_4_bytes(f,&tex_params[current_stack]
						[stack_sizes[current_stack]-1]->tfal[1]);
					read_4_bytes(f,&tex_params[current_stack]
						[stack_sizes[current_stack]-1]->tfal[2]);
				break;
				case 'TVEL':
					read_4_bytes(f,&tex_params[current_stack]
						[stack_sizes[current_stack]-1]->tvel[0]);
					read_4_bytes(f,&tex_params[current_stack]
						[stack_sizes[current_stack]-1]->tvel[1]);
					read_4_bytes(f,&tex_params[current_stack]
						[stack_sizes[current_stack]-1]->tvel[2]);
					int tex_id;
					switch(current_stack) {
					case CTEX:
						tex_id=BASE;
					break;
					case LTEX:
						tex_id=LUMINOSITY;
					break;
					case STEX:
						tex_id=SPECULAR;
					break;
					case DTEX:
						tex_id=DIFFUSE;
					break;
					case BTEX:
						tex_id=BUMP;
					break;
					case RTEX:
						tex_id=GLOSS;
					break;
					default:
						tex_id=-1;
					break;
					}
					if(tex_id>=0) {
						meshes[surf_id]->tex_velocities[3*tex_id]=
							tex_params[current_stack][stack_sizes[current_stack]-1]->tvel[0];
						meshes[surf_id]->tex_velocities[3*tex_id+1]=
							tex_params[current_stack][stack_sizes[current_stack]-1]->tvel[1];
						meshes[surf_id]->tex_velocities[3*tex_id+2]=
							tex_params[current_stack][stack_sizes[current_stack]-1]->tvel[2];
					}
				break;
				case 'TWRP':
					read_2_bytes(f,&tex_params[current_stack]
						[stack_sizes[current_stack]-1]->width_wrap);
					read_2_bytes(f,&tex_params[current_stack]
						[stack_sizes[current_stack]-1]->height_wrap);
				break;
				case 'TIMG':
					char* tmp_s;
					read_S(f,&tmp_s);
#ifdef _LOG_EVERYTHING
					DebugMessage("Texture image %s\n",tmp_s);
#endif
					unsigned int i;
			//for timemachine style reader (everything in single dir) 
/*			int last;
			last=-1;
			for(i=0;i<strlen(tmp_s);i++) {
				if(tmp_s[i]=='\\') last=i;
			}
			if(last>=0) {
				for(i=last+1;i<strlen(tmp_s);i++) {
					tmp_s[i-last-1]=tmp_s[i];
				}
			}
			tmp_s[strlen(tmp_s)-last-1]='\0';*/
			//end specific

			//single dir with subdirs (bayger's lws3d)
			char* tmp_s2;
			int last;
			last=-1;
			for(i=0;i<strlen(tmp_s);i++) {
				if(tmp_s[i]=='\\') last=i;
			}
			if(last>=0) {
				for(i=last+1;i<strlen(tmp_s);i++) {
					tmp_s[i-last-1]=tmp_s[i];
				}
			}
			tmp_s[strlen(tmp_s)-last-1]='\0';
			tmp_s2=new char[strlen(tmp_s)+strlen("textures/")+1];
			tmp_s2[0]='\0';
			strcat(tmp_s2,"textures/");
			strcat(tmp_s2,tmp_s);
			tmp_s=tmp_s2;
			//end specific

					for(i=0;i<strlen(tmp_s);i++)
						if(tmp_s[i]==0x5c) tmp_s[i]=0x2f;  //'\' -> '/'
					tex_params[current_stack][stack_sizes[current_stack]-1]->timg=tmp_s;
				break;
				default:
					vfs->fseek(f,subchunk_length,SEEK_CUR);
				break;
				}
			}

			
			meshes[surf_id]->amb[3]=transparency;
			meshes[surf_id]->dif[3]=transparency;
			meshes[surf_id]->emission[3]=transparency;
			int gen_res;
			tdesc.alpha_mode=TAM_NONE;
			if(transparency!=1.0f) {
				meshes[surf_id]->render_flags|=RM_TRANSPARENT_BIT;
				tdesc.alpha_mode=TAM_NONE;
			}
			if(tex_params[TTEX]!=NULL) {
				if(tex_params[TTEX][stack_sizes[TTEX]-1]->timg) {
					tdesc.alpha_mode=TAM_INVERSE_SEPARATE_IMAGE;
					meshes[surf_id]->render_flags|=RM_TRANSPARENT_BIT;
				}
				tdesc.alpha_file=tex_params[TTEX][stack_sizes[TTEX]-1]->timg;
			}
			else if(meshes[surf_id]->render_flags & RM_TRANSPARENT_BIT) {
				//transparency in the material
				meshes[surf_id]->amb[3]=transparency;
				meshes[surf_id]->dif[3]=transparency;
				meshes[surf_id]->emission[3]=transparency;
			}
			if((tex_params[TTEX]!=NULL)
				&& (tex_params[TTEX][stack_sizes[TTEX]-1]->timg)
				&& (transparency!=1.0f)) {  //set up alpha testing
					meshes[surf_id]->render_flags^=RM_TRANSPARENT_BIT;
					meshes[surf_id]->render_flags|=RM_ALPHA_TEST_BIT;
					meshes[surf_id]->alpha_ref=transparency;
					meshes[surf_id]->amb[3]=1.0f;
					meshes[surf_id]->dif[3]=1.0f;
					meshes[surf_id]->emission[3]=1.0f;
			}
			if(tex_params[CTEX]!=NULL) {
				gen_res=meshes[surf_id]->generate_tex_coords(tex_params[CTEX][stack_sizes[CTEX]-1],BASE);
				tdesc.filename=tex_params[CTEX][stack_sizes[CTEX]-1]->timg;
				if(tex_params[CTEX][stack_sizes[CTEX]-1]->width_wrap!=2) {
					tdesc.wrap_s=GL_CLAMP;
				}
				if(tex_params[CTEX][stack_sizes[CTEX]-1]->height_wrap!=2) {
					tdesc.wrap_t=GL_CLAMP;
				}
				if(gen_res || particle_system) 
					meshes[surf_id]->add_texture(tex_manager.get_texture(tdesc),BASE);
				tdesc.wrap_s=GL_REPEAT;
				tdesc.wrap_t=GL_REPEAT;
			}
			if(tex_params[DTEX]!=NULL) { 
				if(tex_params[CTEX]!=NULL) tdesc.alpha_mode=TAM_NONE;
				gen_res=meshes[surf_id]->generate_tex_coords(tex_params[DTEX][stack_sizes[DTEX]-1],DIFFUSE);
				tdesc.filename=tex_params[DTEX][stack_sizes[DTEX]-1]->timg;
				if(tex_params[DTEX][stack_sizes[DTEX]-1]->width_wrap!=2) {
					tdesc.wrap_s=GL_CLAMP;
				}
				if(tex_params[DTEX][stack_sizes[DTEX]-1]->height_wrap!=2) {
					tdesc.wrap_t=GL_CLAMP;
				}
				if(gen_res) meshes[surf_id]->add_texture(tex_manager.get_texture(tdesc),DIFFUSE);
				tdesc.wrap_s=GL_REPEAT;
				tdesc.wrap_t=GL_REPEAT;
			}
			if(tex_params[RTEX]!=NULL) {
				gen_res=meshes[surf_id]->generate_tex_coords(tex_params[RTEX][stack_sizes[RTEX]-1],GLOSS);
				tdesc.filename=tex_params[RTEX][stack_sizes[RTEX]-1]->timg;
				if(tex_params[RTEX][stack_sizes[RTEX]-1]->width_wrap!=2) {
					tdesc.wrap_s=GL_CLAMP;
				}
				if(tex_params[RTEX][stack_sizes[RTEX]-1]->height_wrap!=2) {
					tdesc.wrap_t=GL_CLAMP;
				}
				if(gen_res) meshes[surf_id]->add_texture(tex_manager.get_texture(tdesc),GLOSS);
				tdesc.wrap_s=GL_REPEAT;
				tdesc.wrap_t=GL_REPEAT;
			}
			if(tex_params[RIMG]!=NULL) {
				if(tex_params[RIMG][stack_sizes[RIMG]-1]->timg) {
					tdesc.filename=tex_params[RIMG][stack_sizes[RIMG]-1]->timg;
					meshes[surf_id]->add_texture(tex_manager.get_texture(tdesc),REFLECTION);
					meshes[surf_id]->render_flags|=RM_REFLECTION_TEXTURE_BIT;
					tdesc.wrap_s=GL_REPEAT;
					tdesc.wrap_t=GL_REPEAT;
				}
			}
			if(tex_params[STEX]!=NULL) {
				if(tex_params[STEX][stack_sizes[STEX]-1]->timg) {
					tdesc.filename=tex_params[STEX][stack_sizes[STEX]-1]->timg;
					meshes[surf_id]->add_texture(tex_manager.get_texture(tdesc),SPECULAR);
					meshes[surf_id]->render_flags|=RM_SPECULAR_TEXTURE_BIT;
					tdesc.wrap_s=GL_REPEAT;
					tdesc.wrap_t=GL_REPEAT;
				}
			}
			if(tex_params[LTEX]!=NULL) {
				gen_res=meshes[surf_id]->generate_tex_coords(tex_params[LTEX][stack_sizes[LTEX]-1],LUMINOSITY);
				tdesc.filename=tex_params[LTEX][stack_sizes[LTEX]-1]->timg;
				if(tex_params[LTEX][stack_sizes[LTEX]-1]->width_wrap!=2) {
					tdesc.wrap_s=GL_CLAMP;
				}
				if(tex_params[LTEX][stack_sizes[LTEX]-1]->height_wrap!=2) {
					tdesc.wrap_t=GL_CLAMP;
				}
				if(gen_res) meshes[surf_id]->add_texture(tex_manager.get_texture(tdesc),LUMINOSITY);
				tdesc.wrap_s=GL_REPEAT;
				tdesc.wrap_t=GL_REPEAT;
				if(tex_params[CTEX]==NULL) {
					meshes[surf_id]->render_flags|=RM_ADDITIVE_BIT;
				}
			}
			if(tex_params[BTEX]!=NULL) { 
				tdesc.alpha_mode=TAM_NONE;
				gen_res=meshes[surf_id]->generate_tex_coords(tex_params[BTEX][stack_sizes[BTEX]-1],BUMP);
				tdesc.filename=tex_params[BTEX][stack_sizes[BTEX]-1]->timg;
				if(gen_res) meshes[surf_id]->add_texture(tex_manager.get_texture(tdesc),BUMP);
			}
			if((meshes[surf_id]->render_flags & RM_ADDITIVE_BIT) &&
				(meshes[surf_id]->render_flags & RM_TRANSPARENT_BIT))
				meshes[surf_id]->render_flags ^= RM_TRANSPARENT_BIT;
			for(i=0;i<NUM_LWO_TEX_TYPES;i++) {
				if(tex_params[i]!=NULL) {
					unsigned int j;
					for(j=0;j<stack_sizes[i];j++) {
						delete tex_params[i][j];
					}
					delete tex_params[i];
					tex_params[i]=NULL;
					stack_sizes[i]=0;
				}
			}
			current_stack=0;
		break;
		default:
			vfs->fseek(f,chunk_length,SEEK_CUR);
		break;
		}
	}

	if(particle_system) {
		//build the particle system:
		//-destroy all the surfaces except first
		//-free unnecesary mem and alloc other mem that is neccesary for particles
		for(i=1;i<num_meshes;i++) {
			delete meshes[i];
			meshes[i]=NULL;
		}
		num_meshes=1;
		meshes[0]->is_particle_system=1;
		delete meshes[0]->vb->vert_coords;
		delete meshes[0]->vb->norm_coords;
		meshes[0]->vb->norm_coords=NULL;
		meshes[0]->render_flags=RM_BASE_TEXTURE_BIT | RM_TRANSPARENT_BIT;
		meshes[0]->vb->vert_coords=new float[3*num_verts];
		for(i=0;i<3*num_verts;i++) {
			meshes[0]->vb->vert_coords[i]=vert_coords[i];
		}
		for(i=0;i<NUM_LWO_TEX_TYPES;i++) {
			if(meshes[0]->vb->tex_coords[i]!=NULL) {
				delete meshes[0]->vb->tex_coords[i];
				meshes[0]->vb->tex_coords[i]=NULL;
			}
		}
		meshes[0]->vb->tex_coords[BASE]=new float[2*4*num_verts];
		for(i=0;i<num_verts;i++) {
			meshes[0]->vb->tex_coords[BASE][2*4*i]=0.0f;
			meshes[0]->vb->tex_coords[BASE][2*4*i+1]=0.0f;
			
			meshes[0]->vb->tex_coords[BASE][2*4*i+2]=1.0f;
			meshes[0]->vb->tex_coords[BASE][2*4*i+3]=0.0f;
			
			meshes[0]->vb->tex_coords[BASE][2*4*i+4]=1.0f;
			meshes[0]->vb->tex_coords[BASE][2*4*i+5]=1.0f;
			
			meshes[0]->vb->tex_coords[BASE][2*4*i+6]=0.0f;
			meshes[0]->vb->tex_coords[BASE][2*4*i+7]=1.0f;
		}
		if(particle_size!=0.0f) {
			meshes[0]->particle_size=particle_size;
		}
		else {
			meshes[0]->render_flags^=RM_BASE_TEXTURE_BIT;
		}
		meshes[0]->particle_coords=new float[3*4*num_verts];
		delete meshes[0]->vb->indices;
		meshes[0]->vb->indices=NULL;
		meshes[0]->vb->num_verts=num_verts;
		meshes[0]->vb->num_tris=0;
	}

	delete vert_coords;    //finally

	//normal generator
	for(i=0;i<num_meshes;i++) meshes[i]->generate_normals();
//	fclose(f);
	vfs->closeFile(f );
}

//koniec loadera LWO

//koniec implementacji Object3D