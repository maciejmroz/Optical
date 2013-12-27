/****************************************************************************************
 motion.cpp - motion&keyframing
 based on Newtek motion code
****************************************************************************************/

#include "engine.h"
#include "debug_msg.h"

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float

void KeyFrame::operator=(const KeyFrame& key) {
	int i;
	for(i=0;i<NUM_CHAN;i++) cv[i]=key.cv[i];
	tens=key.tens;
	cont=key.cont;
	bias=key.bias;
	linear=key.linear;
	step=key.step;
}

/*
 * Compute Hermite spline coeficients for t, where 0 <= t <= 1.
 */
static void Hermite(double t,double *h1,double *h2,double *h3,double *h4) {
	double			 t2, t3, z;

	t2 = t * t;
	t3 = t * t2;
	z = 3.0 * t2 - t3 - t3;

	*h1 = 1.0 - z;
	*h2 = z;
	*h3 = t3 - t2 - t2 + t;
	*h4 = t3 - t2;
}

Motion::Motion() {
	keylist=NULL;
	num_allocated_frames=0;
	steps=0;
	keys=0;
	int i;
	for(i=0;i<NUM_CHAN;i++) cv[i]=0.0;
}

Motion::~Motion() {
	if(keylist!=NULL) delete keylist;
	keylist=NULL;
}

void Motion::alloc_keyframe_mem(int numframes) {
	keylist=new KeyFrame[numframes];
	num_allocated_frames=numframes;
}

void Motion::add_key_frame(ChanVec c_vec,double tens,double cont,double bias,
						   int linear,int step) {
	if(num_allocated_frames==keys) {
		return;
	}
	int i;
	for(i=0;i<NUM_CHAN;i++) {
		keylist[keys].cv[i]=c_vec[i];
	}
	keylist[keys].tens=tens;
	keylist[keys].cont=cont;
	keylist[keys].bias=bias;
	keylist[keys].linear=linear;
	keylist[keys].step=step;
	steps=step;
	keys++;
}

void Motion::set_end_behavior(int eb) {
	end_behavior=eb;
}

/*
 * Compute the motion channel vector for the given step.  Step can be
 * fractional but values correspond to frames.
 */
void Motion::calc(double step)
{
	int			key0,key1;
	double		t, h1, h2, h3, h4, res, d10;
	double		dd0a, dd0b, ds1a, ds1b;
	double		adj0, adj1, dd0, ds1;
	int			i, tlength;

	if(keys==0) return;
	/*
	 * If there is but one key, the values are constant.
	 */
	if(keys == 1) {
        	for (i = 0; i < 9; i++)
			cv[i] = keylist[0].cv[i];
		return;
	}

	switch(end_behavior) {
	case 0:  //reset (=stop now)
		if(step>steps) step=(double)steps-0.0001;  
	break;
	case 1:  //stop
		if(step>steps) step=(double)steps-0.0001;  
	break;
	case 2:  //repeat
		while(step>steps) step-=steps;
	break;
	}

	/*
	 * Get keyframe pair to evaluate.  This should be within the range
	 * of the motion or this will raise an illegal access.
	 */
	key0 = 0;
	while (step > keylist[key0+1].step)
		key0++;
	key1 = (key0+1<keys) ? key0+1 : key0;
	step -= keylist[key0].step;

	/*
	 * Get tween length and fractional tween position.
	 */
	tlength = keylist[key1].step - keylist[key0].step;
	t = step / tlength;
	/*
	 * Precompute spline coefficients.
	 */
	if (!keylist[key1].linear) {
		Hermite (t, &h1, &h2, &h3, &h4);
		dd0a = (1.0 - keylist[key0].tens) * (1.0 + keylist[key0].cont)
			 * (1.0 + keylist[key0].bias);
		dd0b = (1.0 - keylist[key0].tens) * (1.0 - keylist[key0].cont)
			 * (1.0 - keylist[key0].bias);
		ds1a = (1.0 - keylist[key1].tens) * (1.0 - keylist[key1].cont)
			 * (1.0 + keylist[key1].bias);
		ds1b = (1.0 - keylist[key1].tens) * (1.0 + keylist[key1].cont)
			 * (1.0 - keylist[key1].bias);

		if (keylist[key0].step != 0)
			adj0 = tlength / (double)(keylist[key1].step - keylist[((key0>0) ? key0-1 : 0)].step);
		if (keylist[key1].step != steps)
			adj1 = tlength / (double)(keylist[((key1+1<keys) ? key1+1 : key1)].step - keylist[key0].step);
	}

	/*
	 * Compute the channel components.
	 */
	for (i = 0; i < NUM_CHAN; i++) {
		d10 = keylist[key1].cv[i] - keylist[key0].cv[i];

		if (!keylist[key1].linear) {
			if (keylist[key0].step == 0)
				dd0 = .5 * (dd0a + dd0b) * d10;
			else
				dd0 = adj0 * (dd0a
					* (keylist[key0].cv[i] - keylist[((key0>0) ? key0-1 : 0)].cv[i])
					+ dd0b * d10);

			if (keylist[key1].step == steps)
				ds1 = .5 * (ds1a + ds1b) * d10;
			else
				ds1 = adj1 * (ds1a * d10 + ds1b
					* (keylist[((key1+1<keys) ? key1+1 : key1)].cv[i] - keylist[key1].cv[i]));

			res = keylist[key0].cv[i] * h1 + keylist[key1].cv[i] * h2
				+ dd0 * h3 + ds1 * h4;
		} else
			res = keylist[key0].cv[i] + t * d10;

		cv[i] = res;
	}
}

