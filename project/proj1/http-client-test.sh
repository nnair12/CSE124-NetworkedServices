# /bin/sh

echo "Building http-client..."
make

# 200 OK - returns 0
echo -n "GET http://seed-f60-102.ucsd.edu:8000 ... "
if [[ $(./http-client http://seed-f60-102.ucsd.edu:8000) = 0 ]]; then
    echo "OK"
else
    echo "FAIL"
fi

# 200 OK - returns 0
echo -n "GET http://seed-f60-102.ucsd.edu:8000/subdir/file1.html ... "
if [[ $(./http-client http://seed-f60-102.ucsd.edu:8000/subdir/file1.html) = 0 ]]; then
    echo "OK"
else
    echo "FAIL"
fi

# 403 FORBIDDEN - returns 0
echo -n "GET http://seed-f60-102.ucsd.edu:8000/forbidden/file2.html ... "
if [[ $(./http-client http://seed-f60-102.ucsd.edu:8000/forbidden/file2.html) = 0 ]]; then
    echo "OK"
else
    echo "FAIL"
fi

# 404 NOT FOUND - returns 0
echo -n "GET http://seed-f60-102.ucsd.edu:8000/not_here.html ... "
if [[ $(./http-client http://seed-f60-102.ucsd.edu:8000/not_here.html) = 0 ]]; then
    echo "OK"
else
    echo "FAIL"
fi

# 200 OK - returns 0
echo -n "GET http://seed-f60-102.ucsd.edu:8000/kitty1.jpg ... "
if [[ $(./http-client http://seed-f60-102.ucsd.edu:8000/kitty1.jpg) = 0 ]]; then
    echo "OK"
else
    echo "FAIL"
fi

# 200 OK - returns 0
echo -n "GET http://seed-f60-102.ucsd.edu:8000/kitty2.png ... "
if [[ $(./http-client http://seed-f60-102.ucsd.edu:8000/kitty2.png) = 0 ]]; then
    echo "OK"
else
    echo "FAIL"
fi



# BAD SOCKET - returns 1
#echo -n "GET http://seed-f60-102.ucsd.edu:8001 ... "
#if [[ $(./http-client http://seed-f60-102.ucsd.edu:8001) = 1 ]]; then
#    echo "OK"
#else
#    echo "FAIL"
#fi

# PREMATURE CLOSE - returns 2
echo -n "GET http://seed-f60-102.ucsd.edu:8002 ... "
if [[ $(./http-client http://seed-f60-102.ucsd.edu:8002) = 2 ]]; then
    echo "OK"
else
    echo "FAIL"
fi

# BAD SERVER STATUS - returns 3
echo -n "GET http://seed-f60-102.ucsd.edu:8003 ... "
if [[ $(./http-client http://seed-f60-102.ucsd.edu:8003) = 3 ]]; then
    echo "OK"
else
    echo "FAIL"
fi

# BAD RESPONSE HEADERS - returns 4
echo -n "GET http://seed-f60-102.ucsd.edu:8004 ... "
if [[ $(./http-client http://seed-f60-102.ucsd.edu:8004) = 4 ]]; then
    echo "OK"
else
    echo "FAIL"
fi

# BAD RESPONSE BODY - returns 5
echo -n "GET http://seed-f60-102.ucsd.edu:8005 ... "
if [[ $(./http-client http://seed-f60-102.ucsd.edu:8005) = 5 ]]; then
    echo "OK"
else
    echo "FAIL"
fi

# WRONG CONTENT LENGTH - returns 6
echo -n "GET http://seed-f60-102.ucsd.edu:8006 ... "
if [[ $(./http-client http://seed-f60-102.ucsd.edu:8006) = 6 ]]; then
    echo "OK"
else
    echo "FAIL"
fi

# WRONG CONTENT TYPE - returns 7
echo -n "GET http://seed-f60-102.ucsd.edu:8007 ... "
if [[ $(./http-client http://seed-f60-102.ucsd.edu:8007) = 7 ]]; then
    echo "OK"
else
    echo "FAIL"
fi

# 200 OK - returns 0
echo -n "GET http://seed-f60-102.ucsd.edu:8008 ... "
if [[ $(./http-client http://seed-f60-102.ucsd.edu:8008) = 0 ]]; then
    echo "OK"
else
    echo "FAIL"
fi

# 200 OK - returns 0
echo -n "GET http://seed-f60-102.ucsd.edu:8009 ... "
if [[ $(./http-client http://seed-f60-102.ucsd.edu:8009) = 0 ]]; then
    echo "OK"
else
    echo "FAIL"
fi

echo "Cleaning up http-client..."
make clean