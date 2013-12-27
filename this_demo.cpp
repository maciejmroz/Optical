/***************************************************************************
                          this_demo.cpp  -  description
                             -------------------
    begin                : Fri May 5 2000
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/

#include"this_demo.h"
#include"effectbase.h"
//#include"blob1.h"
//#include"particle_effect1.h"
//#include "lh_tunnel.h"
//#include "lh_blobs.h"
//#include "lh_height_field.h"
//#include "lh_volfog.h"
#include "mekka_demo_effects.h"
#include "experimental_effects.h"

#include <windows.h>

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float

extern "C++" HWND win_handle;
extern "C++" HDC hDC;

int enable_flare_fx=0;

DSampleDemo::DSampleDemo() {
	DMEffect *scene3d;
	char filename[256];
	filename[0]='\0';
	OPENFILENAME openfilename;
	openfilename.lStructSize=sizeof(OPENFILENAME);
	openfilename.hInstance=NULL;
	openfilename.hwndOwner=NULL;
	openfilename.lpstrFilter="Lightwave 3D Scene files\0*.lws\0";
	openfilename.lpstrCustomFilter=NULL;
	openfilename.lpstrFile=filename;
	openfilename.nFilterIndex=0;
	openfilename.nMaxFile=256;
	openfilename.lpstrFileTitle=NULL;
	openfilename.lpstrInitialDir=NULL;
	openfilename.lpstrTitle=NULL;
	openfilename.Flags= OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_LONGNAMES;
	openfilename.lpstrDefExt=NULL;
	int result;
	result=GetOpenFileName(&openfilename);
	Texture *loading_tex;
	if(!result) {
	    PostMessage(win_handle, WM_CLOSE, 0, 0);
	}
	else {
		TexLoadDesc td;
		td.filename="textures/loading.jpg";
		loading_tex=new Texture(td);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawQuad(loading_tex);
		glFinish();
		SwapBuffers(hDC);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawQuad(loading_tex);
		glFinish();
		delete loading_tex;
		SwapBuffers(hDC);
		SceneRenderFX *tmp_rfx;
		tmp_rfx=new SceneRenderFX();
		tmp_rfx->blinding_flares=enable_flare_fx;
		scene3d=new ESceneRenderer(filename,tmp_rfx,scn_default_size);
		add_effect(scene3d,10000000);
	}
//	scene3d=new Mekka_UVDistort("textures/bloby_srodek2.jpg");
//	scene3d=new Mekka_2DEffect1();
//	scene3d=new Mekka_FinalScene();
//	scene3d=new EParticle1();
//	scene3d=new LH_TunnelEffect();
//	scene3d=new LH_BlobEffect1();
//	scene3d=new LH_HeightfieldScene();
//	scene3d=new LH_VolumeFogEffect();
//	scene3d=new Mekka_BlobsEffect1();
//	scene3d=new Mekka_BlurredTorus();
//	scene3d=new ExpSineEffectDistort();
//	add_effect(scene3d,1000000);
}

DSampleDemo::~DSampleDemo() {
}
