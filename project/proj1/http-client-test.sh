# /bin/sh

echo "Building http-client..."
make

echo "\nGET http://seed-f60-102.ucsd.edu:8000/kitty1.jpg"
./http-client http://seed-f60-102.ucsd.edu:8000/kitty1.jpg

echo "\nGET http://seed-f60-102.ucsd.edu:8000/kitty2.png"
./http-client http://seed-f60-102.ucsd.edu:8000/kitty2.png

echo "\nGET http://seed-f60-102.ucsd.edu:8000/subdir/file1.html"
./http-client http://seed-f60-102.ucsd.edu:8000/subdir/file1.html

echo "\nGET http://seed-f60-102.ucsd.edu:8000/forbidden/file2.html"
./http-client http://seed-f60-102.ucsd.edu:8000/forbidden/file2.html

echo "\nGET http://seed-f60-102.ucsd.edu:8000/not_here.html"
./http-client http://seed-f60-102.ucsd.edu:8000/not_here.html

echo "\nGET http://www.google.com"
./http-client http://www.google.com

echo "\nGET google.com"
./http-client google.com

echo "\nGET 132.239.253.94"
./http-client 132.239.253.94

echo "\nGET http://132.239.253.94"
./http-client http://132.239.253.94

echo "Cleaning up http-client..."
make clean