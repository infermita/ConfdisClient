#!/bin/sh
INSTALL_DIR=`dirname $0`

if [ "${INSTALL_DIR}" = "." ]; then

    INSTALL_DIR=${PWD}

fi

cd ${INSTALL_DIR}

cat confdis.sql | sqlite3 confdis.db

