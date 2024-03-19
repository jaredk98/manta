@echo off
:: usage: boot.bat -project=<project> -os=<os> -architecture=<architecture> -toolchain=<toolchain> -config=<config> -gfx=<api> -clean=<0/1> -run=<0/1>
	:: -os=             windows, linux, macos
	:: -architecture=   x64, arm, arm64
	:: -toolchain=      msvc, llvm, gnu
	:: -config=         release, debug

:: The first thing we should do is rebase the working directory so the script
:: can be called from anywhere while actually working at the root of the repo.
pushd "%~dp0\"

set toolchain=msvc
set project=

:parse_args
	:: Check if we've finished parsing arguments
	if "%~1" == "" goto build

	:: Parse Arguments
	if /i "%~1" == "-project" (
		set project=%~2
	) else (
		shift
	)

	if /i "%~1" == "-toolchain" (
		set toolchain=%~2
	) else (
		shift
	)

	goto parse_args

:build
	:: Clear Terminal
	cls

	:: Require a project
	if "%project%" == "" (
		echo Bootstrap failed: No -project specified
		goto final
	)

	:: Ensure project exists
	if not exist projects\%project% (
		echo Bootstrap failed: project '%project%' does not exist!
		goto final
	)

	:: Load vcvars64
	if %toolchain% == msvc (
		if "%VSINSTALLDIR%" == "" (
			call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
		)
	)

	:: Skip this step if boot.exe already exists
	if exist projects\%project%\output\boot\boot.exe (
		goto strap
	)

	:: Make sure the projects\%project%\output\boot\objects directory exists
	if not exist projects\%project%\output\boot\objects (
		mkdir projects\%project%\output\boot\objects
	)

	:: Call the compiler to make boot.exe
	echo Boot: %project%, %toolchain% [%*]
	if %toolchain% == msvc (
		cl source\boot\boot.cpp -std:c++20 -EHsc -Isource -Isource\boot -nologo -MD -Fe:projects\%project%\output\boot\boot.exe -Fo:projects\%project%\output\boot\objects
	)
	if %toolchain% == llvm (
		clang source\boot\boot.cpp -std=c++20 -fno-exceptions -Isource -Isource\boot -o projects\%project%\output\boot\boot.exe
	)
	if %toolchain% == gnu (
		gcc source\boot\boot.cpp -std=c++20 -fno-exceptions -Isource -Isource\boot -o projects\%project%\output\boot\boot.exe
	)

:strap
	call projects\%project%\output\boot\boot.exe -verbose=0 %*
	goto final

:final
	popd