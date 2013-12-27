/***************************************************************************
 Implementation of various parametric meshes

 (c) 2001 Maciej Mroz (Frost/Addict)
***************************************************************************/

#include "st_mesh.h"
#include "glstate.h"
#include <math.h>
#include "debug_msg.h"

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float

STMesh::STMesh(ST_MESH_RENDER_MODE rm,int subdivisions) {
	base_tex=NULL;
	render_mode=rm;
	size=subdivisions;
	vert_coords=new float[3*(size+1)*(size+1)];
	norm_coords=new float[3*(size+1)*(size+1)];
	binormal=new float[3*(size+1)*(size+1)];
	tangent=new float[3*(size+1)*(size+1)];
	if(render_mode!=ST_RM_SPHERE_MAP) {
		tex_coords=new float[2*(size+1)*(size+1)];
	}
	else {
		tex_coords=NULL;
	}
	bump_map=NULL;
	base_tex=NULL;
	are_tangents_valid=0;
	cube_normalizer=NULL;
	if(((render_mode==ST_RM_DOT3) ||
		(render_mode==ST_RM_DOT3_SPECULAR)
		&& (hasRegisterCombiners || hasATIFragmentShader)
		&& hasCubeMaps) ||
		((render_mode==ST_RM_DOT3_ARB)
		&& hasTextureEnvCombine
		&& hasTextureEnvDot3
		&& hasCubeMaps)) {
		TexLoadDesc tdesc;
		tdesc.type=TT_CUBE_MAP_NORMALIZER;
		tdesc.cube_map_size=64;
		cube_normalizer=tex_manager.get_texture(tdesc);
		diffuse_bump_coords=new float[3*(size+1)*(size+1)];
		bump_coords=new float[2*(size+1)*(size+1)];
		if(render_mode==ST_RM_DOT3_SPECULAR) {
			specular_bump_coords=new float[3*(size+1)*(size+1)];
		}
		else {
			specular_bump_coords=NULL;
		}
		if(hasATIFragmentShader) {
			fragment_shader=glGenFragmentShadersATI(1);
			glBindFragmentShaderATI(fragment_shader);
			glBeginFragmentShaderATI();
			if(render_mode==ST_RM_DOT3_SPECULAR) {
				//sample base map
				glSampleMapATI(GL_REG_0_ATI,GL_TEXTURE0_ARB,GL_SWIZZLE_STR_ATI);
				//sample normal map
				glSampleMapATI(GL_REG_1_ATI,GL_TEXTURE1_ARB,GL_SWIZZLE_STR_ATI);
				//sample tangent space light vector
				glSampleMapATI(GL_REG_2_ATI,GL_TEXTURE2_ARB,GL_SWIZZLE_STR_ATI);
				//sample tangent space halfangle vector
				glSampleMapATI(GL_REG_3_ATI,GL_TEXTURE3_ARB,GL_SWIZZLE_STR_ATI);
				//C4=dot(L,N)
				glColorFragmentOp2ATI(GL_DOT3_ATI,GL_REG_2_ATI, GL_NONE,GL_SATURATE_BIT_ATI,
					GL_REG_2_ATI, GL_NONE, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI,
					GL_REG_1_ATI, GL_NONE, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI);
				//dot(L,H)
				glColorFragmentOp2ATI(GL_DOT3_ATI,GL_REG_3_ATI, GL_NONE,  GL_SATURATE_BIT_ATI,
					GL_REG_3_ATI, GL_NONE, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI,
					GL_REG_1_ATI, GL_NONE, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI);
				//dot(L,H)^2
				glColorFragmentOp2ATI(GL_MUL_ATI,GL_REG_3_ATI, GL_NONE,GL_NONE,
					GL_REG_3_ATI, GL_NONE, GL_NONE,
					GL_REG_3_ATI, GL_NONE, GL_NONE);
				//dot(L,H)^4
				glColorFragmentOp2ATI(GL_MUL_ATI,GL_REG_3_ATI, GL_NONE,GL_NONE,
					GL_REG_3_ATI, GL_NONE, GL_NONE,
					GL_REG_3_ATI, GL_NONE, GL_NONE);
				//dot(L,H)^16
				glColorFragmentOp2ATI(GL_MUL_ATI,GL_REG_3_ATI, GL_NONE,GL_NONE,
					GL_REG_3_ATI, GL_NONE, GL_NONE,
					GL_REG_3_ATI, GL_NONE, GL_NONE);
				//out: base*diffuse+specular
				glColorFragmentOp3ATI(GL_MAD_ATI,GL_REG_0_ATI, GL_NONE,GL_SATURATE_BIT_ATI,
					GL_REG_0_ATI, GL_NONE,GL_NONE,
					GL_REG_2_ATI, GL_NONE,GL_NONE,
					GL_REG_3_ATI, GL_NONE,GL_NONE);
			}
			else {
				//sample base map
				glSampleMapATI(GL_REG_0_ATI,GL_TEXTURE0_ARB,GL_SWIZZLE_STR_ATI);
				//sample normal map
				glSampleMapATI(GL_REG_1_ATI,GL_TEXTURE1_ARB,GL_SWIZZLE_STR_ATI);
				//sample tangent space light vector
				glSampleMapATI(GL_REG_2_ATI,GL_TEXTURE2_ARB,GL_SWIZZLE_STR_ATI);
				//C4=dot(L,N)
				glColorFragmentOp2ATI(GL_DOT3_ATI,GL_REG_2_ATI, GL_NONE,GL_SATURATE_BIT_ATI,
					GL_REG_2_ATI, GL_NONE, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI,
					GL_REG_1_ATI, GL_NONE, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI);
				//out: base*diffuse
				glColorFragmentOp2ATI(GL_MAD_ATI,GL_REG_0_ATI, GL_NONE,GL_NONE,
					GL_REG_0_ATI, GL_NONE,GL_NONE,
					GL_REG_2_ATI, GL_NONE,GL_NONE);
			}
			glEndFragmentShaderATI();
		}
	}
	else {
		diffuse_bump_coords=NULL;
		specular_bump_coords=NULL;
		bump_coords=NULL;
		if((render_mode==ST_RM_DOT3) ||
			(render_mode==ST_RM_DOT3_SPECULAR)) {
			render_mode=ST_RM_BASE_TEXTURE;
		}
	}
	eye_x=eye_y=eye_z=0.0f;
	light_x=light_y=light_z=0.0f;
	diffuse_col[0]=diffuse_col[1]=diffuse_col[2]=0.7f;
	diffuse_col[3]=0.0f;
	valid_time=-1;
//	indices=new unsigned short[2*size*(size+2)];
	indices=new unsigned short[2*size*(size+1)];
	int i,j;
	for(i=0;i<size;i++) { //num strips
/*		indices[2*i*(size+2)]=i*(size+1);
		indices[2*i*(size+2)+1]=(i+1)*(size+1);
		for(j=0;j<size;j++) {
			indices[2*i*(size+2)+2+2*j]=i*(size+1)+1+j;
			indices[2*i*(size+2)+2+2*j+1]=(i+1)*(size+1)+1+j;
		}
		indices[2*i*(size+2)+2*(size+1)]=(i+1)*(size+1)+size;
		indices[2*i*(size+2)+2*(size+1)+1]=(i+1)*(size+1);*/

		indices[2*i*(size+1)]=i*(size+1);
		indices[2*i*(size+1)+1]=(i+1)*(size+1);
		for(j=0;j<size;j++) {
			indices[2*i*(size+1)+2+2*j]=i*(size+1)+1+j;
			indices[2*i*(size+1)+2+2*j+1]=(i+1)*(size+1)+1+j;
		}
	}
	is_optimized=0;
	if(hasVertexArrayRange) {
		is_optimized=1;
		agp_vert_coords=(float*)agp_alloc->get_agp_block(3*(size+1)*(size+1)*sizeof(float));
		if(!agp_vert_coords) {
			is_optimized=0;
			return;
		}
		agp_norm_coords=(float*)agp_alloc->get_agp_block(3*(size+1)*(size+1)*sizeof(float));
		if(!agp_norm_coords) {
			is_optimized=0;
			return;
		}
		if(tex_coords) {
			agp_tex_coords=(float*)agp_alloc->get_agp_block(2*(size+1)*(size+1)*sizeof(float));
			if(!agp_tex_coords) {
				is_optimized=0;
				return;
			}
		}
		if(diffuse_bump_coords) {
			agp_dif_bump_coords=(float*)agp_alloc->get_agp_block(3*(size+1)*(size+1)*sizeof(float));
			if(!agp_dif_bump_coords) {
				is_optimized=0;
				return;
			}
			if(render_mode==ST_RM_DOT3_SPECULAR) {
				agp_spec_bump_coords=(float*)agp_alloc->get_agp_block(3*(size+1)*(size+1)*sizeof(float));
				if(!agp_spec_bump_coords) {
					is_optimized=0;
					return;
				}
			}
			agp_bump_coords=(float*)agp_alloc->get_agp_block(2*(size+1)*(size+1)*sizeof(float));
			if(!agp_bump_coords) {
				is_optimized=0;
				return;
			}
		}
	}
}

