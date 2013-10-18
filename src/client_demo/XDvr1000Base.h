#pragma once

#ifndef _XLDLLHEADER
#define DLLHEADER _declspec(dllimport)
#else
#define DLLHEADER _declspec(dllexport)
#endif


#ifdef __cplusplus
extern "C"
{
	//���ɾ���� ������Ƶ��
	DLLHEADER LONG WINAPI Initial();
	DLLHEADER BOOL WINAPI Release(LONG lHandle);
	DLLHEADER BOOL WINAPI InputStream(LONG lHandle, PBYTE pBuffer, ULONG ulSize);

	//ʵʱ��Ƶ�鿴
	DLLHEADER BOOL WINAPI StartRMDisplay(LONG lHandle, HWND hWnd, UINT uChannelID);	
	DLLHEADER BOOL WINAPI StopRMDisplay(LONG lHandle, UINT uChannelID);

	//Զ�̻ط�
	DLLHEADER BOOL WINAPI StartPlayback(LONG lHandle, HWND hWnd, UINT uChannelID);
	DLLHEADER BOOL WINAPI StopPlayback(LONG lHandle, UINT uChannelID);

	//����¼��, ����¼��
	DLLHEADER BOOL WINAPI StartRec(LONG lHandle, CString& strFileName);
	DLLHEADER BOOL WINAPI SetRecChannel(LONG lHandle, UINT uChannelID);
	DLLHEADER BOOL WINAPI StopRec(LONG lHandle);

	//�ļ�����
	DLLHEADER BOOL WINAPI StartPlayFile(LONG lHandle, CString& strFileName);
	DLLHEADER BOOL WINAPI SetPlayFileChannel(LONG lHandle, UINT uChannelID, HWND hWnd);
	DLLHEADER BOOL WINAPI StopPlayFile(LONG lHandle);
}


#endif