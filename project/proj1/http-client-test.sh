# /bin/sh

echo "Building http-client..."
make clean
make

echo "\nGET http://www.google.com"
./http-client http://www.google.com