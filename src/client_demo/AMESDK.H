#ifndef AMESDK_H
#define AMESDK_H

// #define AMESDK_DISABLE_EXPORT_CODEC_FUNCTIONS

// #define AMESDK_DISABLE_EXPORT_NETWORK_FUNCTIONS

// #define AMESDK_DISABLE_EXPORT_FILE_FUNCTIONS

// #define AMESDK_DISABLE_EXPORT_GPS_FUNCTIONS

// #define AMESDK_DISABLE_EXPORT_DEINTERLACER_FUNCTIONS

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct _PROGRAM_INFOMATION { ULONG ONID; ULONG TSID; ULONG SID; WCHAR pwszProgramName[ 64 ]; } PROGRAM_INFOMATION;

typedef ULONG (DEVICE_HANDLE);

#ifdef _VBEXPORT // "DUMPBIN.EXE /EXPORTS AMESDK.DLL >> EXPORTS.TXT"

#define AMESDK_EXPORT __stdcall

#else // _VBEXPORT

#ifdef _BCBEXPORT

#define AMESDK_EXPORT __stdcall

#undef AFX_EXT_API

#define AFX_EXT_API __declspec(dllexport) 

#else // _BCBEXPORT

#define AMESDK_EXPORT __cdecl

#endif // _BCBEXPORT

#endif // _VBEXPORT

typedef BOOL (AMESDK_EXPORT *PF_BUFFER_CALLBACK)( double dSampleTime, BYTE * pBuffer, ULONG nBufferLen, BOOL bIsKeyFrame, PVOID pUserData );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_CAPTURE_DEVICE_NAME_ENUMERATION( ULONGLONG * * ppVideoDeviceList /*OUT*/, ULONG * pVideoDeviceSize /*OUT*/, ULONGLONG * * ppVideoEncoderDeviceList /*OUT*/, ULONG * pVideoEncoderDeviceSize /*OUT*/, ULONGLONG * * ppAudioDeviceList /*OUT*/, ULONG * pAudioDeviceSize /*OUT*/, ULONGLONG * * ppAudioEncoderDeviceList /*OUT*/, ULONG * pAudioEncoderDeviceSize /*OUT*/ );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_CAPTURE_DEVICE_ENUMERATION( ULONGLONG * * ppVideoDeviceList /*OUT*/, ULONG * pVideoDeviceSize /*OUT*/, ULONGLONG * * ppVideoEncoderDeviceList /*OUT*/, ULONG * pVideoEncoderDeviceSize /*OUT*/, ULONGLONG * * ppAudioDeviceList /*OUT*/, ULONG * pAudioDeviceSize /*OUT*/, ULONGLONG * * ppAudioEncoderDeviceList /*OUT*/, ULONG * pAudioEncoderDeviceSize /*OUT*/ );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SOUNDCARD_ENUMERATION( CHAR * * ppszSoundCardDevName /*OUT*/, BOOL bNext = FALSE /*IN*/ );

AFX_EXT_API DEVICE_HANDLE AMESDK_EXPORT AMESDK_CREATE( CHAR * pszDevName /*IN*/, 
													  
													   UINT   iDevNum    /*IN*/, 
													   
													   ULONG  eDevType   /*IN*/, 
													   
													   HWND   hDisplayWindow_A        /*IN*/, PF_BUFFER_CALLBACK pBufferCB_A        /*IN*/, PVOID pUserData_A        /*IN*/, 
													   
													   HWND   hDisplayWindow_B = NULL /*IN*/, PF_BUFFER_CALLBACK pBufferCB_B = NULL /*IN*/, PVOID pUserData_B = NULL /*IN*/, 
													   
													   HWND   hDisplayWindow_C = NULL /*IN*/, PF_BUFFER_CALLBACK pBufferCB_C = NULL /*IN*/, PVOID pUserData_C = NULL /*IN*/, 
													   
													   HWND   hDisplayWindow_D = NULL /*IN*/, PF_BUFFER_CALLBACK pBufferCB_D = NULL /*IN*/, PVOID pUserData_D = NULL /*IN*/ );

// EXAMPLE: hDev = AMESDK_CREATE( "TW6802 PCI", 0, 0, NULL, NULL, NULL ); // DON'T NEED DISPLAY VIDEO. DON'T NEED GET FRAME BUFFER FROM CALLBACK FUNCTION.

// EXAMPLE: hDev = AMESDK_CREATE( "TW6802 PCI", 1, 0, NULL, NULL, NULL ); // IF YOU HAVE TWO DEVICES ON SAME PC, YOU CAN USE PARAMETER 2 TO OPEN 2ND DEVICE.

// EXAMPLE: hDev = AMESDK_CREATE( "TW6802 PCI", 0, 0, hWnd, NULL, NULL ); // DISPLAY VIDEO ON AN ATTACHED WINDOW.

// EXAMPLE: hDev = AMESDK_CREATE( "TW6802 PCI", 0, 0, NULL, &bcb, this ); // RETURN FRAME BUTTER TO THE CALLBACK FUNCTION.

// EXAMPLE: hDev = AMESDK_CREATE( "TW6802 PCI", 0, 0, hWnd, &bcb, this ); // BOTH.

AFX_EXT_API DEVICE_HANDLE AMESDK_EXPORT AMESDK_CREATE_EX( CHAR * pszDevName /*IN*/, 
														 
														  UINT   iDevNum    /*IN*/, 
														  
														  ULONG  eDevType   /*IN*/, 
														 
														  HWND   hDisplayWindow_A        /*IN*/, PF_BUFFER_CALLBACK pBufferCB_A        /*IN*/, BOOL bIsAllowOverlayRenderer_A         /*IN*/, BOOL bIsEnableEnhancedVideoRenderer_A         /*IN*/, BOOL bIsMaintainAspectRatio_A         /*IN*/, PVOID pUserData_A        /*IN*/, 
														  
														  HWND   hDisplayWindow_B = NULL /*IN*/, PF_BUFFER_CALLBACK pBufferCB_B = NULL /*IN*/, BOOL bIsAllowOverlayRenderer_B = FALSE /*IN*/, BOOL bIsEnableEnhancedVideoRenderer_B = FALSE /*IN*/, BOOL bIsMaintainAspectRatio_B = FALSE /*IN*/, PVOID pUserData_B = NULL /*IN*/, 
														  
														  HWND   hDisplayWindow_C = NULL /*IN*/, PF_BUFFER_CALLBACK pBufferCB_C = NULL /*IN*/, BOOL bIsAllowOverlayRenderer_C = FALSE /*IN*/, BOOL bIsEnableEnhancedVideoRenderer_C = FALSE /*IN*/, BOOL bIsMaintainAspectRatio_C = FALSE /*IN*/, PVOID pUserData_C = NULL /*IN*/, 
														  
														  HWND   hDisplayWindow_D = NULL /*IN*/, PF_BUFFER_CALLBACK pBufferCB_D = NULL /*IN*/, BOOL bIsAllowOverlayRenderer_D = FALSE /*IN*/, BOOL bIsEnableEnhancedVideoRenderer_D = FALSE /*IN*/, BOOL bIsMaintainAspectRatio_D = FALSE /*IN*/, PVOID pUserData_D = NULL /*IN*/, ... );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_DESTROY( DEVICE_HANDLE hDevHandle /*IN*/ );

// EXAMPLE: AMESDK_DESTROY( hDev );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_RUN( DEVICE_HANDLE hDevHandle /*IN*/ );

// EXAMPLE: AMESDK_RUN( hDev );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_PAUSE( DEVICE_HANDLE hDevHandle /*IN*/ );

// EXAMPLE: AMESDK_PAUSE( hDev );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_STOP( DEVICE_HANDLE hDevHandle /*IN*/ );

// EXAMPLE: AMESDK_STOP( hDev );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_STEP( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nSteps /*IN*/ );

// EXAMPLE: AMESDK_STEP( hDev, nSteps );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_AUTO_STANDARD_DETECTION( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pStandard /*OUT*/ );

// EXAMPLE: AMESDK_AUTO_STANDARD_DETECTION( hDev, &nStandard );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_STANDARD( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pStandard /*OUT*/ );

// EXAMPLE: AMESDK_GET_STANDARD( hDev, &nStandard );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_STANDARD( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nStandard /*IN*/ );

// EXAMPLE: AMESDK_SET_STANDARD( hDev, KS_AnalogVideo_NTSC_M );

// EXAMPLE: AMESDK_SET_STANDARD( hDev, KS_AnalogVideo_PAL_B );

