#include "stdafx.h"
#include "datatype.h"
#include "codec.h"

Codec::Codec():m_lLocalRec(NULL)
{
	::CoInitialize(NULL);
	
	m_lTrdRecFile = INVALID_HANDLE_VALUE;
	for (int i = 0; i < 8; i++)
	{
		m_szRecFileChannel[i] = m_szRMChannel[i] = m_szRecChannel[i] = NULL;
		m_bRecChannel[i] = FALSE;
		
	}

}

Codec::~Codec()
{
	for(UINT i = 0; i < 8; i++)
	{
		m_szRecFileChannel[i] = m_szRMChannel[i] = m_szRecChannel[i] = NULL;
		m_bRecChannel[i] = FALSE;		
	}
	
	
	::CoUninitialize();
}

BOOL Codec::StartRMDisplay(HWND hWnd, UINT uChannelID)
{
	if(m_szRMChannel[uChannelID] != NULL)
		return FALSE;

	m_szRMChannel[uChannelID] = AMESDK_CREATE( "Common Analog Decoder (H.264)", uChannelID, 6, hWnd, NULL, NULL);
	AMESDK_RUN(m_szRMChannel[uChannelID]);
	return TRUE;
}


BOOL Codec::StopRMDisplay(UINT uChannelID)
{
	if(m_szRMChannel[uChannelID] == NULL)
		return FALSE;

	AMESDK_STOP(m_szRMChannel[uChannelID]);
	AMESDK_DESTROY(m_szRMChannel[uChannelID]);
	m_szRMChannel[uChannelID] = NULL;
	return TRUE;
}

BOOL Codec::StartPlayback(HWND hWnd, UINT uChannelID)
{
	if(m_szRecChannel[uChannelID] != NULL)
		return FALSE;

	m_szRecChannel[uChannelID] = AMESDK_CREATE( "Common Analog Decoder (H.264)", uChannelID + 8, 6, hWnd, NULL, NULL);
	AMESDK_RUN(m_szRecChannel[uChannelID]);
	return TRUE;
}

BOOL Codec::StopPlayback(UINT uChannelID)
{
	if(m_szRecChannel[uChannelID] == NULL)
		return FALSE;

	AMESDK_STOP(m_szRecChannel[uChannelID]);
	AMESDK_DESTROY(m_szRecChannel[uChannelID]);
	m_szRecChannel[uChannelID] = NULL;

	return TRUE;
}

