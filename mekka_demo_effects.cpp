

#include "mekka_demo_effects.h"
#include "bass.h"
#include <math.h>

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float

extern HSTREAM music_module;
static const float deg2rad=3.14159f/180.0f;
extern "C++" HDC hDC;
extern "C++" HGLRC hRC;
extern "C++" HDC pbufferDC;
extern "C++" HGLRC pbufferRC;
extern "C++" Texture *pbuffer_texture;

Texture *render_target;

Mekka_Blobs::Mekka_Blobs(float mini_x,float maxi_x,float mini_y,float maxi_y,
				   float mini_z,float maxi_z,int res,Texture *tx) : ScField(mini_x,
				   maxi_x,mini_y,maxi_y,mini_z,maxi_z,res) {
	quad_vertex_buffer=NULL;;
	num_quads=0;
	quad_color_buffer=0;
	quad_table_size=0; 
	tex=tx;
}

Mekka_Blobs::~Mekka_Blobs() {
	if(quad_vertex_buffer) delete quad_vertex_buffer;
	if(quad_color_buffer) delete quad_color_buffer;
}

void Mekka_Blobs::update(float frame) {
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
	DWORD volume=BASS_ChannelGetLevel(music_module);
	float vol_level=(HIWORD(volume)+LOWORD(volume))/256.0f;
	vol_level+=0.1f;
	vol_level*=vol_level;
	vol_level*=vol_level;
	float norm_scale=0.4f+vol_level;
	for(i=0;i<num_triangles;i++) {  //build 3 quads
		if(indices[3*i]<indices[3*i+1]) {
			quad_vertex_buffer[num_quads*4*3]=vertex_buffer[3*indices[3*i]];
			quad_vertex_buffer[num_quads*4*3+1]=vertex_buffer[3*indices[3*i]+1];
			quad_vertex_buffer[num_quads*4*3+2]=vertex_buffer[3*indices[3*i]+2];
			
			quad_vertex_buffer[num_quads*4*3+3]=vertex_buffer[3*indices[3*i+1]];
			quad_vertex_buffer[num_quads*4*3+4]=vertex_buffer[3*indices[3*i+1]+1];
			quad_vertex_buffer[num_quads*4*3+5]=vertex_buffer[3*indices[3*i+1]+2];
			
			quad_vertex_buffer[num_quads*4*3+6]=vertex_buffer[3*indices[3*i+1]]+
				norm_scale*normal_buffer[3*indices[3*i+1]];
			quad_vertex_buffer[num_quads*4*3+7]=vertex_buffer[3*indices[3*i+1]+1]+
				norm_scale*normal_buffer[3*indices[3*i+1]+1];
			quad_vertex_buffer[num_quads*4*3+8]=vertex_buffer[3*indices[3*i+1]+2]+
				norm_scale*normal_buffer[3*indices[3*i+1]+2];
			
			quad_vertex_buffer[num_quads*4*3+9]=vertex_buffer[3*indices[3*i]]+
				norm_scale*normal_buffer[3*indices[3*i]];
			quad_vertex_buffer[num_quads*4*3+10]=vertex_buffer[3*indices[3*i]+1]+
				norm_scale*normal_buffer[3*indices[3*i]+1];
			quad_vertex_buffer[num_quads*4*3+11]=vertex_buffer[3*indices[3*i]+2]+
				norm_scale*normal_buffer[3*indices[3*i]+2];
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
				norm_scale*normal_buffer[3*indices[3*i+2]];
			quad_vertex_buffer[num_quads*4*3+7]=vertex_buffer[3*indices[3*i+2]+1]+
				norm_scale*normal_buffer[3*indices[3*i+2]+1];
			quad_vertex_buffer[num_quads*4*3+8]=vertex_buffer[3*indices[3*i+2]+2]+
				norm_scale*normal_buffer[3*indices[3*i+2]+2];
			
			quad_vertex_buffer[num_quads*4*3+9]=vertex_buffer[3*indices[3*i+1]]+
				norm_scale*normal_buffer[3*indices[3*i+1]];
			quad_vertex_buffer[num_quads*4*3+10]=vertex_buffer[3*indices[3*i+1]+1]+
				norm_scale*normal_buffer[3*indices[3*i+1]+1];
			quad_vertex_buffer[num_quads*4*3+11]=vertex_buffer[3*indices[3*i+1]+2]+
				norm_scale*normal_buffer[3*indices[3*i+1]+2];
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
				norm_scale*normal_buffer[3*indices[3*i+2]];
			quad_vertex_buffer[num_quads*4*3+7]=vertex_buffer[3*indices[3*i+2]+1]+
				norm_scale*normal_buffer[3*indices[3*i+2]+1];
			quad_vertex_buffer[num_quads*4*3+8]=vertex_buffer[3*indices[3*i+2]+2]+
				norm_scale*normal_buffer[3*indices[3*i+2]+2];
			
			quad_vertex_buffer[num_quads*4*3+9]=vertex_buffer[3*indices[3*i]]+
				norm_scale*normal_buffer[3*indices[3*i]];
			quad_vertex_buffer[num_quads*4*3+10]=vertex_buffer[3*indices[3*i]+1]+
				norm_scale*normal_buffer[3*indices[3*i]+1];
			quad_vertex_buffer[num_quads*4*3+11]=vertex_buffer[3*indices[3*i]+2]+
				norm_scale*normal_buffer[3*indices[3*i]+2];
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

void Mekka_Blobs::render() {
	if(hasVertexArrayRange) {
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
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
	c_state->textures[0].set_texture_env(GL_REPLACE);
	c_state->textures[0].client_state_enabled=0;
	float col[4]={1.0f,1.0f,1.0f,0.0f};
	float zero_col[4]={0.0f,0.0f,0.0f,0.0f};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,col);
//	glMaterialfv(GL_FRONT,GL_SPECULAR,col);
	c_state->want_enabled(GL_LIGHTING);
	c_state->want_enabled(GL_DEPTH_TEST);
	c_state->want_disabled(GL_BLEND);
	c_state->want_enabled(GL_CULL_FACE);
	c_state->prerender_setup();
	if(hasATIPNTriangles) {
		glEnable(GL_PN_TRIANGLES_ATI);
		glPNTrianglesiATI(GL_PN_TRIANGLES_POINT_MODE_ATI,GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI);
		glPNTrianglesiATI(GL_PN_TRIANGLES_NORMAL_MODE_ATI,GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI);
		glPNTrianglesiATI(GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI,2);
	}
	ScField::render();
	if(hasATIPNTriangles) {
		glDisable(GL_PN_TRIANGLES_ATI);
	}
	c_state->postrender_setup();
	//render "rays"
	glDepthMask(GL_FALSE);
	c_state->want_disabled(GL_LIGHTING);
	c_state->want_enabled(GL_BLEND);
	c_state->want_disabled(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
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
	c_state->want_enabled(GL_CULL_FACE);
	if(hasVertexArrayRange) {
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
}

Mekka_BlobsEffect1::Mekka_BlobsEffect1() {
	tex=tex_manager.get_texture("textures/bloby.jpg");
	tex2=tex_manager.get_texture("textures/laser.jpg");
	blob=new Mekka_Blobs(-1.2f,1.2f,-1.2f,1.2f,-1.2f,1.2f,16,tex);
	blob->add_blob("blob1.mot");
	blob->add_blob("blob2.mot");
	blob->add_blob("blob3.mot");
	blob->add_blob("blob4.mot");
	Envelope env;
	env=25.0f;
	blob->set_isovalue(&env);
	scn=new Scene3D("scenes/scenka_7.lws",45.0f);
	scn->render();
}

Mekka_BlobsEffect1::~Mekka_BlobsEffect1() {
	delete blob;
	delete scn;
}

void Mekka_BlobsEffect1::render(int time) {
	glPushMatrix();
	scn->get_camera()->draw(scn->get_frame());
	blob->update(30.0f*(float)time*0.001f);
	glScalef(50.0f,50.0f,50.0f);
	glRotatef(0.03f*time,0.0f,1.0f,0.0f);
	glRotatef(0.1f*time,1.0f,0.0f,0.0f);
	glRotatef(0.03f*time,0.0f,0.0f,1.0f);
	blob->render();
	glPopMatrix();
	scn->render(time);


	//render the quad ring
	int num_rays=15;
	float ray_height=1800.0f;
	float ray_width=18.0f;
	float rad=130.0f;

	Vector3 cam_pos,cam_dir,width_dir;
	scn->get_camera()->get_position(cam_pos);
	scn->get_camera()->get_direction(cam_dir);
	Vector3 up_vec;
	up_vec[0]=0.0f;
	up_vec[1]=1.0f;
	up_vec[2]=0.0f;
	VCrossProduct3(up_vec,cam_dir,width_dir);
	VNormalize3(width_dir);
	VSMultiply3(width_dir,ray_width);
	VSMultiply3(up_vec,ray_height);
	Vector3 tmp_pt;

	Vector3 down_vec;
	Vector3 neg_width;
	VCopy3(up_vec,down_vec);
	VSMultiply3(down_vec,-1.0f);
	VCopy3(width_dir,neg_width);
	VSMultiply3(neg_width,-1.0f);
	Vector3 view_dir;


	int i;
	for(i=0;i<(int)max_texture_units;i++) {
		c_state->textures[i].enabled=0;
		c_state->textures[i].clear_tex_gen_flags();
		c_state->textures[i].client_state_enabled=0;
	}
	glDepthMask(GL_FALSE);
	c_state->want_disabled(GL_LIGHTING);
	c_state->want_enabled(GL_BLEND);
//	c_state->want_disabled(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	c_state->textures[0].enabled=1;
	c_state->textures[0].clear_tex_gen_flags();
	c_state->textures[0].client_state_enabled=0;
	c_state->textures[0].tex=tex2;
	c_state->textures[0].set_texture_env(GL_REPLACE);
	c_state->prerender_setup();

	//te linijki sa dlatego ze sie cos zwalilo w state managerze
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	tex2->activate();



	glBegin(GL_QUADS);
	for(i=0;i<num_rays;i++) {
		tmp_pt[0]=sin(6.28/num_rays*i+6.28/10000*time)*rad;
		tmp_pt[1]=0.0f;
		tmp_pt[2]=cos(6.28/num_rays*i+6.28/10000*time)*rad;

		PDiff3(cam_pos,tmp_pt,view_dir);
		VNormalize3(view_dir);
		VCrossProduct3(up_vec,view_dir,width_dir);
		VNormalize3(width_dir);
		VSMultiply3(width_dir,ray_width);
		VCopy3(width_dir,neg_width);
		VSMultiply3(neg_width,-1.0f);

		glTexCoord2f(0.0f,0.0f);
		VAdd3(tmp_pt,up_vec,tmp_pt);
		VAdd3(tmp_pt,neg_width,tmp_pt);
		glVertex3fv(tmp_pt);

		glTexCoord2f(1.0f,0.0f);
		VAdd3(tmp_pt,width_dir,tmp_pt);
		VAdd3(tmp_pt,width_dir,tmp_pt);
		glVertex3fv(tmp_pt);

		glTexCoord2f(1.0f,1.0f);
		VAdd3(tmp_pt,down_vec,tmp_pt);
		VAdd3(tmp_pt,down_vec,tmp_pt);
		glVertex3fv(tmp_pt);

		glTexCoord2f(0.0f,1.0f);
		VAdd3(tmp_pt,neg_width,tmp_pt);
		VAdd3(tmp_pt,neg_width,tmp_pt);
		glVertex3fv(tmp_pt);
	}
	glEnd();
	c_state->postrender_setup();
	glDepthMask(GL_TRUE);
}

Mekka_HelixWall::Mekka_HelixWall(float rad,int num_twists,float height) {
	num_verts=600;
	r=rad;
	twists=num_twists;
	h=height;
	vert_coords=new float[3*num_verts];
	norm_coords=new float[3*num_verts];
	indices=new unsigned short[num_verts];
	int i;
	for(i=0;i<num_verts;i++) indices[i]=i;
	is_optimized=0;
	agp_vert_coords=NULL;
	agp_norm_coords=NULL;
	float step=twists*6.28/(0.5f*num_verts);
	for(i=0;i<num_verts/2;i++) {
		norm_coords[3*2*i]=sin(step*i);
		norm_coords[3*2*i+1]=0.0f;
		norm_coords[3*2*i+2]=cos(step*i);
		norm_coords[3*2*i+3]=sin(step*i);
		norm_coords[3*2*i+4]=0.0f;
		norm_coords[3*2*i+5]=cos(step*i);
	}
	if(hasVertexArrayRange) {
		agp_vert_coords=(float*)agp_alloc->get_agp_block(num_verts*3*sizeof(float));
		if(!agp_vert_coords) return;
		agp_norm_coords=(float*)agp_alloc->get_agp_block(num_verts*3*sizeof(float));
		if(!agp_norm_coords) return;
		is_optimized=1;
		memcpy((void*)agp_norm_coords,(void*)norm_coords,num_verts*3*sizeof(float));
	}
}

Mekka_HelixWall::~Mekka_HelixWall() {
	delete vert_coords;
	delete norm_coords;
	delete indices;
}

void Mekka_HelixWall::render(int time) {
	float step=twists*6.28/(0.5f*num_verts);
	float z_step=h/(0.5f*num_verts);
	unsigned int i;
	for(i=0;i<(unsigned int)(num_verts/2);i++) {
		vert_coords[3*2*i]=(r+0.4f*r*sin(6.28/h*z_step*i+0.00314f*time))*sin(step*i);
		vert_coords[3*2*i+1]=z_step*i;
		vert_coords[3*2*i+2]=(r+0.4f*r*sin(6.28/h*z_step*i+0.00314f*time))*cos(step*i);
		vert_coords[3*2*i+3]=(r+0.4f*r*sin(6.28/h*z_step*i+0.00314f*time))*sin(step*i);
		vert_coords[3*2*i+4]=z_step*i+0.1f;
		vert_coords[3*2*i+5]=(r+0.4f*r*sin(6.28/h*z_step*i+0.00314f*time))*cos(step*i);
	}
	if(is_optimized) 
		memcpy((void*)agp_vert_coords,
			(void*)vert_coords,
			num_verts*3*sizeof(float));
	for(i=0;i<max_texture_units;i++) {
		c_state->textures[i].enabled=0;
		c_state->textures[i].client_state_enabled=0;
		c_state->textures[i].clear_tex_gen_flags();
	}
	c_state->prerender_setup();
	c_state->want_disabled(GL_ALPHA_TEST);
	c_state->want_disabled(GL_CULL_FACE);
	c_state->want_enabled(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	float helix_color[4]={0.3f,0.3f,0.8f,0.7f};
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,helix_color);
	if(!is_optimized && hasVertexArrayRange) {
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	if(is_optimized) {
		glVertexPointer(3,GL_FLOAT,0,agp_vert_coords);
		glNormalPointer(GL_FLOAT,0,agp_norm_coords);
	}
	else {
		glVertexPointer(3,GL_FLOAT,0,vert_coords);
		glNormalPointer(GL_FLOAT,0,norm_coords);
	}
	glDepthMask(0);
	for(i=0;i<2;i++) {
		glPushMatrix();
		glTranslatef(-4.0f+8.0f*i,0.0f,0.0f);

		glPushMatrix();
		glRotatef(time/10,0.0,1.0,0.0);
		glDrawElements(GL_TRIANGLE_STRIP,num_verts,GL_UNSIGNED_SHORT,indices);
		glPopMatrix();

		glPushMatrix();
		glRotatef(120+time/10,0.0,1.0,0.0);
		glDrawElements(GL_TRIANGLE_STRIP,num_verts,GL_UNSIGNED_SHORT,indices);
		glPopMatrix();

		glPushMatrix();
		glRotatef(240+time/10,0.0,1.0,0.0);
		glDrawElements(GL_TRIANGLE_STRIP,num_verts,GL_UNSIGNED_SHORT,indices);
		glPopMatrix();

		glPopMatrix();
	}
	glDepthMask(1);
	c_state->want_enabled(GL_CULL_FACE);
	if(!is_optimized && hasVertexArrayRange) {
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
}

static Envelope* read_envelope(const char* file) {
	int i,num_keys;
	float value;
	int frame,linear;
	double tens,cont,bias;
	Envelope *env=new Envelope();
	char tmp_str[20];
	int f;
	vfs->openFile(file,f);
	vfs->scanString(f,tmp_str);
	vfs->scanInt(f,&num_keys);
	vfs->scanInt(f,&num_keys);
	vfs->scanInt(f,&num_keys);
	env->clear();
	env->alloc_keyframe_mem(num_keys);
	for(i=0;i<num_keys;i++) {
		vfs->scanFloat(f,&value);
		vfs->scanInt(f,&frame);
		vfs->scanInt(f,&linear);
		vfs->scanDouble(f,&tens);
		vfs->scanDouble(f,&cont);
		vfs->scanDouble(f,&bias);
		env->add_key_frame(value,tens,cont,bias,linear,frame);
	}
	vfs->closeFile(f);
	env->set_end_behavior(2);
	return env;
}  //read_envelope


//scenka 1
Mekka_Scenka1::Mekka_Scenka1() {
	scn=new Scene3D("scenes/scenka_5.lws",3.0f);
	napisy=new Scene3D("scenes/napisy_ms_scenka1.lws");
	sphere=new STSphere(ST_RM_MATERIAL_ONLY,50);
	torus=new STTorus(ST_RM_MATERIAL_ONLY,50);
	if((hasRegisterCombiners || hasATIFragmentShader) && hasCubeMaps) {
		mesh=new STMeshMorph(ST_RM_DOT3_SPECULAR,50);
		mesh->set_bump_map("textures/detail.jpg");
	}
	else if(hasTextureEnvDot3 && hasCubeMaps) {
		mesh=new STMeshMorph(ST_RM_DOT3_ARB,50);
		mesh->set_bump_map("textures/detail.jpg");
	}
	else {
		mesh=new STMeshMorph(ST_RM_BASE_TEXTURE,50);
	}
	mesh->set_base_texture("textures/30-09-97.jpg",2.0f,2.0f);
	mesh->add_mesh(sphere);
	mesh->add_mesh(torus);
	Envelope *tmp_env=read_envelope("motions/morph1.env");
	mesh->set_morph_env(*tmp_env);
	delete tmp_env;
	sphere->set_num_waves(5,5);
	torus->set_num_waves(3,5);
	heliksy=new Mekka_HelixWall(1.0f,5,8.0f);
//	tmp_mesh->set_wave_amplitude(0.3f);
//	mesh=(STTorus*) tmp_mesh;
}

Mekka_Scenka1::~Mekka_Scenka1() {
	delete scn;
	delete mesh;
	delete napisy;
	delete sphere;
	delete torus;
	delete heliksy;
}

void Mekka_Scenka1::render(int time) {
	scn->render(time);
	
	glPushMatrix();
	glScalef(4.5f,4.5f,4.5f);
	glTranslatef(0.4f,-4.0f,0.0f);
	heliksy->render(time);
	glPopMatrix();

	sphere->set_wave_amplitude(0.2f*sin(0.001*time));
	torus->set_R_wave_amplitude(0.3f*sin(0.0012*time));
	torus->set_r_wave_amplitude(0.1f*sin(0.002*time));
	if((hasRegisterCombiners && hasCubeMaps)
		|| (hasTextureEnvDot3 && hasCubeMaps)) {
		Vector3 cam_pos;
		scn->get_camera()->get_position(cam_pos);
		Matrix mx,mx2,mx3;
		Vector3 pos;
		pos[0]=2.2f;
		pos[1]=0.0f;
		pos[2]=43.0f;
		MTranslation(mx,pos);
		MRotationY(mx2,0.03*time*deg2rad);
		MMultiply(mx,mx2,mx3);
		MRotationX(mx2,0.04*time*deg2rad);
		MMultiply(mx3,mx2,mx);
		MRotationZ(mx2,0.035*time*deg2rad);
		MMultiply(mx,mx2,mx3);
		mesh->set_eye_pos_indirect(mx3,cam_pos[0],cam_pos[1],cam_pos[2]);
		mesh->set_light_pos_indirect(mx3,-100.0f,25.0f,100.0f);
	}
	mesh->update(time);
	glPushMatrix();
	glTranslatef(2.2f,0.0f,43.0f);
	glRotatef(0.03*time,0.0f,1.0f,0.0f);
	glRotatef(0.04*time,1.0f,0.0f,0.0f);
	glRotatef(0.035*time,0.0f,0.0f,1.0f);
	DWORD volume=BASS_ChannelGetLevel(music_module);
	float vol_level=(HIWORD(volume)+LOWORD(volume))/256.0f;
/*	if(vol_level>0.2f) {
		vol_level-=0.2f;
	}
	else {
		vol_level=0.0f;
	}
	vol_level*=vol_level;
	vol_level*=2.0f;*/
	vol_level*=0.2f;
	vol_level+=1.0f;
	vol_level*=vol_level;
	vol_level-=1.0f;
	glScalef(1.1f+vol_level,1.1f+vol_level,1.1f+vol_level);
	float white_col[]={1.0f,1.0f,1.0f,1.0f};
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,white_col);

	c_state->want_disabled(GL_BLEND);
	c_state->want_disabled(GL_ALPHA_TEST);
	mesh->render();
	glScalef(1.3f,1.3f,1.3f);
	c_state->want_enabled(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	float alpha_col[]={1.0f,1.0f,1.0f,0.3f};
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,alpha_col);
	mesh->render_base_only();
	glPopMatrix();
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
/*	GLboolean	glb;
	glGetBooleanv(GL_DEPTH_WRITEMASK,&glb);
	if(glb) {
		DebugFrameMessage("INFO:Depth mask is true");
	}
	else {
		DebugFrameMessage("INFO:depth mask is false");
	}*/
	napisy->render(time);
}

Mekka_Scene_BlurOut::Mekka_Scene_BlurOut(DMEffect *blur_target,int ef_offset,
										 int ef_blur_length) :
ef(blur_target), offset(ef_offset), blur_length(ef_blur_length) {
	char* tmp;
	tmp=new char[512*512*4];
	ZeroMemory(tmp,512*512*4);
	render_target=new Texture(512,512,tmp);
	delete tmp;
}

Mekka_Scene_BlurOut::~Mekka_Scene_BlurOut() {
	delete render_target;
}


void Mekka_Scene_BlurOut::render(int time) {
	if(height==480) {
		glViewport(0,0,512,480);
	}
	else {
		glViewport(0,0,512,512);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	ef->render(time+offset);
	render_target->backbuffer_copy(512);
	glViewport(0,0,width,height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	int i;
	float fraction=(float)time/blur_length;
	glColor4f(1.0f,1.0f,1.0f,0.25*(1.0-fraction));
	fraction*=0.25f;
	for(i=0;i<4;i++) {
		if(height==480) {
			DrawQuad(render_target,0.0f-fraction*i,0.0f-fraction*i,
				1.0f+fraction*i,1.0f+fraction*i,0.0f,0.9375f,1.0f,0.0f,1,
				GL_SRC_ALPHA,GL_ONE,0);
		}
		else {
			DrawQuad(render_target,0.0f-fraction*i,0.0f-fraction*i,
				1.0f+fraction*i,1.0f+fraction*i,0.0f,1.0f,1.0f,0.0f,1,
				GL_SRC_ALPHA,GL_ONE,0);
		}
	}
}



Mekka_AudioHrzBlur::Mekka_AudioHrzBlur(DMEffect *blur_target,int ef_offset) :
ef(blur_target), offset(ef_offset) {
}

Mekka_AudioHrzBlur::~Mekka_AudioHrzBlur() {
}

void Mekka_AudioHrzBlur::render(int time) {
	if(height==480) {
		glViewport(0,0,512,480);
	}
	else {
		glViewport(0,0,512,512);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	ef->render(time+offset);
	render_target->backbuffer_copy(512);
	glViewport(0,0,width,height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	int i;
	glColor4f(1.0f,1.0f,1.0f,0.25f);
	DWORD volume=BASS_ChannelGetLevel(music_module);
	float vol_level=(HIWORD(volume)+LOWORD(volume))/256.0f;
	if(vol_level>0.2f) {
		vol_level-=0.2f;
	}
	else {
		vol_level=0.0f;
	}
	vol_level*=vol_level;
	for(i=0;i<4;i++) {
		if(height==480) {
			DrawQuad(render_target,0.0f-vol_level*i,0.0f,
				1.0f+vol_level*i,1.0f,0.0f,0.9375f,1.0f,0.0f,1,
				GL_SRC_ALPHA,GL_ONE,0);
		}
		else {
			DrawQuad(render_target,0.0f-vol_level*i,0.0f,
				1.0f+vol_level*i,1.0f,0.0f,1.0f,1.0f,0.0f,1,
				GL_SRC_ALPHA,GL_ONE,0);
		}
	}
}

Mekka_SceneShift::Mekka_SceneShift(DMEffect *effect,int normal_period_p,int shifted_period_p,
								   float shift_amount_p) {
	ef=effect;
	normal_period=normal_period_p;
	shifted_period=shifted_period_p;
	shift_amount=shift_amount_p;
}
void Mekka_SceneShift::render(int time) {
	int tmp_time=time%(normal_period+shifted_period);
	if(tmp_time<normal_period) {
		ef->render(time);
	}
	else {   //render to texture texture shifting
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
//		int i;
		glColor4f(1.0f,1.0f,1.0f,0.5f);
		float amount;
		if((tmp_time-normal_period)<(shifted_period/2)) {
			amount=2.0f*shift_amount*(tmp_time-normal_period)/shifted_period;
		}
		else {
			amount=shift_amount-
				2.0f*shift_amount*(tmp_time-normal_period-shifted_period/2)/shifted_period;
		}
		if(height==480) {
			DrawQuad(render_target,0.0f,0.0f,
				1.0f+amount,1.0f,0.0f,0.9375f,1.0f,0.0f,1,
				GL_SRC_ALPHA,GL_ONE,0);
		}
		else {
			DrawQuad(render_target,0.0f,0.0f,
				1.0f+amount,1.0f,0.0f,1.0f,1.0f,0.0f,1,
				GL_SRC_ALPHA,GL_ONE,0);
		}
		if(height==480) {
			DrawQuad(render_target,0.0f-amount,0.0f,
				1.0f,1.0f,0.0f,0.9375f,1.0f,0.0f,1,
				GL_SRC_ALPHA,GL_ONE,0);
		}
		else {
			DrawQuad(render_target,0.0f-amount,0.0f,
				1.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1,
				GL_SRC_ALPHA,GL_ONE,0);
		}
	}
}


Mekka_AudioImgFlash::Mekka_AudioImgFlash(char *filename,float length) {
	tex=tex_manager.get_texture(filename);
	len=length;
}

void Mekka_AudioImgFlash::render(int time) {
	if(time<0) return;
	if(time>len) return;
	DWORD volume=BASS_ChannelGetLevel(music_module);
	float vol_level=(HIWORD(volume)+LOWORD(volume))/256.0f;
	if(vol_level>0.2f) {
		vol_level-=0.2f;
	}
	else {
		vol_level=0.0f;
	}
	vol_level*=vol_level;
	int i=2;
	if(time<(len*0.5f)) {
		glColor4f(1.0f,1.0f,1.0f,2.0f*time/len);
		DrawQuad(tex,0.0f-vol_level*i,0.0f,1.0f+vol_level*i,1.0f,
			0.0f,0.0f,1.0f,1.0f,1,GL_SRC_ALPHA,GL_ONE,0);
	}
	else {
		glColor4f(1.0f,1.0f,1.0f,1.0f-2.0f*(time-0.5f*len)/len);
		DrawQuad(tex,0.0f-vol_level*i,0.0f,1.0f+vol_level*i,
			1.0f,0.0f,0.0f,1.0f,1.0f,1,GL_SRC_ALPHA,GL_ONE,0);
	}
}

Mekka_SineImgFlash::Mekka_SineImgFlash(char *filename,float length,float ofs,float scl) {
	tex=tex_manager.get_texture(filename);
	len=length;
	offset=ofs;
	scale=scl;
}

void Mekka_SineImgFlash::render(int time) {
	glColor4f(1.0f,1.0f,1.0f,scale*sin(6.28/len*time)+offset);
	DrawQuad(tex,0.0f,0.0f,1.0f,1.0f,
		0.0f,0.0f,1.0f,1.0f,1,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,0);
}

Mekka_CreditsImg::Mekka_CreditsImg() {
	tex=new Texture("SXB_Optical_credits.jpg");
}

void Mekka_CreditsImg::render(int time) {
	if(time<6000) {
		glColor4f(1.0f,1.0f,1.0f,1.0f);
	}
	else if(time<9000) {
		glColor4f(1.0f,1.0f,1.0f,1.0f-(time-6000)/3000.0f);
	}
	else {
		return;
	}
	DrawQuad(tex,0.0f,0.0f,1.0f,1.0f,
		0.0f,0.0f,1.0f,1.0f,1,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,0);
}

Mekka_MultiSceneBlur::Mekka_MultiSceneBlur() {
	last_render=0;
	char *tmp;
	tmp=new char[512*512*4];
	ZeroMemory(tmp,512*512*4);
	additionalRT=new Texture(512,512,tmp);
	delete tmp;
	tex=render_target;
	tex_prev=render_target;
}

Mekka_MultiSceneBlur::~Mekka_MultiSceneBlur() {
	delete additionalRT;
}

void Mekka_MultiSceneBlur::add_effect(DMEffect *ef,int start_tm,int end_tm,int len) {
	effects.add_element(ef);
	end_times.add_element(end_tm);
	lengths.add_element(len);
	start_times.add_element(start_tm);
}

void Mekka_MultiSceneBlur::render(int time) {
	//the new way
	//stage 1: render scene,lerp with tex_prev,move2tex
	int i=0;
	int offset=0;
	while((i<lengths.length-1) && (offset+lengths[i]<time)) {
		offset+=lengths[i];
		i++;
	}
	float x1,y1,x2,y2;
	x1=offset;
	x2=offset+lengths[i];
	y1=start_times[i];
	y2=end_times[i];
	float effect_time=(y1-y2)/(x1-x2)*time+(x1*y2-x2*y1)/(x1-x2);

	if(height==480) {
		glViewport(0,0,512,480);
	}
	else {
		glViewport(0,0,512,512);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	effects[i]->render(effect_time);
	float alpha_val=pow(0.5,(time-last_render)*0.003f);
	glColor4f(1.0f,1.0f,1.0f,alpha_val);
	if(height==480) {
		DrawQuad(tex_prev,0.0f,0.0f,
			1.0f,1.0f,0.0f,0.9375f,1.0f,0.0f,1,
			GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,0);
	}
	else {
		DrawQuad(tex_prev,0.0f,0.0f,
			1.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1,
			GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,0);
	}
	tex->backbuffer_copy(512);
	//stage 2: draw tex, switch texture pointers
	glViewport(0,0,width,height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	if(height==480) {
		DrawQuad(tex,0.0f,0.0f,1.0f,1.0f,0.0f,0.9375f,1.0f,0.0f);
	}
	else {
		DrawQuad(tex,0.0f,0.0f,1.0f,1.0f,0.0f,1.0f,1.0f,0.0f);
	}
	if(tex==tex_prev) {		//we are in initial state
		tex=additionalRT;
	}
	else {					//simple exchange
		Texture* pTmpTex;
		pTmpTex=tex;
		tex=tex_prev;
		tex_prev=pTmpTex;
	}
	last_render=time;

/*	//fill the texture with previous frame
	int i=0;
	int offset=0;
	while((i<lengths.length-1) && (offset+lengths[i]<last_render)) {
		offset+=lengths[i];
		i++;
	}
	float x1,y1,x2,y2;
	x1=offset;
	x2=offset+lengths[i];
	y1=start_times[i];
	y2=end_times[i];
	float effect_time=(y1-y2)/(x1-x2)*last_render+(x1*y2-x2*y1)/(x1-x2);

	if(height==480) {
		glViewport(0,0,512,480);
	}
	else {
		glViewport(0,0,512,512);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	effects[i]->render(effect_time);
	float alpha_val=pow(0.5,(time-last_render)*0.003f);
	glColor4f(1.0f,1.0f,1.0f,alpha_val);
	if(height==480) {
		DrawQuad(render_target,0.0f,0.0f,
			1.0f,1.0f,0.0f,0.9375f,1.0f,0.0f,1,
			GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,0);
	}
	else {
		DrawQuad(render_target,0.0f,0.0f,
			1.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1,
			GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,0);
	}
	
	render_target->backbuffer_copy(512);
	glViewport(0,0,width,height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	//calculate effect time
	i=0;
	offset=0;
	while((i<lengths.length-1) && (offset+lengths[i]<time)) {
		offset+=lengths[i];
		i++;
	}
	x1=offset;
	x2=offset+lengths[i];
	y1=start_times[i];
	y2=end_times[i];
	effect_time=(y1-y2)/(x1-x2)*time+(x1*y2-x2*y1)/(x1-x2);

	effects[i]->render(effect_time);

	glColor4f(1.0f,1.0f,1.0f,alpha_val);
	if(height==480) {
		DrawQuad(render_target,0.0f,0.0f,
			1.0f,1.0f,0.0f,0.9375f,1.0f,0.0f,1,
			GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,0);
	}
	else {
		DrawQuad(render_target,0.0f,0.0f,
			1.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1,
			GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,0);
	}
	last_render=time;*/
}

Mekka_SceneGlow::Mekka_SceneGlow() {
	glow_target=new Scene3D("scenes/rura.lws",100.0f);
	background=new Scene3D("scenes/wnetrze.lws",100.0f);
	char *tmp;
	tmp=new char[256*256*4];
	render_target=new Texture(256,256,tmp);
	delete tmp;
}

Mekka_SceneGlow::~Mekka_SceneGlow() {
	delete glow_target;
	delete background;
	delete render_target;
}

void Mekka_SceneGlow::render(int time) {
	glViewport(0,0,256,256);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glColorMask(0,0,0,0);
	background->render(time);
	glColorMask(1,1,1,1);
	glow_target->render(time);
	render_target->backbuffer_copy(256);
	glViewport(0,0,width,height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilOp(GL_INCR,GL_INCR,GL_INCR);
	c_state->want_enabled(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS,0,0xFFFF);
	glColorMask(0,0,0,0);
	glow_target->render(time);
	glColorMask(1,1,1,1);
	glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
	background->render(time);
	glow_target->render(time);
	glStencilFunc(GL_EQUAL,0,0xFFFF);
	int i;
//	float fraction;
	glColor4f(1.0f,1.0f,1.0f,0.3);

	//normal
	for(i=0;i<6;i++) {
		DrawQuad(render_target,0.0f-0.005f*i,0.0f-0.005f*i,
			1.0f+0.005f*i,1.0f+0.005f*i,0.0f,1.0f,1.0f,0.0f,1,
			GL_SRC_ALPHA,GL_ONE,0);
	}

	c_state->want_disabled(GL_STENCIL_TEST);
}

Mekka_SineDistort::Mekka_SineDistort(char *texture,int subdivisions) {
	size=subdivisions;
	vert_coords=new float[3*(size+1)*(size+1)];
	tex_coords=new float[2*(size+1)*(size+1)];
	colors=new float[4*(size+1)*(size+1)];
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
		tex_coords[2*i+1]=1.0f-vert_coords[3*i+1];
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
		agp_colors=(float*)agp_alloc->get_agp_block(4*(size+1)*(size+1)*sizeof(float));
		if(!agp_colors) {
			is_optimized=0;
			return;
		}
	}
	tex=tex_manager.get_texture(texture);
}

Mekka_SineDistort::~Mekka_SineDistort() {
	if(vert_coords) delete vert_coords;
	if(colors) delete colors;
	if(tex_coords) delete tex_coords;
}

void Mekka_SineDistort::render(int time) {
	int i;
	//update texcoords
	for(i=0;i<(size+1)*(size+1);i++) {
		vert_coords[3*i+2]=-0.2f-0.01f;
	}
	//distort texcoords
	for(i=0;i<(size+1)*(size+1);i++) {
		vert_coords[3*i+2]+=0.1f*(FastCos(vert_coords[3*i+1]*4.0f+0.0008f*time)*
			FastCos(vert_coords[3*i]*4.78f+0.0006f*time)*
			FastCos(vert_coords[3*i+1]*9.21f+0.0007f*time));
	}
	//update colors
	float coef;
	for(i=0;i<(size+1)*(size+1);i++) {
		coef=(-0.1f-vert_coords[3*i+2])*7.0f;
		colors[4*i]=1.0f;
		colors[4*i+1]=1.0f;
		colors[4*i+2]=1.0f;
		colors[4*i+3]=1.0f-coef;
	}
	if(is_optimized) {
		memcpy((void*)agp_vert_coords,(void*)vert_coords,3*(size+1)*(size+1)*sizeof(float));
		memcpy((void*)agp_colors,(void*)colors,4*(size+1)*(size+1)*sizeof(float));
	}
	c_state->want_disabled(GL_LIGHTING);
	c_state->want_disabled(GL_FOG);
	c_state->want_disabled(GL_DEPTH_TEST);
	c_state->want_disabled(GL_ALPHA_TEST);
//	c_state->want_disabled(GL_BLEND);
	float x_size=0.266f;
	float y_size=0.2f;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glFrustum(-0.133,0.133,-0.1,0.1,0.1,2.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
//	glScalef(0.532f,0.4f,1.0f);
	glScalef(0.8f,0.6f,1.0f);
	glTranslatef(-0.5f,-0.5f,0.0f);

	for(i=0;i<(int)max_texture_units;i++) {
		c_state->textures[i].clear_tex_gen_flags();
		c_state->textures[i].enabled=0;
	}
	c_state->textures[0].enabled=1;
	c_state->textures[0].tex=tex;
	c_state->textures[0].client_state_enabled=1;
	c_state->textures[0].set_texture_env(GL_MODULATE);
	if(is_optimized) {
		c_state->textures[0].coord_pointer=agp_tex_coords;
		glVertexPointer(3,GL_FLOAT,0,agp_vert_coords);
		glColorPointer(4,GL_FLOAT,0,agp_colors);
	}
	else {
		c_state->textures[0].coord_pointer=tex_coords;
		glVertexPointer(3,GL_FLOAT,0,vert_coords);
		glColorPointer(4,GL_FLOAT,0,colors);
	}
	c_state->prerender_setup();
	if((!is_optimized) && hasVertexArrayRange) {
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	glDisableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawElements(GL_TRIANGLE_STRIP,2*(size+2)*size,GL_UNSIGNED_SHORT,indices);
	glDisableClientState(GL_COLOR_ARRAY);
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

Mekka_UVDistort::Mekka_UVDistort(Texture *texture,int divisions) {
	tex=texture;
	div_num=divisions;
	vertex_buffer=new float[3*(div_num+3)*(div_num+3)+3*2*3*(div_num+3)];
	//3*2*3 = num_strips * vert_rows * components
	texcoord_buffer=new float[2*(div_num+3)*(div_num+3)+3*2*2*(div_num+3)];

	int i,j;
	for(i=0;i<div_num+3;i++) {
		for(j=0;j<div_num+3;j++) {
			vertex_buffer[3*(i*(div_num+3)+j)]=-0.266f+j*(2*0.266f)/(div_num+1);
			vertex_buffer[3*(i*(div_num+3)+j)+1]=0.2f-i*(2*0.2f)/(div_num+1);
			vertex_buffer[3*(i*(div_num+3)+j)+2]=-0.2f;

			texcoord_buffer[2*(i*(div_num+3)+j)]=j/(float)(div_num+1);
			texcoord_buffer[2*(i*(div_num+3)+j)+1]=i/(float)(div_num+1);

		}
	}
	for(i=2*(div_num+3)*(div_num+3);i<(2*(div_num+3)*(div_num+3)+3*2*2*(div_num+3));i++) {
		texcoord_buffer[i]=0.0f;
	}
	for(i=3*(div_num+3)*(div_num+3);i<(3*(div_num+3)*(div_num+3)+3*2*3*(div_num+3));i++) {
		vertex_buffer[i]=0.0f;
	}
	index_buffer=new unsigned short[(div_num+2)*(div_num+3)*2+3*2*(div_num+3)];  
	//num_strips*vert_per_strip + 3 extra strips

	//fill index buffer
	for(i=0;i<(div_num+2);i++) { //for each strip
		//start the tri strip
		index_buffer[i*(div_num+3)*2]=(i*(div_num+3));
		for(j=0;j<div_num+2;j++) {
			index_buffer[1+i*(div_num+3)*2+2*j]=((i+1)*(div_num+3)+j);
			index_buffer[1+i*(div_num+3)*2+2*j+1]=(i*(div_num+3)+j+1);
		}
		//end the strip
		index_buffer[1+i*(div_num+3)*2+2*(div_num+2)]=((i+1)*(div_num+3)+div_num+2);
	}
	//set up indices for 3 extra strips
	for(i=0;i<3;i++) { //for each strip
		//start the tri strip
		index_buffer[(i+div_num+2)*(div_num+3)*2]=
			(div_num+3)*(div_num+3)+((2*i)*(div_num+3));
		for(j=0;j<div_num+2;j++) {
			index_buffer[1+(i+div_num+2)*(div_num+3)*2+2*j]=
				(div_num+3)*(div_num+3)+((2*i+1)*(div_num+3)+j);
			index_buffer[1+(i+div_num+2)*(div_num+3)*2+2*j+1]=
				(div_num+3)*(div_num+3)+((2*i)*(div_num+3)+j+1);
		}
		//end the strip
		index_buffer[1+(i+div_num+2)*(div_num+3)*2+2*(div_num+2)]=
			(div_num+3)*(div_num+3)+((2*i+1)*(div_num+3)+div_num+2);
	}

	//try to optimize the buffers
	is_optimized=0;
	if(hasVertexArrayRange) {
		is_optimized=1;
		agp_vertex_buffer=(float*)agp_alloc->
			get_agp_block((3*(div_num+3)*(div_num+3)+3*2*3*(div_num+3))*sizeof(float));
		if(!agp_vertex_buffer) {
			is_optimized=0;
			return;
		}
		memcpy((void*)agp_vertex_buffer,(void*)vertex_buffer,
			(3*(div_num+3)*(div_num+3)+3*2*3*(div_num+3))*sizeof(float));
		agp_texcoord_buffer=(float*)agp_alloc->
			get_agp_block((2*(div_num+3)*(div_num+3)+3*2*2*(div_num+3))*sizeof(float));
		if(!agp_texcoord_buffer) {
			is_optimized=0;
			return;
		}
		memcpy((void*)agp_texcoord_buffer,(void*)texcoord_buffer,
			(2*(div_num+3)*(div_num+3)+3*2*2*(div_num+3))*sizeof(float));
	}
}

Mekka_UVDistort::~Mekka_UVDistort() {
	if(vertex_buffer) delete vertex_buffer;
	if(index_buffer) delete index_buffer;
	if(texcoord_buffer) delete texcoord_buffer;
}

//found somewhere on the net - faster than original atan2
static float arctan2(float y, float x)
{
   const float coeff_1 = 0.25*3.14159;
   const float coeff_2 = 0.75*3.14159;

   float abs_y = fabs(y)+1e-10;      // kludge to prevent 0/0 condition
   float angle,r;

   if (x>=0)
   {
      r = (x - abs_y) / (x + abs_y);
      angle = coeff_1 - coeff_1 * r;
   }
   else
   {
      r = (x + abs_y) / (abs_y - x);
      angle = coeff_2 - coeff_1 * r;
   }

   if (y < 0)
   return(-angle);     // negate if in quad III or IV
   else
   return(angle);
}

void Mekka_UVDistort::update(float frame) {
	float time=frame*0.033333333f;
	float p1_x,p1_y;
	float p2_x,p2_y;
	float p3_x,p3_y;
	p1_x=0.266*sin(6.28/17*time);
	p2_x=0.266*sin(6.28f*0.333f+6.28/17*time);
	p3_x=0.266*sin(6.28f*0.666f+6.28/17*time);

	p1_y=0.2*cos(6.28/17*time);
	p2_y=0.2*cos(6.28f*0.333f+6.28/17*time);
	p3_y=0.2*cos(6.28f*0.666f+6.28/17*time);

	static float sp1_y=0.2*sin(0.0);
	static float sp2_y=0.2*sin(6.28f*0.333f);
	static float sp3_y=0.2*sin(6.28f*0.666f);

	int i,j;
	float scale=0.01f;
	float px,py;
	for(i=0;i<(div_num+3);i++) {
		for(j=0;j<(div_num+3);j++) {
			px=vertex_buffer[3*(i*(div_num+3)+j)];
			py=vertex_buffer[3*(i*(div_num+3)+j)+1];
			texcoord_buffer[2*(i*(div_num+3)+j)]=scale*
				(1.0f/((px-p1_x)*(px-p1_x)+
				(py-p1_y)*(py-p1_y))+
				1.0f/((px-p2_x)*(px-p2_x)+
				(py-p2_y)*(py-p2_y))+
				1.0f/((px-p3_x)*(px-p3_x)+
				(py-p3_y)*(py-p3_y)));
			texcoord_buffer[2*(i*(div_num+3)+j)+1]=
				(arctan2(py-p1_y,px-p1_x)+
				arctan2(py-p2_y,px-p2_x)+
				arctan2(py-p3_y,px-p3_x))/3.14159f;
		}
	}
	const float y_size=0.2f;
	const float x_size=0.266f;
	int x_center;

	//POINT 1
	//refresh the indices that were invalid in previous frame
	i=(int)((y_size-sp1_y)*(div_num+1)/(2.0f*y_size));
	//start the tri strip
	index_buffer[i*(div_num+3)*2]=(i*(div_num+3));
	for(j=0;j<div_num+2;j++) {
		index_buffer[1+i*(div_num+3)*2+2*j]=((i+1)*(div_num+3)+j);
		index_buffer[1+i*(div_num+3)*2+2*j+1]=(i*(div_num+3)+j+1);
	}
	//end the strip
	index_buffer[1+i*(div_num+3)*2+2*(div_num+2)]=((i+1)*(div_num+3)+div_num+2);

	//clear indices for the wrong strip
	i=(int)((y_size-p1_y)*(div_num+1)/(2.0f*y_size));
	//start the tri strip
	index_buffer[i*(div_num+3)*2]=0;
	for(j=0;j<div_num+2;j++) {
		index_buffer[1+i*(div_num+3)*2+2*j]=0;
		index_buffer[1+i*(div_num+3)*2+2*j+1]=0;
	}
	//end the strip
	index_buffer[1+i*(div_num+3)*2+2*(div_num+2)]=0;
	sp1_y=p1_y;
	//fill the additional vertex buffer space
	x_center=div_num-(int)((x_size-p1_x)*(div_num+1)/(2.0f*x_size));
	for(j=0;j<div_num+3;j++) {
		vertex_buffer[3*(div_num+3)*(div_num+3)+3*j]=
			vertex_buffer[3*(i*(div_num+3)+j)];
		vertex_buffer[3*(div_num+3)*(div_num+3)+3*j+1]=
			vertex_buffer[3*(i*(div_num+3)+j)+1];
		vertex_buffer[3*(div_num+3)*(div_num+3)+3*j+2]=
			vertex_buffer[3*(i*(div_num+3)+j)+2];
		
		texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j]=
			texcoord_buffer[2*(i*(div_num+3)+j)];
		texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+1]=
			texcoord_buffer[2*(i*(div_num+3)+j)+1];
		if(j<x_center) texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+1]-=2.0f;

		vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)]=
			vertex_buffer[3*((i+1)*(div_num+3)+j)];
		vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)+1]=
			vertex_buffer[3*((i+1)*(div_num+3)+j)+1];
		vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)+2]=
			vertex_buffer[3*((i+1)*(div_num+3)+j)+2];
		
		texcoord_buffer[2*(div_num+3)*(div_num+3)+2*(j+div_num+3)]=
			texcoord_buffer[2*((i+1)*(div_num+3)+j)];
		texcoord_buffer[2*(div_num+3)*(div_num+3)+2*(j+div_num+3)+1]=
			texcoord_buffer[2*((i+1)*(div_num+3)+j)+1];
	}
	int pos1=i;
	int pos2=-1;

	
	//POINT 2
	//refresh the indices that were invalid in previous frame
	i=(int)((y_size-sp2_y)*(div_num+1)/(2.0f*y_size));
	if(i!=pos1) {
		//start the tri strip
		index_buffer[i*(div_num+3)*2]=(i*(div_num+3));
		for(j=0;j<div_num+2;j++) {
			index_buffer[1+i*(div_num+3)*2+2*j]=((i+1)*(div_num+3)+j);
			index_buffer[1+i*(div_num+3)*2+2*j+1]=(i*(div_num+3)+j+1);
		}
		//end the strip
		index_buffer[1+i*(div_num+3)*2+2*(div_num+2)]=((i+1)*(div_num+3)+div_num+2);
	}
	//clear indices for the wrong strip
	i=(int)((y_size-p2_y)*(div_num+1)/(2.0f*y_size));
	//start the tri strip
	index_buffer[i*(div_num+3)*2]=0;
	for(j=0;j<div_num+2;j++) {
		index_buffer[1+i*(div_num+3)*2+2*j]=0;
		index_buffer[1+i*(div_num+3)*2+2*j+1]=0;
	}
	//end the strip
	index_buffer[1+i*(div_num+3)*2+2*(div_num+2)]=0;
	sp2_y=p2_y;
	x_center=div_num-(int)((x_size-p2_x)*(div_num+1)/(2.0f*x_size));
	if(i!=pos1) {
		//fill the additional vertex buffer space
		for(j=0;j<div_num+3;j++) {
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*j+3*2*(div_num+3)]=
				vertex_buffer[3*(i*(div_num+3)+j)];
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*j+1+3*2*(div_num+3)]=
				vertex_buffer[3*(i*(div_num+3)+j)+1];
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*j+2+3*2*(div_num+3)]=
				vertex_buffer[3*(i*(div_num+3)+j)+2];
			
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+2*2*(div_num+3)]=
				texcoord_buffer[2*(i*(div_num+3)+j)];
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+1+2*2*(div_num+3)]=
				texcoord_buffer[2*(i*(div_num+3)+j)+1];
			if(j<x_center) texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+1
				+2*2*(div_num+3)]-=2.0f;
			
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)+3*2*(div_num+3)]=
				vertex_buffer[3*((i+1)*(div_num+3)+j)];
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)+1+3*2*(div_num+3)]=
				vertex_buffer[3*((i+1)*(div_num+3)+j)+1];
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)+2+3*2*(div_num+3)]=
				vertex_buffer[3*((i+1)*(div_num+3)+j)+2];
			
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*(j+div_num+3)+2*2*(div_num+3)]=
				texcoord_buffer[2*((i+1)*(div_num+3)+j)];
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*(j+div_num+3)+1+2*2*(div_num+3)]=
				texcoord_buffer[2*((i+1)*(div_num+3)+j)+1];
		}
		pos2=i;
	}
	else {
		for(j=0;j<div_num+3;j++) {
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*j+3*2*(div_num+3)]=0.0f;
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*j+1+3*2*(div_num+3)]=0.0f;
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*j+2+3*2*(div_num+3)]=0.0f;
			
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+2*2*(div_num+3)]=0.0f;
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+1+2*2*(div_num+3)]=0.0f;
			
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)+3*2*(div_num+3)]=0.0f;
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)+1+3*2*(div_num+3)]=0.0f;
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)+2+3*2*(div_num+3)]=0.0f;
			
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*(j+div_num+3)+2*2*(div_num+3)]=0.0f;
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*(j+div_num+3)+1+2*2*(div_num+3)]=0.0f;
		}
		for(j=0;j<div_num+3;j++) {
			if(j<x_center) texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+1]-=2.0f;
		}
	}
	
	//POINT 3
	//refresh the indices that were invalid in previous frame
	i=(int)((y_size-sp3_y)*(div_num+1)/(2.0f*y_size));
	if((i!=pos1) && (i!=pos2)) {
		//start the tri strip
		index_buffer[i*(div_num+3)*2]=(i*(div_num+3));
		for(j=0;j<div_num+2;j++) {
			index_buffer[1+i*(div_num+3)*2+2*j]=((i+1)*(div_num+3)+j);
			index_buffer[1+i*(div_num+3)*2+2*j+1]=(i*(div_num+3)+j+1);
		}
		//end the strip
		index_buffer[1+i*(div_num+3)*2+2*(div_num+2)]=((i+1)*(div_num+3)+div_num+2);
	}
	//clear indices for the wrong strip
	i=(int)((y_size-p3_y)*(div_num+1)/(2.0f*y_size));
	//start the tri strip
	index_buffer[i*(div_num+3)*2]=0;
	for(j=0;j<div_num+2;j++) {
		index_buffer[1+i*(div_num+3)*2+2*j]=0;
		index_buffer[1+i*(div_num+3)*2+2*j+1]=0;
	}
	//end the strip
	index_buffer[1+i*(div_num+3)*2+2*(div_num+2)]=0;
	sp3_y=p3_y;
	x_center=div_num-(int)((x_size-p3_x)*(div_num+1)/(2.0f*x_size));
	if((i!=pos1) && (i!=pos2)) {
		//fill the additional vertex buffer space
		for(j=0;j<div_num+3;j++) {
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*j+3*4*(div_num+3)]=
				vertex_buffer[3*(i*(div_num+3)+j)];
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*j+1+3*4*(div_num+3)]=
				vertex_buffer[3*(i*(div_num+3)+j)+1];
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*j+2+3*4*(div_num+3)]=
				vertex_buffer[3*(i*(div_num+3)+j)+2];
			
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+2*4*(div_num+3)]=
				texcoord_buffer[2*(i*(div_num+3)+j)];
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+1+2*4*(div_num+3)]=
				texcoord_buffer[2*(i*(div_num+3)+j)+1];
			if(j<x_center) texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+1
				+2*4*(div_num+3)]-=2.0f;
			
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)+3*4*(div_num+3)]=
				vertex_buffer[3*((i+1)*(div_num+3)+j)];
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)+1+3*4*(div_num+3)]=
				vertex_buffer[3*((i+1)*(div_num+3)+j)+1];
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)+2+3*4*(div_num+3)]=
				vertex_buffer[3*((i+1)*(div_num+3)+j)+2];
			
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*(j+div_num+3)+2*4*(div_num+3)]=
				texcoord_buffer[2*((i+1)*(div_num+3)+j)];
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*(j+div_num+3)+1+2*4*(div_num+3)]=
				texcoord_buffer[2*((i+1)*(div_num+3)+j)+1];
		}
	}
	else {
		for(j=0;j<div_num+3;j++) {
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*j+3*4*(div_num+3)]=0.0f;
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*j+1+3*4*(div_num+3)]=0.0f;
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*j+2+3*4*(div_num+3)]=0.0f;
			
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+2*4*(div_num+3)]=0.0f;
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+1+2*4*(div_num+3)]=0.0f;
			
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)+3*4*(div_num+3)]=0.0f;
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)+1+3*4*(div_num+3)]=0.0f;
			vertex_buffer[3*(div_num+3)*(div_num+3)+3*(j+div_num+3)+2+3*4*(div_num+3)]=0.0f;
			
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*(j+div_num+3)+2*4*(div_num+3)]=0.0f;
			texcoord_buffer[2*(div_num+3)*(div_num+3)+2*(j+div_num+3)+1+2*4*(div_num+3)]=0.0f;
		}
		if(i==pos1) {
			for(j=0;j<div_num+3;j++) {
				if(j<x_center) texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+1]-=2.0f;
			}
		}
		if(i==pos2) {
			for(j=0;j<div_num+3;j++) {
				if(j<x_center) texcoord_buffer[2*(div_num+3)*(div_num+3)+2*j+1
					+2*2*(div_num+3)]-=2.0f;
			}
		}
	}
	if(is_optimized) {
		memcpy((void*)agp_vertex_buffer,(void*)vertex_buffer,
			(3*(div_num+3)*(div_num+3)+3*2*3*(div_num+3))*sizeof(float));
		memcpy((void*)agp_texcoord_buffer,(void*)texcoord_buffer,
			(2*(div_num+3)*(div_num+3)+3*2*2*(div_num+3))*sizeof(float));
	}
}