// -----------------------------------------------------------------
// SUPPORT STANDARDS (60HZ): KS_AnalogVideo_NTSC_M      (0x00000001)
//                           KS_AnalogVideo_NTSC_M_J    (0x00000002)
//                           KS_AnalogVideo_NTSC_433    (0x00000004)
//                           KS_AnalogVideo_PAL_M       (0x00000200)
//                           KS_AnalogVideo_PAL_60      (0x00000800)
// -----------------------------------------------------------------
// SUPPORT STANDARDS (50HZ): KS_AnalogVideo_PAL_B       (0x00000010)
//                           KS_AnalogVideo_PAL_D       (0x00000020)
//                           KS_AnalogVideo_PAL_G       (0x00000040)
//                           KS_AnalogVideo_PAL_H       (0x00000080)
//                           KS_AnalogVideo_PAL_I       (0x00000100)
//                           KS_AnalogVideo_PAL_N       (0x00000400)
//                           KS_AnalogVideo_PAL_N_COMBO (0x00100000)
//                           KS_AnalogVideo_SECAM_B     (0x00001000)
//                           KS_AnalogVideo_SECAM_D     (0x00002000)
//                           KS_AnalogVideo_SECAM_G     (0x00004000)
//                           KS_AnalogVideo_SECAM_H     (0x00008000)
//                           KS_AnalogVideo_SECAM_K     (0x00010000)
//                           KS_AnalogVideo_SECAM_K1    (0x00020000)
//                           KS_AnalogVideo_SECAM_L     (0x00040000)
//                           KS_AnalogVideo_SECAM_L1    (0x00080000)
// -----------------------------------------------------------------

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_INPUT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pInput /*OUT*/ );

// EXAMPLE: AMESDK_GET_INPUT( hDev, &nInput );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_INPUT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nInput /*IN*/ );

// EXAMPLE: AMESDK_SET_INPUT( hDev, 0 );

// --------------------------------------------
// SUPPORT INPUTS: 0 (COMPOSITE / COMPOSITE.01)
//                 1 (S-VIDEO   / COMPOSITE.02)
//                 2 (TUNER     / COMPOSITE.03)
//                 3 (1394      / COMPOSITE.04)
// --------------------------------------------

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_FORMAT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pColorSpaceType /*OUT*/, ULONG * pWidth /*OUT*/, ULONG * pHeight /*OUT*/, ULONG * pBitCount /*OUT*/, double * pFrameRate /*OUT*/ );

// EXAMPLE: AMESDK_GET_FORMAT( hDev, &nColorSpaceType, &nWidth, &nHeight, &nBitCount, &dFrameRate );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_FORMAT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, ULONG nBitCount /*IN*/, double dFrameRate /*IN*/ );

// EXAMPLE: AMESDK_SET_FORMAT( hDev, 0x32595559, 720, 480, 16, 30.000000 );

// EXAMPLE: AMESDK_SET_FORMAT( hDev, 0x32595559, 720, 576, 16, 25.000000 );

// ----------------------------------------------------------------------
// SUPPORT FORMATS (60HZ): 720 กั 480 กั 16 กั 30.00, 720 กั 480 กั 16 กั 60.00
//                         704 กั 480 กั 16 กั 30.00, 704 กั 480 กั 16 กั 60.00
//                         640 กั 480 กั 16 กั 30.00, 640 กั 480 กั 16 กั 60.00
//                         352 กั 240 กั 16 กั 30.00, 352 กั 240 กั 16 กั 60.00
//                         320 กั 240 กั 16 กั 30.00, 320 กั 240 กั 16 กั 60.00
//                         180 กั 120 กั 16 กั 30.00, 180 กั 120 กั 16 กั 60.00
//                         176 กั 120 กั 16 กั 30.00, 176 กั 120 กั 16 กั 60.00
//                         160 กั 120 กั 16 กั 30.00, 160 กั 120 กั 16 กั 60.00
//                         720 กั 240 กั 16 กั 60.00
//                         704 กั 240 กั 16 กั 60.00
//                         640 กั 240 กั 16 กั 60.00
// ----------------------------------------------------------------------
// SUPPORT FORMATS (50HZ): 720 กั 576 กั 16 กั 25.00, 720 กั 576 กั 16 กั 50.00
//                         704 กั 576 กั 16 กั 25.00, 704 กั 576 กั 16 กั 50.00
//                         640 กั 576 กั 16 กั 25.00, 640 กั 576 กั 16 กั 50.00
//                         352 กั 288 กั 16 กั 25.00, 352 กั 288 กั 16 กั 50.00
//                         320 กั 288 กั 16 กั 25.00, 320 กั 288 กั 16 กั 50.00
//                         180 กั 144 กั 16 กั 25.00, 180 กั 144 กั 16 กั 50.00
//                         176 กั 144 กั 16 กั 25.00, 176 กั 144 กั 16 กั 50.00
//                         160 กั 144 กั 16 กั 25.00, 160 กั 144 กั 16 กั 50.00
//                         720 กั 288 กั 16 กั 50.00
//                         704 กั 288 กั 16 กั 50.00
//                         640 กั 288 กั 16 กั 50.00
// ----------------------------------------------------------------------

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_FORMAT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pChannels /*OUT*/, ULONG * pBitsPerSample /*OUT*/, ULONG * pSamplesPerSec /*OUT*/ );

// EXAMPLE: AMESDK_GET_FORMAT( hDev, &nChannels, &nBitsPerSample, &nSamplesPerSec );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_FORMAT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nChannels /*IN*/, ULONG nBitsPerSample /*IN*/, ULONG nSamplesPerSec /*IN*/ );

// EXAMPLE: AMESDK_SET_FORMAT( hDev, 1,  8,  8000 );

// EXAMPLE: AMESDK_SET_FORMAT( hDev, 1, 16, 48000 );

// -------------------------------------------------------------------------------
// SUPPORT FORMATS (CHANNELS): 1 (MONO), 2 (STEREO)
// -------------------------------------------------------------------------------
// SUPPORT FORMATS (BITS PER SAMPLE): 8, 16 (BITS)
// -------------------------------------------------------------------------------
// SUPPORT FORMATS (SAMPLES PER SEC): 8000, 16000, 24000, 32000, 40000, 48000 (HZ)
// -------------------------------------------------------------------------------

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_DEINTERLACE( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pDeinterlace /*OUT*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_GET_DEINTERLACE( hDev, &nDeinterlace );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_DEINTERLACE( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nDeinterlace /*IN*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_SET_DEINTERLACE( hDev, nDeinterlace );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_MIRROR( DEVICE_HANDLE hDevHandle /*IN*/, BOOL * pHorizontalMirror /*OUT*/, BOOL * pVerticalMirror /*OUT*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_GET_MIRROR( hDev, &bHorizontalMirror, &bVerticalMirror );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_MIRROR( DEVICE_HANDLE hDevHandle /*IN*/, BOOL bHorizontalMirror /*IN*/, BOOL bVerticalMirror /*IN*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_SET_MIRROR( hDev, FALSE, FALSE );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_DOUBLE_FRAMERATE( DEVICE_HANDLE hDevHandle /*IN*/, BOOL * pDoubleFrameRate /*OUT*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: RESERVED

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_DOUBLE_FRAMERATE( DEVICE_HANDLE hDevHandle /*IN*/, BOOL bDoubleFrameRate /*IN*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: RESERVED

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_DOWNSAMPLE_SCALE( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pDownsampleScale /*OUT*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: RESERVED

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_DOWNSAMPLE_SCALE( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nDownsampleScale /*IN*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: RESERVED

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_OSD_TEXT( DEVICE_HANDLE hDevHandle /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, CHAR * pszString /*IN*/, CHAR * pszFontFamilyName /*IN*/, ULONG nFontStyle /*IN*/, ULONG nFontSize /*IN*/, DWORD dwFontColor /*IN*/, DWORD dwBackgroundColor /*IN*/, ULONG nTransparent /*IN*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_SET_OSD_TEXT( hDev, 0, 0, 0, 0, 0, "Channel#01", "Arial", FontStyleRegular, 12, 0xFFFFFFFF, 0xFF000000, 0xFF, 0 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_OSD_PICTURE( DEVICE_HANDLE hDevHandle /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, CHAR * pszFilePathName /*IN*/, ULONG nTransparent /*IN*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_SET_OSD_PICTURE( hDev, 0, 0, 0, 0, 0, "LOGO.BMP", 0xFF, 0 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_OSD_BUFFER( DEVICE_HANDLE hDevHandle /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, ULONG nColorSpaceType /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameWidth /*IN*/, ULONG nFrameHeight /*IN*/, ULONG nFramePitch /*IN*/, ULONG nTransparent /*IN*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_SET_OSD_BUFFER( hDev, 0, 0, 0, 0, 0, MAKEFOURCC('Y', 'U', 'Y', '2'), p_src, 100, 100, 200, 0xFF, 0 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_VOLUME( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pVolume /*OUT*/ );

// EXAMPLE: AMESDK_GET_VOLUME( hDev, &nVolume );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_VOLUME( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nVolume /*IN*/ );

// EXAMPLE: AMESDK_SET_VOLUME( hDev, nVolume );

// -------------------------------------------------------
// SUPPORT RANGES: 0 (MUTE) ~ 100 (LARGEST)
// -------------------------------------------------------

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_BANDWIDTH( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pBandwidth /*OUT*/ /*MHZ*/ );

// EXAMPLE: AMESDK_GET_BANDWIDTH( hDev, &nBandwidth );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_BANDWIDTH( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nBandwidth /*IN*/ /*MHZ*/ );

// EXAMPLE: AMESDK_SET_BANDWIDTH( hDev, 8 );

// EXAMPLE: AMESDK_SET_BANDWIDTH( hDev, 6 );

// ---------------------------------
// SUPPORT BANDWIDTHS: 5 (5000000HZ)
//                     6 (6000000HZ)
//                     7 (7000000HZ)
//                     8 (8000000HZ)
// ---------------------------------

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_MODE( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pMode /*OUT*/ );

