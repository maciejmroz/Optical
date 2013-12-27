/***************************************************************************
                          effectbase.cpp  -  description
                             -------------------
    begin                : Fri May 5 2000
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/

#include "engine.h"
#include "effectbase.h"

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float


ERotatingQuad::ERotatingQuad(Texture* tex,DMInterpolator* rot_speed, DMInterpolator* x1,
			  DMInterpolator* y1,DMInterpolator* q_size) {
x=x1;
y=y1;
speed=rot_speed;
size=q_size;
texture=tex;
}


/*glBegin(GL_QUADS);
	 glTexCoord2f((0.5f-t_size)*sin(t_speed*time/1000.0f)-(0.5f+t_size)*cos(t_speed*time/1000.0f),
		 (0.5f-t_size)*sin(t_speed*time/1000.0f)+(0.5f+t_size)*cos(t_speed*time/1000.0f));
	 glVertex2f(-0.625f,0.5f);

	 glTexCoord2f((0.5f-t_size)*sin(t_speed*time/1000.0f)-(0.5f-t_size)*cos(t_speed*time/1000.0f),
		 (0.5f-t_size)*sin(t_speed*time/1000.0f)+(0.5f-t_size)*cos(t_speed*time/1000.0f));
	 glVertex2f(-0.625f,-0.5f);

	 glTexCoord2f((0.5f+t_size)*sin(t_speed*time/1000.0f)-(0.5f-t_size)*cos(t_speed*time/1000.0f),
		 (0.5f+t_size)*sin(t_speed*time/1000.0f)+(0.5f-t_size)*cos(t_speed*time/1000.0f));
	 glVertex2f(0.625f,-0.5f);

	 glTexCoord2f((0.5f+t_size)*sin(t_speed*time/1000.0f)-(0.5f+t_size)*cos(t_speed*time/1000.0f),
		 (0.5f+t_size)*sin(t_speed*time/1000.0f)+(0.5f+t_size)*cos(t_speed*time/1000.0f));
	 glVertex2f(0.625f,0.5f);
glEnd();
*/
/*glBegin(GL_QUADS);
	 glTexCoord2f(-t_size*sin(t_speed*time/1000.0f)-t_size*cos(t_speed*time/1000.0f),
		 -t_size*sin(t_speed*time/1000.0f)+t_size*cos(t_speed*time/1000.0f));
	 glVertex2f(-0.625f,0.5f);

	 glTexCoord2f(-t_size*sin(t_speed*time/1000.0f)+t_size*cos(t_speed*time/1000.0f),
		 -t_size*sin(t_speed*time/1000.0f)-t_size*cos(t_speed*time/1000.0f));
	 glVertex2f(-0.625f,-0.5f);

	 glTexCoord2f(t_size*sin(t_speed*time/1000.0f)+t_size*cos(t_speed*time/1000.0f),
		 t_size*sin(t_speed*time/1000.0f)-t_size*cos(t_speed*time/1000.0f));
	 glVertex2f(0.625f,-0.5f);

	 glTexCoord2f(t_size*sin(t_speed*time/1000.0f)-t_size*cos(t_speed*time/1000.0f),
		 t_size*sin(t_speed*time/1000.0f)+t_size*cos(t_speed*time/1000.0f));
	 glVertex2f(0.625f,0.5f);
glEnd();
*/

void ERotatingQuad::render(int time) {
texture->activate();
float t_size=size->interpolate(time);
float t_speed=speed->interpolate(time);
float t_x=x->interpolate(time);
float t_y=y->interpolate(time);
c_state->want_disabled(GL_LIGHTING);
c_state->want_enabled(GL_TEXTURE_2D);
glBegin(GL_QUADS);
	 glTexCoord2f(0.5f+t_size*sin(t_speed*time/1000.0f),
		 0.5f+1.33f*t_size*cos(t_speed*time/1000.0f));
	 glVertex2f(-0.625f,0.5f);

	 glTexCoord2f(0.5f+t_size*sin(0.5f*3.14159f+t_speed*time/1000.0f),
		 0.5f+1.33f*t_size*cos(0.5f*3.14159f+t_speed*time/1000.0f));
	 glVertex2f(-0.625f,-0.5f);

	 glTexCoord2f(0.5f+t_size*sin(3.14159f+t_speed*time/1000.0f),
		 0.5f+1.33f*t_size*cos(3.14159f+t_speed*time/1000.0f));
	 glVertex2f(0.625f,-0.5f);

	 glTexCoord2f(0.5f+t_size*sin(1.5f*3.14159f+t_speed*time/1000.0f),
		 0.5f+1.33f*t_size*cos(1.5f*3.14159f+t_speed*time/1000.0f));
	 glVertex2f(0.625f,0.5f);
glEnd();
}


