FROM ubuntu:16.04
MAINTAINER Dave Dribin

RUN apt-get update && \
    apt-get install -y build-essential libcap-ng0 libcap-ng-dev

WORKDIR /work
