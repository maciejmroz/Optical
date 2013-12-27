/***************************************************************************
                          texture.cpp  -  description
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/


#include "demosystem.h"
#include<GL/gl.h>
#include<GL/glu.h>
#include<ijl.h>
#include<string.h>
#include<stdio.h>
#include <mmsystem.h> //for timing only
#include "debug_msg.h"
#include<png.h>

#pragma warning( disable : 4305 ) //konwersja const double->float 
#pragma warning( disable : 4244 ) //konwersja double->float i int->float

extern LPVFS vfs;
int compress_textures=0;

/*
TexLoadDesc - flexible, extendable and powerful way to load a texture
*/

TexLoadDesc::TexLoadDesc() {
	alpha1=alpha2=alpha3=alpha4=1.0f;
	alpha_bias=1.0f;
	alpha_file=NULL;
	alpha_filetype=TFT_AUTO;
	alpha_mode=TAM_NONE;
	filename=NULL;
	format=TF_RGB;
	img_filetype=TFT_AUTO;
	mag_filter=default_mag_filter;
	min_filter=default_min_filter;
	type=TT_IMAGE;
	wrap_s=wrap_t=GL_REPEAT;
	color_mode=TCM_DEFAULT;
	cube_map_size=0;
	cube_negative_x=NULL;
	cube_positive_x=NULL;
	cube_negative_y=NULL;
	cube_positive_y=NULL;
	cube_negative_z=NULL;
	cube_positive_z=NULL;
}

void TexLoadDesc::operator=(const TexLoadDesc& tdesc) {
	alpha1=tdesc.alpha1;
	alpha2=tdesc.alpha2;
	alpha3=tdesc.alpha3;
	alpha4=tdesc.alpha4;
	alpha_bias=tdesc.alpha_bias;
	if(tdesc.alpha_file!=NULL) {
		alpha_file=new char[strlen(tdesc.alpha_file)+1];
		strcpy(alpha_file,tdesc.alpha_file);
	}
	alpha_filetype=tdesc.alpha_filetype;
	alpha_mode=tdesc.alpha_mode;
	if(tdesc.filename!=NULL) {
		filename=new char[strlen(tdesc.filename)+1];
		strcpy(filename,tdesc.filename);
	}
	format=tdesc.format;
	img_filetype=tdesc.img_filetype;
	mag_filter=tdesc.mag_filter;
	min_filter=tdesc.min_filter;
	type=tdesc.type;
	wrap_s=tdesc.wrap_s;
	wrap_t=tdesc.wrap_t;
	color_mode=tdesc.color_mode;
	cube_map_size=tdesc.cube_map_size;
	if(tdesc.cube_positive_x) {
		cube_positive_x=new char[strlen(tdesc.cube_positive_x)+1];
		strcpy(cube_positive_x,tdesc.cube_positive_x);
	}
	if(tdesc.cube_negative_x) {
		cube_negative_x=new char[strlen(tdesc.cube_negative_x)+1];
		strcpy(cube_negative_x,tdesc.cube_negative_x);
	}

	if(tdesc.cube_positive_y) {
		cube_positive_y=new char[strlen(tdesc.cube_positive_y)+1];
		strcpy(cube_positive_y,tdesc.cube_positive_y);
	}
	if(tdesc.cube_negative_y) {
		cube_negative_y=new char[strlen(tdesc.cube_negative_y)+1];
		strcpy(cube_negative_y,tdesc.cube_negative_y);
	}

	if(tdesc.cube_positive_z) {
		cube_positive_z=new char[strlen(tdesc.cube_positive_z)+1];
		strcpy(cube_positive_z,tdesc.cube_positive_z);
	}
	if(tdesc.cube_negative_z) {
		cube_negative_z=new char[strlen(tdesc.cube_negative_z)+1];
		strcpy(cube_negative_z,tdesc.cube_negative_z);
	}

}

int TexLoadDesc::operator==(const TexLoadDesc& tdesc) {
	if(alpha1!=tdesc.alpha1) return 0;
	if(alpha2!=tdesc.alpha2) return 0;
	if(alpha3!=tdesc.alpha3) return 0;
	if(alpha4!=tdesc.alpha4) return 0;
	if(alpha_bias!=tdesc.alpha_bias) return 0;
	if(((alpha_file!=NULL) && (tdesc.alpha_file==NULL))
		|| ((alpha_file==NULL)&&(tdesc.alpha_file!=NULL))) return 0;
	if((alpha_file!=NULL) && (tdesc.alpha_file!=NULL)) {
		if(strcmp(alpha_file,tdesc.alpha_file)) return 0;
	}
	if(alpha_filetype!=tdesc.alpha_filetype) return 0;
	if(alpha_mode!=tdesc.alpha_mode) return 0;
	if(((filename!=NULL) && (tdesc.filename==NULL))
		|| ((filename==NULL)&&(tdesc.filename!=NULL))) return 0;
	if((filename!=NULL) && (tdesc.filename!=NULL)) {
		if(strcmp(filename,tdesc.filename)) return 0;
	}
	if(format!=tdesc.format) return 0;
	if(img_filetype!=tdesc.img_filetype) return 0;
	if(mag_filter!=tdesc.mag_filter) return 0;
	if(min_filter!=tdesc.min_filter) return 0;
	if(type!=tdesc.type) return 0;
	if(wrap_s!=tdesc.wrap_s) return 0;
	if(wrap_t!=tdesc.wrap_t) return 0;
	if(color_mode!=tdesc.color_mode) return 0;
	if(((cube_negative_x!=NULL) && (tdesc.cube_negative_x==NULL))
		|| ((cube_negative_x==NULL)&&(tdesc.cube_negative_x!=NULL))) return 0;
	if((tdesc.cube_negative_x!=NULL) &&
		(cube_negative_x!=NULL)) {
		if(strcmp(cube_negative_x,tdesc.cube_negative_x)) return 0;
	}
	if(((cube_positive_x!=NULL) && (tdesc.cube_positive_x==NULL))
		|| ((cube_positive_x==NULL)&&(tdesc.cube_positive_x!=NULL))) return 0;
	if((tdesc.cube_positive_x!=NULL) &&
		(cube_positive_x!=NULL)) {
		if(strcmp(cube_positive_x,tdesc.cube_positive_x)) return 0;
	}

	if(((cube_negative_y!=NULL) && (tdesc.cube_negative_y==NULL))
		|| ((cube_negative_y==NULL)&&(tdesc.cube_negative_y!=NULL))) return 0;
	if((tdesc.cube_negative_y!=NULL) &&
		(cube_negative_y!=NULL)) {
		if(strcmp(cube_negative_y,tdesc.cube_negative_y)) return 0;
	}
	if(((cube_positive_y!=NULL) && (tdesc.cube_positive_y==NULL))
		|| ((cube_positive_y==NULL)&&(tdesc.cube_positive_y!=NULL))) return 0;
	if((tdesc.cube_positive_y!=NULL) &&
		(cube_positive_y!=NULL)) {
		if(strcmp(cube_positive_y,tdesc.cube_positive_y)) return 0;
	}

	if(((cube_negative_z!=NULL) && (tdesc.cube_negative_z==NULL))
		|| ((cube_negative_z==NULL)&&(tdesc.cube_negative_z!=NULL))) return 0;
	if((tdesc.cube_negative_z!=NULL) &&
		(cube_negative_z!=NULL)) {
		if(strcmp(cube_negative_z,tdesc.cube_negative_z)) return 0;
	}
	if(((cube_positive_z!=NULL) && (tdesc.cube_positive_z==NULL))
		|| ((cube_positive_z==NULL)&&(tdesc.cube_positive_z!=NULL))) return 0;
	if((tdesc.cube_positive_z!=NULL) &&
		(cube_positive_z!=NULL)) {
		if(strcmp(cube_positive_z,tdesc.cube_positive_z)) return 0;
	}
	return 1;
}

/*
Image helper class - may also be used for raw image access
*/

Image::Image() {
	bytes_per_pixel=0;
	format=IF_RGBA8888;
	palette=NULL;
	pixel_buf=NULL;
	height=0;
	width=0;
}

