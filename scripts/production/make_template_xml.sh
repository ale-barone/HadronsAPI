#!/usr/bin/zsh

ARG=$#
echo $ARG
if (( $ARG!=3 && $ARG!=5  )); then
  echo "usage: $0 <main*.cpp> <ensID> <tsrc start> [<step>  <tsrc end>]"
  exit 1
fi

TEMPL=$1
ENS=$2

if (( $ARG==3 )); then  
  START=$3
  STEP=1
  STOP=$START
elif (( $ARG==5 )); then
  START=$3
  STEP=$4
  STOP=$5
fi

TSRC=$(seq -s " " $START $STEP $STOP)

DIR=`pwd`

# DIRBIN=binXMLgen
# mkdir -p $DIRBIN
DIRXML=../../../XMLs/${ENS}
mkdir -p $DIRXML

# function to copy token for ti ts tf
function replace_t_tokens(){
	XML=$1

	START_NEW="<start>@ti@<\/start>"
	END_NEW="<end>@tf@<\/end>"
	STEP_NEW="<step>@ts@<\/step>"

  if ! grep -q "@ti@" $XML; then 
  	sed -i "s/<start>.*<\/start>/$START_NEW/g" $XML
	fi
  if ! grep -q "@tf@" $XML; then 
    sed -i "s/<end>.*<\/end>/$END_NEW/g" $XML
  fi
  if ! grep -q "@ts@" $XML; then 
  	sed -i "s/<step>.*<\/step>/$STEP_NEW/g" $XML
  fi
}

# compile xml
function make_xml(){
    templ=$1
    ens=$2
    tsrc=$3
    echo "-------------------------------------"
	  echo "Making xml with tsrc=${tsrc} for ens=${ens} ..."
    sed -e "s/@tsrc@/${tsrc}/g" -e "s/@ensID@/${ens}/g" ${templ} > main.cpp
    mv main.cpp ../
    cd ../build; make -j4; cd ${DIR}

    BIN=XMLgen_tsrc${tsrc}_ens${ens}
    mv ../build/my-hadrons-app $DIRXML/$BIN
    cd $DIRXML; ./$BIN

    #make template
    for f in *.xml; do
      replace_t_tokens $f
    done

    rm $BIN
	cd $DIR
}

# create xml
for (( t=${START}; t<=${STOP}; t=t+${STEP} )) do
  make_xml $TEMPL $ENS $t
done