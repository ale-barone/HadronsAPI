#!/usr/bin/zsh

ARG=$#
if (( $ARG < 1 || $ARG==2 || $ARG > 3 )); then
  echo "usage: $0 <tsrc start> [<step>  <tsrc end>]"
  exit 1
fi

if (( $ARG==1 )); then
  START=$1
  STEP=1
  STOP=$START
elif (( $ARG==3 )); then
  START=$1
  STEP=$2
  STOP=$3
fi

TSRC=$(seq -s " " $START $STEP $STOP)

DIR=`pwd`
TEMPL=main_template.cpp

# DIRBIN=binXMLgen
# mkdir -p $DIRBIN
DIRXML=XMLs
mkdir -p $DIRXML

# function to copy token for ti ts tf
function replace_t_tokens(){
	XML=$1

	START_NEW="<start>@ti@<\/start>"
	END_NEW="<end>@tf@<\/end>"
	STEP_NEW="<step>@ts@<\/step>"

	sed -i "s/<start>.*<\/start>/$START_NEW/g" $XML
	sed -i "s/<end>.*<\/end>/$END_NEW/g" $f
	sed -i "s/<step>.*<\/step>/$STEP_NEW/g" $XML
}

# compile xml
function make_xml(){
    TEMPL=$1
    TSRC=$2
    echo "-------------------------------------"
	  echo "Making xml with tsrc=${TSRC}..."
    sed -e "s/@tsrc@/${TSRC}/g" ${TEMPL} > main.cpp
    mv main.cpp ../
    cd ../build; make -j4; cd ${DIR}
 
    BIN=XMLgen_tsrc${TSRC}
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
  make_xml $TEMPL $t
done
