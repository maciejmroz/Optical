
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <GL\gl.h>
#include <stdlib.h>
#include <stdio.h>
#include<objbase.h>
#include<initguid.h>
#include"this_demo.h"
#include"resource.h"
#include <ijl.h>
#include "debug_msg.h"
#include <commdlg.h>

//#define _LIMITED_USE_

#ifdef _LIMITED_USE_
#include <winsock2.h>
#endif


BOOL                    bActive;        // is application active?
HGLRC					hRC;
HDC						hDC;
HDC						pbufferDC=0;
PIXELFORMATDESCRIPTOR	pfd;
int						start;
DSampleDemo*			demo;
Camera					cam;
HWND					win_handle;
LPVFS					vfs;
HPBUFFERARB				PBuffer_handle=0;
HGLRC					pbufferRC=0;
Texture					*pbuffer_texture;

static int want_screenshot;
static int screenshot_num;
static char num_buf[30];
static char name_buf[30];
static int initialized=0;
int width,height;


//dialog data
static int	dlg_res;
static int	dlg_min_filt;
static int	dlg_multitexture;
static int	dlg_tex_res;
static int	dlg_mag_filt;
static int	dlg_jpg_quality;
static int	dlg_normalize;
static int	dlg_lighting;
static int	dlg_num_lights_sel;
static int	dlg_size_sel;
static int	dlg_var;
static int	dlg_blinding_flares;
static int	dlg_immediate_mode_rendering;
static int	dlg_vsync;
//only for adv config
int			dlg_show_fps;
static int	dlg_texture_compression;

extern int compress_textures;

HINSTANCE hInstTmp=NULL;

extern int enable_flare_fx;

