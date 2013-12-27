/**********************************************************************************************
 Life Harmony height field effect implementation
 (c) 2000 Maciej Mroz
**********************************************************************************************/

#include "lh_height_field.h"
#include "debug_msg.h"

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float

LH_Heightfield::LH_Heightfield(Texture *texture,Texture *env,Image *heightmap, int divisions,
							 float size,float heightmap_force, float gravity_force,
							 float spring_force) {
	tex=texture;
	envmap=env;
	hmap=heightmap;
	div_num=divisions;
	field_size=size;
	hmap_force_factor=heightmap_force;
	gravity=gravity_force;
	spring_factor=spring_force;
	vertex_buffer=new float[3*(div_num+3)*(div_num+3)];
	normal_buffer=new float[3*(div_num+3)*(div_num+3)];
	texcoord_buffer=new float[2*(div_num+3)*(div_num+3)];

	velocity_buffer=new float[(div_num+3)*(div_num+3)];
	force_buffer=new float[(div_num+3)*(div_num+3)];
	int i,j;
	for(i=0;i<div_num+3;i++) {
		for(j=0;j<div_num+3;j++) {
			vertex_buffer[3*(i*(div_num+3)+j)]=-size+j*(2*size)/(div_num+1);
			vertex_buffer[3*(i*(div_num+3)+j)+1]=0.0f;
			vertex_buffer[3*(i*(div_num+3)+j)+2]=-size+i*(2*size)/(div_num+1);

			normal_buffer[3*(i*(div_num+3)+j)]=0.0f;
			normal_buffer[3*(i*(div_num+3)+j)+1]=1.0f;
			normal_buffer[3*(i*(div_num+3)+j)+2]=0.0f;

			texcoord_buffer[2*(i*(div_num+3)+j)]=j/(float)(div_num+1);
			texcoord_buffer[2*(i*(div_num+3)+j)+1]=i/(float)(div_num+1);

			velocity_buffer[(i*(div_num+3)+j)]=0.0f;

			force_buffer[(i*(div_num+3)+j)]=0.0f;
		}
	}
	index_buffer=new int[(div_num+2)*(div_num+3)*2];  //num_strips*vert_per_strip
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
	last_frame=0;
	hmap_strength=heightmap_force;
	hmap_blend_factor=1.0f;
}

LH_Heightfield::~LH_Heightfield() {
	if(vertex_buffer) delete vertex_buffer;
	if(normal_buffer) delete normal_buffer;
	if(texcoord_buffer) delete texcoord_buffer;
	if(velocity_buffer) delete velocity_buffer;
	if(force_buffer) delete force_buffer;
}