// EXAMPLE: AMESDK_GET_MODE( hDev, &nStandard );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_MODE( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nMode /*IN*/ );

// EXAMPLE: AMESDK_SET_MODE( hDev, AMTUNER_MODE_TV );

// EXAMPLE: AMESDK_SET_MODE( hDev, AMTUNER_MODE_FM_RADIO );

// -----------------------------------------------------------------
// SUPPORT MODE: AMTUNER_MODE_DEFAULT  (0x00000000)
//               AMTUNER_MODE_TV       (0x00000001)
//               AMTUNER_MODE_FM_RADIO (0x00000002)
//               AMTUNER_MODE_AM_RADIO (0x00000004)

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_FREQUENCY( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pFrequency /*OUT*/ /*HZ*/ );

// EXAMPLE: AMESDK_GET_FREQUENCY( hDev, &Frequency );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_FREQUENCY( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nFrequency /*IN*/ /*HZ*/ );

// EXAMPLE: AMESDK_SET_FREQUENCY( hDev, 473143000 );

// EXAMPLE: AMESDK_SET_FREQUENCY( hDev, 545000000 );

// EXAMPLE: AMESDK_SET_FREQUENCY( hDev, 666000000 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_PROGRAM_LIST( DEVICE_HANDLE hDevHandle /*IN*/, PROGRAM_INFOMATION * pProgramInfoList /*OUT*/, ULONG * pListSize /*OUT*/ );

// EXAMPLE: AMESDK_GET_PROGRAM( hDev, NULL, &nListSize );

// EXAMPLE: AMESDK_GET_PROGRAM( hDev, pProgramInfoList, &nListSize );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_PROGRAM( DEVICE_HANDLE hDevHandle /*IN*/, PROGRAM_INFOMATION * pProgramInfo /*OUT*/ );

// EXAMPLE: AMESDK_GET_PROGRAM( hDev, &sProgramInfo );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_PROGRAM( DEVICE_HANDLE hDevHandle /*IN*/, PROGRAM_INFOMATION sProgramInfo /*IN*/ );

// EXAMPLE: AMESDK_SET_PROGRAM( hDev, sProgramInfo ); // PROGRAM_INFOMATION.PWSZPROGRAMNAME (PARAMETER 4) CAN BE SET TO NULL.

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_STRENGTH( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pStrength /*OUT*/ );

// EXAMPLE: AMESDK_GET_STRENGTH( hDev, &nStrength );

// -------------------------------------------------------
// SUPPORT RANGES: 0 ~ 100 (STRONG)
// -------------------------------------------------------

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_QUALITY( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pQuality /*OUT*/ );

// EXAMPLE: AMESDK_GET_QUALITY( hDev, &nQuality );

// -------------------------------------------------------
// SUPPORT RANGES: 0 ~ 100 (GOOD)
// -------------------------------------------------------

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_PRESENT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pPresent /*OUT*/ );

// EXAMPLE: AMESDK_GET_PRESENT( hDev, &nPresent ); // 0 = NONPRESENT / 1 = PRESENT

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_LOCK( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pLock /*OUT*/ );

// EXAMPLE: AMESDK_GET_LOCK( hDev, &nLock ); // 0 = UNLOCK / 1 = LOCK

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_FPS( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pFps /*OUT*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_GET_FPS( hDev, &nFps );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_CAMERACONTROL_PROPERTY( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nProperty /*IN*/, ULONG * pValue /*OUT*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_GET_CAMERACONTROL_PROPERTY( hDev, CameraControl_Exposure, &nValue );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_CAMERACONTROL_PROPERTY( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nProperty /*IN*/, ULONG nValue /*IN*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_SET_CAMERACONTROL_PROPERTY( hDev, CameraControl_Exposure, 128 );

// -------------------------------------------------------
// SUPPORT PROPERTIES: CameraControl_Pan      (0x00000000)
//                     CameraControl_Tilt     (0x00000001)
//                     CameraControl_Roll     (0x00000002)
//                     CameraControl_Zoom     (0x00000003)
//                     CameraControl_Exposure (0x00000004)
//                     CameraControl_Iris     (0x00000005)
//                     CameraControl_Focus    (0x00000006)
// -------------------------------------------------------
// SUPPORT RANGES: 0 ~ 255
// -------------------------------------------------------

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_VIDEOPROCAMP_PROPERTY_PAGE( DEVICE_HANDLE hDevHandle /*IN*/, HWND hParentWindow /*IN*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_GET_VIDEOPROCAMP_PROPERTY_PAGE( hDev, hWnd );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_VIDEOPROCAMP_PROPERTY( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nProperty /*IN*/, ULONG * pValue /*OUT*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_GET_VIDEOPROCAMP_PROPERTY( hDev, VideoProcAmp_Brightness, &nValue );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_VIDEOPROCAMP_PROPERTY( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nProperty /*IN*/, ULONG nValue /*IN*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_SET_VIDEOPROCAMP_PROPERTY( hDev, VideoProcAmp_Brightness, 128 );

// -------------------------------------------------------------------
// SUPPORT PROPERTIES: VideoProcAmp_Brightness            (0x00000000)
//                     VideoProcAmp_Contrast              (0x00000001)
//                     VideoProcAmp_Hue                   (0x00000002)
//                     VideoProcAmp_Saturation            (0x00000003)
//                     VideoProcAmp_Sharpness             (0x00000004)
//                     VideoProcAmp_Gamma                 (0x00000005)
//                     VideoProcAmp_ColorEnable           (0x00000006)
//                     VideoProcAmp_WhiteBalance          (0x00000007)
//                     VideoProcAmp_BacklightCompensation (0x00000008)
//                     VideoProcAmp_Gain                  (0x00000009)
// -------------------------------------------------------------------
// SUPPORT RANGES: 0 ~ 255
// -------------------------------------------------------------------

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_VIDEOCOMPRESSION_PROPERTY( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nProperty /*IN*/, ULONG * pValue/*OUT*/ );

// EXAMPLE: AMESDK_GET_VIDEOCOMPRESSION_PROPERTY( hDev, VideoCompression_Quality, &nValue );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_VIDEOCOMPRESSION_PROPERTY( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nProperty /*IN*/, ULONG nValue /*IN*/ );

// EXAMPLE: AMESDK_SET_VIDEOCOMPRESSION_PROPERTY( hDev, VideoCompression_Quality, &nValue );

// ----------------------------------------------------------------------
// SUPPORT PROPERTIES: VideoCompression_KeyFrameRate      (   0x00000000)
//                     VideoCompression_Quality           (   0x00000001)
//                     VideoCompression_OverrideKeyFrame  (   0x00000002)
//                     VideoCompression_BitRateMode       (   0x00000003)
//                     VideoCompression_BitRate           (   0x00000004)
//                     VideoCompression_QPStep            (   0x00000005)
//                     VideoCompression_PeakBitRate       (   0x00000006)
//                     VideoCompression_TroughQuality     (   0x00000007)
//                     VideoCompression_PostResolution    (   0x00000008)
//                     VideoCompression_PostFrameRate     (   0x00000009)
// ----------------------------------------------------------------------
// SUPPORT RANGES:     VideoCompression_KeyFrameRate      (0 ~      1000)
//                     VideoCompression_Quality           (0 ~     10000)
//                     VideoCompression_OverrideKeyFrame  (0 ~      1000)
//                     VideoCompression_BitRateMode       (0 ~         2)
//                     VideoCompression_BitRate           (0 ~ 100000000)
//                     VideoCompression_QPStep            (0 ~      1000)
//                     VideoCompression_PeakBitRate       (0 ~ 100000000)
//                     VideoCompression_TroughQuality     (0 ~     10000)
//                     VideoCompression_PostResolution    (             )
//                     VideoCompression_PostFrameRate     (             )
// ----------------------------------------------------------------------

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_CUSTOM_PROPERTY( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nProperty /*IN*/, ULONG * pValue /*OUT*/ );

// EXAMPLE: AMESDK_GET_CUSTOM_PROPERTY( hDev, KSPROPERTY_CUSTOM_XET_GPIO_DATA, &nValue );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_CUSTOM_PROPERTY( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nProperty /*IN*/, ULONG nValue /*IN*/ );

// EXAMPLE: AMESDK_SET_CUSTOM_PROPERTY( hDev, KSPROPERTY_CUSTOM_XET_GPIO_DATA, 0x00000000 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_CUSTOM_PROPERTY_EX( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nProperty /*IN*/, BYTE * pValue /*OUT*/, ULONG nBytes /*IN*/ );

// EXAMPLE: AMESDK_GET_CUSTOM_PROPERTY_EX( hDev, KSPROPERTY_CUSTOM_XET_ANALOG_VIDEO_SWITCH_TABLE, pValue, 12 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_CUSTOM_PROPERTY_EX( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nProperty /*IN*/, BYTE * pValue /*IN*/, ULONG nBytes /*IN*/ );

