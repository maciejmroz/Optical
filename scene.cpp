/*****************************************************************************
 Scene3D & helper classes implementation
 (c) 2000 Maciej Mroz
*****************************************************************************/

#include "engine.h"
#include "debug_msg.h"

static const float deg2rad=3.14159f/180.0f;
unsigned int max_lights=8;
float scn_default_size;
int use_display_lists=0;
int immediate_mode=0;
Scene3D *current_scene;
MovingObject3D *current_object;
ObjectManager obj_manager;

ObjectManager::ObjectManager() {
}

ObjectManager::~ObjectManager() {
	int i;
	for(i=0;i<static_objects.length;i++) delete static_objects[i];
	for(i=0;i<dynamic_objects.length;i++) delete dynamic_objects[i];
}

Object3D* ObjectManager::get_object_dynamic(const char* filename) {
	dynamic_objects.add_element(new Object3D(filename));
	return dynamic_objects[dynamic_objects.length-1];
}

Object3D* ObjectManager::get_object_static(const char* filename) {
	int i;
	for(i=0;i<static_objects.length;i++) {
		if(!strcmp(filename,static_objects[i]->name)) return static_objects[i]; 
	}
	static_objects.add_element(new Object3D(filename));
	static_objects[static_objects.length-1]->optimize();
	return static_objects[static_objects.length-1];
}


//default bounding box for procedural object - of course most
//descendant classes will overload this function
void ProceduralObject::get_bounding_box(BBox *bx) {
	bx->p_min[0]=-1000.0f;
	bx->p_min[1]=-1000.0f;
	bx->p_min[2]=-1000.0f;

	bx->p_max[0]=1000.0f;
	bx->p_max[1]=1000.0f;
	bx->p_max[2]=1000.0f;

	bx->p1[0]=bx->p_min[0];
	bx->p1[1]=bx->p_min[1];
	bx->p1[2]=bx->p_min[2];
	
	bx->p2[0]=bx->p_min[0];
	bx->p2[1]=bx->p_min[1];
	bx->p2[2]=bx->p_max[2];
	
	bx->p3[0]=bx->p_min[0];
	bx->p3[1]=bx->p_max[1];
	bx->p3[2]=bx->p_min[2];
	
	bx->p4[0]=bx->p_min[0];
	bx->p4[1]=bx->p_max[1];
	bx->p4[2]=bx->p_max[2];
	
	bx->p5[0]=bx->p_max[0];
	bx->p5[1]=bx->p_min[1];
	bx->p5[2]=bx->p_min[2];
	
	bx->p6[0]=bx->p_max[0];
	bx->p6[1]=bx->p_min[1];
	bx->p6[2]=bx->p_max[2];
	
	bx->p7[0]=bx->p_max[0];
	bx->p7[1]=bx->p_max[1];
	bx->p7[2]=bx->p_min[2];
	
	bx->p8[0]=bx->p_max[0];
	bx->p8[1]=bx->p_max[1];
	bx->p8[2]=bx->p_max[2];
}

//Bone class implementation
Bone::Bone() {
	active=0;
	name=NULL;
	parent_bone=0;
	rest_direction[0]=rest_direction[1]=rest_direction[2]=0.0f;
	rest_position[0]=rest_position[1]=rest_position[2]=0.0f;
	rest_length=0.0f;
	MIdentity(rest_matrix);
	MIdentity(transform_mtx);
	weights=NULL;
	strength=0.0f;
	scale_strength=0;
	num_weights=0;
}

Bone::Bone(const Bone& bn) {
	active=bn.active;
	if(bn.name) {
		name=new char[strlen(bn.name)+1];
		strcpy(name,bn.name);
	}
	else {
		name=NULL;
	}
	parent_bone=bn.parent_bone;
	rest_direction[0]=bn.rest_direction[0];
	rest_direction[1]=bn.rest_direction[1];
	rest_direction[2]=bn.rest_direction[2];
	rest_position[0]=bn.rest_position[0];
	rest_position[1]=bn.rest_position[1];
	rest_position[2]=bn.rest_position[2];
	rest_length=bn.rest_length;
	MCopy(bn.rest_matrix,rest_matrix);
	MCopy(bn.transform_mtx,transform_mtx);
	scale_strength=bn.scale_strength;
	strength=bn.strength;
	num_weights=bn.num_weights;
	int i;
	if(bn.weights) {
		weights=new float[num_weights];
		for(i=0;i<num_weights;i++) {
			weights[i]=bn.weights[i];
		}
	}
	else {
		weights=NULL;
	}
}

void Bone::operator=(const Bone& bn) {
	active=bn.active;
	if(name) delete name;
	if(bn.name) {
		name=new char[strlen(bn.name)+1];
		strcpy(name,bn.name);
	}
	else {
		name=NULL;
	}
	parent_bone=bn.parent_bone;
	rest_direction[0]=bn.rest_direction[0];
	rest_direction[1]=bn.rest_direction[1];
	rest_direction[2]=bn.rest_direction[2];
	rest_position[0]=bn.rest_position[0];
	rest_position[1]=bn.rest_position[1];
	rest_position[2]=bn.rest_position[2];
	rest_length=bn.rest_length;
	MCopy(bn.rest_matrix,rest_matrix);
	MCopy(bn.transform_mtx,transform_mtx);
	scale_strength=bn.scale_strength;
	strength=bn.strength;
	num_weights=bn.num_weights;
	int i;
	if(weights) delete weights;
	if(bn.weights) {
		weights=new float[num_weights];
		for(i=0;i<num_weights;i++) {
			weights[i]=bn.weights[i];
		}
	}
	else {
		weights=NULL;
	}
}

Bone::~Bone() {
	if(name) delete name;
	if(weights) delete weights;
}

MovingObject3D::MovingObject3D() {
	Object3D::Object3D();
	metamorph=0.0f;
	morph_surfaces=0;
	morph_target=0;
	parent_object=0;
	target_object=0;
	pivot[0]=0.0f;
	pivot[1]=0.0f;
	pivot[2]=0.0f;
	int i;
	for(i=0;i<NUM_CHAN;i++) res[i]=0.0f;
	obj=NULL;
	name=NULL;
	is_dynamic=0;
}

MovingObject3D::MovingObject3D(const char* filename,int type) {
	metamorph=0.0f;
	morph_surfaces=0;
	morph_target=0;
	parent_object=0;
	target_object=0;
	pivot[0]=0.0f;
	pivot[1]=0.0f;
	pivot[2]=0.0f;
	int i;
	for(i=0;i<NUM_CHAN;i++) res[i]=0.0f;
	name=new char[strlen(filename)+1];
	strcpy(name,filename);
	is_dynamic=0;
	load_object();
}

MovingObject3D::~MovingObject3D() {
	if(name) delete name;
}

void MovingObject3D::load_object() {
	if(!name) return;
	if(is_dynamic) {
		obj=obj_manager.get_object_dynamic(name);
	}
	else {
		obj=obj_manager.get_object_static(name);
	}
}

void MovingObject3D::set_pivot_point(Point3 pv) {
	pivot[0]=pv[0];
	pivot[1]=pv[1];
	pivot[2]=pv[2];
}

void MovingObject3D::get_pivot_point(Point3 pv) {
	pv[0]=pivot[0];
	pv[1]=pivot[1];
	pv[2]=pivot[2];
}

void MovingObject3D::calc_transform(float frame) {
	Matrix tmp_mx,tmp_mx2;
	Point3 neg_pivot;
	neg_pivot[0]=-pivot[0];
	neg_pivot[1]=-pivot[1];
	neg_pivot[2]=-pivot[2];
	motion.calc(frame);
	int i;
	for(i=0;i<NUM_CHAN;i++) res[i]=motion.cv[i];
	MIdentity(transform_mtx);
	MTranslation(tmp_mx,res);
	MMultiply(transform_mtx,tmp_mx,tmp_mx2);
	MRotationY(tmp_mx,-res[3]*deg2rad);
	MMultiply(tmp_mx2,tmp_mx,transform_mtx);
	MRotationX(tmp_mx,-res[4]*deg2rad);
	MMultiply(transform_mtx,tmp_mx,tmp_mx2);
	MRotationZ(tmp_mx,res[5]*deg2rad);
	MMultiply(tmp_mx2,tmp_mx,transform_mtx);
	MScale(tmp_mx,res[6],res[7],res[8]);
	MMultiply(transform_mtx,tmp_mx,tmp_mx2);
	MTranslation(tmp_mx,neg_pivot);
	MMultiply(tmp_mx2,tmp_mx,transform_mtx);
}

void MovingObject3D::get_transform(Matrix mx) {
	Matrix mx2;
	if(parent_object) {
		obj_table[parent_object-1]->get_transform(mx2);
		MMultiply(mx2,transform_mtx,mx);
	}
	else {
		int i;
		for(i=0;i<16;i++) mx[i]=transform_mtx[i];
	}
}

void MovingObject3D::apply_transform() {
	if(parent_object) obj_table[parent_object-1]->apply_transform();
	glTranslatef(res[0],res[1],res[2]);
	glRotatef(-res[3],0.0f,1.0f,0.0f);
	glRotatef(-res[4],1.0f,0.0f,0.0f);
	glRotatef(res[5],0.0f,0.0f,1.0f);
	glScalef(res[6],res[7],res[8]);
	glTranslatef(-pivot[0],-pivot[1],-pivot[2]);
}

void MovingObject3D::render(float frame) {
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Entered object renderer\n");
	if(name) DebugFrameMessage("Object name is %s\n",name);
	DebugFrameMessage("Object has %d surfaces\n",obj->get_num_surfaces());
	int i;
	for(i=0;i<obj->get_num_surfaces();i++) {
		DebugFrameMessage("Surface %d pointer is 0x%x\n",i,obj->get_surface(i));
	}
	DebugFrameMessage("Position: %f\t%f\t%f\n",res[0],res[1],res[2]);
	DebugFrameMessage("Rotation: %f\t%f\t%f\n",res[3],res[4],res[5]);
	DebugFrameMessage("Scale: %f\t%f\t%f\n",res[6],res[7],res[8]);
	DebugFrameMessage("Transform matrix:\n");
	Matrix mx;
	get_transform(mx);
	for(i=0;i<4;i++) {
		DebugFrameMessage("%f\t%f\t%f\t%f\n",mx[i],mx[i+4],mx[i+8],mx[i+12]);
	}
#endif
	glPushMatrix();
	apply_transform();
	obj->render(frame);
	glPopMatrix();
}

void MovingObject3D::render_solid_surfaces(float frame) {
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Entered object renderer\n");
	if(name) DebugFrameMessage("Object name is %s\n",name);
	DebugFrameMessage("Object has %d surfaces\n",obj->get_num_surfaces());
	int i;
	for(i=0;i<obj->get_num_surfaces();i++) {
		DebugFrameMessage("Surface %d pointer is 0x%x\n",i,obj->get_surface(i));
	}
	DebugFrameMessage("Position: %f\t%f\t%f\n",res[0],res[1],res[2]);
	DebugFrameMessage("Rotation: %f\t%f\t%f\n",res[3],res[4],res[5]);
	DebugFrameMessage("Scale: %f\t%f\t%f\n",res[6],res[7],res[8]);
	DebugFrameMessage("Transform matrix:\n");
	Matrix mx;
	get_transform(mx);
	for(i=0;i<4;i++) {
		DebugFrameMessage("%f\t%f\t%f\t%f\n",mx[i],mx[i+4],mx[i+8],mx[i+12]);
	}
#endif
	glPushMatrix();
	apply_transform();
	obj->render_solid_surfaces(frame);
	glPopMatrix();
}

