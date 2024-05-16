@echo off
cd /d %~dp0
python ..\excel_to_hpp.py lang-zh-hans.xlsx lang-zh-hans.h lang-zh-hans.txt