Motion& Motion::operator=(const Motion &mot) {
	if(this!=&mot) {
		clear();
		keys=mot.keys;
		steps=mot.steps;
		end_behavior=mot.end_behavior;
		int i;
		for(i=0;i<NUM_CHAN;i++) {
			cv[i]=mot.cv[i];
		}
		alloc_keyframe_mem(mot.num_allocated_frames);
		for(i=0;i<keys;i++) {
			keylist[i]=mot.keylist[i];
		}
	}
	return *this;
}

Motion::Motion(const Motion &mot) {
	int i;
	for(i=0;i<NUM_CHAN;i++) cv[i]=mot.cv[i];
	end_behavior=mot.end_behavior;
	steps=mot.steps;
	keys=mot.keys;
	alloc_keyframe_mem(mot.num_allocated_frames);
	num_allocated_frames=mot.num_allocated_frames;
	for(i=0;i<keys;i++) {
		keylist[i]=mot.keylist[i];
	}
}

Motion::Motion(const char *file) {
	char str_buf[25];
	int tmp,i;
	int num_keys;
	int linear,step;
	double tens,cont,bias;
	ChanVec t_cv;
	int f;
	vfs->openFile(file,f);
	

	keylist=NULL;
	num_allocated_frames=0;
	steps=0;
	keys=0;
	for(i=0;i<NUM_CHAN;i++) cv[i]=0.0;

	vfs->scanString(f,str_buf);  //LWMO
	vfs->scanInt(f,&num_keys);  //eat 1
	vfs->scanInt(f,&num_keys);  //eat 9 (number of channels
	vfs->scanInt(f,&num_keys);
	alloc_keyframe_mem(num_keys);
	for(tmp=0;tmp<num_keys;tmp++) {
		for(i=0;i<NUM_CHAN;i++) vfs->scanFloat(f,&t_cv[i]);
		vfs->scanInt(f,&step);
		vfs->scanInt(f,&linear);
		vfs->scanDouble(f,&tens);
		vfs->scanDouble(f,&cont);
		vfs->scanDouble(f,&bias);
		t_cv[2]=-t_cv[2];   //negate z coord
		add_key_frame(t_cv,tens,cont,bias,linear,step);
	}
	set_end_behavior(2);  //assume repeat
	vfs->closeFile(f);
}

void Motion::clear() {
	if(keylist!=NULL) {
		delete keylist;
	}
	keylist=NULL;
	num_allocated_frames=0;
	steps=0;
	keys=0;
	int i;
	for(i=0;i<NUM_CHAN;i++) {
		cv[i]=0.0f;
	}
}

void EnvFrame::operator=(const EnvFrame& efr) {
	tens=efr.tens;
	cont=efr.cont;
	bias=efr.bias;
	linear=efr.linear;
	step=efr.step;
	value=efr.value;
}

Envelope::Envelope() {
	keylist=NULL;
	num_allocated_frames=0;
	steps=0;
	keys=0;
	res=0;
}

Envelope::Envelope(float val) {
	keylist=new EnvFrame*[1];
	keylist[0]=new EnvFrame;
	num_allocated_frames=1;
	steps=0;
	keys=1;
	res=val;
	keylist[0]->step=0;
	keylist[0]->tens=0.0;
	keylist[0]->cont=0.0;
	keylist[0]->bias=0.0;
	keylist[0]->linear=0;
	keylist[0]->value=val;
}

void Envelope::operator=(const Envelope &env) {
	clear();
	keys=env.keys;
	steps=env.steps;
	end_behavior=env.end_behavior;
	res=env.res;
	alloc_keyframe_mem(env.num_allocated_frames);
	int i;
	for(i=0;i<keys;i++) {
		*(keylist[i])=*(env.keylist[i]);
	}
}

Envelope::~Envelope() {
	if(keylist!=NULL) {
		if(num_allocated_frames) {
			int i;
			for(i=0;i<num_allocated_frames;i++) delete keylist[i];
		}
		delete keylist;
	}
}

void Envelope::clear() {
	if(keylist!=NULL) {
		if(num_allocated_frames) {
			int i;
			for(i=0;i<num_allocated_frames;i++) delete keylist[i];
		}
		delete keylist;
	}
	keylist=NULL;
	num_allocated_frames=0;
	steps=0;
	keys=0;
	res=0;
}

