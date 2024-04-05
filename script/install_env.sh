#!/bin/bash

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'
BOLD='\033[1m'

echo -e "${GREEN}${BOLD}--- robot_viewer runtime begin ----${NC}"

## 1.0 environment variable
script=$(readlink -f "$0")
route=$(dirname "$script")
log_dir=/opt/robot_viewer/log

if [ ! -d ${log_dir} ]; then
    sudo mkdir -p ${log_dir}
fi
session_user=`echo ${SUDO_USER:-$USER}`
uid=`id -un ${session_user}`
gid=`id -gn ${session_user}`
sudo chown -R ${uid}:${gid} ${log_dir}
sudo touch ${log_dir}/robot_viewer.log
sudo chown ${uid}:${gid} ${log_dir}/robot_viewer.log
sudo chmod 664 ${log_dir}/robot_viewer.log

## 2. install lib
apt_pkgs+=(
    #qt
    qt5-default
    qtcreator
    qtbase5-dev
    qtchooser
    qt5-qmake
    qtbase5-dev-tools

    # log
    libspdlog-dev

    #jsoncpp
    libjsoncpp-dev

    # xml
    #libtinyxml-dev
)

for n in ${apt_pkgs[@]}
do
    echo -e "${RED}Processing dpkg pkg ----> " $n ${NC}
    sudo apt-get install -y $n || { fail_dpkgs+=($n); continue; }
    echo -e "${RED}Processing dpkg pkg ----> " $n " $BOLD OK" ${NC}
done


echo -e "${GREEN}${BOLD}--- robot_viewer runtime end ----${NC}"

exit 0
