#ifndef __J_VIDEOADAPTER_H_
#define __J_VIDEOADAPTER_H_

#include "j_obj.h"
#include "j_type.h"
#include "x_ringbuffer.h"
#include "x_log.h"
#include "x_errtype.h"
#include "x_lock.h"

///任务
class J_Task : virtual public J_Obj
{
	public:
		void *m_pParam;

		///任务的工作函数,由使用者实现
		///@return 参见x_error_type.h
		virtual int Run() = 0;
};
typedef std::queue<J_Task *> j_queue_task;

struct J_DevAdapter : virtual public J_Obj
{
	///获得设备状态
	///@return 		READY-已经登录, BROKEN-断线或者未登录状态
	virtual J_DevStatus GetStatus() const  = 0;

	///断线通知
	///@return 		参见x_error_type.h
	virtual int Broken() = 0;

	///构造视频通道对象
	///@param[in] 	pResid 全局唯一标识
	///@param[out] pObj 构造对象的返回值
	///@param[in]	pOwner channel对象所属的adapter
	///@param[in]	nChannel 通道号
	///@param[in] 	nStream 码流类型, 0-主码流, 1-子码流
	///@param[in] 	nMode 传输方式 0-TCP, 1-UDP
	///@return 		参见x_error_type.h
	virtual int MakeChannel(const j_char_t *pResid, J_Obj *&pObj, J_Obj *pOwner, j_int32_t nChannel, j_int32_t nStream = 0, j_int32_t nMode = 0) = 0;
};

struct J_DevInput : virtual public J_Obj
{
	///报警布防
	///@return 		参见x_error_type.h
	virtual j_result_t EnableAlarm() = 0;

	///报警撤防
	///@return 		参见x_error_type.h
	virtual j_result_t DisableAlarm() = 0;

	///报警事件上传
	///param[in]	alarmData 报警数据
	///@return 		参见x_error_type.h
	//virtual j_result_t EventAlarm(const J_AlarmData &alarmData) = 0;
};

struct J_VoiceIcom : virtual public J_Obj
{
	///构造对讲通道对象
	///@param[in] 	pResid 全局唯一标识
	///@param[out] pObj 构造对象的返回值
	///@param[in]	pOwner channel对象所属的adapter
	///@param[in]	nChannel 通道号
	///@return 		参见x_error_type.h
	virtual int MakeVoice(const char *pResid, J_Obj *&pObj, J_Obj *pOwner, j_int32_t nChannel) = 0;
};

struct J_ChannelStream : virtual public J_Obj
{
	///打开视频流
	///@param[out] 	pObj 构造对象的返回值
	///@param[in]	pRingBuffer 环形流队列对象,由StreamManager管理
	///@return 		参见x_error_type.h
	virtual j_result_t OpenStream(J_Obj *&pObj, CRingBuffer *pRingBuffer) = 0;

	///关闭视频流
	///@param[in] 	pObj 将要关闭的流对象
	///@param[in] 	pRingBuffer 环形流队列对象,由StreamManager管理
	///@return 		参见x_error_type.h
	virtual j_result_t CloseStream(J_Obj *pObj, CRingBuffer *pRingBuffer) = 0;

	///是否支持码流类型
	///@return 		true-支持,false-不支持
	virtual j_boolean_t HasMultiStream() = 0;
};

struct J_PtzControl : virtual public J_Obj
{
	///云台控制
	///@param[in]	nCmd 云台控制命令, 见NvrType.h
	///@param[in]	nParam 云台控制参数,
	///				nParam > 0,如果云台控制命令为PTZ_PRE_SET or PTZ_PRE_CLR or PTZ_GOTO_PRE,参数为预置点位置;其他命令,参数为速度,取值范围1-255;
	///				nParam = 0,表示停止命令
	///@return		见x_error_type.h
	virtual j_result_t PtzControl(j_int32_t nCmd, j_int32_t nParam) = 0;
};

struct J_RemoteVod : virtual public J_Obj
{
	///枚举录像文件列表
	///@param[in]	beginTime 查询的起始时间
	///@param[in]	endTime	查询的结束时间
	///@param[out]	fileList 查询结果
	///@return 		参见x_error_type.h
	virtual j_result_t EmunFileByTime(time_t beginTime, time_t endTime, std::vector<J_FileInfo> &fileList) = 0;