void STMesh::set_diffuse_color(float r,float g,float b) {
	diffuse_col[0]=r;
	diffuse_col[1]=g;
	diffuse_col[2]=b;
}

void STMesh::set_base_texture(char* filename,float wrap_s,float wrap_t) {
	base_tex=tex_manager.get_texture(filename);
	base_wrap_s=wrap_s;
	base_wrap_t=wrap_t;
	//generate texture coordinates
	int i,j;
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			tex_coords[2*(i*(size+1)+j)]=1.0f/size*i*base_wrap_s;
			tex_coords[2*(i*(size+1)+j)+1]=1.0f/size*j*base_wrap_t;
		}
	}
	if(is_optimized) {
		memcpy((void*)agp_tex_coords,(const void*)tex_coords,
			2*sizeof(float)*(size+1)*(size+1));
	}
}

void STMesh::set_bump_map(char* filename,float wrap_s,float wrap_t) {
	bump_wrap_s=wrap_s;
	bump_wrap_t=wrap_t;
	TexLoadDesc tdesc;
	tdesc.filename=filename;
	tdesc.type=TT_BUMP_HEIGHTFIELD;
	bump_map=tex_manager.get_texture(tdesc);
	//generate texture coordinates
	int i,j;
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			bump_coords[2*(i*(size+1)+j)]=1.0f/size*i*wrap_s;
			bump_coords[2*(i*(size+1)+j)+1]=1.0f/size*j*wrap_t;
		}
	}
	if(is_optimized) {
		memcpy((void*)agp_bump_coords,(const void*)bump_coords,
			2*sizeof(float)*(size+1)*(size+1));
	}
}

void STMesh::base_tex_bump_pass() {
		c_state->textures[0].enabled=1;
		c_state->textures[0].tex=base_tex;
		c_state->textures[0].client_state_enabled=1;
		c_state->textures[0].set_texture_env(GL_REPLACE);
		c_state->textures[1].enabled=0;
		c_state->textures[1].cube_map_enabled=0;
		if(is_optimized) {
			c_state->textures[0].coord_pointer=agp_tex_coords;
			glVertexPointer(3,GL_FLOAT,0,agp_vert_coords);
			glNormalPointer(GL_FLOAT,0,agp_norm_coords);
		}
		else {
			c_state->textures[0].coord_pointer=tex_coords;
			glVertexPointer(3,GL_FLOAT,0,vert_coords);
			glNormalPointer(GL_FLOAT,0,norm_coords);
		}
		c_state->prerender_setup();
		if((!is_optimized) && hasVertexArrayRange) {
			glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
		}
//		glDrawElements(GL_TRIANGLE_STRIP,2*(size+2)*size,GL_UNSIGNED_SHORT,indices);
		int i;
		if(hasATIPNTriangles) {
			glEnable(GL_PN_TRIANGLES_ATI);
			glPNTrianglesiATI(GL_PN_TRIANGLES_POINT_MODE_ATI,GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI);
			glPNTrianglesiATI(GL_PN_TRIANGLES_NORMAL_MODE_ATI,GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI);
			glPNTrianglesiATI(GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI,2);
		}
		for(i=0;i<size;i++) glDrawElements(GL_TRIANGLE_STRIP,2*(size+1),GL_UNSIGNED_SHORT,
			&indices[2*(size+1)*i]);
		if(hasATIPNTriangles) {
			glDisable(GL_PN_TRIANGLES_ATI);
		}
		if((!is_optimized) && hasVertexArrayRange) {
			glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
		}
		c_state->postrender_setup();
}

