/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestProccessor.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 16:29:15 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/25 23:54:04 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPROCCESOR_HPP
# define REQUESTPROCCESOR_HPP

# include "Global.hpp"

class RequestProccessor {
	private:
		string		_request;		//ex: GET /index.html HTTP/1.1\nHost: www.example.com\n....
		string		_method;		//ex: GET
		string		_uri;			//ex: /index.html
		string		_host;			//ex: www.example.com
		string		_port;			//ex: 80
		string		_connection;	//for subject wa9ila always khas tkon keep alive mn 3ndna hna so hadi makathmch
		string		_content_length;//ex: 123
		string		_body;			//ex: name=Zouhair&age=22 or for upload file ex: file=@/path/to/file and this just for POST method
		string		_query;			//l cgi mnb3d
		string		_cookie;		//for bonus
		
	public:
		RequestProccessor();
		RequestProccessor(string request, string __port);
		~RequestProccessor();
		
		void	parseMultipartFormData(const string &body, const string &boundary);
		
		
		/*getters*/
		string		getRequest() const;
		string		getMethod() const;
		string		getUri() const;
		string		getHost() const;
		string		getPort() const;
		string		getConnection() const;
		string		getContentLength() const;
		string		getBody() const;
		string		getQuery() const;
		string		getCookie() const;
		
};

std::ostream& operator<<(std::ostream& os, const RequestProccessor& req);

#endif
