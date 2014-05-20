; 脚本由 Inno Setup 脚本向导 生成！
; 有关创建 Inno Setup 脚本文件的详细资料请查阅帮助文档！

[Setup]
; 注: AppId的值为单独标识该应用程序。
; 不要为其他安装程序使用相同的AppId值。
; (生成新的GUID，点击 工具|在IDE中生成GUID。)
AppId={{BF979905-190B-4DEC-A179-9D5A0D044ACB}
AppName=我的程序
AppVersion=1.5
;AppVerName=我的程序 1.5
AppPublisher=我的公司
AppPublisherURL=http://www.example.com/
AppSupportURL=http://www.example.com/
AppUpdatesURL=http://www.example.com/
DefaultDirName={pf}\我的程序
DefaultGroupName=我的程序
OutputBaseFilename=setup
Compression=lzma
SolidCompression=yes

[Languages]
Name: "chinesesimp"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "E:\workspace\jorhy-xl\bat\trans_server_run.bat"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\workspace\jorhy-xl\bat\trans_server.bat"; DestDir: "{app}"; Flags: ignoreversion
; 注意: 不要在任何共享系统文件上使用“Flags: ignoreversion”

[Icons]
Name: "{group}\我的程序"; Filename: "{app}\trans_server_run.bat"
Name: "{commondesktop}\我的程序"; Filename: "{app}\trans_server_run.bat"; Tasks: desktopicon

[Run]
Filename: "{app}\trans_server_run.bat"; Description: "{cm:LaunchProgram,我的程序}"; Flags: shellexec postinstall skipifsilent

[Registry]
;添加开机启动Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName:"bwqc"; ValueData:"{app}\trans_server_run.bat";  Flags:uninsdeletevalue

