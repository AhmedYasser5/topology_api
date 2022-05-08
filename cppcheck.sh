#!/bin/sh
cppcheck --bug-hunting --cppcheck-build-dir=cppcheck_code_analysis --enable=all -I include/ -I /usr/include -I /usr/include/gnu/ -I /usr/include/c++/11.2.0/ -I /usr/include/c++/11.2.0/x86_64-pc-linux-gnu/ --language=c++ --platform=unix64 --report-progress --std=c++14 --verbose $@ .
exit $?
