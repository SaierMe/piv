ECHO OFF
SET PATH=%~dp0

DEL /F /S /Q "%PATH%minhook\lib"
DEL /F /Q  "%PATH%src\ConvertUTF.h"
DEL /F /Q  "%PATH%src\ConvertUTF.c"
DEL /F /Q  "%PATH%src\piv_md5.cpp"

DEL /F /Q  "%PATH%p_Ncrypt.v"

DEL /F /Q  "%PATH%p_Base.v"
DEL /F /Q  "%PATH%p_Bass.v"
DEL /F /Q  "%PATH%p_BassPlugin.v"
DEL /F /Q  "%PATH%p_Bcrypt.v"
DEL /F /Q  "%PATH%p_Container.v"
DEL /F /Q  "%PATH%p_Encoding.v"
DEL /F /Q  "%PATH%p_Helper.v"
DEL /F /Q  "%PATH%p_Http.v"
DEL /F /Q  "%PATH%p_Json.v"
DEL /F /Q  "%PATH%p_Logging.v"
DEL /F /Q  "%PATH%p_Markdown.v"
DEL /F /Q  "%PATH%p_Misc.v"
DEL /F /Q  "%PATH%p_Network.v"
DEL /F /Q  "%PATH%p_PEfile.v"
DEL /F /Q  "%PATH%p_Process.v"
DEL /F /Q  "%PATH%p_Std_Base.v"
DEL /F /Q  "%PATH%p_String.v"
DEL /F /Q  "%PATH%p_System.v"
DEL /F /Q  "%PATH%p_Thread.v"
DEL /F /Q  "%PATH%p_Time.v"
DEL /F /Q  "%PATH%p_TOML.v"
DEL /F /Q  "%PATH%p_VMProtect.v"
DEL /F /Q  "%PATH%p_Windows.v"
DEL /F /Q  "%PATH%p_WinHTTP.v"

DEL /F /Q  "%PATH%p_MMKV.v"
DEL /F /Q  "%PATH%p_SevenZip.v"

ECHO 已删除旧版PIV模块的残留文件。
PAUSE
