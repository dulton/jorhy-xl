#include <windows.h>
#include <assert.h>
#include <SDL.h>
#include <SDL_audio.h>

#include "Render.h"

/* Dll */
typedef int (*DLL_SDL_Init)(Uint32 flags);
static DLL_SDL_Init dll_sdl_init = NULL;

typedef int (*DLL_SDL_OpenAudio)(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
static DLL_SDL_OpenAudio dll_sdl_open_audio = NULL;

typedef SDL_Surface * (*DLL_SDL_SetVideoMode)(int width, int height, int bpp, Uint32 flags);
static DLL_SDL_SetVideoMode dll_sdl_set_video_mode = NULL;

typedef void (*DLL_SDL_FreeSurface)(SDL_Surface *surface);
static DLL_SDL_FreeSurface dll_sdl_free_surface = NULL;

typedef void (*DLL_SDL_Quit)();
static DLL_SDL_Quit dll_sdl_quit = NULL;

typedef void (*DLL_SDL_WM_SetCaption)(const char *title, const char *icon);
static DLL_SDL_WM_SetCaption dll_sdl_wm_set_caption = NULL;

typedef SDL_Overlay * (*DLL_SDL_CreateYUVOverlay)(int width, int height, Uint32 format, SDL_Surface *display);
static DLL_SDL_CreateYUVOverlay dll_sdl_create_yuv_overlay = NULL;

typedef void (*DLL_SDL_FreeYUVOverlay)(SDL_Overlay *overlay);
static DLL_SDL_FreeYUVOverlay dll_sdl_free_yuv_overlay = NULL;

typedef int (*DLL_SDL_LockYUVOverlay)(SDL_Overlay *overlay);
static DLL_SDL_LockYUVOverlay dll_sdl_lock_yuv_overlay = NULL;

typedef int (*DLL_SDL_DisplayYUVOverlay)(SDL_Overlay *overlay, SDL_Rect *dstrect);
static DLL_SDL_DisplayYUVOverlay dll_sdl_display_yuv_overlay = NULL;

typedef void (*DLL_SDL_UnlockYUVOverlay)(SDL_Overlay *overlay);
static DLL_SDL_UnlockYUVOverlay dll_sdl_unlock_yuv_overlay = NULL;

typedef void (*DLL_SDL_PauseAudio)(int pause_on);
static DLL_SDL_PauseAudio dll_sdl_pause_audio = NULL;

typedef void (*DLL_SDL_CloseAudio)(void);
static DLL_SDL_CloseAudio dll_sdl_close_audil = NULL;

typedef int (*DLL_SDL_putenv)(const char *variable);
static DLL_SDL_putenv dll_sdl_putenv = NULL;

HINSTANCE sh_sdl_Inst = NULL; 

/* Video Info */
static SDL_Surface* ps_screen;
static SDL_Overlay* ps_bmp;
static SDL_Rect     s_rect;
static int          v_pic_size = 0;
static int			v_frame_size = 0;
static int 			pic_width = 0;
static int 			pic_height = 0;

/* Audio Info */
static SDL_AudioSpec 		s_fmt; 
static char *sp_data 		= NULL;
static HANDLE sh_pcm_event 	= INVALID_HANDLE_VALUE;

int WINAPI InitPlayer()
{
	DWORD err = 0;
	sh_sdl_Inst = LoadLibrary(L"D:\\JoorhyPrj\\dll\\Player\\SDL.dll");
	err = GetLastError();

	/* Init functions */
	dll_sdl_lock_yuv_overlay = (DLL_SDL_LockYUVOverlay)GetProcAddress(sh_sdl_Inst, "SDL_LockYUVOverlay");
	dll_sdl_display_yuv_overlay = (DLL_SDL_DisplayYUVOverlay)GetProcAddress(sh_sdl_Inst, "SDL_DisplayYUVOverlay");
	dll_sdl_unlock_yuv_overlay = (DLL_SDL_UnlockYUVOverlay)GetProcAddress(sh_sdl_Inst, "SDL_UnlockYUVOverlay");

	return 0;
}

void WINAPI DeinitPlayer()
{
	/* Quit SDL */
	dll_sdl_quit = (DLL_SDL_Quit)GetProcAddress(sh_sdl_Inst, "SDL_Quit");
	dll_sdl_quit();
}

int WINAPI SetVideoInfo(int nWidth, int nHeight, HWND hwnd)
{
	if (ps_screen != NULL)
	{
		dll_sdl_free_surface =  (DLL_SDL_FreeSurface)GetProcAddress(sh_sdl_Inst, "SDL_FreeSurface");
		dll_sdl_free_surface(ps_screen);
		ps_screen = NULL;
	}

	/* Set up the screen */
	if (ps_screen == NULL)
	{
		if (hwnd != NULL)
		{
			char sdl_var[64];     
			sprintf(sdl_var, "SDL_WINDOWID=%d", hwnd); //win handle
			dll_sdl_putenv = (DLL_SDL_putenv)GetProcAddress(sh_sdl_Inst, "SDL_putenv");
			dll_sdl_putenv(sdl_var);  
		}

		dll_sdl_init = (DLL_SDL_Init)GetProcAddress(sh_sdl_Inst, "SDL_Init");
		/* Init SDL */
		if(dll_sdl_init(SDL_INIT_EVERYTHING) == -1)
		{
			assert(FALSE);
			return -1;
		}

		dll_sdl_set_video_mode = (DLL_SDL_SetVideoMode)GetProcAddress(sh_sdl_Inst, "SDL_SetVideoMode");
		ps_screen = dll_sdl_set_video_mode(nWidth, nHeight, 0, SDL_SWSURFACE);
		if( !ps_screen )
		{
			assert(FALSE);
			return -1;
		}

		/* Set the window caption */
		dll_sdl_wm_set_caption = (DLL_SDL_WM_SetCaption)GetProcAddress(sh_sdl_Inst, "SDL_WM_SetCaption");
		dll_sdl_wm_set_caption("Render", NULL);


		//SDL_CreateYUVOverlay
		///* Create a YUV overlay */
		//dll_sdl_create_yuv_overlay = (DLL_SDL_CreateYUVOverlay)GetProcAddress(sh_sdl_Inst, "SDL_CreateYUVOverlay");
		//ps_bmp = dll_sdl_create_yuv_overlay(352, 288, SDL_YV12_OVERLAY, ps_screen);

		//pic_width = nWidth;
		//pic_height = nHeight;
		//v_frame_size = nWidth * nHeight;
		//v_pic_size = (v_frame_size * 3) / 2;	

		///* Update the screen */
		//s_rect.x = 0;
		//s_rect.y = 0;
		//s_rect.w = nWidth + 8;
		//s_rect.h = nHeight + 8;
	}

	return 0;
}

void WINAPI RenderYUV(unsigned char *pY, unsigned char *pU, unsigned char *pV, int width, int height, int x, int y, int w, int h)
{
	/* Create a YUV overlay */
	dll_sdl_create_yuv_overlay = (DLL_SDL_CreateYUVOverlay)GetProcAddress(sh_sdl_Inst, "SDL_CreateYUVOverlay");
	ps_bmp = dll_sdl_create_yuv_overlay(width, height, SDL_YV12_OVERLAY, ps_screen);

	/* Apply the image to the screen */
	ps_bmp->pixels[0] = pY;
	ps_bmp->pixels[2] = pU;
	ps_bmp->pixels[1] = pV;

	ps_bmp->pitches[0] = width;
	ps_bmp->pitches[2] = width / 2;
	ps_bmp->pitches[1] = width / 2;

	s_rect.x = x;
	s_rect.y = y;
	s_rect.w = w;
	s_rect.h = h;
	dll_sdl_lock_yuv_overlay(ps_bmp);
	dll_sdl_display_yuv_overlay(ps_bmp, &s_rect);
	dll_sdl_unlock_yuv_overlay(ps_bmp);

	dll_sdl_free_yuv_overlay = (DLL_SDL_FreeYUVOverlay)GetProcAddress(sh_sdl_Inst, "SDL_FreeYUVOverlay");
	dll_sdl_free_yuv_overlay(ps_bmp);
}

void FillPcmData(void *unused, unsigned char *stream, int len)  
{  
	if (sp_data != NULL)
	{
		SDL_memcpy(stream, sp_data, 160 * 2);  
	}
	SetEvent(sh_pcm_event);
}

int WINAPI OpenAudio(int nFreq, int nBitsPerSample, int nChannel)
{
	/* Set fmt */ 
	s_fmt.freq = nFreq;  
	s_fmt.format = AUDIO_S16;  
	s_fmt.channels = nChannel;  
	s_fmt.samples = 160;  
	s_fmt.callback = FillPcmData;  
	s_fmt.userdata = NULL;  

	/* OpenAudio */
	dll_sdl_open_audio = (DLL_SDL_OpenAudio)GetProcAddress(sh_sdl_Inst, "SDL_OpenAudio");
	if (dll_sdl_open_audio(&s_fmt, NULL) < 0)  
	{  
		assert(FALSE); 
		return -1;  
	} 

	sp_data = (char *)malloc(1600);
	if (sp_data == NULL)
	{
		assert(FALSE);
		return -1;
	}

	sh_pcm_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (sh_pcm_event == INVALID_HANDLE_VALUE)
	{
		assert(FALSE);
		return -1;
	}
	SetEvent(sh_pcm_event);

	return 0;
}

int WINAPI StartAudio()
{
	/* Play Audio */
	dll_sdl_pause_audio = (DLL_SDL_PauseAudio)GetProcAddress(sh_sdl_Inst, "SDL_PauseAudio");
	dll_sdl_pause_audio(0);  
	/*while (SDL_GetAudioStatus() == SDL_AUDIO_PLAYING)  
	{  
	SDL_Delay(1000);  
	}*/
}

void WINAPI RenderPCM(short *pData, int nLen)
{
	WaitForSingleObject(sh_pcm_event, INFINITE);
	if (sp_data != NULL)
	{
		memcpy(sp_data, pData, nLen * sizeof(short));
	}
}

void WINAPI CloseAudio()
{
	SetEvent(sh_pcm_event);

	/* Stop Audio */
	dll_sdl_close_audil = dll_sdl_pause_audio = (DLL_SDL_CloseAudio)GetProcAddress(sh_sdl_Inst, "SDL_CloseAudio");
	dll_sdl_close_audil(); 

	if (sp_data != NULL)
	{
		free(sp_data);
		sp_data = NULL;
	}
}