#include "stdafx.h"
#include "codec.h"
#include "XDvr1000Base.h"

LONG WINAPI Initial()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (LONG)new Codec();
}

BOOL WINAPI Release(LONG lHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Codec* lpCodec = (Codec*)lHandle;
	if(lpCodec != NULL)
		delete lpCodec;

	return TRUE;
}

BOOL WINAPI InputStream(LONG lHandle, PBYTE pBuffer, ULONG ulSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Codec* lpCodec = (Codec*)lHandle;	
	return lpCodec->InputStream(pBuffer, ulSize);
}

BOOL WINAPI StartRMDisplay(LONG lHandle, HWND hWnd, UINT uChannelID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Codec* lpCodec = (Codec*)lHandle;
	return 	lpCodec->StartRMDisplay(hWnd, uChannelID);;
}

BOOL WINAPI StopRMDisplay(LONG lHandle, UINT uChannelID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Codec* lpCodec = (Codec*)lHandle;
	return lpCodec->StopRMDisplay(uChannelID);
}

BOOL WINAPI StartPlayback(LONG lHandle, HWND hWnd, UINT uChannelID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Codec* lpCodec = (Codec*)lHandle;
	return lpCodec->StartPlayback(hWnd, uChannelID);
}

BOOL WINAPI StopPlayback(LONG lHandle,UINT uChannelID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Codec* lpCodec = (Codec*)lHandle;
	return lpCodec->StopPlayback(uChannelID);
}

BOOL WINAPI StartRec(LONG lHandle, CString& strFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Codec* lpCodec = (Codec*)lHandle;
	return lpCodec->StartRec(strFileName);
}

BOOL WINAPI SetRecChannel(LONG lHandle, UINT uChannelID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Codec* lpCodec = (Codec*)lHandle;
	return lpCodec->SetRecChannel(uChannelID);
}

BOOL WINAPI StopRec(LONG lHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Codec* lpCodec=(Codec*)lHandle;
	return lpCodec->StopRec();
}

BOOL WINAPI StartPlayFile(LONG lHandle, CString& strFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Codec* lpCodec = (Codec*)lHandle;
	return lpCodec->StartPlayFile(strFileName);
}

BOOL WINAPI SetPlayFileChannel(LONG lHandle, UINT uChannelID, HWND hWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Codec* lpCodec=(Codec*)lHandle;
	return lpCodec->SetPlayFileChannel(uChannelID, hWnd);
}

BOOL WINAPI StopPlayFile(LONG lHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Codec* lpCodec = (Codec*)lHandle;
	return lpCodec->StopPlayFile();
}