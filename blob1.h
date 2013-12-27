/********************
 blob testing effect
 *******************/

#include "effect.h"
#include "field.h"
#include "texture.h"

class EBlob1:public DMEffect {
	ScField* blob;
	Texture* tex,*tex2;
public:
	EBlob1();
	~EBlob1();
	void render(int time);
};