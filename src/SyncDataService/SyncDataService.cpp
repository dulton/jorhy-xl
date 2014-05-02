#if 1
#include "BroadCastListener.h"
#include "AccessOprator.h"
#include "DataDownloader.h"

//#pragma comment(lib, "Debug\\core.lib")

int main(int argc, char **argv)
{
	CBroadCastListener listener;
	CDataDownloader downLoader;

	///��ʼ�����ݿ�
	if (JoAccessObj->Connect("SyncFileInfo.accdb", NULL, NULL, NULL) != J_OK)
	{
		J_OS::LOGINFO("���ݿ�����ʧ��, ��ַ:%s", "SyncFileInfo.accdb");
		return -1;
	}
	JoAccessObj->Init();

	///�������ط������
	downLoader.Start("E:");

	///�����ͻ���������
	if (listener.StartService(8504) != J_OK)
	{
		J_OS::LOGINFO("�����ͻ���������ʧ��, �˿�:%d", 5050);
		return -1;
	}

	while (true)
	{
		Sleep(100);
	}

	///ֹͣ�ͻ���������
	listener.StopService();
	///ֹͣ���ط���
	downLoader.Stop();
	///ע�����ݿ�����
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