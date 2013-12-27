/********************************************************************************
 Header file for scalar field polygoniser
 (c) 2000 Maciej Mroz
********************************************************************************/

#ifndef _FIELD_H_
#define _FIELD_H_

#include "engine.h"

enum BLOB_TYPE {
	BT_BALL,
	BT_CUBE,
	BT_TORUS
};

//base class for all present and future scalar field classes
class ScField: public ProceduralObject {
protected:
	//whole field specific
	float min_x,max_x,min_y,max_y,min_z,max_z;
	Envelope isovalue;
	unsigned int resolution;
	float *field_values;
	float *vertex_buffer;
	float *normal_buffer;
	unsigned int *edges;
	unsigned int *indices;
	unsigned int num_triangles;
	unsigned int num_vertices;

	unsigned int buffer_size;
	unsigned int indices_size;

	//check if the vertex was referenced before
	//if not insert interpolated coordinates into vertex_buffer, and return the index
	int add_interpolated_vertex(float x1,float y1,float z1,float val1,
		float x2,float y2,float z2,float val2,int edge);

	//put the triangle into indices table
	void add_triangle(int index1,int index2,int index3);

	void generate_normals();

	//blob specific
	unsigned int num_blobs;
	Motion *motions;
	BLOB_TYPE *types;
	float *blob_strengths;
public:
	ScField(float mini_x,float maxi_x,float mini_y,float maxi_y,
		float mini_z,float maxi_z,int res=16);
	~ScField();
	void set_isovalue(Envelope* env);
	void add_blob(Motion* mot,BLOB_TYPE bt=BT_BALL,float strength=1.0f);
	void add_blob(const char *mot_file,BLOB_TYPE bt=BT_BALL,float strength=1.0f);
	void update_blob(unsigned int index,Motion* mot,
		BLOB_TYPE bt=BT_BALL,float strength=1.0f);
	unsigned int get_num_blobs();
	virtual void update(float frame);
	virtual void render();	
};

#endif