void MovingObject3D::render_additive_surfaces(float frame) {
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Entered object renderer\n");
	if(name) DebugFrameMessage("Object name is %s\n",name);
	DebugFrameMessage("Object has %d surfaces\n",obj->get_num_surfaces());
	int i;
	for(i=0;i<obj->get_num_surfaces();i++) {
		DebugFrameMessage("Surface %d pointer is 0x%x\n",i,obj->get_surface(i));
	}
	DebugFrameMessage("Position: %f\t%f\t%f\n",res[0],res[1],res[2]);
	DebugFrameMessage("Rotation: %f\t%f\t%f\n",res[3],res[4],res[5]);
	DebugFrameMessage("Scale: %f\t%f\t%f\n",res[6],res[7],res[8]);
	DebugFrameMessage("Transform matrix:\n");
	Matrix mx;
	get_transform(mx);
	for(i=0;i<4;i++) {
		DebugFrameMessage("%f\t%f\t%f\t%f\n",mx[i],mx[i+4],mx[i+8],mx[i+12]);
	}
#endif
	glPushMatrix();
	apply_transform();
	obj->render_additive_surfaces(frame);
	glPopMatrix();
}

void MovingObject3D::render_transparent_surfaces(float frame) {
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Entered object renderer\n");
	if(name) DebugFrameMessage("Object name is %s\n",name);
	DebugFrameMessage("Object has %d surfaces\n",obj->get_num_surfaces());
	int i;
	for(i=0;i<obj->get_num_surfaces();i++) {
		DebugFrameMessage("Surface %d pointer is 0x%x\n",i,obj->get_surface(i));
	}
	DebugFrameMessage("Position: %f\t%f\t%f\n",res[0],res[1],res[2]);
	DebugFrameMessage("Rotation: %f\t%f\t%f\n",res[3],res[4],res[5]);
	DebugFrameMessage("Scale: %f\t%f\t%f\n",res[6],res[7],res[8]);
	DebugFrameMessage("Transform matrix:\n");
	Matrix mx;
	get_transform(mx);
	for(i=0;i<4;i++) {
		DebugFrameMessage("%f\t%f\t%f\t%f\n",mx[i],mx[i+4],mx[i+8],mx[i+12]);
	}
#endif
	glPushMatrix();
	apply_transform();
	obj->render_transparent_surfaces(frame);
	glPopMatrix();
}

void MovingObject3D::get_position(Point3 pos) {
	pos[0]=res[0];
	pos[1]=res[1];
	pos[2]=res[2];
}

void MovingObject3D::get_transformed_bbox(BBox *bx) {
	Matrix mx;
	if(!obj) return;
	BBox bbox=obj->get_bbox(); 
	get_transform(mx);
	MVMultiply3(mx,bbox.p1,bx->p1);
	MVMultiply3(mx,bbox.p2,bx->p2);
	MVMultiply3(mx,bbox.p3,bx->p3);
	MVMultiply3(mx,bbox.p4,bx->p4);
	MVMultiply3(mx,bbox.p5,bx->p5);
	MVMultiply3(mx,bbox.p6,bx->p6);
	MVMultiply3(mx,bbox.p7,bx->p7);
	MVMultiply3(mx,bbox.p8,bx->p8);
}

void MovingObject3D::get_trn_bbox_center(Point3 pt) {
	Matrix mx;
	if(!obj) return;
	get_transform(mx);
	Point3 center;
	BBox bbox=obj->get_bbox(); 
	center[0]=(bbox.p_min[0]+bbox.p_max[0])*0.5f;
	center[1]=(bbox.p_min[1]+bbox.p_max[1])*0.5f;
	center[2]=(bbox.p_min[2]+bbox.p_max[2])*0.5f;
	MVMultiply3(mx,center,pt);
}

void MovingObject3D::setup_morphing() {
	unsigned int i;
	if(!obj) return;
	if(morph_target) {
		for(i=0;i<obj->get_num_surfaces();i++) {
			obj->get_surface(i)->attach_mod(
				new MorphShader(
				obj->get_surface(i)->get_vb(),
				obj->get_surface(i)->get_mod_vb(),
				&metamorph,
				obj_table[morph_target-1]->obj->get_surface(i)->get_vb()
				));
		}
	}
}

MovingLight::MovingLight(GLenum id) {
	Light::Light();
	set_light_id(id);
	parent_object=0;
	target_object=0;
	lens_flare=0;
	flare_intensity=1.0f;
	light_intensity=1.0f;
	falloff=0.0f;
	appear_intensity=0.0f;
	last_vis_check=0.0f;
}

void MovingLight::calc_transform(float frame) {
	Matrix tmp_mx,tmp_mx2;
	motion.calc(frame);
	int i;
	for(i=0;i<NUM_CHAN;i++) res[i]=motion.cv[i];

	MIdentity(transform_mtx);
	MTranslation(tmp_mx,res);
	MMultiply(transform_mtx,tmp_mx,tmp_mx2);
	MRotationY(tmp_mx,-res[3]*deg2rad);
	MMultiply(tmp_mx2,tmp_mx,transform_mtx);
	MRotationX(tmp_mx,-res[4]*deg2rad);
	MMultiply(transform_mtx,tmp_mx,tmp_mx2);
	MRotationZ(tmp_mx,res[5]*deg2rad);
	MMultiply(tmp_mx2,tmp_mx,transform_mtx);
}

void MovingLight::get_transform(Matrix mx) {
	Matrix mx2;
	if(parent_object) {
		obj_table[parent_object-1]->get_transform(mx2);
		MMultiply(mx2,transform_mtx,mx);
	}
	else {
		int i;
		for(i=0;i<16;i++) mx[i]=transform_mtx[i];
	}
}

void MovingLight::apply_transform() {
	if(parent_object) obj_table[parent_object-1]->apply_transform();
	glTranslatef(res[0],res[1],res[2]);
	glRotatef(-res[3],0.0f,1.0f,0.0f);
	glRotatef(-res[4],1.0f,0.0f,0.0f);
	glRotatef(res[5],0.0f,0.0f,1.0f);
}

void MovingLight::get_position(Point3 pos) {
	Matrix mx;
	Point3 pv;
	get_transform(mx);
	pv[0]=0.0f;
	pv[1]=0.0f;
	pv[2]=0.0f;
	MVMultiply3(mx,pv,pos);
}

void MovingLight::draw(float frame) {
	calc_transform(frame);
	glPushMatrix();
	if(target_object) {
		Matrix mx;
		Point3 pv,target,source;
		obj_table[target_object-1]->get_transform(mx);
		obj_table[target_object-1]->get_pivot_point(pv);
		MVMultiply3(mx,pv,target);
		get_transform(mx);
		pv[0]=0.0f;
		pv[1]=0.0f;
		pv[2]=0.0f;
		MVMultiply3(mx,pv,source);
		Vector3 v;
		PDiff3(source,target,v);
		MLookAt(mx,0.0f,0.0f,0.0f,
			v[0],v[1],v[2],
			0.0,1.0,0.0);
		glTranslatef(res[0],res[1],res[2]);
		Matrix mxi;
		MInvert(mx,mxi);
		glMultMatrixf(mxi);
	}
	else { 
		apply_transform();
	}

	if(light_type==2) {  //spot
		float dir[4]={0.0f,0.0f,-1.0f,1.0f};
		glLightfv(light_id,GL_SPOT_DIRECTION,dir);
		cone_angle.calc(frame);
		glLightf(light_id,GL_SPOT_CUTOFF,cone_angle.res);
	}
	else {
		glLightf(light_id,GL_SPOT_CUTOFF,180.0f);
	}
	falloff.calc(frame);
	if(falloff.res!=0.0f) {
		glLightf(light_id,GL_QUADRATIC_ATTENUATION,0.25f/falloff.res);
		glLightf(light_id,GL_CONSTANT_ATTENUATION,0.0f);
	}
	else {
		glLightf(light_id,GL_QUADRATIC_ATTENUATION,0.0f);
		glLightf(light_id,GL_CONSTANT_ATTENUATION,1.0f);
	}
	float dif[4];
	c_state->want_enabled(light_id);
	dif[0]=0.0f;
	dif[1]=0.0f;
	dif[2]=0.0f;
	dif[3]=1.0f;
	glLightfv(light_id,GL_AMBIENT,dif);
	light_intensity.calc(frame);
	dif[0]=light_intensity.res*color[0];
	dif[1]=light_intensity.res*color[1];
	dif[2]=light_intensity.res*color[2];
	dif[3]=1.0f; //light_intensity.res*color[3];
	glLightfv(light_id,GL_DIFFUSE,dif);
	float pos[4]={0.0f,0.0f,0.0f,1.0f};
	if(light_type==0) {
		float dir[4]={0.0f,0.0f,1.0f,0.0f};
		glLightfv(light_id,GL_POSITION,dir);
	}
	else {
		glLightfv(light_id,GL_POSITION,pos);
	}
	glPopMatrix();
}

void MovingLight::get_color(float* col_r,float* col_g,
							float* col_b,float* col_a) {
	*col_r=light_intensity.res*color[0];
	*col_g=light_intensity.res*color[1];
	*col_b=light_intensity.res*color[2];
	*col_a=light_intensity.res*color[3];
}

void MovingLight::set_cone_angle(Envelope env) {
	cone_angle=env;
}

void MovingLight::set_edge_angle(Envelope env) {
	edge_angle=env;
}

void MovingLight::set_falloff(Envelope env) {
	falloff=env;
}

void MovingLight::set_flare_intensity(Envelope env) {
	flare_intensity=env;
}

void MovingLight::set_flare_options(int t) {
	flare_options=t;
}

void MovingLight::set_flare_type(int t) {
	lens_flare=t;
}

void MovingLight::set_light_intensity(Envelope env) {
	light_intensity=env;
}


MovingCamera::MovingCamera() {
	Camera::Camera();
	parent_object=0;
	target_object=0;
	zoom_factor=1.0f;
}

void MovingCamera::calc_transform(float frame) {
	Matrix tmp_mx,tmp_mx2;
	motion.calc(frame);
	int i;
	for(i=0;i<NUM_CHAN;i++) res[i]=motion.cv[i];
	MIdentity(transform_mtx);
	MTranslation(tmp_mx,res);
	MMultiply(transform_mtx,tmp_mx,tmp_mx2);
	MRotationZ(tmp_mx,res[5]*deg2rad);
	MMultiply(tmp_mx2,tmp_mx,transform_mtx);
	MRotationX(tmp_mx,-res[4]*deg2rad);
	MMultiply(transform_mtx,tmp_mx,tmp_mx2);
	MRotationY(tmp_mx,-res[3]*deg2rad);
	MMultiply(tmp_mx2,tmp_mx,transform_mtx);
}