// EXAMPLE: AMESDK_SET_CUSTOM_PROPERTY_EX( hDev, KSPROPERTY_CUSTOM_XET_ANALOG_VIDEO_SWITCH_TABLE, pValue, 12 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_OTHER_SET_OSD_TEXT( ULONG iChannelNum /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, CHAR * pszString /*IN*/, CHAR * pszFontFamilyName /*IN*/, ULONG nFontStyle /*IN*/, ULONG nFontSize /*IN*/, DWORD dwFontColor /*IN*/, DWORD dwBackgroundColor /*IN*/, ULONG nTransparent /*IN*/ );

// EXAMPLE: AMESDK_OTHER_SET_OSD_TEXT( 0, 0, 0, 0, 0, 0, "Channel#01", "Arial", FontStyleRegular, 12, 0xFFFFFFFF, 0xFF000000, 0xFF );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_OTHER_SET_OSD_PICTURE( ULONG iChannelNum /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, CHAR * pszFilePathName /*IN*/, ULONG nTransparent /*IN*/ );

// EXAMPLE: AMESDK_OTHER_SET_OSD_PICTURE( 0, 0, 0, 0, 0, 0, "LOGO.BMP", 0xFF );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_OTHER_SET_OSD_BUFFER( ULONG iChannelNum /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, ULONG nColorSpaceType /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameWidth /*IN*/, ULONG nFrameHeight /*IN*/, ULONG nFramePitch /*IN*/, ULONG nTransparent /*IN*/ );

// EXAMPLE: AMESDK_OTHER_SET_OSD_BUFFER( 0, 0, 0, 0, 0, 0, MAKEFOURCC('Y', 'U', 'Y', '2'), p_src, 100, 100, 200, 0xFF );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_OTHER_SET_OSD_ALPHA_BLENDING( ULONG iChannelNum /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameColorSpaceType /*IN*/, ULONG nFrameWidth /*IN*/, ULONG nFrameHeight /*IN*/ );

// EXAMPLE: AMESDK_OTHER_SET_OSD_ALPHA_BLENDING( 0, pFrameBuffer, MAKEFOURCC('Y', 'U', 'Y', '2'), 720, 480 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_OTHER_REFRESH_DISPLAY_WINDOW( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_OTHER_REFRESH_DISPLAY_WINDOW( hDev );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_OTHER_SNAPSHOT_BMP( DEVICE_HANDLE hDevHandle /*IN*/, CHAR * pszFilePathName /*IN*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_OTHER_SNAPSHOT_BMP( hDev, "C:\\20070408.BMP" );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_OTHER_SNAPSHOT_BMP_EX( DEVICE_HANDLE hDevHandle /*IN*/, CHAR * pszFilePathName /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, ULONG nBitCount /*IN*/ );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_OTHER_SNAPSHOT_BMP_EX( DEVICE_HANDLE hDevHandle /*IN*/, CHAR * pszFilePathName /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, ULONG nBitCount /*IN*/, UINT nCropX /*IN*/, UINT nCropY /*IN*/, UINT nCropW /*IN*/, UINT nCropH /*IN*/, UINT nDstW /*IN*/, UINT nDstH /*IN*/ );

// EXAMPLE: AMESDK_OTHER_SNAPSHOT_BMP_EX( hDev, "C:\\20070408.BMP", po, 0x32595559, 720, 480, 16 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_OTHER_SNAPSHOT_JPG( DEVICE_HANDLE hDevHandle /*IN*/, CHAR * pszFilePathName /*IN*/, ULONG nQuality = 80 /*IN*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_OTHER_SNAPSHOT_JPG( hDev, "C:\\20070408.JPG", 80 ); // QUALITY = 0 ~ 100

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_OTHER_SNAPSHOT_JPG_EX( DEVICE_HANDLE hDevHandle /*IN*/, CHAR * pszFilePathName /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, ULONG nBitCount /*IN*/, ULONG nQuality = 80 /*IN*/ );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_OTHER_SNAPSHOT_JPG_EX( DEVICE_HANDLE hDevHandle /*IN*/, CHAR * pszFilePathName /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, ULONG nBitCount /*IN*/, UINT nCropX /*IN*/, UINT nCropY /*IN*/, UINT nCropW /*IN*/, UINT nCropH /*IN*/, UINT nDstW /*IN*/, UINT nDstH /*IN*/, ULONG nQuality = 80 /*IN*/ );

// EXAMPLE: AMESDK_OTHER_SNAPSHOT_JPG_EX( hDev, "C:\\20070408.JPG", po, 0x32595559, 720, 480, 16, 80 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_OTHER_SNAPSHOT_JPG_EX( DEVICE_HANDLE hDevHandle /*IN*/, BYTE * pStreamBuffer /*OUT*/, ULONG * pStreamBufferSize /*IN/OUT*/, BYTE * pFrameBuffer /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, ULONG nBitCount /*IN*/, ULONG nQuality = 80 /*IN*/ );

// EXAMPLE: RESERVED

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_OTHER_ZOOM( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nLeftPos = 0 /*IN*/, ULONG nTopPos = 0 /*IN*/, double dRate = 1.0 /*IN*/, ULONG nSubChannelNumber = 0 /*IN*/ );

// EXAMPLE: AMESDK_OTHER_ZOOM( hDev, 0, 0, 1.0 ); // ZOOM = 1 ~ 10

AFX_EXT_API BOOL AMESDK_EXPORT COLORSPACE_YUY2_TO_RGB32_MMX( ULONG cxo /*IN*/, ULONG cyo /*IN*/, ULONG cwo /*IN*/, ULONG cxe /*IN*/, ULONG cye /*IN*/, ULONG cwe /*IN*/, BYTE * pOutputFrameBuffer /*OUT*/, BYTE * pInputFrameBuffer /*IN*/, BOOL bHorizontalMirror = FALSE /*IN*/, BOOL bVerticalMirror = FALSE /*IN*/ );

// EXAMPLE: RESERVED

AFX_EXT_API BOOL AMESDK_EXPORT COLORSPACE_UYVY_TO_RGB32_MMX( ULONG cxo /*IN*/, ULONG cyo /*IN*/, ULONG cwo /*IN*/, ULONG cxe /*IN*/, ULONG cye /*IN*/, ULONG cwe /*IN*/, BYTE * pOutputFrameBuffer /*OUT*/, BYTE * pInputFrameBuffer /*IN*/, BOOL bHorizontalMirror = FALSE /*IN*/, BOOL bVerticalMirror = FALSE /*IN*/ );

// EXAMPLE: RESERVED

AFX_EXT_API BOOL AMESDK_EXPORT COLORSPACE_YV12_TO_RGB32_MMX( ULONG cxo /*IN*/, ULONG cyo /*IN*/, ULONG cwo /*IN*/, ULONG cxe /*IN*/, ULONG cye /*IN*/, ULONG cwe /*IN*/, BYTE * pOutputFrameBuffer /*OUT*/, BYTE * pInputFrameBuffer /*IN*/, BOOL bHorizontalMirror = FALSE /*IN*/, BOOL bVerticalMirror = FALSE /*IN*/ );

// EXAMPLE: RESERVED

AFX_EXT_API BOOL AMESDK_EXPORT COLORSPACE_YUY2_TO_RGB24_MMX( ULONG cxo /*IN*/, ULONG cyo /*IN*/, ULONG cwo /*IN*/, ULONG cxe /*IN*/, ULONG cye /*IN*/, ULONG cwe /*IN*/, BYTE * pOutputFrameBuffer /*OUT*/, BYTE * pInputFrameBuffer /*IN*/, BOOL bHorizontalMirror = FALSE /*IN*/, BOOL bVerticalMirror = FALSE /*IN*/ );

// EXAMPLE: RESERVED

AFX_EXT_API BOOL AMESDK_EXPORT COLORSPACE_YUY2_TO_YV12_MMX( ULONG cxo /*IN*/, ULONG cyo /*IN*/, ULONG cwo /*IN*/, ULONG cxe /*IN*/, ULONG cye /*IN*/, ULONG cwe /*IN*/, BYTE * pOutputFrameBuffer /*OUT*/, BYTE * pInputFrameBuffer /*IN*/, BOOL bHorizontalMirror = FALSE /*IN*/, BOOL bVerticalMirror = FALSE /*IN*/ );

// EXAMPLE: RESERVED

AFX_EXT_API BOOL AMESDK_EXPORT RESIZE_YV12_MMX( ULONG cxo /*IN*/, ULONG cyo /*IN*/, ULONG cwo /*IN*/, ULONG cxe /*IN*/, ULONG cye /*IN*/, ULONG cwe /*IN*/, BYTE * pOutputFrameBuffer /*OUT*/, BYTE * pInputFrameBufferr /*IN*/, ULONG nInterpolationMethod = 0x00000001 /*IN*/ );

// EXAMPLE: RESERVED

AFX_EXT_API PVOID AMESDK_EXPORT HELPER_MEMCPY( PVOID pDstBuffer /*IN*/, PVOID pSrcBuffer /*IN*/, ULONG nSize /*IN*/ );

// EXAMPLE: RESERVED