EQuadBlur::EQuadBlur(ERotatingQuad* ef,int samples,int interv) {
	effect=ef;
	numsamples=samples;
	interval=interv;
}

void EQuadBlur::render(int time) {
	int i;
	c_state->want_enabled(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	effect->texture->activate();
	c_state->want_disabled(GL_LIGHTING);
	c_state->want_enabled(GL_TEXTURE_2D);
	if(hasMultitexture && hasTextureEnvCombine) {
		glActiveTextureARB(GL_TEXTURE0_ARB);
		effect->texture->activate();
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glEnable(GL_TEXTURE_2D);
		effect->texture->activate();
		float env_col[4]={0.5f,0.5f,0.5f,0.5f};
		glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE_EXT);
		glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,env_col);
		glTexEnvf(GL_TEXTURE_ENV,GL_COMBINE_RGB_EXT,GL_INTERPOLATE_EXT);
		glTexEnvf(GL_TEXTURE_ENV,GL_COMBINE_ALPHA_EXT,GL_REPLACE);
		glTexEnvf(GL_TEXTURE_ENV,GL_SOURCE0_RGB_EXT,GL_PREVIOUS_EXT);
		glTexEnvf(GL_TEXTURE_ENV,GL_SOURCE1_RGB_EXT,GL_TEXTURE);
		glTexEnvf(GL_TEXTURE_ENV,GL_SOURCE2_RGB_EXT,GL_CONSTANT_EXT);
		glTexEnvf(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA_EXT,GL_PRIMARY_COLOR_EXT);
		glTexEnvf(GL_TEXTURE_ENV,GL_OPERAND0_RGB_EXT,GL_SRC_COLOR);
		glTexEnvf(GL_TEXTURE_ENV,GL_OPERAND1_RGB_EXT,GL_SRC_COLOR);
		glTexEnvf(GL_TEXTURE_ENV,GL_OPERAND2_RGB_EXT,GL_SRC_COLOR);
		glTexEnvf(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA_EXT,GL_SRC_ALPHA);
		glTexEnvf(GL_TEXTURE_ENV,GL_OPERAND2_ALPHA_EXT,GL_SRC_ALPHA);
		for(i=0;i<(numsamples/2);i++) {
			glColor4f(1.0f,1.0f,1.0f,1.0f/(float)(i+1));
			float t_size=effect->size->interpolate(time-2*i*interval);
			float t_speed=effect->speed->interpolate(time-2*i*interval);
			float t_x=effect->x->interpolate(time-2*i*interval);
			float t_y=effect->y->interpolate(time-2*i*interval);
			float t_size2=effect->size->interpolate(time-(2*i+1)*interval);
			float t_speed2=effect->speed->interpolate(time-(2*i+1)*interval);
			float t_x2=effect->x->interpolate(time-(2*i+1)*interval);
			float t_y2=effect->y->interpolate(time-(2*i+1)*interval);
			glBegin(GL_QUADS);
			if((time-2*i*interval)>0) {
				 glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0.5f+t_size*sin(t_speed*(time-2*i*interval)/1000.0f),
					 0.5f+1.33f*t_size*cos(t_speed*(time-2*i*interval)/1000.0f));
			}
			else {
				 glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0.5f,0.5f+1.33f*t_size);
			}
			if((time-(2*i+1)*interval)>0) {
				 glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0.5f+t_size2*sin(t_speed2*(time-(2*i+1)*interval)/1000.0f),
					 0.5f+1.33f*t_size2*cos(t_speed2*(time-(2*i+1)*interval)/1000.0f));
			}
			else {
				 glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0.5f,0.5f+1.33f*t_size2);
			}
			glVertex2f(-0.625f,0.5f);

			if((time-2*i*interval)>0) {
				 glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0.5f+t_size*sin(0.5f*3.14159f+t_speed*(time-2*i*interval)/1000.0f),
					 0.5f+1.33f*t_size*cos(0.5f*3.14159f+t_speed*(time-2*i*interval)/1000.0f));
			}
			else {
				 glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0.5f+t_size*sin(0.5f*3.14159f),
					 0.5f+1.33f*t_size*cos(0.5f*3.14159f));
			}
			if((time-(2*i+1)*interval)>0) {
				 glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0.5f+t_size2*sin(0.5f*3.14159f+t_speed2*(time-(2*i+1)*interval)/1000.0f),
					 0.5f+1.33f*t_size2*cos(0.5f*3.14159f+t_speed2*(time-(2*i+1)*interval)/1000.0f));
			}
			else {
				 glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0.5f+t_size2*sin(0.5f*3.14159f),
					 0.5f+1.33f*t_size2*cos(0.5f*3.14159f));
			}
			glVertex2f(-0.625f,-0.5f);

			if((time-2*i*interval)>0) {
				 glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0.5f+t_size*sin(3.14159f+t_speed*(time-2*i*interval)/1000.0f),
					 0.5f+1.33f*t_size*cos(3.14159f+t_speed*(time-2*i*interval)/1000.0f));
			}
			else {
				 glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0.5f+t_size*sin(3.14159f),
					 0.5f+1.33f*t_size*cos(3.14159f));
			}
			if((time-(2*i+1)*interval)>0) {
				 glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0.5f+t_size2*sin(3.14159f+t_speed2*(time-(2*i+1)*interval)/1000.0f),
					 0.5f+1.33f*t_size2*cos(3.14159f+t_speed2*(time-(2*i+1)*interval)/1000.0f));
			}
			else {
				 glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0.5f+t_size2*sin(3.14159f),
					 0.5f+1.33f*t_size2*cos(3.14159f));
			}
			glVertex2f(0.625f,-0.5f);

			if((time-2*i*interval)>0) {
				 glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0.5f+t_size*sin(1.5f*3.14159f+t_speed*(time-2*i*interval)/1000.0f),
					 0.5f+1.33f*t_size*cos(1.5f*3.14159f+t_speed*(time-2*i*interval)/1000.0f));
			}
			else {
				 glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0.5f+t_size*sin(1.5f*3.14159f),
					 0.5f+1.33f*t_size*cos(1.5f*3.14159f));
			}
			if((time-(2*i+1)*interval)>0) {
				 glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0.5f+t_size2*sin(1.5f*3.14159f+t_speed2*(time-(2*i+1)*interval)/1000.0f),
					 0.5f+1.33f*t_size2*cos(1.5f*3.14159f+t_speed2*(time-(2*i+1)*interval)/1000.0f));
			}
			else {
				 glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0.5f+t_size2*sin(1.5f*3.14159f),
					 0.5f+1.33f*t_size2*cos(1.5f*3.14159f));
			}
			glVertex2f(0.625f,0.5f);
			glEnd();
		}
		glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
	}
	else {
		for(i=0;i<numsamples;i++) {
			glColor4f(1.0f,1.0f,1.0f,1.0f/(float)(i+1));
			if((time-i*interval)>0)	{
				float t_size=effect->size->interpolate(time-i*interval);
				float t_speed=effect->speed->interpolate(time-i*interval);
				float t_x=effect->x->interpolate(time-i*interval);
				float t_y=effect->y->interpolate(time-i*interval);
				glBegin(GL_QUADS);
					 glTexCoord2f(0.5f+t_size*sin(t_speed*(time-i*interval)/1000.0f),
						 0.5f+1.33f*t_size*cos(t_speed*(time-i*interval)/1000.0f));
					 glVertex2f(-0.625f,0.5f);
	
					 glTexCoord2f(0.5f+t_size*sin(0.5f*3.14159f+t_speed*(time-i*interval)/1000.0f),
						 0.5f+1.33f*t_size*cos(0.5f*3.14159f+t_speed*(time-i*interval)/1000.0f));
					 glVertex2f(-0.625f,-0.5f);

					 glTexCoord2f(0.5f+t_size*sin(3.14159f+t_speed*(time-i*interval)/1000.0f),
						 0.5f+1.33f*t_size*cos(3.14159f+t_speed*(time-i*interval)/1000.0f));
					 glVertex2f(0.625f,-0.5f);

					 glTexCoord2f(0.5f+t_size*sin(1.5f*3.14159f+t_speed*(time-i*interval)/1000.0f),
						 0.5f+1.33f*t_size*cos(1.5f*3.14159f+t_speed*(time-i*interval)/1000.0f));
					 glVertex2f(0.625f,0.5f);
				glEnd();

			}
			else {
				float t_size=effect->size->interpolate(0);
				float t_speed=effect->speed->interpolate(0);
				float t_x=effect->x->interpolate(0);
				float t_y=effect->y->interpolate(0);
				glBegin(GL_QUADS);
					 glTexCoord2f(0.5f,0.5f+1.33f*t_size);
					 glVertex2f(-0.625f,0.5f);

					 glTexCoord2f(0.5f+t_size*sin(0.5f*3.14159f),
						 0.5f+1.33f*t_size*cos(0.5f*3.14159f));
					 glVertex2f(-0.625f,-0.5f);

					 glTexCoord2f(0.5f+t_size*sin(3.14159f),
						 0.5f+1.33f*t_size*cos(3.14159f));
					 glVertex2f(0.625f,-0.5f);

					 glTexCoord2f(0.5f+t_size*sin(1.5f*3.14159f),
						 0.5f+1.33f*t_size*cos(1.5f*3.14159f));
					 glVertex2f(0.625f,0.5f);
				glEnd();
			}
		}
	}
	c_state->want_disabled(GL_BLEND);
}

