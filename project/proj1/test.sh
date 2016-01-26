#!/bin/bash

readonly HOSTNAME_1="seed-f60-102.ucsd.edu"
readonly HOSTNAME_2="seed-g60-104.ucsd.edu"
readonly PUBLIC_IP_1="169.228.35.31"
readonly PUBLIC_IP_2="169.228.35.66"
readonly LOCALHOST="127.0.0.1"

path="./124_correct"
host=$LOCALHOST

number_of_tests=0
number_of_tests_passed=0
number_of_tests_failed=0
stop_on_failure=false

make
./httpclient unittest

if [ $# -gt 0 ];
then
    if [ $1 == "-f" -o $1 == "-F" -o $1 == "f" -o $1 == "F" ];
    then
       stop_on_failure=true 
    fi
fi

executeTest()
{
    trap 'echo "Trap result code $?"' CHLD
    if [ "$#" == 4 ];
    then
        host=$1
        port=$2
        resource=$3
        expected_output=$4

        actual_output=$( ./http-client http://$host:$port$resource 2> /dev/null ) 
    elif [ "$#" == 3 ];
    then
        host=$1
        resource=$2
        expected_output=$3

        actual_output=$( ./http-client http://$host$resource 2> /dev/null )
    else
        printf "ExecuteTest: Wrong number of arguments."
        exit
    fi

    exit_status=$?

    let "number_of_tests += 1"

    if [ $exit_status -eq 139 ];
    then
        actual_output="SEGMENTATION FAULT"
    elif [ -z "$actual_output" ];
    then
        actual_output="Your program exited without printing anything to stdout."
    fi

    printTestResult $expected_output "$actual_output" $resource


    if [ "$expected_output" == "$actual_output" ];
    then
        let "number_of_tests_passed += 1"
    else
        let "number_of_tests_failed += 1"

        if $stop_on_failure ;
        then
            exit
        fi
    fi
}

printTestResult()
{
    expected_output=$1
    actual_output=$2
    resource=$3

    if [ $expected_output == "$actual_output" ];
    then
        printf "passed\tResource: $resource\n"
    else
        printf "FAILED\tExpected: $expected_output\tActual: $actual_output\tResource: $resource\n"
    fi
}

printf "\nEXECUTING TEST SUITE...\n\n"

printf "TESTING HOSTNAMES\n"
executeTest $HOSTNAME_1 8000 "/" 0
executeTest $HOSTNAME_2 8000 "/index.html" 0


printf "\nTESTING PUBLIC IPs\n"
executeTest $PUBLIC_IP_1 8000 "/" 0
executeTest $PUBLIC_IP_2 8000 "/index.html" 0

printf "\nTESTING NO PORT WITH HOSTNAMES, PUBLIC IPs, and LOCALHOST\n"
executeTest $HOSTNAME_1 "/" 1
executeTest $HOSTNAME_2 "/index.html" 1
executeTest $PUBLIC_IP_1 "/" 1
executeTest $PUBLIC_IP_2 "/index.html" 1
executeTest $LOCALHOST "/" 1
executeTest $LOCALHOST "/index.html" 1

printf "\n"

for port in {8000..8009} 
do
    printf "TESTING LOCALHOST ON PORT: $port\n"

    expected_output=$(($port % 8000))

    if [ $port == 8008 ] || [ $port == 8009 ];
    then
        expected_output=0
    fi

    resource="/"
    executeTest $host $port $resource $expected_output

    for file in $path/*
    do 
            #this variable is used to handle the well-formed 40* responses on ports 8004-8007
            usual_expected_output=$expected_output 

            if [[ ( $file == "./124_correct/forbidden" || $file == "./124_correct/not_here.html" ) 
                    && $port -ge 8004 && $port -le 8007 ]];
            then
                expected_output=0
            fi

            if [ $file == "./124_correct/forbidden" ] || 
                [ $file == "./124_correct/subdir" ];
            then
                for subfile in $file/*;
                do
                    resource=${subfile:13}
                    executeTest $host $port $resource $expected_output
                done
            else
                resource=${file:13}
                executeTest $host $port $resource $expected_output
            fi

            expected_output=$usual_expected_output
    done

    printf "\n"
done

printf "Total Tests: $number_of_tests\n"
printf "Tests Passed: $number_of_tests_passed\n"
printf "Tests Failed: $number_of_tests_failed\n"
