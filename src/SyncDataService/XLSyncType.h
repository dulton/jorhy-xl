#ifndef __XLSYNCTYPE_H_
#define __XLSYNCTYPE_H_
#include <map>
#include <string>

typedef struct 
{
	char host_id[32];
	char ftp_addr[16];
	int ftp_port;
	char user_name[32];
	char pass_word[32];
} BroadCastFtpInfo;

typedef struct 
{
	char strHostId[32];
	char strFtpAddr[16];
	int nFtpPort;
	char strUsesrName[32];
	char strPassWord[32];
	time_t updateTime;
} DownLoadFtpInfo;

typedef std::map<std::string, DownLoadFtpInfo> HostMap;

#endif //~__XLSYNCTYPE_H_