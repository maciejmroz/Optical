/*****************************************************************************************
 Quadtree terrain implementation
 (c) 200 Maciej Mroz
*****************************************************************************************/

#include "quad_tree.h"
#include "debug_msg.h"

/*	Image *hmap;
	qtree_node *qtree;
	Matrix *transform;
	float x_siz,y_siz;
	float detail;
	float strength;
*/

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float

QTerrain::QTerrain(const char *heightmap,float x_size,float z_size,float detail_level,
				   float h_strength,int depth) {
	hmap=load_image(heightmap);
	if(!hmap) AppCriticalError("QTerrain - could not load heightmap\n");
	MIdentity(transform_mtx);
	x_siz=x_size;
	z_siz=z_size;
	strength=h_strength;
	detail=strength*detail_level;
	rec_level=0;
	max_level=depth;
	build_tree(qtree,-x_size,-z_size,x_size,z_size);
}

QTerrain::~QTerrain() {
}

void QTerrain::build_tree(qtree_node* tn,float x1,float z1,float x2,float z2) {
	rec_level++;
	tn=new qtree_node;
	if(rec_level==max_level) { //generate lowest level tree node
		tn->top_left=NULL;
		tn->top_right=NULL;
		tn->bottom_left=NULL;
		tn->bottom_right=NULL;
		//calc the height
		float frac_x_start,frac_z_start;
		float frac_x_end,frac_z_end;
		int x_start,z_start;
		int x_end,z_end;
		char max_hgt;
		int i,j;
		frac_x_start=(x1+x_siz)/(2.0f*x_siz);
		frac_z_start=(z1+z_siz)/(2.0f*z_siz);

		frac_x_end=(x2+x_siz)/(2.0f*x_siz);
		frac_z_end=(z2+z_siz)/(2.0f*z_siz);

		x_start=(int)(hmap->width*frac_x_start);
		z_start=(int)(hmap->height*frac_z_start);
		x_end=(int)(hmap->width*frac_x_end);
		z_end=(int)(hmap->height*frac_z_end);
		max_hgt=0;
		for(i=z_start;i<z_end;i++) {
			for(j=x_start;j<x_end;j++) {
				if(max_hgt<hmap->pixel_buf[i*hmap->width+j])
					max_hgt=hmap->pixel_buf[i*hmap->width+j];
			}
		}
		tn->height=(max_hgt/255.0f)*strength;
	}
	else {  //generate subnodes
		build_tree(tn->top_left,x1,z1+(z2-z1)*0.5,x1+(x2-x1)*0.5,z2);
		build_tree(tn->top_right,x1+(x2-x1)*0.5,z1+(z2-z1)*0.5,x2,z2);
		build_tree(tn->bottom_left,x1,z1,x1+(x2-x1)*0.5,z1+(z2-z1)*0.5);
		build_tree(tn->bottom_right,x1+(x2-x1)*0.5,z1,x2,z1+(z2-z1)*0.5);
		//calc the height
		float max=tn->top_left->height;
		if(tn->top_right->height>max) max=tn->top_right->height;
		if(tn->bottom_right->height>max) max=tn->bottom_right->height;
		if(tn->bottom_left->height>max) max=tn->bottom_left->height;
	}
	rec_level--;
}

void QTerrain::parse_tree(qtree_node* tn,float x1,float z1,float x2,float z2) {
}

void QTerrain::update(float frame) {
}

void QTerrain::render() {
}