void MovingCamera::get_transform(Matrix mx) {
	Matrix mx2;
	if(parent_object) {
		obj_table[parent_object-1]->get_transform(mx2);
		MMultiply(mx2,transform_mtx,mx);
	}
	else {
		int i;
		for(i=0;i<16;i++) mx[i]=transform_mtx[i];
	}
}

void MovingCamera::apply_transform() {
	if(parent_object) obj_table[parent_object-1]->apply_transform();
	glRotatef(-res[5],0.0f,0.0f,1.0f);
	glRotatef(res[4],1.0f,0.0f,0.0f);
	glRotatef(res[3],0.0f,1.0f,0.0f);
	glTranslatef(-res[0],-res[1],-res[2]);
}


void MovingCamera::get_position(Point3 pos) {
	Matrix mx;
	Point3 pv;
	get_transform(mx);
	pv[0]=0.0f;
	pv[1]=0.0f;
	pv[2]=0.0f;
	MVMultiply3(mx,pv,pos);
}


void MovingCamera::get_direction(Vector3 dir) {
	if(target_object) {
		Point3 obj_pos;
		obj_table[target_object-1]->get_position(obj_pos);
		PDiff3(res,obj_pos,dir);
		return;
	}
	Matrix mx1,mx2,mx3;
	MIdentity(mx1);
	MRotationY(mx2,-res[3]*deg2rad);
	MMultiply(mx1,mx2,mx3);
	MRotationX(mx2,-res[4]*deg2rad);
	MMultiply(mx3,mx2,mx1);
	MRotationZ(mx2,res[5]*deg2rad);
	MMultiply(mx1,mx2,mx3);
	Vector3 base_dir;
	base_dir[0]=0.0f;
	base_dir[1]=0.0f;
	base_dir[2]=-1.0f;
	MVMultiply3(mx3,base_dir,dir);
}

void MovingCamera::get_up_vector(Vector3 up) {
	if(target_object) {
		up[0]=0.0f;
		up[1]=1.0f;
		up[2]=0.0f;
		return;
	}
	Matrix mx1,mx2,mx3;
	MIdentity(mx1);
	MRotationY(mx2,-res[3]*deg2rad);
	MMultiply(mx1,mx2,mx3);
	MRotationX(mx2,-res[4]*deg2rad);
	MMultiply(mx3,mx2,mx1);
	MRotationZ(mx2,res[5]*deg2rad);
	MMultiply(mx1,mx2,mx3);
	Vector3 base_dir;
	base_dir[0]=0.0f;
	base_dir[1]=1.0f;
	base_dir[2]=0.0f;
	MVMultiply3(mx3,base_dir,up);
}

void MovingCamera::draw(float frame) {
	calc_transform(frame);
	zoom_factor.calc(frame);
	float scene_size_factor=current_scene->get_size_factor();
	near_clip=0.08*zoom_factor.res*scene_size_factor;
	far_clip=60.0*scene_size_factor;
	frustum_l=-0.133f*scene_size_factor;
	frustum_r=0.133f*scene_size_factor;
	frustum_t=0.1f*scene_size_factor;
	frustum_b=-0.1f*scene_size_factor;
	glLoadIdentity();
	Matrix mx;
	get_camera_matrix(mx);
	glMultMatrixf(mx);
	update_frustum();
}

void MovingCamera::get_camera_matrix(Matrix mx) {
	Point3 pos,dir,up;
	Matrix mx1,mx2;
	get_position(pos);
	get_direction(dir);
	get_up_vector(up);
	MLookAt(mx1,0.0f,0.0f,0.0f,
		dir[0],dir[1],dir[2],
		up[0],up[1],up[2]);
	pos[0]=-pos[0];
	pos[1]=-pos[1];
	pos[2]=-pos[2];
	MTranslation(mx2,pos);
	MMultiply(mx1,mx2,mx);
}

void MovingCamera::get_frustum_matrix(Matrix mx) {
	MFrustum(mx,frustum_l,frustum_r,frustum_b,frustum_t,near_clip,far_clip);
}

float MovingCamera::get_near_clip() {
	float scene_size_factor=current_scene->get_size_factor();
	return (float)0.08*zoom_factor.res*scene_size_factor;
}

float MovingCamera::get_far_clip() {
	float scene_size_factor=current_scene->get_size_factor();
	return (float)60.0*scene_size_factor;
}

void MovingCamera::get_transformed_frustum(float left[4],float right[4],float top[4],
										   float bottom[4],float near_p[4],float far_p[4]) {
	float scene_size_factor=current_scene->get_size_factor();
	Matrix mx,inv_mx;
	get_camera_matrix(mx);
	MInvert(mx,inv_mx);   //inverse cam matrix will work for both cameras with and without targets
	float n_c=get_near_clip();
	float f_c=get_far_clip();
	Point3 tmp_pt,p1,p2,p3,p4,p5,p6,p7,p8;
	tmp_pt[0]=-0.133f*scene_size_factor;
	tmp_pt[1]=0.1f*scene_size_factor;
	tmp_pt[2]=-n_c;
	MVMultiply3(inv_mx,tmp_pt,p1);
	tmp_pt[0]=-0.133f*scene_size_factor;
	tmp_pt[1]=-0.1f*scene_size_factor;
	tmp_pt[2]=-n_c;
	MVMultiply3(inv_mx,tmp_pt,p2);
	tmp_pt[0]=0.133f*scene_size_factor;
	tmp_pt[1]=-0.1f*scene_size_factor;
	tmp_pt[2]=-n_c;
	MVMultiply3(inv_mx,tmp_pt,p3);
	tmp_pt[0]=0.133f*scene_size_factor;
	tmp_pt[1]=0.1f*scene_size_factor;
	tmp_pt[2]=-n_c;
	MVMultiply3(inv_mx,tmp_pt,p4);

	tmp_pt[0]=-0.133f*scene_size_factor*(f_c/n_c);  //default far clip = 40.0f
	tmp_pt[1]=0.1f*scene_size_factor*(f_c/n_c);
	tmp_pt[2]=-f_c;
	MVMultiply3(inv_mx,tmp_pt,p5);
	tmp_pt[0]=-0.133f*scene_size_factor*(f_c/n_c);  
	tmp_pt[1]=-0.1f*scene_size_factor*(f_c/n_c);
	tmp_pt[2]=-f_c;
	MVMultiply3(inv_mx,tmp_pt,p6);
	tmp_pt[0]=0.133f*scene_size_factor*(f_c/n_c);  
	tmp_pt[1]=-0.1f*scene_size_factor*(f_c/n_c);
	tmp_pt[2]=-f_c;
	MVMultiply3(inv_mx,tmp_pt,p7);
	tmp_pt[0]=0.133f*scene_size_factor*(f_c/n_c);  
	tmp_pt[1]=0.1f*scene_size_factor*(f_c/n_c);
	tmp_pt[2]=-f_c;
	MVMultiply3(inv_mx,tmp_pt,p8);

	//calculate plane coefficients
	Vector3 v1,v2,cross;
	//left
	PDiff3(p1,p5,v1);
	PDiff3(p1,p6,v2);
	VCrossProduct3(v1,v2,cross);
	VNormalize3(cross);
	left[0]=cross[0];
	left[1]=cross[1];
	left[2]=cross[2];
	left[3]=-(p1[0]*cross[0]+p1[1]*cross[1]+p1[2]*cross[2]);
	//right
	PDiff3(p4,p7,v1);
	PDiff3(p4,p8,v2);
	VCrossProduct3(v1,v2,cross);
	VNormalize3(cross);
	right[0]=cross[0];
	right[1]=cross[1];
	right[2]=cross[2];
	right[3]=-(p4[0]*cross[0]+p4[1]*cross[1]+p4[2]*cross[2]);
	//top
	PDiff3(p4,p8,v1);
	PDiff3(p4,p5,v2);
	VCrossProduct3(v1,v2,cross);
	VNormalize3(cross);
	top[0]=cross[0];
	top[1]=cross[1];
	top[2]=cross[2];
	top[3]=-(p4[0]*cross[0]+p4[1]*cross[1]+p4[2]*cross[2]);
	//bottom
	PDiff3(p2,p6,v1);
	PDiff3(p2,p7,v2);
	VCrossProduct3(v1,v2,cross);
	VNormalize3(cross);
	bottom[0]=cross[0];
	bottom[1]=cross[1];
	bottom[2]=cross[2];
	bottom[3]=-(p2[0]*cross[0]+p2[1]*cross[1]+p2[2]*cross[2]);
	//near
	PDiff3(p1,p2,v1);
	PDiff3(p1,p3,v2);
	VCrossProduct3(v1,v2,cross);
	VNormalize3(cross);
	near_p[0]=cross[0];
	near_p[1]=cross[1];
	near_p[2]=cross[2];
	near_p[3]=-(p1[0]*cross[0]+p1[1]*cross[1]+p1[2]*cross[2]);
	//far
	PDiff3(p5,p8,v1);
	PDiff3(p5,p7,v2);
	VCrossProduct3(v1,v2,cross);
	VNormalize3(cross);
	far_p[0]=cross[0];
	far_p[1]=cross[1];
	far_p[2]=cross[2];
	far_p[3]=-(p5[0]*cross[0]+p5[1]*cross[1]+p5[2]*cross[2]);
}

SceneRenderFX::SceneRenderFX() {
	blinding_flares=0;
	fuzzy_camera=0;
	shake_camera=0;
	shake_amount=0;
}

Scene3D::Scene3D(float size) {
	scene_size_factor=size;
	rfx=NULL;
	num_objects=0;
	num_solid_procedurals=0;
	num_transparent_procedurals=0;
	obj_table=NULL;
	solid_procedurals=NULL;
	transparent_procedurals=NULL;
	ambient[0]=0.0f;
	ambient[1]=0.0f;
	ambient[2]=0.0f;
	ambient[3]=1.0f;
	ambient_intensity=0.0f;
	enable_lens_flare=1;
	first_frame=0;
	last_frame=0;
	frames_per_second=30.0f;
	global_flare_intensity=1.0f;
	fog_type=0;
	fog_start=0.0f;
	fog_end=0.0f;
	backdrop_fog=0;
	fog_color[0]=1.0f;
	fog_color[1]=1.0f;
	fog_color[2]=1.0f;
	fog_color[3]=1.0f;
	backdrop_color[0]=1.0f;
	backdrop_color[1]=1.0f;
	backdrop_color[2]=1.0f;
	backdrop_color[3]=1.0f;
	backdrop_fog=0;
	int i;
	for(i=0;i<8;i++) lights[i]=NULL;
	num_lights=0;
	cam=NULL;
	flare_tex[0]=tex_manager.get_texture("flares/flare05.jpg");
	flare_tex[1]=tex_manager.get_texture("flares/flare04.jpg");
	flare_tex[2]=tex_manager.get_texture("flares/flare03.jpg");
	flare_tex[3]=tex_manager.get_texture("flares/flare02.jpg");
	flare_tex[4]=tex_manager.get_texture("flares/flare01.jpg");
	num_transparent_objects=0;
	num_additive_objects=0;
	bbox_centers=NULL;
	is_transparent=NULL;
	is_additive=NULL;
	transparent_order=NULL;
	flare_falloff_time=0.3f;
}