// ##############################################################################################################################################################################
// #
// # SOFTWARE CODEC DECODER/ENCODER FUNCTIONS
// #
// ##############################################################################################################################################################################
// 
#ifndef AMESDK_DISABLE_EXPORT_CODEC_FUNCTIONS

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_FORMAT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pColorSpaceType /*OUT*/, ULONG * pWidth /*OUT*/, ULONG * pHeight /*OUT*/, ULONG * pBitCount /*OUT*/, double * pFrameRate /*OUT*/, ULONG * pRecordMoe /*OUT*/, ULONG * pBitRate /*OUT*/, ULONG * pQuality /*OUT*/, ULONG * pGop /*OUT*/, ULONG * pInterlaceMode /*OUT*/, ULONG * pAspectRatio /*OUT*/, ULONG * pRecordComplexity = NULL /*OUT*/ );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_FORMAT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, ULONG nBitCount /*IN*/, double dFrameRate /*IN*/, ULONG nRecordMoe /*IN*/, ULONG nBitRate /*IN*/, ULONG nQuality /*IN*/, ULONG nGop /*IN*/, ULONG nInterlaceMode /*IN*/, ULONG nAspectRatio /*IN*/, ULONG nRecordComplexity = 0 /*IN*/ );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GET_FORMAT_EX( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pColorSpaceType /*OUT*/, ULONG * pWidth /*OUT*/, ULONG * pHeight /*OUT*/, ULONG * pBitCount, double * pFrameRate /*OUT*/, ULONG * pRecordProfile /*OUT*/, ULONG * pRecordLevel, ULONG * pRecordEntropy /*OUT*/, ULONG * pRecordMoe /*OUT*/, ULONG * pBitRate /*OUT*/, ULONG * pQuality /*OUT*/, ULONG * pGop /*OUT*/, ULONG * pInterlaceMode /*OUT*/, ULONG * pBFrames /*OUT*/, ULONG * pAspectRatio /*OUT*/, ULONG * pRecordComplexity /*OUT*/ );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_FORMAT_EX( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, ULONG nBitCount /*IN*/, double dFrameRate /*IN*/, ULONG nRecordProfile /*IN*/, ULONG nRecordLevel /*IN*/, ULONG nRecordEntropy /*IN*/, ULONG nRecordMoe /*IN*/, ULONG nBitRate /*IN*/, ULONG nQuality /*IN*/, ULONG nGop /*IN*/, ULONG nInterlaceMode /*IN*/, ULONG nBFrames /*IN*/, ULONG nAspectRatio /*IN*/, ULONG nRecordComplexity /*IN*/ );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_SET_FORMAT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nChannels /*IN*/, ULONG nBitsPerSample /*IN*/, ULONG nSamplesPerSec /*IN*/, ULONG nAACOutputADTS );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_CODEC_ENCODE( DEVICE_HANDLE hDevHandle /*IN*/, BYTE * pSrcFrameBuffer /*IN*/, ULONG nSrcFrameColorSpaceType /*IN*/, ULONG nSrcFrameWidth /*IN*/, ULONG nSrcFrameHeight /*IN*/, BYTE * * ppDstStreamBuffer /*OUT*/, ULONG * pDstStreamBufferSize /*OUT*/, BOOL * pIsKeyFrame /*OUT*/, BOOL bForceKeyFrame = FALSE /*IN*/ );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_CODEC_ENCODE( DEVICE_HANDLE hDevHandle /*IN*/, BYTE * pSrcFrameBuffer /*IN*/, ULONG nSrcFrameColorSpaceType /*IN*/, ULONG nSrcFrameWidth /*IN*/, ULONG nSrcFrameHeight /*IN*/, ULONG nSrcFramePitch /*IN*/, ULONG nCropX /*IN*/, ULONG nCropY /*IN*/, ULONG nCropW /*IN*/, ULONG nCropH /*IN*/, BYTE * * ppDstStreamBuffer /*OUT*/, ULONG * pDstStreamBufferSize /*OUT*/, BOOL * pIsKeyFrame /*OUT*/, BOOL bForceKeyFrame = FALSE /*IN*/ );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_CODEC_ENCODE( DEVICE_HANDLE hDevHandle /*IN*/, BYTE * pSrcFrameBuffer /*IN*/, ULONG nSrcFrameBufferSize /*IN*/, BYTE ** ppDstStreamBuffer /*OUT*/, ULONG * pDstStreamBufferSize /*OUT*/, ULONGLONG nSrcnTimeStamp = 0x0000000000000000 /*IN*/, ULONGLONG * pDstTimeStamp = NULL /*OUT*/ );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_CODEC_DECODE( DEVICE_HANDLE hDevHandle /*IN*/, BYTE * pSrcStreamBuffer /*IN*/, ULONG nSrcStreamBufferSize /*IN*/, BOOL bIsKeyFrame /*IN*/ );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_CODEC_DECODE_EX( DEVICE_HANDLE hDevHandle /*IN*/, BYTE * pSrcStreamBuffer /*IN*/, ULONG nSrcStreamBufferSize /*IN*/, BOOL bIsKeyFrame /*IN*/, BYTE * pDstFrameBuffer /*OUT*/, ULONG * pDstFrameBufferSize /*OUT*/ );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_CODEC_IS_HARDWARE_ACCELERATION_SUPPORT();

#endif // AMESDK_DISABLE_EXPORT_CODEC_FUNCTIONS

// ##############################################################################################################################################################################
// #
// # NETWORK SOURCE/RENDERER FUNCTIONS
// #
// ##############################################################################################################################################################################
// 
#ifndef AMESDK_DISABLE_EXPORT_NETWORK_FUNCTIONS

typedef BOOL (AMESDK_EXPORT *PF_NETWORK_JOIN_SESSION_CALLBACK)( ULONG nStreamNumber, CHAR * pszUserName, ULONG dwClientAddress, PVOID pUserData );

typedef BOOL (AMESDK_EXPORT *PF_NETWORK_LEAVE_SESSION_CALLBACK)( ULONG nStreamNumber, CHAR * pszUserName, ULONG dwClientAddress, PVOID pUserData );

typedef BOOL (AMESDK_EXPORT *PF_NETWORK_SET_CUSTOM_PROPERTY_CALLBACK)( ULONG nStreamNumber, CHAR * pszProperty, CHAR * pszValue, PVOID pUserData );

typedef CHAR * (AMESDK_EXPORT *PF_NETWORK_GET_CUSTOM_PROPERTY_CALLBACK)( ULONG nStreamNumber, CHAR * pszProperty, PVOID pUserData );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_NETWORK_SET_VIDEO_STREAM_FORMAT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nStreamNumber /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, ULONG nBitCount /*IN*/, double dFrameRate /*IN*/, ULONG nBandwidth /*IN*/, USHORT wUdpStartPort = 6790 /*IN*/ );

// EXAMPLE: AMESDK_NETWORK_SET_VIDEO_STREAM_FORMAT( hDev, 0, 0x34363248 /*H264*/, 704, 480, 24, 29.970000, 4000000 * 1 );

// EXAMPLE: AMESDK_NETWORK_SET_VIDEO_STREAM_FORMAT( hDev, 0, 0x34363248 /*H264*/, 704, 576, 24, 25.000000, 4000000 * 1 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_NETWORK_SET_AUDIO_STREAM_FORMAT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nStreamNumber /*IN*/, ULONG nStreamType /*IN*/, ULONG nChannels /*IN*/, ULONG nBitsPerSample /*IN*/, ULONG nSamplesPerSec /*IN*/, ULONG nBandwidth /*IN*/, USHORT wUdpStartPort = 6790 /*IN*/ );

// EXAMPLE: AMESDK_NETWORK_SET_AUDIO_STREAM_FORMAT( hDev, 0, 0x00000000 /*PCM*/, 1, 16,  8000,  128000 * 1 );

// EXAMPLE: AMESDK_NETWORK_SET_AUDIO_STREAM_FORMAT( hDev, 0, 0x00000000 /*PCM*/, 2, 16, 48000, 1536000 * 1 );

// EXAMPLE: AMESDK_NETWORK_SET_AUDIO_STREAM_FORMAT( hDev, 0, 0x00000001 /*AAC*/, 1, 16,  8000,  128000 * 1 );
//
// #######################################################################################################
// #                                                                                                     #
// # About AAC License                                                                                   #
// #                                                                                                     #
// # This software development kit contains software programming code and libraries related to AAC       #
// #                                                                                                     #
// # function that is subject to your development use only. All the intellectual properties are held     #
// #                                                                                                     #
// # by individual license groups and/or the property holders. License and royalty fee should be charged #
// #                                                                                                     #
// # by the license groups and/or property holders and paid by you if any of them is implemented into    #
// #                                                                                                     #
// # your commercial product. We will not be responsible for any illegal usage and/or any infringement   #
// #                                                                                                     #
// # of the rights of the individual owners of these intellectual properties.                            #
// #                                                                                                     #
// # Details of AAC could be found in the website:                                                       #
// #                                                                                                     #
// # http://www.vialicensing.com/licensing/aac-overview.aspx                                             #
// #                                                                                                     #
// #######################################################################################################
 
AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_NETWORK_SET_VIDEO_STREAM_BUFFER( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nStreamNumber /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferSize /*IN*/, BOOL bIsKeyFrame /*IN*/, LONGLONG nDelayTime = 0x0000000000000000 /*IN*/, ULONGLONG nTimeStamp = 0x0000000000000000 /*IN*/ );

