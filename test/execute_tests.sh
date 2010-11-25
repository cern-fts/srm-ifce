#! /bin/bash
# Copyright (c) Members of the EGEE Collaboration. 2010.
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
# Execute test suites (unit and regression tests). 
# Authors: Todor Manev IT-GT CERN

SCRIPT=$(readlink -f $0)
SCRIPTPATH=`dirname $SCRIPT`

if [ $(uname -m) == "x86_64" ]; then
	LIB_FOLDER_TYPE=lib64
else
	LIB_FOLDER_TYPE=lib
fi

#goto  workspace folder (../../)
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SCRIPTPATH/../../stage/$LIB_FOLDER_TYPE
echo $LD_LIBRARY_PATH
echo $LIBDIR
if [ -n "$1" ]; then
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$1
fi
echo $LD_LIBRARY_PATH
echo $LIBDIR

function execute_test {

    local testdir=$SCRIPTPATH/$1

    echo -e "\nExecuting test suite \"$1\"\n"
    pushd $testdir &> /dev/null
    ./execute_tests.sh
    local res=$? 
    popd &> /dev/null
    
    if [ $res != 0 ] ; then
        echo 
        echo -e "\nA test in \"$testdir\" failed.\n"
        exit 1
    fi
    echo -e "\nAll tests in \"$testdir\" passed.\n"   
}


execute_test unit
#execute_test regression
#execute_test certification_tests

