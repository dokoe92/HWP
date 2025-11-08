@echo off
REM Windows-only version of run.sh. See README.md and run.sh for more info

REM Check if venv directory exists, if not create it
if not exist venv (
    py -m venv venv
)

set PREFIX=.\venv\Scripts\

REM Install dependencies
%PREFIX%pip install sv_ttk pyserial darkdetect coordinates

REM Run virtual peripheral
%PREFIX%python vp.py %*