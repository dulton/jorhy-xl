@echo off 
:start
	call E:\workspace\jorhy-xl\bat\trans_server.bat
	echo bbbb
:loop  
	tasklist | find "SyncDataService.exe" || goto start
	echo aaaaa
	timeout /t 3 /nobreak > nul 
	goto loop


rem cd "E:\workspace\jorhy-xl\vs2008\SyncDataService\Debug\" SyncDataService.exe