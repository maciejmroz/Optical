/***************************************************************************
                          dmcamera.cpp  -  description
                             -------------------
    begin                : Sat May 6 2000
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/

#include <stdio.h>
#include "engine.h"
#include "glstuff.h"

Camera::~Camera() {
}

Camera::Camera() {
 frustum_l=-0.133f;
 frustum_r=0.133f;
 frustum_t=0.1f;
 frustum_b=-0.1f;
 near_clip=0.1f;
 far_clip=60.0f;
 res[0]=0.0f;
 res[1]=0.0f;
 res[2]=0.0f;
 res[3]=0.0f;
 res[4]=0.0f;
 res[5]=0.0f;
}

void Camera::set_frustum(double l,double r,double b,double t,
                           double n,double f) {
 frustum_l=l;
 frustum_r=r;
 frustum_t=t;
 frustum_b=b;
 near_clip=n;
 far_clip=f;
}

void Camera::set_origin(Point3 o) {
 res[0]=o[0];
 res[1]=o[1];
 res[2]=o[2];
}

void Camera::update_frustum(void) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(frustum_l,frustum_r,frustum_b,frustum_t,near_clip,far_clip);
	glMatrixMode(GL_MODELVIEW);
}



void Camera::set_rotatex(float ang) {
	res[4]=ang;
}

void Camera::set_rotatey(float ang) {
	res[3]=ang;
}

void Camera::set_rotatez(float ang) {
	res[5]=ang;
}

void Camera::apply_transform() {
	glLoadIdentity();
	glRotatef(res[3],0.0f,1.0f,0.0f);
	glRotatef(res[4],1.0f,0.0f,0.0f);
	glRotatef(res[5],0.0f,0.0f,1.0f);
	glTranslatef(-res[0],-res[1],-res[2]);
}

void Camera::set_origin(float xp, float yp, float zp) {
	res[0]=xp;
	res[1]=yp;
	res[2]=zp;
}

void Camera::get_origin(float *xp, float *yp,float *zp) {
	*xp=res[0];
	*yp=res[1];
	*zp=res[2];
}

void Camera::get_origin(Point3 pos) {
	pos[0]=res[0];
	pos[1]=res[2];
	pos[2]=res[3];
}

void Camera::draw(float frame) {
	update_frustum();
	apply_transform();
}