EObjectRenderer::EObjectRenderer(const char* filename,float size,Texture* tex,int type) {
	obj=new Object3D();
	obj->load_LWO_file(filename);
	obj->align_to_box(size);
}

EObjectRenderer::~EObjectRenderer() {
	delete obj;
}

void EObjectRenderer::render(int time) {
	c_state->want_enabled(GL_LIGHTING);
	c_state->want_disabled(GL_BLEND);
	glPushMatrix();
	glTranslatef(0.0,0.0,0.20);
	glRotatef(0.03*time,0.0,1.0,0.0);
	glRotatef(0.1*time,1.0,0.0,0.0);
	glRotatef(0.03*time,0.0,0.0,1.0);
	obj->render(time);
	glPopMatrix();
}

ESceneZoomer::ESceneZoomer(DMEffect* e,float spd) {
	ef=e;
	speed=spd;
	int i;
	for(i=0;i<RENDER_TX_SIZE*RENDER_TX_SIZE*4;i++) data[i]=0;
	glGenTextures(GL_TEXTURE_2D,&tex);
    glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
       RENDER_TX_SIZE, RENDER_TX_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	lastframe=0;

};
//zmiana y na przeciwny
void ESceneZoomer::render(int time) {
	int dif = time - lastframe;
	lastframe=time;
	float zoom=pow(speed,dif/1000.0f);
	c_state->want_disabled(GL_LIGHTING);
	c_state->want_enabled(GL_TEXTURE_2D);
	c_state->want_disabled(GL_BLEND);
	glViewport(0,0,RENDER_TX_SIZE,RENDER_TX_SIZE);
	glBindTexture(GL_TEXTURE_2D,tex);
	glBegin(GL_QUADS);
	 glTexCoord2f(0.5f-0.5f/zoom,0.5f+0.5f/zoom);
	 glVertex2f(-0.625f,0.5f);

	 glTexCoord2f(0.5f-0.5f/zoom,0.5f-0.5f/zoom);
	 glVertex2f(-0.625f,-0.5f);

	 glTexCoord2f(0.5f+0.5f/zoom,0.5f-0.5f/zoom);
	 glVertex2f(0.625f,-0.5f);

	 glTexCoord2f(0.5f+0.5f/zoom,0.5f+0.5f/zoom);
	 glVertex2f(0.625f,0.5f);
	glEnd();
	ef->render(time);
	c_state->want_disabled(GL_LIGHTING);
	c_state->want_enabled(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,tex);
	glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,RENDER_TX_SIZE,RENDER_TX_SIZE);
	glViewport(0,0,640,480);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_QUADS);
	 glTexCoord2f(0.5f-0.5f/zoom,0.5f+0.5f/zoom);
	 glVertex2f(-0.625f,0.5f);

	 glTexCoord2f(0.5f-0.5f/zoom,0.5f-0.5f/zoom);
	 glVertex2f(-0.625f,-0.5f);

	 glTexCoord2f(0.5f+0.5f/zoom,0.5f-0.5f/zoom);
	 glVertex2f(0.625f,-0.5f);

	 glTexCoord2f(0.5f+0.5f/zoom,0.5f+0.5f/zoom);
	 glVertex2f(0.625f,0.5f);
	glEnd();
	ef->render(time);
}

