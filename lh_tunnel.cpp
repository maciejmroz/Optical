/*********************************************************************************************
 Life Harmony tunnel effect implementation :)
*********************************************************************************************/

#include "engine.h"
#include "lh_tunnel.h"
#include "debug_msg.h"

extern HDC hDC;
extern HDC pbufferDC;
extern HGLRC hRC;
extern HPBUFFERARB PBuffer_handle;
extern HGLRC pbufferRC;

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float

/*Tunnel::Tunnel(Texture *tx,float rad,float len,
			   float wav_len,float wav_period,int radial_seg,int len_seg,
			   int tx_radial_tiles,int tx_len_tiles) {
	tex=tx;
	radius=rad;
	length=len;
	wave_length=wav_len;
	wave_period=wav_period;
	radial_segments=radial_seg;
	length_segments=len_seg;
	vertex_buffer=new float[(radial_segments+1)*length_segments*3];
	texcoord_buffer=new float[(radial_segments+1)*length_segments*2];
	normal_buffer=new float[(radial_segments+1)*length_segments*3];
	radial_tiles=tx_radial_tiles;
	length_tiles=tx_len_tiles;
	int i,j;
	//zero everything
	for(i=0;i<((radial_segments+1)*length_segments);i++) {
		vertex_buffer[3*i]=vertex_buffer[3*i+1]=vertex_buffer[3*i+2]=0.0f;
		texcoord_buffer[2*i]=texcoord_buffer[2*i+1]=0.0f;
		normal_buffer[3*i]=normal_buffer[3*i+1]=normal_buffer[3*i+2]=0.0f;
	}
	//fill index buffer - it is always the same
	int vert_num;
	vert_num=0;
	index_buffer=new unsigned int[2*length_segments*radial_segments];
	for(i=0;i<radial_segments;i++) {
		index_buffer[vert_num++]=(i+1)*length_segments;
		for(j=0;j<(length_segments-1);j++) {
			index_buffer[vert_num++]=i*length_segments+j+1;
			index_buffer[vert_num++]=(i+1)*length_segments+j;
		}
		index_buffer[vert_num++]=i*length_segments+length_segments-1;
	}
}

Tunnel::~Tunnel() {
	delete vertex_buffer;
	delete texcoord_buffer;
	delete normal_buffer;
}

#define PI 3.14159265f
#define TWOPI 6.28318530f
#define HALFPI 1.57079632f

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

void Tunnel::update(float frame) {
	int i,j;
	Point3 cam_pos;
	Vector3 cam_dir;
	if(!current_scene) return;
	float z;
	float time;
	time=frame*0.033333333f;
	get_scene_camera()->get_position(cam_pos);
	get_scene_camera()->get_direction(cam_dir);
	for(i=0;i<radial_segments;i++) {
		z=cam_pos[2]+length*0.5f;
		for(j=0;j<length_segments;j++) {
			vertex_buffer[3*(i*length_segments+j)]=sin(6.28/radial_segments*i)*
				radius;
			vertex_buffer[3*(i*length_segments+j)+1]=cos(6.28/radial_segments*i)*
				radius;
			vertex_buffer[3*(i*length_segments+j)+2]=z;
			z-=length/length_segments;
			texcoord_buffer[2*(i*length_segments+j)+1] = -z / (length/length_tiles); + .5;
			texcoord_buffer[2*(i*length_segments+j)] = 
				(float)radial_tiles/(float)radial_segments*(float)i;
			normal_buffer[3*(i*length_segments+j)]=-sin(6.28/radial_segments*i);
			normal_buffer[3*(i*length_segments+j)+1]=-cos(6.28/radial_segments*i);
			normal_buffer[3*(i*length_segments+j)+2]=0.0f;
		
		}
	}
	i=radial_segments;
	z=cam_pos[2]+length*0.5f;
	for(j=0;j<length_segments;j++) {
		vertex_buffer[3*(i*length_segments+j)]=0.0f;
		vertex_buffer[3*(i*length_segments+j)+1]=radius;
		vertex_buffer[3*(i*length_segments+j)+2]=z;
		z-=length/length_segments;
		texcoord_buffer[2*(i*length_segments+j)+1] = -z / (length/length_tiles); + .5;
		texcoord_buffer[2*(i*length_segments+j)] = 
			(float)radial_tiles/(float)radial_segments*(float)i;
		normal_buffer[3*(i*length_segments+j)]=0.0f;
		normal_buffer[3*(i*length_segments+j)+1]=-1.0f;
		normal_buffer[3*(i*length_segments+j)+2]=0.0f;
	}
	Vector3 tmp;
	float tmp_len,tmp_dot;
	for(i=0;i<radial_segments+1;i++) {
		for(j=0;j<length_segments;j++) {
			PDiff3(cam_pos,&vertex_buffer[3*(i*length_segments+j)],tmp);
			tmp_len=VMagnitude3(tmp);
			VNormalize3(tmp);
			tmp_dot=VDotProduct3(tmp,cam_dir);
			tmp[0]=cam_dir[0];
			tmp[1]=cam_dir[1];
			tmp[2]=cam_dir[2];
			VSMultiply3(tmp,sqrt(tmp_len)*(1.0-tmp_dot));
			VAdd3(&vertex_buffer[3*(i*length_segments+j)],tmp,
				&vertex_buffer[3*(i*length_segments+j)]);
		}
	}
}

void Tunnel::render() {
	int i;
	unsigned int ui;
	for(ui=0;ui<max_texture_units;ui++) {
		c_state->textures[ui].enabled=0;
		c_state->textures[ui].client_state_enabled=0;
		c_state->textures[ui].clear_tex_gen_flags();
	}
	c_state->textures[0].enabled=1;
	c_state->textures[0].coord_pointer=texcoord_buffer;
	c_state->textures[0].client_state_enabled=1;
	c_state->textures[0].tex=tex;
	c_state->textures[0].set_texture_env(GL_MODULATE);
	c_state->prerender_setup();
	float tmp_col[4]={1.0f,1.0f,1.0f,1.0f};
	c_state->want_disabled(GL_DEPTH_TEST);
	c_state->want_disabled(GL_BLEND);
	c_state->want_disabled(GL_LIGHTING);
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,tmp_col);
	glVertexPointer(3,GL_FLOAT,0,vertex_buffer);
	glNormalPointer(GL_FLOAT,0,normal_buffer);
	for(i=0;i<radial_segments;i++) {
		glDrawElements(GL_TRIANGLE_STRIP,2*length_segments,
			GL_UNSIGNED_INT,&index_buffer[2*length_segments*i]);
	}
	c_state->postrender_setup();
}*/

