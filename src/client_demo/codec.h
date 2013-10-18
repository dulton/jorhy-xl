#pragma once

#pragma comment(lib, "AMESDK.lib")
#include "AMESDK.H"

class Codec
{
private:
	//ʵʱ��Ƶ��¼��ط�
	DEVICE_HANDLE m_szRMChannel[8], m_szRecChannel[8], m_szRecFileChannel[8];
	
	//����¼��͵����ļ�
	BOOL m_bRecChannel[8];
	HANDLE m_lLocalRec;
	CCriticalSection m_csLocalRec;


	HCRYPTPROV m_hProv;
	HCRYPTHASH m_hHash;	

	//¼���ļ�����
	HANDLE m_lRecFile,m_lTrdRecFile;
	BOOL m_bNeedExit;
	static DWORD WINAPI RecPlayProc(LPVOID lParam);

public:
	Codec();
	virtual ~Codec();

	//������Ƶ
	BOOL InputStream(PBYTE pBuffer, ULONG ulSize);

	//ʵʱ���
	BOOL StartRMDisplay(HWND hWnd, UINT uChannelID);	
	BOOL StopRMDisplay(UINT uChannelID);

	//Զ�̻ط�
	BOOL StartPlayback(HWND hWnd, UINT uChannelID);
	BOOL StopPlayback(UINT uChannelID);

	//����¼��, ����¼��
	BOOL StartRec(CString& strFileName);
	BOOL SetRecChannel(UINT uChannelID);
	BOOL StopRec();

	//�ļ�����
	BOOL StartPlayFile(CString& strFileName);
	BOOL SetPlayFileChannel(UINT uChannelID, HWND hWnd);
	BOOL StopPlayFile();
};