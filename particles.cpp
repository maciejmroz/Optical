/****************************************************************************************
 Particle system class implementation
 (c) 2000 Maciej Mroz
****************************************************************************************/

#include "particles.h"
#include "debug_msg.h"

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float

ParticleSystem::ParticleSystem(Motion mot,Texture *p_tex) {
	emitter_mot=mot;
	tex=p_tex;
	appear_time=0;
	disappear_time=0;
	life_time=0;
	emission_intensity=0;
	gravity[0]=0.0f;
	gravity[1]=0.0f;
	gravity[2]=0.0f;
	color[0]=1.0f;
	color[1]=1.0f;
	color[2]=1.0f;
	life_time=0;
	max_size=0;
	min_size=0;
	max_velocity=0;
	min_velocity=0;
	positions=NULL;
	velocities=NULL;
	sizes=NULL;
	colors=NULL;
	ages=NULL;
	spread=0;
	num_particles=0;
	last_emission=0.0f;
	last_update=0.0f;
	last_rand=0.123f;
	vertex_buffer=NULL;
	color_buffer=NULL;
	texcoord_buffer=NULL;
	current_size=0;
}

ParticleSystem::~ParticleSystem() {
	if(positions) delete positions;
	if(velocities) delete velocities;
	if(sizes) delete sizes;
	if(colors) delete colors;
	if(ages) delete ages;
	if(vertex_buffer) delete vertex_buffer;
	if(color_buffer) delete color_buffer;
	if(texcoord_buffer) delete texcoord_buffer;
}

