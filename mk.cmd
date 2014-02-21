@echo off

SET EMXOMFLD_LINKER=c:\ibmcpp\bin\ilink.exe -NOFREE -PM:VIO -NoLogo
set Args=-f Makefile.os2

if x%1 == x goto IssueMake

:WhileArgsOkay
if x%1 == x goto IssueMake
set args=%Args% %1

rem
rem Slide the arg list left and do it again :)
rem
shift
goto WhileArgsOkay

:IssueMake
gmake %Args%

goto :Done

:Usage
echo Usage:
echo  mk [nmake args] 
echo  Example:
echo  [D:\] RePrint lockaa_a pinsba_m
echo ÿ
echo mk issues nmake with the default argument of /nologo
:Done
