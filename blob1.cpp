
#include "engine.h"
#include "blob1.h"

EBlob1::EBlob1() {
	blob=new ScField(-1.2f,1.2f,-1.2f,1.2f,-1.2f,1.2f,24);
	blob->add_blob("blob1.mot",BT_CUBE);
	blob->add_blob("blob2.mot",BT_TORUS);
	blob->add_blob("blob3.mot");
	blob->add_blob("blob4.mot",BT_TORUS);
	Envelope env;
	env=25.0f;
	blob->set_isovalue(&env);
	tex=new Texture("textures/goldrock.jpg");
	tex2=new Texture("textures/addict.jpg");
}

EBlob1::~EBlob1() {
	delete blob;
	delete tex;
	delete tex2;
}

void EBlob1::render(int time) {
	blob->update(30.0f*(float)time*0.001f);
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
	c_state->prerender_setup();
	int i;
	c_state->want_disabled(GL_DEPTH_TEST);
	c_state->want_enabled(GL_BLEND);
//	c_state->want_disabled(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	for(i=0;i<6;i++) {
		glPushMatrix();
		glRotatef(0.03f*(time+40*i),0.0f,1.0f,0.0f);
		glRotatef(0.1f*(time+40*i),1.0f,0.0f,0.0f);
		glRotatef(0.03f*(time+40*i),0.0f,0.0f,1.0f);
		blob->render();
		glPopMatrix();
	}
	c_state->postrender_setup();
	DrawQuad(tex2,0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,1.0f,1,GL_ONE_MINUS_DST_ALPHA,GL_DST_ALPHA);
}