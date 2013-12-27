/***************************************************************************
                          mathutils.cpp  -  description
                             -------------------
    begin                : Fri May 5 2000
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/


#include"mathutils.h"

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float

void VAdd3(const Vector3 v1,const Vector3 v2,Vector3 v3) {
	v3[0]=v1[0]+v2[0];
	v3[1]=v1[1]+v2[1];
	v3[2]=v1[2]+v2[2];
}

void VAdd4(const Vector4 v1,const Vector4 v2,Vector4 v3) {
	v3[0]=v1[0]+v2[0];
	v3[1]=v1[1]+v2[1];
	v3[2]=v1[2]+v2[2];
	v3[3]=v1[3]+v2[3];
}

void VCopy3(const Vector3 v1,Vector3 v2) {
	v2[0]=v1[0];
	v2[1]=v1[1];
	v2[2]=v1[2];
}

void VCopy4(const Vector4 v1,Vector4 v2) {
	v2[0]=v1[0];
	v2[1]=v1[1];
	v2[2]=v1[2];
	v2[3]=v1[3];
}

float VDotProduct3(const Vector3 v1,const Vector3 v2) {
	return v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2];
}

float VDotProduct4(const Vector4 v1,const Vector4 v2) {
	return v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2]+v1[3]*v2[3];
}

void VCrossProduct3(const Vector3 v1,const Vector3 v2,Vector3 v3) {
	v3[0]=v1[1]*v2[2]-v1[2]*v2[1];
	v3[1]=v1[2]*v2[0]-v1[0]*v2[2];
	v3[2]=v1[0]*v2[1]-v1[1]*v2[0];
}

void VCrossProduct4(const Vector4 v1,const Vector4 v2,Vector4 v3) {
	v3[0]=v1[1]*v2[2]-v1[2]*v2[1];
	v3[1]=v1[2]*v2[0]-v1[0]*v2[2];
	v3[2]=v1[0]*v2[1]-v1[1]*v2[0];
	v3[3]=1.0f;
}

void VNormalize3(Vector3 v1) {
	float len=sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]);
	if(len > 0.0f) {
	v1[0]/=len;
	v1[1]/=len;
	v1[2]/=len;
	}
	else {
		v1[0]=0.0f;
		v1[1]=0.0f;
		v1[2]=1.0f;
	}
}

void VNormalize4(Vector4 v1) {
	float len=sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]);
	if(len > 0.0f) {
	v1[0]/=len;
	v1[1]/=len;
	v1[2]/=len;
	}
}

float VMagnitude3(const Vector3 v1) {
	return sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]);
}

float VMagnitude4(const Vector4 v1) {
	return sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]);
}

void VSMultiply3(Vector3 v1,float s1) {
	v1[0]*=s1;
	v1[1]*=s1;
	v1[2]*=s1;
}

void VSMultiply4(Vector4 v1,float s1) {
	v1[0]*=s1;
	v1[1]*=s1;
	v1[2]*=s1;
	v1[3]*=s1;
}

void MAdd(const Matrix m1, const Matrix m2, Matrix m3) {
	int i;
	for(i=0;i<16;i++) m3[i]=m1[i]+m2[i];
}

void MCopy(const Matrix src,Matrix dst) {
	int i;
	for(i=0;i<16;i++) dst[i]=src[i];
}

void MMultiply(const Matrix m1,const Matrix m2, Matrix m3) {
	m3[0]=m1[0]*m2[0]+m1[4]*m2[1]+m1[8]*m2[2]+m1[12]*m2[3];
	m3[1]=m1[1]*m2[0]+m1[5]*m2[1]+m1[9]*m2[2]+m1[13]*m2[3];
	m3[2]=m1[2]*m2[0]+m1[6]*m2[1]+m1[10]*m2[2]+m1[14]*m2[3];
	m3[3]=m1[3]*m2[0]+m1[7]*m2[1]+m1[11]*m2[2]+m1[15]*m2[3];

	m3[4]=m1[0]*m2[4]+m1[4]*m2[5]+m1[8]*m2[6]+m1[12]*m2[7];
	m3[5]=m1[1]*m2[4]+m1[5]*m2[5]+m1[9]*m2[6]+m1[13]*m2[7];
	m3[6]=m1[2]*m2[4]+m1[6]*m2[5]+m1[10]*m2[6]+m1[14]*m2[7];
	m3[7]=m1[3]*m2[4]+m1[7]*m2[5]+m1[11]*m2[6]+m1[15]*m2[7];

	m3[8]=m1[0]*m2[8]+m1[4]*m2[9]+m1[8]*m2[10]+m1[12]*m2[11];
	m3[9]=m1[1]*m2[8]+m1[5]*m2[9]+m1[9]*m2[10]+m1[13]*m2[11];
	m3[10]=m1[2]*m2[8]+m1[6]*m2[9]+m1[10]*m2[10]+m1[14]*m2[11];
	m3[11]=m1[3]*m2[8]+m1[7]*m2[9]+m1[11]*m2[10]+m1[15]*m2[11];

	m3[12]=m1[0]*m2[12]+m1[4]*m2[13]+m1[8]*m2[14]+m1[12]*m2[15];
	m3[13]=m1[1]*m2[12]+m1[5]*m2[13]+m1[9]*m2[14]+m1[13]*m2[15];
	m3[14]=m1[2]*m2[12]+m1[6]*m2[13]+m1[10]*m2[14]+m1[14]*m2[15];
	m3[15]=m1[3]*m2[12]+m1[7]*m2[13]+m1[11]*m2[14]+m1[15]*m2[15];
}

void MVMultiply3(const Matrix m1,const Vector3 v1,Vector3 v2) {
	v2[0]=m1[0]*v1[0]+m1[4]*v1[1]+m1[8]*v1[2]+m1[12];
	v2[1]=m1[1]*v1[0]+m1[5]*v1[1]+m1[9]*v1[2]+m1[13];
	v2[2]=m1[2]*v1[0]+m1[6]*v1[1]+m1[10]*v1[2]+m1[14];
}

void MVMultiply4(const Matrix m1,const Vector4 v1,Vector4 v2) {
	v2[0]=m1[0]*v1[0]+m1[4]*v1[1]+m1[8]*v1[2]+m1[12]*v1[3];
	v2[1]=m1[1]*v1[0]+m1[5]*v1[1]+m1[9]*v1[2]+m1[13]*v1[3];
	v2[2]=m1[2]*v1[0]+m1[6]*v1[1]+m1[10]*v1[2]+m1[14]*v1[3];
	v2[3]=m1[3]*v1[0]+m1[7]*v1[1]+m1[11]*v1[2]+m1[15]*v1[3];
}

void MTranspose(Matrix m1) {
	int i,j;
	float t;
	for(i=0;i<4;i++)
		for(j=i+1;j<4;j++) {
			t=m1[4*i+j];
			m1[4*i+j]=m1[4*j+i];
			m1[4*j+i]=t;
	}
}

void MIdentity(Matrix m1) {
	m1[0]=1.0f;
	m1[1]=0.0f;
	m1[2]=0.0f;
	m1[3]=0.0f;
	m1[4]=0.0f;
	m1[5]=1.0f;
	m1[6]=0.0f;
	m1[7]=0.0f;
	m1[8]=0.0f;
	m1[9]=0.0f;
	m1[10]=1.0f;
	m1[11]=0.0f;
	m1[12]=0.0f;
	m1[13]=0.0f;
	m1[14]=0.0f;
	m1[15]=1.0f;
}

void MTranslation(Matrix m1,const Vector3 v1) {
	m1[0]=1.0f;
	m1[1]=0.0f;
	m1[2]=0.0f;
	m1[3]=0.0f;
	m1[4]=0.0f;
	m1[5]=1.0f;
	m1[6]=0.0f;
	m1[7]=0.0f;
	m1[8]=0.0f;
	m1[9]=0.0f;
	m1[10]=1.0f;
	m1[11]=0.0f;
	m1[12]=v1[0];
	m1[13]=v1[1];
	m1[14]=v1[2];
	m1[15]=1.0f;
}

void MRotationX(Matrix m1,float ang) {
	m1[0]=1.0f;
	m1[1]=0.0f;
	m1[2]=0.0f;
	m1[3]=0.0f;
	m1[4]=0.0f;
	m1[5]=cos(ang);
	m1[6]=sin(ang);
	m1[7]=0.0f;
	m1[8]=0.0f;
	m1[9]=-sin(ang);
	m1[10]=cos(ang);
	m1[11]=0.0f;
	m1[12]=0.0f;
	m1[13]=0.0f;
	m1[14]=0.0f;
	m1[15]=1.0f;
}

void MRotationY(Matrix m1,float ang) {
	m1[0]=cos(ang);
	m1[1]=0.0f;
	m1[2]=-sin(ang);
	m1[3]=0.0f;
	m1[4]=0.0f;
	m1[5]=1.0f;
	m1[6]=0.0f;
	m1[7]=0.0f;
	m1[8]=sin(ang);
	m1[9]=0.0f;
	m1[10]=cos(ang);
	m1[11]=0.0f;
	m1[12]=0.0f;
	m1[13]=0.0f;
	m1[14]=0.0f;
	m1[15]=1.0f;
}

void MRotationZ(Matrix m1,float ang) {
	m1[0]=cos(ang);
	m1[1]=sin(ang);
	m1[2]=0.0f;
	m1[3]=0.0f;
	m1[4]=-sin(ang);
	m1[5]=cos(ang);
	m1[6]=0.0f;
	m1[7]=0.0f;
	m1[8]=0.0f;
	m1[9]=0.0f;
	m1[10]=1.0f;
	m1[11]=0.0f;
	m1[12]=0.0f;
	m1[13]=0.0f;
	m1[14]=0.0f;
	m1[15]=1.0f;
}

void MRotationXsc(Matrix m1,float sin_phi,float cos_phi) {
	m1[0]=1.0f;
	m1[1]=0.0f;
	m1[2]=0.0f;
	m1[3]=0.0f;
	m1[4]=0.0f;
	m1[5]=cos_phi;
	m1[6]=sin_phi;
	m1[7]=0.0f;
	m1[8]=0.0f;
	m1[9]=-sin_phi;
	m1[10]=cos_phi;
	m1[11]=0.0f;
	m1[12]=0.0f;
	m1[13]=0.0f;
	m1[14]=0.0f;
	m1[15]=1.0f;
}

void MRotationYsc(Matrix m1,float sin_phi,float cos_phi) {
	m1[0]=cos_phi;
	m1[1]=0.0f;
	m1[2]=-sin_phi;
	m1[3]=0.0f;
	m1[4]=0.0f;
	m1[5]=1.0f;
	m1[6]=0.0f;
	m1[7]=0.0f;
	m1[8]=sin_phi;
	m1[9]=0.0f;
	m1[10]=cos_phi;
	m1[11]=0.0f;
	m1[12]=0.0f;
	m1[13]=0.0f;
	m1[14]=0.0f;
	m1[15]=1.0f;
}

void MRotationZsc(Matrix m1,float sin_phi,float cos_phi) {
	m1[0]=cos_phi;
	m1[1]=sin_phi;
	m1[2]=0.0f;
	m1[3]=0.0f;
	m1[4]=-sin_phi;
	m1[5]=cos_phi;
	m1[6]=0.0f;
	m1[7]=0.0f;
	m1[8]=0.0f;
	m1[9]=0.0f;
	m1[10]=1.0f;
	m1[11]=0.0f;
	m1[12]=0.0f;
	m1[13]=0.0f;
	m1[14]=0.0f;
	m1[15]=1.0f;
}

void MRotationAxis(Matrix m1,const Vector3 v1,const float ang) {
	m1[0]=v1[0]*v1[0]+cos(ang)*(1.0f-v1[0]*v1[0]);
	m1[1]=v1[0]*v1[1]*(1.0f-cos(ang))+v1[2]*sin(ang);
	m1[2]=v1[0]*v1[2]*(1.0f-cos(ang))-v1[1]*sin(ang);
	m1[3]=0.0f;
	m1[4]=v1[0]*v1[1]*(1.0f-cos(ang))-v1[2]*sin(ang);
	m1[5]=v1[1]*v1[1]+cos(ang)*(1.0f-v1[1]*v1[1]);
	m1[6]=v1[1]*v1[2]*(1.0f-cos(ang))+v1[0]*sin(ang);
	m1[7]=0.0f;
	m1[8]=v1[0]*v1[2]*(1.0f-cos(ang))-v1[1]*sin(ang);
	m1[9]=v1[1]*v1[2]*(1.0f-cos(ang))-v1[0]*sin(ang);
	m1[10]=v1[2]*v1[2]+cos(ang)*(1.0f-v1[2]*v1[2]);
	m1[11]=0.0f;
	m1[12]=0.0f;
	m1[13]=0.0f;
	m1[14]=0.0f;
	m1[15]=1.0f;
}

void MScale(Matrix m1,const float xs,const float ys,const float zs) {
	m1[0]=xs;
	m1[1]=0.0f;
	m1[2]=0.0f;
	m1[3]=0.0f;
	m1[4]=0.0f;
	m1[5]=ys;
	m1[6]=0.0f;
	m1[7]=0.0f;
	m1[8]=0.0f;
	m1[9]=0.0f;
	m1[10]=zs;
	m1[11]=0.0f;
	m1[12]=0.0f;
	m1[13]=0.0f;
	m1[14]=0.0f;
	m1[15]=1.0f;
}

int MInvert(Matrix m,Matrix out) {

/*
 * Compute inverse of 4x4 transformation matrix.
 * Code contributed by Jacques Leroy <jle@star.be>
 * Code lifted from Brian Paul's Mesa freeware OpenGL implementation.
 * Return GL_TRUE for success, GL_FALSE for failure (singular matrix)
 */