void Image::convert() {
	GLubyte *new_data;
	new_data=new GLubyte[width*height*4];
	int i,j;
	switch(format) {
	case IF_LUM8:
		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				new_data[4*i*width+4*j]=pixel_buf[i*width+j];
				new_data[4*i*width+4*j+1]=pixel_buf[i*width+j];
				new_data[4*i*width+4*j+2]=pixel_buf[i*width+j];
				new_data[4*i*width+4*j+3]=255;
			}
		}
	break;
	case IF_LUM16:
		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				new_data[4*i*width+4*j]=(((unsigned char*)pixel_buf)[i*width+j]) >> 8;
				new_data[4*i*width+4*j+1]=(((unsigned char*)pixel_buf)[i*width+j]) >> 8;
				new_data[4*i*width+4*j+2]=(((unsigned char*)pixel_buf)[i*width+j]) >> 8;
				new_data[4*i*width+4*j+3]=255;
			}
		}
	break;
	case IF_PALETTIZED:
		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				new_data[4*i*width+4*j]=palette[3*pixel_buf[i*width+j]];
				new_data[4*i*width+4*j+1]=palette[3*pixel_buf[i*width+j]+1];
				new_data[4*i*width+4*j+2]=palette[3*pixel_buf[i*width+j]+2];
				new_data[4*i*width+4*j+3]=255;
			}
		}
	break;
	case IF_PALETTIZED_RGBA:
		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				new_data[4*i*width+4*j]=palette[4*pixel_buf[i*width+j]];
				new_data[4*i*width+4*j+1]=palette[4*pixel_buf[i*width+j]+1];
				new_data[4*i*width+4*j+2]=palette[4*pixel_buf[i*width+j]+2];
				new_data[4*i*width+4*j+3]=palette[4*pixel_buf[i*width+j]+3];
			}
		}
	break;
	case IF_RGB555:
		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				new_data[4*i*width+4*j]=((((unsigned char*)pixel_buf)[i*width+j]) >> 10) & 0x1f;
				new_data[4*i*width+4*j+1]=((((unsigned char*)pixel_buf)[i*width+j]) >> 5) & 0x1f;
				new_data[4*i*width+4*j+2]=(((unsigned char*)pixel_buf)[i*width+j]) & 0x1f;
				new_data[4*i*width+4*j+3]=255;
			}
		}
	break;
	case IF_RGB565:
		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				new_data[4*i*width+4*j]=((((unsigned char*)pixel_buf)[i*width+j]) >> 11) & 0x1f;
				new_data[4*i*width+4*j+1]=((((unsigned char*)pixel_buf)[i*width+j]) >> 5) & 0x3f;
				new_data[4*i*width+4*j+2]=(((unsigned char*)pixel_buf)[i*width+j]) & 0x1f;
				new_data[4*i*width+4*j+3]=255;
			}
		}
	break;
	case IF_RGB888:
		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				new_data[4*i*width+4*j]=pixel_buf[3*i*width+3*j];
				new_data[4*i*width+4*j+1]=pixel_buf[3*i*width+3*j+1];
				new_data[4*i*width+4*j+2]=pixel_buf[3*i*width+3*j+2];
				new_data[4*i*width+4*j+3]=255;
			}
		}
	break;
	case IF_BGR888:
		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				new_data[4*i*width+4*j]=pixel_buf[3*i*width+3*j+2];
				new_data[4*i*width+4*j+1]=pixel_buf[3*i*width+3*j+1];
				new_data[4*i*width+4*j+2]=pixel_buf[3*i*width+3*j];
				new_data[4*i*width+4*j+3]=255;
			}
		}
	break;
	case IF_RGBA8888: //everything is converted to this format by convert() method
	break;
	case IF_RGBA4444:
		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				new_data[4*i*width+4*j]=((((unsigned char*)pixel_buf)[i*width+j]) >> 12) & 0xf;
				new_data[4*i*width+4*j+1]=((((unsigned char*)pixel_buf)[i*width+j]) >> 8) & 0xf;
				new_data[4*i*width+4*j+2]=((((unsigned char*)pixel_buf)[i*width+j]) >> 4) & 0xf;
				new_data[4*i*width+4*j+3]=(((unsigned char*)pixel_buf)[i*width+j]) & 0xf;
			}
		}
	break;
	case IF_BGRA8888:
		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				new_data[4*i*width+4*j]=pixel_buf[4*i*width+4*j+2];
				new_data[4*i*width+4*j+1]=pixel_buf[4*i*width+4*j+1];
				new_data[4*i*width+4*j+2]=pixel_buf[4*i*width+4*j];
				new_data[4*i*width+4*j+3]=pixel_buf[4*i*width+4*j+3];
			}
		}
	break;
	case IF_ABGR8888:
		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				new_data[4*i*width+4*j]=pixel_buf[4*i*width+4*j+3];
				new_data[4*i*width+4*j+1]=pixel_buf[4*i*width+4*j+2];
				new_data[4*i*width+4*j+2]=pixel_buf[4*i*width+4*j+1];
				new_data[4*i*width+4*j+3]=pixel_buf[4*i*width+4*j];
			}
		}
	break;
	}
	delete pixel_buf;
	pixel_buf=new_data;
	format=IF_RGBA8888;
}

void Image::make_grayscale() {
	if(format==IF_GRAYSCALE) return;
	if(format!=IF_RGBA8888) convert();
	GLubyte *new_data;
	new_data=new GLubyte[width*height];
	int i,j;
	for(i=0;i<height;i++) {
		for(j=0;j<width;j++) {
			new_data[i*width+j]=(GLubyte)(((unsigned int)pixel_buf[4*i*width+4*j]+
			(unsigned int)pixel_buf[4*i*width+4*j+1]+
			(unsigned int)pixel_buf[4*i*width+4*j+2])/3);
		}
	}
	delete pixel_buf;
	pixel_buf=new_data;
	format=IF_GRAYSCALE;
}

void Image::make_normal_map() {
	if(format!=IF_RGBA8888) convert();
	resize();
	make_grayscale();
	const float oneOver255 = 1.0/255.0;
	int i, j;
	GLubyte *nmap;
	float sqlen, reciplen, nx, ny, nz, c, cx, cy, dcx, dcy;
	nmap = new GLubyte[4*width*width];
	float scale=1.0f;
	for (i=0; i<width; i++) {
		for (j=0; j<width; j++) {
			/* Expand [0,255] texel values to the [0,1] range. */
			c = pixel_buf[i*width + j] * oneOver255;
			/* Expand the texel to its right. */
			cx = pixel_buf[i*width + (j+1)%width] * oneOver255;
			/* Expand the texel one up. */
			cy = pixel_buf[((i+1)%width)*width + j] * oneOver255;
			dcx = scale * (c - cx);
			dcy = scale * (c - cy);
			/* Normalize the vector. */
			sqlen = dcx*dcx + dcy*dcy + 1;
			reciplen = 1.0/sqrt(sqlen);
			nx = dcy*reciplen;
			ny = -dcx*reciplen;
			nz = reciplen;
			/* Repack the normalized vector into an RGB unsigned byte
			vector in the normal map image. */
			nmap[4*(i*width+j)+2] = 128 + 127*nx;
			nmap[4*(i*width+j)+1] = 128 + 127*ny;
			nmap[4*(i*width+j)] = 128 + 127*nz;
			/* The highest resolution mipmap level always has a
			unit length magnitude. */
			nmap[4*(i*width+j)+3] = 255;
		}
	}
	delete pixel_buf;
	pixel_buf=nmap;
	format=IF_NORMAL_MAP;
}

void Image::downsample_normal_map() {
	const float oneOver127 = 1.0/127.0;
	const float oneOver255 = 1.0/255.0;
	GLubyte *nmap;
	float x, y, z, l, invl;
	float mag00, mag01, mag10, mag11;
	int i, j, ii, jj;
	/* Allocate space for the down-sampled normal map level. */
	int w2,h2;
	w2=h2=width;
	width/=2;
	height=width;
	nmap = new GLubyte[4*width*width];
	for (i=0; i<h2; i+=2) {
		for (j=0; j<w2; j+=2) {
		/* The "%w2" and "%h2" modulo arithmetic makes sure that
			Nx1 and 1xN normal map levels are handled correctly. */
			/* Fetch the magnitude of the four vectors to be down-sampled. */
			mag00 = oneOver255 * pixel_buf[4*((i)*w2+(j ))+3];
			mag01 = oneOver255 * pixel_buf[4*((i)*w2+((j+1)%h2))+3];
			mag10 = oneOver255 * pixel_buf[4*(((i+1)%w2)*w2+(j))+3];
			mag11 = oneOver255 * pixel_buf[4*(((i+1)%w2)*w2+((j+1)%h2))+3];
			/* Sum 2x2 footprint of red component scaled back to [-1,1] range. */
			x = mag00 * (oneOver127 * pixel_buf[4*((i)*w2+(j ))+2] - 1.0);
			x += mag01 * (oneOver127 * pixel_buf[4*((i)*w2+((j+1)%h2))+2] - 1.0);
			x += mag10 * (oneOver127 * pixel_buf[4*(((i+1)%w2)*w2+(j))+2] - 1.0);
			x += mag11 * (oneOver127 * pixel_buf[4*(((i+1)%w2)*w2+((j+1)%h2))+2] - 1.0);
			/* Sum 2x2 footprint of green component scaled back to [-1,1] range. */
			y = mag00 * (oneOver127 * pixel_buf[4*((i)*w2+(j ))+1] - 1.0);
			y += mag01 * (oneOver127 * pixel_buf[4*((i)*w2+((j+1)%h2))+1] - 1.0);
			y += mag10 * (oneOver127 * pixel_buf[4*(((i+1)%w2)*w2+(j))+1] - 1.0);
			y += mag11 * (oneOver127 * pixel_buf[4*(((i+1)%w2)*w2+((j+1)%h2))+1] - 1.0);
			/* Sum 2x2 footprint of blue component scaled back to [-1,1] range. */
			z = mag00 * (oneOver127 * pixel_buf[4*((i)*w2+(j ))] - 1.0);
			z += mag01 * (oneOver127 * pixel_buf[4*((i)*w2+((j+1)%h2))] - 1.0);
			z += mag10 * (oneOver127 * pixel_buf[4*(((i+1)%w2)*w2+(j))] - 1.0);
			z += mag11 * (oneOver127 * pixel_buf[4*(((i+1)%w2)*w2+((j+1)%h2))] - 1.0);
			/* Compute length of the (x,y,z) vector. */
			l = sqrt(x*x + y*y + z*z);
			if (l == 0.0) {
			/* Ugh, a zero length vector. Avoid division by zero and just
				use the unperturbed normal (0,0,1). */
				x = 0.0;
				y = 0.0;
				z = 1.0;
			} else {
				/* Normalize the vector to unit length. */
				invl = 1.0/l;
				x = x*invl;
				y = y*invl;
				z = z*invl;
			}
			ii = i >> 1;
			jj = j >> 1;
			/* Pack the normalized vector into an RGB unsigned byte vector
			in the down-sampled image. */
			nmap[4*(ii*width+jj)+2] = 128 + 127*x;
			nmap[4*(ii*width+jj)+1] = 128 + 127*y;
			nmap[4*(ii*width+jj)] = 128 + 127*z;
			/* Store the magnitude of the average vector in the alpha
			component so we keep track of the magnitude. */
			l = l/4.0;
			if (l > 1.0) {
				nmap[4*(ii*width+jj)+3] = 255;
			} else {
				nmap[4*(ii*width+jj)+3] = 255*l;
			}
		}
	}
	delete pixel_buf;
	pixel_buf=nmap;
}

