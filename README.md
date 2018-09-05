# Simple RESTful web server

[![Build Status](https://travis-ci.org/kealan/rest-server.svg?branch=master)](https://travis-ci.org/kealan/rest-server)

This is a very simple web server that can parse JSON and respond with JSON.
It is a template upon which more complicated services can be developed.

# Quick start

Install travis cli

    gem install travis

Add your secure environmental values.

    travis encrypt DOCKER_USERNAME=username --add
    travis encrypt DOCKER_PASSWORD=password --add

Build server 

    ./build.sh -s Release 

# Start service

As part of the CI the server docker image is written to Docker hub

Run the container

    docker run --rm -p 8000:8000 kealan/rest-server:$(cat VERSION)

Call the service

    curl -i -H "Content-Type: application/json" http://localhost:8000/status --trace-ascii /dev/stdout --trace-ascii /dev/stdout 
    curl -i -H "Content-Type: application/json" -X POST -d '{"user": "bob": "email": "bob@example.com"}' http://localhost:8000/data --trace-ascii /dev/stdout 
    curl -i -H "Content-Type: application/json" -X POST -d '{"user": "bob": "email": "bob@example.com"}' http://localhost:8000/unknown --trace-ascii /dev/stdout 

