#!/bin/sh
#
# test.sh
#
# Test the server
#
# @author Kealan McCusker <kealanmccusker@gmail.com>>
# -----------------------------------------------------------------------------

# NOTES:

export VERSION=$(cat VERSION)
cd ./test

# Change tag in docker-compose file to latest version
cp docker-compose.yml.tpl docker-compose.yml
sed -i "s/tag/$VERSION/" docker-compose.yml

# Start service and test
docker-compose up --build &
sleep 300
docker-compose ps 
docker-compose logs request > log.txt
docker-compose down

# Check log
grep "TEST1 PASSED" ./log.txt
if [ $? = 0 ]; then
    echo "TEST1 PASSED"
else 
    echo "TEST1 FAILED"
    exit 1
fi

grep "TEST2 PASSED" ./log.txt
if [ $? = 0 ]; then
    echo "TEST2 PASSED"
else 
    echo "TEST2 FAILED"
    exit 1
fi

grep "TEST3 PASSED" ./log.txt
if [ $? = 0 ]; then
    echo "TEST3 PASSED"
else 
    echo "TEST3 FAILED"
    exit 1
fi

exit 0