#define SWAP_ROWS(a, b) { double *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]

 double wtmp[4][8];
 double m0, m1, m2, m3, s;
 double *r0, *r1, *r2, *r3;

 r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

 r0[0] = MAT(m,0,0), r0[1] = MAT(m,0,1),
 r0[2] = MAT(m,0,2), r0[3] = MAT(m,0,3),
 r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,

 r1[0] = MAT(m,1,0), r1[1] = MAT(m,1,1),
 r1[2] = MAT(m,1,2), r1[3] = MAT(m,1,3),
 r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,

 r2[0] = MAT(m,2,0), r2[1] = MAT(m,2,1),
 r2[2] = MAT(m,2,2), r2[3] = MAT(m,2,3),
 r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,

 r3[0] = MAT(m,3,0), r3[1] = MAT(m,3,1),

 r3[2] = MAT(m,3,2), r3[3] = MAT(m,3,3),
 r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

 /* choose pivot - or die */
 if (fabs(r3[0])>fabs(r2[0])) SWAP_ROWS(r3, r2);
 if (fabs(r2[0])>fabs(r1[0])) SWAP_ROWS(r2, r1);
 if (fabs(r1[0])>fabs(r0[0])) SWAP_ROWS(r1, r0);
 if (0.0 == r0[0]) {
  return 0;
 }

 /* eliminate first variable     */
 m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
 s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
 s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
 s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
 s = r0[4];
 if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
 s = r0[5];
 if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
 s = r0[6];
 if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
 s = r0[7];
 if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

 /* choose pivot - or die */
 if (fabs(r3[1])>fabs(r2[1])) SWAP_ROWS(r3, r2);
 if (fabs(r2[1])>fabs(r1[1])) SWAP_ROWS(r2, r1);
 if (0.0 == r1[1]) {
  return 0;
 }

 /* eliminate second variable */
 m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
 r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
 r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
 s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
 s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
 s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
 s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

 /* choose pivot - or die */
 if (fabs(r3[2])>fabs(r2[2])) SWAP_ROWS(r3, r2);
 if (0.0 == r2[2]) {
  return 0;
 }

 /* eliminate third variable */
 m3 = r3[2]/r2[2];
 r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
 r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
 r3[7] -= m3 * r2[7];

 /* last check */
 if (0.0 == r3[3]) {
  return 0;
 }

 s = 1.0/r3[3];              /* now back substitute row 3 */
 r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

 m2 = r2[3];                 /* now back substitute row 2 */

 s  = 1.0/r2[2];
 r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
 r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
 m1 = r1[3];
 r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
 r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
 m0 = r0[3];
 r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
 r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

 m1 = r1[2];                 /* now back substitute row 1 */
 s  = 1.0/r1[1];
 r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
 r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
 m0 = r0[2];
 r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
 r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

 m0 = r0[1];                 /* now back substitute row 0 */
 s  = 1.0/r0[0];
 r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
 r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

 MAT(out,0,0) = r0[4]; MAT(out,0,1) = r0[5],
 MAT(out,0,2) = r0[6]; MAT(out,0,3) = r0[7],
 MAT(out,1,0) = r1[4]; MAT(out,1,1) = r1[5],
 MAT(out,1,2) = r1[6]; MAT(out,1,3) = r1[7],
 MAT(out,2,0) = r2[4]; MAT(out,2,1) = r2[5],
 MAT(out,2,2) = r2[6]; MAT(out,2,3) = r2[7],
 MAT(out,3,0) = r3[4]; MAT(out,3,1) = r3[5],
 MAT(out,3,2) = r3[6]; MAT(out,3,3) = r3[7]; 

 return 1;

