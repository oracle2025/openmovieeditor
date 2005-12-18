#!/bin/sh
mkdir openme
cp -R *.cxx *.H *.h *.c openme/
cp -R timeline/ openme/
sloccount --wide --multiproject openme > sloccount_tmp
python sloc2html.py sloccount_tmp > openme_sloccount.html
echo openme_sloccount.html generated
rm -R openme/
rm sloccount_tmp
