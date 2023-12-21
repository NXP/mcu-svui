@echo off
cd env
echo home = %~dp0Python39> pyvenv.cfg
echo implementation = CPython>> pyvenv.cfg
echo version_info = 3.9.7.final.0>> pyvenv.cfg
echo virtualenv = 20.8.1>> pyvenv.cfg
echo include-system-site-packages = false>> pyvenv.cfg
echo base-prefix = %~dp0Python39>> pyvenv.cfg
echo base-exec-prefix = %~dp0Python39>> pyvenv.cfg
echo base-executable = %~dp0Python39\python.exe>> pyvenv.cfg
cd ..
call env\Scripts\activate
cd Scripts\sln_svui_iot_open_boot
python -W ignore open_prog_full.py -fbb -ivd
set /p DUMMY=Hit ENTER to close this shell...