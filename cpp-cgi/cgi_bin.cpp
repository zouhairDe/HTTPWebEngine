#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

int main() {

    string contentLengthStr = getenv("CONTENT_LENGTH") ? getenv("CONTENT_LENGTH") : "0";
    int contentLength = stoi(contentLengthStr);

    string postData;
    for (int i = 0; i < contentLength; ++i) {
        char c = cin.get();
        postData += c;
    }

    string responseToSend;
    responseToSend +=  "<!DOCTYPE html><html><body>";
    responseToSend +=  "<h1>Thank you!</h1>";
    responseToSend +=  "<p>Your form was received by the CGI script.</p>";
    responseToSend +=  "<pre>" + postData + "</pre>";
    responseToSend +=  "</body></html>";

    cout << "HTTP/1.1 200 OK\r\n";
    cout << "Content-Type: text/html\r\n";
    cout << "Content-Length: " << responseToSend.length() << "\r\n";
    cout << "Connection: close\r\n";
    cout << "\r\n";
    cout << responseToSend;
    cout.flush();

    return 0;
}