Scene3D::Scene3D(const char* filename,float size) {
	scene_size_factor=size;
	rfx=NULL;
	num_objects=0;
	num_solid_procedurals=0;
	num_transparent_procedurals=0;
	obj_table=NULL;
	solid_procedurals=NULL;
	transparent_procedurals=NULL;
	ambient[0]=0.0f;
	ambient[1]=0.0f;
	ambient[2]=0.0f;
	ambient[3]=0.0f;
	ambient_intensity=0.0f;
	enable_lens_flare=1;
	first_frame=0;
	last_frame=0;
	frames_per_second=30.0f;
	global_flare_intensity=1.0f;
	fog_type=0;
	fog_start=0.0f;
	fog_end=0.0f;
	backdrop_fog=0;
	fog_color[0]=1.0f;
	fog_color[1]=1.0f;
	fog_color[2]=1.0f;
	fog_color[3]=1.0f;
	backdrop_color[0]=1.0f;
	backdrop_color[1]=1.0f;
	backdrop_color[2]=1.0f;
	backdrop_color[3]=1.0f;
	backdrop_fog=0;
	int i;
	for(i=0;i<8;i++) lights[i]=NULL;
	num_lights=0;
	cam=NULL;
	flare_tex[0]=tex_manager.get_texture("flares/flare05.jpg");
	flare_tex[1]=tex_manager.get_texture("flares/flare04.jpg");
	flare_tex[2]=tex_manager.get_texture("flares/flare03.jpg");
	flare_tex[3]=tex_manager.get_texture("flares/flare02.jpg");
	flare_tex[4]=tex_manager.get_texture("flares/flare01.jpg");
	num_transparent_objects=0;
	num_additive_objects=0;
	bbox_centers=NULL;
	is_transparent=NULL;
	is_additive=NULL;
	transparent_order=NULL;
	flare_falloff_time=0.3f;
	load_LWS_file(filename);
}

void Scene3D::add_object(MovingObject3D* obj) {
	if(num_objects==0) {
		obj_table=new MovingObject3D*[1];
		obj_table[num_objects++]=obj;
	}
	else {
		MovingObject3D** tmp_obj;
		tmp_obj=new MovingObject3D*[num_objects+1];
		unsigned int i;
		for(i=0;i<num_objects;i++) tmp_obj[i]=obj_table[i];
		tmp_obj[num_objects++]=obj;
		delete obj_table;
		obj_table=tmp_obj;
	}
}

void Scene3D::attach_procedural_object(ProceduralObject* obj,int stage) {
	switch(stage) {
	case 0:
		obj->set_current_scene(this);
		if(num_solid_procedurals==0) {
			solid_procedurals=new ProceduralObject*[1];
			solid_procedurals[num_solid_procedurals++]=obj;
		}
		else {
			ProceduralObject** tmp_obj;
			tmp_obj=new ProceduralObject*[num_solid_procedurals+1];
			unsigned int i;
			for(i=0;i<num_solid_procedurals;i++) tmp_obj[i]=solid_procedurals[i];
			tmp_obj[num_solid_procedurals++]=obj;
			delete solid_procedurals;
			solid_procedurals=tmp_obj;
		}
	break;
	case 1:
		obj->set_current_scene(this);
		if(num_transparent_procedurals==0) {
			transparent_procedurals=new ProceduralObject*[1];
			transparent_procedurals[num_transparent_procedurals++]=obj;
		}
		else {
			ProceduralObject** tmp_obj;
			tmp_obj=new ProceduralObject*[num_transparent_procedurals+1];
			unsigned int i;
			for(i=0;i<num_transparent_procedurals;i++) tmp_obj[i]=transparent_procedurals[i];
			tmp_obj[num_transparent_procedurals++]=obj;
			delete transparent_procedurals;
			transparent_procedurals=tmp_obj;
		}
	break;
	}
}

void Scene3D::disable_flare() {
	enable_lens_flare=0;
}

void Scene3D::enable_flare() {
	enable_lens_flare=1;
}

void Scene3D::set_ambient_color(float amb_r,float amb_g,float amb_b) {
	ambient[0]=amb_r;
	ambient[1]=amb_g;
	ambient[2]=amb_b;
	ambient[3]=0.0f;
}

void Scene3D::set_ambient_intensity(Envelope env) {
	ambient_intensity=env;
}

void Scene3D::set_camera(MovingCamera *camera) {
	cam=camera;
}

void Scene3D::set_global_flare_intensity(Envelope env) {
	global_flare_intensity=env;
}

void Scene3D::set_render_fx(SceneRenderFX *render_fx) {
	rfx=render_fx;
}

enum LWS_KEYWORD {
	LWSC,
	FIRST_FRAME,
	LAST_FRAME,
	PREVIEW_FIRST_FRAME,
	PREVIEW_LAST_FRAME,
	FRAMES_PER_SECOND,
	LOAD_OBJECT,
	OBJECT_MOTION,
	END_BEHAVIOR,
	ADD_NULL_OBJECT,
	PIVOT_POINT,
	PARENT_OBJECT,
	UNAFFECTED_BY_FOG,
	AMBIENT_COLOR,
	AMB_INTENSITY,
	ADD_LIGHT,
	LIGHT_MOTION,
	LIGHT_COLOR,
	LGT_INTENSITY,
	LIGHT_TYPE,
	FALLOFF,
	CONE_ANGLE,
	ENABLE_LENS_FLARE,
	GLOBAL_FLARE_INTENSITY,
	TARGET_OBJECT,
	LENS_FLARE,
	FLARE_INTENSITY,
	CAMERA_MOTION,
	ZOOM_FACTOR,
	FOG_TYPE,
	FOG_MIN_DIST,
	FOG_MAX_DIST,
	FOG_COLOR,
	BACKDROP_COLOR,
	BACKDROP_FOG,
	METAMORPH,
	MORPH_TARGET,
	ADD_BONE,
	BONE_NAME,
	BONE_ACTIVE,
	BONE_REST_POSITION,
	BONE_REST_DIRECTION,
	BONE_REST_LENGTH,
	SCALE_BONE_STRENGTH,
	BONE_STRENGTH,
	BONE_MOTION,
	BONE_LIMITED_RANGE,
	BONE_MIN_RANGE,
	BONE_MAX_RANGE,
	BONE_FALLOFF_TYPE,                                //per object property!!!!!!
	UNKNOWN_LWS_KEYWORD
};

static LWS_KEYWORD map_keyword(const char* str) {
	if(strcmp("LWSC",str)==0) return LWSC;
	if(strcmp("FirstFrame",str)==0) return FIRST_FRAME;
	if(strcmp("LastFrame",str)==0) return LAST_FRAME;
	if(strcmp("PreviewFirstFrame",str)==0) return PREVIEW_FIRST_FRAME;
	if(strcmp("PreviewLastFrame",str)==0) return PREVIEW_LAST_FRAME;
	if(strcmp("FramesPerSecond",str)==0) return FRAMES_PER_SECOND;
	if(strcmp("LoadObject",str)==0) return LOAD_OBJECT;
	if(strcmp("ObjectMotion",str)==0) return OBJECT_MOTION;
	if(strcmp("AddNullObject",str)==0) return ADD_NULL_OBJECT;
	if(strcmp("PivotPoint",str)==0) return PIVOT_POINT;
	if(strcmp("ParentObject",str)==0) return PARENT_OBJECT;
	if(strcmp("UnaffectedByFog",str)==0) return UNAFFECTED_BY_FOG;
	if(strcmp("AmbientColor",str)==0) return AMBIENT_COLOR;
	if(strcmp("AmbIntensity",str)==0) return AMB_INTENSITY;
	if(strcmp("AddLight",str)==0) return ADD_LIGHT;
	if(strcmp("LightMotion",str)==0) return LIGHT_MOTION;
	if(strcmp("LightColor",str)==0) return LIGHT_COLOR;
	if(strcmp("LgtIntensity",str)==0) return LGT_INTENSITY;
	if(strcmp("LightType",str)==0) return LIGHT_TYPE;
	if(strcmp("LightRange",str)==0) return FALLOFF;  //seems to be different than official spec
	if(strcmp("ConeAngle",str)==0) return CONE_ANGLE;
	if(strcmp("EnableLensFlare",str)==0) return ENABLE_LENS_FLARE;
	if(strcmp("GlobalFlareIntensity",str)==0) return GLOBAL_FLARE_INTENSITY;
	if(strcmp("TargetObject",str)==0) return TARGET_OBJECT;
	if(strcmp("LensFlare",str)==0) return LENS_FLARE;
	if(strcmp("FlareIntensity",str)==0) return FLARE_INTENSITY;
	if(strcmp("CameraMotion",str)==0) return CAMERA_MOTION;
	if(strcmp("ZoomFactor",str)==0) return ZOOM_FACTOR;
	if(strcmp("FogType",str)==0) return FOG_TYPE;
	if(strcmp("FogMinDist",str)==0) return FOG_MIN_DIST;
	if(strcmp("FogMaxDist",str)==0) return FOG_MAX_DIST;
	if(strcmp("FogColor",str)==0) return FOG_COLOR;
	if(strcmp("BackdropColor",str)==0) return BACKDROP_COLOR;
	if(strcmp("BackdropFog",str)==0) return BACKDROP_FOG;
	if(strcmp("Metamorph",str)==0) return METAMORPH;
	if(strcmp("MorphTarget",str)==0) return MORPH_TARGET;

	if(strcmp("AddBone",str)==0) return ADD_BONE;
	if(strcmp("BoneName",str)==0) return BONE_NAME;
	if(strcmp("BoneActive",str)==0) return BONE_ACTIVE;
	if(strcmp("BoneRestPosition",str)==0) return BONE_REST_POSITION;
	if(strcmp("BoneRestDirection",str)==0) return BONE_REST_DIRECTION;
	if(strcmp("BoneRestLength",str)==0) return BONE_REST_LENGTH;
	if(strcmp("ScaleBoneStrength",str)==0) return SCALE_BONE_STRENGTH;
	if(strcmp("BoneLimitedRange",str)==0) return BONE_LIMITED_RANGE;
	if(strcmp("BoneMinRange",str)==0) return BONE_MIN_RANGE;
	if(strcmp("BoneMaxRange",str)==0) return BONE_MAX_RANGE;
	if(strcmp("BoneStrength",str)==0) return BONE_STRENGTH;
	if(strcmp("BoneMotion",str)==0) return BONE_MOTION;
	if(strcmp("BoneFalloffType",str)==0) return BONE_FALLOFF_TYPE;
	return UNKNOWN_LWS_KEYWORD;
}

Scene3D::~Scene3D() {
	if(cam!=NULL) delete cam;
	unsigned int i;
	for(i=1;i<8;i++) if(lights[i]!=NULL) delete lights[i];
	for(i=0;i<num_objects;i++) if(obj_table[i]!=NULL) delete obj_table[i];
	if(bbox_centers!=NULL) delete bbox_centers;
	if(is_transparent!=NULL) delete is_transparent;
	if(is_additive!=NULL) delete is_additive;
	if(transparent_order!=NULL) delete transparent_order;
	if(rfx) delete rfx;
}