void LH_Heightfield::update(float frame) {
	int i,j;
	int tmp_ind;
	float time=frame*0.033333333f;
	float time_dif=time-last_frame*0.033333333f;
//	if(time_dif>0.03) time_dif=0.03;                   //just a hack!!!!!!!!!!!!!!!!!!!!!!!!
	last_frame=frame;
	float wave1_len=4.0f+sin(2.1*time);
	float wave2_len=5.0f+sin(1.8*time);
	if(hmap) {
		int x,y;
		for(i=1;i<(div_num+2);i++) {
			for(j=1;j<(div_num+2);j++) {
				x=(int)((j/(float)(div_num+1))*hmap->width);
				y=(int)((i/(float)(div_num+1))*hmap->height);
				vertex_buffer[3*(i*(div_num+3)+j)+1]=
					hmap_blend_factor*
					(hmap->pixel_buf[4*(y*hmap->width+x)]/255.0f*hmap_strength)+
					(1.0f-hmap_blend_factor)*0.1f*
					(sin(vertex_buffer[3*(i*(div_num+3)+j)]*wave1_len+3.14*time)+
					sin(vertex_buffer[3*(i*(div_num+3)+j)+2]*wave2_len+3.14*time));
			}
		}
	}

	//spring matrix code :))))
/*	if(hmap) {
		int x,y;
		for(i=1;i<(div_num+2);i++) {
			for(j=1;j<(div_num+2);j++) {
				x=(int)((j/(float)(div_num+1))*hmap->width);
				y=(int)((i/(float)(div_num+1))*hmap->height);
				vertex_buffer[3*(i*(div_num+3)+j)+1]=
					hmap->pixel_buf[4*(y*hmap->width+x)]/255.0f*hmap_force_factor;
			}
		}
	}
	else {


	//calculate forces
	for(i=1;i<(div_num+2);i++) {
		for(j=1;j<(div_num+2);j++) {
			tmp_ind=(i*(div_num+3)+j);
			force_buffer[tmp_ind]=
				(vertex_buffer[3*((i+1)*(div_num+3)+j)+1]-vertex_buffer[3*tmp_ind+1]+
				vertex_buffer[3*((i-1)*(div_num+3)+j)+1]-vertex_buffer[3*tmp_ind+1]+
				vertex_buffer[3*(i*(div_num+3)+j+1)+1]-vertex_buffer[3*tmp_ind+1]+
				vertex_buffer[3*(i*(div_num+3)+j-1)+1]-vertex_buffer[3*tmp_ind+1])
				*spring_factor-gravity;  //+hmap_force
		}
	}
	//update velocities
	for(i=1;i<(div_num+2);i++) {
		for(j=1;j<(div_num+2);j++) {
			velocity_buffer[(i*(div_num+3)+j)]+=time_dif*force_buffer[(i*(div_num+3)+j)];
		}
	}
	//update positions
	for(i=1;i<(div_num+2);i++) {
		for(j=1;j<(div_num+2);j++) {
			vertex_buffer[3*(i*(div_num+3)+j)+1]+=time_dif*velocity_buffer[(i*(div_num+3)+j)];
		}
	}
	}*/
	//(almost) true normals
	Vector3 v1,v2,v_tmp_cross;
	for(i=1;i<(div_num+2);i++) {
		for(j=1;j<(div_num+2);j++) {
			tmp_ind=(i*(div_num+3)+j);
			normal_buffer[3*tmp_ind]=0.0f;
			normal_buffer[3*tmp_ind+1]=0.0f;
			normal_buffer[3*tmp_ind+2]=0.0f;
			PDiff3(&vertex_buffer[3*tmp_ind],
				&vertex_buffer[3*(i*(div_num+3)+j-1)],
				v1);
			PDiff3(&vertex_buffer[3*tmp_ind],
				&vertex_buffer[3*((i+1)*(div_num+3)+j)],
				v2);
			VCrossProduct3(v1,v2,v_tmp_cross);
			VAdd3(&normal_buffer[3*tmp_ind],
				v_tmp_cross,
				&normal_buffer[3*tmp_ind]);
			//pierwszy potrzebny wektor jest juz w v2
			PDiff3(&vertex_buffer[3*tmp_ind],
				&vertex_buffer[3*(i*(div_num+3)+j+1)],
				v1);
			VCrossProduct3(v2,v1,v_tmp_cross);
			VAdd3(&normal_buffer[3*tmp_ind],
				v_tmp_cross,
				&normal_buffer[3*tmp_ind]);
			//pierwszy potrzebny wektor jest juz w v1
			PDiff3(&vertex_buffer[3*tmp_ind],
				&vertex_buffer[3*((i-1)*(div_num+3)+j)],
				v2);
			VCrossProduct3(v1,v2,v_tmp_cross);
			VAdd3(&normal_buffer[3*tmp_ind],
				v_tmp_cross,
				&normal_buffer[3*tmp_ind]);
			//pierwszy potrzebny wektor jest juz w v2
			PDiff3(&vertex_buffer[3*tmp_ind],
				&vertex_buffer[3*(i*(div_num+3)+j-1)],
				v1);
			VCrossProduct3(v2,v1,v_tmp_cross);
			VAdd3(&normal_buffer[3*tmp_ind],
				v_tmp_cross,
				&normal_buffer[3*tmp_ind]);
		}
	}
}

void LH_Heightfield::render() {
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

	if(hasMultitexture) {
		c_state->textures[1].enabled=1;
		c_state->textures[1].set_texture_env(GL_ADD);
		c_state->textures[1].tex=envmap;
		c_state->textures[1].tex_gen_s_enabled=1;
		c_state->textures[1].tex_gen_s_mode=GL_SPHERE_MAP;
		c_state->textures[1].tex_gen_t_enabled=1;
		c_state->textures[1].tex_gen_t_mode=GL_SPHERE_MAP;
	}

	c_state->prerender_setup();
	float tmp_col[4]={1.0f,1.0f,1.0f,1.0f};
	float zero[4]={0.0f,0.0f,0.0f,0.0f};
	float zero2[4]={1.0f,1.0f,1.0f,0.0f};
	c_state->want_enabled(GL_DEPTH_TEST);
	c_state->want_enabled(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ZERO);
	c_state->want_enabled(GL_LIGHTING);
	c_state->want_enabled(GL_CULL_FACE);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,tmp_col);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,zero2);
	glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,zero);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,zero);
	glColor4fv(tmp_col);

	glVertexPointer(3,GL_FLOAT,0,vertex_buffer);
	glNormalPointer(GL_FLOAT,0,normal_buffer);

	if(hasCompiledVertexArray) {
		glLockArraysEXT(0,(div_num+3)*(div_num+3));
	}
	for(i=0;i<(div_num+2);i++) {
		glDrawElements(GL_TRIANGLE_STRIP,2*(div_num+3),
			GL_UNSIGNED_INT,&index_buffer[2*(div_num+3)*i]);
	}
	if(hasCompiledVertexArray) {
		glUnlockArraysEXT();
	}
}

