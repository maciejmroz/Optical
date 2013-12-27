/***************************************************************************
                          glstuff.cpp  -  description
                             -------------------
    begin                : Fri May 5 2000
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/


#ifdef _WIN32
#include<windows.h>
#endif
#include<GL/gl.h>

#ifdef _WIN32
#include "glext.h"
#endif 

#include<stdio.h>
#include "debug_msg.h"
#include "glstuff.h"

/*** OPENGL INITIALIZATION AND CHECKS ***/

// Check if required extensions exist. 
#include <string.h>

static int isExtensionSupported(const char *extension) {
  const GLubyte *extensions = NULL;
  const GLubyte *start;
  GLubyte *where, *terminator;

// Extension names should not have spaces. 
  where = (GLubyte *) strchr(extension, ' ');
  if (where || *extension == '\0')
    return 0;
  extensions = glGetString(GL_EXTENSIONS);
  // It takes a bit of care to be fool-proof about parsing the
  //   OpenGL extensions string. Don't be fooled by sub-strings,
  //   etc. 
  start = extensions;
  for (;;) {
    where = (GLubyte *) strstr((const char *) start, extension);
    if (!where)
      break;
    terminator = where + strlen(extension);
    if (where == start || *(where - 1) == ' ')
      if (*terminator == ' ' || *terminator == '\0')
        return 1;
    start = terminator;
  }
  return 0;
}
 
//z ponizszych linii copy/paste
//iniclalizacja rozszerzen OpenGL
//

//w wyniku inicjalizacji odpowiednie wartosci sa zmieniane na 1
// ponizsze zmienne sa zadeklarowane jako extern w glstuff.h 
// i sa tym sposobem globalne dla programu
int max_texture_size=256;
GLenum default_min_filter=GL_LINEAR_MIPMAP_LINEAR;
GLenum default_mag_filter=GL_LINEAR;
int hasMultitexture=0;
unsigned int max_texture_units=1;
int hasTextureEnvAdd=0;
int hasTextureEnvCombine=0;
int hasBgra=0;
int hasPointParameters=0;
int hasNVTexgenReflection=0;
int hasAbgr=0;
int hasPackedPixels=0;
int hasCompiledVertexArray=0;
int hasStencilWrap=0;
int hasNVTextureEnvCombine4=0;
int hasFogCoord=0;
int hasRescaleNormal=0;
int hasSecondaryColor=0;
int hasSeparateSpecularColor=0;
int hasTextureLODBias=0;
int hasNVBlendSquare=0;
int hasVertexWeighting=0;
int hasNVFogDistance=0;
int hasNVLightMaxExponent=0;
int hasWglSwapControl=0;
int hasVertexArrayRange=0;
int hasCubeMaps=0;
int hasRegisterCombiners=0;
int max_cube_map_size=256;
int hasWglPixelFormat=0;
int hasPbuffers=0;
int hasTextureCompression=0;
int hasTextureEnvDot3=0;
int hasATIFragmentShader=0;
int hasATIPNTriangles=0;

//wskazniki do odpowiednich funkcji (prototypy w glext.h)
#ifdef _WIN32
// ARB_multitexture  
PFNGLMULTITEXCOORD1IARBPROC glMultiTexCoord1iARB=0;
PFNGLMULTITEXCOORD1IVARBPROC glMultiTexCoord1ivARB=0;
PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB=0;
PFNGLMULTITEXCOORD1FVARBPROC glMultiTexCoord1fvARB=0;
PFNGLMULTITEXCOORD1DARBPROC glMultiTexCoord1dARB=0;
PFNGLMULTITEXCOORD1DVARBPROC glMultiTexCoord1dvARB=0;
PFNGLMULTITEXCOORD1SARBPROC glMultiTexCoord1sARB=0;
PFNGLMULTITEXCOORD1SVARBPROC glMultiTexCoord1svARB=0;

PFNGLMULTITEXCOORD2IARBPROC glMultiTexCoord2iARB=0;
PFNGLMULTITEXCOORD2IVARBPROC glMultiTexCoord2ivARB=0;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB=0;
PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fvARB=0;
PFNGLMULTITEXCOORD2DARBPROC glMultiTexCoord2dARB=0;
PFNGLMULTITEXCOORD2DVARBPROC glMultiTexCoord2dvARB=0;
PFNGLMULTITEXCOORD2SARBPROC glMultiTexCoord2sARB=0;
PFNGLMULTITEXCOORD2SVARBPROC glMultiTexCoord2svARB=0;

PFNGLMULTITEXCOORD3IARBPROC glMultiTexCoord3iARB=0;
PFNGLMULTITEXCOORD3IVARBPROC glMultiTexCoord3ivARB=0;
PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB=0;
PFNGLMULTITEXCOORD3FVARBPROC glMultiTexCoord3fvARB=0;
PFNGLMULTITEXCOORD3DARBPROC glMultiTexCoord3dARB=0;
PFNGLMULTITEXCOORD3DVARBPROC glMultiTexCoord3dvARB=0;
PFNGLMULTITEXCOORD3SARBPROC glMultiTexCoord3sARB=0;
PFNGLMULTITEXCOORD3SVARBPROC glMultiTexCoord3svARB=0;

PFNGLMULTITEXCOORD4IARBPROC glMultiTexCoord4iARB=0;
PFNGLMULTITEXCOORD4IVARBPROC glMultiTexCoord4ivARB=0;
PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB=0;
PFNGLMULTITEXCOORD4FVARBPROC glMultiTexCoord4fvARB=0;
PFNGLMULTITEXCOORD4DARBPROC glMultiTexCoord4dARB=0;
PFNGLMULTITEXCOORD4DVARBPROC glMultiTexCoord4dvARB=0;
PFNGLMULTITEXCOORD4SARBPROC glMultiTexCoord4sARB=0;
PFNGLMULTITEXCOORD4SVARBPROC glMultiTexCoord4svARB=0;