// EXAMPLE: AMESDK_NETWORK_SET_VIDEO_STREAM_BUFFER( hDev, 0, po, 0x00096000, TRUE );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_NETWORK_SET_AUDIO_STREAM_BUFFER( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nStreamNumber /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferSize /*IN*/, LONGLONG nDelayTime = 0x0000000000000000 /*IN*/, ULONGLONG nTimeStamp = 0x0000000000000000 /*IN*/ );

// EXAMPLE: AMESDK_NETWORK_SET_AUDIO_STREAM_BUFFER( hDev, 0, po, 320 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_NETWORK_GET_VIDEO_STREAM_STATISTICS( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nStreamNumber /*IN*/, ULONG * pBufferQueueSize /*IN*/, ULONG * pFrameQueueSize /*IN*/, ULONG * pClients /*IN*/ );

// EXAMPLE: AMESDK_NETWORK_GET_VIDEO_STREAM_STATISTICS( hDev, 0, &nBufferQueueSize, &nFrameQueueSize, &nClients );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_NETWORK_GET_AUDIO_STREAM_STATISTICS( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nStreamNumber /*IN*/, ULONG * pBufferQueueSize /*IN*/, ULONG * pFrameQueueSize /*IN*/, ULONG * pClients /*IN*/ );

// EXAMPLE: AMESDK_NETWORK_GET_AUDIO_STREAM_STATISTICS( hDev, 0, &nBufferQueueSize, &nFrameQueueSize, &nClients );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_NETWORK_SET_STREAMING_ADAPTER( DEVICE_HANDLE hDevHandle, CHAR * pszOutgoingNetworkInterface /*IN*/ );

// EXAMPLE: AMESDK_NETWORK_SET_STREAMING_ADAPTER( hDev, "127.0.0.1" );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_NETWORK_SET_STREAMING_PORT( DEVICE_HANDLE hDevHandle /*IN*/, USHORT wRtxpPort /*IN*/, ULONG nReclamationTestSeconds = 60 /*IN*/, USHORT wRtxpOverHttpPort = 0 /*IN*/ );

// EXAMPLE: AMESDK_NETWORK_SET_STREAMING_PORT( hDev, 554 );

// EXAMPLE: AMESDK_NETWORK_SET_STREAMING_PORT( hDev, 554, 60, 8080 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_NETWORK_SET_STREAMING_FOLDER( DEVICE_HANDLE hDevHandle /*IN*/, CHAR * pszWebServerRootFolderPath /*IN*/, CHAR * pszSubFolderPath /*IN*/ );

// EXAMPLE: AMESDK_NETWORK_SET_STREAMING_FOLDER( hDev, "C:\\APACHE\\WWW\\", "HLS\\" );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_NETWORK_SET_USER_ACCOUNT( DEVICE_HANDLE hDevHandle /*IN*/, CHAR * pszUserName /*IN*/, CHAR * pUserPassword /*IN*/ );

// EXAMPLE: AMESDK_NETWORK_SET_USER_ACCOUNT( hDev, "root", "root" );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_NETWORK_SET_CALLBACK( DEVICE_HANDLE hDevHandle /*IN*/, PF_NETWORK_JOIN_SESSION_CALLBACK pJoinSessionCB /*IN*/, PF_NETWORK_LEAVE_SESSION_CALLBACK pLeaveSessionCB /*IN*/, PF_NETWORK_SET_CUSTOM_PROPERTY_CALLBACK pSetCustomPropertyCB /*IN*/, PF_NETWORK_GET_CUSTOM_PROPERTY_CALLBACK pGetCustomPropertyCB /*IN*/, PVOID pUserData /*IN*/ );

// EXAMPLE: AMESDK_NETWORK_SET_CALLBACK( hDev, on_join_session, on_leave_session, on_set_custom_property, on_get_custom_property, this );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_NETWORK_SET_CUSTOM_PROPERTY( DEVICE_HANDLE hDevHandle /*IN*/, CHAR * pszProperty /*IN*/, CHAR * pszValue /*IN*/, ULONG nBytes /*IN*/ );

// EXAMPLE: AMESDK_NETWORK_SET_CUSTOM_PROPERTY( hDev, "ptz", p_cmd, sizeof(p_cmd) );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_NETWORK_GET_CUSTOM_PROPERTY( DEVICE_HANDLE hDevHandle /*IN*/, CHAR * pszProperty /*IN*/, CHAR * pszValue /*OUT*/, ULONG nBytes /*IN*/ );

// EXAMPLE: AMESDK_NETWORK_GET_CUSTOM_PROPERTY( hDev, "gps", p_gps, sizeof(p_gps) );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_NETWORK_CONNECT_STREAMING_SERVER( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nProtocol = 0x00000000 /*IN*/, USHORT wUdpStartPort = 0 /*IN*/ );

// EXAMPLE: AMESDK_NETWORK_CONNECT_STREAMING_SERVER( hDev, 0 ); // UDP

// EXAMPLE: AMESDK_NETWORK_CONNECT_STREAMING_SERVER( hDev, 1 ); // TCP

// EXAMPLE: AMESDK_NETWORK_CONNECT_STREAMING_SERVER( hDev, 2 ); // HTTP

#endif // AMESDK_DISABLE_EXPORT_NETWORK_FUNCTIONS

// ##############################################################################################################################################################################
// #
// # FILE SOURCE/RENDERER FUNCTIONS
// #
// ##############################################################################################################################################################################
// 
#ifndef AMESDK_DISABLE_EXPORT_FILE_FUNCTIONS

#include <AFXTEMPL.H>

#pragma pack( 1 )

typedef struct _AMESDK_FILE_INFO
{
	CHAR		m_pszFileName[ MAX_PATH ];

	ULONGLONG   m_nFileSize;

	ULONGLONG	m_nFileStartTime;

	ULONGLONG	m_nFileStopTime;

	ULONGLONG	m_nVideoStreamStartTime;

	ULONGLONG	m_nVideoStreamStopTime;

	ULONGLONG	m_nAudioStreamStartTime;

	ULONGLONG	m_nAudioStreamStopTime;

	ULONG		m_nVideoFrameNumber;

	ULONG		m_nVideoColorSpaceType;

	ULONG		m_nVideoWidth;

	ULONG		m_nVideoHeight;

	ULONG		m_nVideoBitCount;

	double		m_dVideoFrameRate;

	DWORD		m_dwVideoCustomFlags;

	ULONG		m_nVideoMotionRatio; // RESERVED

	ULONG		m_nAudioFrameNumber;

	ULONG		m_nAudioChannels;

	ULONG		m_nAudioBitsPerSample;

	ULONG		m_nAudioSamplesPerSec;

	ULONG		m_nAudioSamplesPerStreamBuffer; // RESERVED

	DWORD		m_dwReserved; // RESERVED

} AMESDK_FILE_INFO;

#pragma pack(   )

typedef struct _AMESDK_SECTION_INFO
{
	ULONG       m_nStartSample;

	ULONG       m_nStopSample;

	ULONGLONG	m_nStartTimeStamp;

	ULONGLONG	m_nStopTimeStamp;

	ULONG		m_nSectionNumber;

} AMESDK_SECTION_INFO;

typedef struct _AMESDK_MOTION_INFO
{
	ULONG       m_nStartSample;

	ULONG       m_nStopSample;

	ULONGLONG	m_nStartTimeStamp;

	ULONGLONG	m_nStopTimeStamp;

	BYTE		m_nStrength;

} AMESDK_MOTION_INFO;

typedef struct _AMESDK_GPS_INFO
{
	ULONG       m_nStartSample;

	ULONG       m_nStopSample;

	ULONGLONG	m_nStartTimeStamp;

	ULONGLONG	m_nStopTimeStamp;

	double      m_dLongitude;

	double   	m_dLatitude;

	double		m_dSpeed;

	double		m_dAngle;

	BOOL		m_bIsVaild;

} AMESDK_GPS_INFO;

typedef struct _AMESDK_POS_INFO
{
	ULONG       m_nStartSample;

	ULONG       m_nStopSample;

	ULONGLONG	m_nStartTimeStamp;

	ULONGLONG	m_nStopTimeStamp;

} AMESDK_POS_INFO;

#define AMESDK_FILE_CUSTOMFLAG_ISINTERLEAVED           0x00000100

#define AMESDK_FILE_CUSTOMFLAG_ISSYNCHRONIZATION       0x01000000

#define AMESDK_FILE_CUSTOMFLAG_ISSYNCHRONIZATION_VIDEO 0x01000000

#define AMESDK_FILE_CUSTOMFLAG_ISSYNCHRONIZATION_AUDIO 0x04000000

#define AMESDK_FILE_CUSTOMFLAG_NONINDEX                0x02000000

#define AMESDK_FILE_CUSTOMFLAG_MP4_SPECIAL             0x08000000

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_VIDEO_STREAM_FORMAT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pColorSpaceType /*OUT*/, ULONG * pWidth /*OUT*/, ULONG * pHeight /*OUT*/, ULONG * pBitCount /*OUT*/, double * pFrameRate /*OUT*/, DWORD * pCustomFlags /*OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_VIDEO_STREAM_FORMAT( hDev, &nColorSpaceType, &nWidth, &nHeight, &nBitCount, &dFrameRate, &dwCustomFlags );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_SET_VIDEO_STREAM_FORMAT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, ULONG nBitCount /*IN*/, double dFrameRate /*IN*/, DWORD dwCustomFlags /*IN*/ );