void Envelope::dump() {
	DebugMessage("Envelope content dump:\n");
	int i;
	DebugMessage("numkeys %d\n",keys);
	DebugMessage("num_allocated_frames %d\n",num_allocated_frames);
	DebugMessage("end_behavior %d\n",end_behavior);
	DebugMessage("steps %d\n",steps);
	for(i=0;i<keys;i++) {
		DebugMessage("value frame linear tens cont bias %f %d %d %f %f %f\n",
			keylist[i]->value,keylist[i]->step,keylist[i]->linear,
			keylist[i]->tens,keylist[i]->cont,keylist[i]->bias);
	}
}

void Envelope::alloc_keyframe_mem(int numframes) {
	keylist=new EnvFrame*[numframes];
	int i;
	for(i=0;i<numframes;i++) keylist[i]=new EnvFrame;
	num_allocated_frames=numframes;
}

void Envelope::add_key_frame(float value,double tens,double cont,double bias,
						   int linear,int step) {
	if(num_allocated_frames==keys) {
		return;
	}
	keylist[keys]->value=value;
	keylist[keys]->tens=tens;
	keylist[keys]->cont=cont;
	keylist[keys]->bias=bias;
	keylist[keys]->linear=linear;
	keylist[keys]->step=step;
	steps=step;
	keys++;
}

void Envelope::set_end_behavior(int eb) {
	end_behavior=eb;
}

/*
 * Compute the envelope value for the given step.  Step can be
 * fractional but values correspond to frames.
 */
void Envelope::calc(double step)
{
	int			key0,key1;
	double		t, h1, h2, h3, h4, d10;
	double		dd0a, dd0b, ds1a, ds1b;
	double		adj0, adj1, dd0, ds1;
	int			tlength;

	/*
	 * If there is but one key, the values are constant.
	 */
	if(keys == 1) {
		res= keylist[0]->value;
		return;
	}

	switch(end_behavior) {
	case 0:  //reset (=stop now)
		if(step>steps) step=(double)steps-0.0001;  
	break;
	case 1:  //stop
		if(step>steps) step=(double)steps-0.0001;
	break;
	case 2:  //repeat
		while(step>steps) step-=steps;
	break;
	}
	/*
	 * Get keyframe pair to evaluate.  This should be within the range
	 * of the motion or this will raise an illegal access.
	 */
	key0 = 0;
	while (step > keylist[key0+1]->step)
		key0++;
	key1 = (key0+1<keys) ? key0+1 : key0;
	step -= keylist[key0]->step;

	/*
	 * Get tween length and fractional tween position.
	 */
	tlength = keylist[key1]->step - keylist[key0]->step;
	t = step / tlength;

	/*
	 * Precompute spline coefficients->
	 */
	if (!keylist[key1]->linear) {
		Hermite (t, &h1, &h2, &h3, &h4);
		dd0a = (1.0 - keylist[key0]->tens) * (1.0 + keylist[key0]->cont)
			 * (1.0 + keylist[key0]->bias);
		dd0b = (1.0 - keylist[key0]->tens) * (1.0 - keylist[key0]->cont)
			 * (1.0 - keylist[key0]->bias);
		ds1a = (1.0 - keylist[key1]->tens) * (1.0 - keylist[key1]->cont)
			 * (1.0 + keylist[key1]->bias);
		ds1b = (1.0 - keylist[key1]->tens) * (1.0 + keylist[key1]->cont)
			 * (1.0 - keylist[key1]->bias);

		if (keylist[key0]->step != 0)
			adj0 = tlength / (double)(keylist[key1]->step - keylist[((key0>0) ? key0-1 : 0)]->step);
		if (keylist[key1]->step != steps)
			adj1 = tlength / (double)(keylist[((key1+1<keys) ? key1+1 : key1)]->step - keylist[key0]->step);
	}
	/*
	 * Compute the channel components->
	 */
		d10 = keylist[key1]->value - keylist[key0]->value;

		if (!keylist[key1]->linear) {
			if (keylist[key0]->step == 0)
				dd0 = .5 * (dd0a + dd0b) * d10;
			else
				dd0 = adj0 * (dd0a
					* (keylist[key0]->value - keylist[((key0>0) ? key0-1 : 0)]->value)
					+ dd0b * d10);

			if (keylist[key1]->step == steps)
				ds1 = .5 * (ds1a + ds1b) * d10;
			else
				ds1 = adj1 * (ds1a * d10 + ds1b
					* (keylist[((key1+1<keys) ? key1+1 : key1)]->value - keylist[key1]->value));

			res = keylist[key0]->value * h1 + keylist[key1]->value * h2
				+ dd0 * h3 + ds1 * h4;
		} else
			res = keylist[key0]->value + t * d10;

}