static void read_envelope(int f,Envelope& env) {
	int i,num_keys;
	float value;
	int frame,linear;
	double tens,cont,bias;
	vfs->scanInt(f,&num_keys);
	vfs->scanInt(f,&num_keys);
	env.clear();
	env.alloc_keyframe_mem(num_keys);
#ifdef _LOG_EVERYTHING
			DebugMessage("envelope - numkeys %d\n",num_keys);
#endif
	for(i=0;i<num_keys;i++) {
		vfs->scanFloat(f,&value);
		vfs->scanInt(f,&frame);
		vfs->scanInt(f,&linear);
		vfs->scanDouble(f,&tens);
		vfs->scanDouble(f,&cont);
		vfs->scanDouble(f,&bias);

//		fscanf(f,"%f",&value);
//		fscanf(f,"%d",&frame);
//		fscanf(f,"%d",&linear);
//		fscanf(f,"%lf",&tens);
//		fscanf(f,"%lf",&cont);
//		fscanf(f,"%lf",&bias);
		env.add_key_frame(value,tens,cont,bias,linear,frame);
#ifdef _LOG_EVERYTHING
			DebugMessage("value frame linear tens cont bias %f %d %d %f %f %f\n",
				value,frame,linear,tens,cont,bias);
#endif
	}
	char tmp_str[20];
//	fscanf(f,"%s",tmp_str);  //eat EndBehavior
	vfs->scanString(f,tmp_str);
//	fscanf(f,"%d",&i);
	vfs->scanInt(f,&i);
#ifdef _LOG_EVERYTHING
	DebugMessage("Envelope - EndBehavior %d\n",i);
#endif
	env.set_end_behavior(i);
	env.dump();
}  //read_envelope

static Envelope* read_envelope(int f) {
	Envelope *env;
	env=new Envelope();
	int i,num_keys;
	float value;
	int frame,linear;
	double tens,cont,bias;
	vfs->scanInt(f,&num_keys);
	vfs->scanInt(f,&num_keys);
//	fscanf(f,"%d",&num_keys);
//	fscanf(f,"%d",&num_keys);
	env->alloc_keyframe_mem(num_keys);
	for(i=0;i<num_keys;i++) {
		vfs->scanFloat(f,&value);
		vfs->scanInt(f,&frame);
		vfs->scanInt(f,&linear);
		vfs->scanDouble(f,&tens);
		vfs->scanDouble(f,&cont);
		vfs->scanDouble(f,&bias);
//		fscanf(f,"%f",&value);
//		fscanf(f,"%d",&frame);
//		fscanf(f,"%d",&linear);
//		fscanf(f,"%lf",&tens);
//		fscanf(f,"%lf",&cont);
//		fscanf(f,"%lf",&bias);
		env->add_key_frame(value,tens,cont,bias,linear,frame);
	}
	char tmp_str[20];
	vfs->scanString(f,tmp_str);
	vfs->scanInt(f,&i);
//	fscanf(f,"%s",tmp_str);  //eat EndBehavior
//	fscanf(f,"%d",&i);
	env->set_end_behavior(i);
	return env;
}