	///打开DVR录像点播流
	///@param[out] 	pObj 构造对象的返回值
	///@return 		参见x_error_type.h
	virtual j_result_t OpenVodStream(J_Obj *&pObj) = 0;

	///关闭DVR录像点播流
	///@param[in] 	pObj 将要关闭的解析器对象
	///@return 		参见x_error_type.h
	virtual j_result_t CloseVodStream(J_Obj *pObj) = 0;
};

struct J_StreamParser : virtual public J_Obj
{
	///打开视频流分析器
	///@param[out] 	pObj 构造对象的返回值
	///@return 		参见x_error_type.h
	virtual j_result_t OpenParser(J_Obj *&pObj) = 0;

	///关闭视频流分析器
	///@param[in] 	pObj 将要关闭的解析器对象
	///@return 		参见x_error_type.h
	virtual j_result_t CloseParser(J_Obj *pObj) = 0;
};

struct J_MediaStream : virtual public J_Obj
{
	///启动流传输
	///@return 		参见x_error_type.h
	virtual j_result_t Startup() = 0;

	///停止流传输
	///@return 		参见x_error_type.h
	virtual j_result_t Shutdown() = 0;

	///添加目标流
	///@param[in] 	pRingBuffer 环形流队列对象,由J_VideoStream管理
	///@return 		参见x_error_type.h
	virtual j_result_t AddRingBuffer(CRingBuffer *pRingBuffer) = 0;

	///删除目标流
	///@param[in] 	pRingBuffer 环形流队列对象,由J_VideoStream管理
	///@return 		参见x_error_type.h
	virtual j_result_t DelRingBuffer(CRingBuffer *pRingBuffer) = 0;

	///流对象中流队列的数目
	///@return 		流队列个数
	virtual j_result_t RingBufferCount() = 0;
};

/*struct J_MediaRecord : virtual public J_Obj
{
	///启动视频录像
	///@param[in] 	pResid 全局唯一标识
	///@param[in] 	nDuration 录像持续时间,单位毫秒
	///@return 		参见x_error_type.h
	virtual int StartRecord(const char *pResid, int nDuration) = 0;

	///停止视频录像
	///@param[in] 	pResid 全局唯一标识
	///@return 		参见x_error_type.h
	virtual int StopRecord(const char *pResid) = 0;
};*/

struct J_MediaParser : virtual public J_Obj
{
	///初始化解析器
	///@param		nDataType 数据类型
	///@return 		参见x_error_type.h
	virtual j_result_t Init(int nDataType) = 0;

	///逆初始化解析器
	virtual j_result_t Deinit() = 0;

	///输入音视频数据
	///@param[in] 	pData 音视频数据
	///@param[in] 	nLen 数据长度
	///@return 		参见x_error_type.h
	virtual j_result_t InputData(const j_char_t *pData, j_int32_t nLen) = 0;

	///获取一包数据
	///@param[out] 	pData 音视频数据
	///@param[out]		streamHeader 流数据头信息
	///@return 			参见x_error_type.h
	virtual j_result_t GetOnePacket(j_char_t *pData, J_StreamHeader &streamHeader) = 0;
};

struct J_MediaObj : virtual public J_Obj
{
	///处理事务
	///@param[in]	asioData 异步IO数据
	///@param[in]	nCmdType 命令类型
	///@return 		参见x_error_type.h
	virtual j_result_t Process(J_AsioDataBase &asioData) = 0;

	///清除Client资源
	///@return 		参见x_error_type.h
	virtual j_result_t Clearn() = 0;

	///运行
	///@return 		参见x_error_type.h
	virtual j_result_t Run(j_boolean_t bFlag = true) = 0;
};

struct J_RealMediaObj : virtual public J_Obj
{
	///获取Client的资源ID
	///@return 		资源ID,错误返回NULL
	virtual const j_char_t *GetResid() const = 0;
};

struct J_RemoteVodStream : virtual public J_Obj
{
	///开始播放
	virtual j_result_t Play(time_t beginTime, time_t endTime, CRingBuffer *&pRingBuffer) = 0;