#undef MAT
#undef SWAP_ROWS
}

void MFrustum(Matrix m,double l, double r, double b, 
			  double t,double n, double f) {
  m[0] = (2.0*n) / (r-l);
  m[1] = 0.0;
  m[2] = 0.0;
  m[3] = 0.0;

  m[4] = 0.0;
  m[5] = (2.0*n) / (t-b);
  m[6] = 0.0;
  m[7] = 0.0;

  m[8] = (r+l) / (r-l);
  m[9] = (t+b) / (t-b);
  m[10] = -(f+n) / (f-n);
  m[11] = -1.0;

  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = -(2.0*f*n) / (f-n);
  m[15] = 0.0;
}

void MPerspective(Matrix m,double fovy, double aspect,
				  double zNear, double zFar)
{
  double xmin, xmax, ymin, ymax;

  ymax = zNear * tan(fovy * 3.14159 / 360.0);
  ymin = -ymax;

  xmin = ymin * aspect;
  xmax = ymax * aspect;

  MFrustum(m, xmin, xmax, ymin, ymax, zNear, zFar);
}


/* Build a 4x4 matrix transform based on the parameters for gluLookAt.
 * Code lifted from Brian Paul's MesaGLU.
 */
void MLookAt(Matrix m,double eyex, double eyey, double eyez,
			 double centerx, double centery, double centerz,
			 double upx, double upy, double upz)
{
   double x[3], y[3], z[3];
   double mag;

   /* Make rotation matrix */

   /* Z vector */
   z[0] = eyex - centerx;
   z[1] = eyey - centery;
   z[2] = eyez - centerz;
   mag = sqrt( z[0]*z[0] + z[1]*z[1] + z[2]*z[2] );
   if (mag) {  /* mpichler, 19950515 */
      z[0] /= mag;
      z[1] /= mag;
      z[2] /= mag;
   }

   /* Y vector */
   y[0] = upx;
   y[1] = upy;
   y[2] = upz;

   /* X vector = Y cross Z */
   x[0] =  y[1]*z[2] - y[2]*z[1];
   x[1] = -y[0]*z[2] + y[2]*z[0];
   x[2] =  y[0]*z[1] - y[1]*z[0];

   /* Recompute Y = Z cross X */
   y[0] =  z[1]*x[2] - z[2]*x[1];
   y[1] = -z[0]*x[2] + z[2]*x[0];
   y[2] =  z[0]*x[1] - z[1]*x[0];

   /* mpichler, 19950515 */
   /* cross product gives area of parallelogram, which is < 1.0 for
    * non-perpendicular unit-length vectors; so normalize x, y here
    */

   mag = sqrt( x[0]*x[0] + x[1]*x[1] + x[2]*x[2] );
   if (mag) {
      x[0] /= mag;
      x[1] /= mag;
      x[2] /= mag;
   }

   mag = sqrt( y[0]*y[0] + y[1]*y[1] + y[2]*y[2] );
   if (mag) {
      y[0] /= mag;
      y[1] /= mag;
      y[2] /= mag;
   }

#define M(row,col)  m[col*4+row]
   M(0,0) = x[0];  M(0,1) = x[1];  M(0,2) = x[2];  M(0,3) = -x[0]*eyex + -x[1]*eyey + -x[2]*eyez;
   M(1,0) = y[0];  M(1,1) = y[1];  M(1,2) = y[2];  M(1,3) = -y[0]*eyex + -y[1]*eyey + -y[2]*eyez;
   M(2,0) = z[0];  M(2,1) = z[1];  M(2,2) = z[2];  M(2,3) = -z[0]*eyex + -z[1]*eyey + -z[2]*eyez;
   M(3,0) = 0.0;   M(3,1) = 0.0;   M(3,2) = 0.0;   M(3,3) = 1.0;
#undef M
}


