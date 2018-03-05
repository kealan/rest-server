#!/bin/sh
#
# start.sh
#
# Test the server
#
# @author Kealan McCusker <kealanmccusker@gmail.com>>
# -----------------------------------------------------------------------------

# NOTES:

echo "Start test"
sleep 240

/usr/bin/curl -i -H "Content-Type: application/json" http://rest-server:8000/status > response.txt
grep "HTTP/1.1 200 OK" ./response.txt
if [ $? = 0 ]; then
    echo "TEST1 PASSED"
else 
    echo "TEST1 FAILED"
fi

/usr/bin/curl -i -H "Content-Type: application/json" -X POST -d '{"user": "bob": "email": "bob@example.com"}' http://rest-server:8000/data > response.txt
(grep "HTTP/1.1 200 OK" ./response.txt) && (grep "bob" ./response.txt) && (grep "bob@example.com" ./response.txt)
if [ $? = 0 ]; then
    echo "TEST2 PASSED"
else 
    echo "TEST2 FAILED"
fi

/usr/bin/curl -i -H "Content-Type: application/json" -X POST -d '{"user": "bob": "email": "bob@example.com"}' http://rest-server:8000/unknown  > response.txt
grep "HTTP/1.1 400 Bad Request" ./response.txt
if [ $? = 0 ]; then
    echo "TEST3 PASSED"
else 
    echo "TEST3 FAILED"
fi

echo "End test"
sleep 500