	///停止播放
	virtual j_result_t Stop() = 0;

	///暂停播放
	virtual j_result_t Pause() = 0;

	///设置播放速度
	virtual j_result_t SetSpeed(j_int32_t nSpeed, j_boolean_t bFlag = true) = 0;

	///设置播放位置
	virtual j_result_t SetPos() = 0;
};

struct J_FileReader : virtual public J_Obj
{
	///获得媒体上下文信息
	///@param[out]	MediaContext 上下文描述信息,需要调用者释放资源
	///@return 		参见x_error_type.h
	virtual j_result_t GetContext(J_MediaContext *&mediaContext) = 0;

	///获得媒体数据包
	///@return 		参见x_error_type.h
	virtual j_result_t GetPacket(j_char_t *pBuffer, J_StreamHeader &streamHeader) = 0;

	///暂停
	///@return 		参见x_error_type.h
	virtual j_result_t Pause() = 0;

	///设置速度
	///@param[in]	nScale速度比例
	///@return 		参见x_error_type.h
	virtual j_result_t SetScale(float nScale = 1) = 0;

	///设置时间
	///@param[in]	s_time绝对开始时间或者相对开始时间
	///@param[in]	e_time绝对结束时间或者相对结束时间
	///@return 		参见x_error_type.h
	virtual j_result_t SetTime(j_uint64_t s_time, j_uint64_t e_time) = 0;

	///设置位置
	///@param[in]	nPos相对开始位置的偏移
	///@return 		参见x_error_type.h
	virtual j_result_t SetPosition(j_int32_t nPos) = 0;
	
	///获取视频数据
	///@param[in]	beginTime开始时间
	///@param[in]	nIval时间间隔
	///@return 		参见x_error_type.h
	virtual j_result_t GetMediaData(j_uint64_t beginTime, j_int32_t nIval) = 0;
};

struct J_JoManager : virtual public J_Obj
{
	///获取设备信息
	///@param[in]	pResid 资源标示
	///@param[out]	devInfo 设备信息
	///@return 		参见x_error_type.h
	virtual j_result_t GetStreamServerInfo(const j_char_t *pResid, J_StreamServerInfo &ssInfo) = 0;
	
	///获取资源信息
	///@param[out]	resInfo 全部资源信息
	///@return 		参见x_error_type.h
	virtual j_result_t GetResourceInfo(ResourceMap &resInfo) = 0;

	///通过通道查询通道信息
	///@param[in]		pResid 资源ID
	///@param[out]	resInfo 通道信息
	///@return 		参见x_error_type.h
	virtual j_result_t GetChannelInfo(const char *pResid, J_ResourceInfo &resInfo) = 0;
	
	///启动录像
	///@return 		参见x_error_type.h
	virtual j_result_t StartRecord() = 0;
	
	///停止录像
	///@return 		参见x_error_type.h
	virtual j_result_t StopRecord() = 0;
};

struct J_CommandFilter : virtual public J_Obj
{
	///获取命令类型
	///@return 	命令类型,见j_nvrtype.h
	virtual j_result_t GetCommandType() const { return m_nCommandType; }

	///获取资源ID
	///@return 	资源ID
	virtual const j_char_t *GetResid() const { return m_strResid; }

	///获取码流类型
	///@return 	码流类型
	virtual j_result_t GetStreamType() const { return m_nStreamType; }

protected:
	j_int32_t m_nCommandType;
	j_int32_t m_nStreamType;
	j_char_t m_strResid[16];
};

struct J_VodCommandFilter : public J_CommandFilter
{
	///获得开始时间
	///@return		开始时间
	virtual j_uint64_t GetBeginTime() const { return m_beginTime; }

	///获得结束时间
	///@return		结束时间
	virtual j_uint64_t GetEndTime() const { return m_endTime; }

	///获得播放速度
	///@return 		播放速度
	virtual j_int32_t GetScale() const { return m_scale; }
	
	///获得播放模式
	///@return 		播放模式
	virtual j_int32_t GetMode() const { return m_mode; }

protected:
	j_uint64_t m_beginTime;
	j_uint64_t m_endTime;
	j_int32_t m_scale;
	j_int32_t m_mode;
};

