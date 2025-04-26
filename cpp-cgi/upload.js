#!/usr/bin/env node

// Read environment variables
const method = process.env.REQUEST_METHOD || 'GET';
const contentLength = parseInt(process.env.CONTENT_LENGTH || '0', 10);

// Function to read POST data if needed
function readPostData(callback) {
    let body = '';
    process.stdin.on('data', chunk => {
        body += chunk;
    });
    process.stdin.on('end', () => {
        callback(body);
    });
}

var response;
function sendResponse(body) {
    response = "<!DOCTYPE html>";
    response += "<html><body>";
    response += "<h1>JavaScript CGI!</h1>";
    response += "<p>Method: " + method + "</p>";
    response += "<pre>" + body + "</pre>";
    response += "<h2>Environment Variables</h2>";
    response += "<table border='1'>";
    response += "<tr><th>Variable</th><th>Value</th></tr>";
    for (const [key, value] of Object.entries(process.env)) {
        response += "<tr><td>" + key + "</td><td>" + value + "</td></tr>";
    }
    response += "</table>";
    response += "<h2>POST Data</h2>";
    response += "<pre>" + body + "</pre>";
    response += "<h2>Request Headers</h2>";
    response += "<table border='1'>";
    response += "<tr><th>Header</th><th>Value</th></tr>";
    for (const [key, value] of Object.entries(process.env)) {
        if (key.startsWith('HTTP_')) {
            const headerName = key.substring(5).replace(/_/g, '-').toLowerCase();
            response += "<tr><td>" + headerName + "</td><td>" + value + "</td></tr>";
        }
    }
    response += "</table>";
    response += "<h2>Form Data</h2>";
    response += "<table border='1'>";
    response += "<tr><th>Field</th><th>Value</th></tr>";
    const formData = new URLSearchParams(body);
    for (const [key, value] of formData.entries()) {
        response += "<tr><td>" + key + "</td><td>" + value + "</td></tr>";
    }
    response += "</table>";
    response += "</body></html>";

    console.log("HTTP/1.1 200 OK\r");
    console.log("Date: " + new Date().toUTCString() + "\r");
    console.log("Server: Node.js CGI\r");
    console.log("Connection: keep-alive\r");
    console.log("Content-Type: text/html\r");
    console.log("Content-Length: " + response.length + "\r");
    console.log("\r");
    console.log(response);
    console.log("\r");
    
    
}

// Main
if (method === 'POST') {
    readPostData(sendResponse);
} else {
    sendResponse('No body was sent <3');
}