PFNGLACTIVETEXTUREARBPROC glActiveTextureARB=0;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB=0;

//EXT_point_parameters
PFNGLPOINTPARAMETERFEXTPROC glPointParameterfEXT = 0;
PFNGLPOINTPARAMETERFVEXTPROC glPointParameterfvEXT = 0;
 
//EXT_compiled_vertex_array

PFNGLLOCKARRAYSEXTPROC glLockArraysEXT=0;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT=0;

//EXT_fog_coord
PFNGLFOGCOORDFEXTPROC glFogCoordfEXT=0;
PFNGLFOGCOORDDEXTPROC glFogCoorddEXT=0;
PFNGLFOGCOORDFVEXTPROC glFogCoordfvEXT=0;
PFNGLFOGCOORDDVEXTPROC glFogCoorddvEXT=0;
PFNGLFOGCOORDPOINTEREXTPROC glFogCoordPointerEXT=0;

//EXT_secondary_color
PFNGLSECONDARYCOLOR3BEXTPROC glSecondaryColor3bEXT=0;
PFNGLSECONDARYCOLOR3SEXTPROC glSecondaryColor3sEXT=0;
PFNGLSECONDARYCOLOR3IEXTPROC glSecondaryColor3iEXT=0;
PFNGLSECONDARYCOLOR3FEXTPROC glSecondaryColor3fEXT=0;
PFNGLSECONDARYCOLOR3DEXTPROC glSecondaryColor3dEXT=0;
PFNGLSECONDARYCOLOR3UBEXTPROC glSecondaryColor3ubEXT=0;
PFNGLSECONDARYCOLOR3USEXTPROC glSecondaryColor3usEXT=0;
PFNGLSECONDARYCOLOR3UIEXTPROC glSecondaryColor3uiEXT=0;
PFNGLSECONDARYCOLOR3BVEXTPROC glSecondaryColor3bvEXT=0;
PFNGLSECONDARYCOLOR3SVEXTPROC glSecondaryColor3svEXT=0;
PFNGLSECONDARYCOLOR3IVEXTPROC glSecondaryColor3ivEXT=0;
PFNGLSECONDARYCOLOR3FVEXTPROC glSecondaryColor3fvEXT=0;
PFNGLSECONDARYCOLOR3DVEXTPROC glSecondaryColor3dvEXT=0;
PFNGLSECONDARYCOLOR3UBVEXTPROC glSecondaryColor3ubvEXT=0;
PFNGLSECONDARYCOLOR3USVEXTPROC glSecondaryColor3usvEXT=0;
PFNGLSECONDARYCOLOR3UIVEXTPROC glSecondaryColor3uivEXT=0;
PFNGLSECONDARYCOLORPOINTEREXTPROC glSecondaryColorPointerEXT=0;

//EXT_wertex_weighting
PFNGLVERTEXWEIGHTFEXTPROC glVertexWeightfEXT=0;
PFNGLVERTEXWEIGHTFVEXTPROC glVertexWeightfvEXT=0;
PFNGLVERTEXWEIGHTPOINTEREXTPROC glVertexWeightPointerEXT=0;

//WGL_EXT_swap_control
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT=0;
PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT=0;

//NV_vertex_array_range
PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV=0;
PFNGLFLUSHVERTEXARRAYRANGENVPROC glFlushVertexArrayRangeNV=0;
PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV=0;
PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV=0;

//NV_register_combiners
PFNGLCOMBINERPARAMETERFVNVPROC glCombinerParameterfvNV=0;
PFNGLCOMBINERPARAMETERIVNVPROC glCombinerParameterivNV=0;
PFNGLCOMBINERPARAMETERFNVPROC glCombinerParameterfNV=0;
PFNGLCOMBINERPARAMETERINVPROC glCombinerParameteriNV=0;
PFNGLCOMBINERINPUTNVPROC glCombinerInputNV=0;
PFNGLCOMBINEROUTPUTNVPROC glCombinerOutputNV=0;
PFNGLFINALCOMBINERINPUTNVPROC glFinalCombinerInputNV=0;
PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC glGetCombinerInputParameterfvNV=0;
PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC glGetCombinerInputParameterivNV=0;
PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC glGetCombinerOutputParameterfvNV=0;
PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC glGetCombinerOutputParameterivNV=0;
PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV=0;
PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV=0;

PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB=0;

//WGL_ARB_pixel_format
PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB=0;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB=0;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB=0;

//WGL_ARB_pbuffer
PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB=0;
PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB=0;
PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB=0;
PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB=0;
PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB=0;

//ARB_texture_compression
PFNGLCOMPRESSEDTEXIMAGE3DARBPROC glCompressedTexImage3DARB=0;
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB=0;
PFNGLCOMPRESSEDTEXIMAGE1DARBPROC glCompressedTexImage1DARB=0;
PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC glCompressedTexSubImage3DARB=0;
PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC glCompressedTexSubImage2DARB=0;
PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC glCompressedTexSubImage1DARB=0;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC glGetCompressedTexImageARB=0;

//ATI_fragment_shader
PFNGLGENFRAGMENTSHADERSATIPROC		glGenFragmentShadersATI=0;
PFNGLBINDFRAGMENTSHADERATIPROC		glBindFragmentShaderATI=0;
PFNGLDELETEFRAGMENTSHADERATIPROC	glDeleteFragmentShaderATI=0;
PFNGLBEGINFRAGMENTSHADERATIPROC	glBeginFragmentShaderATI=0;
PFNGLENDFRAGMENTSHADERATIPROC		glEndFragmentShaderATI=0;
PFNGLPASSTEXCOORDATIPROC			glPassTexCoordATI=0;
PFNGLSAMPLEMAPATIPROC				glSampleMapATI=0;
PFNGLCOLORFRAGMENTOP1ATIPROC		glColorFragmentOp1ATI=0;
PFNGLCOLORFRAGMENTOP2ATIPROC		glColorFragmentOp2ATI=0;
PFNGLCOLORFRAGMENTOP3ATIPROC		glColorFragmentOp3ATI=0;
PFNGLALPHAFRAGMENTOP1ATIPROC		glAlphaFragmentOp1ATI=0;
PFNGLALPHAFRAGMENTOP2ATIPROC		glAlphaFragmentOp2ATI=0;
PFNGLALPHAFRAGMENTOP3ATIPROC		glAlphaFragmentOp3ATI=0;
PFNGLSETFRAGMENTSHADERCONSTANTATIPROC	glSetFragmentShaderConstantATI=0;