struct J_RequestFilter : virtual public J_Obj
{
	///命令解析
	///@param[in]	nSocket	
	///@return		见x_error_type.h
	virtual j_result_t Parser(J_AsioDataBase &asioData) = 0;
	
	///获取资源类型
	///@return	成功-资源类型,失败-NULL
	virtual const j_char_t *GetResourceType() = 0;

	///数据转换
	///@param[in]	pInputData 		输入数据
	///@param[in]	streamHeader 	输入数据长度
	///@param[out] 	pOutputData		输出数据
	///@param[out]	nOutputLen		输出数据长度
	///@return		见x_error_type.h
	virtual j_result_t Convert(const j_char_t *pInputData, J_StreamHeader &streamHeader, j_char_t *pOutputData, j_int32_t &nOutLen) = 0;

	///完成会话
	///@param[in]	nSocket	
	///@return		见x_error_type.h
	virtual j_result_t Complete(J_AsioDataBase &asioData) = 0;
};

struct J_MuxFilter : virtual public J_Obj
{
	///初始化
	///@return	见x_error_type.h
	virtual j_result_t Init() = 0;

	///逆初始化
	virtual j_void_t Deinit() = 0;

	///数据转换
	///@param[in]	pInputData 		输入数据
	///@param[in]	streamHeader 	输入数据长度
	///@param[out] 	pOutputData		输出数据
	///@param[out]	nOutputLen		输出数据长度
	///@param[in]	pExtdata			扩展数据
	///@return		见x_error_type.h
	virtual j_result_t Convert(const j_char_t *pInputData, J_StreamHeader &streamHeader, j_char_t *pOutputData, j_int32_t &nOutLen, j_void_t *pExtdata = 0) = 0;
};

struct J_AsioUser : virtual public J_Obj
{
	///连接事件完成(用于网络IO)
	///@param[in]	asioData IO数据集
	///@param[in]	错误码,见x_error_type.h	
	virtual j_result_t OnAccept(const J_AsioDataBase *pAsioData, j_result_t nRet) = 0;

	///读事件完成
	///@param[in]	asioData IO数据集
	///@param[in]	错误码,见x_error_type.h
	virtual j_result_t OnRead(const J_AsioDataBase *pAsioData, j_result_t nRet) = 0;

	///写事件完成
	///@param[in]	asioData IO数据集
	///@param[in]	错误码,见x_error_type.h
	virtual j_result_t OnWrite(const J_AsioDataBase *pAsioData, j_result_t nRet) = 0;

	///断线事件完成(用于网络IO)
	///@param[in]	错误码,见x_error_type.h
	virtual j_result_t OnBroken(const J_AsioDataBase *pAsioData, j_result_t nRet) = 0;
};

struct J_CommandParser : virtual public J_Obj
{
	///添加用户
	///@param[in]	nSocket 用户ID
	///@param[in]	pAddr 用户Address
	///@param[in]	nPort 用户Port
	///@return		见x_error_type.h
	virtual j_result_t AddUser(j_socket_t nSocket, const j_char_t *pAddr, j_int16_t nPort) = 0;
	
	///解析命令
	///@param[in]	nSocket 用户ID
	///@param[in]	pResponse 返回数据
	///@param[out]	nRespLen 返回数据长度
	///@return		见x_error_type.h
	virtual j_result_t ProcessRequest(J_AsioDataBase *pAsioData_in, J_AsioDataBase *pAsioData_out) = 0;
	
	///删除用户
	///@param[in]	nSocket 用户ID
	///@return		见x_error_type.h
	virtual j_result_t DelUser(j_socket_t nSocket) = 0;
};

struct J_EventParser : virtual public J_Obj
{
	///事件解析
	///@param[in]	pEventData 事件数据
	///@return		见x_error_type.h
	virtual j_result_t AnalyzePacket(const j_uint8_t *pEventData) = 0;
};