void Image::resize(int size) {

	int i,j,k;
	int new_size;
	if(size==0) {
		new_size=1;
		while(new_size<width) new_size*=2;
		while(new_size<height) new_size*=2;
	}
	else {
		new_size=size;
	}
	if(new_size>max_texture_size) new_size=max_texture_size;
	if((new_size==width) && (new_size==height)
		&& (new_size<=max_texture_size)) return;     //CHANGE!!!!!

	//box filtering code from Brian Paul's MesaGLU

	GLfloat sx,sy;
	GLfloat *tempin,*tempout;
	if (new_size > 1) {
		sx = (GLfloat) (width-1) / (GLfloat) (new_size-1);
		sy = (GLfloat) (height-1) / (GLfloat) (new_size-1);
	}
	else {
		sx = (GLfloat) (width-1);
		sy = (GLfloat) (height-1);
	}

	tempin=new GLfloat[4*width*height];
	tempout=new GLfloat[4*new_size*new_size];
	for(i=0;i<height;i++) {
		for(j=0;j<width;j++) {
			tempin[4*i*width+4*j]=(GLfloat)pixel_buf[4*i*width+4*j];
			tempin[4*i*width+4*j+1]=(GLfloat)pixel_buf[4*i*width+4*j+1];
			tempin[4*i*width+4*j+2]=(GLfloat)pixel_buf[4*i*width+4*j+2];
			tempin[4*i*width+4*j+3]=(GLfloat)pixel_buf[4*i*width+4*j+3];
		}
	}
   if (sx<1.0 && sy<1.0) {
      // magnify both width and height:  use weighted sample of 4 pixels 
      GLint i0, i1, j0, j1;
      GLfloat alpha, beta;
      GLfloat *src00, *src01, *src10, *src11;
      GLfloat s1, s2;
      GLfloat *dst;

      for (i=0;i<new_size;i++) {
	 i0 = (GLint)(i * sy);
	 i1 = i0 + 1;
	 if (i1 >= height) i1 = height-1;
	 alpha = i*sy - i0;
	 for (j=0;j<new_size;j++) {
	    j0 = (GLint)(j * sx);
	    j1 = j0 + 1;
	    if (j1 >= width) j1 = width-1;
	    beta = j*sx - j0;

	    // compute weighted average of pixels in rect (i0,j0)-(i1,j1) 
	    src00 = tempin + (i0 * width + j0) * 4;
	    src01 = tempin + (i0 * width + j1) * 4;
	    src10 = tempin + (i1 * width + j0) * 4;
	    src11 = tempin + (i1 * width + j1) * 4;

	    dst = tempout + (i * new_size + j) * 4;

	    for (k=0;k<4;k++) {
	       s1 = *src00++ * (1.0f-beta) + *src01++ * beta;
	       s2 = *src10++ * (1.0f-beta) + *src11++ * beta;
	       *dst++ = s1 * (1.0f-alpha) + s2 * alpha;
	    }
	 }
      }
   }
   else {
      // shrink width and/or height:  use an unweighted box filter 
      GLint i0, i1;
      GLint j0, j1;
      GLint ii, jj;
      GLfloat sum, *dst;

	for (i=0;i<new_size;i++) {
		i0 = (GLint)(i * sy);
		i1 = i0 + 1;
		if (i1 >= height) i1 = height-1; 
		for (j=0;j<new_size;j++) {
			j0 = (GLint)(j * sx);
			j1 = j0 + 1;
			if (j1 >= width) j1 = width-1;

			dst = tempout + (i * new_size + j) * 4;
	
			// compute average of pixels in the rectangle (i0,j0)-(i1,j1) 
			for (k=0;k<4;k++) {
				sum = 0.0;
				for (ii=i0;ii<=i1;ii++) {
					for (jj=j0;jj<=j1;jj++) {
						sum += *(tempin + (ii * width + jj) * 4 + k);
					}
				}
				sum /= (j1-j0+1) * (i1-i0+1);
				*dst++ = sum;
			}
		}
	}
   }

	GLubyte* out;
	out=new GLubyte[4*new_size*new_size];
	for(i=0;i<new_size;i++) {
		for(j=0;j<new_size;j++) {
			out[4*i*new_size+4*j]=(GLubyte)tempout[4*i*new_size+4*j];
			out[4*i*new_size+4*j+1]=(GLubyte)tempout[4*i*new_size+4*j+1];
			out[4*i*new_size+4*j+2]=(GLubyte)tempout[4*i*new_size+4*j+2];
			out[4*i*new_size+4*j+3]=(GLubyte)tempout[4*i*new_size+4*j+3];
		}
	}
	delete pixel_buf;
	delete tempin;
	delete tempout;
	pixel_buf=out;
	width=height=new_size;
}

/*

  Texture class implementation

*/

//#define _TIME_LOAD_JPG_

Image* load_JPEG_image(const char *filename) {

#ifdef _TIME_LOAD_JPG_
	int start_time,end_time;
	start_time=timeGetTime();
#endif

	char* jfif_buffer;
	int f_handle;
	GC_ERROR err_code;
	err_code=vfs->openFileAsPtr(filename,f_handle,(unsigned char**)&jfif_buffer);
	int jpg_size=vfs->getFileSize(f_handle);

//----------------------------------------------------------
// An example using the IntelR JPEG Library:
// -- Decode a JPEG image from a JFIF file to general pixel buffer.
//----------------------------------------------------------
	BOOL bres;
	IJLERR jerr;
	DWORD x; // pixels in scan line
	x=0;
	DWORD y; // number of scan lines
	y=0;
	DWORD c; // number of channels
	c=0;
	DWORD wholeimagesize;
	BYTE* pixel_buf;
	pixel_buf=NULL;
	// Allocate the IJL JPEG_CORE_PROPERTIES structure.
	JPEG_CORE_PROPERTIES jcprops;
	bres = TRUE;
	// Initialize the IntelR JPEG Library.
	jerr = ijlInit(&jcprops);
	if(IJL_OK != jerr)
	{
	bres = FALSE;
	}
	// Get information on the JPEG image
	// (i.e., width, height, and channels).
	jcprops.JPGFile=NULL;  //new char[strlen(filename)+1];
	jcprops.JPGBytes=(unsigned char*)jfif_buffer;
	jcprops.JPGSizeBytes=jpg_size;
//	jcprops.JPGFile=const_cast<LPCSTR>(filename);
	jerr = ijlRead(&jcprops, IJL_JBUFF_READPARAMS);
	if(IJL_OK != jerr)
	{
	bres = FALSE;
	}
	// Set up local data.
	x = jcprops.JPGWidth;
	y = jcprops.JPGHeight;
	c = 3; // Decode into a 3 channel pixel buffer.
	// Compute size of desired pixel buffer.
	wholeimagesize = (x *y *c);
	// Allocate memory to hold the decompressed image data.
	pixel_buf = new BYTE [wholeimagesize];
	// Set up the info on the desired DIB properties.
	jcprops.DIBWidth = x;
	jcprops.DIBHeight = y; // Implies a bottom-up DIB.
	jcprops.DIBChannels = c;
	jcprops.DIBColor = IJL_RGB;
	jcprops.DIBPadBytes = 0;
	jcprops.DIBBytes = pixel_buf;
	// Set the JPG color space ... this will always be
	// somewhat of an educated guess at best because JPEG
	// is "color blind" (i.e., nothing in the bit stream
	// tells you what color space the data was encoded from).
	// However, in this example we assume that we are
	// reading JFIF files which means that 3 channel images
	// are in the YCbCr color space and 1 channel images are
	// in the Y color space.
	switch(jcprops.JPGChannels) {
	case 1:
	{
	jcprops.JPGColor = IJL_G;
	break;
	}
	case 3:
	{
	jcprops.JPGColor = IJL_YCBCR;
	break;
	}
	default:
	{
	// This catches everything else, but no
	// color twist will be performed by the IJL.
	jcprops.DIBColor = (IJL_COLOR)IJL_OTHER;
	jcprops.JPGColor = (IJL_COLOR)IJL_OTHER;
	break;
	}
	}
	// Now get the actual JPEG image data into the pixel buffer.
	jerr = ijlRead(&jcprops, IJL_JBUFF_READWHOLEIMAGE);
	if(IJL_OK != jerr) {
		bres = FALSE;
	}
	if(FALSE == bres) {
		if(jfif_buffer) delete jfif_buffer;
		if(NULL != pixel_buf) {
			delete [] pixel_buf;
			pixel_buf = NULL;
			return NULL;
		}
	}
	// Clean up the IntelR JPEG Library.
	ijlFree(&jcprops);
	vfs->closeFile(f_handle);

#ifdef _TIME_LOAD_JPG_
	end_time=timeGetTime();
	DebugFileMessage("timing.txt","load_JPEG_image took %d ms\n",end_time-start_time);
#endif

	Image* img=new Image();
	img->bytes_per_pixel=3;
	img->format=IF_RGB888;
	img->height=y;
	img->width=x;
	img->pixel_buf=pixel_buf;
	img->palette=NULL;
	return img;
}

Image* Texture::load_JPEG_ex(const char *filename) {
	return load_JPEG_image(filename);
}

/*
  TGA loader
  kod zrodlowy thx to NVIDIA
  niewielkie wlasne przerobki (przede wszystkim C->C++, usuniecie wszystkich
  wewnetrznych komunikatow, niewielkie modyfikacje w typach danych itd)
*/

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

struct TgaHeader {
  uint8 idLength;
  uint8 colorMapType;

