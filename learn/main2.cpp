//hada client (in minitalk terms)

#include <algorithm>
#include <climits>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <time.h>
#include <cmath>
#include <set>
#include <sstream>
#include <signal.h>
#include <fstream>
#include <istream>
#include <ostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

using namespace std;

/* ------------------------------------------Mn hna-----------------------------------------  */

namespace ANSIColor                                                                                                                                                                                
{                                                                                                                                                                                              
    enum Code                                                                                                                                                                                  
    {               
		FS_BOLD	 	= 1,    
		FS_DEFAULT 	= 22,                                                                                                                                                                       
        FG_RED      = 31,                                                                                                                                                                      
        FG_GREEN    = 32,                                                                                                                                                                      
        FG_BLUE     = 34,                                                                                                                                                                      
        FG_DEFAULT  = 39,                                                                                                                                                                      
                                                                                                                                                                                               
        BG_RED      = 41,                                                                                                                                                                      
        BG_GREEN    = 42,                                                                                                                                                                      
        BG_BLUE     = 44,                                                                                                                                                                      
        BG_DEFAULT  = 49                                                                                                                                                                  
    };                                                                                                                                                                                         
                                                                                                                                                                                               
class Change                                                                                                                                                                                   
{                                                                                                                                                                                              
  Code code;                                                                                                                                                                                   
                                                                                                                                                                                               
 public:                                                                                                                                                                                       
                                                                                                                                                                                               
 Change(Code _code) : code(_code) {}                                                                                                                                                           
                                                                                                                                                                                               
                                                                                                                                                                                               
  friend ostream&  
    operator<<(ostream& os, const Change& chng)                                                                                                                        
     {                                                                                                                                                                                            
       return os << "\033[" << chng.code << "m";                                                                                                                                                  
     }                                                                                                                                                                                            
                                                                                                                                                                                               
};  
                                                                                                                                                                                              
} //namespace Color 
 
  ANSIColor::Change red(ANSIColor::FG_RED);                                                                                                                                                            
  ANSIColor::Change green(ANSIColor::FG_GREEN); 
  ANSIColor::Change blue(ANSIColor::FG_BLUE); 
  ANSIColor::Change def(ANSIColor::FG_DEFAULT); 
  ANSIColor::Change bg_red(ANSIColor::BG_RED);
  ANSIColor::Change bg_green(ANSIColor::BG_GREEN);
  ANSIColor::Change bg_blue(ANSIColor::BG_BLUE);
  ANSIColor::Change bg_def(ANSIColor::BG_DEFAULT);
  
  ANSIColor::Change bold(ANSIColor::FS_BOLD);
  ANSIColor::Change def_bold(ANSIColor::FS_DEFAULT);


int main() {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        cerr << bold << red << "Error: socket creation failed" << endl;
        return 1;
    }
    cout << red << bold << green << "Socket created successfully: " << client_fd << def << endl;

    // Prepare the server address to connect to
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    
    // Convert IP address from string to binary form 
	long binary_addr = inet_addr("127.0.0.1");
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        cerr << bold << red << "Error: Invalid address" << endl;
        close(client_fd);
        return 1;
    }
	else
		cout << red << bold << blue << "Address: \"127.0.0.1\" successfully converted to:" << binary_addr << def << endl; // 127.0.0.1 = 16777343

    // Connect to server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        cerr << bold << red << "Error: Connection failed" << endl;
        close(client_fd);//memory managment wla zbi, hyd 3lia
        return 1;
    }
    cout << red << bold << green << "Connected to server!" << def << endl;

    // Communication loop
    string message;
    char buffer[1024];
    while (true) {
        cout << red << bold << blue << "Enter message (or 'quit' to exit): " << def;
        getline(cin, message);

        if (message == "quit" || message == "exit" || message == "9awed") {
			send(client_fd, message.c_str(), message.length(), 0);
        	recv(client_fd, buffer, sizeof(buffer), 0);
			cout << red << bold << red << "Server: " << def << buffer << endl;
			cout << red << bold << red << "quiting..." << def << endl;
            break;
        }

        // Send message
        send(client_fd, message.c_str(), message.length(), 0);

        // Receive response
        memset(buffer, 0, sizeof(buffer));
        recv(client_fd, buffer, sizeof(buffer), 0);
        cout << red << bold << green << "Server: " << def << buffer << endl;
    }

    // Clean up
    close(client_fd);
    return 0;
}