// EXAMPLE: AMESDK_FILE_SET_VIDEO_STREAM_FORMAT( hDev, 0x44495658 /*XVID*/, 704, 480, 24, 29.970000, AMESDK_FILE_CUSTOMFLAG_ISINTERLEAVED );

// EXAMPLE: AMESDK_FILE_SET_VIDEO_STREAM_FORMAT( hDev, 0x34363248 /*H264*/, 704, 480, 24, 29.970000, AMESDK_FILE_CUSTOMFLAG_ISINTERLEAVED | AMESDK_FILE_CUSTOMFLAG_ISSYNCHRONIZATION );

// EXAMPLE: AMESDK_FILE_SET_VIDEO_STREAM_FORMAT( hDev, 0x44495658 /*XVID*/, 352, 288, 24, 25.000000, AMESDK_FILE_CUSTOMFLAG_ISSYNCHRONIZATION );

// EXAMPLE: AMESDK_FILE_SET_VIDEO_STREAM_FORMAT( hDev, 0x44495658 /*XVID*/, 352, 288, 24, 25.000000, AMESDK_FILE_CUSTOMFLAG_NONINDEX );

// EXAMPLE: AMESDK_FILE_SET_VIDEO_STREAM_FORMAT( hDev, 0x34363248 /*H264*/, 352, 288, 24, 25.000000, 0x00000000 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_AUDIO_STREAM_FORMAT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pStreamType /*OUT*/, ULONG * pChannels /*OUT*/, ULONG * pBitsPerSample /*OUT*/, ULONG * pSamplesPerSec /*OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_AUDIO_STREAM_FORMAT( hDev, &StreamType, &nChannels, &nBitsPerSample, &nSamplesPerSec );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_SET_AUDIO_STREAM_FORMAT( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nStreamType /*IN*/, ULONG nChannels /*IN*/, ULONG nBitsPerSample /*IN*/, ULONG nSamplesPerSec /*IN*/ );

// EXAMPLE: AMESDK_FILE_SET_AUDIO_STREAM_FORMAT( hDev, 0x00000000 /*PCM*/, 1, 16, 48000 );

// EXAMPLE: AMESDK_FILE_SET_AUDIO_STREAM_FORMAT( hDev, 0x00000001 /*AAC*/, 2, 16, 8000 );
//
// #######################################################################################################
// #                                                                                                     #
// # About AAC License                                                                                   #
// #                                                                                                     #
// # This software development kit contains software programming code and libraries related to AAC       #
// #                                                                                                     #
// # function that is subject to your development use only. All the intellectual properties are held     #
// #                                                                                                     #
// # by individual license groups and/or the property holders. License and royalty fee should be charged #
// #                                                                                                     #
// # by the license groups and/or property holders and paid by you if any of them is implemented into    #
// #                                                                                                     #
// # your commercial product. We will not be responsible for any illegal usage and/or any infringement   #
// #                                                                                                     #
// # of the rights of the individual owners of these intellectual properties.                            #
// #                                                                                                     #
// # Details of AAC could be found in the website:                                                       #
// #                                                                                                     #
// # http://www.vialicensing.com/licensing/aac-overview.aspx                                             #
// #                                                                                                     #
// #######################################################################################################
 
AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_SET_VIDEO_STREAM_BUFFER( DEVICE_HANDLE hDevHandle /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferSize /*IN*/, BOOL bIsKeyFrame /*IN*/, LONGLONG nDelayTime = 0x0000000000000000 /*IN*/, ULONGLONG nTimeStamp = 0x0000000000000000 /*IN*/, AMESDK_SECTION_INFO * pSectionData = NULL /*IN*/, AMESDK_MOTION_INFO * pMotionData = NULL /*IN*/, AMESDK_GPS_INFO * pGpsData = NULL /*IN*/, AMESDK_POS_INFO * pPosData = NULL /*IN*/ );

// EXAMPLE: AMESDK_FILE_SET_VIDEO_STREAM_BUFFER( hDev, po, 0x00096000, TRUE );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_SET_AUDIO_STREAM_BUFFER( DEVICE_HANDLE hDevHandle /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferSize /*IN*/, LONGLONG nDelayTime = 0x0000000000000000 /*IN*/, ULONGLONG nTimeStamp = 0x0000000000000000 /*IN*/, AMESDK_SECTION_INFO * pSectionData = NULL /*IN*/ );

// EXAMPLE: AMESDK_FILE_SET_AUDIO_STREAM_BUFFER( hDev, po, 1920 );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_VIDEO_STREAM_BUFFER( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nSample /*IN*/, BYTE * pStreamBuffer /*OUT*/, ULONG * pStreamBufferSize /*IN/OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_VIDEO_STREAM_BUFFER( hDev, 0, pStreamBuffer, &nStreamBufferSize );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_AUDIO_STREAM_BUFFER( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nSample /*IN*/, BYTE * pStreamBuffer /*OUT*/, ULONG * pStreamBufferSize /*IN/OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_AUDIO_STREAM_BUFFER( hDev, 0, pStreamBuffer, &nStreamBufferSize );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_VIDEO_STREAM_DATA( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nSample /*IN*/, ULONGLONG * pTimeStamp /*OUT*/, BOOL * pIsKeyFrame /*OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_VIDEO_STREAM_DATA( hDev, 0, &nTimeStamp, &bIsKeyFrame );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_AUDIO_STREAM_DATA( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nSample /*IN*/, ULONGLONG * pTimeStamp /*OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_AUDIO_STREAM_DATA( hDev, 0, &nTimeStamp );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_VIDEO_STREAM_TIMESTAMP_MAP( DEVICE_HANDLE hDevHandle /*IN*/, ULONGLONG * * ppTimeStampMap /*OUT*/, ULONG * pTimeStampMapSize /*OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_VIDEO_STREAM_TIMESTAMP_MAP( hDev, &pTimeStampMap, &nTimeStampMapSize );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_AUDIO_STREAM_TIMESTAMP_MAP( DEVICE_HANDLE hDevHandle /*IN*/, ULONGLONG * * ppTimeStampMap /*OUT*/, ULONG * pTimeStampMapSize /*OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_AUDIO_STREAM_TIMESTAMP_MAP( hDev, &pTimeStampMap, &nTimeStampMapSize );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_VIDEO_STREAM_SECTION_MAP( DEVICE_HANDLE hDevHandle /*IN*/, AMESDK_SECTION_INFO * * ppSectionMap /*OUT*/, ULONG * pSectionMapSize /*OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_VIDEO_STREAM_SECTION_MAP( hDev, &pSectionMap, &nSectionMapSize );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_AUDIO_STREAM_SECTION_MAP( DEVICE_HANDLE hDevHandle /*IN*/, AMESDK_SECTION_INFO * * ppSectionMap /*OUT*/, ULONG * pSectionMapSize /*OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_AUDIO_STREAM_SECTION_MAP( hDev, &pSectionMap, &nSectionMapSize );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_VIDEO_STREAM_MOTION_MAP( DEVICE_HANDLE hDevHandle /*IN*/, AMESDK_MOTION_INFO * * ppMotionMap /*OUT*/, ULONG * pMotionMapSize /*OUT*/, ULONG * pMotionRatio /*OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_VIDEO_STREAM_MOTION_MAP( hDev, &pMotionMap, &nMotionMapSize, &nMotionRatio );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_VIDEO_STREAM_GPS_MAP( DEVICE_HANDLE hDevHandle /*IN*/, AMESDK_GPS_INFO * * ppGpsMap /*OUT*/, ULONG * pGpsMapSize /*OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_VIDEO_STREAM_GPS_MAP( hDev, &pGpsMap, &nGpsMapSize );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_VIDEO_STREAM_POS_MAP( DEVICE_HANDLE hDevHandle /*IN*/, AMESDK_POS_INFO * * ppPosMap /*OUT*/, ULONG * pPosMapSize /*OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_VIDEO_STREAM_POS_MAP( hDev, &pPosMap, &nPosMapSize );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_MEDIA_LENGTH( DEVICE_HANDLE hDevHandle /*IN*/, LONGLONG * pLength /*OUT*/, DWORD dwTimeUnits /*IN*/ );

// EXAMPLE: AMESDK_FILE_GET_MEDIA_LENGTH( hDev, &nLength, 0x00000000 ); // TIME_FORMAT_MEDIA_TIME

// EXAMPLE: AMESDK_FILE_GET_MEDIA_LENGTH( hDev, &nLength, 0x00000001 ); // TIME_FORMAT_FRAME

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_MEDIA_POSITION( DEVICE_HANDLE hDevHandle /*IN*/, LONGLONG * pPosition /*OUT*/, DWORD dwTimeUnits /*IN*/ );

// EXAMPLE: AMESDK_FILE_GET_MEDIA_POSITION( hDev, &nPosition, 0x00000000 ); // TIME_FORMAT_MEDIA_TIME

// EXAMPLE: AMESDK_FILE_GET_MEDIA_POSITION( hDev, &nPosition, 0x00000001 ); // TIME_FORMAT_FRAME

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_SET_MEDIA_POSITION( DEVICE_HANDLE hDevHandle /*IN*/, LONGLONG nPosition /*IN*/, DWORD dwTimeUnits /*IN*/ );