void STMesh::diffuse_bump_pass() {
GLfloat Iambient[4] = {0.0, 0.0, 0.0, 0.0}; /* Ambient illumination. */

GLfloat Kdiffuse[4] = {
	diffuse_col[0],
	diffuse_col[1],
	diffuse_col[2],
	diffuse_col[3]
}; /* Diffuse material characteristic. */

glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, Iambient);
glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, Kdiffuse);
/*** GENERAL Combiner ZERO, RGB portion. ***/
/* Argb = 3x3 matrix column1 = expand(texture0rgb) = N’ */
glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV,
GL_TEXTURE0_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
/* Brgb = expand(texture1rgb) = L */
glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV,
GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
/* spare0rgb = Argb dot Brgb
= expand(texture0rgb) dot expand(texture1rgb) = L dot N’ */
glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB,
GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV,
GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
/*** GENERAL Combiner ZERO, Alpha portion. ***/
/* Aa = 1 */
glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV,
GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
/* Ba = expand(texture1b) = Lz */
glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV,
GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_BLUE);
/* Ca = 1 */
glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV,
GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
/* Da = expand(texture1b) = Lz */
glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV,
GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_BLUE);
/* spare0a = 4*(1*Lz + 1*Lz) = 8*expand(texture1b) */
glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA,
GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,
GL_SCALE_BY_FOUR_NV, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
/*** GENERAL Combiner ONE, RGB portion. ***/
/* Argb = spare0rgb = L dot N’ */
glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV,
GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* Brgb = expand(texture0a) = normal map denormalization factor */
glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV,
GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
/* spare0rgb = Argb Brgb = L dot N’ scaled by the normal map denormalization factor */
glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB,
GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV,
GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
/*** GENERAL Combiner ONE, Alpha portion. ***/
/* Discard all outputs. */
glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA,
GL_DISCARD_NV, GL_DISCARD_NV, GL_DISCARD_NV,
GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
/*** FINAL Combiner. ***/
/* A = spare0a = per-pixel self-shadowing term */
glFinalCombinerInputNV(GL_VARIABLE_A_NV,
GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
/* B = EF */
glFinalCombinerInputNV(GL_VARIABLE_B_NV,
GL_E_TIMES_F_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* C = zero */
glFinalCombinerInputNV(GL_VARIABLE_C_NV,
GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* D = C0 = ambient illumination contribution */
glFinalCombinerInputNV(GL_VARIABLE_D_NV,
GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* E = C1 = diffuse material characteristic */
glFinalCombinerInputNV(GL_VARIABLE_E_NV,
GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* F = spare0rgb = diffuse illumination contribution = L dot N’ */
glFinalCombinerInputNV(GL_VARIABLE_F_NV,
GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* diffuse RGB color = A*E*F + D = diffuse modulated by self-shadowing term and the
diffuse material characteristic + ambient */
/* G = spare0a = self-shadowing term = 8*expand(texture1b) */
glFinalCombinerInputNV(GL_VARIABLE_G_NV,
GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);

	c_state->want_enabled(GL_REGISTER_COMBINERS_NV);
	c_state->textures[0].clear_tex_gen_flags();
	c_state->textures[0].tex=bump_map;
	c_state->textures[0].enabled=1;
	c_state->textures[0].client_state_enabled=1;
	c_state->textures[1].clear_tex_gen_flags();
	c_state->textures[1].cube_map_enabled=1;
	c_state->textures[1].enabled=0;
	c_state->textures[1].tex=cube_normalizer;
	c_state->textures[1].client_state_enabled=1;
	if(is_optimized) {
		c_state->textures[0].coord_pointer=agp_bump_coords;
		c_state->textures[1].coord_pointer=agp_dif_bump_coords;
	}
	else {
		c_state->textures[0].coord_pointer=bump_coords;
		c_state->textures[1].coord_pointer=diffuse_bump_coords;
	}
	c_state->prerender_setup();

	if((!is_optimized) && hasVertexArrayRange) {
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
//	glDrawElements(GL_TRIANGLE_STRIP,2*(size+2)*size,GL_UNSIGNED_SHORT,indices);
		int i;
	for(i=0;i<size;i++) glDrawElements(GL_TRIANGLE_STRIP,2*(size+1),GL_UNSIGNED_SHORT,
		&indices[2*(size+1)*i]);
	if((!is_optimized) && hasVertexArrayRange) {
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	c_state->postrender_setup();
	c_state->textures[1].cube_map_enabled=0;
	c_state->want_disabled(GL_REGISTER_COMBINERS_NV);
  

}

void STMesh::specular_bump_pass() {

glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
/*** GENERAL Combiner ZERO, RGB portion. ***/
/* Argb = 3x3 matrix column1 = expand(texture0rgb) = N’ */
glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV,
GL_TEXTURE0_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
/* Brgb = expand(texture1rgb) = H */
glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV,
GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
/* spare0rgb = Argb dot Brgb = expand(texture0rgb) dot expand(texture1rgb)
= N’ dot H */
glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB,
GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV,
GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
/*** GENERAL Combiner ZERO, Alpha portion. ***/
/* Aa = texture1b = unexpanded Hz */
glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV,
GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_BLUE);
/* Ba = 1 */
glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV,
GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
/* spare0a = 1 * texture1b = unexpanded Hz */
glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA,
GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV,
GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
/*** GENERAL Combiner ONE, RGB portion. ***/
/* Argb = 0 */
glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV,
GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* Brgb = 0 */
glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV,
GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* Crgb = spare0rgb = H dot N’ */
glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV,
GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* Drgb = spare0rgb = H dot N’ */
glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV,
GL_SPARE0_NV, GL_SIGNED_IDENTITY_NV, GL_RGB);
/* spare0rgb = ((spare0a >= 0.5) ? spare0rgb^2 : 0)
= ((H dot N > 0) ? (H dot N’)^2 : 0) */
glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB,
GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,
GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_TRUE);
/*** GENERAL Combiner ONE, Alpha portion. ***/
/* Discard all outputs. */
glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA,
GL_DISCARD_NV, GL_DISCARD_NV, GL_DISCARD_NV,
GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
/*** FINAL Combiner. ***/
/* A = EF */
glFinalCombinerInputNV(GL_VARIABLE_A_NV,
GL_E_TIMES_F_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* B = EF */
glFinalCombinerInputNV(GL_VARIABLE_B_NV,
GL_E_TIMES_F_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* C = zero */
glFinalCombinerInputNV(GL_VARIABLE_C_NV,
GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* D = zero = no extra specular illumination contribution */
glFinalCombinerInputNV(GL_VARIABLE_D_NV,
GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* F = spare0rgb = (H dot N’)^2 */
glFinalCombinerInputNV(GL_VARIABLE_E_NV,
GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* F = spare0rgb = (H dot N’)^2 */
glFinalCombinerInputNV(GL_VARIABLE_F_NV,
GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
/* specular RGB color = A*B = (E*F)*(E*F) = (H dot N’)^8 */
/* G = 0 */
glFinalCombinerInputNV(GL_VARIABLE_G_NV,
GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);

	c_state->want_enabled(GL_REGISTER_COMBINERS_NV);
	c_state->textures[0].clear_tex_gen_flags();
	c_state->textures[0].tex=bump_map;
	c_state->textures[0].enabled=1;
	c_state->textures[0].client_state_enabled=1;
	c_state->textures[1].clear_tex_gen_flags();
	c_state->textures[1].cube_map_enabled=1;
	c_state->textures[1].enabled=0;
	c_state->textures[1].tex=cube_normalizer;
	c_state->textures[1].client_state_enabled=1;
	if(is_optimized) {
		c_state->textures[0].coord_pointer=agp_bump_coords;
		c_state->textures[1].coord_pointer=agp_spec_bump_coords;
	}
	else {
		c_state->textures[0].coord_pointer=bump_coords;
		c_state->textures[1].coord_pointer=specular_bump_coords;
	}
	c_state->prerender_setup();

	//quick hack!!!!!!!!!!!!!!
/*	c_state->want_active_client_texture_ARB(GL_TEXTURE1_ARB);
	if(is_optimized) {
		glTexCoordPointer(3,GL_FLOAT,0,agp_spec_bump_coords);
	}
	else {
		glTexCoordPointer(3,GL_FLOAT,0,specular_bump_coords);
	}*/

	if((!is_optimized) && hasVertexArrayRange) {
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
//	glDrawElements(GL_TRIANGLE_STRIP,2*(size+2)*size,GL_UNSIGNED_SHORT,indices);
	int i;
	for(i=0;i<size;i++) glDrawElements(GL_TRIANGLE_STRIP,2*(size+1),GL_UNSIGNED_SHORT,
		&indices[2*(size+1)*i]);
	if((!is_optimized) && hasVertexArrayRange) {
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	c_state->postrender_setup();
	c_state->textures[1].cube_map_enabled=0;
	c_state->want_disabled(GL_REGISTER_COMBINERS_NV);
  
}

void STMesh::render_base_only() {
	unsigned int i;
	c_state->want_enabled(GL_LIGHTING);
	for(i=0;i<max_texture_units;i++) {
		c_state->textures[i].clear_tex_gen_flags();
		c_state->textures[i].enabled=0;
		c_state->textures[i].cube_map_enabled=0;
	}
	c_state->textures[0].enabled=1;
	c_state->textures[0].tex=base_tex;
	c_state->textures[0].client_state_enabled=1;
	c_state->textures[0].set_texture_env(GL_MODULATE);
	if(is_optimized) {
		c_state->textures[0].coord_pointer=agp_tex_coords;
		glVertexPointer(3,GL_FLOAT,0,agp_vert_coords);
		glNormalPointer(GL_FLOAT,0,agp_norm_coords);
	}
	else {
		c_state->textures[0].coord_pointer=tex_coords;
		glVertexPointer(3,GL_FLOAT,0,vert_coords);
		glNormalPointer(GL_FLOAT,0,norm_coords);
	}
	c_state->prerender_setup();
	if((!is_optimized) && hasVertexArrayRange) {
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	if(hasCompiledVertexArray) {
		glLockArraysEXT(0,2*(size+1)*size);
	}
//	glDrawElements(GL_TRIANGLE_STRIP,2*(size+2)*size,GL_UNSIGNED_SHORT,indices);
	if(hasATIPNTriangles) {
		glEnable(GL_PN_TRIANGLES_ATI);
		glPNTrianglesiATI(GL_PN_TRIANGLES_POINT_MODE_ATI,GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI);
		glPNTrianglesiATI(GL_PN_TRIANGLES_NORMAL_MODE_ATI,GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI);
		glPNTrianglesiATI(GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI,2);
	}
	for(i=0;i<(unsigned int)size;i++) glDrawElements(GL_TRIANGLE_STRIP,2*(size+1),GL_UNSIGNED_SHORT,
		&indices[2*(size+1)*i]);
	if(hasATIPNTriangles) {
		glDisable(GL_PN_TRIANGLES_ATI);
	}
	if(hasCompiledVertexArray) {
		glUnlockArraysEXT();
	}
	if((!is_optimized) && hasVertexArrayRange) {
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	c_state->postrender_setup();
}

void STMesh::diffuse_bump_pass_arb() {
	c_state->textures[0].clear_tex_gen_flags();
	c_state->textures[0].tex=bump_map;
	c_state->textures[0].enabled=1;
	c_state->textures[0].client_state_enabled=1;
	c_state->textures[1].clear_tex_gen_flags();
	c_state->textures[1].cube_map_enabled=1;
	c_state->textures[1].enabled=0;
	c_state->textures[1].tex=cube_normalizer;
	c_state->textures[1].client_state_enabled=1;
//	c_state->textures[1].set_texture_env(GL_REPLACE);
	c_state->textures[1].set_texture_env(GL_COMBINE_EXT);
	c_state->textures[1].env_combine_param(GL_SOURCE0_RGB_EXT,GL_PREVIOUS_EXT);
	c_state->textures[1].env_combine_param(GL_SOURCE1_RGB_EXT,GL_TEXTURE);
	c_state->textures[1].env_combine_param(GL_OPERAND0_RGB_EXT,GL_SRC_COLOR);
	c_state->textures[1].env_combine_param(GL_OPERAND1_RGB_EXT,GL_SRC_COLOR);
	c_state->textures[1].env_combine_param(GL_SOURCE0_ALPHA_EXT,GL_PREVIOUS_EXT);
	c_state->textures[1].env_combine_param(GL_SOURCE1_ALPHA_EXT,GL_TEXTURE);
	c_state->textures[1].env_combine_param(GL_OPERAND0_ALPHA_EXT,GL_SRC_ALPHA);
	c_state->textures[1].env_combine_param(GL_OPERAND1_ALPHA_EXT,GL_SRC_ALPHA);
	c_state->textures[1].env_combine_param(GL_COMBINE_RGB_EXT,GL_DOT3_RGBA_ARB);
	c_state->textures[1].force_combine_update=1;

	if(is_optimized) {
		c_state->textures[0].coord_pointer=agp_bump_coords;
		c_state->textures[1].coord_pointer=agp_dif_bump_coords;
	}
	else {
		c_state->textures[0].coord_pointer=bump_coords;
		c_state->textures[1].coord_pointer=diffuse_bump_coords;
	}
	c_state->prerender_setup();

	if((!is_optimized) && hasVertexArrayRange) {
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	int i;
	for(i=0;i<size;i++) glDrawElements(GL_TRIANGLE_STRIP,2*(size+1),GL_UNSIGNED_SHORT,
		&indices[2*(size+1)*i]);
	if((!is_optimized) && hasVertexArrayRange) {
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	c_state->postrender_setup();
	c_state->textures[1].cube_map_enabled=0;
}

void STMesh::render_diffuse_only() {
	if(!(hasRegisterCombiners && hasCubeMaps)) render_base_only();
	if(is_optimized) {
		glVertexPointer(3,GL_FLOAT,0,agp_vert_coords);
		glNormalPointer(GL_FLOAT,0,agp_norm_coords);
	}
	else {
		glVertexPointer(3,GL_FLOAT,0,vert_coords);
		glNormalPointer(GL_FLOAT,0,norm_coords);
	}
	diffuse_bump_pass();
	c_state->want_disabled(GL_REGISTER_COMBINERS_NV);
}

void STMesh::dot3_bump_fragment_shader() {
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Entered ATI_fragment_shader bump");
#endif
	//setup base texture
	unsigned int i;
	for(i=0;i<max_texture_units;i++) {
		c_state->textures[i].clear_tex_gen_flags();
		c_state->textures[i].enabled=0;
		c_state->textures[i].cube_map_enabled=0;
	}
	c_state->textures[0].enabled=1;
	c_state->textures[0].tex=base_tex;
	c_state->textures[0].client_state_enabled=1;
	if(is_optimized) {
		c_state->textures[0].coord_pointer=agp_tex_coords;
		glVertexPointer(3,GL_FLOAT,0,agp_vert_coords);
		glNormalPointer(GL_FLOAT,0,agp_norm_coords);
	}
	else {
		c_state->textures[0].coord_pointer=tex_coords;
		glVertexPointer(3,GL_FLOAT,0,vert_coords);
		glNormalPointer(GL_FLOAT,0,norm_coords);
	}
	//setup normal map
	c_state->textures[1].clear_tex_gen_flags();
	c_state->textures[1].tex=bump_map;
	c_state->textures[1].enabled=1;
	c_state->textures[1].client_state_enabled=1;
	if(is_optimized) {
		c_state->textures[1].coord_pointer=agp_bump_coords;
	}
	else {
		c_state->textures[1].coord_pointer=bump_coords;
	}
	//setup tangent space light vector normalizer
	c_state->textures[2].clear_tex_gen_flags();
	c_state->textures[2].cube_map_enabled=1;
	c_state->textures[2].enabled=0;
	c_state->textures[2].tex=cube_normalizer;
	c_state->textures[2].client_state_enabled=1;
	if(is_optimized) {
		c_state->textures[2].coord_pointer=agp_dif_bump_coords;
	}
	else {
		c_state->textures[2].coord_pointer=diffuse_bump_coords;
	}
	if(render_mode==ST_RM_DOT3_SPECULAR) {
		//setup tangent space halfangle vector normalizer
		c_state->textures[3].clear_tex_gen_flags();
		c_state->textures[3].cube_map_enabled=1;
		c_state->textures[3].enabled=0;
		c_state->textures[3].tex=cube_normalizer;
		c_state->textures[3].client_state_enabled=1;
		if(is_optimized) {
			c_state->textures[3].coord_pointer=agp_spec_bump_coords;
		}
		else {
			c_state->textures[3].coord_pointer=specular_bump_coords;
		}
	}
	c_state->prerender_setup();
	glEnable(GL_FRAGMENT_SHADER_ATI);
	glBindFragmentShaderATI(fragment_shader);
	if((!is_optimized) && hasVertexArrayRange) {
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	if(hasCompiledVertexArray) {
		glLockArraysEXT(0,2*(size+1)*size);
	}
	if(hasATIPNTriangles) {
		glEnable(GL_PN_TRIANGLES_ATI);
		glPNTrianglesiATI(GL_PN_TRIANGLES_POINT_MODE_ATI,GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI);
		glPNTrianglesiATI(GL_PN_TRIANGLES_NORMAL_MODE_ATI,GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI);
		glPNTrianglesiATI(GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI,2);
	}
	for(i=0;i<(unsigned int)size;i++) glDrawElements(GL_TRIANGLE_STRIP,2*(size+1),GL_UNSIGNED_SHORT,
		&indices[2*(size+1)*i]);
	if(hasATIPNTriangles) {
		glDisable(GL_PN_TRIANGLES_ATI);
	}
	if(hasCompiledVertexArray) {
		glUnlockArraysEXT();
	}
	if((!is_optimized) && hasVertexArrayRange) {
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	glDisable(GL_FRAGMENT_SHADER_ATI);
	c_state->postrender_setup();
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Leaving ATI_fragment_shader bump");
#endif
}

void STMesh::render(int time) {
//	int i;
	switch(render_mode) {
	case ST_RM_BASE_TEXTURE:
		render_base_only();
	break;
	case ST_RM_DOT3:
	case ST_RM_DOT3_SPECULAR:
		if(hasATIFragmentShader) {
			dot3_bump_fragment_shader();
			return;
		}
		if(!(hasRegisterCombiners && hasCubeMaps)) return;
		if(is_optimized) {
			glVertexPointer(3,GL_FLOAT,0,agp_vert_coords);
			glNormalPointer(GL_FLOAT,0,agp_norm_coords);
		}
		else {
			glVertexPointer(3,GL_FLOAT,0,vert_coords);
			glNormalPointer(GL_FLOAT,0,norm_coords);
		}
		diffuse_bump_pass();
		c_state->want_enabled(GL_BLEND);
		glBlendFunc(GL_DST_COLOR,GL_ZERO);
		glColorMask(1,1,1,0);
		base_tex_bump_pass();
		glColorMask(1,1,1,1);
		c_state->want_enabled(GL_BLEND);
		glBlendFunc(GL_DST_ALPHA,GL_ONE);
		if(render_mode==ST_RM_DOT3_SPECULAR) {
//			glColor4f(1.0f,1.0f,1.0f,0.5f);
//			c_state->want_disabled(GL_REGISTER_COMBINERS_NV);
//			DrawQuad(NULL,0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,1.0f,1,GL_DST_COLOR,
//				GL_ZERO);
//			c_state->want_enabled(GL_REGISTER_COMBINERS_NV);
//			c_state->want_enabled(GL_DEPTH_TEST);
			glBlendFunc(GL_DST_ALPHA,GL_ONE);
			specular_bump_pass();
		}
		c_state->want_disabled(GL_REGISTER_COMBINERS_NV);
	break;
	case ST_RM_DOT3_ARB:
		if(!(hasTextureEnvDot3 && hasCubeMaps)) return;
		if(is_optimized) {
			glVertexPointer(3,GL_FLOAT,0,agp_vert_coords);
			glNormalPointer(GL_FLOAT,0,agp_norm_coords);
		}
		else {
			glVertexPointer(3,GL_FLOAT,0,vert_coords);
			glNormalPointer(GL_FLOAT,0,norm_coords);
		}
		c_state->want_disabled(GL_LIGHTING);
		diffuse_bump_pass_arb();
		c_state->want_enabled(GL_LIGHTING);
		c_state->want_enabled(GL_BLEND);
		glBlendFunc(GL_DST_COLOR,GL_ZERO);
//		glColorMask(1,1,1,0);
		base_tex_bump_pass();
//		glColorMask(1,1,1,1);
//		c_state->want_enabled(GL_BLEND);
	break;
	}
}


//based (as most of bump mapping code) on NVIDIA's
//"Practical and Robust Bump Mapping ..."
void STMesh::update_cube_map_coords() {
	int i;
	float x, y, z, xx, yy, zz, invlen;
	float *vertex,*tangent_vec,*binormal_vec,*normal,
		*tangent_light,*tangent_half_light;
	for (i=0; i<(size+1)*(size+1); i++) {
		vertex=&vert_coords[3*i];
		tangent_vec=&tangent[3*i];
		binormal_vec=&binormal[3*i];
		normal=&norm_coords[3*i];
		tangent_light=&diffuse_bump_coords[3*i];
		tangent_half_light=&specular_bump_coords[3*i];
		/* Subtract vertex position from the light position to compute the
		(unnormalized) direction vector to the light in object space. */
		x = light_x - vertex[0];
		y = light_y - vertex[1];
		z = light_z - vertex[2];
		/* Rotate the direction vector to the light into the vertex’s
		tangent space. */
		xx = x*tangent_vec[0] + y*tangent_vec[1] + z*tangent_vec[2];
		yy = x*binormal_vec[0] + y*binormal_vec[1] + z*binormal_vec[2];
		zz = x*normal[0] + y*normal[1] + z*normal[2];
		
		/* Normalize the tangent-space light vector. */
		invlen = 1.0/sqrt(xx*xx+yy*yy+zz*zz);
		tangent_light[0] = xx*invlen;
		tangent_light[1] = yy*invlen;
		tangent_light[2] = zz*invlen;
		if(render_mode==ST_RM_DOT3_SPECULAR) {
			/* Subtract vertex position from the eye position to compute the
			(unnormalized) direction vector to the eye in object space. */
			x = eye_x - vertex[0];
			y = eye_y - vertex[1];
			z = eye_z - vertex[2];
			/* Rotate the direction vector to the eye into the vertex’s
			tangent space. */
			xx = x*tangent_vec[0] + y*tangent_vec[1] + z*tangent_vec[2];
			yy = x*binormal_vec[0] + y*binormal_vec[1] + z*binormal_vec[2];
			zz = x*normal[0] + y*normal[1] + z*normal[2];
			/* Normalize the tangent-space eye vector. */
			invlen = 1.0/sqrt(xx*xx+yy*yy+zz*zz);
			xx = xx*invlen;
			yy = yy*invlen;
			zz = zz*invlen;
			/* Form the half-angle vector by adding the normalized light and eye vectors. */
			xx += tangent_light[0];
			yy += tangent_light[1];
			zz += tangent_light[2];
			invlen = 1.0/sqrt(xx*xx+yy*yy+zz*zz);
			/* Store the normalized tangent-space half-angle vector. */
			tangent_half_light[0] = xx*invlen;
			tangent_half_light[1] = yy*invlen;
			tangent_half_light[2] = zz*invlen;
		}
	}
}

void STMesh::update(int time) {
//	int i,j;
	if(valid_time!=time) {
		update_vertices(time);
		update_tangents(time);
		update_binormals(time);
		update_normals();
			//fast fix :))
//			for(i=0;i<(size+1)*(size+1);i++) {
//				VCrossProduct3(&norm_coords[3*i],&tangent[3*i],&binormal[3*i]);
//			}
		are_tangents_valid=1;
		if(is_optimized) {
			memcpy((void*)agp_norm_coords,(const void*)norm_coords,
				3*sizeof(float)*(size+1)*(size+1));
			memcpy((void*)agp_vert_coords,(const void*)vert_coords,
				3*sizeof(float)*(size+1)*(size+1));
		}
		if((render_mode==ST_RM_DOT3) ||
			(render_mode==ST_RM_DOT3_SPECULAR)
			|| (render_mode==ST_RM_DOT3_ARB)) {
			update_cube_map_coords();
			if(is_optimized) {
				memcpy((void*)agp_dif_bump_coords,(const void*)diffuse_bump_coords,
					3*sizeof(float)*(size+1)*(size+1));
				if(render_mode==ST_RM_DOT3_SPECULAR) {
					memcpy((void*)agp_spec_bump_coords,(const void*)specular_bump_coords,
						3*sizeof(float)*(size+1)*(size+1));
				}
			}
		}
	}
	if(!are_tangents_valid) {
		update_tangents(time);
		update_binormals(time);
		update_normals();
			//fast fix :))
//			for(i=0;i<(size+1)*(size+1);i++) {
//				VCrossProduct3(&norm_coords[3*i],&tangent[3*i],&binormal[3*i]);
//			}
		are_tangents_valid=1;
		if(is_optimized) {
			memcpy((void*)agp_norm_coords,(const void*)norm_coords,
				3*sizeof(float)*(size+1)*(size+1));
		}
		if((render_mode==ST_RM_DOT3) ||
			(render_mode==ST_RM_DOT3_SPECULAR)
			|| (render_mode==ST_RM_DOT3_ARB)) {
			update_cube_map_coords();
			if(is_optimized) {
				memcpy((void*)agp_dif_bump_coords,(const void*)diffuse_bump_coords,
					3*sizeof(float)*(size+1)*(size+1));
				if(render_mode==ST_RM_DOT3_SPECULAR) {
					memcpy((void*)agp_spec_bump_coords,(const void*)specular_bump_coords,
						3*sizeof(float)*(size+1)*(size+1));
				}
			}
		}
	}
}

void STMesh::set_light_pos(float x,float y,float z) {
	light_x=x;
	light_y=y;
	light_z=z;
	are_tangents_valid=0;
}

void STMesh::set_light_pos_indirect(Matrix obj_transform,float x,float y,float z) {
	Matrix mxi;
	MInvert(obj_transform,mxi);
	Vector3 world_pos,obj_pos;
	world_pos[0]=x;
	world_pos[1]=y;
	world_pos[2]=z;
	MVMultiply3(mxi,world_pos,obj_pos);
	light_x=obj_pos[0];
	light_y=obj_pos[1];
	light_z=obj_pos[2];
	are_tangents_valid=0;
}

void STMesh::set_eye_pos(float x,float y,float z) {
	eye_x=x;
	eye_y=y;
	eye_z=z;
	are_tangents_valid=0;
}

void STMesh::set_eye_pos_indirect(Matrix obj_transform,float x,float y,float z) {
	Matrix mxi;
	MInvert(obj_transform,mxi);
	Vector3 world_pos,obj_pos;
	world_pos[0]=x;
	world_pos[1]=y;
	world_pos[2]=z;
	MVMultiply3(mxi,world_pos,obj_pos);
	eye_x=obj_pos[0];
	eye_y=obj_pos[1];
	eye_z=obj_pos[2];
	are_tangents_valid=0;
}

void STMesh::update_normals() {
	int i;
	for(i=0;i<(size+1)*(size+1);i++) {
		VCrossProduct3(&binormal[3*i],&tangent[3*i],&norm_coords[3*i]);
		VNormalize3(&norm_coords[3*i]);
	}
}

STMesh::~STMesh() {
	if(vert_coords) delete vert_coords;
	if(norm_coords) delete norm_coords;
	if(tex_coords) delete tex_coords;
	if(diffuse_bump_coords) delete diffuse_bump_coords;
	if(specular_bump_coords) specular_bump_coords;
	if(indices) delete indices;
	if((render_mode==ST_RM_DOT3) ||
		(render_mode==ST_RM_DOT3_SPECULAR)) {
		if(hasATIFragmentShader) {
			glDeleteFragmentShaderATI(fragment_shader);
		}
	}
}

//derived classes implementation

#define PI 3.14159f
#define HALFPI 1.570795f


STSphere::STSphere(ST_MESH_RENDER_MODE rm, int subdivisions) :
				STMesh(rm,subdivisions) {
	ampl=0.0f;
	s_waves=t_waves=0;
}

void STSphere::update_tangents(int time) {
	int i,j;
	float s,t,r,rt;
	float cos_s,cos_t;
	float sin_s,sin_t;
	for(i=0;i<size;i++) {
		for(j=0;j<size;j++) {
			s=(1.0f/size*i)*2.0f*PI;
			t=(1.0f/size*j-0.5f)*PI;
			cos_s=FastCos(s);
			cos_t=FastCos(t);
			sin_s=FastSin(s);
			sin_t=FastSin(t);
			rt=ampl*FastSin(s*s_waves)*t_waves*FastCos(t*t_waves);
			r=1.0f+ampl*FastSin(s*s_waves)*FastSin(t*t_waves);

			tangent[3*(i*(size+1)+j)]=-r*sin_t*cos_s+rt*cos_t*cos_s;
			tangent[3*(i*(size+1)+j)+1]=-r*sin_t*sin_s+rt*cos_t*sin_s;
			tangent[3*(i*(size+1)+j)+2]=r*cos_t+rt*sin_t;
		}
	}
	for(i=0;i<(size+1);i++) {
		tangent[3*(i*(size+1)+size)]=tangent[3*i*(size+1)];
		tangent[3*(i*(size+1)+size)+1]=tangent[3*i*(size+1)+1];
		tangent[3*(i*(size+1)+size)+2]=tangent[3*i*(size+1)+2];
	}
	for(j=0;j<(size+1);j++) {
		tangent[3*(size*(size+1)+j)]=tangent[3*j];
		tangent[3*(size*(size+1)+j)+1]=tangent[3*j+1];
		tangent[3*(size*(size+1)+j)+2]=tangent[3*j+2];
	}
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			VNormalize3(&tangent[3*(i*(size+1)+j)]);
		}
	}
}

void STSphere::update_binormals(int time) {
	
	int i,j;
	float s,t,r,rs;
	float cos_s,cos_t;
	float sin_s,sin_t;
	for(i=0;i<size;i++) {
		for(j=0;j<size;j++) {
			s=(1.0f/size*i)*2.0f*PI;
			t=(1.0f/size*j-0.5f)*PI;
			cos_s=FastCos(s);
			cos_t=FastCos(t);
			sin_s=FastSin(s);
			sin_t=FastSin(t);
			rs=ampl*FastSin(t*t_waves)*s_waves*FastCos(s*s_waves);
			r=1.0f+ampl*FastSin(s*s_waves)*FastSin(t*t_waves);
			binormal[3*(i*(size+1)+j)]=-r*sin_s*cos_t+rs*cos_t*cos_s;
			binormal[3*(i*(size+1)+j)+1]=r*cos_t*cos_s+rs*cos_t*sin_s;
			binormal[3*(i*(size+1)+j)+2]=rs*sin_t;
		}
	}
	for(i=0;i<(size+1);i++) {
		binormal[3*(i*(size+1)+size)]=binormal[3*i*(size+1)];
		binormal[3*(i*(size+1)+size)+1]=binormal[3*i*(size+1)+1];
		binormal[3*(i*(size+1)+size)+2]=binormal[3*i*(size+1)+2];
	}
	for(j=0;j<(size+1);j++) {
		binormal[3*(size*(size+1)+j)]=binormal[3*j];
		binormal[3*(size*(size+1)+j)+1]=binormal[3*j+1];
		binormal[3*(size*(size+1)+j)+2]=binormal[3*j+2];
	}
	//normalizacja
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			VNormalize3(&binormal[3*(i*(size+1)+j)]);
		}
	}
}

void STSphere::update_vertices(int time) {
	int i,j;
	float s,t,r;
	float cos_s,cos_t;
	float sin_s,sin_t;
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			s=(1.0f/size*i)*2.0f*PI;
			t=(1.0f/size*j-0.5f)*PI;
			cos_s=FastCos(s);
			cos_t=FastCos(t);
			sin_s=FastSin(s);
			sin_t=FastSin(t);
			r=1.0f+ampl*FastSin(s*s_waves)*FastSin(t*t_waves);
			vert_coords[3*(i*(size+1)+j)]=r*cos_t*cos_s;
			vert_coords[3*(i*(size+1)+j)+1]=r*cos_t*sin_s;
			vert_coords[3*(i*(size+1)+j)+2]=r*sin_t;
		}
	}
}

void STSphere::set_num_waves(int s,int t) {
	s_waves=s;
	t_waves=t;
}

void STSphere::set_wave_amplitude(float a) {
	ampl=a;
}


//Torus
STTorus::STTorus(ST_MESH_RENDER_MODE rm,int subdivisions) :
STMesh(rm,subdivisions) {
	R_waves=r_waves=0;
	R_ampl=r_ampl=0;
}

void STTorus::set_R_wave_amplitude(float R) {
	R_ampl=R;
}

void STTorus::set_r_wave_amplitude(float r) {
	r_ampl=r;
}

void STTorus::set_num_waves(int R,int r) {
	R_waves=R;
	r_waves=r;
}

void STTorus::update_tangents(int time) {
	//dF/ds
	int i,j;
	float s,t,r,R;
	float cos_s,cos_t;
	float sin_s,sin_t;
	R=1.0f;
	r=0.4f;
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			s=(1.0f/size*i)*2.0f*PI;
			t=(1.0f/size*j)*2.0f*PI;
			cos_s=FastCos(s);
			cos_t=FastCos(t);
			sin_s=FastSin(s);
			sin_t=FastSin(t);
			r=0.4f+r_ampl*FastSin(t*r_waves);
			R=1.0f+R_ampl*FastSin(t*R_waves);
			binormal[3*(i*(size+1)+j)]=-sin_s*cos_t*r;
			binormal[3*(i*(size+1)+j)+1]=sin_s*sin_t*r;
			binormal[3*(i*(size+1)+j)+2]=cos_s*r;
		}
	}
	for(i=0;i<(size+1);i++) {
		binormal[3*(i*(size+1)+size)]=binormal[3*i*(size+1)];
		binormal[3*(i*(size+1)+size)+1]=binormal[3*i*(size+1)+1];
		binormal[3*(i*(size+1)+size)+2]=binormal[3*i*(size+1)+2];
	}
	for(j=0;j<(size+1);j++) {
		binormal[3*(size*(size+1)+j)]=binormal[3*j];
		binormal[3*(size*(size+1)+j)+1]=binormal[3*j+1];
		binormal[3*(size*(size+1)+j)+2]=binormal[3*j+2];
	}
	//normalizacja
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			VNormalize3(&binormal[3*(i*(size+1)+j)]);
		}
	}
}

void STTorus::update_binormals(int time) {
	//dF/dt
	int i,j;
	float s,t,r,R,rt,Rt;
	float cos_s,cos_t;
	float sin_s,sin_t;
	R=1.0f;
	r=0.4f;
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			s=(1.0f/size*i)*2.0f*PI;
			t=(1.0f/size*j)*2.0f*PI;
			cos_s=FastCos(s);
			cos_t=FastCos(t);
			sin_s=FastSin(s);
			sin_t=FastSin(t);
			r=0.4f+r_ampl*FastSin(t*r_waves);
			R=1.0f+R_ampl*FastSin(t*R_waves);
			rt=r_ampl*r_waves*FastCos(t*r_waves);
			Rt=r_ampl*R_waves*FastCos(t*R_waves);
			tangent[3*(i*(size+1)+j)]=-sin_t*(R+r*cos_s)+cos_t*(Rt+rt*cos_s);
			tangent[3*(i*(size+1)+j)+1]=-cos_t*(R+r*cos_s)-sin_t*(Rt+rt*cos_s);
			tangent[3*(i*(size+1)+j)+2]=rt*sin_s;
		}
	}
	for(i=0;i<(size+1);i++) {
		tangent[3*(i*(size+1)+size)]=tangent[3*i*(size+1)];
		tangent[3*(i*(size+1)+size)+1]=tangent[3*i*(size+1)+1];
		tangent[3*(i*(size+1)+size)+2]=tangent[3*i*(size+1)+2];
	}
	for(j=0;j<(size+1);j++) {
		tangent[3*(size*(size+1)+j)]=tangent[3*j];
		tangent[3*(size*(size+1)+j)+1]=tangent[3*j+1];
		tangent[3*(size*(size+1)+j)+2]=tangent[3*j+2];
	}
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			VNormalize3(&tangent[3*(i*(size+1)+j)]);
		}
	}
}

void STTorus::update_vertices(int time) {
 	int i,j;
	float s,t,r,R;
	float cos_s,cos_t;
	float sin_s,sin_t;
	R=1.0f;
	r=0.4f;
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			s=(1.0f/size*i)*2.0f*PI;
			t=(1.0f/size*j)*2.0f*PI;
			cos_s=FastCos(s);
			cos_t=FastCos(t);
			sin_s=FastSin(s);
			sin_t=FastSin(t);
			r=0.4f+r_ampl*FastSin(t*r_waves);
			R=1.0f+R_ampl*FastSin(t*R_waves);
			vert_coords[3*(i*(size+1)+j)]=cos_t*(R+r*cos_s);
			vert_coords[3*(i*(size+1)+j)+1]=-sin_t*(R+r*cos_s);
			vert_coords[3*(i*(size+1)+j)+2]=r*sin_s;
		}
	}
}

STMeshMorph::STMeshMorph(ST_MESH_RENDER_MODE rm,int subdivisions) :
STMesh(rm,subdivisions) {
}

STMeshMorph::~STMeshMorph() {
}

void STMeshMorph::add_mesh(STMesh *m) {
	meshes.add_element(m);
}

void STMeshMorph::set_morph_env(Envelope &env) {
	amount=env;
}

void STMeshMorph::update_binormals(int time) {
	amount.calc(time*0.03);
	if(meshes.length==0) return;
	int source=(int)amount.res;
	int target;
	if(source>=meshes.length) source=meshes.length-1;
	if(source<0) source=0;
	if((source+1)>=meshes.length) target=source;
	else target=source+1;
	float coeff=amount.res-floor(amount.res);
	if(amount.res<0.0) coeff=0.0f;
	meshes[source]->update_binormals(time);
	meshes[target]->update_binormals(time);
	int i;
	float *data1,*data2;
	data1=meshes[source]->binormal;
	data2=meshes[target]->binormal;
	for(i=0;i<3*(size+1)*(size+1);i++) {
		binormal[i]=(1.0f-coeff)*data1[i]+coeff*data2[i];
	}
}

void STMeshMorph::update_tangents(int time) {
	amount.calc(time*0.03);
	if(meshes.length==0) return;
	int source=(int)amount.res;
	int target;
	if(source>=meshes.length) source=meshes.length-1;
	if(source<0) source=0;
	if((source+1)>=meshes.length) target=source;
	else target=source+1;
	float coeff=amount.res-floor(amount.res);
	if(amount.res<0.0) coeff=0.0f;
	meshes[source]->update_tangents(time);
	meshes[target]->update_tangents(time);
	int i;
	float *data1,*data2;
	data1=meshes[source]->tangent;
	data2=meshes[target]->tangent;
	for(i=0;i<3*(size+1)*(size+1);i++) {
		tangent[i]=(1.0f-coeff)*data1[i]+coeff*data2[i];
	}
}

void STMeshMorph::update_vertices(int time) {
	amount.calc(time*0.03);
	if(meshes.length==0) return;
	int source=(int)amount.res;
	int target;
	if(source>=meshes.length) source=meshes.length-1;
	if(source<0) source=0;
	if((source+1)>=meshes.length) target=source;
	else target=source+1;
	float coeff=(float)amount.res-floor(amount.res);
	if(amount.res<0.0) coeff=0.0f;
	meshes[source]->update_vertices(time);
	meshes[target]->update_vertices(time);
	int i;
	float *data1,*data2;
	data1=meshes[source]->vert_coords;
	data2=meshes[target]->vert_coords;
	for(i=0;i<3*(size+1)*(size+1);i++) {
		vert_coords[i]=(1.0f-coeff)*data1[i]+coeff*data2[i];
	}
}

STXYQuad::STXYQuad(float xsize,float ysize,ST_MESH_RENDER_MODE rm,int subdivisions)
: STMesh(rm,subdivisions) {
 	int i,j;
	float s,t;
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			s=(1.0f/size*i);
			t=(1.0f/size*j);
			vert_coords[3*(i*(size+1)+j)]=t*xsize;
			vert_coords[3*(i*(size+1)+j)+1]=0.0f;
			vert_coords[3*(i*(size+1)+j)+2]=s*ysize;
			tangent[3*(i*(size+1)+j)]=1.0f;
			tangent[3*(i*(size+1)+j)+1]=0.0f;
			tangent[3*(i*(size+1)+j)+2]=0.0f;
			binormal[3*(i*(size+1)+j)]=0.0f;
			binormal[3*(i*(size+1)+j)+1]=0.0f;
			binormal[3*(i*(size+1)+j)+2]=1.0f;
		}
	}
	update(0);
}