void PDiff3(const Point3 p1,const Point3 p2,Vector3 v1) {
	v1[0]=p2[0]-p1[0];
	v1[1]=p2[1]-p1[1];
	v1[2]=p2[2]-p1[2];
}

void PDiff4(const Point4 p1,const Point4 p2,Vector4 v1) {
	v1[0]=p2[0]-p1[0];

	v1[1]=p2[1]-p1[1];
	v1[2]=p2[2]-p1[2];
	v1[3]=p2[3]-p1[3];
}

void PVAdd3(const Point3 p1,const Vector3 v1, Point3 p2) {
	p2[0]=p1[0]+v1[0];
	p2[1]=p1[1]+v1[1];
	p2[2]=p1[2]+v1[2];
}

void PVAdd4(const Point4 p1,const Vector4 v1, Point4 p2) {
	p2[0]=p1[0]+v1[0];
	p2[1]=p1[1]+v1[1];
	p2[2]=p1[2]+v1[2];
	p2[3]=p1[3]+v1[3];
}

/*** FAST INVERSE SQUARE ROOT ***/

/* From "Graphics Gems V", Alan Paeth (Editor)
 * ISBN 0125434553/9649-1547332-306386
 * Published by Ap Profession, 1995
 */

/* Compute the Inverse Square Root
 * of an IEEE Single Precision Floating-Point number.
 *
 * Written by Ken Turkowski.
 */

