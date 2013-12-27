/***************************************************************************
                          this_demo.cpp  -  description
                             -------------------
    begin                : Fri May 5 2000
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/

#include"optical.h"
#include"../effectbase.h"
#include "../mekka_demo_effects.h"

#include <windows.h>

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float

extern "C++" HWND win_handle;
extern "C++" HDC hDC;

int enable_flare_fx=0;

DSampleDemo::DSampleDemo() {

	Texture *loading_tex;
	TexLoadDesc td;
	td.filename="textures/Loading.jpg";
	loading_tex=new Texture(td);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	DrawQuad(loading_tex);
	glFinish();
	SwapBuffers(hDC);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	DrawQuad(loading_tex);
	glFinish();
	delete loading_tex;
	SwapBuffers(hDC);

	DMChannel	*channel_3d=new DMChannel();
	DMChannel	*channel_2d=new DMChannel();
	DMChannel	*extra_channel2d=new DMChannel();
	DMScene		*tmp_scene=new DMScene();

	DMEffect *scenka1=new Mekka_Scenka1();
	channel_3d->add_effect(scenka1,61700);
	channel_3d->add_effect(new Mekka_Scene_BlurOut(scenka1,61700,2000),2000);
	DMEffect *matrix_letters=new ESceneRenderer("scenka4c.lws");
	channel_3d->add_effect(matrix_letters,15500);
	channel_3d->add_effect(new Mekka_SceneGlow(),15000);
//	channel_3d->add_effect(new EEmptyEffect(),15000);
	DMEffect *blobs_effect=new Mekka_BlobsEffect1();
	channel_3d->add_effect(blobs_effect,16000);
	DMEffect *scene5=new ESceneRenderer("scenka_10.lws");
	channel_3d->add_effect(scene5,16000);
	channel_3d->add_effect(new Mekka_2DEffect1(),24000);
	DMEffect *srubki=new ESceneRenderer("scenka_ba.lws",NULL,1.2f);
	channel_3d->add_effect(new Mekka_SceneShift(srubki,2000,1500,0.05f),28000);
	channel_3d->add_effect(new EEmptyEffect(),3000);
	DMEffect *tmp_ef=new ESceneRenderer("scenka_9.lws",NULL,2.5f);
	channel_3d->add_effect(tmp_ef,5000);
	channel_3d->add_effect(new Mekka_AudioHrzBlur(tmp_ef,5000),21000);
	Mekka_MultiSceneBlur *multi_blur=new Mekka_MultiSceneBlur();
	multi_blur->add_effect(tmp_ef,20000,-2000,10000);
	multi_blur->add_effect(srubki,16000,13000,5000);
	multi_blur->add_effect(scene5,12000,6000,6000);
	multi_blur->add_effect(matrix_letters,12100,11000,6000);
	channel_3d->add_effect(multi_blur,27000);
	channel_3d->add_effect(new Mekka_FinalScene(),14000);
	channel_3d->add_effect(new EEmptyEffect(),1000);

	channel_2d->add_effect(new EFadeIn(1000),1000);
	channel_2d->add_effect(new EEmptyEffect(),8200);
	channel_2d->add_effect(new EAdditiveImgFlash("clip1.jpg",1000,0.0f,0.0f,0.5f,0.5f),1000);
	channel_2d->add_effect(new EEmptyEffect(),3000);
	channel_2d->add_effect(new EAdditiveImgFlash("clip2.jpg",1000,0.5f,0.0f,1.0f,0.5f),1000);
	channel_2d->add_effect(new EEmptyEffect(),3000);
	channel_2d->add_effect(new EAdditiveImgFlash("clip3.jpg",1000,0.5f,0.5f,1.0f,1.0f),1000);
	channel_2d->add_effect(new EEmptyEffect(),2000);
	channel_2d->add_effect(new EAdditiveImgFlash("clip4.jpg",1000,0.0f,0.5f,0.5f,1.0f),1000);
	channel_2d->add_effect(new EEmptyEffect(),22000);
	channel_2d->add_effect(new EAdditiveImgFlash("clip5.jpg",1000,0.0f,0.0f,0.5f,0.5f),1000);
	channel_2d->add_effect(new EEmptyEffect(),4000);
	channel_2d->add_effect(new EAdditiveImgFlash("clip6.jpg",1000,0.5f,0.0f,1.0f,0.5f),1000);
	channel_2d->add_effect(new EEmptyEffect(),4000);
	channel_2d->add_effect(new EAdditiveImgFlash("clip7.jpg",1000,0.5f,0.5f,1.0f,1.0f),1000);
	channel_2d->add_effect(new EEmptyEffect(),70800);
	channel_2d->add_effect(new EColorFlash(2500),2500);
	channel_2d->add_effect(new EEmptyEffect(),39000);
	channel_2d->add_effect(new EColorFlash(2500),2500);
	channel_2d->add_effect(new EEmptyEffect(),500);
	channel_2d->add_effect(new EColorFlash(2500),2500);
	channel_2d->add_effect(new EEmptyEffect(),2000);
	channel_2d->add_effect(new EColorFlash(2500),2500);
	channel_2d->add_effect(new EEmptyEffect(),500);
	channel_2d->add_effect(new EColorFlash(2500),2500);
	channel_2d->add_effect(new EEmptyEffect(),14000);
	channel_2d->add_effect(new Mekka_AudioImgFlash("beat0.jpg",3500),3500);
	channel_2d->add_effect(new Mekka_AudioImgFlash("beat1.jpg",3500),3500);
	channel_2d->add_effect(new Mekka_AudioImgFlash("beat2.jpg",3500),3500);
	channel_2d->add_effect(new Mekka_AudioImgFlash("beat3.jpg",3500),3500);
	channel_2d->add_effect(new EEmptyEffect(),39500);
	channel_2d->add_effect(new EImgFadeIn("SXB_addict_logo1.jpg",3000),2000);

	extra_channel2d->add_effect(new EEmptyEffect(),67200);
	extra_channel2d->add_effect(new Mekka_SineImgFlash("babe.jpg",7000,0.0f,1.1f),7000);
	extra_channel2d->add_effect(new EEmptyEffect(),22000);
	extra_channel2d->add_effect(new Mekka_SineImgFlash("SXB_for_bloby_512.jpg",4000,-0.2f,1.0f),14000);
	extra_channel2d->add_effect(new EEmptyEffect(),68000);
	extra_channel2d->add_effect(new Mekka_SineDistort2("SXB_Optical_credits.jpg"),11000);

	
	tmp_scene->add_channel(channel_3d);
	tmp_scene->add_channel(extra_channel2d);
	tmp_scene->add_channel(channel_2d);

	add_effect(tmp_scene,600000);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

DSampleDemo::~DSampleDemo() {
}