void Scene3D::load_LWS_file(const char* filename) {
	int f;
	unsigned int i;
//	f=fopen(filename,"rb");
	GC_ERROR err_code;
	err_code=vfs->openFile(filename,f);
	if(err_code!=GC_OK) {
#ifdef _LOG_EVERYTHING
		DebugFileMessage("warnings.log","File %s not found!\n",filename);
#endif
		return;
	}
	char str_buf[256];
	ChanVec cv;
	LWS_KEYWORD keyword_id;
	Envelope env;
	Envelope* p_env;
	int current_read=-1;
	int more_lights_forbidden=0;
	while(!vfs->eof(f)) {
//		fscanf(f,"%s",str_buf);
		vfs->scanString(f,str_buf);
		keyword_id=map_keyword(str_buf);
		switch(keyword_id) {
		case FIRST_FRAME:
//			fscanf(f,"%d",&first_frame);
			vfs->scanInt(f,&first_frame);
		break;
		case LAST_FRAME:
//			fscanf(f,"%d",&last_frame);
			vfs->scanInt(f,&last_frame);
		break;
		case PREVIEW_FIRST_FRAME:
			int tmp;
//			fscanf(f,"%d",&tmp);
			vfs->scanInt(f,&tmp);
			if(tmp<first_frame) first_frame=tmp;
		break;
		case PREVIEW_LAST_FRAME:
//			fscanf(f,"%d",&tmp);
			vfs->scanInt(f,&tmp);
			if(tmp>last_frame) last_frame=tmp;
		break;
		case FRAMES_PER_SECOND:
//			fscanf(f,"%f",&frames_per_second);
			vfs->scanFloat(f,&frames_per_second);
		break;
		case LOAD_OBJECT:
			current_read=0;
//			fscanf(f,"%s",str_buf);
			vfs->scanString(f,str_buf);
			//for timemachine style reader (everything in single dir) 
/*			int last;
			last=-1;
			for(i=0;i<strlen(str_buf);i++) {
				if(str_buf[i]=='\\') last=i;
			}
			if(last>=0) {
				for(i=last+1;i<strlen(str_buf);i++) {
					str_buf[i-last-1]=str_buf[i];
				}
			}
			str_buf[strlen(str_buf)-last-1]='\0';*/
			//end specific

			//single dir with subdirs (bayger's lws3d)
			char tmp_s2[256];
			int last;
			last=-1;
			for(i=0;i<strlen(str_buf);i++) {
				if(str_buf[i]=='\\') last=i;
			}
			if(last>=0) {
				for(i=last+1;i<strlen(str_buf);i++) {
					str_buf[i-last-1]=str_buf[i];
				}
			}
			str_buf[strlen(str_buf)-last-1]='\0';
			tmp_s2[0]='\0';
			strcat(tmp_s2,"objects/");
			strcat(tmp_s2,str_buf);
			i=0;
			while(str_buf[i]=tmp_s2[i++]);
			//end specific

			for(i=0;i<strlen(str_buf);i++)
				if(str_buf[i]==0x5c) str_buf[i]=0x2f;  //'\' -> '/'
			MovingObject3D* tmp_obj;
//			tmp_obj=new MovingObject3D(str_buf);
			tmp_obj=new MovingObject3D();
			add_object(tmp_obj);
			obj_table[num_objects-1]->name=new char[strlen(str_buf)+1];
			strcpy(obj_table[num_objects-1]->name,str_buf);
			obj_table[num_objects-1]->is_dynamic=0;
		break;
		case OBJECT_MOTION:
			int num_keys;
			vfs->scanString(f,str_buf);
			vfs->scanInt(f,&num_keys);
			vfs->scanInt(f,&num_keys);

//			fscanf(f,"%s",str_buf);    //eat "(unnamed)"
//			fscanf(f,"%d",&num_keys);  //eat '9'
//			fscanf(f,"%d",&num_keys);
			obj_table[num_objects-1]->motion.alloc_keyframe_mem(num_keys);
			int linear,step;
			double tens,cont,bias;
			for(tmp=0;tmp<num_keys;tmp++) {
//				for(i=0;i<NUM_CHAN;i++) fscanf(f,"%f",&cv[i]);
				for(i=0;i<NUM_CHAN;i++) vfs->scanFloat(f,&cv[i]);
				vfs->scanInt(f,&step);
				vfs->scanInt(f,&linear);
				vfs->scanDouble(f,&tens);
				vfs->scanDouble(f,&cont);
				vfs->scanDouble(f,&bias);
//				fscanf(f,"%d",&step);
//				fscanf(f,"%d",&linear);
//				fscanf(f,"%lf",&tens);
//				fscanf(f,"%lf",&cont);
//				fscanf(f,"%lf",&bias);
				cv[2]=-cv[2];   //negate z coord
				obj_table[num_objects-1]->motion.add_key_frame(cv,tens,cont,bias,
					linear,step);
			}
			vfs->scanString(f,str_buf);
			vfs->scanInt(f,&tmp);
//			fscanf(f,"%s",str_buf); //EndBehavior
//			fscanf(f,"%d",&tmp);
			obj_table[num_objects-1]->motion.set_end_behavior(tmp);
		break;
		case PIVOT_POINT:
			Point3 pv;
			vfs->scanFloat(f,&pv[0]);
			vfs->scanFloat(f,&pv[1]);
			vfs->scanFloat(f,&pv[2]);
//			fscanf(f,"%f",&pv[0]);
//			fscanf(f,"%f",&pv[1]);
//			fscanf(f,"%f",&pv[2]);
			pv[2]=-pv[2];
			obj_table[num_objects-1]->set_pivot_point(pv);
		break;
		case PARENT_OBJECT:
			int p_id;
			vfs->scanInt(f,&p_id);
//			fscanf(f,"%d",&p_id);
			switch(current_read) {
			case 0:
				obj_table[num_objects-1]->parent_object=p_id;
			break;
			case 1:
				lights[num_lights-1]->parent_object=p_id;
			break;
			case 2:
				cam->parent_object=p_id;
			break;
			default:
#ifdef _LOG_EVERYTHING
				DebugFileMessage("warnings.log","Scene3D::load_LWS_file - wrong current read id for parent object\n");
#endif
			break;
			}
		break;
		case TARGET_OBJECT:
			int t_id;
			vfs->scanInt(f,&t_id);
//			fscanf(f,"%d",&t_id);
			switch(current_read) {
			case 0:
				obj_table[num_objects-1]->target_object=t_id;
			break;
			case 1:
				lights[num_lights-1]->target_object=t_id;
			break;
			case 2:
				cam->target_object=t_id;
			break;
			default:
#ifdef _LOG_EVERYTHING
				DebugFileMessage("warnings.log","Scene3D::load_LWS_file - wrong current read id for target object\n");
#endif
			break;
			}
		break;
		case ADD_NULL_OBJECT:
			current_read=0;
			tmp_obj=new MovingObject3D();
			add_object(tmp_obj);
		break;
		case AMBIENT_COLOR:
			vfs->scanInt(f,&tmp);
//			fscanf(f,"%d",&tmp);
			ambient[0]=(float)tmp/255.0f;
			vfs->scanInt(f,&tmp);
//			fscanf(f,"%d",&tmp);
			ambient[1]=(float)tmp/255.0f;
			vfs->scanInt(f,&tmp);
//			fscanf(f,"%d",&tmp);
			ambient[2]=(float)tmp/255.0f;
			ambient[3]=1.0f;
		break;
		case AMB_INTENSITY:
			vfs->scanString(f,str_buf);
//			fscanf(f,"%s",str_buf);
			if(strcmp("(envelope)",str_buf)==0) {
				p_env=read_envelope(f);
				ambient_intensity=*p_env;
				delete p_env;
			}
			else {
				float tmp_f;
				sscanf(str_buf,"%f",&tmp_f);
				ambient_intensity=tmp_f;
			}
		break;
		case ADD_LIGHT:
			if(num_lights<32) {
				current_read=1;
				lights[num_lights]=new MovingLight(GL_LIGHT0+num_lights);
				num_lights++;
				lights[num_lights-1]->obj_table=obj_table;
			}
			else more_lights_forbidden=1;
		break;
		case LIGHT_MOTION:
			if(!more_lights_forbidden) {
				vfs->scanString(f,str_buf);
				vfs->scanInt(f,&num_keys);
				vfs->scanInt(f,&num_keys);
//				fscanf(f,"%s",str_buf);
//				fscanf(f,"%s",str_buf);
//				fscanf(f,"%d",&num_keys);
				lights[num_lights-1]->motion.alloc_keyframe_mem(num_keys);
				for(tmp=0;tmp<num_keys;tmp++) {
					for(i=0;i<NUM_CHAN;i++) vfs->scanFloat(f,&cv[i]);
					vfs->scanInt(f,&step);
					vfs->scanInt(f,&linear);
					vfs->scanDouble(f,&tens);
					vfs->scanDouble(f,&cont);
					vfs->scanDouble(f,&bias);
//					for(i=0;i<NUM_CHAN;i++) fscanf(f,"%f",&cv[i]);
//						fscanf(f,"%d",&step);
//						fscanf(f,"%d",&linear);
//						fscanf(f,"%lf",&tens);
//						fscanf(f,"%lf",&cont);
//						fscanf(f,"%lf",&bias);
					cv[2]=-cv[2];   //negate z coord
					lights[num_lights-1]->motion.add_key_frame(cv,tens,cont,bias,
						linear,step);
				}
				vfs->scanString(f,str_buf);
				vfs->scanInt(f,&tmp);
//				fscanf(f,"%s",str_buf); //EndBehavior
//				fscanf(f,"%d",&tmp);
				lights[num_lights-1]->motion.set_end_behavior(tmp);
			}
		break;
		case LIGHT_COLOR:
			float col_r,col_g,col_b;
			if(!more_lights_forbidden) {
				vfs->scanInt(f,&tmp);
//				fscanf(f,"%d",&tmp);
				col_r=(float)tmp/255.0f;
				vfs->scanInt(f,&tmp);
//				fscanf(f,"%d",&tmp);
				col_g=(float)tmp/255.0f;
				vfs->scanInt(f,&tmp);
//				fscanf(f,"%d",&tmp);
				col_b=(float)tmp/255.0f;
				lights[num_lights-1]->set_color(col_r,col_g,col_b);
			}
#ifdef _LOG_EVERYTHING
			DebugMessage("Light %d has color %f %f %f\n",num_lights-1,col_r,col_g,col_b);
#endif
		break;
		case LGT_INTENSITY:
			if(!more_lights_forbidden) {
				vfs->scanString(f,str_buf);
//				fscanf(f,"%s",str_buf);
				if(strcmp("(envelope)",str_buf)==0) {
					p_env=read_envelope(f);
					lights[num_lights-1]->light_intensity=*p_env;
					delete p_env;
				}
				else {
					float tmp_f;
					sscanf(str_buf,"%f",&tmp_f);
					lights[num_lights-1]->light_intensity=tmp_f;
				}
			}
		break;
		case FALLOFF:
			if(!more_lights_forbidden) {
				vfs->scanString(f,str_buf);
//				fscanf(f,"%s",str_buf);
				if(strcmp("(envelope)",str_buf)==0) {
					p_env=read_envelope(f);
					lights[num_lights-1]->falloff=*p_env;
					delete p_env;
				}
				else {
					float tmp_f;
					sscanf(str_buf,"%f",&tmp_f);
					lights[num_lights-1]->falloff=tmp_f;
				}
			}
		break;
		case LIGHT_TYPE:
			if(!more_lights_forbidden) {
//				fscanf(f,"%d",&tmp);
				vfs->scanInt(f,&tmp);
				lights[num_lights-1]->set_type(tmp);
			}
		break;
		case CAMERA_MOTION:
			current_read=2;
			cam=new MovingCamera();
			cam->obj_table=obj_table;
			vfs->scanString(f,str_buf);
			vfs->scanInt(f,&num_keys);
			vfs->scanInt(f,&num_keys);
//			fscanf(f,"%s",str_buf);
//			fscanf(f,"%d",&num_keys);
//			fscanf(f,"%d",&num_keys);
			cam->motion.alloc_keyframe_mem(num_keys);
			for(tmp=0;tmp<num_keys;tmp++) {
				for(i=0;i<NUM_CHAN;i++) vfs->scanFloat(f,&cv[i]);
				vfs->scanInt(f,&step);
				vfs->scanInt(f,&linear);
				vfs->scanDouble(f,&tens);
				vfs->scanDouble(f,&cont);
				vfs->scanDouble(f,&bias);
//				for(i=0;i<NUM_CHAN;i++) fscanf(f,"%f",&cv[i]);
//				fscanf(f,"%d",&step);
//				fscanf(f,"%d",&linear);
//				fscanf(f,"%lf",&tens);
//				fscanf(f,"%lf",&cont);
//				fscanf(f,"%lf",&bias);
				cv[2]=-cv[2];   //negate z coord
				cam->motion.add_key_frame(cv,tens,cont,bias,
					linear,step);
			}
			vfs->scanString(f,str_buf);
			vfs->scanInt(f,&tmp);
//			fscanf(f,"%s",str_buf); //EndBehavior
//			fscanf(f,"%d",&tmp);
			cam->motion.set_end_behavior(tmp);
		break;
		case ZOOM_FACTOR:
			vfs->scanString(f,str_buf);
//			fscanf(f,"%s",str_buf);
			if(strcmp("(envelope)",str_buf)==0) {
				p_env=read_envelope(f);
				cam->zoom_factor=*p_env;
				delete p_env;
			}
			else {
				float tmp_f;
				sscanf(str_buf,"%f",&tmp_f);
				cam->zoom_factor=tmp_f;
			}
		break;
		case CONE_ANGLE:
			vfs->scanString(f,str_buf);
//			fscanf(f,"%s",str_buf);
			if(strcmp("(envelope)",str_buf)==0) {
				read_envelope(f,env);
				lights[num_lights-1]->cone_angle=env;
			}
			else {
				float tmp_f;
				sscanf(str_buf,"%f",&tmp_f);
				lights[num_lights-1]->cone_angle=tmp_f;
			}
		break;
		case GLOBAL_FLARE_INTENSITY:
			vfs->scanString(f,str_buf);
//			fscanf(f,"%s",str_buf);
			if(strcmp("(envelope)",str_buf)==0) {
				p_env=read_envelope(f);
				global_flare_intensity=*p_env;
				delete p_env;
			}
			else {
				float tmp_f;
				sscanf(str_buf,"%f",&tmp_f);
				global_flare_intensity=tmp_f;
			}
		break;
		case ENABLE_LENS_FLARE:
			enable_lens_flare=0;
		break;
		case LENS_FLARE:
			if(!more_lights_forbidden) {
				lights[num_lights-1]->lens_flare=1;
			}
		break;
		case FLARE_INTENSITY:
			if(!more_lights_forbidden) {
				vfs->scanString(f,str_buf);
//				fscanf(f,"%s",str_buf);
				if(strcmp("(envelope)",str_buf)==0) {
					p_env=read_envelope(f);
					lights[num_lights-1]->flare_intensity=*p_env;
					delete p_env;
				}
				else {
					float tmp_f;
					sscanf(str_buf,"%f",&tmp_f);
					lights[num_lights-1]->flare_intensity=tmp_f;
				}
			}
		break;
		case FOG_TYPE:
			vfs->scanInt(f,&tmp);
//			fscanf(f,"%d",&tmp);
			fog_type=tmp;
		break;
		case FOG_MIN_DIST:
			vfs->scanString(f,str_buf);
//			fscanf(f,"%s",str_buf);
			if(strcmp("(envelope)",str_buf)==0) {
				p_env=read_envelope(f);
				fog_start=*p_env;
				delete p_env;
			}
			else {
				float tmp_f;
				sscanf(str_buf,"%f",&tmp_f);
				fog_start=tmp_f;
			}
		break;
		case FOG_MAX_DIST:
			vfs->scanString(f,str_buf);
//			fscanf(f,"%s",str_buf);
			if(strcmp("(envelope)",str_buf)==0) {
				p_env=read_envelope(f);
				fog_end=*p_env;
				delete p_env;
			}
			else {
				float tmp_f;
				sscanf(str_buf,"%f",&tmp_f);
				fog_end=tmp_f;
			}
		break;
		case FOG_COLOR:
			vfs->scanInt(f,&tmp);
//			fscanf(f,"%d",&tmp);
			fog_color[0]=(float)tmp/255.0f;
			vfs->scanInt(f,&tmp);
//			fscanf(f,"%d",&tmp);
			fog_color[1]=(float)tmp/255.0f;
			vfs->scanInt(f,&tmp);
//			fscanf(f,"%d",&tmp);
			fog_color[2]=(float)tmp/255.0f;
			fog_color[3]=1.0f;
		break;
		case BACKDROP_COLOR:
			vfs->scanInt(f,&tmp);
//			fscanf(f,"%d",&tmp);
			backdrop_color[0]=(float)tmp/255.0f;
			vfs->scanInt(f,&tmp);
//			fscanf(f,"%d",&tmp);
			backdrop_color[1]=(float)tmp/255.0f;
			vfs->scanInt(f,&tmp);
//			fscanf(f,"%d",&tmp);
			backdrop_color[2]=(float)tmp/255.0f;
			backdrop_color[3]=1.0f;
		break;
		case BACKDROP_FOG:
			vfs->scanInt(f,&tmp);
//			fscanf(f,"%d",&tmp);
			backdrop_fog=tmp;
		break;
		case METAMORPH:
			vfs->scanString(f,str_buf);
//			fscanf(f,"%s",str_buf);
			if(strcmp("(envelope)",str_buf)==0) {
				p_env=read_envelope(f);
				obj_table[num_objects-1]->metamorph=*p_env;
				delete p_env;
			}
			else {
				float tmp_f;
				sscanf(str_buf,"%f",&tmp_f);
				obj_table[num_objects-1]->metamorph=tmp_f;
			}
		break;
		case MORPH_TARGET:
			vfs->scanInt(f,&tmp);
			obj_table[num_objects-1]->morph_target=tmp;
		break;
		}
	}
	vfs->closeFile(f);
//	int* crash;
//	crash[1000000]=0;
	for(i=0;i<num_objects;i++) obj_table[i]->is_dynamic=0;
	for(i=0;i<num_objects;i++) {
		if(obj_table[i]->morph_target) {
			obj_table[i]->is_dynamic=1;
			obj_table[obj_table[i]->morph_target-1]->is_dynamic=1;
		}
	}

	for(i=0;i<num_objects;i++) 	{
		obj_table[i]->load_object();
		obj_table[i]->obj_table=obj_table;
	}
	for(i=0;i<num_lights;i++) 	lights[i]->obj_table=obj_table;
	cam->obj_table=obj_table;
	is_transparent=new int[num_objects];
	is_additive=new int[num_objects];
	bbox_centers=new float[num_objects];
	num_transparent_objects=0;
	for(i=0;i<num_objects;i++) {
		if(obj_table[i]->obj) {
			if((obj_table[i]->obj->has_transparent_surfaces())
				&& !(obj_table[i]->obj->has_additive_surfaces())) {
				num_transparent_objects++;
				is_transparent[i]=1;
			}
			else {
				is_transparent[i]=0;
			}

			if(obj_table[i]->obj->has_additive_surfaces()) {
				is_additive[i]=1;
				num_additive_objects++;
			}
			else {
				is_additive[i]=0;
			}
		}
		else {
			is_transparent[i]=0;
			is_additive[i]=0;
		}
	}
	transparent_order=new int[num_transparent_objects];
	for(i=0;i<num_objects;i++) 	{
		obj_table[i]->setup_morphing();
	}
#ifdef _LOG_EVERYTHING
	DebugMessage("Dumping all objects - see objdump.log for details\n");
	DebugFileSetup("objdump.log");
	for(i=0;i<num_objects;i++) {
		if(obj_table[i]->obj) obj_table[i]->obj->dump("objdump.log");
	}
#endif
}

