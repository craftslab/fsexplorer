; -- install-setup-iss --

[Setup]
AppName=FS Explorer
AppVerName=FS Explorer
AppVersion=14.04
DefaultDirName={pf}\fs-explorer
DefaultGroupName=FS Explorer
UninstallDisplayIcon={app}\bin\fs-explorer.exe
Compression=lzma2
SolidCompression=yes
OutputDir=install-win

[Dirs]
Name: "{app}\bin"
Name: "{app}\lib"

[Files]
Source: ".\release\bin\*"; DestDir: "{app}\bin"
Source: ".\release\lib\*"; DestDir: "{app}\lib"
Source: ".\release\ReleaseNote.txt"; DestDir: "{app}\"

[Icons]
Name: "{group}\FS Explorer"; Filename: "{app}\bin\fs-explorer.exe"
