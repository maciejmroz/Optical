/***************************************************************************
                          interpolators.cpp  -  description
                             -------------------
    begin                : Fri May 5 2000
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/


#include "interpolators.h"
#include<stdlib.h>

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float

/*
implementacja klasy podstawowej DMInterpolator
*/

DMInterpolator::DMInterpolator() {
	positions=NULL;
	values=NULL;
	numsegments=0;
}

DMInterpolator::~DMInterpolator() {
	if(values!=NULL) delete values;
	if(positions!=NULL) delete positions;
}

void DMInterpolator::addsegment(float x1,float x2,float x3, float x4, float length) {
	float *tmp1;
	int i;
	if(positions==NULL) {
		positions=new float[1];
	}
	else {
		tmp1=new float[numsegments+1];
		for(i=0;i<numsegments;i++) tmp1[i]=positions[i];
		delete positions;
		positions=tmp1;
	}

	if(values==NULL) {
		values=new float[4];
	}
	else {
		tmp1=new float[4*(numsegments+1)];
		for(i=0;i<4*numsegments;i++) tmp1[i]=values[i];
		delete values;
		values=tmp1;
	}
	values[4*numsegments]=x1;
	values[4*numsegments+1]=x2;
	values[4*numsegments+2]=x3;
	values[4*numsegments+3]=x4;
	numsegments++;
if(numsegments==1) positions[numsegments-1]=length;
else positions[numsegments-1]=positions[numsegments-2]+length;
}


/*
klasy odziedziczone z DMInterpolator
*/

float DMLinearInterpolator::interpolate(float position) {
	float length;
	int i;
	if(numsegments<1) return 0.0f;
	length=positions[numsegments-1];
	if(position<0) return values[0];
	if(position>length) return values[4*numsegments-1]; //3 jest specyficzne dla interp. liniowego
	i=0;
	while(position>positions[i] && i<(numsegments-1)) i++;
	float tm,seglen;
	if(i==0) seglen=positions[0];
	else seglen=positions[i]-positions[i-1];
	if(i==0) tm=position;
	else tm=(position-positions[i-1]);
	return values[4*i]+tm*(values[4*i+1]-values[4*i])/seglen;
}

float DMCLinearInterpolator::interpolate(float position) {

	float length;
	int i;
	if(numsegments<1) return 0.0f;
	length=positions[numsegments-1];
	if(position>0) while(position>length) position-=length;
	else while(position<0) position+=length;
	i=0;
	while(position>positions[i] && i<(numsegments-1)) i++;
	float tm,seglen;
	if(i==0) seglen=positions[0];
	else seglen=positions[i]-positions[i-1];
	if(i==0) tm=position;
	else tm=(position-positions[i-1]);
	return values[4*i]+tm*(values[4*i+1]-values[4*i])/seglen;

}



float DMBezierInterpolator::interpolate(float position) {
	float length;
	int i;
	if(numsegments<1) return 0.0f;
	length=positions[numsegments-1];
	if(position<0) return values[0];
	if(position>length) return values[4*numsegments-1]; 
	i=0;
	while(position>positions[i] && i<(numsegments-1)) i++;
	float tm,seglen;
	if(i==0) seglen=positions[0];
	else seglen=positions[i]-positions[i-1];
	if(i==0) tm=position/seglen;
	else tm=(position-positions[i-1])/seglen;
	return values[4*i]*(1.0-tm)*(1.0-tm)*(1.0-tm)
		+values[4*i+1]*3.0*tm*(1.0-tm)*(1.0-tm)
		+values[4*i+2]*3.0*tm*tm*(1.0-tm)
		+values[4*i+3]*tm*tm*tm;
}

float DMBezierInterpolator::derive(float position) {
	float length;
	int i;
	if(numsegments<1) return 0.0f;
	length=positions[numsegments-1];
	if(position<0) return values[0];
	if(position>length) return values[4*numsegments-1];
	i=0;
	while(position>positions[i] && i<(numsegments-1)) i++;
	float tm,seglen;
	if(i==0) seglen=positions[0];
	else seglen=positions[i]-positions[i-1];
	if(i==0) tm=position/seglen;
	else tm=(position-positions[i-1])/seglen;
	return -3.0f*values[4*i]*(1.0-tm)*(1.0-tm)
		+values[4*i+1]*3.0*(tm*tm-1.0)
		+values[4*i+2]*6.0*tm*(1.0-2.0*tm)
		+values[4*i+3]*3.0*tm*tm;
}

float DMCBezierInterpolator::interpolate(float position) {
	float length;
	int i;
	if(numsegments<1) return 0.0f;
	length=positions[numsegments-1];
	if(position>0) while(position>length) position-=length;
	else while(position<0) position+=length;
	i=0;
	while(position>positions[i] && i<(numsegments-1)) i++;
	float tm,seglen;
	if(i==0) seglen=positions[0];
	else seglen=positions[i]-positions[i-1];
	if(i==0) tm=position/seglen;
	else tm=(position-positions[i-1])/seglen;
	return values[4*i]*(1.0-tm)*(1.0-tm)*(1.0-tm)
		+values[4*i+1]*3.0*tm*(1.0-tm)*(1.0-tm)
		+values[4*i+2]*3.0*tm*tm*(1.0-tm)
		+values[4*i+3]*tm*tm*tm;
}

//klasa DMP3BezierInterpolator - nie dziedziczy
//z DMInterpolator !!!!

DMP3BezierInterpolator::DMP3BezierInterpolator() {
 x=NULL;
 y=NULL;
 z=NULL;
}

DMP3BezierInterpolator::~DMP3BezierInterpolator() {
 if(x!=NULL) delete x;
 if(y!=NULL) delete y;
 if(z!=NULL) delete z;
}

void DMP3BezierInterpolator::addsegment(Point3 p1, Point3 p2,
                            Point3 p3, Point3 p4, float length) {
 if(x==NULL) {
  x=new DMBezierInterpolator();
  y=new DMBezierInterpolator();
  z=new DMBezierInterpolator();
 }
 x->addsegment(p1[0],p2[0],p3[0],p4[0],length);
 y->addsegment(p1[1],p2[1],p3[1],p4[1],length);
 z->addsegment(p1[2],p2[2],p3[2],p4[2],length);
}

void DMP3BezierInterpolator::interpolate(float position,
                             Point3 result) {
  result[0]=x->interpolate(position);
  result[1]=y->interpolate(position);
  result[2]=z->interpolate(position);
}

void DMP3BezierInterpolator::derive(float position,
                             Point3 result) {
  result[0]=x->derive(position);
  result[1]=y->derive(position);
  result[2]=z->derive(position);
}