void Mekka_UVDistort::render() {
	int i;
	unsigned int ui;
	for(ui=0;ui<max_texture_units;ui++) {
		c_state->textures[ui].enabled=0;
		c_state->textures[ui].client_state_enabled=0;
		c_state->textures[ui].clear_tex_gen_flags();
	}
	c_state->textures[0].enabled=1;
	if(!is_optimized) {
		c_state->textures[0].coord_pointer=texcoord_buffer;
	}
	else {
		c_state->textures[0].coord_pointer=agp_texcoord_buffer;
	}
	c_state->textures[0].client_state_enabled=1;
	c_state->textures[0].tex=tex;
	c_state->textures[0].set_texture_env(GL_MODULATE);
	c_state->prerender_setup();
	c_state->want_disabled(GL_DEPTH_TEST);
	c_state->want_enabled(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	c_state->want_disabled(GL_LIGHTING);
	c_state->want_enabled(GL_CULL_FACE);

	if(!is_optimized) {
		glVertexPointer(3,GL_FLOAT,0,vertex_buffer);
	}
	else {
		glVertexPointer(3,GL_FLOAT,0,agp_vertex_buffer);
	}
	glDisableClientState(GL_NORMAL_ARRAY);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glFrustum(-0.133,0.133,-0.1,0.1,0.1,2.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if(hasVertexArrayRange && (!is_optimized)) {
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	if(hasCompiledVertexArray) {
		glLockArraysEXT(0,(div_num+3)*(div_num+3)+2*3*(div_num+3));
	}
	for(i=0;i<(div_num+2+3);i++) {
		glDrawElements(GL_TRIANGLE_STRIP,2*(div_num+3),
			GL_UNSIGNED_SHORT,&index_buffer[2*(div_num+3)*i]);
	}

	if(hasCompiledVertexArray) {
		glUnlockArraysEXT();
	}
	if(hasVertexArrayRange && (!is_optimized)) {
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	glEnableClientState(GL_NORMAL_ARRAY);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}


Mekka_2DEffect1::Mekka_2DEffect1() {
	uvd1=new Mekka_SineDistort("textures/bloby_srodek2.jpg");
	uvd2=new Mekka_SineDistort("textures/bloby_srodek2.jpg");
	uvd3=new Mekka_SineDistort("textures/bloby_srodek2.jpg");
	uvd4=new Mekka_SineDistort("textures/bloby_srodek2.jpg");
	rings=new Mekka_UVDistort(tex_manager.get_texture("textures/bloby_srodek2.jpg"),50);
}

Mekka_SineDistort2::Mekka_SineDistort2(char *texture,int subdivisions) {
	size=subdivisions;
	vert_coords=new float[3*(size+1)*(size+1)];
	tex_coords=new float[2*(size+1)*(size+1)];
	colors=new float[4*(size+1)*(size+1)];
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
		tex_coords[2*i+1]=1.0f-vert_coords[3*i+1];
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
		agp_colors=(float*)agp_alloc->get_agp_block(4*(size+1)*(size+1)*sizeof(float));
		if(!agp_colors) {
			is_optimized=0;
			return;
		}
	}
	tex=tex_manager.get_texture(texture);
}

Mekka_SineDistort2::~Mekka_SineDistort2() {
	if(vert_coords) delete vert_coords;
	if(colors) delete colors;
	if(tex_coords) delete tex_coords;
}

void Mekka_SineDistort2::render(int time) {
	if(time>11000) return;
	int i;
	//update texcoords
	for(i=0;i<(size+1)*(size+1);i++) {
		vert_coords[3*i+2]=-0.2f-0.01f;
	}
	//distort texcoords
	for(i=0;i<(size+1)*(size+1);i++) {
		vert_coords[3*i+2]+=0.015f*(FastCos(vert_coords[3*i+1]*4.0f+0.001f*time)*
			FastCos(vert_coords[3*i]*4.78f+0.0016f*time)*
			FastCos(vert_coords[3*i+1]*9.21f+0.0009f*time));
	}
	//update colors
//	float coef;
	for(i=0;i<(size+1)*(size+1);i++) {
		colors[4*i]=1.0f;
		colors[4*i+1]=1.0f;
		colors[4*i+2]=1.0f;
		if(time<8000) {
			colors[4*i+3]=1.0f;
		}
		else {
			colors[4*i+3]=1.0f-(time-8000)/3000.0f;
		}
	}
	if(is_optimized) {
		memcpy((void*)agp_vert_coords,(void*)vert_coords,3*(size+1)*(size+1)*sizeof(float));
		memcpy((void*)agp_colors,(void*)colors,4*(size+1)*(size+1)*sizeof(float));
	}
	c_state->want_disabled(GL_LIGHTING);
	c_state->want_disabled(GL_FOG);
	c_state->want_disabled(GL_DEPTH_TEST);
	c_state->want_disabled(GL_ALPHA_TEST);
	c_state->want_enabled(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	float x_size=0.266f;
	float y_size=0.2f;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glFrustum(-0.133,0.133,-0.1,0.1,0.1,2.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	if(time<8000) {
		glScalef(0.60f,0.45f,1.0f);
	}
	else {
		glScalef(0.60f+0.8f*(time-8000)/3000,0.45f+0.6f*(time-8000)/3000,1.0f);
	}
	glTranslatef(-0.5f,-0.5f,0.0f);

	for(i=0;i<(int)max_texture_units;i++) {
		c_state->textures[i].clear_tex_gen_flags();
		c_state->textures[i].enabled=0;
	}
	c_state->textures[0].enabled=1;
	c_state->textures[0].tex=tex;
	c_state->textures[0].client_state_enabled=1;
	c_state->textures[0].set_texture_env(GL_MODULATE);
	if(is_optimized) {
		c_state->textures[0].coord_pointer=agp_tex_coords;
		glVertexPointer(3,GL_FLOAT,0,agp_vert_coords);
		glColorPointer(4,GL_FLOAT,0,agp_colors);
	}
	else {
		c_state->textures[0].coord_pointer=tex_coords;
		glVertexPointer(3,GL_FLOAT,0,vert_coords);
		glColorPointer(4,GL_FLOAT,0,colors);
	}
	c_state->prerender_setup();
	if((!is_optimized) && hasVertexArrayRange) {
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	glDisableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawElements(GL_TRIANGLE_STRIP,2*(size+2)*size,GL_UNSIGNED_SHORT,indices);
	glDisableClientState(GL_COLOR_ARRAY);
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



Mekka_FinalScene::Mekka_FinalScene() {
	scn=new Scene3D("scenes/final_height.lws");
	if(hasRegisterCombiners && hasCubeMaps) {
		background=new STXYQuad(12.0f,12.0f,ST_RM_DOT3,10);
		background->set_bump_map("textures/detail.jpg",3.0f,3.0f);
		background->set_diffuse_color(1.0f,1.0f,1.0f);
	}
	else if(hasTextureEnvDot3 && hasCubeMaps) {
		background=new STXYQuad(12.0f,12.0f,ST_RM_DOT3_ARB,10);
		background->set_bump_map("textures/detail.jpg",3.0f,3.0f);
		background->set_diffuse_color(1.0f,1.0f,1.0f);
	}
	else {
		background=new STXYQuad(12.0f,12.0f,ST_RM_BASE_TEXTURE,10);
	}
	background->set_base_texture("textures/szachownica.jpg",15.0f,15.0f);
	cool_writings[0]=load_image("textures/we.jpg");
	cool_writings[0]->convert();
	if(hasRegisterCombiners && hasCubeMaps) {
		hmap=new STHeightMap(cool_writings[0],4.0f,4.0f,ST_RM_DOT3);
		hmap->set_bump_map("textures/detail.jpg",1.0f,1.0f);
		hmap->set_diffuse_color(1.0f,1.0f,1.0f);
	}
	else if(hasTextureEnvDot3 && hasCubeMaps) {
		hmap=new STHeightMap(cool_writings[0],4.0f,4.0f,ST_RM_DOT3_ARB);
		hmap->set_bump_map("textures/detail.jpg",1.0f,1.0f);
		hmap->set_diffuse_color(1.0f,1.0f,1.0f);
	}
	else {
		hmap=new STHeightMap(cool_writings[0],4.0f,4.0f);
	}
	hmap->set_base_texture("textures/szachownica.jpg",5.0f,5.0f);
	cool_writings[1]=load_image("textures/love.jpg");
	cool_writings[1]->convert();
	cool_writings[2]=load_image("textures/you.jpg");
	cool_writings[2]->convert();
}

Mekka_FinalScene::~Mekka_FinalScene() {
	delete scn;
	delete cool_writings[0];
	delete cool_writings[1];
	delete cool_writings[2];
	delete hmap;
	delete background;
}

void Mekka_FinalScene::render(int time) {
	hmap->set_hmap_strength(0.5f);
	if(time<4000) {
		hmap->set_hmap(cool_writings[0]);
		hmap->set_hmap_strength(0.4f*FastSin(time/8000.0f*6.28f));
	}
	else if(time<10000) {
		hmap->set_hmap(cool_writings[1]);
		hmap->set_hmap_strength(0.8f*FastSin((time-4000)/12000.0f*6.28f));
	}
	else {
		hmap->set_hmap(cool_writings[2]);
		hmap->set_hmap_strength(0.67f*FastSin((time-10000)/16000.0f*6.28f));
	}
	float white_col[]={1.0f,1.0f,1.0f,1.0f};
	float black_col[]={0.0f,0.0f,0.0f,0.0f};
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,white_col);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,white_col);

	//hmap	
	if((hasRegisterCombiners && hasCubeMaps)
		|| (hasTextureEnvDot3 && hasCubeMaps)) {
		Vector3 cam_pos;
		scn->get_camera()->get_position(cam_pos);
		Matrix mx;
		Vector3 pos;
		pos[0]=-2.0f;
		pos[1]=0.0f;
		pos[2]=-2.0f;
		MTranslation(mx,pos);
		hmap->set_eye_pos_indirect(mx,cam_pos[0],cam_pos[1],cam_pos[2]);
		hmap->set_light_pos_indirect(mx,0.0f,5.0f,0.0f);
	}
	hmap->update(time);
	scn->render(time);
	glPushMatrix();
	glTranslatef(-2.0f,0.0f,-2.0f);
	c_state->want_disabled(GL_BLEND);
	c_state->want_disabled(GL_ALPHA_TEST);
	hmap->render();
	glPopMatrix();

	//background
	if((hasRegisterCombiners && hasCubeMaps)
		|| (hasTextureEnvDot3 && hasCubeMaps)) {
		Vector3 cam_pos;
		scn->get_camera()->get_position(cam_pos);
		Matrix mx;
		Vector3 pos;
		pos[0]=-6.0f;
		pos[1]=0.0f;
		pos[2]=-6.0f;
		MTranslation(mx,pos);
		background->set_eye_pos_indirect(mx,cam_pos[0],cam_pos[1],cam_pos[2]);
		background->set_light_pos_indirect(mx,0.0f,5.0f,0.0f);
	}
	glPushMatrix();
	glTranslatef(-6.0f,-0.001f,-6.0f);
	c_state->want_disabled(GL_BLEND);
	c_state->want_disabled(GL_ALPHA_TEST);
	background->update(time);
	background->render();
	glPopMatrix();
}


Mekka_2DEffect1::~Mekka_2DEffect1() {
	delete uvd1;
	delete uvd2;
	delete uvd3;
	delete uvd4;
	delete rings;
}

void Mekka_2DEffect1::render(int time) {
	c_state->want_enabled(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	uvd1->render(time);
	uvd2->render(time+1000);
	uvd3->render(time+2000);
	uvd4->render(time+3000);
	float tmp_col[4]={1.0f,1.0f,1.0f,1.0f};
	tmp_col[3]=0.5f*FastSin(6.28f*time/12000);
	glColor4fv(tmp_col);
	rings->update(time*0.03333);
	rings->render();
}

//experimental torus scene
Mekka_BlurredTorus::Mekka_BlurredTorus() {
	scn=new Scene3D("scenes/torusik.lws");
	background=new Scene3D("scenes/skybox.lws");
}

Mekka_BlurredTorus::~Mekka_BlurredTorus() {
	delete scn;
}

void Mekka_BlurredTorus::render(int time) {
	int i;
	background->render(time);
	c_state->want_disabled(GL_CULL_FACE);
	c_state->lock_state(GL_CULL_FACE);
	for(i=0;i<4;i++) {
		scn->render(time+190*i);
	}
	c_state->unlock_state(GL_CULL_FACE);
}
