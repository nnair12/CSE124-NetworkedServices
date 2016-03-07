#!/bin/sh

thrift -out server/gen-py -r --gen py thrift/TritonTransfer.thrift
thrift -out client/gen-py -r --gen py thrift/TritonTransfer.thrift