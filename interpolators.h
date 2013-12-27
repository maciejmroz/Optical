/***************************************************************************
                          interpolators.h  -  description
                             -------------------
    begin                : Fri May 5 2000
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/


#ifndef _DM_INTERPOLATORS_H_
#define _DM_INTERPOLATORS_H_

#include "mathutils.h"

//klasa bazowa wszystkich interpolatorow
class DMInterpolator {
protected:
	float *values,*positions;
	int numsegments;
public:
	DMInterpolator();
	virtual ~DMInterpolator();
	void addsegment(float x1,float x2,float x3, float x4, float length);
	virtual float interpolate(float position)=0;
};

//interpolator liniowy (z zamknietym cyklem)
class DMLinearInterpolator: public DMInterpolator {
public:
	virtual float interpolate(float position);
};


//interpolator liniowy (z zamknietym cyklem)
class DMCLinearInterpolator: public DMInterpolator {
public:
	virtual float interpolate(float position);
};


//interpolator beziera 
class DMBezierInterpolator: public DMInterpolator {
public:
	virtual float interpolate(float position);
	float derive(float position);
};


//interpolator beziera (cykliczny)
class DMCBezierInterpolator: public DMInterpolator {
public:
	virtual float interpolate(float position);
};

//interpolator do Point3
class DMP3BezierInterpolator {
DMBezierInterpolator* x,*y,*z;
public:
  DMP3BezierInterpolator();
  ~DMP3BezierInterpolator();
	void addsegment(Point3 p1,Point3 p2, Point3 p3,
                  Point3 p4, float length);
	void interpolate(float position,Point3 result);
	void derive(float position,Point3 result);
};

#endif