STHeightMap::STHeightMap(Image *height_map,float xsize,float ysize,
						 ST_MESH_RENDER_MODE rm,int subdivisions):
STMesh(rm,subdivisions) {
	hmap=height_map;
	xsiz=xsize;
	ysiz=ysize;
}

void STHeightMap::set_hmap(Image *height_map) {
	hmap=height_map;
}

void STHeightMap::set_hmap_strength(float s) {
	strength=s;
}

void STHeightMap::update_binormals(int time) {
 	int i,j;
	register int tmp_int;
	register float inv_siz=1.0f/ysiz;
	for(i=0;i<size;i++) {
		for(j=0;j<size;j++) {
			tmp_int=3*(i*(size+1)+j);
			binormal[tmp_int]=0.0f;
			binormal[tmp_int+1]=2.0f*
				(vert_coords[tmp_int+3*size+4]
				-vert_coords[tmp_int+1]);
			binormal[tmp_int+2]=inv_siz;
		}
	}
	for(i=0;i<(size+1);i++) {
		binormal[3*(i*(size+1)+size)]=binormal[3*i*(size+1)];
		binormal[3*(i*(size+1)+size)+1]=binormal[3*i*(size+1)+1];
		binormal[3*(i*(size+1)+size)+2]=binormal[3*i*(size+1)+2];
	}
	for(j=0;j<(size+1);j++) {
		binormal[3*(size*(size+1)+j)]=binormal[3*j];
		binormal[3*(size*(size+1)+j)+1]=binormal[3*j+1];
		binormal[3*(size*(size+1)+j)+2]=binormal[3*j+2];
	}
	//normalizacja
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			VNormalize3(&binormal[3*(i*(size+1)+j)]);
		}
	}
}