//ATI_pn_triangles
PFNGLPNTRIANGLESIATIPROC	glPNTrianglesiATI=0;
PFNGLPNTRIANGLESFATIPROC	glPNTrianglesfATI=0;

#endif

//extension initialization

extern HDC hDC;

static int isWglExtensionSupported(const char *extension) {
  const GLubyte *extensions = NULL;
  const GLubyte *start;
  GLubyte *where, *terminator;

// Extension names should not have spaces. 
  where = (GLubyte *) strchr(extension, ' ');
  if (where || *extension == '\0')
    return 0;
  extensions = (const unsigned char*)wglGetExtensionsStringARB(hDC);
  // It takes a bit of care to be fool-proof about parsing the
  //   OpenGL extensions string. Don't be fooled by sub-strings,
  //   etc. 
  start = extensions;
  for (;;) {
    where = (GLubyte *) strstr((const char *) start, extension);
    if (!where)
      break;
    terminator = where + strlen(extension);
    if (where == start || *(where - 1) == ' ')
      if (*terminator == ' ' || *terminator == '\0')
        return 1;
    start = terminator;
  }
  return 0;
}

void InitWglExtensions() {
	wglGetExtensionsStringARB=(PFNWGLGETEXTENSIONSSTRINGARBPROC)
		wglGetProcAddress("wglGetExtensionsStringARB");
	if(wglGetExtensionsStringARB) {
		PrintfBox(wglGetExtensionsStringARB(hDC));
		if(isWglExtensionSupported("WGL_ARB_pixel_format")) {
			hasWglPixelFormat=1;
			wglGetPixelFormatAttribivARB=(PFNWGLGETPIXELFORMATATTRIBIVARBPROC)
				wglGetProcAddress("wglGetPixelFormatAttribivARB");
			if(!wglGetPixelFormatAttribivARB) {
				PrintfBox("wglGetPixelFormatAttribivARB==NULL!!!");
				hasWglPixelFormat=0;
			}
			wglGetPixelFormatAttribfvARB=(PFNWGLGETPIXELFORMATATTRIBFVARBPROC)
				wglGetProcAddress("wglGetPixelFormatAttribfvARB");
			if(!wglGetPixelFormatAttribfvARB) {
				PrintfBox("wglGetPixelFormatAttribfvARB==NULL");
				hasWglPixelFormat=0;
			}
			wglChoosePixelFormatARB=(PFNWGLCHOOSEPIXELFORMATARBPROC)
				wglGetProcAddress("wglChoosePixelFormatARB");
			if(!wglChoosePixelFormatARB) {
				PrintfBox("wglChoosePixelFormatARB==NULL!!!");
				hasWglPixelFormat=0;
			}
		}
		if(isWglExtensionSupported("WGL_ARB_pbuffer")) {
			hasPbuffers=1;
			wglCreatePbufferARB=(PFNWGLCREATEPBUFFERARBPROC)
				wglGetProcAddress("wglCreatePbufferARB");
			if(!wglCreatePbufferARB) {
				PrintfBox("wglCreatePbufferARB==NULL!!!");
				hasPbuffers=0;
			}
			wglGetPbufferDCARB=(PFNWGLGETPBUFFERDCARBPROC)
				wglGetProcAddress("wglGetPbufferDCARB");
			if(!wglGetPbufferDCARB) {
				PrintfBox("wglGetPbufferDCARB==NULL!!!");
				hasPbuffers=0;
			}
			wglReleasePbufferDCARB=(PFNWGLRELEASEPBUFFERDCARBPROC)
				wglGetProcAddress("wglReleasePbufferDCARB");
			if(!wglReleasePbufferDCARB) {
				PrintfBox("wglReleasePbufferDCARB==NULL");
				hasPbuffers=0;
			}
			wglDestroyPbufferARB=(PFNWGLDESTROYPBUFFERARBPROC)
				wglGetProcAddress("wglDestroyPbufferARB");
			if(!wglDestroyPbufferARB) {
				PrintfBox("wglDestroyPbufferARB==NULL!!!");
				hasPbuffers=0;
			}
			wglQueryPbufferARB=(PFNWGLQUERYPBUFFERARBPROC)
				wglGetProcAddress("wglQueryPbufferARB");
			if(!wglQueryPbufferARB) {
				PrintfBox("wglQueryPbufferARB==NULL!!!");
				hasPbuffers=0;
			}
		}
	}
}

