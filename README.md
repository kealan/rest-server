# server

[![Build Status](https://travis-ci.org/kealan/server.svg?branch=master)](https://travis-ci.org/kealan/server)

# Quick start

Install travis cli

    gem install travis

Add your secure environmental values.

    travis encrypt DOCKER_USERNAME=username --add
    travis encrypt DOCKER_PASSWORD=password --add

# Start service

Run the container

    docker run -it --rm -p 8000:8000 kealan/rest-server:$(cat VERSION)

Call the service

    curl -i -H "Content-Type: application/json" http://localhost:8000/status --trace-ascii /dev/stdout --trace-ascii /dev/stdout 
    curl -i -H "Content-Type: application/json" -X POST -d '{"user": "bob": "email": "bob@example.com"}' http://localhost:8000/data --trace-ascii /dev/stdout 
    curl -i -H "Content-Type: application/json" -X POST -d '{"user": "bob": "email": "bob@example.com"}' http://localhost:8000/unknown --trace-ascii /dev/stdout 