EFadeOut::EFadeOut(float length) {
 len=length;
}

void EFadeOut::render(int time) {
 if(time<len) {
 glColor4f(0.0f,0.0f,0.0f,time/len);
 DrawQuad(NULL,0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,1.0f,1,GL_SRC_ALPHA,
	 GL_ONE_MINUS_SRC_ALPHA,0);
 }
}

EFadeIn::EFadeIn(float length) {
 len=length;
}

void EFadeIn::render(int time) {
 if(time<len) {
 glColor4f(0.0f,0.0f,0.0f,1.0f-time/len);
 DrawQuad(NULL,0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,1.0f,1,GL_SRC_ALPHA,
	 GL_ONE_MINUS_SRC_ALPHA,0);
 }
}

EColorFlash::EColorFlash(float length,float scr_x0,float scr_y0,float scr_x1,
						 float scr_y1) {
	x0=scr_x0;
	y0=scr_y0;
	x1=scr_x1;
	y1=scr_y1;
	len=length;
}

void EColorFlash::render(int time) {
	if(time<0) return;
	if(time>len) return;
	if(time<(len*0.5f)) {
		glColor4f(1.0f,1.0f,1.0f,2.0f*time/len);
		DrawQuad(NULL,x0,y0,x1,y1,0.0f,0.0f,1.0f,1.0f,1,GL_SRC_ALPHA,
			GL_ONE_MINUS_SRC_ALPHA,0);
	}
	else {
		glColor4f(1.0f,1.0f,1.0f,1.0f-2.0f*(time-0.5f*len)/len);
		DrawQuad(NULL,x0,y0,x1,y1,0.0f,0.0f,1.0f,1.0f,1,GL_SRC_ALPHA,
			GL_ONE_MINUS_SRC_ALPHA,0);
	}
}

