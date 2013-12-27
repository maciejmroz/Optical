

#include "lh_blobs.h"

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float

LH_Blob1::LH_Blob1(float mini_x,float maxi_x,float mini_y,float maxi_y,
				   float mini_z,float maxi_z,int res,Texture *tx) : ScField(mini_x,
				   maxi_x,mini_y,maxi_y,mini_z,maxi_z,res) {
	quad_vertex_buffer=NULL;;
	num_quads=0;
	quad_color_buffer=0;
	quad_table_size=0; 
	tex=tx;
}

LH_Blob1::~LH_Blob1() {
	if(quad_vertex_buffer) delete quad_vertex_buffer;
	if(quad_color_buffer) delete quad_color_buffer;
}

void LH_Blob1::update(float frame) {
	ScField::update(frame);
	if(quad_table_size<3*num_triangles) {
		if(quad_vertex_buffer) delete quad_vertex_buffer;
		if(quad_color_buffer) delete quad_color_buffer;
		quad_vertex_buffer=new float[3*num_triangles*4*3];
		quad_color_buffer=new float[3*num_triangles*4*4];
		quad_table_size=3*num_triangles;
	}
	num_quads=0;
	unsigned int i;
	for(i=0;i<num_triangles;i++) {  //build 3 quads
		if(indices[3*i]<indices[3*i+1]) {
			quad_vertex_buffer[num_quads*4*3]=vertex_buffer[3*indices[3*i]];
			quad_vertex_buffer[num_quads*4*3+1]=vertex_buffer[3*indices[3*i]+1];
			quad_vertex_buffer[num_quads*4*3+2]=vertex_buffer[3*indices[3*i]+2];
			
			quad_vertex_buffer[num_quads*4*3+3]=vertex_buffer[3*indices[3*i+1]];
			quad_vertex_buffer[num_quads*4*3+4]=vertex_buffer[3*indices[3*i+1]+1];
			quad_vertex_buffer[num_quads*4*3+5]=vertex_buffer[3*indices[3*i+1]+2];
			
			quad_vertex_buffer[num_quads*4*3+6]=vertex_buffer[3*indices[3*i+1]]+
				0.4*normal_buffer[3*indices[3*i+1]];
			quad_vertex_buffer[num_quads*4*3+7]=vertex_buffer[3*indices[3*i+1]+1]+
				0.4*normal_buffer[3*indices[3*i+1]+1];
			quad_vertex_buffer[num_quads*4*3+8]=vertex_buffer[3*indices[3*i+1]+2]+
				0.4*normal_buffer[3*indices[3*i+1]+2];
			
			quad_vertex_buffer[num_quads*4*3+9]=vertex_buffer[3*indices[3*i]]+
				0.4*normal_buffer[3*indices[3*i]];
			quad_vertex_buffer[num_quads*4*3+10]=vertex_buffer[3*indices[3*i]+1]+
				0.4*normal_buffer[3*indices[3*i]+1];
			quad_vertex_buffer[num_quads*4*3+11]=vertex_buffer[3*indices[3*i]+2]+
				0.4*normal_buffer[3*indices[3*i]+2];
			num_quads++;
		}

		//quad 2
		if(indices[3*i+1]<indices[3*i+2]) {
			quad_vertex_buffer[num_quads*4*3]=vertex_buffer[3*indices[3*i+1]];
			quad_vertex_buffer[num_quads*4*3+1]=vertex_buffer[3*indices[3*i+1]+1];
			quad_vertex_buffer[num_quads*4*3+2]=vertex_buffer[3*indices[3*i+1]+2];
			
			quad_vertex_buffer[num_quads*4*3+3]=vertex_buffer[3*indices[3*i+2]];
			quad_vertex_buffer[num_quads*4*3+4]=vertex_buffer[3*indices[3*i+2]+1];
			quad_vertex_buffer[num_quads*4*3+5]=vertex_buffer[3*indices[3*i+2]+2];
			
			quad_vertex_buffer[num_quads*4*3+6]=vertex_buffer[3*indices[3*i+2]]+
				0.4*normal_buffer[3*indices[3*i+2]];
			quad_vertex_buffer[num_quads*4*3+7]=vertex_buffer[3*indices[3*i+2]+1]+
				0.4*normal_buffer[3*indices[3*i+2]+1];
			quad_vertex_buffer[num_quads*4*3+8]=vertex_buffer[3*indices[3*i+2]+2]+
				0.4*normal_buffer[3*indices[3*i+2]+2];
			
			quad_vertex_buffer[num_quads*4*3+9]=vertex_buffer[3*indices[3*i+1]]+
				0.4*normal_buffer[3*indices[3*i+1]];
			quad_vertex_buffer[num_quads*4*3+10]=vertex_buffer[3*indices[3*i+1]+1]+
				0.4*normal_buffer[3*indices[3*i+1]+1];
			quad_vertex_buffer[num_quads*4*3+11]=vertex_buffer[3*indices[3*i+1]+2]+
				0.4*normal_buffer[3*indices[3*i+1]+2];
			num_quads++;
		}

		//quad 3
		if(indices[3*i+2]<indices[3*i]) {
			quad_vertex_buffer[num_quads*4*3]=vertex_buffer[3*indices[3*i]];
			quad_vertex_buffer[num_quads*4*3+1]=vertex_buffer[3*indices[3*i]+1];
			quad_vertex_buffer[num_quads*4*3+2]=vertex_buffer[3*indices[3*i]+2];
			
			quad_vertex_buffer[num_quads*4*3+3]=vertex_buffer[3*indices[3*i+2]];
			quad_vertex_buffer[num_quads*4*3+4]=vertex_buffer[3*indices[3*i+2]+1];
			quad_vertex_buffer[num_quads*4*3+5]=vertex_buffer[3*indices[3*i+2]+2];
			
			quad_vertex_buffer[num_quads*4*3+6]=vertex_buffer[3*indices[3*i+2]]+
				0.4*normal_buffer[3*indices[3*i+2]];
			quad_vertex_buffer[num_quads*4*3+7]=vertex_buffer[3*indices[3*i+2]+1]+
				0.4*normal_buffer[3*indices[3*i+2]+1];
			quad_vertex_buffer[num_quads*4*3+8]=vertex_buffer[3*indices[3*i+2]+2]+
				0.4*normal_buffer[3*indices[3*i+2]+2];
			
			quad_vertex_buffer[num_quads*4*3+9]=vertex_buffer[3*indices[3*i]]+
				0.4*normal_buffer[3*indices[3*i]];
			quad_vertex_buffer[num_quads*4*3+10]=vertex_buffer[3*indices[3*i]+1]+
				0.4*normal_buffer[3*indices[3*i]+1];
			quad_vertex_buffer[num_quads*4*3+11]=vertex_buffer[3*indices[3*i]+2]+
				0.4*normal_buffer[3*indices[3*i]+2];
			num_quads++;
		}
	}
	for(i=0;i<num_quads;i++) {              //num_quads replaced 3*num_triangles
		quad_color_buffer[4*4*i]=1.0f;
		quad_color_buffer[4*4*i+1]=1.0f;
		quad_color_buffer[4*4*i+2]=1.0f;
		quad_color_buffer[4*4*i+3]=0.2f;
		quad_color_buffer[4*4*i+4]=1.0f;
		quad_color_buffer[4*4*i+5]=1.0f;
		quad_color_buffer[4*4*i+6]=1.0f;
		quad_color_buffer[4*4*i+7]=0.2f;
		quad_color_buffer[4*4*i+8]=1.0f;
		quad_color_buffer[4*4*i+9]=1.0f;
		quad_color_buffer[4*4*i+10]=1.0f;
		quad_color_buffer[4*4*i+11]=0.0f;
		quad_color_buffer[4*4*i+12]=1.0f;
		quad_color_buffer[4*4*i+13]=1.0f;
		quad_color_buffer[4*4*i+14]=1.0f;
		quad_color_buffer[4*4*i+15]=0.0f;
	}
}

