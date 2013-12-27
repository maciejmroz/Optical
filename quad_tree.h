/*****************************************************************************************
 Quadtree based terrain renderer
 (c) 2000 Maciej Mroz
*****************************************************************************************/

#ifndef _QUADTREE_TERRAIN_H_
#define _QUADTREE_TERRAIN_H_

#include "engine.h"

class qtree_node {
	qtree_node *top_left;
	qtree_node *top_right;
	qtree_node *bottom_left;
	qtree_node *bottom_right;
	float height;
	friend class QTerrain;
};

class QTerrain : public ProceduralObject {
	Image *hmap;
	qtree_node *qtree;
	Matrix transform_mtx;
	int rec_level;
	int max_level;
	float x_siz,z_siz;
	float detail;
	float strength;
	void build_tree(qtree_node* tn,float x1,float z1,float x2,float z2);
	void parse_tree(qtree_node* tn,float x1,float z1,float x2,float z2);
	void delete_tree();
public:
	QTerrain(const char *heightmap,float x_size,float z_size,float detail_level,
		float h_strength,int depth=8);
	~QTerrain();
	void update(float frame=0.0f);
	void render();
};

#endif