struct J_PlayerObj : virtual public J_Obj
{
	///播放视频
	///@param	 hWnd 播放窗口
	///@return	 参见x_errtype.j
	virtual j_result_t PlayMedia(j_wnd_t hWnd, j_int32_t nDevid) = 0;
	///停止播放
	///@return	 参见x_errtype.j
	virtual j_result_t StopMedia(j_int32_t nDevid) = 0;
	///处理媒体数据
	///@return	 参见x_errtype.j
	virtual j_result_t ProcessMedia() = 0;
};

struct J_Player : virtual public J_Obj
{
	///播放视频
	///@param	 hWnd 播放窗口
	///@return	 参见x_errtype.j
	virtual j_result_t Play(j_wnd_t hWnd) = 0;
	///停止播放
	///@return	 参见x_errtype.j
	virtual j_result_t Stop() = 0;
	///填充数据
	///@param	 pData 数据缓存区
	///@param	 nLen 数据长度
	///@return	 参见x_errtype.j
	virtual j_result_t InputData(j_char_t *pData, J_StreamHeader &streamHeader) = 0;
};

struct J_Decoder : virtual public J_Obj
{
	///初始化解码器
	///@return	 参见x_errtype.j
	virtual j_result_t InidDecoder() = 0;
	///刷新解码缓冲
	///@return	 参见x_errtype.j
	virtual j_result_t FlushBuffer() = 0;
	///获取解码参数
	///@param	 decParam 解码参数
	///@return	 参见x_errtype.j
	virtual j_result_t GetDecodeParam(J_VideoDecodeParam &decParam) = 0;
	///解码一帧数据
	///@param	 pInputData 解码前数据
	///@param	 nInputLen 解码前数据长度
	///@param	 pOutuptData 解码后数据
	///@param	 nOutputLen 解码后数据长度
	///@return	 参见x_errtype.j
	virtual j_result_t DecodeOneFrame(j_char_t *pInputData, j_int32_t nInputLen, j_char_t *pOutuptData, j_int32_t &nOutputLen) = 0;
	///释放解码器
	///@return	 参见x_errtype.j
	virtual j_result_t DeinitDecoder() = 0;

protected:
	j_int32_t m_codeId;			//标志这个类的解码类型
	J_VideoDecodeParam m_decParam;
};

struct J_Render : virtual public J_Obj
{
	///初始化Render
	///@param	 hwnd 窗口句柄
	///@return	 参见x_errtype.j
	virtual j_result_t InitRender(j_wnd_t hwnd) = 0;
	///显示图像
	///@param	 pData YUV数据
	///@param	 nLen 数据长度
	virtual j_result_t DisplayFrame(j_char_t *pData, j_int32_t nLen) = 0;
	///释放Render
	///@return	 参见x_errtype.j
	virtual j_result_t DeinitRender() = 0;
};

template <typename CBase>
class J_BaseVideoStream : public CBase
{
protected:
	J_BaseVideoStream() {}
	~J_BaseVideoStream() {}

public:
	int AddRingBuffer(CRingBuffer *pRingBuffer)
	{
		TLock(m_vecLocker);
		m_vecRingBuffer.push_back(pRingBuffer);
		TUnlock(m_vecLocker);

		return J_OK;
	}

	int DelRingBuffer(CRingBuffer *pRingBuffer)
	{
		TLock(m_vecLocker);
		std::vector<CRingBuffer *>::iterator it = m_vecRingBuffer.begin();
		for (; it != m_vecRingBuffer.end(); it++)
		{
			if (*it == pRingBuffer)
			{
				m_vecRingBuffer.erase(it);
				TUnlock(m_vecLocker);

				return J_OK;
			}
		}
		TUnlock(m_vecLocker);
		return J_NOT_EXIST;
	}

	int RingBufferCount()
	{
		TLock(m_vecLocker);
		int count = m_vecRingBuffer.size();
		TUnlock(m_vecLocker);

		return  count;
	}

public:
	J_OS::TLocker_t m_vecLocker;
	std::vector<CRingBuffer *> m_vecRingBuffer;
};

///全局标识
template <template <typename CBase> class CBaseAdapter, typename CChannel = J_ChannelStream>
class J_ResidTmpl : public CBaseAdapter<CChannel>
{
public:
	j_string_t m_resid;
};

#endif //~__J_VIDEOADAPTER_H_
