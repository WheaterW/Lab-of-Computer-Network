@echo off
:: appname ������
:: inputname �����ļ���
:: outputname ����ļ���
:: resultname �������̨����ض����ļ���

set protocol="SR"
set appname="./LAB2_RDT/Debug/RDT.exe"
set inputname="input.txt"
set outputname="output.txt"
set resultname="result.txt"

for /l %%i in (1,1,10) do (
    echo Test %appname% %%i:
    %appname% %protocol% > %resultname% 2>&1
    fc /N %inputname% %outputname%
)
pause