BOOL CALLBACK DlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam) {
	HWND hCombo;
	switch(message) {
	case WM_INITDIALOG:
		CheckRadioButton(hDlg,IDC_RES640,IDC_RES1024,IDC_RES640);
		CheckRadioButton(hDlg,IDC_FILTER_LML,IDC_FILTER_N,IDC_FILTER_LML);
		CheckRadioButton(hDlg,IDC_TEXTURE_256,IDC_TEXTURE_UNLIMITED,IDC_TEXTURE_512);
		CheckRadioButton(hDlg,IDC_MAG_LINEAR,IDC_MAG_NEAREST,IDC_MAG_LINEAR);
		CheckRadioButton(hDlg,IDC_JPG_POOR,IDC_JPG_EXCELLENT,IDC_JPG_GOOD);
		CheckDlgButton(hDlg,IDC_NORMALIZE_CHECK,BST_CHECKED);
		CheckDlgButton(hDlg,IDC_LIGHTING_CHECK,BST_CHECKED);
		CheckDlgButton(hDlg,IDC_VAR_CHECK,BST_CHECKED);
		CheckDlgButton(hDlg,IDC_BLINDING_FLARES_CHECK,BST_CHECKED);
		CheckDlgButton(hDlg,IDC_IMMEDIATE_MODE_CHECK,BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_VSYNC_CHECK,BST_CHECKED);
		CheckDlgButton(hDlg,IDC_MULTITEXTURE_CHECK,BST_CHECKED);

		hCombo=GetDlgItem(hDlg,IDC_MAXLIGHTS_COMBO);
		SendMessage(hCombo,CB_RESETCONTENT,0,0);
		SendMessage(hCombo,CB_ADDSTRING,0,(LPARAM)"1");
		SendMessage(hCombo,CB_ADDSTRING,0,(LPARAM)"2");
		SendMessage(hCombo,CB_ADDSTRING,0,(LPARAM)"4");
		SendMessage(hCombo,CB_ADDSTRING,0,(LPARAM)"8");
		SendMessage(hCombo,CB_SETCURSEL,3,0);
		dlg_num_lights_sel=3;

		hCombo=GetDlgItem(hDlg,IDC_SCENE_SIZE_COMBO);
		SendMessage(hCombo,CB_RESETCONTENT,0,0);
		SendMessage(hCombo,CB_ADDSTRING,0,(LPARAM)"1-Normal");
		SendMessage(hCombo,CB_ADDSTRING,0,(LPARAM)"5-Meduim");
		SendMessage(hCombo,CB_ADDSTRING,0,(LPARAM)"25-Large");
		SendMessage(hCombo,CB_ADDSTRING,0,(LPARAM)"100-Huge");
		SendMessage(hCombo,CB_SETCURSEL,0,0);
		dlg_size_sel=0;

		dlg_vsync=1;
		dlg_immediate_mode_rendering=0;
		dlg_var=1;
		dlg_normalize=1;
		dlg_lighting=1;
		dlg_blinding_flares=1;
		dlg_res=0;
		dlg_min_filt=0;
		dlg_mag_filt=0;
		dlg_multitexture=1;
		dlg_tex_res=1;
		dlg_jpg_quality=75;
		return TRUE;
	break;
	case WM_COMMAND:
		switch(LOWORD(wParam)) {
		case IDC_RES640:
			dlg_res=0;
		break;
		case IDC_RES800:
			dlg_res=1;
		break;
		case IDC_RES1024:
			dlg_res=2;
		break;
		case IDC_FILTER_LML:
			dlg_min_filt=0;
		break;
		case IDC_FILTER_LMN:
			dlg_min_filt=1;
		break;
		case IDC_FILTER_N:
			dlg_min_filt=2;
		break;
		case IDC_MULTITEXTURE_CHECK:
			dlg_multitexture=1-dlg_multitexture;
		break;
		case IDC_NORMALIZE_CHECK:
			dlg_normalize=1-dlg_normalize;
		break;
		case IDC_DISPLAY_LIST_CHECK:
			dlg_var=1-dlg_var;
		break;
		case IDC_BLINDING_FLARES_CHECK:
			dlg_blinding_flares=1-dlg_blinding_flares;
		break;
		case IDC_VSYNC_CHECK:
			dlg_vsync=1-dlg_vsync;
		break;
		case IDC_IMMEDIATE_MODE_CHECK:
			dlg_immediate_mode_rendering=1-dlg_immediate_mode_rendering;
		break;
		case IDC_LIGHTING_CHECK:
			dlg_lighting=1-dlg_lighting;
			hCombo=GetDlgItem(hDlg,IDC_MAXLIGHTS_COMBO);
			if(dlg_lighting) {
				EnableWindow(hCombo,TRUE);
			}
			else {
				EnableWindow(hCombo,FALSE);
			}
		break;
		case IDC_MAXLIGHTS_COMBO:
			switch(HIWORD(wParam)) {
			case CBN_SELCHANGE:
				hCombo=GetDlgItem(hDlg,IDC_MAXLIGHTS_COMBO);
				dlg_num_lights_sel=SendMessage(hCombo,CB_GETCURSEL,0,0);
			break;
			}
		break;
		case IDC_SCENE_SIZE_COMBO:
			switch(HIWORD(wParam)) {
			case CBN_SELCHANGE:
				hCombo=GetDlgItem(hDlg,IDC_SCENE_SIZE_COMBO);
				dlg_size_sel=SendMessage(hCombo,CB_GETCURSEL,0,0);
			break;
			}
		break;
		case IDC_TEXTURE_256:
			dlg_tex_res=0;
		break;
		case IDC_TEXTURE_512:
			dlg_tex_res=1;
		break;
		case IDC_TEXTURE_UNLIMITED:
			dlg_tex_res=2;
		break;
		case IDC_MAG_LINEAR:
			dlg_mag_filt=0;
		break;
		case IDC_MAG_NEAREST:
			dlg_mag_filt=1;
		break;
		case IDC_JPG_POOR:
			dlg_jpg_quality=25;
		break;
		case IDC_JPG_GOOD:
			dlg_jpg_quality=75;
		break;
		case IDC_JPG_EXCELLENT:
			dlg_jpg_quality=100;
		break;
		case IDOK:
			EndDialog(hDlg,1);
			return TRUE;
		break;
		case IDCANCEL:
			EndDialog(hDlg,0);
			return TRUE;
		break;
		}
	break;
	}
	return FALSE;
}

