#!/bin/sh

# install dependencies in a python virtual environment and 
# run the virtual peripheral
# Works on Linux and Windows with git bash and Python 3 installed

# create virtual environment for installing dependencies
if [ ! -e venv ]
then
	python -m venv venv
fi

PREFIX=./venv/bin/

if [ ! -e ./venv/bin/ ]
then 
	PREFIX=./venv/Scripts/
fi

# install dependencies
${PREFIX}/pip install sv_ttk pyserial darkdetect coordinates


# run virtual peripheral
${PREFIX}/python vp.py $@