void InitExtensions(void) {
#ifdef _LOG_EVERYTHING
	DebugMessage("Initializing OpenGL extensions - see extensions.log\n");
#endif
  PrintfBox((const char*)glGetString(GL_EXTENSIONS));
  if(isExtensionSupported("GL_ARB_multitexture")) {
	hasMultitexture=1;
	int tmp;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&tmp);
	max_texture_units=(unsigned int)(tmp);
  }
  glGetIntegerv(GL_MAX_TEXTURE_SIZE,&max_texture_size);
  if(isExtensionSupported("GL_EXT_bgra")) {
	hasBgra=1;
  }
  if(isExtensionSupported("GL_EXT_texture_env_add")) {
    hasTextureEnvAdd = 1;
  }
  if(isExtensionSupported("GL_EXT_texture_env_combine")) {
    hasTextureEnvCombine = 1;
  }
  if(isExtensionSupported("GL_EXT_point_parameters")) {
	hasPointParameters=1;
  }
  if(isExtensionSupported("GL_NV_texgen_reflection")) {
	hasNVTexgenReflection=1;
  }
  if(isExtensionSupported("GL_EXT_abgr")) {
	hasAbgr=1;
  }
  if(isExtensionSupported("GL_EXT_packed_pixels")) {
	hasPackedPixels=1;
  }
  if(isExtensionSupported("GL_EXT_compiled_vertex_array")) {
	hasCompiledVertexArray=1;
  }
  if(isExtensionSupported("GL_EXT_stencil_wrap")) {
	hasStencilWrap=1;
  }
  if(isExtensionSupported("GL_NV_texture_env_combine4")) {
	hasNVTextureEnvCombine4=1;
  }
  if(isExtensionSupported("GL_EXT_fog_coord")) {
	hasFogCoord=1;
  }
  if(isExtensionSupported("GL_EXT_rescale_normal")) {
	hasRescaleNormal=1;
  }
  if(isExtensionSupported("GL_EXT_secondary_color")) {
	hasSecondaryColor=1;
  }
  if(isExtensionSupported("GL_EXT_separate_specular_color")) {
	hasSeparateSpecularColor=1;
  }
  if(isExtensionSupported("GL_EXT_texture_lod_bias")) {
	hasTextureLODBias=1;
  }
  if(isExtensionSupported("GL_NV_blend_square")) {
	hasNVBlendSquare=1;
  }
  if(isExtensionSupported("GL_EXT_vertex_weighting")) {
	hasVertexWeighting=1;
  }
  if(isExtensionSupported("GL_NV_fog_distance")) {
	hasNVFogDistance=1;
  }
  if(isExtensionSupported("GL_NV_light_max_exponent")) {
	hasNVLightMaxExponent=1;
  }
  if(isExtensionSupported("WGL_EXT_swap_control")) {
	hasWglSwapControl=1;
  }
  if(isExtensionSupported("GL_NV_vertex_array_range")) {
	  hasVertexArrayRange=1;
  }
  if(isExtensionSupported("GL_ARB_texture_cube_map")) {
	hasCubeMaps=1;
	max_cube_map_size=max_texture_size;
	int tmp;
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB,&tmp);
	if(tmp<max_cube_map_size) max_cube_map_size=tmp;
  }
  if(isExtensionSupported("GL_NV_register_combiners")) {
	  hasRegisterCombiners=1;
  }
  if(isExtensionSupported("GL_ARB_texture_compression")) {
	  hasTextureCompression=1;
  }
  if(isExtensionSupported("GL_ARB_texture_env_dot3")) {
	  hasTextureEnvDot3=1;
  }
  if(isExtensionSupported("GL_ATI_fragment_shader")) {
	  hasATIFragmentShader=1;
  }
  if(isExtensionSupported("GL_ATI_pn_triangles")) {
	  hasATIPNTriangles=1;
  }


#ifdef _LOG_EVERYTHING

DebugFileMessage("extensions.log","ARB_multitexture:\t\t\t");
if(hasMultitexture) DebugFileMessage("extensions.log","found (%d texture units)\n",max_texture_units);
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","EXT_bgra:\t\t\t\t");
if(hasBgra) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","EXT_texture_env_add:\t\t\t");
if(hasTextureEnvAdd) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","EXT_texture_env_combine:\t\t");
if(hasTextureEnvCombine) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","EXT_point_parameters:\t\t\t");
if(hasPointParameters) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","NV_texgen_reflection:\t\t\t");
if(hasNVTexgenReflection) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","EXT_abgr:\t\t\t\t");
if(hasAbgr) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","EXT_packed_pixels:\t\t\t");
if(hasPackedPixels) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","EXT_compiled_vertex_array:\t\t");
if(hasCompiledVertexArray) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","EXT_stencil_wrap:\t\t\t");
if(hasStencilWrap) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","NV_texture_env_combine4:\t\t");
if(hasNVTextureEnvCombine4) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","EXT_fog_coord:\t\t\t\t");
if(hasFogCoord) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","EXT_rescale_normal:\t\t\t");
if(hasRescaleNormal) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","EXT_secondary_color:\t\t\t");
if(hasSecondaryColor) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","EXT_separate_specular_color:\t\t");
if(hasSeparateSpecularColor) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","EXT_texture_lod_bias:\t\t\t");
if(hasTextureLODBias) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","NV_blend_square:\t\t\t");
if(hasNVBlendSquare) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","EXT_vertex_weighting:\t\t\t");
if(hasVertexWeighting) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","NV_fog_distance:\t\t\t");
if(hasNVFogDistance) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","NV_light_max_exponent:\t\t\t");
if(hasNVLightMaxExponent) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","WGL_EXT_swap_control:\t\t\t");
if(hasWglSwapControl) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","GL_NV_vertex_array_range:\t\t");
if(hasVertexArrayRange) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","GL_ARB_texture_cube_map:\t\t");
if(hasCubeMaps) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","GL_NV_register_combiners:\t\t");
if(hasRegisterCombiners) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","GL_ARB_texture_compression:\t\t");
if(hasTextureCompression) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","GL_ARB_texture_env_dot3:\t\t");
if(hasTextureEnvDot3) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","GL_ATI_fragment_shader:\t\t");
if(hasATIFragmentShader) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

DebugFileMessage("extensions.log","GL_ATI_pn_triangles:\t\t");
if(hasATIPNTriangles) DebugFileMessage("extensions.log","found\n");
else DebugFileMessage("extensions.log","not found\n");

#endif

