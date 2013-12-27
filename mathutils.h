/***************************************************************************
                          mathutils.h  -  description
                             -------------------
    begin                : Fri May 5 2000
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/


#ifndef _MATHUTILS_H_
#define _MATHUTILS_H_

#include<math.h>

//"typy" podstawowe
typedef float Point3[3];
typedef float Vector3[3];
typedef float Point4[4];
typedef float Vector4[4];

//macierz przechowywana zgodnie ze stylem OpenGL
typedef float Matrix[16];

//
//operacje podstawowe dla wektorow
//

//dodawanie
void VAdd3(const Vector3 v1,const Vector3 v2,Vector3 v3);
void VAdd4(const Vector4 v1,const Vector4 v2,Vector4 v3);

//kopiowanie
void VCopy3(const Vector3 v1,Vector3 v2);
void VCopy4(const Vector4 v1,Vector4 v2);

//iloczyn skalarny
float VDotProduct3(const Vector3 v1,const Vector3 v2);
float VDotProduct4(const Vector4 v1,const Vector4 v2);

//iloczyn wektorowy
void VCrossProduct3(const Vector3 v1,const Vector3 v2,Vector3 v3);
void VCrossProduct4(const Vector4 v1,const Vector4 v2,Vector4 v3);

//normalizacja
void VNormalize3(Vector3 v1);
void VNormalize4(Vector4 v1);

//dlugosc wektora
float VMagnitude3(const Vector3 v1);
float VMagnitude4(const Vector4 v1);

//mnozenie wektora przez skalar
void VSMultiply3(Vector3 v1,float s1);
void VSMultiply4(Vector4 v1,float s1);

//
//operacje podstawowe dla macierzy
//

//dodawanie i mnozenie 
void MAdd(const Matrix m1, const Matrix m2, Matrix m3);
void MMultiply(const Matrix m1,const Matrix m2, Matrix m3);

//mnozenie macierzy przez wektor
void MVMultiply3(const Matrix m1,const Vector3 v1,Vector3 v2);
void MVMultiply4(const Matrix m1,const Vector4 v1,Vector4 v2);

//kopiowanie
void MCopy(const Matrix src,Matrix dst);

//transpozycja 
void MTranspose(Matrix m1);

//macierz odwrotna
int MInvert(Matrix m,Matrix out);

//macierz projekcji perspektywicznej
void MFrustum(Matrix m,double l, double r, double b, 
			  double t,double n, double f);

//j.w. ale z parametrami a la gluLookAt
void MPerspective(Matrix m,double fovy, double aspect,
				  double zNear, double zFar);


/* Build a 4x4 matrix transform based on the parameters for gluLookAt.
 * Code lifted from Brian Paul's MesaGLU.
 */
void MLookAt(Matrix m,double eyex, double eyey, double eyez,
			 double centerx, double centery, double centerz,
			 double upx, double upy, double upz);

//macierz jednostkowa
void MIdentity(Matrix m1);
//translacja,rotacja i skalowanie
void MTranslation(Matrix m1,const Vector3 v1);
void MRotationX(Matrix m1,float ang);
void MRotationY(Matrix m1,float ang);
void MRotationZ(Matrix m1,float ang);
void MRotationAxis(Matrix m1,const Vector3 v1,const float ang);
void MScale(Matrix m1,const float xs,const float ys,const float zs);

void MRotationXsc(Matrix m1,float sin_phi,float cos_phi);
void MRotationYsc(Matrix m1,float sin_phi,float cos_phi);
void MRotationZsc(Matrix m1,float sin_phi,float cos_phi);

//tworzenie wektora z punktu, punkt + wektor itd
void PDiff3(const Point3 p1,const Point3 p2,Vector3 v1);
void PDiff4(const Point4 p1,const Point4 p2,Vector4 v1);

void PVAdd3(const Point3 p1,const Vector3 v1,Point3 p2);
void PVAdd4(const Point4 p1,const Vector4 v1,Point4 p2);

//
// Floating Point Math
//
float __fastcall FastAbs(float a);
float __fastcall FastSin(float a);
float __fastcall FastCos(float a);


#endif