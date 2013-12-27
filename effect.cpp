/***************************************************************************
                          effect.cpp
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/


#include"demosystem.h"
#include<stdio.h>
#include<stdlib.h>
#include "debug_msg.h"

int avg_fps=0;

DMChannel::DMChannel() {
	num_effects=0;
	overall_length=0;
	effects=NULL;
	effect_lengths=NULL;
}

DMChannel::~DMChannel() {
	if(effects != NULL) {
		int i;
		for(i=0;i<num_effects;i++) {
			delete effects[i];
		}
		delete effects;
	}
	if(effect_lengths != NULL) delete effect_lengths;
	num_effects=0;
	overall_length=0;
	effects=NULL;
	effect_lengths=NULL;
}

void DMChannel::add_effect(DMEffect* ef,int len) {
	if(num_effects==0) {
		effects=new DMEffect*[1];
		effect_lengths=new int[1];
	}
	else {
		DMEffect** temp_ef=new DMEffect*[num_effects+1];
		int*		temp_lens=new int[num_effects+1];
		int			temp_i;
		for(temp_i=0;temp_i<num_effects;temp_i++) {
			temp_ef[temp_i]=effects[temp_i];
			temp_lens[temp_i]=effect_lengths[temp_i];
		}
		delete effects;
		delete effect_lengths;
		effects=temp_ef;
		effect_lengths=temp_lens;
	}
	effects[num_effects]=ef;
	effect_lengths[num_effects]=len;
	num_effects++;
	overall_length+=len;
}

void DMChannel::render(int time) {
	if(time < 0) return;
	if(num_effects==0) return;
	int offset=time;
	int i=0;
	while((offset>effect_lengths[i]) && (i<(num_effects-1))) {
		offset-=effect_lengths[i];
		i++;
	}
	effects[i]->render(offset);
}

DMScene::DMScene() {
	num_channels=0;
	channels=NULL;
}

DMScene::~DMScene() {
	if(channels!=NULL) {
		int i;
		for(i=0;i<num_channels;i++) {
			delete channels[i];
		}
		delete channels;
	}
}

void DMScene::add_channel(DMEffect* chn) {
	if(num_channels==0) {
		channels=new DMEffect*[1];
	}
	else {
		DMEffect** temp_chn=new DMEffect*[num_channels+1];
		int			temp_i;
		for(temp_i=0;temp_i<num_channels;temp_i++) {
			temp_chn[temp_i]=channels[temp_i];
		}
		delete channels;
		channels=temp_chn;
	}
	channels[num_channels]=chn;
	num_channels++;
}

void DMScene::render(int time) {
	int i;
	for(i=0;i<num_channels;i++) channels[i]->render(time);
}

void DMDemo::render(int time) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	DMChannel::render(time);
	num_frames++;
	last=time;
	if(hasVertexArrayRange) glFlushVertexArrayRangeNV();
	else glFlush();
}

DMDemo::DMDemo() {
	num_frames=0;
	last=0;
}

extern int dlg_show_fps;

DMDemo::~DMDemo() {
//	avg_fps=1000*num_frames/last;
	if(dlg_show_fps) PrintfBox("Average fps: %d\n",1000*num_frames/last);
#ifdef _LOG_EVERYTHING
	DebugFileMessage("fps.log","%d frames generated in %d ms - %d average FPS\n",
		num_frames,last,1000*num_frames/last);
#endif
}