#ifdef WIN32
  if(hasMultitexture) {
  // Retrieve ARB_multitexture routines. 
  glMultiTexCoord1iARB =
    (PFNGLMULTITEXCOORD1IARBPROC) wglGetProcAddress("glMultiTexCoord1iARB");
  glMultiTexCoord1ivARB =
    (PFNGLMULTITEXCOORD1IVARBPROC) wglGetProcAddress("glMultiTexCoord1ivARB");
  glMultiTexCoord1fARB =
    (PFNGLMULTITEXCOORD1FARBPROC) wglGetProcAddress("glMultiTexCoord1fARB");
  glMultiTexCoord1fvARB =
    (PFNGLMULTITEXCOORD1FVARBPROC) wglGetProcAddress("glMultiTexCoord1fvARB");
  glMultiTexCoord1dARB =
    (PFNGLMULTITEXCOORD1DARBPROC) wglGetProcAddress("glMultiTexCoord1dARB");
  glMultiTexCoord1dvARB =
    (PFNGLMULTITEXCOORD1DVARBPROC) wglGetProcAddress("glMultiTexCoord1dvARB");
  glMultiTexCoord1sARB =
    (PFNGLMULTITEXCOORD1SARBPROC) wglGetProcAddress("glMultiTexCoord1sARB");
  glMultiTexCoord1svARB =
    (PFNGLMULTITEXCOORD1SVARBPROC) wglGetProcAddress("glMultiTexCoord1svARB");

  glMultiTexCoord2iARB =
    (PFNGLMULTITEXCOORD2IARBPROC) wglGetProcAddress("glMultiTexCoord2iARB");
  glMultiTexCoord2ivARB =
    (PFNGLMULTITEXCOORD2IVARBPROC) wglGetProcAddress("glMultiTexCoord2ivARB");
  glMultiTexCoord2fARB =
    (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
  glMultiTexCoord2fvARB =
    (PFNGLMULTITEXCOORD2FVARBPROC) wglGetProcAddress("glMultiTexCoord2fvARB");
  glMultiTexCoord2dARB =
    (PFNGLMULTITEXCOORD2DARBPROC) wglGetProcAddress("glMultiTexCoord2dARB");
  glMultiTexCoord2dvARB =
    (PFNGLMULTITEXCOORD2DVARBPROC) wglGetProcAddress("glMultiTexCoord2dvARB");
  glMultiTexCoord2sARB =
    (PFNGLMULTITEXCOORD2SARBPROC) wglGetProcAddress("glMultiTexCoord2sARB");
  glMultiTexCoord2svARB =
    (PFNGLMULTITEXCOORD2SVARBPROC) wglGetProcAddress("glMultiTexCoord2svARB");

  glMultiTexCoord3iARB =
    (PFNGLMULTITEXCOORD3IARBPROC) wglGetProcAddress("glMultiTexCoord3iARB");
  glMultiTexCoord3ivARB =
    (PFNGLMULTITEXCOORD3IVARBPROC) wglGetProcAddress("glMultiTexCoord3ivARB");
  glMultiTexCoord3fARB =
    (PFNGLMULTITEXCOORD3FARBPROC) wglGetProcAddress("glMultiTexCoord3fARB");
  glMultiTexCoord3fvARB =
    (PFNGLMULTITEXCOORD3FVARBPROC) wglGetProcAddress("glMultiTexCoord3fvARB");
  glMultiTexCoord3dARB =
    (PFNGLMULTITEXCOORD3DARBPROC) wglGetProcAddress("glMultiTexCoord3dARB");
  glMultiTexCoord3dvARB =
    (PFNGLMULTITEXCOORD3DVARBPROC) wglGetProcAddress("glMultiTexCoord3dvARB");
  glMultiTexCoord3sARB =
    (PFNGLMULTITEXCOORD3SARBPROC) wglGetProcAddress("glMultiTexCoord3sARB");
  glMultiTexCoord3svARB =
    (PFNGLMULTITEXCOORD3SVARBPROC) wglGetProcAddress("glMultiTexCoord3svARB");
  
  glMultiTexCoord4iARB =
    (PFNGLMULTITEXCOORD4IARBPROC) wglGetProcAddress("glMultiTexCoord4iARB");
  glMultiTexCoord4ivARB =
    (PFNGLMULTITEXCOORD4IVARBPROC) wglGetProcAddress("glMultiTexCoord4ivARB");
  glMultiTexCoord4fARB =
    (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");
  glMultiTexCoord4fvARB =
    (PFNGLMULTITEXCOORD4FVARBPROC) wglGetProcAddress("glMultiTexCoord4fvARB");
  glMultiTexCoord4dARB =
    (PFNGLMULTITEXCOORD4DARBPROC) wglGetProcAddress("glMultiTexCoord4dARB");
  glMultiTexCoord4dvARB =
    (PFNGLMULTITEXCOORD4DVARBPROC) wglGetProcAddress("glMultiTexCoord4dvARB");
  glMultiTexCoord4sARB =
    (PFNGLMULTITEXCOORD4SARBPROC) wglGetProcAddress("glMultiTexCoord4sARB");
  glMultiTexCoord4svARB =
    (PFNGLMULTITEXCOORD4SVARBPROC) wglGetProcAddress("glMultiTexCoord4svARB");

  glActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");

  glClientActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
 
  }

  if(hasPointParameters) {
   glPointParameterfEXT = (PFNGLPOINTPARAMETERFEXTPROC) wglGetProcAddress("glPointParameterfEXT");
   glPointParameterfvEXT = (PFNGLPOINTPARAMETERFVEXTPROC) wglGetProcAddress("glPointParameterfvEXT");
  }

  if(hasCompiledVertexArray) {
   glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC) wglGetProcAddress("glLockArraysEXT");
   glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC) wglGetProcAddress("glUnlockArraysEXT");
  }

  if(hasFogCoord) {
	glFogCoordfEXT = (PFNGLFOGCOORDFEXTPROC) wglGetProcAddress("glFogCoordfEXT");
	glFogCoorddEXT = (PFNGLFOGCOORDDEXTPROC) wglGetProcAddress("glFogCoorddEXT");
	glFogCoordfvEXT = (PFNGLFOGCOORDFVEXTPROC) wglGetProcAddress("glFogCoordfvEXT");
	glFogCoorddvEXT = (PFNGLFOGCOORDDVEXTPROC) wglGetProcAddress("glFogCoorddvEXT");
	glFogCoordPointerEXT = (PFNGLFOGCOORDPOINTEREXTPROC) wglGetProcAddress("glFogCoordPointerEXT");
  }

  if(hasSecondaryColor) {
	glSecondaryColor3bEXT = 
		(PFNGLSECONDARYCOLOR3BEXTPROC) wglGetProcAddress("glSecondaryColor3bEXT");
	glSecondaryColor3sEXT = 
		(PFNGLSECONDARYCOLOR3SEXTPROC) wglGetProcAddress("glSecondaryColor3sEXT");
	glSecondaryColor3iEXT = 
		(PFNGLSECONDARYCOLOR3IEXTPROC) wglGetProcAddress("glSecondaryColor3iEXT");
	glSecondaryColor3fEXT =
		(PFNGLSECONDARYCOLOR3FEXTPROC) wglGetProcAddress("glSecondaryColor3fEXT");
	glSecondaryColor3dEXT =
		(PFNGLSECONDARYCOLOR3DEXTPROC) wglGetProcAddress("glSecondaryColor3dEXT");
	glSecondaryColor3ubEXT = 
		(PFNGLSECONDARYCOLOR3UBEXTPROC) wglGetProcAddress("glSecondaryColor3ubEXT");
	glSecondaryColor3usEXT = 
		(PFNGLSECONDARYCOLOR3USEXTPROC) wglGetProcAddress("glSecondaryColor3usEXT");
	glSecondaryColor3uiEXT = 
		(PFNGLSECONDARYCOLOR3UIEXTPROC) wglGetProcAddress("glSecondaryColor3uiEXT");
	glSecondaryColor3bvEXT = 
		(PFNGLSECONDARYCOLOR3BVEXTPROC) wglGetProcAddress("glSecondaryColor3bvEXT");
	glSecondaryColor3svEXT = 
		(PFNGLSECONDARYCOLOR3SVEXTPROC) wglGetProcAddress("glSecondaryColor3svEXT");
	glSecondaryColor3ivEXT = 
		(PFNGLSECONDARYCOLOR3IVEXTPROC) wglGetProcAddress("glSecondaryColor3ivEXT");
	glSecondaryColor3fvEXT =
		(PFNGLSECONDARYCOLOR3FVEXTPROC) wglGetProcAddress("glSecondaryColor3fvEXT");
	glSecondaryColor3dvEXT =
		(PFNGLSECONDARYCOLOR3DVEXTPROC) wglGetProcAddress("glSecondaryColor3dvEXT");
	glSecondaryColor3ubvEXT = 
		(PFNGLSECONDARYCOLOR3UBVEXTPROC) wglGetProcAddress("glSecondaryColor3ubvEXT");
	glSecondaryColor3usvEXT = 
		(PFNGLSECONDARYCOLOR3USVEXTPROC) wglGetProcAddress("glSecondaryColor3usvEXT");
	glSecondaryColor3uivEXT = 
		(PFNGLSECONDARYCOLOR3UIVEXTPROC) wglGetProcAddress("glSecondaryColor3uivEXT");
	glSecondaryColorPointerEXT =
		(PFNGLSECONDARYCOLORPOINTEREXTPROC) wglGetProcAddress("glSecondaryColorPointerEXT");
  }

  if(hasVertexWeighting) {
	glVertexWeightfEXT = (PFNGLVERTEXWEIGHTFEXTPROC) wglGetProcAddress("glVertexWeightfEXT");
	glVertexWeightfvEXT = (PFNGLVERTEXWEIGHTFVEXTPROC) wglGetProcAddress("glVertexWeightfvEXT");
	glVertexWeightPointerEXT = 
		(PFNGLVERTEXWEIGHTPOINTEREXTPROC) wglGetProcAddress("glVertexWeightPointerEXT");

  }
  if(hasWglSwapControl) {
	  wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
	  wglGetSwapIntervalEXT = 
		  (PFNWGLGETSWAPINTERVALEXTPROC) wglGetProcAddress("wglGetSwapIntervalEXT");
  }

  if(hasVertexArrayRange) {
	glVertexArrayRangeNV= (PFNGLVERTEXARRAYRANGENVPROC) wglGetProcAddress("glVertexArrayRangeNV");
	glFlushVertexArrayRangeNV= (PFNGLFLUSHVERTEXARRAYRANGENVPROC) 
		wglGetProcAddress("glFlushVertexArrayRangeNV");
	wglAllocateMemoryNV= (PFNWGLALLOCATEMEMORYNVPROC) wglGetProcAddress("wglAllocateMemoryNV");
	wglFreeMemoryNV= (PFNWGLFREEMEMORYNVPROC) wglGetProcAddress("wglFreeMemoryNV");
	if(!glVertexArrayRangeNV) PrintfBox("glVertexArrayRangeNV==NULL!!!!");
	if(!glFlushVertexArrayRangeNV) PrintfBox("glFlushVertexArrayRangeNV==NULL!!!!");
	if(!wglAllocateMemoryNV) PrintfBox("wglAllocateMemoryNV==NULL!!!!");
	if(!wglFreeMemoryNV) PrintfBox("wglFreeMemoryNV==NULL!!!!");
  }

  if(hasRegisterCombiners) {
	glCombinerParameterfvNV= 
		(PFNGLCOMBINERPARAMETERFVNVPROC) wglGetProcAddress("glCombinerParameterfvNV");
	if(!glCombinerParameterfvNV) PrintfBox("glCombinerParameterfvNV=NULL!!!");
	glCombinerParameterivNV= 
		(PFNGLCOMBINERPARAMETERIVNVPROC) wglGetProcAddress("glCombinerParameterivNV");
	if(!glCombinerParameterivNV) PrintfBox("glCombinerParameterivNV==NULL!!!");
	glCombinerParameterfNV=
		(PFNGLCOMBINERPARAMETERFNVPROC) wglGetProcAddress("glCombinerParameterfNV");
	if(!glCombinerParameterfNV) PrintfBox("glCombinerParameterfNV==NULL!!!");
	glCombinerParameteriNV=
		(PFNGLCOMBINERPARAMETERINVPROC) wglGetProcAddress("glCombinerParameteriNV");
	if(!glCombinerParameteriNV) PrintfBox("glCombinerParameteriNV==NULL!!!");
	glCombinerInputNV= (PFNGLCOMBINERINPUTNVPROC)
		wglGetProcAddress("glCombinerInputNV");
	if(!glCombinerInputNV) PrintfBox("glCombinerInputNV==NULL!!!");
	glCombinerOutputNV=(PFNGLCOMBINEROUTPUTNVPROC)
		wglGetProcAddress("glCombinerOutputNV");
	if(!glCombinerOutputNV) PrintfBox("glCombinerOutputNV==NULL!!!");
	glFinalCombinerInputNV=(PFNGLFINALCOMBINERINPUTNVPROC)
		wglGetProcAddress("glFinalCombinerInputNV");
	if(!glFinalCombinerInputNV) PrintfBox("glFinalCombinerInputNV==NULL!!!");
	glGetCombinerInputParameterfvNV=(PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC)
		wglGetProcAddress("glGetCombinerInputParameterfvNV");
	if(!glGetCombinerInputParameterfvNV) PrintfBox("glGetCombinerInputParameterfvNV==NULL!!!");
	glGetCombinerInputParameterivNV=(PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC)
		wglGetProcAddress("glGetCombinerInputParameterivNV");
	if(!glGetCombinerInputParameterivNV) PrintfBox("glGetCombinerInputParameterivNV==NULL!!!");
	glGetCombinerOutputParameterfvNV=(PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC)
		wglGetProcAddress("glGetCombinerOutputParameterfvNV");
	if(!glGetCombinerOutputParameterfvNV)
		PrintfBox("glGetCombinerOutputParameterfvNV==NULL!!!");
	glGetCombinerOutputParameterivNV=(PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC)
		wglGetProcAddress("glGetCombinerOutputParameterivNV");
	if(!glGetCombinerOutputParameterivNV)
		PrintfBox("glGetCombinerOutputParameterivNV==NULL!!!");
	glGetFinalCombinerInputParameterfvNV=(PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC)
		wglGetProcAddress("glGetFinalCombinerInputParameterfvNV");
	if(!glGetFinalCombinerInputParameterfvNV)
		PrintfBox("glGetFinalCombinerInputParameterfvNV==NULL!!!");
	glGetFinalCombinerInputParameterivNV=(PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC)
		wglGetProcAddress("glGetFinalCombinerInputParameterivNV");
	if(!glGetFinalCombinerInputParameterivNV)
		PrintfBox("glGetFinalCombinerInputParameterivNV==NULL!!!");
  }
  if(hasTextureCompression) {
	glCompressedTexImage3DARB= 
		(PFNGLCOMPRESSEDTEXIMAGE3DARBPROC) wglGetProcAddress("glCompressedTexImage3DARB");
	if(!glCompressedTexImage3DARB) PrintfBox("glCompressedTexImage3DARB=NULL!!!");
	glCompressedTexImage2DARB= 
		(PFNGLCOMPRESSEDTEXIMAGE2DARBPROC) wglGetProcAddress("glCompressedTexImage2DARB");
	if(!glCompressedTexImage2DARB) PrintfBox("glCompressedTexImage2DARB=NULL!!!");
	glCompressedTexImage1DARB= 
		(PFNGLCOMPRESSEDTEXIMAGE1DARBPROC) wglGetProcAddress("glCompressedTexImage1DARB");
	if(!glCompressedTexImage1DARB) PrintfBox("glCompressedTexImage1DARB=NULL!!!");
	glCompressedTexSubImage3DARB= 
		(PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC) wglGetProcAddress("glCompressedTexSubImage3DARB");
	if(!glCompressedTexSubImage3DARB) PrintfBox("glCompressedTexSubImage3DARB=NULL!!!");
	glCompressedTexSubImage2DARB= 
		(PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC) wglGetProcAddress("glCompressedTexSubImage2DARB");
	if(!glCompressedTexSubImage2DARB) PrintfBox("glCompressedTexSubImage2DARB=NULL!!!");
	glCompressedTexSubImage1DARB= 
		(PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC) wglGetProcAddress("glCompressedTexSubImage1DARB");
	if(!glCompressedTexSubImage1DARB) PrintfBox("glCompressedTexSubImage1DARB=NULL!!!");
	glGetCompressedTexImageARB= 
		(PFNGLGETCOMPRESSEDTEXIMAGEARBPROC) wglGetProcAddress("glGetCompressedTexImageARB");
	if(!glGetCompressedTexImageARB) PrintfBox("glGetCompressedTexImageARB=NULL!!!");
  }
  if(hasATIFragmentShader) {
	  glGenFragmentShadersATI=
		  (PFNGLGENFRAGMENTSHADERSATIPROC)wglGetProcAddress("glGenFragmentShadersATI");
	  if(!glGenFragmentShadersATI) PrintfBox("glGenFragmentShadersATI==NULL!!!");
	  glBindFragmentShaderATI=
		  (PFNGLBINDFRAGMENTSHADERATIPROC)wglGetProcAddress("glBindFragmentShaderATI");
	  if(!glBindFragmentShaderATI) PrintfBox("glBindFragmentShaderATI==NULL!!!");
	  glDeleteFragmentShaderATI=
		  (PFNGLDELETEFRAGMENTSHADERATIPROC)wglGetProcAddress("glDeleteFragmentShaderATI");
	  if(!glDeleteFragmentShaderATI) PrintfBox("glDeleteFragmentShaderATI==NULL!!!");
	  glBeginFragmentShaderATI=
		  (PFNGLBEGINFRAGMENTSHADERATIPROC)wglGetProcAddress("glBeginFragmentShaderATI");
	  if(!glBeginFragmentShaderATI) PrintfBox("glBeginFragmentShaderATI==NULL!!!");
	  glEndFragmentShaderATI=
		  (PFNGLENDFRAGMENTSHADERATIPROC)wglGetProcAddress("glEndFragmentShaderATI");
	  if(!glEndFragmentShaderATI) PrintfBox("glEndFragmentShaderATI==NULL!!!");
	  glPassTexCoordATI=
		  (PFNGLPASSTEXCOORDATIPROC)wglGetProcAddress("glPassTexCoordATI");
	  if(!glPassTexCoordATI) PrintfBox("glPassTexCoordATI==NULL!!!");
	  glSampleMapATI=
		  (PFNGLSAMPLEMAPATIPROC)wglGetProcAddress("glSampleMapATI");
	  if(!glSampleMapATI) PrintfBox("glSampleMapATI==NULL!!!");
	  glColorFragmentOp1ATI=
		  (PFNGLCOLORFRAGMENTOP1ATIPROC)wglGetProcAddress("glColorFragmentOp1ATI");
	  if(!glColorFragmentOp1ATI) PrintfBox("glColorFragmentOp1ATI==NULL!!!");
	  glColorFragmentOp2ATI=
		  (PFNGLCOLORFRAGMENTOP2ATIPROC)wglGetProcAddress("glColorFragmentOp2ATI");
	  if(!glColorFragmentOp2ATI) PrintfBox("glColorFragmentOp2ATI==NULL!!!");
	  glColorFragmentOp3ATI=
		  (PFNGLCOLORFRAGMENTOP3ATIPROC)wglGetProcAddress("glColorFragmentOp3ATI");
	  if(!glColorFragmentOp3ATI) PrintfBox("glColorFragmentOp3ATI==NULL!!!");
	  glAlphaFragmentOp1ATI=
		  (PFNGLALPHAFRAGMENTOP1ATIPROC)wglGetProcAddress("glAlphaFragmentOp1ATI");
	  if(!glAlphaFragmentOp1ATI) PrintfBox("glAlphaFragmentOp1ATI==NULL!!!");
	  glAlphaFragmentOp2ATI=
		  (PFNGLALPHAFRAGMENTOP2ATIPROC)wglGetProcAddress("glAlphaFragmentOp2ATI");
	  if(!glAlphaFragmentOp2ATI) PrintfBox("glAlphaFragmentOp2ATI==NULL!!!");
	  glAlphaFragmentOp3ATI=
		  (PFNGLALPHAFRAGMENTOP3ATIPROC)wglGetProcAddress("glAlphaFragmentOp3ATI");
	  if(!glAlphaFragmentOp3ATI) PrintfBox("glAlphaFragmentOp3ATI==NULL!!!");
	  glSetFragmentShaderConstantATI=
		  (PFNGLSETFRAGMENTSHADERCONSTANTATIPROC)wglGetProcAddress("glSetFragmentShaderConstantATI");
	  if(!glSetFragmentShaderConstantATI) PrintfBox("glSetFragmentShaderConstantATI==NULL!!!");
  }
  if(hasATIPNTriangles) {
	  glPNTrianglesiATI=(PFNGLPNTRIANGLESIATIPROC)wglGetProcAddress("glPNTrianglesiATI");
	  if(!glPNTrianglesiATI) {
		  PrintfBox("glPNTrianglesiATI=NULL!!!");
	  }
	  glPNTrianglesfATI=(PFNGLPNTRIANGLESFATIPROC)wglGetProcAddress("glPNTrianglesfATI");
	  if(!glPNTrianglesfATI) {
		  PrintfBox("glPNTrianglesfATI=NULL!!!");
	  }
  }
  
