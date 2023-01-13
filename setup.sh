# © Copyright 2023, D0MlNIC, All Rights Reserved.

#!/usr/bin/env bash

if [ $# = 1 ]; then
    directory="$1"
else
    directory="/usr/share/"
fi

app_name="Privacy_Enforced"
version="V1.0"
company="/Companies/"
path=$directory$app_name

if [ ! -d "$path" ]; then
    mkdir "$path"
    #cd "$path" # || exit
    echo "[App Details]" > "$path""/config.ini"
    echo "app_name = $app_name" >> "$path""/config.ini"
    echo "version = $version" >> "$path""/config.ini"
    echo "path = $path" >> "$path""/config.ini"

    cp -r Companies $path$company
    cp -r *.ini "$path/"
fi