  /* The image type. */
#define TGA_TYPE_MAPPED 1
#define TGA_TYPE_COLOR 2
#define TGA_TYPE_GRAY 3
#define TGA_TYPE_MAPPED_RLE 9
#define TGA_TYPE_COLOR_RLE 10
#define TGA_TYPE_GRAY_RLE 11
  uint8 imageType;

  /* Color Map Specification. */
  /* We need to separately specify high and low bytes to avoid endianness
     and alignment problems. */
  uint8 colorMapIndexLo, colorMapIndexHi;
  uint8 colorMapLengthLo, colorMapLengthHi;
  uint8 colorMapSize;

  /* Image Specification. */
  uint8 xOriginLo, xOriginHi;
  uint8 yOriginLo, yOriginHi;

  uint8 widthLo, widthHi;
  uint8 heightLo, heightHi;

  uint8 bpp;

  /* Image descriptor.
     3-0: attribute bpp
     4:   left-to-right ordering
     5:   top-to-bottom ordering
     7-6: zero
     */
#define TGA_DESC_ABITS 0x0f
#define TGA_DESC_HORIZONTAL 0x10
#define TGA_DESC_VERTICAL 0x20
  uint8 descriptor;

};

struct TgaFooter {
  uint32 extensionAreaOffset;
  uint32 developerDirectoryOffset;
#define TGA_SIGNATURE "TRUEVISION-XFILE"
  uint8 signature[16];
  uint8 dot;
  uint8 null;
};


/* Include stuff that should be in <GL/gl.h> that might not be in your version. */

/* EXT_bgra defines from <GL/gl.h> */
#ifndef GL_EXT_bgra
#define GL_BGR_EXT                          0x80E0
#define GL_BGRA_EXT                         0x80E1
#endif


static int totbytes = 0;

struct RLEstate {
  unsigned char *statebuf;
  int statelen;
  int laststate;
};

static int std_fread(RLEstate *rleInfo, void *buf, size_t datasize, size_t nelems,int fp)
{
  return vfs->fread(fp,datasize*nelems,buf);
}

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#define RLE_PACKETSIZE 0x80

/* Decode a bufferful of file. */
static int rle_fread(RLEstate *rleInfo, void *vbuf, size_t datasize, size_t nelems, int fp)
{

  unsigned char *buf = (unsigned char*)vbuf;
  int j, k;
  int buflen, count, bytes, curbytes;
  unsigned char *p;

  /* Scale the buffer length. */
  buflen = nelems * datasize;

  j = 0;
  curbytes = totbytes;
  while (j < buflen) {
    if (rleInfo->laststate < rleInfo->statelen) {
      /* Copy bytes from our previously decoded buffer. */
      bytes = MIN(buflen - j, rleInfo->statelen - rleInfo->laststate);
      memcpy(buf + j, rleInfo->statebuf + rleInfo->laststate, bytes);
      j += bytes;
      rleInfo->laststate += bytes;

      /* If we used up all of our state bytes, then reset them. */
      if (rleInfo->laststate >= rleInfo->statelen) {
        rleInfo->laststate = 0;
        rleInfo->statelen = 0;
      }

      /* If we filled the buffer, then exit the loop. */
      if (j >= buflen) break;
    }

    /* Decode the next packet. */
	unsigned char tmp_char;
    vfs->fread(fp,1,&tmp_char);
	count=tmp_char;
    if (vfs->eof(fp)) {
      return j / datasize;
    }

    /* Scale the byte length to the size of the data. */
    bytes = ((count & ~RLE_PACKETSIZE) + 1) * datasize;

    if (j + bytes <= buflen) {
      /* We can copy directly into the image buffer. */
      p = buf + j;
    } else {
      /* Allocate the state buffer if we haven't already. */
      if (!rleInfo->statebuf) {
        rleInfo->statebuf = new unsigned char[RLE_PACKETSIZE * datasize];
      }
      p = rleInfo->statebuf;
    }

    if (count & RLE_PACKETSIZE) {
      /* Fill the buffer with the next value. */
      if ((size_t)vfs->fread(fp,datasize,p) != datasize) {
        return j / datasize;
      }

      /* Optimized case for single-byte encoded data. */
      if (datasize == 1) {
        memset(p + 1, *p, bytes - 1);
      } else {
        for (k = datasize; k < bytes; k += datasize) {
          memcpy(p + k, p, datasize);
        }
      }
    } else {
      /* Read in the buffer. */
      if (vfs->fread(fp,bytes,p) != 1) {
        return j / datasize;
      }
    }


    /* We may need to copy bytes from the state buffer. */
    if (p == rleInfo->statebuf) {
      rleInfo->statelen = bytes;
    } else {
      j += bytes;
    }
  }

  return nelems;
}


/*
  real loader
*/
Image* load_TGA_image(const char* filename) {

  TgaHeader tgaHeader;
  TgaFooter tgaFooter;
  char horzrev, vertrev;
  int width, height, bpp;
  int start, end, dir;
  int i, j, k;
  int pelbytes, wbytes;
  IF_FORMAT format;
  int components;
  RLEstate rleRec;
  RLEstate *rleInfo;
  int rle;
  int index, colors, length;
  GLubyte *cmap, *pixels, *data;
  int (*myfread)(RLEstate *rleInfo, void*, size_t, size_t, int);
  Image *img;
  int fp;
  vfs->openFile(filename,fp);
  int yFlip=1;

  /* Check the footer. */
  if ((vfs->fseek(fp,vfs->getFileSize(fp)-sizeof(tgaFooter),SEEK_CUR) != GC_OK)
      || (vfs->fread(fp,sizeof(tgaFooter),&tgaFooter) != sizeof(tgaFooter))) {
	vfs->closeFile(fp);
    return NULL;
  }  

  /* Check the signature. */
  if (memcmp(tgaFooter.signature, TGA_SIGNATURE,
             sizeof(tgaFooter.signature)) == 0) {
  } else {
  }

  vfs->closeFile(fp);
  vfs->openFile(filename,fp);		//temporary hack since 
									//only SEEK_CUR is supported
									//very fast in datafile mode anyway
  if(vfs->fread(fp,sizeof(tgaHeader),&tgaHeader) != sizeof(tgaHeader)) {
	vfs->closeFile(fp);
    return NULL;
  }

  if (tgaHeader.idLength && (vfs->fseek(fp,tgaHeader.idLength,SEEK_CUR) != GC_OK)) {
	vfs->closeFile(fp);
    return NULL;
  }
  
  /* Reassemble the multi-byte values correctly, regardless of
     host endianness. */
  width = (tgaHeader.widthHi << 8) | tgaHeader.widthLo;
  height = (tgaHeader.heightHi << 8) | tgaHeader.heightLo;
  bpp = tgaHeader.bpp;
  if (width == 0 || height == 0 || bpp == 0) {
	vfs->closeFile(fp);
    return NULL;
  }

  horzrev = tgaHeader.descriptor & TGA_DESC_HORIZONTAL;
  vertrev = !(tgaHeader.descriptor & TGA_DESC_VERTICAL);
  if (yFlip) {
    vertrev = !vertrev;
  }

  rle = 0;
  switch (tgaHeader.imageType) {
  case TGA_TYPE_MAPPED_RLE:
    rle = 1;
  case TGA_TYPE_MAPPED:
    /* Test for alpha channel. */
    format = IF_PALETTIZED;
    components = 1;
    break;

  case TGA_TYPE_GRAY_RLE:
    rle = 1;
  case TGA_TYPE_GRAY:
    format = IF_LUM8;
    components = 1;
    break;

  case TGA_TYPE_COLOR_RLE:
    rle = 1;
  case TGA_TYPE_COLOR:
    /* Test for alpha channel. */
    if (bpp == 32) {
      format = IF_BGRA8888;
      components = 4;
    } else {
      format = IF_BGR888;
      components = 3;
    }
    break;

  default:
	vfs->closeFile(fp);
    return NULL;
  }

  if ((format == IF_BGRA8888 && bpp != 32) ||
      (format == IF_BGR888 && bpp != 24) ||
      ((format == IF_LUM8 || format == IF_PALETTIZED) && bpp != 8)) {
    /* FIXME: We haven't implemented bit-packed fields yet. */
	vfs->closeFile(fp);
    return NULL;
  }

  /* Check that we have a color map only when we need it. */
  if (format == IF_PALETTIZED) {
    if (tgaHeader.colorMapType != 1) {
	vfs->closeFile(fp);
      return NULL;
    }
  } else if (tgaHeader.colorMapType != 0) {
	vfs->closeFile(fp);
    return NULL;
  }

  if (tgaHeader.colorMapType == 1) {
    /* We need to read in the colormap. */
    index = (tgaHeader.colorMapIndexHi << 8) | tgaHeader.colorMapIndexLo;
    length = (tgaHeader.colorMapLengthHi << 8) | tgaHeader.colorMapLengthLo;

    if (length == 0) {
	vfs->closeFile(fp);
      return NULL;
    }
    if (tgaHeader.colorMapSize != 24) {
      /* We haven't implemented bit-packed fields yet. */
	vfs->closeFile(fp);
      return NULL;
    }

    pelbytes = tgaHeader.colorMapSize / 8;
    colors = length + index;
    cmap = new GLubyte[colors * pelbytes];

    /* Zero the entries up to the beginning of the map. */
    memset(cmap, 0, index * pelbytes);

    /* Read in the rest of the colormap. */
    if ((size_t)vfs->fread(fp,pelbytes*length,cmap) != (size_t) pelbytes*length) {
	vfs->closeFile(fp);
      return NULL;
    }

    if (pelbytes >= 3) {
      /* Rearrange the colors from BGR to RGB. */
      int tmp;
      for (j = index; j < length * pelbytes; j += pelbytes) {
        tmp = cmap[j];
        cmap[j] = cmap[j + 2];
        cmap[j + 2] = tmp;
      }
    }
  } else {
    colors = 0;
    cmap = NULL;
  }

  /* Allocate the data. */
  pelbytes = bpp / 8;
  pixels = new unsigned char[width * height * pelbytes];

  if (rle) {
    rleRec.statebuf = 0;
    rleRec.statelen = 0;
    rleRec.laststate = 0;
    rleInfo = &rleRec;
    myfread = rle_fread;
  } else {
    rleInfo = NULL;
    myfread = std_fread;
  }

  wbytes = width * pelbytes;

  if (vertrev) {
    start = 0;
    end = height;
    dir = 1;
  } else {
    /* We need to reverse loading order of rows. */
    start = height-1;
    end = -1;
    dir = -1;
  }

  for (i = start; i != end; i += dir) {
    data = pixels + i*wbytes;

    /* Suck in the data one row at a time. */
    if (myfread(rleInfo, data, pelbytes, width, fp) != width) {
      /* Probably premature end of file. */
	vfs->closeFile(fp);
      return NULL;
    }  

    if (horzrev) {
      /* We need to mirror row horizontally. */
      for (j = 0; j < width/2; j++) {
        GLubyte tmp;

        for (k = 0; k < pelbytes; k++) {
          tmp = data[j*pelbytes+k];
          data[j*pelbytes+k] = data[(width-j-1)*pelbytes+k];
          data[(width-j-1)*pelbytes+k] = tmp;
        }
      }
    }
  }

  if (rle) {
    free(rleInfo->statebuf);
  }


  img = new Image;
  img->width = width;
  img->height = height;
  img->format = format; 
  img->palette = cmap; 
  img->bytes_per_pixel=components;
  img->pixel_buf = pixels;
  return img;
}  //load_TGA_image