void STHeightMap::update_tangents(int time) {
 	int i,j;
	register int tmp_int;
	register float inv_siz=1.0f/xsiz;
	for(i=0;i<size;i++) {
		for(j=0;j<size;j++) {
			tmp_int=3*(i*(size+1)+j);
			tangent[tmp_int]=inv_siz;
			tangent[tmp_int+1]=2.0f*
				(vert_coords[tmp_int+4]
				-vert_coords[tmp_int+1]);
			tangent[tmp_int+2]=0.0f;
		}
	}
	for(i=0;i<(size+1);i++) {
		tangent[3*(i*(size+1)+size)]=tangent[3*i*(size+1)];
		tangent[3*(i*(size+1)+size)+1]=tangent[3*i*(size+1)+1];
		tangent[3*(i*(size+1)+size)+2]=tangent[3*i*(size+1)+2];
	}
	for(j=0;j<(size+1);j++) {
		tangent[3*(size*(size+1)+j)]=tangent[3*j];
		tangent[3*(size*(size+1)+j)+1]=tangent[3*j+1];
		tangent[3*(size*(size+1)+j)+2]=tangent[3*j+2];
	}
	//normalizacja
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			VNormalize3(&tangent[3*(i*(size+1)+j)]);
		}
	}
}

void STHeightMap::update_vertices(int time) {
 	int i,j;
	int i2,j2;
	float s,t;
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			s=(1.0f/size*i);
			t=(1.0f/size*j);
			i2=s*hmap->width;
			j2=t*hmap->width;
			if(i2==128) i2=127;
			if(j2==128) j2=127;
			vert_coords[3*(i*(size+1)+j)]=t*xsiz;
			vert_coords[3*(i*(size+1)+j)+1]=
				hmap->pixel_buf
				[4*(i2*hmap->width+j2)]
				/255.0f*strength;
			vert_coords[3*(i*(size+1)+j)+2]=s*ysiz;
		}
	}
}