#!/bin/bash

# Check if an argument is provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 [get|post]"
    exit 1
fi

# Convert argument to lowercase
REQUEST_TYPE=$(echo "$1" | tr '[:upper:]' '[:lower:]')

case $REQUEST_TYPE in
    "get")
        while true; do
            curl http://localhost:8081
            # sleep 1  # Optional: add a delay to avoid flooding the server
        done
        ;;
    "post")
        while true; do
            curl -X POST -F "filename=@/home/mzeggaf/vscode.tar.gz" http://localhost:8081/
            # sleep 1  # Optional: add a delay to avoid flooding the server
        done
        ;;
    *)
        echo "Invalid argument. Use 'get' or 'post'."
        exit 1
        ;;
esac