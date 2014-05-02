#if 1
#include "BroadCastListener.h"
#include "AccessOprator.h"
#include "DataDownloader.h"

//#pragma comment(lib, "Debug\\core.lib")

int main(int argc, char **argv)
{
	CBroadCastListener listener;
	CDataDownloader downLoader;

	///初始化数据库
	if (JoAccessObj->Connect("SyncFileInfo.accdb", NULL, NULL, NULL) != J_OK)
	{
		J_OS::LOGINFO("数据库连接失败, 地址:%s", "SyncFileInfo.accdb");
		return -1;
	}
	JoAccessObj->Init();

	///启动下载服务程序
	downLoader.Start("E:");

	///启动客户监听服务
	if (listener.StartService(8504) != J_OK)
	{
		J_OS::LOGINFO("启动客户监听服务失败, 端口:%d", 5050);
		return -1;
	}

	while (true)
	{
		Sleep(100);
	}

	///停止客户监听服务
	listener.StopService();
	///停止下载服务
	downLoader.Stop();
	///注销数据库连接
	JoAccessObj->Release();

	return 0;
}

#else

////ftp test
#include "FtpHelper.h"

void OnMassage(void *, bool bFinished)
{
	printf("%s\n", bFinished ? "Finish" : "Error");
}

int main(int argc, char **argv)
{
	CFtpHelper helper;
	printf ("longin return %s\n", helper.Login("127.0.0.1", 8504, "joo", "1") == 0 ? "success" : "failuer");
	VecFileName vecFileName;
	helper.ListFile("", vecFileName);
	VecFileName::iterator it = vecFileName.begin();
	for (; it != vecFileName.end(); ++it)
	{
		printf("%s\n", it->c_str());
	}
	helper.DownLoadFile("FirefoxSetup3.6.12_cn.zip", "aaa.zip", OnMassage, NULL);

	return 0;
}

#endif