EAdditiveImgFlash::EAdditiveImgFlash(char *filename,float length,float scr_x0,float scr_y0,
									 float scr_x1,float scr_y1) {
	tex=tex_manager.get_texture(filename);
	x0=scr_x0;
	y0=scr_y0;
	x1=scr_x1;
	y1=scr_y1;
	len=length;
}

void EAdditiveImgFlash::render(int time) {
	if(time<0) return;
	if(time>len) return;
	if(time<(len*0.5f)) {
		glColor4f(1.0f,1.0f,1.0f,2.0f*time/len);
		DrawQuad(tex,x0,y0,x1,y1,0.0f,0.0f,1.0f,1.0f,1,GL_SRC_ALPHA,GL_ONE,0);
	}
	else {
		glColor4f(1.0f,1.0f,1.0f,1.0f-2.0f*(time-0.5f*len)/len);
		DrawQuad(tex,x0,y0,x1,y1,0.0f,0.0f,1.0f,1.0f,1,GL_SRC_ALPHA,GL_ONE,0);
	}
}

EImgFadeIn::EImgFadeIn(char *filename,float length,float scr_x0,float scr_y0,
									 float scr_x1,float scr_y1) {
	tex=tex_manager.get_texture(filename);
	x0=scr_x0;
	y0=scr_y0;
	x1=scr_x1;
	y1=scr_y1;
	len=length;
}

void EImgFadeIn::render(int time) {  //hacked for Mekka demo
	if(time<0) return;
	float alpha_val;
	if(time>len) {
		alpha_val=1.0f-(time-len)/3000.0f;
	}
	else if(time<len-2000) {
		alpha_val=time/(len-2000);
	}
	else {
		alpha_val=1.0f;
	}
	glColor4f(1.0f,1.0f,1.0f,alpha_val);
	DrawQuad(tex,x0,y0,x1,y1,0.0f,0.0f,1.0f,1.0f,1,GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,0);
}


ESierpinski::ESierpinski(float size,int depth,Texture* tx) {
 siz=size;
 dep=depth;
 tex=tx;
}