#endif
}

extern "C++" HDC hDC;
extern "C++" HGLRC hRC;
extern "C++" HDC pbufferDC;
extern "C++" HGLRC pbufferRC;

AGPAllocator::AGPAllocator(int size) {
	if(!hasVertexArrayRange) return;
	block_size=size-(size%32);
	agp_mem=wglAllocateMemoryNV(block_size,0.0f,0.0f,0.5f); //AGP priority
//	if(!agp_mem) {
//		agp_mem=wglAllocateMemoryNV(block_size,0.0f,0.0f,0.5f); //AGP priority
//	}
	if(!agp_mem) {
		PrintfBox("Could not allocate any fast memory!");
		return;
	}
	glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	if(pbufferDC && pbufferRC) {
		wglMakeCurrent(pbufferDC,pbufferRC);
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
		wglMakeCurrent(hDC,hRC);
	}
	glVertexArrayRangeNV(block_size,agp_mem);
	mem_top=agp_mem;
}

AGPAllocator::~AGPAllocator() {
	if(hasVertexArrayRange)	{
		if(agp_mem) wglFreeMemoryNV(agp_mem);
	}
}

void* AGPAllocator::get_agp_block(int size) {
	if(!hasVertexArrayRange) return NULL;
	unsigned int new_size;
	if(size%32 !=0) {
		new_size=(size/32)*32+32;
	}
	else {
		new_size=size;
	}
	if(!agp_mem) return NULL;
	if(block_size-((char*)mem_top-(char*)agp_mem)<new_size) {
		return NULL;
	}
	else {
		void* tmp=mem_top;
		mem_top=(void*)((char*)mem_top+new_size);
		return tmp;
	}
}
 
AGPAllocator* agp_alloc=NULL;

