/********************
 particle system testing effect
 *******************/

#include "effect.h"
#include "particles.h"
#include "texture.h"
#include "engine.h"

class EParticle1:public DMEffect {
	ParticleSystem *p_system1,*p_system2,*p_system3;
	Scene3D *scn;
	Texture *tex;
public:
	EParticle1();
	~EParticle1();
	void render(int time);
};