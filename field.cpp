/*****************************************************************************************
 Implementation of scalar field polygoniser
 (c) 2000 Maciej Mroz
*****************************************************************************************/

#include "engine.h"
#include "field.h"
#include "debug_msg.h"


ScField::ScField(float mini_x,float maxi_x,float mini_y,float maxi_y,
				 float mini_z,float maxi_z,int res) {
	resolution=res;
	field_values=new float[resolution*resolution*resolution];
	edges=new unsigned int[3*resolution*resolution*resolution];
	num_blobs=0;
	motions=NULL;
	blob_strengths=NULL;
	indices=NULL;
	vertex_buffer=NULL;
	normal_buffer=NULL;
	isovalue=0.0f;
	max_x=maxi_x;
	max_y=maxi_y;
	max_z=maxi_z;
	min_x=mini_x;
	min_y=mini_y;
	min_z=mini_z;
	types=NULL;
	num_triangles=0;
	num_vertices=0;
	buffer_size=0;
	indices_size=0;
}

ScField::~ScField() {
	if(field_values) delete field_values;
	if(edges) delete edges;
	if(indices) delete indices;
	if(vertex_buffer) delete vertex_buffer;
	if(blob_strengths) delete blob_strengths;
	if(motions) delete [] motions;
	if(types) delete types;
	if(normal_buffer) delete normal_buffer;
}

void ScField::add_blob(Motion *mot,BLOB_TYPE bt,float strength) {
	if(num_blobs==0) {
		motions=new Motion[1];
		motions[0]=*mot;
		types=new BLOB_TYPE[1];
		types[0]=bt;
		blob_strengths=new float[1];
		blob_strengths[0]=strength;
		num_blobs=1;
	}
	else {
		Motion *tmp_mot;
		BLOB_TYPE *tmp_types;
		float *tmp_strengths;
		unsigned int i;
		tmp_mot=new Motion[num_blobs+1];
		tmp_types=new BLOB_TYPE[num_blobs+1];
		tmp_strengths=new float[num_blobs+1];
		for(i=0;i<num_blobs;i++) {
			tmp_mot[i]=motions[i];
			tmp_types[i]=types[i];
			tmp_strengths[i]=blob_strengths[i];
		}
		delete blob_strengths;
		delete types;
		delete [] motions;
		blob_strengths=tmp_strengths;
		types=tmp_types;
		motions=tmp_mot;
		types[num_blobs]=bt;
		motions[num_blobs]=*mot;
		blob_strengths[num_blobs++]=strength;
	}
}

void ScField::add_blob(const char *mot_file,BLOB_TYPE bt,float strength) {
	Motion *mot;
	mot=new Motion(mot_file);
	if(num_blobs==0) {
		motions=new Motion[1];
		motions[0]=*mot;
		types=new BLOB_TYPE[1];
		types[0]=bt;
		blob_strengths=new float[1];
		blob_strengths[0]=strength;
		num_blobs=1;
	}
	else {
		Motion *tmp_mot;
		BLOB_TYPE *tmp_types;
		float *tmp_strengths;
		unsigned int i;
		tmp_mot=new Motion[num_blobs+1];
		tmp_types=new BLOB_TYPE[num_blobs+1];
		tmp_strengths=new float[num_blobs+1];
		for(i=0;i<num_blobs;i++) {
			tmp_mot[i]=motions[i];
			tmp_types[i]=types[i];
			tmp_strengths[i]=blob_strengths[i];
		}
		delete blob_strengths;
		delete types;
		delete [] motions;
		blob_strengths=tmp_strengths;
		types=tmp_types;
		motions=tmp_mot;
		types[num_blobs]=bt;
		motions[num_blobs]=*mot;
		blob_strengths[num_blobs++]=strength;
	}
	delete mot;
}

unsigned int ScField::get_num_blobs() {
	return num_blobs;
}

void ScField::set_isovalue(Envelope *env) {
	isovalue=*env;
}