HelixString::HelixString(Texture *tex,float len,float diameter,float min_radius,float max_radius,
		float wav_len,float wav_period, float maximum_twist,
		float twist_time,int num_segments) {
	length=len;
	diam=diameter;
	tex1=tex;
	min_rad=min_radius;
	max_rad=max_radius;
	wave_length=wav_len;
	wave_period=wav_period;
	max_twist=maximum_twist;
	twist_period=twist_time;
	segments=num_segments;
	vertex_buffer=new float[3*(2*num_segments+2)];
	normal_buffer=new float[3*(2*num_segments+2)];
	texcoord_buffer=new float[2*(2*num_segments+2)];
}

HelixString::~HelixString() {
	if(vertex_buffer) delete vertex_buffer;
	if(normal_buffer) delete normal_buffer;
	if(texcoord_buffer) delete texcoord_buffer;
}

void HelixString::update(float frame) {
	int i;
	Point3 cam_pos;
	Vector3 cam_dir;
	if(!current_scene) return;
//	float x,y,z,r,a,sin_val,cos_val;
	float x,z,r,a,sin_val,cos_val;
	Vector3 deriv_vec,dir_vec;
	float time=frame*0.033333333f;
	float twist=max_twist; // *sin(6.28*time/twist_period);
	get_scene_camera()->get_position(cam_pos);
	get_scene_camera()->get_direction(cam_dir);
	z=cam_pos[2]+length*0.5f;
	//2 vertices starting the strip
	r=0.5*(min_rad+max_rad)+(max_rad-min_rad)*0.5*sin(6.28*z/wave_length+6.28*time/wave_period);
	a=6.28*twist/segments;
	sin_val=r*sin(a*0);
	cos_val=r*cos(a*0);
	normal_buffer[0]=-sin_val;
	normal_buffer[1]=-cos_val;
	normal_buffer[2]=0.0f;
	VNormalize3(&normal_buffer[0]);
	normal_buffer[3]=-sin_val;
	normal_buffer[4]=-cos_val;
	normal_buffer[5]=0.0f;
	VNormalize3(&normal_buffer[3]);
	deriv_vec[0]=a*cos_val;
	deriv_vec[1]=-a*sin_val;
	deriv_vec[2]=0.0f;
	VNormalize3(deriv_vec);
	VCrossProduct3(&normal_buffer[0],deriv_vec,dir_vec);
	VNormalize3(dir_vec);
	VSMultiply3(dir_vec,diam);
	vertex_buffer[0]=sin_val+dir_vec[0];
	vertex_buffer[1]=cos_val+dir_vec[1];
	vertex_buffer[2]=z+dir_vec[2];

	vertex_buffer[3]=sin_val-dir_vec[0];
	vertex_buffer[4]=cos_val-dir_vec[1];
	vertex_buffer[5]=z-dir_vec[2];

	for(i=1;i<segments+1;i++) {
		z=cam_pos[2]+length*0.5f-length/segments*i;
		r=0.5*(min_rad+max_rad)+(max_rad-min_rad)*0.5*sin(6.28*z/wave_length+6.28*time/wave_period);
		a=6.28*twist/segments*sin(60/6.28*frame);
		sin_val=r*sin(a*i);
		cos_val=r*cos(a*i);
		normal_buffer[6*i]=-sin_val;
		normal_buffer[6*i+1]=-cos_val;
		normal_buffer[6*i+2]=0.0f;
		VNormalize3(&normal_buffer[6*i]);
		VCopy3(&normal_buffer[6*i],&normal_buffer[6*i+3]);
		deriv_vec[0]=a*cos_val;
		deriv_vec[1]=-a*sin_val;
		deriv_vec[2]=0.0f;
		VNormalize3(deriv_vec);
		VCrossProduct3(&normal_buffer[6*i],deriv_vec,dir_vec);
		VNormalize3(dir_vec);
		VSMultiply3(dir_vec,diam);
		vertex_buffer[6*i]=sin_val+dir_vec[0];
		vertex_buffer[6*i+1]=cos_val+dir_vec[1];
		vertex_buffer[6*i+2]=z+dir_vec[2];
		
		vertex_buffer[6*i+3]=sin_val-dir_vec[0];
		vertex_buffer[6*i+4]=cos_val-dir_vec[1];
		vertex_buffer[6*i+5]=z-dir_vec[2];
	}
	//generate texture coordinates
	float length_tiles=8;
	for(i=0;i<2*(segments+1);i++) {
		texcoord_buffer[2*i+1] = -vertex_buffer[3*i+2]
			/ (length/length_tiles); + .5;
		x=vertex_buffer[3*i];
		texcoord_buffer[2*i]=x/ (length/length_tiles) + .5; 
	}
}

