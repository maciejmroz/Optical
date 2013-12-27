
#include "experimental_effects.h"
#include "effectbase.h"

ExpSineEffectDistort::ExpSineEffectDistort() {
	char* tmp;
	tmp=new char[512*512*4];
	render_target=new Texture(512,512,tmp);
	delete tmp;


	size=50;
	vert_coords=new float[3*(size+1)*(size+1)];
	tex_coords=new float[2*(size+1)*(size+1)];
	indices=new unsigned short[2*size*(size+2)];
	int i,j;
	for(i=0;i<size;i++) { //num strips
		indices[2*i*(size+2)]=i*(size+1);
		indices[2*i*(size+2)+1]=(i+1)*(size+1);
		for(j=0;j<size;j++) {
			indices[2*i*(size+2)+2+2*j]=i*(size+1)+1+j;
			indices[2*i*(size+2)+2+2*j+1]=(i+1)*(size+1)+1+j;
		}
		indices[2*i*(size+2)+2*(size+1)]=(i+1)*(size+1)+size;
		indices[2*i*(size+2)+2*(size+1)+1]=(i+1)*(size+1);
	}
	for(i=0;i<(size+1);i++) {
		for(j=0;j<(size+1);j++) {
			vert_coords[3*(i*(size+1)+j)]=1.0f/size*i;
			vert_coords[3*(i*(size+1)+j)+1]=1.0f/size*j;
			vert_coords[3*(i*(size+1)+j)+2]=-0.2f;
		}
	}
	for(i=0;i<(size+1)*(size+1);i++) {
		tex_coords[2*i]=vert_coords[3*i];
		if(height==480) {
			tex_coords[2*i+1]=0.935f*(1.0f-vert_coords[3*i+1]);
		}
		else {
			tex_coords[2*i+1]=1.0f-vert_coords[3*i+1];
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
		agp_tex_coords=(float*)agp_alloc->get_agp_block(2*(size+1)*(size+1)*sizeof(float));
		if(!agp_tex_coords) {
			is_optimized=0;
			return;
		}
		memcpy((void*)agp_tex_coords,(void*)tex_coords,2*(size+1)*(size+1)*sizeof(float));
	}
	ef=new ESceneRenderer("scenes/superknot.lws",NULL,5.0f);
}

ExpSineEffectDistort::~ExpSineEffectDistort() {
	delete render_target;
	delete ef;
}

void ExpSineEffectDistort::render(int time) {
	//update render target
	if(height==480) {
		glViewport(0,0,512,480);
	}
	else {
		glViewport(0,0,512,512);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	ef->render(time);
	render_target->backbuffer_copy(512);
	glViewport(0,0,width,height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	int i,j;
	//update texcoords
	for(i=0;i<(size+1)*(size+1);i++) {
		vert_coords[3*i+2]=-0.2f-0.01f;
	}
	//distort texcoords
	for(i=0;i<(size+1)*(size+1);i++) {
		vert_coords[3*i+2]+=0.05f*(FastCos(vert_coords[3*i+1]*6.0f+0.001f*time)*
			FastCos(vert_coords[3*i]*5.28f+0.0012f*time)*
			FastCos(vert_coords[3*i+1]*9.21f+0.0016f*time));
	}
	if(is_optimized) {
		memcpy((void*)agp_vert_coords,(void*)vert_coords,3*(size+1)*(size+1)*sizeof(float));
	}
	c_state->want_disabled(GL_LIGHTING);
	c_state->want_disabled(GL_FOG);
	c_state->want_disabled(GL_DEPTH_TEST);
	c_state->want_disabled(GL_ALPHA_TEST);
	c_state->want_disabled(GL_BLEND);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	float x_size=0.266f;
	float y_size=0.2f;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glFrustum(-0.133,0.133,-0.1,0.1,0.1,2.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glScalef(0.8f,0.6f,1.0f);
	glTranslatef(-0.5f,-0.5f,0.0f);

	for(i=0;i<max_texture_units;i++) {
		c_state->textures[i].clear_tex_gen_flags();
		c_state->textures[i].enabled=0;
	}
	c_state->textures[0].enabled=1;
	c_state->textures[0].tex=render_target;
	c_state->textures[0].client_state_enabled=1;
	c_state->textures[0].set_texture_env(GL_MODULATE);
	if(is_optimized) {
		c_state->textures[0].coord_pointer=agp_tex_coords;
		glVertexPointer(3,GL_FLOAT,0,agp_vert_coords);
	}
	else {
		c_state->textures[0].coord_pointer=tex_coords;
		glVertexPointer(3,GL_FLOAT,0,vert_coords);
	}
	c_state->prerender_setup();
	if((!is_optimized) && hasVertexArrayRange) {
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	glDisableClientState(GL_NORMAL_ARRAY);

	glDrawElements(GL_TRIANGLE_STRIP,2*(size+2)*size,GL_UNSIGNED_SHORT,indices);

	glEnableClientState(GL_NORMAL_ARRAY);
	if((!is_optimized) && hasVertexArrayRange) {
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	c_state->postrender_setup();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

}