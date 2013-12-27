/**************************************************************************************
  light.cpp - light class implementation
  (c) 2000 Maciej Mroz
**************************************************************************************/

#include "engine.h"


Light::Light(GLenum id) {
	light_id=id;
}

Light::Light() {
}

void Light::set_light_id(GLenum id) {
	light_id=id;
}

void Light::draw(float frame) {
	glLightfv(light_id,GL_DIFFUSE,color);
	glLightfv(light_id,GL_POSITION,&res[0]);
}

void Light::set_color(float col_r,float col_g,float col_b) {
	color[0]=col_r;
	color[1]=col_g;
	color[2]=col_b;
	color[3]=1.0f;
}

void Light::set_name(const char* s) {
	name=new char[strlen(s)+1];
	strcpy(name,s);
}

void Light::set_position(float x, float y, float z) {
	res[0]=x;
	res[1]=y;
	res[2]=z;
}

void Light::set_rotation(float rx,float ry,float rz) {
	res[3]=rz;
	res[4]=ry;
	res[5]=rx;
}

void Light::set_type(int t) {
	light_type=t;
}

