/***************************************************************************
                          texture.h  -  description
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/


#ifndef _DM_TEXTURE_H_
#define _DM_TEXTURE_H_

#include<stdio.h>
#include"glstuff.h"

enum TF_FORMAT {  //internal texture format
	TF_RGB,
	TF_RGBA  //default
};

enum TFT_FILETYPE {
	TFT_AUTO,  //default
	TFT_PNG_IMAGE,
	TFT_JPEG_IMAGE,
	TFT_TGA_IMAGE
};

enum TT_TYPE {
	TT_IMAGE,  //default
	TT_CUBE_MAP,
	TT_CUBE_MAP_NORMALIZER,
	TT_BUMP_HEIGHTFIELD,
	TT_PROC
};

enum TAM_ALPHA_MODE {
	TAM_NONE,  //default
	TAM_CONSTANT, 
	TAM_IMAGE_INTERNAL, //this is useful for 32-bit TGA
	TAM_SEPARATE_IMAGE, //VERY useful
	TAM_INVERSE_SEPARATE_IMAGE,  //LW default
	TAM_INTENSITY,
	TAM_INVERSE_INTENSITY,
	TAM_GRADIENT, // (alpha1,alpha2)->(alpha3,alpha4)
	TAM_RADIAL_GRADIENT, //(alpha1,alpha2,radius=alpha3)
	TAM_INVERSE_RADIAL_GRADIENT,
	TAM_RED,
	TAM_GREEN,
	TAM_BLUE,
	TAM_INVERSE_RED,
	TAM_INVERSE_GREEN,
	TAM_INVERSE_BLUE
};

enum TCM_COLOR_MODE {
	TCM_DEFAULT,
	TCM_INVERT
};

class TexLoadDesc {
public:
	void operator=(const TexLoadDesc& tdesc);
	int operator==(const TexLoadDesc& tdesc);
	TT_TYPE type;
	TF_FORMAT format;
	TFT_FILETYPE img_filetype;
	TFT_FILETYPE alpha_filetype;
	TAM_ALPHA_MODE alpha_mode;
	TCM_COLOR_MODE color_mode;
	char* filename;
	char* alpha_file;
	GLenum min_filter;
	GLenum mag_filter;
	float alpha1,alpha2,alpha3,alpha4;  //alpha generator parameters
	float alpha_bias;
	GLenum wrap_s,wrap_t;
	//cupe mapping support
	int cube_map_size;					//used for normalizer only
	char *cube_positive_x;
	char *cube_negative_x;
	char *cube_positive_y;
	char *cube_negative_y;
	char *cube_positive_z;
	char *cube_negative_z;
	TexLoadDesc();
};

enum IF_FORMAT {
	IF_LUM8,
	IF_LUM16,
	IF_PALETTIZED,
	IF_PALETTIZED_RGBA,
	IF_RGB555,
	IF_RGB565,
	IF_RGB888,
	IF_BGR888,
	IF_RGBA8888, //everything is converted to this format by convert() method
	IF_RGBA4444,
	IF_BGRA8888,
	IF_ABGR8888, 
	IF_GRAYSCALE, //for heightmaps
	IF_NORMAL_MAP //encoded (BGRA!!!) normal map
};

class Image {
public:
	GLubyte *palette;
	GLubyte *pixel_buf;
	int bytes_per_pixel;
	IF_FORMAT format;
	int width;
	int height;
	Image();
	void convert();
	void make_grayscale();
	void make_normal_map();
	void downsample_normal_map();
	void resize(int size=0);  //resize to nearest power of 2
};

//image loaders for general purposes - also used internally
//by Texture class	
Image* load_JPEG_image(const char* filename);
Image* load_PNG_image(const char* filename);
Image* load_TGA_image(const char* filename);
Image* load_image(const char* filename);


class Texture {
	int is_cube_map;
	GLuint texture_id;
	int tx_width,tx_height;
	float lod_bias;
	void load_JPEG(const char* filename);
	void load_PNG(const char* filename);
	void load_TGA(const char* filename);
	void load_GL_texture(Image* img,TexLoadDesc& tdesc,Image* alpha_img=NULL);
	void load_cube_map(TexLoadDesc &tdesc);
	void load_bump_map(TexLoadDesc &tdesc);
	void gen_cube_map_normalizer(int size);
public:
	Texture(int width,int height,void* buffer,TF_FORMAT format=TF_RGBA);
	Texture(const char* filename,TFT_FILETYPE type=TFT_AUTO);
	Texture(FILE* f,TFT_FILETYPE type); //TX_AUTO niedozwolony
	Texture(TexLoadDesc& tdesc);
	~Texture();
	Image* load_JPEG_ex(const char* filename);
	Image* load_PNG_ex(const char* filename);
	Image* load_TGA_ex(const char* filename);
	void update(int time=0);
	void activate();
	void backbuffer_copy(int size);
	void reload(GLubyte* buffer,int size);
};

//manager tekstur
//ogolnie rzecz biorac tablica asocjacyjna string->pointer
//zapenia jednokrotne ladowanie wszystkich tekstur
class TextureManager {
	Texture** textures;
	TexLoadDesc** tex_descs;
	int num_textures;
	Texture** force_textures;
	int num_force_textures;
public:
	TextureManager();
	Texture* get_texture(char* filename,TFT_FILETYPE type=TFT_AUTO,int force_reload=0);
	Texture* get_texture(TexLoadDesc& tdesc,int force_reload=0);
	~TextureManager();
};

extern "C++" TextureManager tex_manager;

#endif