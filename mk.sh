#!/bin/bash

ROOT_DIR="$(dirname $(readlink -f "${BASH_SOURCE[0]}"))"
BUILD_DIR=${ROOT_DIR}/build

clean() {
    if [ "$1" = "all" ]; then
        echo "Remove ${BUILD_DIR}"
        rm -r ${BUILD_DIR}
    else
        cd ${BUILD_DIR} > /dev/null
            make clean
        cd - > /dev/null
    fi
}

setenv() {
    echo "Create ${BUILD_DIR}"
    mkdir -p ${BUILD_DIR}
    cd ${BUILD_DIR} > /dev/null
        cmake ..
    cd - > /dev/null
}

build() {
    if [ ! -d ${BUILD_DIR} ]; then
        setenv
    fi

    cd ${BUILD_DIR} > /dev/null
        make $@
    cd - > /dev/null
}

#
# Main
#
case "$1" in 
    "clean") shift
             clean $@;;
    "setenv") setenv ;;
    *)  build $@;;
esac 