void ScField::update_blob(unsigned int index,Motion *mot,BLOB_TYPE bt,float strength) {
	if(index>=num_blobs) return;
	motions[index]=*mot;
	blob_strengths[index]=strength;
	types[index]=bt;
}

int ScField::add_interpolated_vertex(float x1,float y1,float z1,float val1,
									 float x2,float y2,float z2,float val2,
									 int edge) {
	if(edges[edge]) {
		return (edges[edge]-1);
	}
	//we really have to add a vertex
	float mu;
	unsigned int i;
	if(num_vertices==buffer_size) {
		float* tmp_buf;
		tmp_buf=new float[3*(buffer_size+resolution*resolution)];
		if(normal_buffer) delete normal_buffer;
		normal_buffer=new float[3*(buffer_size+resolution*resolution)];
		for(i=0;i<3*buffer_size;i++) tmp_buf[i]=vertex_buffer[i];
		delete vertex_buffer;
		vertex_buffer=tmp_buf;
		buffer_size+=resolution*resolution;
		if(fabs(isovalue.res-val1) < 0.00001f) {
			vertex_buffer[3*num_vertices]=x1;
			vertex_buffer[3*num_vertices+1]=y1;
			vertex_buffer[3*num_vertices+2]=z1;
		}
		else if(fabs(isovalue.res-val2) < 0.00001f) {
			vertex_buffer[3*num_vertices]=x2;
			vertex_buffer[3*num_vertices+1]=y2;
			vertex_buffer[3*num_vertices+2]=z2;
		}
		else if(fabs(val2-val1)<0.00001f) {
			vertex_buffer[3*num_vertices]=x1;
			vertex_buffer[3*num_vertices+1]=y1;
			vertex_buffer[3*num_vertices+2]=z1;
		}
		else {
			mu = (isovalue.res - val1) / (val2 - val1);
			vertex_buffer[3*num_vertices] = x1 + mu * (x2 - x1);
			vertex_buffer[3*num_vertices+1] = y1 + mu * (y2 - y1);
			vertex_buffer[3*num_vertices+2] = z1 + mu * (z2 - z1);
		}
		num_vertices++;
		edges[edge]=num_vertices;
		return (edges[edge]-1);
	}
	else {
		if(fabs(isovalue.res-val1) < 0.00001f) {
			vertex_buffer[3*num_vertices]=x1;
			vertex_buffer[3*num_vertices+1]=y1;
			vertex_buffer[3*num_vertices+2]=z1;
		}
		else if(fabs(isovalue.res-val2) < 0.00001f) {
			vertex_buffer[3*num_vertices]=x2;
			vertex_buffer[3*num_vertices+1]=y2;
			vertex_buffer[3*num_vertices+2]=z2;
		}
		else if(fabs(val2-val1)<0.00001f) {
			vertex_buffer[3*num_vertices]=x1;
			vertex_buffer[3*num_vertices+1]=y1;
			vertex_buffer[3*num_vertices+2]=z1;
		}
		else {
			mu = (isovalue.res - val1) / (val2 - val1);
			vertex_buffer[3*num_vertices] = x1 + mu * (x2 - x1);
			vertex_buffer[3*num_vertices+1] = y1 + mu * (y2 - y1);
			vertex_buffer[3*num_vertices+2] = z1 + mu * (z2 - z1);
		}
		num_vertices++;
		edges[edge]=num_vertices;
		return (edges[edge]-1);
	}
}

void ScField::add_triangle(int index1,int index2,int index3) {
	unsigned int i;
	unsigned int *tmp_uint;
	if((3*num_triangles)==indices_size) {
		tmp_uint=new unsigned int[indices_size+3000];
		for(i=0;i<indices_size;i++) tmp_uint[i]=indices[i];
		indices_size+=3000;
		delete indices;
		indices=tmp_uint;
		indices[3*num_triangles]=index3;
		indices[3*num_triangles+1]=index2;
		indices[3*num_triangles+2]=index1;
		num_triangles++;
	}
	else {
		indices[3*num_triangles]=index3;
		indices[3*num_triangles+1]=index2;
		indices[3*num_triangles+2]=index1;
		num_triangles++;
	}
}

