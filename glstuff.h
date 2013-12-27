/***************************************************************************
                          glstuff.h  -  description
                             -------------------
    begin                : Fri May 5 2000
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/


#ifndef _GL_STUFF_H_
#define _GL_STUFF_H_

#ifdef _WIN32
#include<windows.h>
#endif
#include<GL/gl.h>

#ifdef _WIN32
#include "glext.h"
//z ponizszych linii copy/paste
//inicjalizacja rozszerzen OpenGL
// ARB_multitexture  
extern "C++" PFNGLMULTITEXCOORD1IARBPROC glMultiTexCoord1iARB;
extern "C++" PFNGLMULTITEXCOORD1IVARBPROC glMultiTexCoord1ivARB;
extern "C++" PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB;
extern "C++" PFNGLMULTITEXCOORD1FVARBPROC glMultiTexCoord1fvARB;
extern "C++" PFNGLMULTITEXCOORD1DARBPROC glMultiTexCoord1dARB;
extern "C++" PFNGLMULTITEXCOORD1DVARBPROC glMultiTexCoord1dvARB;
extern "C++" PFNGLMULTITEXCOORD1SARBPROC glMultiTexCoord1sARB;
extern "C++" PFNGLMULTITEXCOORD1SVARBPROC glMultiTexCoord1svARB;

extern "C++" PFNGLMULTITEXCOORD2IARBPROC glMultiTexCoord2iARB;
extern "C++" PFNGLMULTITEXCOORD2IVARBPROC glMultiTexCoord2ivARB;
extern "C++" PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
extern "C++" PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fvARB;
extern "C++" PFNGLMULTITEXCOORD2DARBPROC glMultiTexCoord2dARB;
extern "C++" PFNGLMULTITEXCOORD2DVARBPROC glMultiTexCoord2dvARB;
extern "C++" PFNGLMULTITEXCOORD2SARBPROC glMultiTexCoord2sARB;
extern "C++" PFNGLMULTITEXCOORD2SVARBPROC glMultiTexCoord2svARB;

extern "C++" PFNGLMULTITEXCOORD3IARBPROC glMultiTexCoord3iARB;
extern "C++" PFNGLMULTITEXCOORD3IVARBPROC glMultiTexCoord3ivARB;
extern "C++" PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB;
extern "C++" PFNGLMULTITEXCOORD3FVARBPROC glMultiTexCoord3fvARB;
extern "C++" PFNGLMULTITEXCOORD3DARBPROC glMultiTexCoord3dARB;
extern "C++" PFNGLMULTITEXCOORD3DVARBPROC glMultiTexCoord3dvARB;
extern "C++" PFNGLMULTITEXCOORD3SARBPROC glMultiTexCoord3sARB;
extern "C++" PFNGLMULTITEXCOORD3SVARBPROC glMultiTexCoord3svARB;

extern "C++" PFNGLMULTITEXCOORD4IARBPROC glMultiTexCoord4iARB;
extern "C++" PFNGLMULTITEXCOORD4IVARBPROC glMultiTexCoord4ivARB;
extern "C++" PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB;
extern "C++" PFNGLMULTITEXCOORD4FVARBPROC glMultiTexCoord4fvARB;
extern "C++" PFNGLMULTITEXCOORD4DARBPROC glMultiTexCoord4dARB;
extern "C++" PFNGLMULTITEXCOORD4DVARBPROC glMultiTexCoord4dvARB;
extern "C++" PFNGLMULTITEXCOORD4SARBPROC glMultiTexCoord4sARB;
extern "C++" PFNGLMULTITEXCOORD4SVARBPROC glMultiTexCoord4svARB;

extern "C++" PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
extern "C++" PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;

//EXT_point_parameters
extern "C++" PFNGLPOINTPARAMETERFEXTPROC glPointParameterfEXT;
extern "C++" PFNGLPOINTPARAMETERFVEXTPROC glPointParameterfvEXT;

//EXT_compiled_vertex_array 
extern "C++" PFNGLLOCKARRAYSEXTPROC glLockArraysEXT;
extern "C++" PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;


//EXT_fog_coord
extern "C++" PFNGLFOGCOORDFEXTPROC glFogCoordfEXT;
extern "C++" PFNGLFOGCOORDDEXTPROC glFogCoorddEXT;
extern "C++" PFNGLFOGCOORDFVEXTPROC glFogCoordfvEXT;
extern "C++" PFNGLFOGCOORDDVEXTPROC glFogCoorddvEXT;
extern "C++" PFNGLFOGCOORDPOINTEREXTPROC glFogCoordPointerEXT;

//EXT_secondary_color
extern "C++" PFNGLSECONDARYCOLOR3BEXTPROC glSecondaryColor3bEXT;
extern "C++" PFNGLSECONDARYCOLOR3SEXTPROC glSecondaryColor3sEXT;
extern "C++" PFNGLSECONDARYCOLOR3IEXTPROC glSecondaryColor3iEXT;
extern "C++" PFNGLSECONDARYCOLOR3FEXTPROC glSecondaryColor3fEXT;
extern "C++" PFNGLSECONDARYCOLOR3DEXTPROC glSecondaryColor3dEXT;
extern "C++" PFNGLSECONDARYCOLOR3UBEXTPROC glSecondaryColor3ubEXT;
extern "C++" PFNGLSECONDARYCOLOR3USEXTPROC glSecondaryColor3usEXT;
extern "C++" PFNGLSECONDARYCOLOR3UIEXTPROC glSecondaryColor3uiEXT;
extern "C++" PFNGLSECONDARYCOLOR3BVEXTPROC glSecondaryColor3bvEXT;
extern "C++" PFNGLSECONDARYCOLOR3SVEXTPROC glSecondaryColor3svEXT;
extern "C++" PFNGLSECONDARYCOLOR3IVEXTPROC glSecondaryColor3ivEXT;
extern "C++" PFNGLSECONDARYCOLOR3FVEXTPROC glSecondaryColor3fvEXT;
extern "C++" PFNGLSECONDARYCOLOR3DVEXTPROC glSecondaryColor3dvEXT;
extern "C++" PFNGLSECONDARYCOLOR3UBVEXTPROC glSecondaryColor3ubvEXT;
extern "C++" PFNGLSECONDARYCOLOR3USVEXTPROC glSecondaryColor3usvEXT;
extern "C++" PFNGLSECONDARYCOLOR3UIVEXTPROC glSecondaryColor3uivEXT;
extern "C++" PFNGLSECONDARYCOLORPOINTEREXTPROC glSecondaryColorPointerEXT;

//WGL_EXT_swap_control
extern "C++" PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
extern "C++" PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;

//NV_vertex_array_range
extern "C++" PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV;
extern "C++" PFNGLFLUSHVERTEXARRAYRANGENVPROC glFlushVertexArrayRangeNV;
extern "C++" PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV;
extern "C++" PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV;

//NV_register_combiners
extern "C++" PFNGLCOMBINERPARAMETERFVNVPROC glCombinerParameterfvNV;
extern "C++" PFNGLCOMBINERPARAMETERIVNVPROC glCombinerParameterivNV;
extern "C++" PFNGLCOMBINERPARAMETERFNVPROC glCombinerParameterfNV;
extern "C++" PFNGLCOMBINERPARAMETERINVPROC glCombinerParameteriNV;
extern "C++" PFNGLCOMBINERINPUTNVPROC glCombinerInputNV;
extern "C++" PFNGLCOMBINEROUTPUTNVPROC glCombinerOutputNV;
extern "C++" PFNGLFINALCOMBINERINPUTNVPROC glFinalCombinerInputNV;
extern "C++" PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC glGetCombinerInputParameterfvNV;
extern "C++" PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC glGetCombinerInputParameterivNV;
extern "C++" PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC glGetCombinerOutputParameterfvNV;
extern "C++" PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC glGetCombinerOutputParameterivNV;
extern "C++" PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV;
extern "C++" PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV;

extern "C++" PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;

//WGL_ARB_pixel_format
extern "C++" PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
extern "C++" PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB;
extern "C++" PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

//WGL_ARB_pbuffer
extern "C++" PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB;
extern "C++" PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB;
extern "C++" PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
extern "C++" PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB;
extern "C++" PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB;

//ARB_texture_compression
//TODO!!!!

//ATI_fragment_shader
extern "C++" PFNGLGENFRAGMENTSHADERSATIPROC		glGenFragmentShadersATI;
extern "C++" PFNGLBINDFRAGMENTSHADERATIPROC		glBindFragmentShaderATI;
extern "C++" PFNGLDELETEFRAGMENTSHADERATIPROC	glDeleteFragmentShaderATI;
extern "C++" PFNGLBEGINFRAGMENTSHADERATIPROC	glBeginFragmentShaderATI;
extern "C++" PFNGLENDFRAGMENTSHADERATIPROC		glEndFragmentShaderATI;
extern "C++" PFNGLPASSTEXCOORDATIPROC			glPassTexCoordATI;
extern "C++" PFNGLSAMPLEMAPATIPROC				glSampleMapATI;
extern "C++" PFNGLCOLORFRAGMENTOP1ATIPROC		glColorFragmentOp1ATI;
extern "C++" PFNGLCOLORFRAGMENTOP2ATIPROC		glColorFragmentOp2ATI;
extern "C++" PFNGLCOLORFRAGMENTOP3ATIPROC		glColorFragmentOp3ATI;
extern "C++" PFNGLALPHAFRAGMENTOP1ATIPROC		glAlphaFragmentOp1ATI;
extern "C++" PFNGLALPHAFRAGMENTOP2ATIPROC		glAlphaFragmentOp2ATI;
extern "C++" PFNGLALPHAFRAGMENTOP3ATIPROC		glAlphaFragmentOp3ATI;
extern "C++" PFNGLSETFRAGMENTSHADERCONSTANTATIPROC	glSetFragmentShaderConstantATI;

//ATI_pn_triangles
extern "C++" PFNGLPNTRIANGLESIATIPROC	glPNTrianglesiATI;
extern "C++" PFNGLPNTRIANGLESFATIPROC	glPNTrianglesfATI;


#endif 


extern "C++" int max_texture_size;
extern "C++" GLenum default_min_filter;
extern "C++" GLenum default_mag_filter;
extern "C++" int hasMultitexture;
extern "C++" unsigned int max_texture_units;
extern "C++" int hasTextureEnvAdd;
extern "C++" int hasTextureEnvCombine;
extern "C++" int hasBgra;
extern "C++" int hasPointParameters;
extern "C++" int hasNVTexgenReflection;
extern "C++" int hasAbgr;
extern "C++" int hasPackedPixels;
extern "C++" int hasCompiledVertexArray;
extern "C++" int hasStencilWrap;
extern "C++" int hasNVTextureEnvCombine4;
extern "C++" int hasFogCoord;
extern "C++" int hasRescaleNormal;
extern "C++" int hasSecondaryColor;
extern "C++" int hasSeparateSpecularColor;
extern "C++" int hasTextureLODBias;
extern "C++" int hasNVBlendSquare;
extern "C++" int hasVertexWeighting;
extern "C++" int hasNVFogDistance;
extern "C++" int hasNVLightMaxExponent;
extern "C++" int hasWglSwapControl;
extern "C++" int hasVertexArrayRange;
extern "C++" int hasCubeMaps;
extern "C++" int hasRegisterCombiners;
extern "C++" int max_cube_map_size;
extern "C++" int hasWglPixelFormat;
extern "C++" int hasPbuffers;
extern "C++" int hasTextureCompression;
extern "C++" int hasTextureEnvDot3;
extern "C++" int hasATIFragmentShader;
extern "C++" int hasATIPNTriangles;

void InitExtensions();
void InitWglExtensions();

//NV_vertex_array_range support class
class AGPAllocator {
	void *agp_mem;				//vertex array range
	void *mem_top;				//first free byte
	unsigned int block_size;	//block size
public:
	AGPAllocator(int size);
	~AGPAllocator();
	void* get_agp_block(int size);
};

extern "C++" AGPAllocator* agp_alloc;

#endif