Image* Texture::load_TGA_ex(const char* filename) {
	return load_TGA_image(filename);
}

static int f_png_file_handle;

static void /* PRIVATE */
png_vfs_read_data(png_structp png_ptr, png_bytep data, png_size_t length) {
	vfs->fread(f_png_file_handle,length,data);
}


Image* load_PNG_image(const char* filename) {
	png_structp png_ptr;
	png_infop info_ptr;
	int bit_depth; // color_type, interlace_type;

	vfs->openFile(filename,f_png_file_handle);
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	info_ptr = png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));
	png_set_read_fn(png_ptr,NULL,png_vfs_read_data);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	png_bytep *row_pointers;
	int num_channels;
	int img_width;
	int img_height;
//	int img_byte_width;
	row_pointers = png_get_rows(png_ptr, info_ptr);
	num_channels = png_get_channels(png_ptr, info_ptr);
	img_width = png_get_image_width(png_ptr, info_ptr);
	img_height = png_get_image_height(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr,info_ptr);
//	color_type = png_get_color_type(png_ptr, info_ptr);

	Image* img=new Image();
	img->width=img_width;
	img->height=img_height;
	img->bytes_per_pixel=num_channels;
	img->pixel_buf=new unsigned char[img_width*img_height*num_channels];
	switch(num_channels) {
	case 3:
		img->format=IF_RGB888;
	break;
	case 4:
		img->format=IF_RGBA8888;
	break;
	}
	int i;
	unsigned char* copy_ptr=img->pixel_buf;
	for(i=0;i<img_height;i++) {
		memcpy(copy_ptr,row_pointers[i],img_width*num_channels);
		copy_ptr+=img_width*num_channels;
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	vfs->closeFile(f_png_file_handle);

	return img;;
}

Image* load_image(const char* filename) {
	const char* str;
	str=filename+strlen(filename)-4;
	if(*str=='.') {
		str+=1;
		if(strcmp(str,"tga")==0) {
			return load_TGA_image(filename);
		}
		if(strcmp(str,"png")==0) {
			return load_PNG_image(filename);
		}
		if(strcmp(str,"TGA")==0) {
			return load_TGA_image(filename);
		}
		if(strcmp(str,"PNG")==0) {
			return load_PNG_image(filename);
		}
		if(strcmp(str,"jpg")==0) {
			return load_JPEG_image(filename);
		}
		if(strcmp(str,"JPG")==0) {
			return load_JPEG_image(filename);
		}
	}
	return NULL;
}

Image* Texture::load_PNG_ex(const char *filename) {
	return load_PNG_image(filename);
}


void Texture::load_PNG(const char* filename) {
	//TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

void Texture::load_cube_map(TexLoadDesc& tdesc) {
	is_cube_map=1;
	Image	*pos_x,*neg_x,*pos_y,*neg_y,*pos_z,*neg_z;
	int max_size=0;
	pos_x=neg_x=pos_y=neg_y=pos_z=neg_z=NULL;
	if(!hasCubeMaps) PrintfBox("Cube mapping not supported!");
	if(!(pos_x && neg_x && pos_y && neg_y && pos_z && neg_z)) {
		PrintfBox("Incorrect parameters for cube map loader!!!");
		return;
	}
	pos_x=load_image(tdesc.cube_positive_x);
	pos_x->convert();
	pos_x->resize();
	if(pos_x->width>max_size) max_size=pos_x->width;
	neg_x=load_image(tdesc.cube_negative_x);
	neg_x->convert();
	neg_x->resize();
	if(neg_x->width>max_size) max_size=neg_x->width;
	pos_y=load_image(tdesc.cube_positive_y);
	pos_y->convert();
	pos_y->resize();
	if(pos_y->width>max_size) max_size=pos_y->width;
	neg_y=load_image(tdesc.cube_negative_y);
	neg_y->convert();
	neg_y->resize();
	if(neg_y->width>max_size) max_size=neg_y->width;
	pos_z=load_image(tdesc.cube_positive_z);
	pos_z->convert();
	pos_z->resize();
	if(pos_z->width>max_size) max_size=pos_z->width;
	neg_z=load_image(tdesc.cube_negative_z);
	neg_z->convert();
	neg_z->resize();
	if(neg_z->width>max_size) max_size=neg_z->width;

	if(max_size>max_cube_map_size) max_size=max_cube_map_size;
	pos_x->resize(max_size);
	neg_x->resize(max_size);
	pos_y->resize(max_size);
	neg_y->resize(max_size);
	pos_z->resize(max_size);
	neg_z->resize(max_size);

	//ladowanie do tekstury opengl 
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, texture_id);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, tdesc.mag_filter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, tdesc.min_filter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glPixelStorei(GL_UNPACK_ALIGNMENT,0);
	GLenum internal_format;
	internal_format= (tdesc.alpha_mode==TAM_NONE) ? GL_RGB : GL_RGBA;
	if(compress_textures) {
		if(internal_format==GL_RGBA) internal_format=GL_COMPRESSED_RGBA_ARB;
		if(internal_format==GL_RGB) internal_format=GL_COMPRESSED_RGB_ARB;
	}
#ifdef _LOG_EVERYTHING
	DebugFileMessage("textures.log","Calling glTexImage2D with level 0\n");
#endif
	//load all faces
	//negative x
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,0,internal_format,neg_x->width,
		neg_x->height,0,GL_RGBA,GL_UNSIGNED_BYTE,neg_x->pixel_buf);
	switch(tdesc.min_filter) {
	case GL_NEAREST:
	break;
	default:
		//we don't really know all min filters (EXT_texture_filter_anisotropic etc), but
		//non-mipmapped filters are not likely to be different than the above
		int i;
		i=1;
		int tmp;
		tmp=neg_x->width;
		while(tmp!=1) {
			tmp/=2;
			neg_x->resize(tmp);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,i,internal_format,tmp,tmp,0,
				GL_RGBA,GL_UNSIGNED_BYTE,neg_x->pixel_buf);
			i++;
		}
	break;
	}
	//positive x
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,0,internal_format,pos_x->width,
		pos_x->height,0,GL_RGBA,GL_UNSIGNED_BYTE,pos_x->pixel_buf);
	switch(tdesc.min_filter) {
	case GL_NEAREST:
	break;
	default:
		//we don't really know all min filters (EXT_texture_filter_anisotropic etc), but
		//non-mipmapped filters are not likely to be different than the above
		int i;
		i=1;
		int tmp;
		tmp=pos_x->width;
		while(tmp!=1) {
			tmp/=2;
			pos_x->resize(tmp);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,i,internal_format,tmp,tmp,0,
				GL_RGBA,GL_UNSIGNED_BYTE,pos_x->pixel_buf);
			i++;
		}
	break;
	}
	//negative y
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,0,internal_format,neg_y->width,
		neg_y->height,0,GL_RGBA,GL_UNSIGNED_BYTE,neg_y->pixel_buf);
	switch(tdesc.min_filter) {
	case GL_NEAREST:
	break;
	default:
		//we don't really know all min filters (EXT_texture_filter_anisotropic etc), but
		//non-mipmapped filters are not likely to be different than the above
		int i;
		i=1;
		int tmp;
		tmp=neg_y->width;
		while(tmp!=1) {
			tmp/=2;
			neg_y->resize(tmp);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,i,internal_format,tmp,tmp,0,
				GL_RGBA,GL_UNSIGNED_BYTE,neg_y->pixel_buf);
			i++;
		}
	break;
	}
	//positive y
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,0,internal_format,pos_y->width,
		pos_y->height,0,GL_RGBA,GL_UNSIGNED_BYTE,pos_y->pixel_buf);
	switch(tdesc.min_filter) {
	case GL_NEAREST:
	break;
	default:
		//we don't really know all min filters (EXT_texture_filter_anisotropic etc), but
		//non-mipmapped filters are not likely to be different than the above
		int i;
		i=1;
		int tmp;
		tmp=pos_y->width;
		while(tmp!=1) {
			tmp/=2;
			pos_y->resize(tmp);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,i,internal_format,tmp,tmp,0,
				GL_RGBA,GL_UNSIGNED_BYTE,pos_y->pixel_buf);
			i++;
		}
	break;
	}
	//negative z
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB,0,internal_format,neg_z->width,
		neg_z->height,0,GL_RGBA,GL_UNSIGNED_BYTE,neg_z->pixel_buf);
	switch(tdesc.min_filter) {
	case GL_NEAREST:
	break;
	default:
		//we don't really know all min filters (EXT_texture_filter_anisotropic etc), but
		//non-mipmapped filters are not likely to be different than the above
		int i;
		i=1;
		int tmp;
		tmp=neg_z->width;
		while(tmp!=1) {
			tmp/=2;
			neg_z->resize(tmp);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB,i,internal_format,tmp,tmp,0,
				GL_RGBA,GL_UNSIGNED_BYTE,neg_z->pixel_buf);
			i++;
		}
	break;
	}
	//positive z
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,0,internal_format,pos_z->width,
		pos_z->height,0,GL_RGBA,GL_UNSIGNED_BYTE,pos_z->pixel_buf);
	switch(tdesc.min_filter) {
	case GL_NEAREST:
	break;
	default:
		//we don't really know all min filters (EXT_texture_filter_anisotropic etc), but
		//non-mipmapped filters are not likely to be different than the above
		int i;
		i=1;
		int tmp;
		tmp=pos_z->width;
		while(tmp!=1) {
			tmp/=2;
			pos_z->resize(tmp);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,i,internal_format,tmp,tmp,0,
				GL_RGBA,GL_UNSIGNED_BYTE,pos_z->pixel_buf);
			i++;
		}
	break;
	}
	//delete images
	delete pos_x;
	delete pos_y;
	delete pos_z;
	delete neg_x;
	delete neg_y;
	delete neg_z;
}

