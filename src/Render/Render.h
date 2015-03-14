#ifndef __PLAYER_H_
#define __PLAYER_H_

#ifdef LIBRENDER_EXPORTS
#define PLAYER_API __declspec(dllexport)
#else
#define PLAYER_API  __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/// 接口
PLAYER_API int WINAPI InitPlayer();
PLAYER_API void WINAPI DeinitPlayer();
PLAYER_API int WINAPI SetVideoInfo(int nWidth, int nHeight, HWND hwnd);
PLAYER_API void WINAPI RenderYUV(unsigned char *pY, unsigned char *pU, unsigned char *pV, int width, int height, int x, int y, int w, int h);
PLAYER_API int WINAPI OpenAudio(int nFreq, int nBitsPerSample, int nChannel);
PLAYER_API int WINAPI StartAudio();
PLAYER_API void WINAPI RenderPCM(short *pData, int nLen);
PLAYER_API void WINAPI CloseAudio();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //~__PLAYER_H_