static void recursive_sierp(float x1,float y1,float z1,
                            float x2,float y2,float z2,
                            float x3,float y3,float z3,
                            float x4,float y4,float z4,
                            int depth) {
 if(depth>1) {
  recursive_sierp(x4-(x4-x1)*0.5f,y4-(y4-y1)*0.5f,z4-(z4-z1)*0.5f,
                  x4-(x4-x2)*0.5f,y4-(y4-y2)*0.5f,z4-(z4-z2)*0.5f,
                  x4-(x4-x3)*0.5f,y4-(y4-y3)*0.5f,z4-(z4-z3)*0.5f,
                  x4,y4,z4,
                  depth-1);
  recursive_sierp(x1,y1,z1,
                  x1-(x1-x2)*0.5f,y1-(y1-y2)*0.5f,z1-(z1-z2)*0.5f,
                  x1-(x1-x3)*0.5f,y1-(y1-y3)*0.5f,z1-(z1-z3)*0.5f,
                  x1-(x1-x4)*0.5f,y1-(y1-y4)*0.5f,z1-(z1-z4)*0.5f,
                  depth-1);
  recursive_sierp(x2-(x2-x1)*0.5f,y2-(y2-y1)*0.5f,z2-(z2-z1)*0.5f,
                  x2,y2,z2,
                  x2-(x2-x3)*0.5f,y2-(y2-y3)*0.5f,z2-(z2-z3)*0.5f,
                  x2-(x2-x4)*0.5f,y2-(y2-y4)*0.5f,z2-(z2-z4)*0.5f,
                  depth-1);
  recursive_sierp(x3-(x3-x1)*0.5f,y3-(y3-y1)*0.5f,z3-(z3-z1)*0.5f,
                  x3-(x3-x2)*0.5f,y3-(y3-y2)*0.5f,z3-(z3-z2)*0.5f,
                  x3,y3,z3,
                  x3-(x3-x4)*0.5f,y3-(y3-y4)*0.5f,z3-(z3-z4)*0.5f,
                  depth-1);
 }
 else {
  glVertex3f(x1,y1,z1); glVertex3f(x2,y2,z2); glVertex3f(x3,y3,z3);
  glVertex3f(x4,y4,z4); glVertex3f(x2,y2,z2); glVertex3f(x1,y1,z1);
  glVertex3f(x4,y4,z4); glVertex3f(x3,y3,z3); glVertex3f(x2,y2,z2);
  glVertex3f(x4,y4,z4); glVertex3f(x1,y1,z1); glVertex3f(x3,y3,z3);
 }

}

void ESierpinski::render(int time) {
  float fogcolor[4]={1.0f,0.0f,0.0f,1.0f };
  glFogi(GL_FOG_MODE,GL_LINEAR);
  glFogfv(GL_FOG_COLOR,fogcolor);
  glFogf(GL_FOG_START,0.0f);
  glFogf(GL_FOG_END,0.8f);
  c_state->want_enabled(GL_FOG);
  c_state->want_disabled(GL_LIGHTING);
  c_state->want_enabled(GL_TEXTURE_2D);
  c_state->want_disabled(GL_BLEND);
  c_state->want_enabled(GL_DEPTH_TEST);
  glBegin(GL_QUADS);
	 glVertex3f(-2.5f,1.5f,-1.0f);
	 glVertex3f(-2.5f,-1.5f,-1.0f);
	 glVertex3f(2.5f,-1.5f,-1.0f);
	 glVertex3f(2.5f,1.5f,-1.0f);
  glEnd();

  glPushMatrix();
  glRotatef(0.03*time,0.0,1.0,0.0);
  glRotatef(0.1*time,1.0,0.0,0.0);
  glRotatef(0.03*time,0.0,0.0,1.0);

  glTexGenf(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
  glTexGenf(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
  c_state->want_enabled(GL_TEXTURE_GEN_S);
  c_state->want_enabled(GL_TEXTURE_GEN_T);
  if(tex==NULL) c_state->want_disabled(GL_TEXTURE_2D);
  else tex->activate();
  glBegin(GL_TRIANGLES);
  recursive_sierp(-0.5f*siz,-0.28868f*siz,-0.2f*siz,        //-0.3876f*siz,
                  0.5f*siz,-0.28868f*siz,-0.2f*siz,
                  0.0f*siz,0.57735f*siz,-0.2f*siz,
                  0.0f*siz,0.0f*siz,0.6f*siz,         //0.429f*siz,
                  dep);
  glEnd();
  glPopMatrix();
}


void ESceneRenderer::render(int time) {
	scene->render(time);
}

ESceneRenderer::ESceneRenderer(const char* filename,
							   SceneRenderFX *tmp_rfx,float size_factor) {
	scene=new Scene3D(filename,size_factor);
	scene->set_render_fx(tmp_rfx);
}

ESceneRenderer::~ESceneRenderer() {
	delete scene;
}
