#!/bin/bash
#
# build.sh
#
# Build Server
#
# @author Kealan McCusker <kealanmccusker@gmail.com>
# ------------------------------------------------------------------------------

# NOTES:

# set -x 
usage()
{
cat << EOF
usage: $0 -s [Debug Release Coverage ASan][-h] [-d]

OPTIONS:
   -s      Build Server
   -d      Delete build files
   -k      Kill process
   -h      Print usage
EOF
}

CURRENTDIR=${PWD}

# Check for no options defined
if [ $# -eq 0 ];
then
    usage
    exit 
fi

function delete_files()
{
  echo "clean"
  rm -rf build
  rm -rf target
  rm -rf main
}

function build_server()
{
  echo "SAKKE"
  cd $CURRENTDIR
  rm -rf target/$BUILD_TYPE
  mkdir -p target/$BUILD_TYPE
  cd target/$BUILD_TYPE
  cmake -D SERVICE=test -D CMAKE_BUILD_TYPE=$BUILD_TYPE -D CMAKE_INSTALL_PREFIX=${CURRENTDIR}/build/$BUILD_TYPE ../..
  make 
  cp ./src/main $CURRENTDIR
  # make test
  make install
}


while getopts ":hs:kd" OPTION
do
     case $OPTION in
         h)
             usage 
             exit 1 
             ;;
         s)
             BUILD_TYPE=$OPTARG
             build_server
             ;;
         d)
             delete_files
             exit 0
             ;;
         k)
             echo "killall -q main || true"
             killall -q main || true
             exit 0
             ;;
         :) 
             echo "ERROR: Missing required argument"
             usage
             exit 1
             ;;
         \?)
             echo "ERROR: Invalid option: -$OPTARG"
             usage
             exit 1
             ;;
     esac
done
exit 0