void LH_Blob1::render() {
	unsigned int i;
	for(i=0;i<max_texture_units;i++) {
		c_state->textures[i].enabled=0;
		c_state->textures[i].clear_tex_gen_flags();
		c_state->textures[i].client_state_enabled=0;
	}
	c_state->textures[0].enabled=1;
	c_state->textures[0].tex_gen_s_enabled=1;
	c_state->textures[0].tex_gen_s_mode=GL_SPHERE_MAP;
	c_state->textures[0].tex_gen_t_enabled=1;
	c_state->textures[0].tex_gen_t_mode=GL_SPHERE_MAP;
	c_state->textures[0].tex=tex;
	c_state->textures[0].set_texture_env(GL_MODULATE);
	c_state->textures[0].client_state_enabled=0;
	float col[4]={1.0f,1.0f,1.0f,0.3f};
	float zero_col[4]={0.0f,0.0f,0.0f,0.0f};
	glMaterialfv(GL_FRONT,GL_DIFFUSE,col);
	glMaterialfv(GL_FRONT,GL_AMBIENT,zero_col);
	glMaterialfv(GL_FRONT,GL_SPECULAR,zero_col);
	c_state->want_enabled(GL_LIGHTING);
	c_state->want_enabled(GL_DEPTH_TEST);
	c_state->want_disabled(GL_BLEND);
	c_state->want_enabled(GL_CULL_FACE);
	c_state->prerender_setup();
	ScField::render();
	c_state->postrender_setup();
	//render "rays"
	glDepthMask(GL_FALSE);
	c_state->want_disabled(GL_LIGHTING);
	c_state->want_enabled(GL_BLEND);
	c_state->want_disabled(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
//	glNormalPointer(GL_FLOAT,0,normal_buffer);
	c_state->textures[0].enabled=0;
	c_state->textures[0].clear_tex_gen_flags();
	c_state->textures[0].client_state_enabled=0;
	c_state->prerender_setup();
	glDisableClientState(GL_NORMAL_ARRAY);
	glColorPointer(4,GL_FLOAT,0,quad_color_buffer);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,quad_vertex_buffer);
	glDrawArrays(GL_QUADS,0,4*num_quads);
	glEnableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDepthMask(GL_TRUE);
	c_state->postrender_setup();
}

LH_BlobEffect1::LH_BlobEffect1() {
	tex=new Texture("textures/goldrock.jpg");
	tex2=new Texture("textures/addict.jpg");
	blob=new LH_Blob1(-1.2f,1.2f,-1.2f,1.2f,-1.2f,1.2f,16,tex);
	blob->add_blob("blob1.mot");
	blob->add_blob("blob2.mot");
	blob->add_blob("blob3.mot");
	blob->add_blob("blob4.mot");
	Envelope env;
	env=25.0f;
	blob->set_isovalue(&env);
}

LH_BlobEffect1::~LH_BlobEffect1() {
	delete blob;
	delete tex;
	delete tex2;
}

void LH_BlobEffect1::render(int time) {
	DrawQuad(tex2);
	blob->update(30.0f*(float)time*0.001f);
	glPushMatrix();
	glRotatef(0.03f*time,0.0f,1.0f,0.0f);
	glRotatef(0.1f*time,1.0f,0.0f,0.0f);
	glRotatef(0.03f*time,0.0f,0.0f,1.0f);
	blob->render();
	glPopMatrix();
}