/* Specified parameters */
#define LOOKUP_BITS    6   /* Number of mantissa bits for lookup */
#define EXP_POS       23   /* Position of the exponent */
#define EXP_BIAS     127   /* Bias of exponent */
/* The mantissa is assumed to be just down from the exponent */

/* Derived parameters */
#define LOOKUP_POS   (EXP_POS-LOOKUP_BITS)  /* Position of mantissa lookup */
#define SEED_POS     (EXP_POS-8)            /* Position of mantissa seed */
#define TABLE_SIZE   (2 << LOOKUP_BITS)     /* Number of entries in table */
#define LOOKUP_MASK  (TABLE_SIZE - 1)           /* Mask for table input */
#define GET_EXP(a)   (((a) >> EXP_POS) & 0xFF)  /* Extract exponent */
#define SET_EXP(a)   ((a) << EXP_POS)           /* Set exponent */
#define GET_EMANT(a) (((a) >> LOOKUP_POS) & LOOKUP_MASK)  /* Extended mantissa
                                                           * MSB's */
#define SET_MANTSEED(a) (((unsigned long)(a)) << SEED_POS)  /* Set mantissa
                                                             * 8 MSB's */

static unsigned char iSqrt[TABLE_SIZE];

union _flint {
  unsigned long    i;
  float            f;
} _fi, _fo;