void ScField::generate_normals() {
	unsigned int i,j;
	register float x_2,y_2,z_2;
	float func;
	for(i=0;i<num_vertices;i++) {
		normal_buffer[3*i]=0.0f;
		normal_buffer[3*i+1]=0.0f;
		normal_buffer[3*i+2]=0.0f;
		for(j=0;j<num_blobs;j++) {
			//it is good idea to precalc squares of x, y and z - they are used many times
			x_2=(vertex_buffer[3*i]-motions[j].cv[0])*(vertex_buffer[3*i]-motions[j].cv[0]);
			y_2=(vertex_buffer[3*i+1]-motions[j].cv[1])*(vertex_buffer[3*i+1]-motions[j].cv[1]);
			z_2=(vertex_buffer[3*i+2]-motions[j].cv[2])*(vertex_buffer[3*i+2]-motions[j].cv[2]);
			switch(types[j]) {
			case BT_BALL:
				func=(x_2+y_2+z_2);
				normal_buffer[3*i]+=2.0f*(vertex_buffer[3*i]-motions[j].cv[0])/
					(func*func);
				normal_buffer[3*i+1]+=2.0f*(vertex_buffer[3*i+1]-motions[j].cv[1])/
					(func*func);
				normal_buffer[3*i+2]+=2.0f*(vertex_buffer[3*i+2]-motions[j].cv[2])/
					(func*func);
			break;
			case BT_CUBE:
				func=x_2*x_2+y_2*y_2+z_2*z_2;
				normal_buffer[3*i]+=(0.3f*x_2*(vertex_buffer[3*i]-motions[j].cv[0]))/
					(func*func);
				normal_buffer[3*i+1]+=(0.3f*y_2*(vertex_buffer[3*i+1]-motions[j].cv[1]))/
					(func*func);
				normal_buffer[3*i+2]+=(0.3f*z_2*(vertex_buffer[3*i+2]-motions[j].cv[2]))/
					(func*func);
			break;
			case BT_TORUS:
				func=x_2*x_2+y_2*y_2+z_2*z_2+2*x_2*y_2+
					2*x_2*z_2+2*y_2*z_2-
					2*x_2*(0.35f*0.35f+0.001f*0.001f)+
					2*y_2*(0.35f*0.35f-0.001f*0.001f)-
					2*z_2*(0.35f*0.35f+0.001f*0.001f)+
					(0.35f*0.35f-0.001f*0.001f)*(0.35f*0.35f-0.001f*0.001f);
				normal_buffer[3*i]+=(0.4f*x_2*(vertex_buffer[3*i]-motions[j].cv[0])+
					0.4f*(vertex_buffer[3*i]-motions[j].cv[0])*y_2+
					0.4f*(vertex_buffer[3*i]-motions[j].cv[0])*z_2-
					0.4f*(vertex_buffer[3*i]-motions[j].cv[0])*(0.35f*0.35f+0.001f*0.001f))/
					(func*func);
				normal_buffer[3*i+1]+=(0.4f*y_2*(vertex_buffer[3*i+1]-motions[j].cv[1])+
					0.4f*(vertex_buffer[3*i+1]-motions[j].cv[1])*x_2+
					0.4f*(vertex_buffer[3*i+1]-motions[j].cv[1])*z_2+
					0.4f*(vertex_buffer[3*i+1]-motions[j].cv[1])*(0.35f*0.35f-0.001f*0.001f))/
					(func*func);
				normal_buffer[3*i+2]+=(0.4f*z_2*(vertex_buffer[3*i+2]-motions[j].cv[2])+
					0.4f*(vertex_buffer[3*i+2]-motions[j].cv[2])*y_2+
					0.4f*(vertex_buffer[3*i+2]-motions[j].cv[2])*x_2-
					0.4f*(vertex_buffer[3*i+2]-motions[j].cv[2])*(0.35f*0.35f+0.001f*0.001f))/
					(func*func);
			break;
			}
		}
		VNormalize3(&normal_buffer[3*i]);
	}
}

