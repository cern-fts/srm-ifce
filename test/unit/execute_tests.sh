#! /bin/bash
# Copyright (c) Members of the EGEE Collaboration. 2004.
# See http://www.eu-egee.org/partners/ for details on the copyright holders.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# Execute the unit tests
# Author:Todor Manev CERN IT-GT

SCRIPT=$(readlink -f $0)
SCRIPTPATH=`dirname $SCRIPT`
BUILD_DIR=$SCRIPTPATH/../../build  #script is in /test/unit

ccheck_path=`locate libcheck.so.0 | head -1 | sed 's/libcheck.so.0//'`
cgsi_path=`locate libcgsi_plugin_gsoap_2.7.so.1 | head -1 | sed 's/libcgsi_plugin_gsoap_2.7.so.1//'`
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ccheck_path:$cgsi_path

pushd $BUILD_DIR/src &> /dev/null
./gfal_srm_ifce_unittest
res=$?
popd &> /dev/null

exit $res

