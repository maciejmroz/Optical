/***************************************************************************
                          effectbase.h  -  description
                             -------------------
    begin                : Fri May 5 2000
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/

#include"demosystem.h"

//pusty efekt
class EEmptyEffect: public DMEffect {
public:
	virtual void render(int time) {};
};
//obracajacy sie kwadrat
//x i y - polozenie srodka
//speed - predkosc obrotowa w rdianach na sekunde
//size - rozmiar
class ERotatingQuad: public DMEffect {
protected:
	DMInterpolator* speed; //radiany/s
	DMInterpolator* x;
	DMInterpolator* y;
	DMInterpolator* size;
	Texture* texture;
public:
	ERotatingQuad(Texture* tex,DMInterpolator* rot_speed, DMInterpolator* x1,
		DMInterpolator* y1,DMInterpolator* q_size);
	virtual void render(int time);
	friend class EQuadBlur;
};

class EQuadBlur: public DMEffect {
	int numsamples;
	int interval;
	ERotatingQuad* effect;
public:
	EQuadBlur(ERotatingQuad* ef,int samples=8,int interv=20);
	virtual void render(int time);
};

class EObjectRenderer: public DMEffect {
	Object3D* obj;
public:
	EObjectRenderer(const char* filename,float size,Texture* tex=NULL,int type=0);
	virtual void render(int time);
	~EObjectRenderer();
};


#define RENDER_TX_SIZE 128

class ESceneZoomer: public DMEffect {
	DMEffect* ef;
	unsigned int tex;
	int maxsize;
	GLubyte data[RENDER_TX_SIZE*RENDER_TX_SIZE*4];
	float speed;
	int lastframe;
public:
	ESceneZoomer(DMEffect* e,float spd=1.5f); // ile razy na sekunde
	virtual void render(int time);
};

class EFadeOut: public DMEffect {
  float len;
public:
  EFadeOut(float length);
  virtual void render(int time);
};

class EFadeIn: public DMEffect {
  float len;
public:
  EFadeIn(float length);
  virtual void render(int time);
};

class EColorFlash : public DMEffect {
	float len;
	float x0,y0,x1,y1;
public:
	EColorFlash(float length,float scr_x0=0.0f,
		float scr_y0=0.0f,float scr_x1=1.0f,float scr_y1=1.0f);
	virtual void render(int time);
};

class EAdditiveImgFlash : public DMEffect {
	float len;
	Texture *tex;
	float x0,y0,x1,y1;
public:
	EAdditiveImgFlash(char* filename,float length,float scr_x0=0.0f,
		float scr_y0=0.0f,float scr_x1=1.0f,float scr_y1=1.0f);
	virtual void render(int time);
};

class EImgFadeIn : public DMEffect {
	float len;
	Texture *tex;
	float x0,y0,x1,y1;
public:
	EImgFadeIn(char* filename,float length,float scr_x0=0.0f,
		float scr_y0=0.0f,float scr_x1=1.0f,float scr_y1=1.0f);
	virtual void render(int time);
};

class ESierpinski: public DMEffect {
 float siz;
 int dep;
 Texture* tex;
public:
 ESierpinski(float size,int depth,Texture* tx=NULL);
 virtual void render(int time);
};

class ESceneRenderer: public DMEffect {
	Scene3D* scene;
public:
	ESceneRenderer(const char* filename,
		SceneRenderFX *tmp_rfx=NULL,float size_factor=1.0f);
	~ESceneRenderer();
	void render(int time=0);
};