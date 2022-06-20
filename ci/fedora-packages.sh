#!/usr/bin/env bash
set -e

if [[ -f /usr/bin/dnf ]]; then
  dnf update -y
  dnf install -y dnf-plugins-core git rpm-build which tree python2 \
                 cmake make gcc gcc-c++
else
  yum install -y yum-utils git rpm-build epel-rpm-macros make which tree python2 \
                 cmake cmake3 make gcc gcc-c++
fi