void update_frame(void);

void change_size(GLsizei w,GLsizei h) {
	cam.set_frustum(-0.125,0.125,-0.1,0.1,0.1,40.0);
	cam.set_origin(0.0,0.0,1.5);
	cam.update_frustum();
	cam.apply_transform();
	glViewport(0, 0, w, h);
}

static char filename[256];
static OPENFILENAME openfilename;


long FAR PASCAL WindowProc( HWND hWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam )
{
	int nPixelFormat;
    //paramerty swiatla
	float amb[4]={0.0f,0.0f,0.0f,0.0f};
	float dif[4]={1.0f,1.0f,1.0f,1.0f};
	float spec[4]={0.0f,0.0f,0.0f,0.0f};
	float pos[4]={0.0f,0.0f,1.0f,0.0f};
    switch( message )
    {
    case WM_ACTIVATEAPP:
	bActive = wParam;
	break;

    case WM_CREATE:
		want_screenshot=0;
		screenshot_num=0;
		win_handle=hWnd;
		hDC=GetDC(hWnd);
#ifdef _LOG_EVERYTHING
		DebugMessage("Device context handle is 0x%x\n",hDC);
#endif
		ZeroMemory(&pfd,sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion=1;
		pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType=PFD_TYPE_RGBA;
		pfd.cColorBits=24;
		pfd.cRedBits=0;
		pfd.cRedShift=0;
		pfd.cGreenBits=0;
		pfd.cGreenShift=0;
		pfd.cBlueBits=0;
		pfd.cBlueShift=0;
		pfd.cAlphaBits=0;
		pfd.cAlphaShift=0;
		pfd.cAccumBits=0;
		pfd.cAccumRedBits=0;
		pfd.cAccumGreenBits=0;
		pfd.cAccumBlueBits=0;
		pfd.cAccumAlphaBits=0;
		pfd.cDepthBits=24;
		pfd.cStencilBits=8;
		pfd.cAuxBuffers=0;
		pfd.iLayerType=PFD_MAIN_PLANE;
		pfd.bReserved=0;
		pfd.dwLayerMask=0;
		pfd.dwVisibleMask=0;
		pfd.dwDamageMask=0;

		nPixelFormat = ChoosePixelFormat(hDC, &pfd);
#ifdef _LOG_EVERYTHING
		DebugMessage("Retrieved pixel format:\n");
		DebugMessage("ColorBits %d\n",pfd.cColorBits);
		DebugMessage("DepthBits %d\n",pfd.cDepthBits);
		DebugMessage("StencilBits %d\n",pfd.cStencilBits);
#endif
//		PrintfBox("Color bits: %d",pfd.cColorBits);
//		PrintfBox("DepthBits %d\n",pfd.cDepthBits);
//		PrintfBox("StencilBits %d\n",pfd.cStencilBits);
		int res;;
		res=SetPixelFormat(hDC,nPixelFormat,&pfd);
#ifdef _LOG_EVERYTHING
		DebugMessage("SetPixelFormat return value is %d\n",res);
#endif
		hRC=wglCreateContext(hDC);
		wglMakeCurrent(hDC,hRC);
		c_state=&state;
		InitWglExtensions();
		//check if we have Pbuffers
		hasPbuffers=0;
		if(hasWglPixelFormat && hasPbuffers) {
			//try to allocate one
			int format;
			unsigned int num_formats;
			float f_attrib_list[]= {
				0.0f
			};

			int attrib_list[]= {
				WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
				WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB,GL_FALSE,
				WGL_ALPHA_BITS_ARB,8,
				WGL_COLOR_BITS_ARB,24,
				WGL_DEPTH_BITS_ARB,24,
				WGL_STENCIL_BITS_ARB,8,
				WGL_PIXEL_TYPE_ARB,WGL_TYPE_RGBA_ARB,
				WGL_DRAW_TO_PBUFFER_ARB,GL_TRUE,		//we need pbuffer support :))))				
				0
			};
			if(wglChoosePixelFormatARB(hDC,(const int*)attrib_list,
				(const float*)f_attrib_list,1,&format,
				&num_formats)==TRUE) {
				int pbuf_create_attribs[]= {
					WGL_PBUFFER_LARGEST_ARB,GL_TRUE,
					0
				};
				PBuffer_handle=wglCreatePbufferARB(hDC,format,
					512,512,(const int*)&pbuf_create_attribs);
				if(!PBuffer_handle) {
					PrintfBox("Could not create pbuffer");
				}
				int pb_width,pb_height;
				wglQueryPbufferARB(PBuffer_handle,WGL_PBUFFER_HEIGHT_ARB,&pb_height);
				wglQueryPbufferARB(PBuffer_handle,WGL_PBUFFER_WIDTH_ARB,&pb_width);
				if((pb_width!=512) || (pb_height!=512)) {
					PrintfBox("Failed to allocate pixel buffer big enough");
					if(PBuffer_handle) {
						wglDestroyPbufferARB(PBuffer_handle);
					}
				}
				pbufferDC=wglGetPbufferDCARB(PBuffer_handle);
				if(!pbufferDC) {
					PrintfBox("Could not get pbuffer DC");
				}
				pbufferRC=wglCreateContext(pbufferDC);
				if(!pbufferRC) {
					PrintfBox("Could not create rendering context");
				}
				if(!wglShareLists(hRC, pbufferRC)) {
					PrintfBox("wglShareLists failed");
				};
				wglMakeCurrent(pbufferDC,pbufferRC);
				c_state=&alt_state;

				if(!immediate_mode) {
					glEnableClientState(GL_VERTEX_ARRAY);
					glEnableClientState(GL_NORMAL_ARRAY);
				}

				//init some state etc for pbuffer rendering context
				glClearColor(0.0f,0.0f,0.0f,0.0f);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				c_state->want_enabled(GL_TEXTURE_2D);
				c_state->want_enabled(GL_CULL_FACE);
				glDepthFunc(GL_LEQUAL);
				glFogf(GL_FOG_MODE,GL_LINEAR);
				if(dlg_normalize) glEnable(GL_NORMALIZE);
				if(!dlg_lighting) {
					c_state->want_disabled(GL_LIGHTING);
					c_state->lock_state(GL_LIGHTING);
				}
				else {
					int i;
					max_lights=1;
					for(i=0;i<dlg_num_lights_sel;i++) {
						max_lights<<=1;
					}
					c_state->want_enabled(GL_LIGHTING);
					c_state->want_enabled(GL_LIGHT0);
				}
				glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
				glViewport(0,0,512,512);

				wglMakeCurrent(hDC,hRC);
				c_state=&state;

				//create render target texture
				char *tmp_char;
				tmp_char=new char[512*512*4];
				pbuffer_texture=new Texture(512,512,tmp_char);
				delete tmp_char;
			}
			else {
				PrintfBox("No matching pbuffer format found");
			}
		}
		InitExtensions();

		/*******************************
		fast hack to disable compiled_vertex_array everywhere
		*****************************/
//		hasCompiledVertexArray=0;

		hasMultitexture=dlg_multitexture;
		switch(dlg_min_filt) {
		case 0:
			default_min_filter=GL_LINEAR_MIPMAP_LINEAR;
		break;
		case 1:
			default_min_filter=GL_LINEAR_MIPMAP_NEAREST;
		break;
		case 2:
			default_min_filter=GL_NEAREST;
		break;
		}
		immediate_mode=dlg_immediate_mode_rendering;

		glClearColor(0.0f,0.0f,0.0f,0.0f);
		if(!immediate_mode) {
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
		}
		c_state->want_enabled(GL_TEXTURE_2D);
		c_state->want_enabled(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glFogf(GL_FOG_MODE,GL_LINEAR);
		if(dlg_normalize) glEnable(GL_NORMALIZE);
		if(!dlg_lighting) {
			c_state->want_disabled(GL_LIGHTING);
			c_state->lock_state(GL_LIGHTING);
		}
		else {
			int i;
			max_lights=1;
			for(i=0;i<dlg_num_lights_sel;i++) {
//				max_lights<<=1;
			}
			c_state->want_enabled(GL_LIGHTING);
			c_state->want_enabled(GL_LIGHT0);
		}
		switch(dlg_size_sel) {
		case 0:
			scn_default_size=1;
		break;
		case 1:
			scn_default_size=5;
		break;
		case 2:
			scn_default_size=25;
		break;
		case 3:
			scn_default_size=100;
		break;
		}
		switch(dlg_tex_res) {
		case 0:
			max_texture_size=256;
		break;
		case 1:
			if(max_texture_size>512) max_texture_size=512;
		break;
		case 2:
		break;
		}
		switch(dlg_mag_filt) {
		case 0:
			default_mag_filter=GL_LINEAR;
		break;
		case 1:
			default_mag_filter=GL_NEAREST;
		break;
		}
		if(!dlg_var) hasVertexArrayRange=0;
		if(hasVertexArrayRange) {
//			PrintfBox("NV_vertex_array_range detected and enabled\n");
			hasCompiledVertexArray=0;
		}
		if(hasTextureCompression && dlg_texture_compression) {
			compress_textures=1;
			PrintfBox("Texture compression enabled");
		}
		enable_flare_fx=dlg_blinding_flares;
		enable_flare_fx=0;
		start=0;
		if((!dlg_vsync) &&
			hasWglSwapControl) wglSwapIntervalEXT(0);
		SetTimer(hWnd,1,1000,NULL);
		initialized=1;
		agp_alloc=new AGPAllocator(4000000);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);

	break;

    case WM_SETCURSOR:
	SetCursor(NULL);
	return TRUE;

	case WM_TIMER:
		return 0;
	break;

    case WM_SIZE:
		change_size(LOWORD(lParam),HIWORD(lParam));
	break;
 
    case WM_KEYDOWN:
	switch( wParam )
	{
	case VK_ESCAPE:
		delete demo;
	    PostMessage(hWnd, WM_CLOSE, 0, 0);
	break;
	case VK_F11:
		want_screenshot=1;
	break;
	}
	break;

    case WM_DESTROY:
		if(pbufferDC) {
			wglReleasePbufferDCARB(PBuffer_handle,pbufferDC);
			wglDestroyPbufferARB(PBuffer_handle);
		}
		delete agp_alloc;
		wglMakeCurrent(hDC,NULL);
		wglDeleteContext(hRC);
		ChangeDisplaySettings(NULL,0);
		PostQuitMessage( 0 );
	break;
	case WM_SYSCOMMAND:
		switch (wParam){
			case SC_SCREENSAVE:
			case SC_MONITORPOWER:
				return 0;
			break;
			default:
			break;
		}
	break;
    }
	return DefWindowProc(hWnd, message, wParam, lParam);
} /* WindowProc */

/*
 * This function is called if the initialization function fails
 */
BOOL initFail( HWND hwnd )
{
    MessageBox( hwnd, "Init FAILED", "OpenGL Framework", MB_OK );
    DestroyWindow( hwnd );
    return FALSE;

} /* initFail */

/*
 * doInit - do work required for every instance of the application:
 *                create the window, initialize data
 */
static BOOL doInit( HINSTANCE hInstance, int nCmdShow )
{
    HWND                hwnd;
    WNDCLASS            wc;
	int					result;

	want_screenshot=0;
//	result=DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG1),NULL,DlgProc);
	dlg_num_lights_sel=3;
	dlg_size_sel=1;
	dlg_vsync=1;
	dlg_immediate_mode_rendering=0;
	dlg_var=1;
	dlg_normalize=1;
	dlg_lighting=1;
	dlg_blinding_flares=1;
	dlg_res=0;
	dlg_min_filt=0;
	dlg_mag_filt=0;
	dlg_multitexture=1;
	dlg_tex_res=1;
	dlg_jpg_quality=100;
	hInstTmp=hInstance;
	dlg_texture_compression=0;
	dlg_show_fps=1;
//	result=DialogBox(hInstance,MAKEINTRESOURCE(IDD_DEMO_CONFIG),NULL,MekkaDemoBaseDlgProc);

//	if(!result) return FALSE;
 
    /*
     * set up and register window class
     */
    wc.style =CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = "LWS Player";
    wc.lpszClassName = "LWS Player";
    RegisterClass( &wc );
    
	switch(dlg_res) {
	case 0:
		height=480;
		width=640;
	break;
	case 1:
		height=600;
		width=800;
	break;
	case 2:
		height=768;
		width=1024;
	break;
	}
/*	DEVMODE dev;
	dev.dmSize=sizeof(DEVMODE);
	dev.dmPelsHeight=height;
	dev.dmPelsWidth=width;
	dev.dmFields= DM_PELSWIDTH | DM_PELSHEIGHT;
	ChangeDisplaySettings(&dev,CDS_FULLSCREEN);
*/
    /*
     * okno
     */
    hwnd = CreateWindowEx(
	0,
	"LWS Player",
	"LWS Player",
	WS_POPUP |
	WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_EX_TOPMOST,
	0,
	0,
	width, //caly ekran
	height,
	NULL,
	NULL,
	hInstance,
	NULL );

    if( !hwnd )
    {
	PrintfBox("hwnd==NULL");
	return FALSE;
    }
	DEVMODE dev;
	dev.dmSize=sizeof(DEVMODE);
	dev.dmPelsHeight=height;
	dev.dmPelsWidth=width;
	dev.dmBitsPerPel=32;
	dev.dmFields= DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
	ChangeDisplaySettings(&dev,CDS_FULLSCREEN);

    ShowWindow( hwnd, nCmdShow );
    UpdateWindow( hwnd );


    return TRUE;

} /* doInit */

/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
			LPSTR lpCmdLine, int nCmdShow)
{
    MSG         msg;

    lpCmdLine = lpCmdLine;
    hPrevInstance = hPrevInstance;

#ifdef _LOG_EVERYTHING
	DebugSetup();
	DebugFileSetup("extensions.log");
	DebugFileSetup("fps.log");
	DebugFileSetup("warnings.log");
	DebugFileSetup("geometry.log");
	DebugFileSetup("textures.log");
#endif

    if( !doInit( hInstance, nCmdShow ) )
    {
	return FALSE;
    }


    while( 1 )
    {
	if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
	{
	    if( !GetMessage( &msg, NULL, 0, 0 ) )
	    {
		return msg.wParam;
	    }
	    TranslateMessage(&msg); 
	    DispatchMessage(&msg);
	}
	else if( bActive )
	{
	    update_frame();
        }
        else
        {
            // make sure we go to sleep if we have nothing else to do
            WaitMessage();
        }
    }
} /* WinMain */

