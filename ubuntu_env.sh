#!/bin/bash

apt-get update
apt-get install -y sudo
sudo apt-get install -y git
sudo apt-get install -y python3-pip
sudo apt-get install -y cmake
pip3 install -U conan
