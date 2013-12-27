
#include"engine.h"

void VertexShader::attach_vshader(VertexShader *vshader) {
	if(next) next->attach_vshader(vshader);
	else next=vshader;
}

VertexShader::VertexShader(VertexBuffer *in_buf,VertexBuffer *out_buf) {
	in=in_buf;
	out=out_buf;
	next=NULL;
}

MorphShader::MorphShader(VertexBuffer *in_buf,VertexBuffer *out_buf,
						 Envelope* amount,VertexBuffer *morph_target):
						VertexShader(in_buf,out_buf) {
	am=*amount;
	target=morph_target;
	if(!out->vert_coords) out->vert_coords=new float[3*in->num_verts];
}

unsigned int MorphShader::vshade(unsigned int flags,float frame) {
	float *src,*dst,*tgt;
	src=in->vert_coords;
	dst=out->vert_coords;
	tgt=target->vert_coords;
	if(flags & VS_VERTICES) src=out->vert_coords; //read from out
	int tgt_size=3*in->num_verts;
	if(in->num_verts==target->num_verts) {
		am.calc(frame);
		int i;
		for(i=0;i<tgt_size;i++) {
			dst[i]=src[i]+(tgt[i]-src[i])*am.res;
		}
	}
	if(next) return next->vshade(VS_VERTICES | flags,frame);
	else return VS_VERTICES | flags;
}

Skeleton::Skeleton(VertexBuffer *in_buf,VertexBuffer *out_buf,
				   VarTable<Bone*,1> *bone_set):
				   VertexShader(in_buf,out_buf) {
	int i;
	//TODO: clear bone_tbl otherwise hard to track errors may occur in future
	for(i=0;i<bone_set->length;i++) {
		bone_tbl.add_element((*bone_set)[i]);
		bone_tbl[i]->weights=new float[in_buf->num_verts];
	}
	vertex_accum=new float[3*in_buf->num_verts];
	//make weight precalcs

}

Skeleton::~Skeleton() {
	int i;
	for(i=0;i<bone_tbl.length;i++) {
		if(bone_tbl[i]->weights) delete bone_tbl[i]->weights;
		delete bone_tbl[i];
	}
	delete vertex_accum;
}

unsigned int Skeleton::vshade(unsigned int flags,float frame) {
	//core of the skeletal deformation shader
	return 0;
}