//Cube map normalizer generator by Mark Kilgard/NVIDIA Corp
//see "Practical and Robust Bump-mapping Technique for Today’s GPUs"
//somewhere on www.nvidia.com/developer
static void getCubeVector(int i, int cubesize, int x, int y, float *vector) {
	float s, t, sc, tc, mag;
	s = ((float)x + 0.5) / (float)cubesize;
	t = ((float)y + 0.5) / (float)cubesize;
	sc = s*2.0 - 1.0;
	tc = t*2.0 - 1.0;
	switch (i) { /* See Table 1 for the rationale for these cases. */
	case 0: vector[0] = 1.0; vector[1] = -tc; vector[2] = -sc; break;
	case 1: vector[0] = -1.0; vector[1] = -tc; vector[2] = sc; break;
	case 2: vector[0] = sc; vector[1] = 1.0; vector[2] = tc; break;
	case 3: vector[0] = sc; vector[1] = -1.0; vector[2] = -tc; break;
	case 4: vector[0] = sc; vector[1] = -tc; vector[2] = 1.0; break;
	case 5: vector[0] = -sc; vector[1] = -tc; vector[2] = -1.0; break;
	}
	mag = 1.0/sqrt(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);
	vector[0] *= mag;
	vector[1] *= mag;
	vector[2] *= mag;
}

static int makeNormalizeVectorCubeMap(int size) {
	float vector[3];
	int i, x, y;
	GLubyte *pixels;
	pixels = new GLubyte[size*size*3];
	if (pixels == NULL) {
		return 0; /* Memory allocation failed. */
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	for (i = 0; i < 6; i++) {
		for (y = 0; y < size; y++) {
			for (x = 0; x < size; x++) {
				getCubeVector(i, size, x, y, vector);
				pixels[3*(y*size+x) + 0] = 128 + 127*vector[0];
				pixels[3*(y*size+x) + 1] = 128 + 127*vector[1];
				pixels[3*(y*size+x) + 2] = 128 + 127*vector[2];
			}
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB+i, 0, GL_RGB8,
			size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	}
	free(pixels);
	return 1; /* Success. */
}

void Texture::gen_cube_map_normalizer(int size) {
	if(!hasCubeMaps) PrintfBox("Cube mapping not supported!");
	is_cube_map=1;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, texture_id);
	int res=makeNormalizeVectorCubeMap(size);
	if(!res) PrintfBox("Failed to generate cube map normalizer");
}

void Texture::load_bump_map(TexLoadDesc &tdesc) {
	Image *img;
	img=load_image(tdesc.filename);
//	img->convert();
//	img->resize();
	img->make_normal_map();
	int level = 0;
	//ladowanie do tekstury opengl 
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT,0);
	/* Load original maximum resolution normal map. */
	/* The BGRA color component ordering is fastest for NVIDIA. */
	glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA8, img->width, img->width, level,
	GL_BGRA_EXT, GL_UNSIGNED_BYTE, img->pixel_buf);
	/* Down-sample the normal map for mipmap levels down to 1x1. */
	while (img->width>1) {
	level++;
	img->downsample_normal_map();
	glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA8, img->width, img->height, 0,
	GL_BGRA_EXT, GL_UNSIGNED_BYTE, img->pixel_buf);
	}
	delete img;	
}


Texture::Texture(TexLoadDesc &tdesc) {
	is_cube_map=0;
	lod_bias=0.0f;
	Image* img,*alpha_img;
	char* filename=tdesc.filename;
	img=NULL;
	alpha_img=NULL;
	if(tdesc.type==TT_CUBE_MAP) {
		load_cube_map(tdesc);
		return;
	}
	if(tdesc.type==TT_CUBE_MAP_NORMALIZER) {
		gen_cube_map_normalizer(tdesc.cube_map_size);
		return;
	}
	if(tdesc.type==TT_BUMP_HEIGHTFIELD) {
		load_bump_map(tdesc);
		return;
	}

	switch(tdesc.img_filetype) {
	case TFT_AUTO:
		int i;
		i=strlen(filename);
		const char* str;
		str=filename+i-4;
		if(*str=='.') {
			str+=1;
			if(strcmp(str,"tga")==0) {
				img=load_TGA_ex(filename);
			}
			if(strcmp(str,"png")==0) {
				img=load_PNG_ex(filename);
			}
			if(strcmp(str,"TGA")==0) {
				img=load_TGA_ex(filename);
			}
			if(strcmp(str,"PNG")==0) {
				img=load_PNG_ex(filename);
			}
			if(strcmp(str,"jpg")==0) {
				img=load_JPEG_ex(filename);
			}
			if(strcmp(str,"JPG")==0) {
				img=load_JPEG_ex(filename);
			}
		}
		else {
			if(strcmp(str,"jpeg")==0) {
				img=load_JPEG_ex(filename);
			}
			if(strcmp(str,"JPEG")==0) {
				img=load_JPEG_ex(filename);
			}
		}
	break;
	case TFT_PNG_IMAGE:
		img=load_PNG_ex(filename);
	break;
	case TFT_JPEG_IMAGE:
		img=load_JPEG_ex(filename);
	break;
	case TFT_TGA_IMAGE:
		img=load_TGA_ex(filename);
	break;
	}
	if(img==NULL) return;
	img->convert();
	float ratio=1.0f;
	if(img->width!=img->height) {
		ratio=(float)img->width/(float)img->height;
		if(ratio<1.0f) ratio=1.0f/ratio;
	}
	lod_bias=1.0f-ratio;
	img->resize();

	if((tdesc.alpha_mode==TAM_SEPARATE_IMAGE) ||
		(tdesc.alpha_mode==TAM_INVERSE_SEPARATE_IMAGE)) {
		filename=tdesc.alpha_file;
		switch(tdesc.alpha_filetype) {
		case TFT_AUTO:
			int i;
			i=strlen(filename);
			const char* str;
			str=filename+i-4;
			if(*str=='.') {
				str+=1;
				if(strcmp(str,"tga")==0) {
					alpha_img=load_TGA_ex(filename);
				}
				if(strcmp(str,"png")==0) {
					alpha_img=load_PNG_ex(filename);
				}
				if(strcmp(str,"TGA")==0) {
					alpha_img=load_TGA_ex(filename);
				}
				if(strcmp(str,"PNG")==0) {
					alpha_img=load_PNG_ex(filename);
				}
				if(strcmp(str,"jpg")==0) {
					alpha_img=load_JPEG_ex(filename);
				}
				if(strcmp(str,"JPG")==0) {
					alpha_img=load_JPEG_ex(filename);
				}
			}
			else {
				if(strcmp(str,"jpeg")==0) {
					alpha_img=load_JPEG_ex(filename);
				}
				if(strcmp(str,"JPEG")==0) {
					alpha_img=load_JPEG_ex(filename);
				}
			}
		break;
		case TFT_PNG_IMAGE:
			alpha_img=load_PNG_ex(filename);
		break;
		case TFT_JPEG_IMAGE:
			alpha_img=load_JPEG_ex(filename);
		break;
		case TFT_TGA_IMAGE:
			alpha_img=load_TGA_ex(filename);
		break;
		}
		alpha_img->convert();
		alpha_img->resize(img->width);
	}
	load_GL_texture(img,tdesc,alpha_img);
}

