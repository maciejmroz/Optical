/***************************************************************************
                          effect.h  -  description
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/


#ifndef _DM_EFFECT_H_
#define _DM_EFFECT_H_


//klasa bazowa dla wszystkich efektow
class DMEffect {					
public:
	virtual void render(int time)=0;
	virtual ~DMEffect() {};
};

//klasa pozwalajaca na liniowe laczenie efektow

class DMChannel: public DMEffect {
	DMEffect** effects;
	int* effect_lengths;
	int num_effects;
	int overall_length;
public:
	DMChannel();
	~DMChannel();
	void add_effect(DMEffect* ef,int len);
	virtual void render(int time);
};

//klasa pozwalajaca na reownolegle (warstwowe) laczenie efektow
class DMScene: public DMEffect {
	DMEffect** channels;
	int num_channels;
	int length;
public:
	DMScene();
	~DMScene();
	void add_channel(DMEffect* chn);
	virtual void render(int time);
};

class DMDemo:public DMChannel {
	int num_frames,last;
public:
	DMDemo();
	~DMDemo();
	virtual void render(int time);
};

#endif