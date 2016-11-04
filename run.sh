#!/bin/bash
FILE_PATH="`readlink -f "$0"`"
cd "`dirname "$FILE_PATH"`"
exec "./HTYBrowser" "$@"