void Scene3D::optimize() {  //this one is meaningless now
}

void Scene3D::render_flares() {

	Vector3 view_dir,tmp,light_dir,dx,dy,axis,sx,sy,pos,center;
	Point3 from,light_pos;
	Vector3 old_light_pos,camera_up,ux,uy;  //for flare visibility checking
	global_flare_intensity.calc(frame);
	cam->get_direction(view_dir);
	VNormalize3(view_dir);

	cam->get_position(from);

	float near_clip=2.0f*cam->get_near_clip();
	tmp[0]=view_dir[0];
	tmp[1]=view_dir[1];
	tmp[2]=view_dir[2];
	VSMultiply3(tmp,near_clip);
	VAdd3(tmp,from,center);

	float blind_color[4];
	blind_color[0]=0.0f;
	blind_color[1]=0.0f;
	blind_color[2]=0.0f;
	blind_color[3]=0.0f;

	unsigned int i;
	for(i=0;i<num_lights;i++) {
		if(lights[i]->lens_flare) {
			lights[i]->get_position(light_pos);
			lights[i]->flare_intensity.calc(frame);
			PDiff3(from,light_pos,light_dir);
			VNormalize3(light_dir);

			float dot;
			dot = VDotProduct3(light_dir,view_dir);

			old_light_pos[0]=light_pos[0];
			old_light_pos[1]=light_pos[1];
			old_light_pos[2]=light_pos[2];

			/* light_pos = from + dot(light_dir,view_dir)*near_clip*light_dir */
			tmp[0]=light_dir[0];
			tmp[1]=light_dir[1];
			tmp[2]=light_dir[2];
			VSMultiply3(tmp,near_clip*dot);
			VAdd3(from,tmp,light_pos);

			/* axis = light - center */
			PDiff3(center,light_pos,axis);
	
			VCrossProduct3(view_dir,axis,tmp);
			VCrossProduct3(tmp,view_dir,axis);
			
			dx[0]=axis[0];
			dx[1]=axis[1];
			dx[2]=axis[2];
			VNormalize3(dx);
			
			VCrossProduct3(dx,view_dir,dy);
			
			//here we draw flares for light i
			float color[4];
			int num_flares=5;
			int j=0;

			//check flare visibility
			Matrix cam_mtx,frustum_mtx;
			Vector4 transformed_light,tmp_v41,tmp_v42;
			float depth_buf[16];
			float depth_buf2[16];
			int depth_change=0;
			cam->get_camera_matrix(cam_mtx);
			cam->get_frustum_matrix(frustum_mtx);
			tmp_v41[0]=old_light_pos[0];
			tmp_v41[1]=old_light_pos[1];
			tmp_v41[2]=old_light_pos[2];
			tmp_v41[3]=1.0f;
			MVMultiply4(cam_mtx,tmp_v41,tmp_v42);
			MVMultiply4(frustum_mtx,tmp_v42,transformed_light);
			transformed_light[0]=transformed_light[0]/transformed_light[3];
			transformed_light[1]=transformed_light[1]/transformed_light[3];
			transformed_light[0]=width/2*(transformed_light[0]+1.0f);
			transformed_light[1]=height/2*(transformed_light[1]+1.0f);
			int x,y;
			x=(int)transformed_light[0];
			y=(int)transformed_light[1];
			if((x>2) && (x<width-2)
				&& (y>2) && (y<height-2)) {
				glReadPixels(x-2,y-2,4,4,GL_DEPTH_COMPONENT,GL_FLOAT,depth_buf);

				Vector3 ld_far,ld_near;
				PDiff3(old_light_pos,from,ld_far);
				PDiff3(light_pos,from,ld_near);
				float scale_factor=VMagnitude3(ld_far)/VMagnitude3(ld_near);
				cam->get_up_vector(camera_up);
				VNormalize3(camera_up);
				ux[0]=camera_up[0];
				ux[1]=camera_up[1];
				ux[2]=camera_up[2];
				VCrossProduct3(ux,view_dir,uy);
				VCrossProduct3(view_dir,uy,ux);
				VNormalize3(uy);
				VNormalize3(ux);
				
				sx[0]=ux[0];
				sx[1]=ux[1];
				sx[2]=ux[2];
				VSMultiply3(sx,4*0.532f/width*scale_factor);	//8 pikseli ekranowych
				
				sy[0]=uy[0];
				sy[1]=uy[1];
				sy[2]=uy[2];
				VSMultiply3(sy,4*0.4f/height*scale_factor);		//8 pikseli ekranowych
				
				c_state->want_enabled(GL_DEPTH_TEST);
				tmp[0]=axis[0];
				tmp[1]=axis[1];
				tmp[2]=axis[2];
				
				pos[0]=old_light_pos[0];
				pos[1]=old_light_pos[1];
				pos[2]=old_light_pos[2];
				
				glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
				glBegin(GL_QUADS);
				
				glTexCoord2f(0.0f,0.0f);
				VAdd3(pos,sx,tmp);
				VAdd3(tmp,sy,tmp);
				glVertex3fv(tmp);
				
				
				glTexCoord2f(1.0f,0.0f);
				PDiff3(sx,pos,tmp);
				VAdd3(tmp,sy,tmp);
				glVertex3fv(tmp);
				
				glTexCoord2f(1.0f,1.0f);
				PDiff3(sx,pos,tmp);
				PDiff3(sy,tmp,tmp);
				glVertex3fv(tmp);
				
				
				glTexCoord2f(0.0f,1.0f);
				VAdd3(sx,pos,tmp);
				PDiff3(sy,tmp,tmp);
				glVertex3fv(tmp);
				
				glEnd();
				glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
				glReadPixels(x-2,y-2,4,4,GL_DEPTH_COMPONENT,GL_FLOAT,depth_buf2);
				for(j=0;j<16;j++) if(depth_buf[j]!=depth_buf2[j]) depth_change=1;
			}
			////////////////////
			if(depth_change) {
				lights[i]->appear_intensity+=(frame-lights[i]->last_vis_check)/
					(frames_per_second*flare_falloff_time);
				if(lights[i]->appear_intensity>1.0f) lights[i]->appear_intensity=1.0f;
				lights[i]->last_vis_check=frame;
			}
			else {
				lights[i]->appear_intensity-=(frame-lights[i]->last_vis_check)/
					(frames_per_second*flare_falloff_time);
				if(lights[i]->appear_intensity<0.0f) lights[i]->appear_intensity=0.0f;
				lights[i]->last_vis_check=frame;
			}
			if(lights[i]->appear_intensity!=0.0f) {
//				lights[i]->get_color(&color[0],&color[1],
//					&color[2],&color[3]);
				color[0]=color[1]=color[2]=1.0f;
				color[3]=global_flare_intensity.res*lights[i]->flare_intensity.res
					*lights[i]->appear_intensity;
				//calculate the accumulated blinding effect
				float blind_factor;
				blind_factor=dot;
				blind_factor*=blind_factor;
				blind_factor*=blind_factor;    //dot^4
				blind_factor*=blind_factor;
				blind_factor*=blind_factor;    //dot^16
				blind_factor*=blind_factor;
				blind_factor*=blind_factor;    //dot^64
				blind_factor*=blind_factor;
				blind_factor*=blind_factor;    //dot^256
				blind_color[0]+=blind_factor*lights[i]->appear_intensity*color[0];
				blind_color[1]+=blind_factor*lights[i]->appear_intensity*color[1];
				blind_color[2]+=blind_factor*lights[i]->appear_intensity*color[2];
				//draw the flare
				glColor4fv(color);
				unsigned int k;
				for(k=0;k<max_texture_units;k++) {
					c_state->textures[k].clear_tex_gen_flags();
					c_state->textures[k].enabled=0;
				}
				c_state->textures[0].enabled=1;

				c_state->want_disabled(GL_DEPTH_TEST);
				c_state->want_enabled(GL_BLEND);
				c_state->want_disabled(GL_LIGHTING);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
				for(j=0;j<num_flares;j++) {
					sx[0]=dx[0];
					sx[1]=dx[1];
					sx[2]=dx[2];
					float flare_factor;
					flare_factor=0.03f*(num_flares-j);
					VSMultiply3(sx,scene_size_factor*flare_factor);		//rozmiar flary
					
					sy[0]=dy[0];
					sy[1]=dy[1];
					sy[2]=dy[2];
					VSMultiply3(sy,scene_size_factor*flare_factor);		//rozmiar flary
					
					c_state->textures[0].tex=flare_tex[j];
					tmp[0]=axis[0];
					tmp[1]=axis[1];
					tmp[2]=axis[2];
					VSMultiply3(tmp,-0.5f*(float)(j));
					
					VAdd3(light_pos,tmp,pos);
					
					c_state->prerender_setup();
					glBegin(GL_QUADS);
					
					glTexCoord2f(0.0f,0.0f);
					VAdd3(pos,sx,tmp);
					VAdd3(tmp,sy,tmp);
					glVertex3fv(tmp);
					
					
					glTexCoord2f(1.0f,0.0f);
					PDiff3(sx,pos,tmp);
					VAdd3(tmp,sy,tmp);
					glVertex3fv(tmp);
					
					glTexCoord2f(1.0f,1.0f);
					PDiff3(sx,pos,tmp);
					PDiff3(sy,tmp,tmp);
					glVertex3fv(tmp);
					
					
					glTexCoord2f(0.0f,1.0f);
					VAdd3(sx,pos,tmp);
					PDiff3(sy,tmp,tmp);
					glVertex3fv(tmp);
					
					glEnd();
				}
			}
		}
	}
	if((blind_color[0]!=0.0f) ||
		(blind_color[1]!=0.0f) ||
		(blind_color[2]!=0.0f)) {
		if(blind_color[0]>1.0f) blind_color[0]=1.0f;
		if(blind_color[1]>1.0f) blind_color[1]=1.0f;
		if(blind_color[2]>1.0f) blind_color[2]=1.0f;
		blind_color[3]=(blind_color[0]+blind_color[1]+
			blind_color[2])*0.3333333f;
		if((rfx!=NULL) &&
			(rfx->blinding_flares)) {
			glColor4fv(blind_color);
			DrawQuad(NULL,0.0f,0.0f,1.0f,1.0f,
				0.0f,0.0f,1.0f,1.0f,1,GL_ONE,GL_ONE);
		}
	}
}

