; in the name of ALLAH
; YA MAHDI

#define MyAppName "Ketab-e-Man Server"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Your Name"
#define MyAppExeName "Ketab_e_Man_Server.exe"

; مسیر پوشه‌ی خروجی بیلد را با -D در خط فرمان ست کنید (پایین توضیح داده شده)
#ifndef BuildDir
  #define BuildDir "cmake-build-release\server"
#endif

[Setup]
AppId={{B6F1E9C2-4D3A-4B1E-9F2A-1234567890AB}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf}\Ketab_e_Man
DefaultGroupName={#MyAppName}
OutputDir=installer_output
OutputBaseFilename=Ketab_e_Man_Setup_{#MyAppVersion}
Compression=lzma2
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64compatible
; چون سرور می‌خواهد کنار خودش (پوشه‌ی نصب) فایل بنویسد
PrivilegesRequired=admin

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
; همه چیز داخل پوشه‌ی خروجی بیلد (exe, dll ها, پلاگین‌ها) به‌جز assets/data
; چون assets/data ممکن است حاوی دیتای قبلی از اجرای بیلد باشد و نباید overwrite شود
Source: "{#BuildDir}\*"; DestDir: "{app}"; Excludes: "assets\data\*"; Flags: recursesubdirs createallsubdirs ignoreversion

; پوشه‌ی assets\data فقط اگر در مقصد وجود نداشت ساخته/کپی شود (اولین نصب)
Source: "{#BuildDir}\assets\data\*"; DestDir: "{app}\assets\data"; Flags: recursesubdirs createallsubdirs ignoreversion onlyifdoesntexist

[Dirs]
; اطمینان از وجود پوشه‌ی دیتا حتی اگر خالی باشد، با دسترسی نوشتن برای همه‌ی کاربران
Name: "{app}\assets\data"; Permissions: users-modify

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Launch {#MyAppName}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
; در آنینستال، فقط باینری‌ها حذف شوند؛ دیتای کاربر (assets/data) دست‌نخورده بماند
; اگر می‌خواهید دیتا هم پاک شود، خط زیر را از کامنت خارج کنید:
; Type: filesandordirs; Name: "{app}\assets\data"