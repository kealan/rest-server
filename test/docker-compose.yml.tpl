version: '3'

services:

  rest-server:
    image: "kealan/rest-server:tag"
    container_name: "rest-server"
    hostname: "rest-server"
    ports:
      - "8001:8000"

  request:
    build: ./request
    container_name: "request"
    depends_on:
      - rest-server


