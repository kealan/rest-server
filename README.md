# Simple RESTful web server

[![Build Status](https://travis-ci.com/kealan/rest-server.svg?branch=master)](https://travis-ci.com/kealan/rest-server)

This is a very simple web server that can parse JSON and respond with JSON.
It is a template upon which more complicated services can be developed.

# Quick start

Install travis cli

```
sudo apt-get install -y ruby ruby-dev
sudo gem install travis
gem install travis
cd /var/lib/gems/
sudo chmod -R 755 ./
/var/lib/gems/2.5.0/gems/travis-1.8.10/bin/travis encrypt DOCKER_USERNAME=username --add
```

or

```
sudo snap install travis
/snap/bin/travis encrypt DOCKER_USERNAME=username --add
```

```
$ travis login --pro
Outdated CLI version, run `gem install travis`.
We need your GitHub login to identify you.
This information will not be sent to Travis CI, only to api.github.com.
The password will not be displayed.

Try running with --github-token or --auto if you don't want to enter your password anyway.

Username: kealan
Password for kealan: **********************
Two-factor authentication code for kealan: 390134
```

Add your secure environmental values.

```
travis encrypt DOCKER_USERNAME=username --com --add
travis encrypt DOCKER_PASSWORD=password --com --add
```

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

