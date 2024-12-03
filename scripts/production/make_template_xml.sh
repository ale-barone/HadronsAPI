#!/usr/bin/zsh

ARG=$#
if (( $ARG < 1 || $ARG==3 || $ARG>4  )); then
  echo "usage: $0 <ensID> <tsrc start> [<step>  <tsrc end>]"
  exit 1
fi

if (( $ARG==2 )); then
  ENS=$1
  START=$2
  STEP=1
  STOP=$START
elif (( $ARG==4 )); then
  ENS=$1
  START=$2
  STEP=$3
  STOP=$4
fi

TSRC=$(seq -s " " $START $STEP $STOP)

DIR=`pwd`
TEMPL=main_template.cpp

# DIRBIN=binXMLgen
# mkdir -p $DIRBIN
DIRXML=XMLs/${ENS}
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
    TEMPL=$1
    ENS=$2
    TSRC=$3
    echo "-------------------------------------"
	  echo "Making xml with tsrc=${TSRC} for ens=${ENS} ..."
    sed -e "s/@tsrc@/${TSRC}/g" -e "s/@ensID@/${ENS}/g" ${TEMPL} > main.cpp
    mv main.cpp ../
    cd ../build; make -j4; cd ${DIR}
 
    BIN=XMLgen_tsrc${TSRC}_ens${ENS}
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