void HelixString::render() {
//	int i;
	unsigned int ui;
	for(ui=0;ui<max_texture_units;ui++) {
		c_state->textures[ui].enabled=0;
		c_state->textures[ui].client_state_enabled=0;
		c_state->textures[ui].clear_tex_gen_flags();
	}
	c_state->textures[0].enabled=1;
	c_state->textures[0].coord_pointer=texcoord_buffer;
	c_state->textures[0].client_state_enabled=1;
	c_state->textures[0].tex=tex1;
	c_state->textures[0].set_texture_env(GL_MODULATE);
	c_state->prerender_setup();
	float tmp_col[4]={0.5f,1.0f,1.0f,0.2f};
	c_state->want_disabled(GL_DEPTH_TEST);
	c_state->want_enabled(GL_BLEND);
	c_state->want_enabled(GL_LIGHTING);
	c_state->want_enabled(GL_CULL_FACE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,tmp_col);
	glColor4fv(tmp_col);
	glVertexPointer(3,GL_FLOAT,0,vertex_buffer);
	glNormalPointer(GL_FLOAT,0,normal_buffer);
	glDrawArrays(GL_QUAD_STRIP,0,2*(segments+1));
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
	c_state->postrender_setup();
	c_state->want_enabled(GL_CULL_FACE);
}

HelixParticles::HelixParticles(Motion mot,Texture *p_tex) : ParticleSystem(mot,p_tex) {
}


void HelixParticles::calc_motion(float frame) {
	float time;
	time=frame*0.033333333f;
	e_x=radius*sin(6.28/2.0*time+angle_offset);
	e_y=radius*cos(6.28/2.0*time+angle_offset);
	e_z=start-speed*time;
}

void HelixParticles::set_motion_parameters(float rad,float spd,float start_point,
										   float ang_offset) {
	radius=rad;
	speed=spd;
	start=start_point;
	angle_offset=ang_offset;
}