static int check_plane(BBox* bb,float plane[4]) {
	if((bb->p1[0]*plane[0]+bb->p1[1]*plane[1]+bb->p1[2]*plane[2]+plane[3])<0.0f) return 0;
	if((bb->p2[0]*plane[0]+bb->p2[1]*plane[1]+bb->p2[2]*plane[2]+plane[3])<0.0f) return 0;
	if((bb->p3[0]*plane[0]+bb->p3[1]*plane[1]+bb->p3[2]*plane[2]+plane[3])<0.0f) return 0;
	if((bb->p4[0]*plane[0]+bb->p4[1]*plane[1]+bb->p4[2]*plane[2]+plane[3])<0.0f) return 0;
	if((bb->p5[0]*plane[0]+bb->p5[1]*plane[1]+bb->p5[2]*plane[2]+plane[3])<0.0f) return 0;
	if((bb->p6[0]*plane[0]+bb->p6[1]*plane[1]+bb->p6[2]*plane[2]+plane[3])<0.0f) return 0;
	if((bb->p7[0]*plane[0]+bb->p7[1]*plane[1]+bb->p7[2]*plane[2]+plane[3])<0.0f) return 0;
	if((bb->p8[0]*plane[0]+bb->p8[1]*plane[1]+bb->p8[2]*plane[2]+plane[3])<0.0f) return 0;
	return 1;
}

int cmp_centers(const void* c1,const void* c2) {
	if((current_scene->bbox_centers[*((int*)c1)])>(current_scene->bbox_centers[*((int*)c2)])) {
		return -1;
	}
	else if((current_scene->bbox_centers[*((int*)c1)])<(current_scene->bbox_centers[*((int*)c2)])) {
		return 1;
	}
	else return 0;
}

void Scene3D::render(int time) {
	current_scene=this;
	if(!cam) {
#ifdef _LOG_EVERYTHING
		DebugFileMessage("warnings.log","Scene3D::render - no camera!\n");
#endif
		return;
	}
	frame=frames_per_second*((float)time/1000.0f);
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Entered frame renderer ...\n");
	DebugFrameMessage("Frame serial number is %d\n",DebugGetFrame());
	DebugFrameMessage("Frame time in frames :))) is %f\n",frame);
	DebugMessage("Rendering frame %d\n",DebugGetFrame());
#endif
	c_state->want_enabled(GL_LIGHTING);
	c_state->want_enabled(GL_DEPTH_TEST);
	c_state->want_disabled(GL_BLEND);
	unsigned int i;
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Calculating lights positions and rotations ...\n");
#endif
	for(i=0;i<num_lights;i++) if(lights[i]!=NULL) lights[i]->motion.calc(frame);
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Disabling unneccesary lights ...\n");
#endif
	if(num_lights<8) for(i=((num_lights<max_lights) ? num_lights : max_lights);i<8;i++)
		c_state->want_disabled(GL_LIGHT0+i);
	float tmp_amb[4];
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Calculating and setting global ambient intensity ...\n");
#endif
	ambient_intensity.calc(frame);
	tmp_amb[0]=ambient[0]*ambient_intensity.res;
	tmp_amb[1]=ambient[1]*ambient_intensity.res;
	tmp_amb[2]=ambient[2]*ambient_intensity.res;
	tmp_amb[3]=1.0f;
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,tmp_amb);
	//calc the transforms and set up camera
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("There are %d objects in the scene\n",num_objects);
	DebugFrameMessage("Calculating transforms for all objects ...\n");
#endif
	for(i=0;i<num_objects;i++) obj_table[i]->calc_transform(frame);
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Setting up camera ...\n");
#endif
	cam->draw(frame);
	//set up fog
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Checking for fog ...\n");
#endif
	if(fog_type) {
		fog_start.calc(frame);
		fog_end.calc(frame);
		glFogf(GL_FOG_START,fog_start.res);
		glFogf(GL_FOG_END,fog_end.res);
#ifdef _LOG_EVERYTHING
		DebugFrameMessage("Fog start: %f\n",fog_start.res);
		DebugFrameMessage("Fog end: %f\n",fog_end.res);
#endif
		if(backdrop_fog) {
			glFogfv(GL_FOG_COLOR,backdrop_color);
			glColor4fv(backdrop_color);
		}
		else {
			glFogfv(GL_FOG_COLOR,fog_color);
			glColor4fv(fog_color);
		}
		DrawQuad(NULL);
		c_state->want_enabled(GL_FOG);
	}
	else {
		c_state->want_disabled(GL_FOG);
	}
	//draw the scene
	c_state->want_enabled(GL_LIGHTING);
	c_state->want_enabled(GL_DEPTH_TEST);
	c_state->want_disabled(GL_BLEND);
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Setting up lights ...\n");
#endif
	for(i=0;i<num_lights;i++) c_state->want_enabled(GL_LIGHT0+i);
	for(i=0;i<num_lights;i++) lights[i]->draw(frame);
	float left_p[4], right_p[4], top_p[4],bottom_p[4], near_p[4], far_p[4];
	cam->get_transformed_frustum(left_p,right_p,top_p,bottom_p,
		near_p,far_p);
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Transformed camera frustum is:\n");
	DebugFrameMessage("\tleft plane: %f\t%f\t%f\t%f\n",left_p[0],left_p[1],left_p[2],left_p[3]);
	DebugFrameMessage("\tright plane: %f\t%f\t%f\t%f\n",right_p[0],right_p[1],right_p[2],right_p[3]);
	DebugFrameMessage("\ttop plane: %f\t%f\t%f\t%f\n",top_p[0],top_p[1],top_p[2],top_p[3]);
	DebugFrameMessage("\tbottom plane: %f\t%f\t%f\t%f\n",bottom_p[0],bottom_p[1],bottom_p[2],bottom_p[3]);
	DebugFrameMessage("\tnear plane: %f\t%f\t%f\t%f\n",near_p[0],near_p[1],near_p[2],near_p[3]);
	DebugFrameMessage("\tfar plane: %f\t%f\t%f\t%f\n",far_p[0],far_p[1],far_p[2],far_p[3]);
#endif
	BBox bbox;
	int clip_it;
	unsigned int tmp=0;
	Point3 cam_pos;
	cam->get_position(cam_pos);
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Camera position:\t%f\t%f\t%f\n",cam_pos[0],cam_pos[1],
		cam_pos[2]);
#endif
//	Point3 tmp_pt;
//	Vector3 tmp_v;
	//render the stuff
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Rendering non-transparent objects\n");
#endif
	for(i=0;i<num_objects;i++) {
		if(obj_table[i]->obj) {
			current_object=obj_table[i];
			obj_table[i]->get_transformed_bbox(&bbox);
			clip_it=check_plane(&bbox,left_p);
			if(!clip_it) clip_it=check_plane(&bbox,right_p);
			if(!clip_it) clip_it=check_plane(&bbox,top_p);
			if(!clip_it) clip_it=check_plane(&bbox,bottom_p);
			if(!clip_it) clip_it=check_plane(&bbox,near_p);
			if(!clip_it) clip_it=check_plane(&bbox,far_p);
			if(!clip_it) {
				if(is_transparent[i] || is_additive[i]) {
					glDepthMask(GL_FALSE);
					c_state->want_enabled(GL_BLEND);
//					c_state->lock_state(GL_BLEND);
				}
				obj_table[i]->render_solid_surfaces(frame);
				if(obj_table[i]->obj->has_additive_surfaces()) {
					obj_table[i]->render_additive_surfaces(frame);					
				}
				obj_table[i]->render_transparent_surfaces(frame);
				if(is_transparent[i] || is_additive[i]) {
					glDepthMask(GL_TRUE);
//					c_state->unlock_state(GL_BLEND);
				}
			}
		}
	}
	//render possible solid procedural objects
	for(i=0;i<num_solid_procedurals;i++) {
		solid_procedurals[i]->get_bounding_box(&bbox);
		clip_it=check_plane(&bbox,left_p);
		if(!clip_it) clip_it=check_plane(&bbox,right_p);
		if(!clip_it) clip_it=check_plane(&bbox,top_p);
		if(!clip_it) clip_it=check_plane(&bbox,bottom_p);
		if(!clip_it) clip_it=check_plane(&bbox,near_p);
		if(!clip_it) clip_it=check_plane(&bbox,far_p);
		if(!clip_it) {
			solid_procedurals[i]->render(frame);
		}
	}
/*	//sort transparent objects
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("Depth sorting transparent objects ...\n");
#endif
	qsort((void*)transparent_order,(size_t)tmp,sizeof(int),cmp_centers);
	//render transparent objects
	for(i=0;i<tmp;i++) {
		obj_table[transparent_order[i]]->get_transformed_bbox(&bbox);
		clip_it=check_plane(&bbox,left_p);
		if(!clip_it) clip_it=check_plane(&bbox,right_p);
		if(!clip_it) clip_it=check_plane(&bbox,top_p);
		if(!clip_it) clip_it=check_plane(&bbox,bottom_p);
		if(!clip_it) clip_it=check_plane(&bbox,near_p);
		if(!clip_it) clip_it=check_plane(&bbox,far_p);
		if(!clip_it) obj_table[transparent_order[i]]->render(frame);
	}
	//render possible transparent procedural objects
	for(i=0;i<num_transparent_procedurals;i++) {
		transparent_procedurals[i]->get_bounding_box(&bbox);
		clip_it=check_plane(&bbox,left_p);
		if(!clip_it) clip_it=check_plane(&bbox,right_p);
		if(!clip_it) clip_it=check_plane(&bbox,top_p);
		if(!clip_it) clip_it=check_plane(&bbox,bottom_p);
		if(!clip_it) clip_it=check_plane(&bbox,near_p);
		if(!clip_it) clip_it=check_plane(&bbox,far_p);
		if(!clip_it) {
			transparent_procedurals[i]->render(frame);
		}
	}
	//render addive objects in the scene
	if(num_additive_objects) {
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		c_state->want_enabled(GL_BLEND);
		c_state->lock_state(GL_BLEND);
		for(i=0;i<num_objects;i++) {
			if(obj_table[i]->has_additive_surfaces()) {
				obj_table[i]->get_transformed_bbox(&bbox);
				clip_it=check_plane(&bbox,left_p);
				if(!clip_it) clip_it=check_plane(&bbox,right_p);
				if(!clip_it) clip_it=check_plane(&bbox,top_p);
				if(!clip_it) clip_it=check_plane(&bbox,bottom_p);
				if(!clip_it) clip_it=check_plane(&bbox,near_p);
				if(!clip_it) clip_it=check_plane(&bbox,far_p);
				if(!clip_it) obj_table[i]->render_additive_surfaces(frame);
			}
		}
		glDepthMask(GL_TRUE);
		c_state->unlock_state(GL_BLEND);
	}*/
	if(enable_lens_flare) render_flares();
}