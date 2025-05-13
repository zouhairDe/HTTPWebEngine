# WebServ - HTTP Server Implementation

## Overview
WebServ is a high-performance HTTP server implementation written in C++98 that handles HTTP requests and responses. It supports multiple virtual servers, route-based handling, CGI execution, and other essential HTTP functionalities.

## Features

- **Configuration File Support**: Dynamic server configuration using `.conf` files
- **Virtual Hosting**: Manage multiple server instances with different domains on the same port
- **Route-Based Request Handling**: Configure different behaviors for different URL paths
- **HTTP Methods**: Support for GET, POST, and DELETE methods
- **Directory Listing**: Optional directory browsing capability
- **File Upload**: Handle file uploads with customizable storage locations
- **CGI Support**: Execute CGI scripts (including JavaScript via Node.js)
- **Error Pages**: Custom error pages for different HTTP status codes

## Building

To build the server:

```bash
make        # build release version
make debug  # build with debug symbols
make opt    # build with optimization flags
```

## Configuration

WebServ uses a simple configuration file format. Here's an example:

```conf
[server, "server1"]
	host = localhost
	port = 8081
	server_names = example.com, example.org
	client_max_body_size = 10M
	root = chatroom/
	error_page_404 = chatroom/error/404.html
	error_page_403 = chatroom/error/403.html
	error_page_500 = chatroom/error/500.html

	[route, "/"]
		index = chatroom/index.html 
		allowed_methods = POST, DELETE, GET
		directory_listing = on
	["/"]

	[route, "/uploads"]
		allowed_methods = POST, GET, DELETE
		upload_store = chatroom/uploads/store/
		client_max_body_size = 10M
		directory_listing = on
	["/uploads"]
["server1"]
```

## Usage

```bash
./webserv <config_file>
```

For example:
```bash
./webserv config/server.conf
```

## Architecture

The project is structured around several main components:

- **ConfigParser**: Handles parsing of configuration files
- **Server**: Represents a virtual server with its own settings
- **Route**: Defines behavior for specific URL paths
- **RequestProcessor**: Processes HTTP requests and generates responses
- **CGI**: Manages Common Gateway Interface script execution
- **File**: Abstracts file operations

## Error Handling

WebServ includes robust error handling with custom error pages and detailed logging of issues. Each HTTP status code (404, 403, 500, etc.) can have a custom error page configured.

## Dependencies

- Standard C++ library
- POSIX-compliant system (for socket operations)
- Node.js (optional, for JavaScript CGI support)

## License

This project was developed as an educational exercise at 1337 School.