LH_TunnelEffect::LH_TunnelEffect() {
	scn=new Scene3D("scenes/lh_tunnel.lws");
//	tex=new Texture("textures/babelki.jpg");
	back_tex=new Texture("textures/addfme.jpg");

	tunnel1=new HelixString(tex,30.0f,1.5f,0.9f,2.7f,12.0f,5.0f,20.0f,50.0f,1001);
	tunnel1->set_current_scene(scn);
	
	tunnel2=new HelixString(tex,30.0f,1.5f,1.7f,3.5f,12.0f,5.0f,20.0f,50.0f,1001);
	tunnel2->set_current_scene(scn);
	
//	TexLoadDesc tdesc;
//	tdesc.filename="flares/flare1.jpg";
//	tdesc.min_filter=GL_NEAREST;
//	tdesc.wrap_s=GL_CLAMP;
//	tdesc.wrap_t=GL_CLAMP;
	p_tex=new Texture("textures/flare1.jpg");
	Motion* tmp_mot;
	tmp_mot=new Motion();
	p_system1=new HelixParticles(*tmp_mot,p_tex);
	p_system1->set_color(0.25f,0.5f,0.5f);
	p_system1->set_current_scene(scn);
	p_system1->set_emission(25.0f);
	p_system1->set_sizes(0.1f,0.2f);
	p_system1->set_times(1.0f,1.0f,20.0f);
	p_system1->set_spread(0.03f);
	p_system1->set_motion_parameters(1.1f,1.0f,-3.0f,0.0f);

	p_system2=new HelixParticles(*tmp_mot,p_tex);
	p_system2->set_color(0.25f,0.5f,0.5f);
	p_system2->set_current_scene(scn);
	p_system2->set_emission(25.0f);
	p_system2->set_sizes(0.1f,0.2f);
	p_system2->set_times(1.0f,1.0f,20.0f);
	p_system2->set_spread(0.03f);
	p_system2->set_motion_parameters(1.1f,1.0f,-3.0f,0.333f*6.28f);

	p_system3=new HelixParticles(*tmp_mot,p_tex);
	p_system3->set_color(0.25f,0.5f,0.5f);
	p_system3->set_current_scene(scn);
	p_system3->set_emission(25.0f);
	p_system3->set_sizes(0.1f,0.2f);
	p_system3->set_times(1.0f,1.0f,20.0f);
	p_system3->set_spread(0.03f);
	p_system3->set_motion_parameters(1.1f,1.0f,-3.0f,0.666f*6.28f);

	GLubyte* tmp_buf=new GLubyte[512*512*4];
	render_target=new Texture(512,512,tmp_buf);
	delete tmp_buf;
}

LH_TunnelEffect::~LH_TunnelEffect() {
//	delete tex;
	delete p_tex;
	delete render_target;
	delete back_tex;
	delete scn;
	delete tunnel1;
	delete tunnel2;
	delete p_system1;
	delete p_system2;
	delete p_system3;
}

void LH_TunnelEffect::render(int time) {
	if(hasPbuffers && (pbufferRC) && (pbufferDC)) {
		if(!wglMakeCurrent(pbufferDC,pbufferRC)) {
			int res;
			wglQueryPbufferARB(PBuffer_handle,WGL_PBUFFER_LOST_ARB,&res);
			if(res) PrintfBox("Pbuffer lost");
			else PrintfBox("Last error %d",GetLastError());
		};
		c_state->synchronize();
	}
	glViewport(0,0,512,512);
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	DrawQuad(back_tex);
	current_scene=scn;
	(scn->get_camera())->draw(30.0f*(float)time*0.001f);
	p_system1->update(30.0f*(float)time*0.001f);
	p_system2->update(30.0f*(float)time*0.001f);
	p_system3->update(30.0f*(float)time*0.001f);
//	scn->render(time);
	p_system1->render();
	p_system2->render();
	p_system3->render();
	render_target->backbuffer_copy(512);
	if(hasPbuffers && (pbufferRC) && (pbufferDC)) {
		if(!wglMakeCurrent(hDC,hRC)) {
			PrintfBox("wglmakeCurrent failed");
		}
		c_state->synchronize();
	}
	glViewport(0,0,width,height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	float x,y;
	x=0.05*sin(time*0.0001*6.28);
	y=0.05*sin(time*0.00012*6.28);
	int i;
	float px,py;
	glColor4f(1.0f,1.0f,1.0f,0.25f);
	for(i=1;i<5;i++) {
		px=x*i+0.5f;
		py=y*i+0.5f;
		DrawQuad(render_target,-px*(1.0f+0.1f*i)+px,
			-py*(1.0f+0.1f*i)+py,(1.0f-px)*(1.0f+0.1f*i)+px,
			(1.0f-py)*(1.0f+0.1f*i)+py,0.0f,1.0f,1.0f,0.0f,1,GL_SRC_ALPHA,
			GL_ONE,0);
	}
}