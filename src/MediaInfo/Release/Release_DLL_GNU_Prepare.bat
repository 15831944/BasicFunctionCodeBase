@rem MediaInfo
@rem Copyright (C) 2002-2012 MediaArea.net SARL, Info@MediaArea.net
@rem
@rem This program is free software: you can redistribute it and/or modify it
@rem under the terms of the GNU Lesser General Public License as published by
@rem the Free Software Foundation, either version 3 of the License, or
@rem (at your option) any later version.
@rem
@rem This program is distributed in the hope that it will be useful,
@rem but WITHOUT ANY WARRANTY; without even the implied warranty of
@rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
@rem GNU Lesser General Public License for more details.
@rem
@rem You should have received a copy of the GNU Lesser General Public License
@rem along with this program. If not, see <http:@remwww.gnu.org/licenses/>.
@rem
@rem ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
@rem ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

@echo off

rem --- Clean up ---
del MediaInfo_DLL_GNU_Prepare.7z
rmdir MediaInfo_DLL_GNU_Prepare /S /Q

rem --- Copying : From lib ---
call Release_Lib_GNU_Prepare SkipCleanUp SkipCompression
rename MediaInfo_Lib_GNU_Prepare MediaInfo_DLL_GNU_Prepare

rem --- Copying : Project ---
xcopy ..\Project\GNU\Library\AddThisToRoot_DLL_compile.sh MediaInfo_DLL_GNU_Prepare\Project\GNU\Library\

rem --- Copying : Release ---
xcopy Release_DLL_GNU.sub MediaInfo_DLL_GNU_Prepare\Release\
xcopy Release_DLL_*.sh MediaInfo_DLL_GNU_Prepare\Release\

rem --- Copying : Information files ---
copy ..\History.txt MediaInfo_DLL_GNU_Prepare\
copy ..\License.html MediaInfo_DLL_GNU_Prepare\
copy ReadMe_GUI_Linux.txt MediaInfo_DLL_GNU_Prepare\Release\
copy ReadMe_GUI_Mac.txt MediaInfo_DLL_GNU_Prepare\Release\

rem --- Copying : Sources ---
xcopy ..\Source\Doc\Doxyfile MediaInfo_DLL_GNU_Prepare\Source\Doc\ /S
xcopy ..\Source\Doc\Documentation.html MediaInfo_DLL_GNU_Prepare\Source\Doc\ /S
xcopy ..\Source\Example\HowToUse* MediaInfo_DLL_GNU_Prepare\Source\Example\ /S
xcopy ..\Source\MediaInfoDLL\MediaInfoDLL.cs MediaInfo_DLL_GNU_Prepare\Source\MediaInfoDLL\ /S
xcopy ..\Source\MediaInfoDLL\MediaInfoDLL.JNA.java MediaInfo_DLL_GNU_Prepare\Source\MediaInfoDLL\ /S
xcopy ..\Source\MediaInfoDLL\MediaInfoDLL.JNative.java MediaInfo_DLL_GNU_Prepare\Source\MediaInfoDLL\ /S
xcopy ..\Source\MediaInfoDLL\MediaInfoDLL.py MediaInfo_DLL_GNU_Prepare\Source\MediaInfoDLL\ /S
xcopy ..\Source\MediaInfoDLL\MediaInfoDLL3.py MediaInfo_DLL_GNU_Prepare\Source\MediaInfoDLL\ /S


rem --- Preparing Archive : MediaInfoLib ---
move MediaInfo_DLL_GNU_Prepare MediaInfoLib
mkdir MediaInfo_DLL_GNU_Prepare
move MediaInfoLib MediaInfo_DLL_GNU_Prepare

rem --- Preparing Archive : ZenLib ---
cd ..\..\ZenLib\Release
call Release_GNU_Prepare.bat SkipCleanUp SkipCompression
cd ..\..\MediaInfoLib\Release
move ..\..\ZenLib\Release\ZenLib_GNU_Prepare MediaInfo_DLL_GNU_Prepare\ZenLib

rem --- Shared ---
mkdir MediaInfo_DLL_GNU_Prepare\Shared
xcopy ..\..\Shared\Project\_Common\* MediaInfo_DLL_GNU_Prepare\Shared\Project\_Common\ /S
xcopy ..\..\Shared\Project\curl\*.sh MediaInfo_DLL_GNU_Prepare\Shared\Project\curl\ /S
xcopy ..\..\Shared\Project\zlib\*.sh MediaInfo_DLL_GNU_Prepare\Shared\Project\zlib\ /S


rem --- Compressing Archive ---
if "%2"=="SkipCompression" goto SkipCompression
..\..\Shared\Binary\Windows_i386\7-Zip\7z a -r -t7z -mx9 MediaInfo_DLL_GNU_Prepare.7z MediaInfo_DLL_GNU_Prepare\*
:SkipCompression

rem --- Clean up ---
if "%1"=="SkipCleanUp" goto SkipCleanUp
rmdir MediaInfo_DLL_GNU_Prepare /S /Q
:SkipCleanUp
