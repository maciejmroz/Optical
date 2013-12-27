/**************************************************************************************
 My volume fog implementation
***************************************************************************************/

#include "lh_volfog.h"

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float

VolumeParticles::VolumeParticles(Texture *p_tex,int numparticles,float p_size,
		float min_x,float max_x,
		float min_y,float max_y,
		float min_z,float max_z) {
	float last_rand=0.12f;
	size=p_size;
	tex=p_tex;
	num_particles=numparticles;
	positions=new float[3*num_particles];
	texcoord_buffer=new float[4*2*num_particles];
	vertex_buffer=new float[4*3*num_particles];
	color_buffer=new float[4*4*num_particles];
	int i;
	for(i=0;i<num_particles;i++) {

/*		last_rand=next_float(last_rand);
		positions[3*i]=min_x+(max_x-min_x)*last_rand;
		last_rand=next_float(last_rand);
		positions[3*i+1]=min_y+(max_y-min_y)*last_rand;
		last_rand=next_float(last_rand);
		positions[3*i+2]=min_z+(max_z-min_z)*last_rand;*/

		positions[3*i]=min_x+(max_x-min_x)*(i/(int)sqrt(num_particles))/sqrt(num_particles);
		last_rand=next_float(last_rand);
		positions[3*i+1]=min_y+(max_y-min_y)*last_rand;
		positions[3*i+2]=min_z+(max_z-min_z)*(i%(int)sqrt(num_particles))/sqrt(num_particles);

		last_rand=next_float(last_rand);
		positions[3*i]+=0.4f*((max_x-min_x)/(int)(sqrt(num_particles)))*(last_rand-0.5f);
		last_rand=next_float(last_rand);
		positions[3*i+2]+=0.4f*((max_z-min_z)/(int)(sqrt(num_particles)))*(last_rand-0.5f);

		last_rand=next_float(last_rand);
		color_buffer[4*4*i]=0.2f+0.2f*last_rand;
		color_buffer[4*4*i+1]=0.2f+0.2f*last_rand;
		color_buffer[4*4*i+2]=0.2f+0.2f*last_rand;
		color_buffer[4*4*i+3]=1.0f;

		color_buffer[4*4*i+4]=0.2f+0.2f*last_rand;
		color_buffer[4*4*i+5]=0.2f+0.2f*last_rand;
		color_buffer[4*4*i+6]=0.2f+0.2f*last_rand;
		color_buffer[4*4*i+7]=1.0f;

		color_buffer[4*4*i+8]=0.2f+0.2f*last_rand;
		color_buffer[4*4*i+9]=0.2f+0.2f*last_rand;
		color_buffer[4*4*i+10]=0.2f+0.2f*last_rand;
		color_buffer[4*4*i+11]=1.0f;

		color_buffer[4*4*i+12]=0.2f+0.2f*last_rand;
		color_buffer[4*4*i+13]=0.2f+0.2f*last_rand;
		color_buffer[4*4*i+14]=0.2f+0.2f*last_rand;
		color_buffer[4*4*i+15]=1.0f;

		texcoord_buffer[2*4*i]=0.0f;
		texcoord_buffer[2*4*i+1]=0.0f;

		texcoord_buffer[2*4*i+2]=1.0f;
		texcoord_buffer[2*4*i+3]=0.0f;
	
		texcoord_buffer[2*4*i+4]=1.0f;
		texcoord_buffer[2*4*i+5]=1.0f;

		texcoord_buffer[2*4*i+6]=0.0f;
		texcoord_buffer[2*4*i+7]=1.0f;

	}
}

VolumeParticles::~VolumeParticles() {
	if(positions) delete positions;
	if(texcoord_buffer) delete texcoord_buffer;
	if(vertex_buffer) delete vertex_buffer;
	if(color_buffer) delete color_buffer;
}

void VolumeParticles::update(float frame) {
	Vector3 view_dir,cam_up,view_up,view_side;
	get_scene_camera()->get_direction(view_dir);
	get_scene_camera()->get_up_vector(cam_up);
	VCrossProduct3(cam_up,view_dir,view_side);
	VCrossProduct3(view_dir,view_side,view_up);
	VNormalize3(view_side);
	VNormalize3(view_up);
	Vector3 sx,sy;
	int i;
	for(i=0;i<num_particles;i++) {
		sx[0]=view_side[0];
		sx[1]=view_side[1];
		sx[2]=view_side[2];

		sy[0]=view_up[0];
		sy[1]=view_up[1];
		sy[2]=view_up[2];

		VSMultiply3(sx,size);
		VSMultiply3(sy,size);

		vertex_buffer[4*3*i]=positions[3*i]-sx[0]+sy[0];
		vertex_buffer[4*3*i+1]=positions[3*i+1]-sx[1]+sy[1];
		vertex_buffer[4*3*i+2]=positions[3*i+2]-sx[2]+sy[2];

		vertex_buffer[4*3*i+3]=positions[3*i]+sx[0]+sy[0];
		vertex_buffer[4*3*i+4]=positions[3*i+1]+sx[1]+sy[1];
		vertex_buffer[4*3*i+5]=positions[3*i+2]+sx[2]+sy[2];

		vertex_buffer[4*3*i+6]=positions[3*i]+sx[0]-sy[0];
		vertex_buffer[4*3*i+7]=positions[3*i+1]+sx[1]-sy[1];
		vertex_buffer[4*3*i+8]=positions[3*i+2]+sx[2]-sy[2];

		vertex_buffer[4*3*i+9]=positions[3*i]-sx[0]-sy[0];
		vertex_buffer[4*3*i+10]=positions[3*i+1]-sx[1]-sy[1];
		vertex_buffer[4*3*i+11]=positions[3*i+2]-sx[2]-sy[2];

	}
}

void VolumeParticles::render() {
	unsigned int i;
	for(i=0;i<max_texture_units;i++) {
		c_state->textures[i].clear_tex_gen_flags();
		c_state->textures[i].enabled=0;
		c_state->textures[i].client_state_enabled=0;
	}
	c_state->textures[0].enabled=1;
	c_state->textures[0].client_state_enabled=1;
	c_state->textures[0].tex=tex;
	c_state->textures[0].coord_pointer=texcoord_buffer;
	c_state->textures[0].set_texture_env(GL_MODULATE);
	c_state->prerender_setup();
	glDepthMask(GL_FALSE);
	c_state->want_enabled(GL_BLEND);
	c_state->want_disabled(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glColorPointer(4,GL_FLOAT,0,color_buffer);
	glVertexPointer(3,GL_FLOAT,0,vertex_buffer);
	glEnableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glDrawArrays(GL_QUADS,0,4*num_particles);

	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glDepthMask(GL_TRUE);
	c_state->postrender_setup();
};

LH_VolumeFogEffect::LH_VolumeFogEffect() {
	scn=new Scene3D("scenes/volfog.lws");
	
	p_system1=new 
		VolumeParticles(tex_manager.get_texture("textures/fog.jpg"),50,0.7f,
		-2.5f,2.5f,
		0.6f,1.2f,
		-2.5f,2.5f);
	p_system1->set_current_scene(scn);
}

LH_VolumeFogEffect::~LH_VolumeFogEffect() {
	delete scn;
	delete p_system1;
}

void LH_VolumeFogEffect::render(int time) {
	scn->render(time);
	p_system1->update((float)time);           //does not matter anyway
	p_system1->render();
}
