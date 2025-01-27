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
#include <fcntl.h>
using namespace std;

string to_string(int n) {
	stringstream ss;
	ss << n;
	return ss.str();
}

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
  
/* ------------------------------------------tal hna-----------------------------------------  */// ghir dial output colors hhh


							/*

							example usage:
							
							string s = "Do it in Color
							cout << red << red << s << def
							cout << red << green << s << def;
							cout << red << blue << s << def;                                                                                                                                                                                                                                                                      
							cout << red << def << s << "\n"; 

							*/


// #define AF_INET 2	/* IPv4	*/
// #define AF_INET6 10 /* IPv6	*/
// #define SOCK_STREAM 1 /* stream (connection) socket	*/
// #define SOL_SOCKET 1 /* socket level	*/
// #define SO_REUSEADDR 2 /* allow reuse of local addresses	*/hmmm i get it f subject glik should never timouts.. maybe who knows
// #define INADDR_ANY 0 /* any IP address	*/ Accept connections on any IP address

int main()
{
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);//0 hia protocol, w ghaliban system kiyakhed l appropriate one// chnahoma lakhrin? lahoa3lam
	if (socket_fd == -1)
	{
		cerr << bold << red << "Error: socket creation failed" << endl;
		return 1;
	}
	else 
		cout << red << bold << green << "Socket created successfully\nFile descriptor: " << socket_fd << def_bold << endl;
	
	int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        cerr << bold << red << "Error: setsockopt failed" << endl;
        close(socket_fd);
        return 1;
    }
	else
		cout << red << bold << blue << "Socket options set successfully" << def_bold << endl;
	
	// Prepare the server address structure
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);
	
	// Bind socket to address
    if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        cerr << bold << red << "Error: bind failed" << endl;
        close(socket_fd);
        return 1;
    }
    cout << red << bold << green << "Socket bound successfully" << def << endl;
	
	// Listen for connections
    if (listen(socket_fd, 3) == -1) {
        cerr << bold << red << "Error: listen failed" << endl;
        close(socket_fd);
        return 1;
    }
    cout << red << bold << blue << "Server is listening on port 8080..." << def << endl;
	
	// Accept connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        cerr << bold << red << "Error: accept failed" << endl;
        close(socket_fd);
        return 1;
    }
	close(socket_fd);//sditha hna another client (fte7 terminal) could not connect
    cout << red << bold << green << "Client connected!" << def << endl;// commenti dakchi li lt7t w dkh l google w dkhl l 127.0.0.1:8080 atprinta connected
	
	// Communication loop
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
		
		// system("lsof -i:8080");
		// system("lsof -i:8080 | grep LISTEN");
		// system("lsof -i:8080 | grep ESTABLISHED");
		// system("lsof -i:8080 | grep CLOSE_WAIT");
		// system("lsof -i:8080 | grep TIME_WAIT");
		// system("lsof -i:8080 | grep FIN_WAIT_1");
		// system("lsof -i:8080 | grep FIN_WAIT_2");
		// system("lsof -i:8080 | grep CLOSING");
		// system("lsof -i:8080 | grep LAST_ACK");
		// system("lsof -i:8080 | grep SYN_RECV");
		// system("lsof -i:8080 | grep SYN_SENT");
		
		// system("netstat -tulnap | grep 8080");
		// system("lsof -c server");
        
        // Receive message
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            cout << red << bold << red << "Client disconnected (Empty body: chof dok 2 stora khawyin t7t akhir http header)" << def << endl;
            break;
        }
		
		// if (string(buffer) == "quit" || string(buffer) == "exit" || string(buffer) == "9awed") {
		// 	send(client_fd, "thla ajmi hyd mn hna\n", string("thla ajmi hyd mn hna").length(), 0);
		// 	cout << red << bold << red << "Client disconnected" << def << endl;
		// 	break;
		// }

        cout << red << bold << blue << "Client: " << def << buffer << endl;

        // string response = "HTTP/1.1 200 OK";
		// response += "\nDate: Fri, 01 Jul 2022 12:00:00 GMT";
		// response += "\nServer: Apache/2.4.41 (Ubuntu)";
		// response += "\nLast-Modified: Mon, 13 Jun 2022 10:00:00 GMT";
		// response += "\nContent-Length: 1234";
		// response += "\nContent-Type: text/html; charset=UTF-8";
		// response += "\nConnection: keep-alive";
		// response += "\n\n";
		// response += "<!DOCTYPE html>";
		// response += "<html lang=\"en\">";
		// response += "<head>";
		// response += "<meta charset=\"UTF-8\">";
		// response += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
		// response += "<title>Example Page</title>";
		// response += "</head>";
		// response += "<body>";
		// response += "<h1>Welcome to Example.com!</h1>";	
		// response += "<p>This is an example web page.</p>";
		// response += "</body>";
		// response += "</html>";
		
		
		int fd = open("../error/403.html", O_RDONLY);
		if (fd < 0) {
			cerr << "Error opening file" << endl;
			return 1;
		}
		// Get file size
		off_t file_size = lseek(fd, 0, SEEK_END);
		cerr << red << "File size: " << file_size << endl;
		lseek(fd, 0, SEEK_SET);  //SEEK_SET: Reset to beginning

		// Create buffer of exact size needed
		char* file_buffer = new char[file_size + 1];
		memset(file_buffer, 0, file_size + 1);
		
		cerr << red << "File buffer allocated"<< endl;

		//   Read file in chunks
		ssize_t total_read = 0;
		while (total_read < file_size) {
			ssize_t bytes_read = read(fd, file_buffer + total_read, file_size - total_read);
			if (bytes_read <= 0) break;
			total_read += bytes_read;
		}
		
		cerr << red << "File read successfully" << endl;
		cerr << red << "Total read: " << total_read << endl;

		// 5. Add file content to response
		// if (total_read == file_size) {
		// 	response += string(file_buffer, file_size);
		// }
		
		// cerr << red << "Response size: " << response.length() << endl;
		
        // response += buffer;
		// cerr << bold << green << "Response: " << def << response << endl;
        string http_headers = "HTTP/1.1 200 OK\r\n";
		http_headers += "Date: Fri, 01 Jul 2022 12:00:00 GMT\r\n";
		http_headers += "Server: Apache/2.4.41 (Ubuntu)\r\n";
		http_headers += "Last-Modified: Mon, 13 Jun 2022 10:00:00 GMT\r\n";
		http_headers += "Content-Length: " + to_string(file_size) + "\r\n";
		http_headers += "Content-Type: text/html; charset=UTF-8\r\n";
		http_headers += "Connection: keep-alive\r\n";
		http_headers += "\r\n";
		
		cerr << red << "HTTP headers: " << http_headers << endl;

		// Send headers first
		send(client_fd, http_headers.c_str(), http_headers.length(), 0);

		// Then send file content separately
		if (total_read == file_size) {
			send(client_fd, file_buffer, file_size, 0);
			cerr << red << "File sent successfully" << endl;
			cerr << green << "File size: " << file_size << endl;
			cerr << blue << "File content: " << file_buffer << endl;
		}
    }

    close(client_fd);
    close(socket_fd);
	
	return 0;
}

