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

@rem echo off
@rem --- General ---
cd ..
del *.~* *.obj *.o *.tds *.dcu *.ddp *.opt *.ncb *.suo *.ilk *.idb *.pdb *.pch *.plg *.aps *.user *.win *.layout *.local *.depend *.identcache *.tgs *.tgw *.sdf /Q /S
del *.~* *.obj *.o *.tds *.dcu *.ddp *.opt *.ncb *.suo *.ilk *.idb *.pdb *.pch *.plg *.aps *.user *.win *.layout *.local *.depend  *.identcache *.tgs *.tgw *.sdf /AH /Q /S
del *.ogg *.ogm *.avi *.mkv /Q /S
del *.ogg *.ogm *.avi *.mkv /AH /Q /S
cd Release

@rem --- In Project files ---
cd ..\Project
del *.exe *.dll *.a *.lib *.exp *.class *.zip *.7z /Q /S
del *.exe *.dll *.a *.lib *.exp *.class *.zip *.7z /AH /Q /S
cd ..\Release

@rem MS Visual Studio specific ---
cd ..\Project
rmdir MSCS\Example\bin /Q /S
rmdir MSCS\Example\obj /Q /S
rmdir MSJS\Example\bin /Q /S
rmdir MSJS\Example\obj /Q /S
rmdir MSVB\Example\bin /Q /S
rmdir MSVB\Example\obj /Q /S
rmdir MSVC\Dll\Release /Q /S
rmdir MSVC\Dll\Release_Ansi /Q /S
rmdir MSVC\Example\Release /Q /S
rmdir MSVC\Example\Release_Ansi /Q /S
rmdir MSVC\Library\Release /Q /S
rmdir MSVC\Library\Release_Ansi /Q /S
cd ..\Release

@rem Borland Developper Studio specific ---
cd ..\Source
rmdir Example\__history /Q /S
rmdir MediaInfo\__history /Q /S
rmdir MediaInfo\Archive\__history /Q /S
rmdir MediaInfo\Audio\__history /Q /S
rmdir MediaInfo\Image\__history /Q /S
rmdir MediaInfo\Multiple\__history /Q /S
rmdir MediaInfo\Text\__history /Q /S
rmdir MediaInfo\Video\__history /Q /S
rmdir MediaInfoDLL\__history /Q /S
rmdir PreRelease\__history /Q /S
rmdir PreRelease\VCL\__history /Q /S
rmdir PreRelease\WxWidgets\__history /Q /S
cd ..\Release
cd ..\Project
rmdir BCB\__history /Q /S
rmdir BCB\Dll\__history /Q /S
rmdir BCB\Dll\Debug_Build /Q /S
rmdir BCB\Dll\Release_Build /Q /S
rmdir BCB\Example\__history /Q /S
rmdir BCB\Example\Debug_Build /Q /S
rmdir BCB\Example\Release_Build /Q /S
rmdir BCB\Library\__history /Q /S
rmdir BCB\Library\Debug_Build /Q /S
rmdir BCB\Library\Release_Build /Q /S
rmdir BCB\PreRelease\__history /Q /S
rmdir BCB\PreRelease\Debug_Build /Q /S
rmdir BCB\PreRelease\Release_Build /Q /S
rmdir Delphi\__history /Q /S
rmdir Delphi\Example\__history /Q /S
rmdir Delphi\Example\ModelSupport /Q /S
rmdir MSVC\DLL\Win32 /Q /S
rmdir MSVC\DLL\x64 /Q /S
rmdir MSVC\Example\Win32 /Q /S
rmdir MSVC\Example\x64 /Q /S
rmdir MSVC\Library\Win32 /Q /S
rmdir MSVC\Library\x64 /Q /S
cd ..\Release

@rem Code::Blocks specific ---
cd ..\Project
rmdir CodeBlocks\Dll\.objs /Q /S
rmdir CodeBlocks\Library\.objs /Q /S
rmdir CodeBlocks\PreRelease\.objs /Q /S
cd ..\Release

@rem GCC specific ---
cd ..\Project
rmdir GCC\Example\.deps /Q /S
rmdir GCC\Example\.objs /Q /S
rmdir GCC\Library\.deps /Q /S
rmdir GCC\Library\.objs /Q /S
cd ..\Release

@rem Release specific ---
del *.zip *.gz *.bz2 *.lib *.dll *.exp *.a *.so *.7z /Q /S
del MediaInfo*.exe
