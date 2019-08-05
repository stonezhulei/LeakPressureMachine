@echo off
call "%VS100COMNTOOLS%vsvars32.bat"

if exist "deploy" (
	rmdir deploy /q/s
)

cd Leakpress

:MSBuild /t:Clean;Rebuild /p:Configuration=Release /m
msbuild -t:clean -p:configuration="release" -m -t:rebuild

cd ..
mkdir deploy
if exist "deploy" (
	copy Leakpress\Release\Leakpress.exe deploy\Leakpress.exe
	copy Leakpress\Release\MfcDllTool.dll deploy\MfcDllTool.dll
	xcopy Leakpress\Release\*.pdb deploy\*.pdb
	xcopy Leakpress\Leakpress\Config\*.ini deploy\*.ini
	xcopy Leakpress\Leakpress\DllTool\dll\*.dll deploy\*.dll
	xcopy Leakpress\Leakpress\DllTool\dll\VC100.CRT\release\*.dll deploy\*.dll
)

@echo on

pause