#! /bin/bash

mkdir -p src/autogen build; aclocal -I /home/tmanev/workspace/lcg_util/stage/share/build/m4; libtoolize --force; autoheader; automake --foreign --add-missing --copy; autoconf mkdir -p src/autogen build; aclocal -I /home/tmanev/workspace/lcg_util/stage/share/build/m4; libtoolize --force; autoheader; automake --foreign --add-missing --copy; autoconf

cd build; /home/tmanev/workspace/lcg_util/org.glite.data.srm-ifce/configure --enable-debug --enable-wall --prefix=/home/tmanev/workspace/lcg_util/org.glite.data.srm-ifce/etics-tmp --with-version=1.11.4 --with-release=1 --with-globus-prefix=/home/tmanev/workspace/lcg_util/repository/vdt/globus/4.0.7-VDT-1.10.1/sl5_x86_64_gcc412 --with-globus-thr-flavor=gcc64dbgpthr --with-globus-nothr-flavor=gcc64dbg --with-gsoap-location=/home/tmanev/workspace/lcg_util/repository/externals/gsoap/2.7.6b/sl5_x86_64_gcc412 --with-gsoap-version=2.7.6b --with-cgsi-gsoap-location=/home/tmanev/workspace/lcg_util/stage --with-swig-prefix=/home/tmanev/workspace/lcg_util/repository/externals/swig/1.3.21/sl5_x86_64_gcc412 --with-lfc-location=/home/tmanev/workspace/lcg_util/stage --with-dpm-location=/home/tmanev/workspace/lcg_util/stage --with-dcap-location=/home/tmanev/workspace/lcg_util/repository/externals/dcache-dcap/1.8.0/sl5_x86_64_gcc412/dcap --with-voms-location=/home/tmanev/workspace/lcg_util/stage 


