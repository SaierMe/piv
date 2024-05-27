ECHO OFF
SET PATH=%~dp0

IF EXIST "%PATH%minhook\lib" DEL /F /S /Q "%PATH%minhook\lib"
IF EXIST "%PATH%minhook\lib" RD /S /Q "%PATH%minhook\lib"
IF EXIST "%PATH%src\ConvertUTF.h" DEL /F /Q  "%PATH%src\ConvertUTF.h"
IF EXIST "%PATH%src\ConvertUTF.c" DEL /F /Q  "%PATH%src\ConvertUTF.c"
IF EXIST "%PATH%src\piv_md5.cpp" DEL /F /Q  "%PATH%src\piv_md5.cpp"

IF EXIST "%PATH%p_Ncrypt.v" DEL /F /Q  "%PATH%p_Ncrypt.v"

IF EXIST "%PATH%p_Base.v" DEL /F /Q  "%PATH%p_Base.v"
IF EXIST "%PATH%p_Bass.v" DEL /F /Q  "%PATH%p_Bass.v"
IF EXIST "%PATH%p_BassPlugin.v" DEL /F /Q  "%PATH%p_BassPlugin.v"
IF EXIST "%PATH%p_Bcrypt.v" DEL /F /Q  "%PATH%p_Bcrypt.v"
IF EXIST "%PATH%p_Container.v" DEL /F /Q  "%PATH%p_Container.v"
IF EXIST "%PATH%p_Encoding.v" DEL /F /Q  "%PATH%p_Encoding.v"
IF EXIST "%PATH%p_Helper.v" DEL /F /Q  "%PATH%p_Helper.v"
IF EXIST "%PATH%p_Http.v" DEL /F /Q  "%PATH%p_Http.v"
IF EXIST "%PATH%p_Json.v" DEL /F /Q  "%PATH%p_Json.v"
IF EXIST "%PATH%p_Logging.v" DEL /F /Q  "%PATH%p_Logging.v"
IF EXIST "%PATH%p_Markdown.v" DEL /F /Q  "%PATH%p_Markdown.v"
IF EXIST "%PATH%p_Misc.v" DEL /F /Q  "%PATH%p_Misc.v"
IF EXIST "%PATH%p_Network.v" DEL /F /Q  "%PATH%p_Network.v"
IF EXIST "%PATH%p_PEfile.v" DEL /F /Q  "%PATH%p_PEfile.v"
IF EXIST "%PATH%p_Process.v" DEL /F /Q  "%PATH%p_Process.v"
IF EXIST "%PATH%p_Std_Base.v" DEL /F /Q  "%PATH%p_Std_Base.v"
IF EXIST "%PATH%p_String.v" DEL /F /Q  "%PATH%p_String.v"
IF EXIST "%PATH%p_System.v" DEL /F /Q  "%PATH%p_System.v"
IF EXIST "%PATH%p_Thread.v" DEL /F /Q  "%PATH%p_Thread.v"
IF EXIST "%PATH%p_Time.v" DEL /F /Q  "%PATH%p_Time.v"
IF EXIST "%PATH%p_TOML.v" DEL /F /Q  "%PATH%p_TOML.v"
IF EXIST "%PATH%p_VMProtect.v" DEL /F /Q  "%PATH%p_VMProtect.v"
IF EXIST "%PATH%p_Windows.v" DEL /F /Q  "%PATH%p_Windows.v"
IF EXIST "%PATH%p_WinHTTP.v" DEL /F /Q  "%PATH%p_WinHTTP.v"

IF EXIST "%PATH%p_MMKV.v" DEL /F /Q  "%PATH%p_MMKV.v"
IF EXIST "%PATH%p_SevenZip.v" DEL /F /Q  "%PATH%p_SevenZip.v"

IF EXIST "%PATH%src\piv_md5.h" DEL /F /Q  "%PATH%src\piv_md5.h"
IF EXIST "%PATH%src\piv_base.hpp" DEL /F /Q  "%PATH%src\piv_base.hpp"
IF EXIST "%PATH%src\string_view.hpp" DEL /F /Q  "%PATH%src\string_view.hpp"
IF EXIST "%PATH%src\piv_tomlplusplus.hpp" DEL /F /Q  "%PATH%src\piv_tomlplusplus.hpp"

IF EXIST "%PATH%piv_lib.vgrp" DEL /F /Q  "%PATH%piv_lib.vgrp"

IF EXIST "%PATH%MMKV\debug" DEL /F /S /Q "%PATH%MMKV\debug"
IF EXIST "%PATH%MMKV\release" DEL /F /S /Q "%PATH%MMKV\release"
IF EXIST "%PATH%MMKV\debug" RD /S /Q "%PATH%MMKV\debug"
IF EXIST "%PATH%MMKV\release" RD /S /Q "%PATH%MMKV\release"
IF EXIST "%PATH%MMKV\piv_MMKV.hpp" DEL /F /Q "%PATH%MMKV\piv_MMKV.hpp"
IF EXIST "%PATH%MMKV\include\PBUtility.h" DEL /F /Q "%PATH%MMKV\include\PBUtility.h"

IF EXIST "%PATH%bit7z\lib\win32\15" DEL /F /S /Q "%PATH%bit7z\lib\win32\15"
IF EXIST "%PATH%bit7z\lib\win32\16" DEL /F /S /Q "%PATH%bit7z\lib\win32\16"
IF EXIST "%PATH%bit7z\lib\win32\17" DEL /F /S /Q "%PATH%bit7z\lib\win32\17"
IF EXIST "%PATH%bit7z\lib\x64\15" DEL /F /S /Q "%PATH%bit7z\lib\x64\15"
IF EXIST "%PATH%bit7z\lib\x64\16" DEL /F /S /Q "%PATH%bit7z\lib\x64\16"
IF EXIST "%PATH%bit7z\lib\x64\17" DEL /F /S /Q "%PATH%bit7z\lib\x64\17"

IF EXIST "%PATH%bit7z\lib\win32\15" RD /S /Q "%PATH%bit7z\lib\win32\15"
IF EXIST "%PATH%bit7z\lib\win32\16" RD /S /Q "%PATH%bit7z\lib\win32\16"
IF EXIST "%PATH%bit7z\lib\win32\17" RD /S /Q "%PATH%bit7z\lib\win32\17"
IF EXIST "%PATH%bit7z\lib\x64\15" RD /S /Q "%PATH%bit7z\lib\x64\15"
IF EXIST "%PATH%bit7z\lib\x64\16" RD /S /Q "%PATH%bit7z\lib\x64\16"
IF EXIST "%PATH%bit7z\lib\x64\17" RD /S /Q "%PATH%bit7z\lib\x64\17"

IF EXIST "%PATH%minhook\src" RD /S /Q "%PATH%minhook\src"

ECHO 已删除 2024-05-27 版之前的PIV模块残留文件.
PAUSE
