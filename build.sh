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
usage: $0 -s [Debug Release Coverage ASan][-h] [-d] [-c]

OPTIONS:
   -s      Build Server
   -d      Build and run docker container n.b. must be built locally first
   -c      Delete build files
   -k      Kill process / docker comtaiers
   -f      Format code
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
  echo "Cleanup"
  rm -rf build
  rm -rf target
  rm -rf main
}

function build_server()
{
  echo "Build Server"
  cd $CURRENTDIR
  rm -rf target/$BUILD_TYPE
  mkdir -p target/$BUILD_TYPE
  cd target/$BUILD_TYPE
  cmake -D SERVICE=test -D CMAKE_BUILD_TYPE=$BUILD_TYPE -D CMAKE_INSTALL_PREFIX=${CURRENTDIR}/build/$BUILD_TYPE ../..
  make 
  # make test
  make install
  echo "cp ${CURRENTDIR}/build/$BUILD_TYPE/bin/main ${CURRENTDIR}"
  cp ${CURRENTDIR}/build/$BUILD_TYPE/bin/main ${CURRENTDIR}
}

function build_docker()
{
  echo "Build and run docker"
  export VERSION=$(cat VERSION)
  docker build -t rest-server .
  docker tag rest-server $DOCKER_USERNAME/rest-server:$VERSION
  docker run -it --rm -p 8000:8000 kealan/rest-server:1.1.0
}


while getopts ":hs:kdcf" OPTION
do
     case $OPTION in
         h)
             usage 
             exit 1 
             ;;
         s)
             BUILD_TYPE=$OPTARG
             delete_files
             build_server
             ;;
         f)
             astyle --style=allman --recursive --suffix=none '*.c'
             exit 0
             ;;
         c)
             delete_files
             exit 0
             ;;
         d)
             build_docker
             ;;
         k)
             echo "killall -q main || true"
             killall -q main || true
             CONTAINER_ID=`docker ps -a | grep rest-server | cut -c1-12`
             docker stop $CONTAINER_ID
             docker rm  $CONTAINER_ID
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