void update_frame(void) {
	static int			frame=0;
	DWORD thisTickCount=timeGetTime();
	if(!initialized) return;
	if(frame) {
		demo->render(thisTickCount-start);
	}
	else {
		//initialize vfs
		CreateVfs(NULL,IID_IVfs ,(void**)&vfs);
		GC_ERROR err_code;
		err_code=vfs->openFilesystem();
		if(err_code!=GC_OK) {
			PrintfBox("openFilesystem failed!!!");
		}
		demo=new DSampleDemo();

		vfs->closeFilesystem();
		vfs->Release();
		start=timeGetTime();
	}
	frame++;

#ifdef _LOG_EVERYTHING
	DebugSetFrame(frame);
	DebugFrameSetup();
#endif

	if(want_screenshot) {
		itoa(screenshot_num,num_buf,10);
		name_buf[0]='\0';
		strcat(name_buf,"screen");
		strcat(name_buf,num_buf);
		strcat(name_buf,".jpg");
		unsigned char* pixel_buf;
		pixel_buf=new unsigned char[width*height*3];
		glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixel_buf);
		//mirror image to bet bottom-up pixel order
		int i,j;
		unsigned char tmp;
		for(i=0;i<height/2;i++)
			for(j=0;j<width;j++) {
				tmp=pixel_buf[3*(width*i+j)];
				pixel_buf[3*(width*i+j)]=pixel_buf[3*(width*(height-1-i)+j)];
				pixel_buf[3*(width*(height-1-i)+j)]=tmp;
				tmp=pixel_buf[3*(width*i+j)+1];
				pixel_buf[3*(width*i+j)+1]=pixel_buf[3*(width*(height-1-i)+j)+1];
				pixel_buf[3*(width*(height-1-i)+j)+1]=tmp;
				tmp=pixel_buf[3*(width*i+j)+2];
				pixel_buf[3*(width*i+j)+2]=pixel_buf[3*(width*(height-1-i)+j)+2];
				pixel_buf[3*(width*(height-1-i)+j)+2]=tmp;

		}
		//----------------------------------------------------------
		// An example using the IntelR JPEG Library:
		// -- Encode a JFIF file from Windows DIB.
		//----------------------------------------------------------
		IJLERR jerr;
		// Allocate the IJL JPEG_CORE_PROPERTIES structure.
		JPEG_CORE_PROPERTIES jcprops;
		// Initialize the IntelR JPEG Library.
		jerr = ijlInit(&jcprops);
		//dib_line_width = bmih->biWidth * 3;
		//dib_pad_bytes = IJL_DIB_PAD_BYTES(bmih->biWidth,3);
		// Set up information to write from the pixel buffer.
		jcprops.DIBWidth = width;
		jcprops.DIBHeight = height; // Implies a bottom-up DIB.
		jcprops.DIBBytes = pixel_buf;
		jcprops.DIBPadBytes = 0;
		// Note: the following are default values and thus
		// do not need to be set.
		jcprops.DIBChannels = 3;
		jcprops.DIBColor = IJL_RGB;
		jcprops.JPGFile = const_cast<LPTSTR>(name_buf);
		// Specify JPEG file creation parameters.
		jcprops.JPGWidth = width;
		jcprops.JPGHeight = height;
		// Note: the following are default values and thus
		// do not need to be set.
		jcprops.JPGChannels = 3;
		jcprops.JPGColor = IJL_YCBCR;
		jcprops.JPGSubsampling = IJL_411; // 4:1:1 subsampling.
		jcprops.jquality = dlg_jpg_quality; // Select "good" image quality
		// Write the actual JPEG image from the pixel buffer.
		jerr = ijlWrite(&jcprops,IJL_JFILE_WRITEWHOLEIMAGE);
		// Clean up the IntelR JPEG Library.
		ijlFree(&jcprops);

		delete pixel_buf;
		screenshot_num++;
		want_screenshot=0;
	}
/*	static float seed=0.0f;
	seed=next_float(seed);
	DebugMessage("Our random seed is %u\n",rand_seed);
	DebugMessage("Our next random float is %f\n",seed);*/
	SwapBuffers(hDC);
}