void ParticleSystem::alloc_mem(int size) {
	if(positions) delete positions;
	if(velocities) delete velocities;
	if(sizes) delete sizes;
	if(colors) delete colors;
	if(ages) delete ages;
	if(vertex_buffer) delete vertex_buffer;
	if(color_buffer) delete color_buffer;
	if(texcoord_buffer) delete texcoord_buffer;
	num_particles=0;
	positions = new float[3*size];
	velocities = new float[3*size];
	sizes = new float[size];
	ages=new float[size];
	colors=new float[4*size];
	vertex_buffer=new float[4*3*size];
	color_buffer=new float[4*4*size];
	texcoord_buffer=new float[4*2*size];
	int i;
	for(i=0;i<size;i++) {
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

void ParticleSystem::set_emission(float emission) {
	alloc_mem((int)(emission*life_time)+1);
	emission_intensity=emission;
}

void ParticleSystem::set_gravity(float gx,float gy,float gz) {
	gravity[0]=gx;
	gravity[1]=gy;
	gravity[2]=gz;
}

void ParticleSystem::set_color(float r,float g,float b) {
	color[0]=r;
	color[1]=g;
	color[2]=b;
}

void ParticleSystem::set_sizes(Envelope min,Envelope max) {
	min_size=min;
	max_size=max;
}

void ParticleSystem::set_spread(Envelope spd) {
	spread=spd;
}

void ParticleSystem::set_times(float appear,float disappear,float life) {
	alloc_mem((int)(emission_intensity*life)+1);
	appear_time=appear;
	disappear_time=disappear;
	life_time=life;
}

void ParticleSystem::set_velocities(Envelope min,Envelope max) {
	min_velocity=min;
	max_velocity=max;
}

void ParticleSystem::calc_motion(float frame) {
	emitter_mot.calc(frame);
	e_x=emitter_mot.cv[0];
	e_y=emitter_mot.cv[1];
	e_z=emitter_mot.cv[2];
}

void ParticleSystem::update(float frame) {
	int max_particles=(int)(emission_intensity*life_time);
	float emission_interval=1.0f/emission_intensity;
	float time_in_seconds=frame*0.033333333f;
	calc_motion(frame);
	min_velocity.calc(frame);
	max_velocity.calc(frame);
	spread.calc(frame);
	min_size.calc(frame);
	max_size.calc(frame);
	int old_num_particles=num_particles;
	int i;
	if((time_in_seconds-last_emission)>emission_interval) {
		float tmp;
		for(tmp=last_emission+emission_interval;tmp<time_in_seconds;tmp+=emission_interval) {
			//emit enough particles
			if(num_particles<max_particles) {
				//velocity
				last_rand=next_float(last_rand);
				velocities[3*num_particles]=min_velocity.res
					+last_rand*(max_velocity.res-min_velocity.res);
				last_rand=next_float(last_rand);
				velocities[3*num_particles+1]=min_velocity.res
					+last_rand*(max_velocity.res-min_velocity.res);
				last_rand=next_float(last_rand);
				velocities[3*num_particles+2]=min_velocity.res
					+last_rand*(max_velocity.res-min_velocity.res);
				//spread
				last_rand=next_float(last_rand);
				velocities[3*num_particles]+=2.0f*(last_rand-0.5f)*(spread.res);
				last_rand=next_float(last_rand);
				velocities[3*num_particles+1]+=2.0f*(last_rand-0.5f)*(spread.res);
				last_rand=next_float(last_rand);
				velocities[3*num_particles+2]+=2.0f*(last_rand-0.5f)*(spread.res);
				//size
				last_rand=next_float(last_rand);
				sizes[num_particles]=min_size.res+last_rand*(max_size.res-min_size.res);
				//age
				ages[num_particles]=time_in_seconds-tmp;
				//position
				positions[3*num_particles]=e_x+
					ages[num_particles]*velocities[3*num_particles];
				positions[3*num_particles+1]=e_y+
					ages[num_particles]*velocities[3*num_particles+1];
				positions[3*num_particles+2]=e_z+
					ages[num_particles]*velocities[3*num_particles+2];
				//color
				colors[4*num_particles]=color[0];
				colors[4*num_particles+1]=color[1];
				colors[4*num_particles+2]=color[2];
				if(ages[num_particles]<appear_time) {
					colors[4*num_particles+3]=ages[num_particles]/appear_time;
				}
				else if(ages[num_particles]<(life_time-disappear_time)) {
					colors[4*num_particles+3]=1.0f;
				}
				else if(ages[num_particles]<life_time) {
					colors[4*num_particles+3]=(life_time-ages[num_particles])/disappear_time;
				}
				else {
					colors[4*num_particles+3]=0.0f;
				}
				last_emission=tmp;
				num_particles++;
			}
		}
	}
	//update existing particles
	float update_time;
	update_time=time_in_seconds-last_update;
	last_update=time_in_seconds;
	for(i=0;i<old_num_particles;i++) {
		positions[3*i]+=velocities[3*i]*update_time;
		positions[3*i+1]+=velocities[3*i+1]*update_time;
		positions[3*i+2]+=velocities[3*i+2]*update_time;
		velocities[3*i]+=gravity[0]*update_time;
		velocities[3*i+1]+=gravity[1]*update_time;
		velocities[3*i+2]+=gravity[2]*update_time;
		ages[i]+=update_time;
		if(ages[i]>life_time) { //kill the particle and emit new one
			//velocity
			last_rand=next_float(last_rand);
			velocities[3*i]=min_velocity.res
				+last_rand*(max_velocity.res-min_velocity.res);
			last_rand=next_float(last_rand);
			velocities[3*i+1]=min_velocity.res
				+last_rand*(max_velocity.res-min_velocity.res);
			last_rand=next_float(last_rand);
			velocities[3*i+2]=min_velocity.res
				+last_rand*(max_velocity.res-min_velocity.res);
			//spread
			last_rand=next_float(last_rand);
			velocities[3*i]+=2.0f*(last_rand-0.5f)*(spread.res);
			last_rand=next_float(last_rand);
			velocities[3*i+1]+=2.0f*(last_rand-0.5f)*(spread.res);
			last_rand=next_float(last_rand);
			velocities[3*i+2]+=2.0f*(last_rand-0.5f)*(spread.res);
			//size
			last_rand=next_float(last_rand);
			sizes[i]=min_size.res+last_rand*(max_size.res-min_size.res);
			//age
			ages[i]=ages[i]-floor(ages[i]/life_time)*life_time;
			//position
			positions[3*i]=e_x+ages[i]*velocities[3*i];
			positions[3*i+1]=e_y+ages[i]*velocities[3*i+1];
			positions[3*i+2]=e_z+ages[i]*velocities[3*i+2];
			//color
			colors[4*i]=color[0];
			colors[4*i+1]=color[1];
			colors[4*i+2]=color[2];
		}
		if(ages[i]<appear_time) {
			colors[4*i+3]=ages[i]/appear_time;
		}
		else if(ages[i]<(life_time-disappear_time)) {
			colors[4*i+3]=1.0f;
		}
		else if(ages[i]<life_time) {
			colors[4*i+3]=(life_time-ages[i])/disappear_time;
		}
		else {
			colors[4*i+3]=0.0f;
		}
	}
	//fill the buffers
	if(!current_scene) return;
	Vector3 view_dir,cam_up,view_up,view_side;
	get_scene_camera()->get_direction(view_dir);
	get_scene_camera()->get_up_vector(cam_up);
	VCrossProduct3(cam_up,view_dir,view_side);
	VCrossProduct3(view_dir,view_side,view_up);
	VNormalize3(view_side);
	VNormalize3(view_up);
	Vector3 sx,sy;
	for(i=0;i<num_particles;i++) {
		sx[0]=view_side[0];
		sx[1]=view_side[1];
		sx[2]=view_side[2];

		sy[0]=view_up[0];
		sy[1]=view_up[1];
		sy[2]=view_up[2];

		VSMultiply3(sx,sizes[i]);
		VSMultiply3(sy,sizes[i]);

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

		color_buffer[4*4*i]=colors[4*i];
		color_buffer[4*4*i+1]=colors[4*i+1];
		color_buffer[4*4*i+2]=colors[4*i+2];
		color_buffer[4*4*i+3]=colors[4*i+3];

		color_buffer[4*4*i+4]=colors[4*i];
		color_buffer[4*4*i+5]=colors[4*i+1];
		color_buffer[4*4*i+6]=colors[4*i+2];
		color_buffer[4*4*i+7]=colors[4*i+3];

		color_buffer[4*4*i+8]=colors[4*i];
		color_buffer[4*4*i+9]=colors[4*i+1];
		color_buffer[4*4*i+10]=colors[4*i+2];
		color_buffer[4*4*i+11]=colors[4*i+3];

		color_buffer[4*4*i+12]=colors[4*i];
		color_buffer[4*4*i+13]=colors[4*i+1];
		color_buffer[4*4*i+14]=colors[4*i+2];
		color_buffer[4*4*i+15]=colors[4*i+3];
	}
}

void ParticleSystem::render() {
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
//	c_state->want_disabled(GL_DEPTH_TEST);
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
}