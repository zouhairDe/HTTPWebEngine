/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestProcessor.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 16:29:15 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 20:54:44 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPROCCESOR_HPP
# define REQUESTPROCCESOR_HPP

# include "Global.hpp"
# include "Server.hpp"
# include "CGI.hpp"

# define REQUEST_BUFFER_SIZE 1024 * 16

class Server;
class Route;
class File;
class CGI;

class RequestProcessor {
    private:
        string				_request;       //ex: GET /index.html HTTP/1.1\nHost: www.example.com\n....
        string				_method;        //ex: GET
        string				_uri;           //ex: /index.html
        string				_host;          //ex: www.example.com
        string				_port;          //ex: 80
        string				_connection;    //for subject wa9ila always khas tkon keep alive mn 3ndna hna so hadi makathmch
        size_t				_content_length;//ex: 123
		string				_content_type;  //ex: application/x-www-form-urlencoded or multipart/form-data
        string				_body;          //ex: name=Zouhair&age=22 or for upload file ex: file=@/path/to/file and this just for POST method
        string				_query;         //l cgi mnb3d
        string				_cookie;        //for bonus
        string				_filename;      //for upload file
        string				_fileContent;   //actual content of uploaded file
		string				_transfere_encoding;
		bool				_headers_parsed;   //true if headers are parsed
		bool				_received;      //true if the request is fully received
		bool				_responded;      //true if the request is fully sent
		int					_client_socket; //socket of client from epoll
		size_t				_body_size;      //current size of body
        map<string, string>	_formFields; //for storing form fields
        string				_fileContentType;
		string				_responseToSend;
		int					_status;
		string				_authorization;
		vector<string>		dangerousPatterns;
		ofstream			*_fileStream;
		
		
		public:
		RequestProcessor();
		RequestProcessor(string request, string __port, Server *server);
		// RequestProcessor(const RequestProcessor &req);
		RequestProcessor &operator=(const RequestProcessor &req);
		~RequestProcessor();
		
		int					parseHeaders(string req);
		int					parseBody(string req);
		bool				processMultipartFormData(const string &boundary);
		void				parseFormUrlEncoded(const string &body);
		void				debugRequest() const;
		string  			getExtensionFromContentType(const string& contentType) const;
		void				parseTextPlainUpload(const string &body);
		Server				*_server;
		Route 				*_route;
		File				*_file;
		CGI					*_cgi;
		int					fd;
		void    			init_dangerousePatterns();
		int					receiveRequest(int client_socket);
		string				createResponse(void);
		string				generateHttpHeaders(Server *server, int status_code, long fileSize);
		string 				generateContentType();
		string				getStatusMessage(int status_code) const;
		File				*GetFile(string path) const;
		string  			createDirectoryListing(const string& path) const;
		File*				handleDirectory(const string& path) const;
		string  			ReturnServerErrorPage(Server *server, int status_code);
		File*				GETResponse(string root, string fullPath);
		string 				processIndexFiles(vector<string> &indexFiles) const;
		int					sendResponse();
		string 				DELETEResponse(string root, string requestedPath);
		string 				RedirectionPage(string redirectionUrl, int status_code);
		bool 				isUriBad(string uri);
		bool    			cgiInUri();
		string				handleCgi();
		string 				checkRedirectionFile(string path);
		string 				GenerateCostumeErrorPage(int status_code, string error_message);
		int 				handleTransferEncodingChunked(string &body);

		/*getters*/
		string				getRequest() const;
		string				getMethod() const;
		string				getUri() const;
		string				getHost() const;
		string				getPort() const;
		string				getConnection() const;
		size_t				getContentLength() const;
		string				getContentType() const;	
		string				getBody() const;
		string				getQuery() const;
		string				getCookie() const;
		string				getStoreFileName() const;
		string      		getFileContent() const;
		int					getSocket() const;
		void				clear();
		void				log() const;
		map<string, string> getFormFields() const;
		string 				getFileContentType() const;
		bool				responded() const;
		bool				received() const;
		string				getResponseToSend() const;
		string				getAuthorization() const;

		/*setters*/
		void				setPort(string new_port);
		void				setResponded(bool responded);
		void				setReceived(bool received);
		void				setResponseToSend(string response);

};

std::ostream& operator<<(std::ostream& os, const RequestProcessor& req);

#endif