void
makeInverseSqrtLookupTable(void)
{
  register long f;
  register unsigned char *h;
  union _flint fi, fo;

  h = iSqrt;
  for (f = 0, h = iSqrt; f < TABLE_SIZE; f++) {
    fi.i = ((EXP_BIAS-1) << EXP_POS) | (f << LOOKUP_POS);
    fo.f = (float) (1.0 / sqrt(fi.f));
    *h++ = (unsigned char)
           (((fo.i + (1<<(SEED_POS-2))) >> SEED_POS) & 0xFF); /* rounding */
  }
  iSqrt[TABLE_SIZE / 2] = 0xFF;    /* Special case for 1.0 */
}

/* Non-WinTel platforms don't need fastcall. */
#ifndef FASTCALL
#define FASTCALL
#endif

/* The following returns the inverse square root. */
static float FASTCALL
invSqrt(float x)
{
  unsigned long a = ((union _flint*)(&x))->i;
  float arg = x;
  union _flint seed;
  float r;

  seed.i = SET_EXP(((3*EXP_BIAS-1) - GET_EXP(a)) >> 1)
         | SET_MANTSEED(iSqrt[GET_EMANT(a)]);

  /* Seed: accurate to LOOKUP_BITS */
  r = seed.f;

  /* First iteration: accurate to 2*LOOKUP_BITS */
  r = (float) ((3.0 - r * r * arg) * r * 0.5);

#if 0  /* Wow!  We don't need this much precision! */
  /* Second iteration: accurate to 4*LOOKUP_BITS */
  r = (float) ((3.0 - r * r * arg) * r * 0.5);
#endif

  return r;
}


/*---------------------------------------------------------------------------
	Floating Point math
	Author: Ignacio Castaño Aguado (part of his Titan engine code)
---------------------------------------------------------------------------*/

__declspec(naked) float __fastcall FastAbs(float a) {
	__asm {
		fld		DWORD PTR [esp+4] 
		fabs
		ret 4
	}
}

__declspec(naked) float __fastcall FastSin(float a) {
	__asm {
		fld		DWORD PTR [esp+4] 
		fsin
		ret 4
	}
}

__declspec(naked) float __fastcall FastCos(float a) {
	__asm {
		fld		DWORD PTR [esp+4] 
		fcos
		ret 4
	}
}