void LH_Heightfield::set_hmap(Image *heightmap) {
	hmap=heightmap;
}

void LH_Heightfield::set_hmap_blend(float blend_factor) {
	hmap_blend_factor=blend_factor;
}

void LH_Heightfield::set_hmap_strength(float strength) {
	hmap_strength=strength;
}

LH_UVDistort::LH_UVDistort(Texture *texture,int divisions) {
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
	index_buffer=new int[(div_num+2)*(div_num+3)*2+3*2*(div_num+3)];  
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
}

LH_UVDistort::~LH_UVDistort() {
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

void LH_UVDistort::update(float frame) {
	float time=frame*0.033333333f;
	float p1_x,p1_y;
	float p2_x,p2_y;
	float p3_x,p3_y;
	p1_x=0.266*sin(1.0-6.28/7*time);
	p2_x=0.266*sin(2.0+6.28/6*time);
	p3_x=0.266*sin(3.0-6.28/8*time);

	p1_y=0.2*sin(4.0+6.28/11*time);
	p2_y=0.2*sin(5.0-6.28/9*time);
	p3_y=0.2*sin(6.0+6.28/5*time);

	static float sp1_y=0.2*sin(4.0);
	static float sp2_y=0.2*sin(5.0);
	static float sp3_y=0.2*sin(6.0);

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
}

void LH_UVDistort::render() {
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
	float tmp_col[4]={1.0f,1.0f,1.0f,0.25f};
	c_state->want_disabled(GL_DEPTH_TEST);
	c_state->want_enabled(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	c_state->want_disabled(GL_LIGHTING);
	c_state->want_enabled(GL_CULL_FACE);
	glColor4fv(tmp_col);

	glVertexPointer(3,GL_FLOAT,0,vertex_buffer);
	glDisableClientState(GL_NORMAL_ARRAY);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glFrustum(-0.133,0.133,-0.1,0.1,0.1,2.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if(hasCompiledVertexArray) {
		glLockArraysEXT(0,(div_num+3)*(div_num+3)+2*3*(div_num+3));
	}
	for(i=0;i<(div_num+2+3);i++) {
		glDrawElements(GL_TRIANGLE_STRIP,2*(div_num+3),
			GL_UNSIGNED_INT,&index_buffer[2*(div_num+3)*i]);
	}

	if(hasCompiledVertexArray) {
		glUnlockArraysEXT();
	}
	glEnableClientState(GL_NORMAL_ARRAY);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

LH_FlyingTexEffect::LH_FlyingTexEffect(char *filename,int num_samples,float start_x,
									   float start_y,float vel_x,float vel_y,float size_x,
									   float size_y) {
	napis=tex_manager.get_texture(filename);
	numsamples=num_samples;
	s_x=start_x;
	s_y=start_y;
	v_x=vel_x;
	v_y=vel_y;
	siz_x=size_x;
	siz_y=size_y;
}

LH_FlyingTexEffect::~LH_FlyingTexEffect() {
}

void LH_FlyingTexEffect::render(int time) {
	float x,y;
	float tmp_siz_x,tmp_siz_y;
	int i;
	for(i=0;i<numsamples;i++) {
		x=(1.0f+0.03f*i)*((s_x+v_x*(time*0.001f))-0.5f)+0.5f;
		y=(1.0f+0.03f*i)*((s_y+v_y*(time*0.001f))-0.5f)+0.5f;
		tmp_siz_x=(1.0f+0.04f*i*fabs(x-0.5))*siz_x;
		tmp_siz_y=(1.0f+0.04f*i*fabs(y-0.5))*siz_y;
		glColor4f(1.0f,1.0f,1.0f,1.0f/numsamples);
		DrawQuad(napis,x-tmp_siz_x,y-tmp_siz_y,x+tmp_siz_x,y+tmp_siz_y,
			0.0f,0.0f,1.0f,1.0f,1,
			GL_SRC_ALPHA,GL_ONE,0);
	}
}

LH_HeightfieldEffect::LH_HeightfieldEffect() {
	Texture *back_map;
	uvdistort=new LH_UVDistort(back_map,50);		//error, but it is dead code anyway
}

LH_HeightfieldEffect::~LH_HeightfieldEffect() {
	if(scn) delete scn;
	if(hmap) delete hmap;
	if(field) delete field;
	if(uvdistort) delete uvdistort;
}

void LH_HeightfieldEffect::render(int time) {

	int i;
	for(i=0;i<4;i++) {
		uvdistort->update(30.0f*(float)(time+35*i)*0.001f);
		uvdistort->render();
	}
}

LH_HeightfieldScene::LH_HeightfieldScene() {
	DMChannel *chn=new DMChannel();
	chn->add_effect(
		new LH_HeightfieldEffect(),
		100000);
	add_channel(chn);
	chn=new DMChannel();
}