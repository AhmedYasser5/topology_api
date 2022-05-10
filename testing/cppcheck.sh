#!/bin/sh
cppcheck --bug-hunting --cppcheck-build-dir=cppcheck_code_analysis --enable=all -I include --language=c++ --platform=unix64 --report-progress --std=c++14 --verbose $@ .
exit $?