using namespace std;


/*

cmd = lsof -i == hadchi li lt7t so 3la 3 mhlol 2 mrat?????
COMMAND      PID     USER   FD   TYPE  DEVICE SIZE/OFF NODE NAME
gnome-sof 382527 zouddach   19u  IPv4 2498278      0t0  TCP e2r6p2.1337.ma:46784->151.101.65.91:https (CLOSE_WAIT)
brave     382862 zouddach   19u  IPv4 2536861      0t0  TCP e2r6p2.1337.ma:40710->lb-140-82-112-26-iad.github.com:https (ESTABLISHED)
brave     382862 zouddach   22u  IPv4 2493082      0t0  TCP e2r6p2.1337.ma:43402->151.101.65.32:https (ESTABLISHED)
brave     382862 zouddach   24u  IPv4 2542008      0t0  TCP e2r6p2.1337.ma:57810->162.159.134.234:https (ESTABLISHED)
brave     382862 zouddach   27u  IPv4 2626097      0t0  TCP e2r6p2.1337.ma:34928->server-108-157-109-38.mad56.r.cloudfront.net:https (ESTABLISHED)
brave     382862 zouddach   37u  IPv4 2497527      0t0  TCP e2r6p2.1337.ma:43396->ec2-34-237-73-95.compute-1.amazonaws.com:https (ESTABLISHED)
brave     382862 zouddach   72u  IPv4 2501538      0t0  TCP e2r6p2.1337.ma:50644->45.224.186.35.bc.googleusercontent.com:https (ESTABLISHED)
code      384882 zouddach   22u  IPv4 2626134      0t0  TCP e2r6p2.1337.ma:53670->13.107.246.43:https (ESTABLISHED)
code      384882 zouddach   28u  IPv4 2628764      0t0  TCP e2r6p2.1337.ma:34570->20.189.173.8:https (ESTABLISHED)
code      384882 zouddach   31u  IPv4 2626139      0t0  TCP e2r6p2.1337.ma:44654->lb-140-82-121-6-fra.github.com:https (ESTABLISHED)
code      384882 zouddach   32u  IPv4 2626142      0t0  TCP e2r6p2.1337.ma:48984->lb-140-82-113-21-iad.github.com:https (ESTABLISHED)
code      384949 zouddach   49u  IPv4 2629018      0t0  TCP e2r6p2.1337.ma:45564->lb-140-82-113-21-iad.github.com:https (ESTABLISHED)
code      384949 zouddach   60u  IPv4 2629327      0t0  TCP e2r6p2.1337.ma:49272->20.199.39.224:https (ESTABLISHED)
code      384949 zouddach   61u  IPv4 2626078      0t0  TCP e2r6p2.1337.ma:40044->lb-140-82-113-21-iad.github.com:https (ESTABLISHED)
code      384949 zouddach   62u  IPv4 2630026      0t0  TCP e2r6p2.1337.ma:44650->lb-140-82-121-6-fra.github.com:https (ESTABLISHED)
code      384949 zouddach   66u  IPv4 2628117      0t0  TCP e2r6p2.1337.ma:44760->169.254.169.254:http (SYN_SENT)
reciveHHH 398949 zouddach    4u  IPv4 2629549      0t0  TCP localhost:http-alt->localhost:50460 (ESTABLISHED) ----> //server binary hhh
sendHHHH. 399057 zouddach    3u  IPv4 2627967      0t0  TCP localhost:50460->localhost:http-alt (ESTABLISHED) ----> //client binary hhh

*/