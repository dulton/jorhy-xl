; �ű��� Inno Setup �ű��� ���ɣ�
; �йش��� Inno Setup �ű��ļ�����ϸ��������İ����ĵ���

[Setup]
; ע: AppId��ֵΪ������ʶ��Ӧ�ó���
; ��ҪΪ������װ����ʹ����ͬ��AppIdֵ��
; (�����µ�GUID����� ����|��IDE������GUID��)
AppId={{BF979905-190B-4DEC-A179-9D5A0D044ACB}
AppName=�ҵĳ���
AppVersion=1.5
;AppVerName=�ҵĳ��� 1.5
AppPublisher=�ҵĹ�˾
AppPublisherURL=http://www.example.com/
AppSupportURL=http://www.example.com/
AppUpdatesURL=http://www.example.com/
DefaultDirName={pf}\�ҵĳ���
DefaultGroupName=�ҵĳ���
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
; ע��: ��Ҫ���κι���ϵͳ�ļ���ʹ�á�Flags: ignoreversion��

[Icons]
Name: "{group}\�ҵĳ���"; Filename: "{app}\trans_server_run.bat"
Name: "{commondesktop}\�ҵĳ���"; Filename: "{app}\trans_server_run.bat"; Tasks: desktopicon

[Run]
Filename: "{app}\trans_server_run.bat"; Description: "{cm:LaunchProgram,�ҵĳ���}"; Flags: shellexec postinstall skipifsilent

[Registry]
;��ӿ�������Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName:"bwqc"; ValueData:"{app}\trans_server_run.bat";  Flags:uninsdeletevalue

