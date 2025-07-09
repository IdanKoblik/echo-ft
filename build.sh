#!/bin/bash

protoc --c_out=. filechunk.proto
gcc sender.c filechunk.pb-c.c -lprotobuf-c -o sender
gcc receiver.c filechunk.pb-c.c -lprotobuf-c -o receiver