// EXAMPLE: AMESDK_FILE_SET_MEDIA_POSITION( hDev, nPosition, 0x00000000 ); // TIME_FORMAT_MEDIA_TIME

// EXAMPLE: AMESDK_FILE_SET_MEDIA_POSITION( hDev, nPosition, 0x00000001 ); // TIME_FORMAT_FRAME

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_MEDIA_PLAYBACK_RATE( DEVICE_HANDLE hDevHandle /*IN*/, double * pRate /*OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_MEDIA_PLAYBACK_RATE( hDev, &dRate );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_SET_MEDIA_PLAYBACK_RATE( DEVICE_HANDLE hDevHandle /*IN*/, double dRate /*IN*/ );

// EXAMPLE: AMESDK_FILE_SET_MEDIA_PLAYBACK_RATE( hDev, dRate );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_GET_MEDIA_PLAYBACK_SKIP_MODE( DEVICE_HANDLE hDevHandle /*IN*/, ULONG * pSkipMode /*IN*/ );

// EXAMPLE: AMESDK_FILE_GET_MEDIA_PLAYBACK_SKIP_MODE( hDev, &nSkipMpde );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_SET_MEDIA_PLAYBACK_SKIP_MODE( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nSkipMode /*IN*/ );

// EXAMPLE: AMESDK_FILE_SET_MEDIA_PLAYBACK_SKIP_MODE( hDev, nSkipMpde );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_SEEK( DEVICE_HANDLE hDevHandle /*IN*/, LONG nSeeks /*IN*/ );

// EXAMPLE: AMESDK_FILE_SEEK( hDev, nSeeks );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_FLUSH( DEVICE_HANDLE hDevHandle /*IN*/ );

// EXAMPLE: AMESDK_FILE_FLUSH( hDev );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_EXPORT( DEVICE_HANDLE hDevHandle /*IN*/, ULONGLONG nStartSampleTime /*IN*/, ULONGLONG nStopSampleTime /*IN*/, CHAR * pszExportedFileName /*IN*/ );

// EXAMPLE: AMESDK_FILE_EXPORT( hDev, 50000000, 300000000, "EXPORT.MP4" );

#ifndef AMESDK_DISABLE_MFC_SUPPORT

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_DELETE_THE_OLDEST_FILE( CArray<AMESDK_FILE_INFO, AMESDK_FILE_INFO> * pFileInfoList /*IN/OUT*/ );

// EXAMPLE: AMESDK_FILE_DELETE_THE_OLDEST_FILE( pFileInfoList );

AFX_EXT_API CArray<AMESDK_FILE_INFO, AMESDK_FILE_INFO> * AMESDK_EXPORT AMESDK_FILE_GET_FILE_INFO_LIST( CHAR * pszSrcDirectory /*IN*/, ULONGLONG nStartSearchSystemTime /*IN*/, ULONGLONG nStopSearchSystemTime /*IN*/, BOOL bAutoRepair = FALSE /*IN*/, volatile ULONG * pSearchProgress = NULL /*OUT*/, volatile ULONG * pRepairProgress = NULL /*OUT*/ );

// EXAMPLE: pFileInfoList = AMESDK_FILE_GET_FILE_INFO_LIST( "D:\\SC\\", nStartSearchSystemTime, nStopSearchSystemTime ); // FILE_INFO_LIST_RESET

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_UPDATE_FILE_INFO_LIST( CArray<AMESDK_FILE_INFO, AMESDK_FILE_INFO> * pFileInfoList /*IN/OUT*/, CHAR * pszSrcDirectory /*IN*/, ULONGLONG nStartSearchSystemTime /*IN*/, ULONGLONG nStopSearchSystemTime /*IN*/, BOOL bAutoRepair = FALSE /*IN*/, volatile ULONG * pSearchProgress = NULL /*OUT*/, volatile ULONG * pRepairProgress = NULL /*OUT*/ );

// EXAMPLE: AMESDK_FILE_GET_FILE_INFO_LIST( pFileInfoList, "D:\\SC\\", nStartSearchSystemTime, nStopSearchSystemTime, 0x00000001 ); // FILE_INFO_LIST_UPDATE

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_FREE_FILE_INFO_LIST( CArray<AMESDK_FILE_INFO, AMESDK_FILE_INFO> * pFileInfoList /*IN/OUT*/ );

// EXAMPLE: AMESDK_FILE_FREE_FILE_INFO_LIST( pFileInfoList );

#endif

AFX_EXT_API PVOID AMESDK_EXPORT AMESDK_FILE_EXPORT_START( CHAR * pszSrcDirectory /*IN*/, CHAR * pszDstDirectory /*IN*/, ULONGLONG nStartExportSystemTime /*IN*/, ULONGLONG nStopExportSystemTime /*IN*/, ULONGLONG nMaxExportFileSize = 1073741824 /*1GB*/ /*IN*/, volatile ULONG * pExportProgress = NULL /*OUT*/ );

// EXAMPLE: pFileRenderer = AMESDK_FILE_EXPORT_START( "D:\\SC\\", "E:\\BACKUP\\", nStartExportSystemTime, nStopExportSystemTime, 8 * 1024 * 1024 * 1024 /*  8GB*/, &nExportProgress );

// EXAMPLE: pFileRenderer = AMESDK_FILE_EXPORT_START( "D:\\SC\\", "E:\\BACKUP\\", nStartExportSystemTime, nStopExportSystemTime, 1 * 1024 * 1024 * 1024 /*  1GB*/, &nExportProgress );

// EXAMPLE: pFileRenderer = AMESDK_FILE_EXPORT_START( "D:\\SC\\", "E:\\BACKUP\\", nStartExportSystemTime, nStopExportSystemTime,      512 * 1024 * 1024 /*512MB*/, &nExportProgress );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_EXPORT_STOP( PVOID pDstFileRenderer /*IN*/ );

// EXAMPLE: AAMESDK_FILE_EXPORT_STOP( pDstFileRenderer );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_EXPORT_COPY( PVOID pDstFileRenderer /*IN*/, DEVICE_HANDLE hSrcFileRenderer /*IN*/, ULONGLONG nStartExportSystemTime /*IN*/, ULONGLONG nStopExportSystemTime /*IN*/, BOOL bFlush = FALSE /*IN*/ );

// EXAMPLE: AMESDK_FILE_EXPORT_COPY( pDstFileRenderer, hSrcFileRenderer, nStartExportSystemTime, nStopExportSystemTime, FALSE );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_SELF_REPAIR( CHAR * pszSrcFileName /*IN*/, volatile ULONG * pRepairProgress = NULL /*OUT*/ );

// EXAMPLE: AMESDK_FILE_SELF_REPAIR( "C:\\00000000.AVI" );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_REPAIR( CHAR * pszSrcFileName /*IN*/, CHAR * pszDstFileName = NULL /*IN*/, volatile ULONG * pRepairProgress = NULL /*OUT*/ );

// EXAMPLE: AMESDK_FILE_REPAIR( "C:\\00000000.AVI", "C:\\00000001.AVI" );

// EXAMPLE: AMESDK_FILE_REPAIR( "C:\\00000000.AVI" );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_DIAGNOSIS( CHAR * pszFileName /*IN*/, BOOL * pIsHealthy /*OUT*/, BOOL * pHasIndex /*OUT*/ );

// EXAMPLE: AMESDK_FILE_DIAGNOSIS( "C:\\00000000.AVI", &bIsHealthy, &bHasIndex );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_FILE_STD_RESTORE( CHAR * pszSrcFileName /*IN*/, CHAR * pszDstFileName /*IN*/, volatile ULONG * pRestoreProgress = NULL /*OUT*/ );

// EXAMPLE: RESERVED (CONVERT OUR AVI TO STANDARD AVI)

#endif // AMESDK_DISABLE_EXPORT_FILE_FUNCTIONS

// ##############################################################################################################################################################################
// #
// # GPS SOURCE FUNCTIONS
// #
// ##############################################################################################################################################################################
// 
#ifndef AMESDK_DISABLE_EXPORT_GPS_FUNCTIONS

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_GPS_GET_DATA( DEVICE_HANDLE hDevHandle /*IN*/, double * pLongitude /*OUT*/, double * pLatitude /*OUT*/, double * pSpeed /*OUT*/, double * pAngle /*OUT*/, BOOL * pIsVaild /*OUT*/ );

#endif // AMESDK_DISABLE_EXPORT_GPS_FUNCTIONS

// ##############################################################################################################################################################################
// #
// # GPS SOURCE FUNCTIONS
// #
// ##############################################################################################################################################################################
// 
#ifndef AMESDK_DISABLE_EXPORT_DEINTERLACER_FUNCTIONS

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_DI_DEINTERLACE_METHOD( DEVICE_HANDLE hDevHandle /*IN*/, ULONG nDeinterlaceType /*IN*/ );

AFX_EXT_API BOOL AMESDK_EXPORT AMESDK_DI_DEINTERLACE( DEVICE_HANDLE hDevHandle /*IN*/, BYTE * pFrameBuffer /*IN/OUT*/, ULONG nFrameBufferSize /*IN*/ );

#endif // AMESDK_DISABLE_EXPORT_DEINTERLACER_FUNCTIONS

#endif // AMESDK_H
