@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
cd /d C:\dev\theospdf
msbuild vs2022\SumatraPDF.sln /t:SumatraPDF /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145 /m /v:minimal /nologo
echo BUILD_EXIT_CODE=%ERRORLEVEL%