void Texture::load_GL_texture(Image* img,TexLoadDesc &tdesc,Image* alpha_img) {
	//we have 32-bit rgba with power of 2 size now
	//generate alpha channel, mipmaps and load up the texture
	int i,j;
	switch(tdesc.color_mode) {
	case TCM_DEFAULT:
	break;
	case TCM_INVERT:
		for(i=0;i<img->height;i++) {
			for(j=0;j<img->width;j++) {
				img->pixel_buf[4*(i*img->width+j)]=
					(GLubyte)255-img->pixel_buf[4*(i*img->width+j)];
				img->pixel_buf[4*(i*img->width+j)+1]=
					(GLubyte)255-img->pixel_buf[4*(i*img->width+j)+1];
				img->pixel_buf[4*(i*img->width+j)+2]=
					(GLubyte)255-img->pixel_buf[4*(i*img->width+j)+2];
			}
		}
	break;
	}
	switch(tdesc.alpha_mode) {
	case TAM_NONE:
	break;
	case TAM_CONSTANT:
		for(i=0;i<img->height;i++) {
			for(j=0;j<img->width;j++) {
				img->pixel_buf[4*(i*img->width+j)+3]=(GLubyte)(tdesc.alpha1*255.0f);
			}
		}
	break;
	case TAM_IMAGE_INTERNAL: //this is useful for 32-bit TGA/PNG
	break;
	case TAM_SEPARATE_IMAGE: 
		for(i=0;i<img->height;i++) {
			for(j=0;j<img->width;j++) {
				img->pixel_buf[4*(i*img->width+j)+3]=
					(GLubyte)(((GLuint)alpha_img->pixel_buf[4*(i*img->width+j)]
					+(GLuint)alpha_img->pixel_buf[4*(i*img->width+j)+1]
					+(GLuint)alpha_img->pixel_buf[4*(i*img->width+j)+2])/3);
			}
		}
	break;
	case TAM_INVERSE_SEPARATE_IMAGE: //LW transparency textures
		for(i=0;i<img->height;i++) {
			for(j=0;j<img->width;j++) {
				img->pixel_buf[4*(i*img->width+j)+3]=
					(GLubyte)(255.0f-((GLfloat)alpha_img->pixel_buf[4*(i*img->width+j)]
					+(GLfloat)alpha_img->pixel_buf[4*(i*img->width+j)+1]
					+(GLfloat)alpha_img->pixel_buf[4*(i*img->width+j)+2])/3.0f);
			}
		}
	break;
	case TAM_INTENSITY: //(r+g+b)/3
		for(i=0;i<img->height;i++) {
			for(j=0;j<img->width;j++) {
				img->pixel_buf[4*(i*img->width+j)+3]=
					(GLubyte)(((GLfloat)img->pixel_buf[4*(i*img->width+j)]
					+(GLfloat)img->pixel_buf[4*(i*img->width+j)+1]
					+(GLfloat)img->pixel_buf[4*(i*img->width+j)+2])/3.0f);
			}
		}
	break;
	case TAM_INVERSE_INTENSITY:  //1-(r+g+b)/3
		for(i=0;i<img->height;i++) {
			for(j=0;j<img->width;j++) {
				img->pixel_buf[4*(i*img->width+j)+3]=
					(GLubyte)(1.0f-(((GLfloat)img->pixel_buf[4*(i*img->width+j)]
					+(GLfloat)img->pixel_buf[4*(i*img->width+j)+1]
					+(GLfloat)img->pixel_buf[4*(i*img->width+j)+2])/3.0f));
			}
		}
	break;
	case TAM_GRADIENT: // (alpha1,alpha2)->(alpha3,alpha4)
	break;
	case TAM_RADIAL_GRADIENT: //(alpha1,alpha2,radius=alpha3)
		int x,y,r;
		x=(int)(tdesc.alpha1*img->width);
		y=(int)(tdesc.alpha2*img->height);
		r=(int)(tdesc.alpha3*img->width);
		int tmp_r;
		if(r>0) {
			for(i=0;i<img->height;i++) {
				for(j=0;j<img->width;j++) {
					tmp_r=(int)(sqrt((j-x)*(j-x)+(i-y)*(i-y)));
					img->pixel_buf[4*(i*img->width+j)+3]=
						(tmp_r > r) ? 0 : (GLubyte)((GLfloat)tmp_r/(GLfloat)r);
				}
			}
		}
	break;
	case TAM_INVERSE_RADIAL_GRADIENT:
		x=(int)(tdesc.alpha1*img->width);
		y=(int)(tdesc.alpha2*img->height);
		r=(int)(tdesc.alpha3*img->width);
		if(r>0) {
			for(i=0;i<img->height;i++) {
				for(j=0;j<img->width;j++) {
					tmp_r=(int)(sqrt((j-x)*(j-x)+(i-y)*(i-y)));
					img->pixel_buf[4*(i*img->width+j)+3]=
						(tmp_r > r) ? 255 : (GLubyte)(1.0f-(GLfloat)tmp_r/(GLfloat)r);
				}
			}
		}
	break;
	case TAM_RED:
		for(i=0;i<img->height;i++) {
			for(j=0;j<img->width;j++) {
				img->pixel_buf[4*(i*img->width+j)+3]=img->pixel_buf[4*(i*img->width+j)];
			}
		}
	break;
	case TAM_GREEN:
		for(i=0;i<img->height;i++) {
			for(j=0;j<img->width;j++) {
				img->pixel_buf[4*(i*img->width+j)+3]=img->pixel_buf[4*(i*img->width+j)+1];
			}
		}
	break;
	case TAM_BLUE:
		for(i=0;i<img->height;i++) {
			for(j=0;j<img->width;j++) {
				img->pixel_buf[4*(i*img->width+j)+3]=img->pixel_buf[4*(i*img->width+j)+2];
			}
		}
	break;
	case TAM_INVERSE_RED:
		for(i=0;i<img->height;i++) {
			for(j=0;j<img->width;j++) {
				img->pixel_buf[4*(i*img->width+j)+3]=
					(GLubyte)(1.0f-(GLfloat)img->pixel_buf[4*(i*img->width+j)]);
			}
		}
	break;
	case TAM_INVERSE_GREEN:
		for(i=0;i<img->height;i++) {
			for(j=0;j<img->width;j++) {
				img->pixel_buf[4*(i*img->width+j)+3]=
					(GLubyte)(1.0f-(GLfloat)img->pixel_buf[4*(i*img->width+j)+1]);
			}
		}
	break;
	case TAM_INVERSE_BLUE:
		for(i=0;i<img->height;i++) {
			for(j=0;j<img->width;j++) {
				img->pixel_buf[4*(i*img->width+j)+3]=
					(GLubyte)(1.0f-(GLfloat)img->pixel_buf[4*(i*img->width+j)+2]);
			}
		}
	break;
	}
	//ladowanie do tekstury opengl 
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tdesc.mag_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tdesc.min_filter);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tdesc.wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tdesc.wrap_t);
#ifdef _LOG_EVERYTHING
	DebugFileMessage("textures.log","GL_REPEAT is %d, GL_CLAMP is %d\n",GL_REPEAT,GL_CLAMP);
	DebugFileMessage("textures.log","WRAP_S id %d\n",tdesc.wrap_s);
	DebugFileMessage("textures.log","WRAP_T id %d\n",tdesc.wrap_t);
#endif
	glPixelStorei(GL_UNPACK_ALIGNMENT,0);
	GLenum internal_format;
	internal_format= (tdesc.alpha_mode==TAM_NONE) ? GL_RGB : GL_RGBA;
	if(compress_textures) {
		if(internal_format==GL_RGBA) internal_format=GL_COMPRESSED_RGBA_ARB;
		if(internal_format==GL_RGB) internal_format=GL_COMPRESSED_RGB_ARB;
	}
//	gluBuild2DMipmaps(GL_TEXTURE_2D,internal_format,img->width,img->height,
//		GL_RGBA,GL_UNSIGNED_BYTE,img->pixel_buf);
#ifdef _LOG_EVERYTHING
	DebugFileMessage("textures.log","Calling glTexImage2D with level 0\n");
#endif

#ifdef _TIME_LOAD_JPG_
	int start_time,end_time;
	start_time=timeGetTime();
#endif

	glTexImage2D(GL_TEXTURE_2D,0,internal_format,img->width,img->height,0,GL_RGBA,GL_UNSIGNED_BYTE,img->pixel_buf);

	switch(tdesc.min_filter) {
	case GL_NEAREST:
	break;
	default:
		//we don't really know all min filters (EXT_texture_filter_anisotropic etc), but
		//non-mipmapped filters are not likely to be different than the above
		int i;
		i=1;
		int tmp;
		tmp=img->width;
		while(tmp!=1) {
			tmp/=2;
			//img->resize(tmp);
			int tmp_x,tmp_y;
			unsigned char *pixels=img->pixel_buf;
			for(tmp_y=0;tmp_y<tmp;tmp_y++) {
				for(tmp_x=0;tmp_x<tmp;tmp_x++) {
					pixels[4*(tmp_y*tmp+tmp_x)]=(unsigned char)(
						((unsigned int)pixels[4*(tmp_y*tmp*4+tmp_x*2)]+
						(unsigned int)pixels[4*(tmp_y*tmp*4+tmp_x*2+1)]+
						(unsigned int)pixels[4*((2*tmp_y+1)*tmp*2+tmp_x*2)]+
						(unsigned int)pixels[4*((2*tmp_y+1)*tmp*2+tmp_x*2+1)])/4);
					pixels[4*(tmp_y*tmp+tmp_x)+1]=(unsigned char)(
						((unsigned int)pixels[4*(tmp_y*tmp*4+tmp_x*2)+1]+
						(unsigned int)pixels[4*(tmp_y*tmp*4+tmp_x*2+1)+1]+
						(unsigned int)pixels[4*((2*tmp_y+1)*tmp*2+tmp_x*2)+1]+
						(unsigned int)pixels[4*((2*tmp_y+1)*tmp*2+tmp_x*2+1)+1])/4);
					pixels[4*(tmp_y*tmp+tmp_x)+2]=(unsigned char)(
						((unsigned int)pixels[4*(tmp_y*tmp*4+tmp_x*2)+2]+
						(unsigned int)pixels[4*(tmp_y*tmp*4+tmp_x*2+1)+2]+
						(unsigned int)pixels[4*((2*tmp_y+1)*tmp*2+tmp_x*2)+2]+
						(unsigned int)pixels[4*((2*tmp_y+1)*tmp*2+tmp_x*2+1)+2])/4);
					pixels[4*(tmp_y*tmp+tmp_x)+3]=(unsigned char)(
						((unsigned int)pixels[4*(tmp_y*tmp*4+tmp_x*2)+3]+
						(unsigned int)pixels[4*(tmp_y*tmp*4+tmp_x*2+1)+3]+
						(unsigned int)pixels[4*((2*tmp_y+1)*tmp*2+tmp_x*2)+3]+
						(unsigned int)pixels[4*((2*tmp_y+1)*tmp*2+tmp_x*2+1)+3])/4);

				}
			}
#ifdef _LOG_EVERYTHING
				DebugFileMessage("textures.log","Calling glTexImage2D with level %d\n",i);
#endif
			glTexImage2D(GL_TEXTURE_2D,i,internal_format,tmp,tmp,0,GL_RGBA,GL_UNSIGNED_BYTE,img->pixel_buf);
			i++;
		}
	break;
	}
