#!/bin/bash

# Color vars
SUCCESS_COLOR='\033[32;1m'
WARNING_COLOR='\033[33m'
ERROR_COLOR='\033[31;1m'
MESSAGE_COLOR='\033[94;1m'
NC='\033[0m'


usage() {
    echo "Usage: $0 -d <directory> -t <test-pattern> [-n (no cleanup)]"
    exit 1
}

clean_up_libfs() {
    killall -- libfs-daemon > /dev/null  2>&1 
    rm -rf "$HOME/.local/share/libfs"
    libfs-daemon > daemon.log 2>&1  &  
}

PATH=$PATH:../binaries
DIRECTORY="."
TEST='*'
CLEANUP=1

while getopts "d:t:n" opt; do
    case $opt in
        d)
            DIRECTORY=$OPTARG
            ;;
        t)
            TEST=$OPTARG
            ;;
        n)
            CLEANUP=0
            ;;
        *)
            usage
            ;;
    esac
done

# Clean up libfs

if [ $CLEANUP -eq 1 ]; then
    clean_up_libfs
fi

# Gather the script files with their expected outputs

printf "Directory: $DIRECTORY\n"
TESTS=$(find $DIRECTORY -name "$TEST.sh")
TESTS=$(echo "$TESTS" | grep -v "test.sh")
TEST_COUNT=$(echo "$TESTS" | wc -l)
TESTS_PASSED=0

if [ $TEST_COUNT -eq 0 ] || [ -z "$TESTS" ]; then
    printf "${ERROR_COLOR}No tests found!${NC}\n${MESSAGE_COLOR}If you are sure there are tests, try using a different test name pattern.${NC}\n"
    exit 1
fi

printf "Discovered ${MESSAGE_COLOR}${TEST_COUNT}${NC} tests\n"

for test in $TESTS; do
    printf "Running ${MESSAGE_COLOR}$test${NC}... "

    # Run the test
    output=$(bash $test)
    status=$?

    expected_exit_code=0
    expected_output=""
    specpath=$(echo $test | sed 's/\.sh/\.spec/')
    if [ -f "$specpath" ]; then
        spec=$(cat $specpath)
        expected_exit_code=$(echo "$spec" | head -n 1)
        expected_output=$(echo "$spec" | tail -n +2)
    else
        printf "\n${WARNING_COLOR}No spec found, assuming exit code 0 and no output${NC}... "
    fi

    # Check the output
    if [ "$status" = "$expected_exit_code" ]; then
        if [ "$output" = "$expected_output" ]; then
            printf "${SUCCESS_COLOR}OK${NC}\n"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            printf "${ERROR_COLOR}FAIL${NC}\n"
            printf "Output diff:\n"
            diff --color=always -u <(echo "$expected") <(echo "$output")
        fi
    else
        printf "${ERROR_COLOR}FAIL${NC}\n"
        printf "Test exited with status $status\n"
    fi
done

if [ $TESTS_PASSED -eq $TEST_COUNT ]; then
    printf "${SUCCESS_COLOR}All tests passed${NC}\n"
else
    printf "${ERROR_COLOR}$TESTS_PASSED/$TEST_COUNT tests passed${NC}\n"
fi

