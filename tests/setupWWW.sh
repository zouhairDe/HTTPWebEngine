#!/bin/bash

		###	hada ki settupi www/ f /tmp/ 

# Check if a site name is provided
if [ $# -eq 0 ]; then
    echo "Error: Please provide a site name"
    exit 1
fi

SITE_NAME=$1

# Check if www/ exists
if [ ! -d "/tmp/www" ]; then
    echo "No www/ found"
    echo "Creating www/ folder..."
    mkdir -p /tmp/www
fi

# Create site-specific folder
SITE_PATH="/tmp/www/$SITE_NAME"
echo "Creating site folder: $SITE_PATH"
mkdir -p "$SITE_PATH"

# Create index.html
echo "Creating index.html..."
echo "<html><body><h1>Welcome to $SITE_NAME</h1></body></html>" > "$SITE_PATH/index.html"

# Copy error folder
if [ -d "./error" ]; then
    echo "Copying error folder..."
    cp -r ./error "$SITE_PATH/"
else
    echo "No error folder found in current directory"
    exit 1
fi

echo "Setup complete for $SITE_NAME"