//credits for the tables used in polygonizer go to Paul Bourke
int edgeTable[256]={
	0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
	0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
	0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
	0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
	0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
	0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
	0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
	0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
	0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
	0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
	0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
	0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
	0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
	0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
	0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
	0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
	0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
	0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
	0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
	0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
	0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
	0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
	0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
	0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
	0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
	0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
	0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
	0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
	0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
	0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
	0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
	0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   
};
int triTable[256][16] = {
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
	{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
	{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
	{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
	{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
	{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
	{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
	{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
	{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
	{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
	{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
	{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
	{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
	{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
	{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
	{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
	{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
	{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
	{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
	{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
	{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
	{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
	{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
	{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
	{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
	{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
	{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
	{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
	{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
	{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
	{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
	{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
	{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
	{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
	{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
	{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
	{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
	{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
	{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
	{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
	{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
	{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
	{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
	{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
	{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
	{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
	{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
	{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
	{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
	{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
	{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
	{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
	{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
	{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
	{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
	{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
	{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
	{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
	{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
	{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
	{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
	{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
	{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
	{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
	{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
	{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
	{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
	{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
	{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
	{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
	{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
	{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
	{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
	{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
	{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
	{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
	{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
	{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
	{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
	{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
	{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
	{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
	{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
	{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
	{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
	{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
	{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
	{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
	{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
	{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
	{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
	{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
	{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
	{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
	{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
	{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
	{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

void ScField::update(float frame) {
	unsigned int x_pos,y_pos,z_pos;
	int table_offset;
	unsigned int i;
	int s_i;
	num_triangles=0;  //"clear" indices list
	num_vertices=0; //"clear" vertices list
	unsigned int vertlist[12];  //temporary table for storing indices

	//zero edge occupation table
#ifdef _WIN32
	ZeroMemory(edges,3*resolution*resolution*resolution*sizeof(unsigned int));
#else
	for(i=0;i<(3*resolution*resolution*resolution);i++) {
		edges[i]=0;
	}
#endif
	//calculate blobs positions and rotations
	for(i=0;i<num_blobs;i++) {
		motions[i].calc(frame);
	}
	//calculate isovalue
	isovalue.calc(frame);
	//calculate field values
	float x,y,z,x_step,y_step,z_step;
	x=min_x;
	y=min_y;
	z=min_z;
	x_step=(max_x-min_x)/(float)(resolution-1);
	y_step=(max_y-min_y)/(float)(resolution-1);
	z_step=(max_z-min_z)/(float)(resolution-1);
	for(z_pos=0;z_pos<resolution;z_pos++) {
		y=min_y;
		for(y_pos=0;y_pos<resolution;y_pos++) {
			table_offset=resolution*(resolution*z_pos+y_pos);
			x=min_x;
			for(x_pos=0;x_pos<resolution;x_pos++) {
				field_values[table_offset+x_pos]=0.0f;
				for(i=0;i<num_blobs;i++) {
					float x_2,y_2,z_2;
					x_2=(motions[i].cv[0]-x)*(motions[i].cv[0]-x);
					y_2=(motions[i].cv[1]-y)*(motions[i].cv[1]-y);
					z_2=(motions[i].cv[2]-z)*(motions[i].cv[2]-z);
					switch(types[i]) {
					case BT_BALL:
						float tmp_f;
						tmp_f=x_2+y_2+z_2;
						if(tmp_f!=0) field_values[table_offset+x_pos]+=2.0f*blob_strengths[i]/tmp_f;
						else field_values[table_offset+x_pos]+=1000.0f;
					break;
					case BT_CUBE:
						//special case of superellipsoid?
						field_values[table_offset+x_pos]+=
							0.1f/(x_2*x_2+y_2*y_2+z_2*z_2);
					break;
					case BT_TORUS:
						field_values[table_offset+x_pos]+=0.1f/
							(x_2*x_2+y_2*y_2+z_2*z_2+2*x_2*y_2+
							2.0f*x_2*z_2+2*y_2*z_2-
							2.0f*x_2*(0.35f*0.35f+0.001f*0.001f)+
							2.0f*y_2*(0.35f*0.35f-0.001f*0.001f)-
							2.0f*z_2*(0.35f*0.35f+0.001f*0.001f)+
							(0.35f*0.35f-0.001f*0.001f)*(0.35f*0.35f-0.001f*0.001f));
					break;
					}
				}
				x+=x_step;
			}
			y+=y_step;
		}
		z+=z_step;
	}
	//do the marching cubes (based on Paul Bourke's code)
	//if want to understand the algorithm, better see original code
	unsigned int cubeindex;
	x=min_x;
	y=min_y;
	z=min_z;
	unsigned int res_2;
	res_2=resolution*resolution;
	for(z_pos=0;z_pos<(resolution-1);z_pos++) {
		y=min_y;
		for(y_pos=0;y_pos<(resolution-1);y_pos++) {
			table_offset=resolution*(resolution*z_pos+y_pos);
			x=min_x;
			for(x_pos=0;x_pos<(resolution-1);x_pos++) {
				/*Determine the index into the edge table which
				tells us which vertices are inside of the surface*/
				cubeindex = 0;
				if(field_values[table_offset+x_pos+res_2] > isovalue.res) cubeindex |= 1;
				if(field_values[table_offset+x_pos+res_2+1] > isovalue.res) cubeindex |= 2;
				if(field_values[table_offset+x_pos+1] > isovalue.res) cubeindex |= 4;
				if(field_values[table_offset+x_pos] > isovalue.res) cubeindex |= 8;
				if(field_values[table_offset+x_pos+res_2+resolution] > isovalue.res) cubeindex |= 16;
				if(field_values[table_offset+x_pos+res_2+1+resolution] > isovalue.res) cubeindex |= 32;
				if(field_values[table_offset+x_pos+1+resolution] > isovalue.res) cubeindex |= 64;
				if(field_values[table_offset+x_pos+resolution] > isovalue.res) cubeindex |= 128;
				// Find the vertices where the surface intersects the cube 
				if (edgeTable[cubeindex]) {  //there are some triangles to generate
					if(edgeTable[cubeindex] & 1) 
						vertlist[0]=add_interpolated_vertex(x,y,z+z_step,
						field_values[table_offset+x_pos+res_2],
						x+x_step,y,z+z_step,
						field_values[table_offset+x_pos+res_2+1],
						3*(table_offset+x_pos+res_2));
					if(edgeTable[cubeindex] & 2) 
						vertlist[1]=add_interpolated_vertex(x+x_step,y,z+z_step,
						field_values[table_offset+x_pos+res_2+1],
						x+x_step,y,z,
						field_values[table_offset+x_pos+1],
						3*(table_offset+x_pos+1)+2);
					if(edgeTable[cubeindex] & 4) 
						vertlist[2]=add_interpolated_vertex(x+x_step,y,z,
						field_values[table_offset+x_pos+1],
						x,y,z,
						field_values[table_offset+x_pos],
						3*(table_offset+x_pos));
					if(edgeTable[cubeindex] & 8) 
						vertlist[3]=add_interpolated_vertex(x,y,z,
						field_values[table_offset+x_pos],
						x,y,z+z_step,
						field_values[table_offset+x_pos+res_2],
						3*(table_offset+x_pos)+2);

					if(edgeTable[cubeindex] & 16) 
						vertlist[4]=add_interpolated_vertex(x,y+y_step,z+z_step,
						field_values[table_offset+x_pos+res_2+resolution],
						x+x_step,y+y_step,z+z_step,
						field_values[table_offset+x_pos+res_2+resolution+1],
						3*(table_offset+x_pos+res_2+resolution));
					if(edgeTable[cubeindex] & 32) 
						vertlist[5]=add_interpolated_vertex(x+x_step,y+y_step,z+z_step,
						field_values[table_offset+x_pos+res_2+1+resolution],
						x+x_step,y+y_step,z,
						field_values[table_offset+x_pos+1+resolution],
						3*(table_offset+x_pos+1+resolution)+2);
					if(edgeTable[cubeindex] & 64) 
						vertlist[6]=add_interpolated_vertex(x+x_step,y+y_step,z,
						field_values[table_offset+x_pos+1+resolution],
						x,y+y_step,z,
						field_values[table_offset+x_pos+resolution],
						3*(table_offset+x_pos+resolution));
					if(edgeTable[cubeindex] & 128) 
						vertlist[7]=add_interpolated_vertex(x,y+y_step,z,
						field_values[table_offset+x_pos+resolution],
						x,y+y_step,z+z_step,
						field_values[table_offset+x_pos+res_2+resolution],
						3*(table_offset+x_pos+resolution)+2);
					if(edgeTable[cubeindex] & 256) 
						vertlist[8]=add_interpolated_vertex(x,y,z+z_step,
						field_values[table_offset+x_pos+res_2],
						x,y+y_step,z+z_step,
						field_values[table_offset+x_pos+res_2+resolution],
						3*(table_offset+x_pos+res_2)+1);
					if(edgeTable[cubeindex] & 512) 
						vertlist[9]=add_interpolated_vertex(x+x_step,y,z+z_step,
						field_values[table_offset+x_pos+res_2+1],
						x+x_step,y+y_step,z+z_step,
						field_values[table_offset+x_pos+res_2+resolution+1],
						3*(table_offset+x_pos+res_2+1)+1);
					if(edgeTable[cubeindex] & 1024) 
						vertlist[10]=add_interpolated_vertex(x+x_step,y,z,
						field_values[table_offset+x_pos+1],
						x+x_step,y+y_step,z,
						field_values[table_offset+x_pos+resolution+1],
						3*(table_offset+x_pos+1)+1);
					if(edgeTable[cubeindex] & 2048) 
						vertlist[11]=add_interpolated_vertex(x,y,z,
						field_values[table_offset+x_pos],
						x,y+y_step,z,
						field_values[table_offset+x_pos+resolution],
						3*(table_offset+x_pos)+1);

					/* Create the triangle */
					for (s_i=0;triTable[cubeindex][s_i]!=-1;s_i+=3) {
						add_triangle(vertlist[triTable[cubeindex][s_i  ]],
							vertlist[triTable[cubeindex][s_i+1]],
							vertlist[triTable[cubeindex][s_i+2]]);
					}

				}
				x+=x_step;
			}
			y+=y_step;
		}
		z+=z_step;
	}
	generate_normals();
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("num_triangles - %d\n",num_triangles);
	DebugFrameMessage("num_vertices - %d\n",num_vertices);
#endif

}

void ScField::render() {
	glNormalPointer(GL_FLOAT,0,normal_buffer);
	glVertexPointer(3,GL_FLOAT,0,vertex_buffer);
	if(hasCompiledVertexArray) {
		glLockArraysEXT(0,num_vertices);
	}
	glDrawElements(GL_TRIANGLES,3*num_triangles,GL_UNSIGNED_INT,indices);
	if(hasCompiledVertexArray) {
		glUnlockArraysEXT();
	}
#ifdef _LOG_EVERYTHING
	DebugFrameMessage("num_triangles = %d, 3*num_triangles = %d:\n",num_triangles,3*num_triangles);
#endif
}