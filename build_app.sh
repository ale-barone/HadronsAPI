#!/usr/bin/zsh

# Hadrons src dir
SRC='/path/to/Hadrons/source'
# Hadrons install dir
HAD='/path/to/hadrons/install'
# Grid install dir
GRID='/path/to/grid/install'
# HadronsAPI install dir (from https://github.com/ale-barone/HadronsAPI on master branch)
API=`pwd`'/API'
# dir to build the application
DIR='app'

cp -r ${SRC}/application-template $DIR
cd $DIR
./bootstrap.sh
mkdir build
cd build
../configure --with-grid=$GRID --with-hadrons=$HAD CC=clang CXX=clang++ CXXFLAGS="-g -std=c++11 -I${API}"

mkdir template
cp scripts/main_template.cpp
cp scripts/make_template_xml.sh

mkdir ../template
cd ../template
cp ../../scripts/main_template.cpp .
cp ../../scripts/make_template_xml.sh .

