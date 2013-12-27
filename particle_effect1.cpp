
#include "particle_effect1.h"

EParticle1::EParticle1() {
	TexLoadDesc tdesc;
	tdesc.filename="flares/flare1.jpg";
	tdesc.min_filter=GL_NEAREST;
	tdesc.wrap_s=GL_CLAMP;
	tdesc.wrap_t=GL_CLAMP;
	tex=new Texture(tdesc);
	Motion* tmp_mot;
	scn=new Scene3D("particles.lws");
	tmp_mot=new Motion("particle1.mot");
	p_system1=new ParticleSystem(*tmp_mot,tex);
	p_system1->set_color(1.0f,0.0f,0.0f);
	p_system1->set_current_scene(scn);
	p_system1->set_emission(20.0f);
	p_system1->set_sizes(0.1f,0.3f);
	p_system1->set_times(1.0f,1.0f,10.0f);
	p_system1->set_spread(0.3f);

	tmp_mot=new Motion("particle2.mot");
	p_system2=new ParticleSystem(*tmp_mot,tex);
	p_system2->set_color(0.0f,0.0f,1.0f);
	p_system2->set_current_scene(scn);
	p_system2->set_emission(20.0f);
	p_system2->set_sizes(0.1f,0.3f);
	p_system2->set_times(1.0f,1.0f,10.0f);
	p_system2->set_spread(0.3f);

	tmp_mot=new Motion("particle3.mot");
	p_system3=new ParticleSystem(*tmp_mot,tex);
	p_system3->set_color(0.0f,1.0f,0.0f);
	p_system3->set_current_scene(scn);
	p_system3->set_emission(20.0f);
	p_system3->set_sizes(0.1f,0.3f);
	p_system3->set_times(1.0f,1.0f,10.0f);
	p_system3->set_spread(0.3f);
}

EParticle1::~EParticle1() {
	delete tex;
	delete scn;
	delete p_system1;
	delete p_system2;
	delete p_system3;
}

void EParticle1::render(int time) {
	scn->render(time);
	p_system1->update(30.0f*(float)time*0.001f);
	p_system2->update(30.0f*(float)time*0.001f);
	p_system3->update(30.0f*(float)time*0.001f);
	p_system1->render();
	p_system2->render();
	p_system3->render();
}