#ifdef _TIME_LOAD_JPG_
	end_time=timeGetTime();
	DebugFileMessage("timing.txt","teximage2d calls for all mipmap levels took %d ms\n",
		end_time-start_time);

#endif

}


/*
  JPEG loader
  uzywa Intel JPEG library
  kod na prawie na zywca wyrwany z dokumentacji
*/
void Texture::load_JPEG(const char* filename) {
	TexLoadDesc tdesc;
	Image* img=load_JPEG_ex(filename);
	img->convert();
	float ratio=1.0f;
	if(img->width!=img->height) {
		ratio=(float)img->width/(float)img->height;
		if(ratio<1.0f) ratio=1.0f/ratio;
	}
	lod_bias=1.0f-ratio;
	img->resize();
	load_GL_texture(img,tdesc);
	delete img->pixel_buf;
	delete img;
}


void Texture::load_TGA(const char* filename) {
	TexLoadDesc tdesc;
	Image* img=load_TGA_ex(filename);
	img->convert();
	float ratio=1.0f;
	if(img->width!=img->height) {
		ratio=(float)img->width/(float)img->height;
		if(ratio<1.0f) ratio=1.0f/ratio;
	}
	lod_bias=1.0f-ratio;
	img->resize();
	load_GL_texture(img,tdesc);
	delete img->pixel_buf;
	delete img;
}


Texture::Texture(const char* filename,TFT_FILETYPE type) {
	lod_bias=0.0f;
	is_cube_map=0;
	switch(type) {
	case TFT_AUTO:
		int i;
		i=strlen(filename);
		const char* str;
		str=filename+i-4;
		if(*str=='.') {
			str+=1;
			if(strcmp(str,"tga")==0) {
				load_TGA(filename);
				return;
			}
			if(strcmp(str,"png")==0) {
				load_PNG(filename);
				return;
			}
			if(strcmp(str,"TGA")==0) {
				load_TGA(filename);
				return;
			}
			if(strcmp(str,"PNG")==0) {
				load_PNG(filename);
				return;
			}
			if(strcmp(str,"jpg")==0) {
				load_JPEG(filename);
				return;
			}
			if(strcmp(str,"JPG")==0) {
				load_JPEG(filename);
				return;
			}
		}
		else {
			if(strcmp(str,"jpeg")==0) {
				load_JPEG(filename);
				return;
			}
			if(strcmp(str,"JPEG")==0) {
				load_JPEG(filename);
				return;
			}
		}
	break;
	case TFT_PNG_IMAGE:
		load_PNG(filename);
	break;
	case TFT_JPEG_IMAGE:
		load_JPEG(filename);
	break;
	case TFT_TGA_IMAGE:
		load_TGA(filename);
	break;
	}
}

Texture::Texture(FILE* f,TFT_FILETYPE type) {
}

Texture::Texture(int width,int height,void* buffer,TF_FORMAT format) {
	is_cube_map=0;
	lod_bias=0.0f;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT,0);  //moze trzeba zmienic
	switch(format) {
	case TF_RGBA:
		glTexImage2D(GL_TEXTURE_2D,0,4,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,buffer);
		break;
	case TF_RGB:
		glTexImage2D(GL_TEXTURE_2D,0,3,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,buffer);
		break;
	}
}

void Texture::activate() {
	if(is_cube_map) {
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARB,texture_id);
	}
	else {
		glBindTexture(GL_TEXTURE_2D,texture_id);
//		PrintfBox("lod bias %f",lod_bias);
		if(hasTextureLODBias && 
			(lod_bias!=c_state->textures[c_state->current_texture].lod_bias)) {
//			PrintfBox("elou");
			c_state->textures[c_state->current_texture].lod_bias=lod_bias;
			glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT,GL_TEXTURE_LOD_BIAS_EXT,lod_bias);
		}
	}
}

Texture::~Texture() {
	glDeleteTextures(1,&texture_id);
}

void Texture::update(int time) {
}

void Texture::backbuffer_copy(int size) {
	glBindTexture(GL_TEXTURE_2D,texture_id);
	glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,size,size);
}

void Texture::reload(GLubyte *buffer,int size) {
	glBindTexture(GL_TEXTURE_2D,texture_id);
	glTexSubImage2D(GL_TEXTURE_2D,0,0,0,size,size,GL_RGB,GL_UNSIGNED_BYTE,buffer);
}

TextureManager::TextureManager() {
    num_force_textures=0;
	num_textures=0;
	tex_descs=NULL;
	textures=NULL;
	force_textures=NULL;
}

TextureManager::~TextureManager() {
	int i;
	if(num_textures>0) {
		for(i=0;i<num_textures;i++) {
			delete tex_descs[i];
			delete textures[i];
		}
		delete tex_descs;
		delete textures;
	}
	if(num_force_textures>0) {
		int i;
		for(i=0;i<num_force_textures;i++) {
			delete force_textures[i];
		}
		delete force_textures;
	}
}

Texture* TextureManager::get_texture(char* filename,TFT_FILETYPE type,int force_reload) {
TexLoadDesc tdesc;
tdesc.filename=filename;
tdesc.img_filetype=type;
return get_texture(tdesc,force_reload);
}

//#define _GET_TEXTURE_TIMER_

Texture* TextureManager::get_texture(TexLoadDesc& tdesc,int force_reload) {
#ifdef _LOG_EVERYTHING
	DebugFileMessage("textures.log","Entered get_texture.\n");
	DebugFileMessage("textures.log","filename is %s\n",tdesc.filename);
#endif
	int i;

#ifdef _GET_TEXTURE_TIMER_
	int start_time,end_time;
	start_time=timeGetTime();
	DebugFileMessage("timing.txt","entering get_texture\n");
#endif

	if(force_reload) {
		Texture** tmp_tex;
		tmp_tex=new Texture*[num_force_textures+1];
		for(i=0;i<num_force_textures;i++) tmp_tex[i]=force_textures[i];
		tmp_tex[num_force_textures++]=new Texture(tdesc);
		if(force_textures!=NULL) delete force_textures;
		force_textures=tmp_tex;
		return force_textures[num_force_textures-1];
	}
	else {
		if(num_textures==0) {
			textures=new Texture*[1];
			textures[num_textures++]=new Texture(tdesc);
			tex_descs=new TexLoadDesc*[1];
			tex_descs[0]=new TexLoadDesc;
			*(tex_descs[0])=tdesc;
#ifdef _LOG_EVERYTHING
			if(textures[0]==NULL) {
				DebugFileMessage("warnings.log","TextureManager - returning NULL");
			}
#endif
#ifdef _GET_TEXTURE_TIMER_
			end_time=timeGetTime();
			if(tdesc.filename) {
				DebugFileMessage("timing.txt","get_texture call took %d ms (%s)\n",end_time-start_time,
					tdesc.filename);
			}
			else {
				DebugFileMessage("timing.txt","get_texture call took %d ms\n",end_time-start_time);
			}
#endif
			return textures[0];
		}
		else {
			for(i=0;i<num_textures;i++)
				if(*(tex_descs[i])==tdesc) {
#ifdef _LOG_EVERYTHING
			if(textures[i]==NULL) {
				DebugFileMessage("warnings.log","TextureManager - returning NULL");
			}
#endif
#ifdef _GET_TEXTURE_TIMER_
			end_time=timeGetTime();
			if(tdesc.filename) {
				DebugFileMessage("timing.txt","get_texture call took %d ms (%s)\n",end_time-start_time,
					tdesc.filename);
			}
			else {
				DebugFileMessage("timing.txt","get_texture call took %d ms\n",end_time-start_time);
			}
#endif
					return textures[i];
				}
			Texture** tmp_tex;
			TexLoadDesc** tmp_descs;
			tmp_tex=new Texture*[num_textures+1];
			tmp_descs=new TexLoadDesc*[num_textures+1];
			for(i=0;i<num_textures;i++) {
				tmp_tex[i]=textures[i];
				tmp_descs[i]=tex_descs[i];
			}
			tmp_tex[num_textures]=new Texture(tdesc);
			tmp_descs[num_textures]=new TexLoadDesc;
			*(tmp_descs[num_textures++])=tdesc;
			delete tex_descs;
			tex_descs=tmp_descs;
			delete textures;
			textures=tmp_tex;
#ifdef _LOG_EVERYTHING
			if(textures[num_textures-1]==NULL) {
				DebugFileMessage("warnings.log","TextureManager - returning NULL");
			}
#endif
#ifdef _GET_TEXTURE_TIMER_
			end_time=timeGetTime();
			if(tdesc.filename) {
				DebugFileMessage("timing.txt","get_texture call took %d ms (%s)\n",end_time-start_time,
					tdesc.filename);
			}
			else {
				DebugFileMessage("timing.txt","get_texture call took %d ms\n",end_time-start_time);
			}
#endif
			return textures[num_textures-1];
		}
	}
}

TextureManager tex_manager;