BOOL Codec::StartRec(CString& strFileName)
{
	m_csLocalRec.Lock();

	m_lLocalRec = CreateFile(strFileName,GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	if(m_lLocalRec == INVALID_HANDLE_VALUE)
	{
		m_csLocalRec.Unlock();
		return FALSE;
	}

	FILEHEADER fileheader;
	ZeroMemory(&fileheader, sizeof(FILEHEADER));
	time(&fileheader.tmStartTime);

	DWORD dwBytesOfWrite;
	WriteFile(m_lLocalRec, &fileheader, sizeof(FILEHEADER), &dwBytesOfWrite, NULL);

	CryptAcquireContext(&m_hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	CryptCreateHash(m_hProv, CALG_MD5, 0, 0, &m_hHash);

	m_csLocalRec.Unlock();	
	return TRUE;
}

BOOL Codec::SetRecChannel(UINT uChannelID)
{
	m_csLocalRec.Lock();
	if(uChannelID >= 8)
	{
		m_csLocalRec.Unlock();
		return FALSE;
	}

	m_bRecChannel[uChannelID] = TRUE;
	m_csLocalRec.Unlock();
	return TRUE;
}

BOOL Codec::StopRec()
{
	m_csLocalRec.Lock();
	memset(m_bRecChannel, 0, 8 * sizeof(BOOL));

	HANDLE lMapLocalRec = CreateFileMapping(m_lLocalRec, NULL, PAGE_READWRITE, 0, 0, NULL);
	if(lMapLocalRec == INVALID_HANDLE_VALUE)
	{
		m_csLocalRec.Unlock();
		return FALSE;
	}
	LPFILEHEADER lpFileHeader = (LPFILEHEADER)MapViewOfFile(lMapLocalRec,FILE_MAP_ALL_ACCESS, 0, 0, sizeof(FILEHEADER));
	if(lpFileHeader == NULL)
	{
		m_csLocalRec.Unlock();
		return FALSE;
	}

	DWORD dwSize(16);
	CryptGetHashParam(m_hHash, HP_HASHVAL,lpFileHeader->szHash, &dwSize, 0);
	FlushViewOfFile(lpFileHeader, sizeof(FILEHEADER));
	UnmapViewOfFile(lpFileHeader);
	CloseHandle(lMapLocalRec);
	CloseHandle(m_lLocalRec);

	CryptDestroyHash(m_hHash);
	CryptReleaseContext(m_hProv, 0);
	m_csLocalRec.Unlock();

	return TRUE;
}


BOOL Codec::InputStream(LPBYTE pBuffer, ULONG ulSize)
{
	if(ulSize < sizeof(BLOCKHEADER))
		return FALSE;

	LPBLOCKHEADER lpBlock = (LPBLOCKHEADER) pBuffer;
	if(lpBlock->ulVideoSize + sizeof(BLOCKHEADER) != ulSize)
		return FALSE;

	if(m_szRMChannel[lpBlock->uChannelID] != NULL)
		AMESDK_CODEC_DECODE(m_szRMChannel[lpBlock->uChannelID],  pBuffer + sizeof(BLOCKHEADER), ulSize - sizeof(BLOCKHEADER), lpBlock->bIsKeyFrame);

	if(m_szRecChannel[lpBlock->uChannelID] != NULL)
		AMESDK_CODEC_DECODE(m_szRecChannel[lpBlock->uChannelID],  pBuffer + sizeof(BLOCKHEADER), ulSize - sizeof(BLOCKHEADER), lpBlock->bIsKeyFrame);

	if(m_bRecChannel[lpBlock->uChannelID])
	{
		DWORD dwBytesOfWrite(0);
		CryptHashData(m_hHash, pBuffer, ulSize, 0);
		WriteFile(m_lLocalRec, pBuffer, ulSize, &dwBytesOfWrite, NULL);
		FlushFileBuffers(m_lLocalRec);
	}
	return TRUE;
}

BOOL Codec::StartPlayFile(CString& strFileName)
{
	m_lRecFile = CreateFile(strFileName, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
	if(m_lRecFile == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dwReadOfBytes(0);
	FILEHEADER FileHeader ;

	ZeroMemory(&FileHeader, sizeof(FILEHEADER));
	ReadFile(m_lRecFile, &FileHeader, sizeof(FILEHEADER), &dwReadOfBytes, NULL);


	HCRYPTPROV hProv;
	HCRYPTHASH hHash;

	CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);

	BYTE szBuffer[1024];
	while(ReadFile(m_lRecFile, szBuffer, 1024, &dwReadOfBytes, NULL) && dwReadOfBytes != 0)
		CryptHashData(hHash, szBuffer, dwReadOfBytes, 0);
	
	BYTE szHash[16];
	DWORD dwHashSize(16);
	CryptGetHashParam(hHash, HP_HASHVAL,szHash, &dwHashSize, 0);
	//if(0!= memcmp(szHash, FileHeader.szHash, 16))
	//	return FALSE;

	CryptDestroyHash(m_hHash);
	CryptReleaseContext(m_hProv, 0);
	
	m_bNeedExit = FALSE;
	m_lTrdRecFile = CreateThread(NULL, 0, RecPlayProc, this, 0, NULL);
	return TRUE;	
}

DWORD Codec::RecPlayProc(LPVOID lParam)
{
	Codec* lpCodec = (Codec*)lParam;

	SetFilePointer(lpCodec->m_lRecFile, sizeof(FILEHEADER), NULL, FILE_BEGIN);
	DWORD dwBytesOfRead(0);
	BLOCKHEADER block;

	while(!lpCodec->m_bNeedExit)
	{
		if(!ReadFile(lpCodec->m_lRecFile,&block, sizeof(BLOCKHEADER), &dwBytesOfRead, NULL) || dwBytesOfRead <= 0)
			break;
			
		if(lpCodec->m_szRecFileChannel[block.uChannelID] != NULL)
		{
			PBYTE pTempBuffer = new BYTE[block.ulVideoSize];

			if(ReadFile(lpCodec->m_lRecFile, pTempBuffer, block.ulVideoSize, &dwBytesOfRead, NULL) && dwBytesOfRead != 0)
				AMESDK_CODEC_DECODE(lpCodec->m_szRecFileChannel[block.uChannelID], pTempBuffer, block.ulVideoSize, block.bIsKeyFrame);

			delete[] pTempBuffer;
			Sleep(1000 / 25);
		}
		else
			SetFilePointer(lpCodec->m_lRecFile, block.ulVideoSize, NULL, FILE_CURRENT);

	}

	return TRUE;
}

BOOL Codec::SetPlayFileChannel(UINT uChannelID, HWND hWnd)
{
	if(uChannelID >= 8)
		return FALSE;

	m_szRecFileChannel[uChannelID] = AMESDK_CREATE( "Common Analog Decoder (H.264)", uChannelID + 16, 6, hWnd, NULL, NULL);
	AMESDK_RUN(m_szRecFileChannel[uChannelID]);

	return TRUE;
}

BOOL Codec::StopPlayFile()
{
	if(m_lTrdRecFile != INVALID_HANDLE_VALUE)
	{
		m_bNeedExit = TRUE;
		WaitForSingleObject(m_lTrdRecFile, INFINITE);
	}

	if(m_lRecFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_lRecFile);

	for(int i = 0; i < 8; i++)
	{
		if(m_szRecFileChannel[i] != NULL)
		{
			AMESDK_STOP(m_szRecFileChannel[i]);
			AMESDK_DESTROY(m_szRecFileChannel[i]);
			m_szRecFileChannel[i] = NULL;
		}
	}
	return TRUE;
}