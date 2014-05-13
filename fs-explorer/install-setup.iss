; -- install-setup.iss --

[Setup]
AppName=FS Explorer
AppVerName=FS Explorer
AppVersion=14.04
DefaultDirName={pf}\fs-explorer
DefaultGroupName=FS Explorer
Compression=lzma2
SolidCompression=yes
OutputDir=install32\win

[Dirs]
Name: "{app}\bin"
Name: "{app}\bin\platforms"

[Files]
Source: ".\release\bin\*"; DestDir: "{app}\bin"
Source: ".\release\lib\*"; DestDir: "{app}\bin"
Source: ".\release\lib\platforms\*"; DestDir: "{app}\bin\platforms"
Source: ".\release\ReleaseNote.txt"; DestDir: "{app}"
Source: ".\release\icon.ico"; DestDir: "{app}"

[Icons]
Name: "{group}\FS Explorer"; Filename: "{app}\bin\fs-explorer.exe"; IconFilename: "{app}\icon.ico"
Name: "{group}\Uninstall FS Explorer"; Filename: "{uninstallexe}"
Name: "{group}\Release Note"; Filename: "{app}\ReleaseNote.txt"

[UninstallDelete]
Type: files; Name: "{app}\bin\settings.ini"
