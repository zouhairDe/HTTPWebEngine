#include "Route.hpp"
#include "File.hpp"
#include "RequestProcessor.hpp"

string			cpp11_replace(string &str, string toReplace, string replacement);
string			trim(const string &s);
vector<string>	split(string str, char delimiter);
string			cpp11_toString(int n);

Route::Route()
	: RouteDirectoryListing(false), RouteGETMethod(false),
	  RoutePOSTMethod(false), ClientMaxBodySize(1024 * 1024)//mega default li kidir ngnix
{
	_redirectionUrl = make_pair("", -1);
}

Route::~Route() {}

vector<string> Route::getRouteIndexFiles() const
{
	return RouteIndexFiles;
}

bool Route::getRouteDirectoryListing() const
{
	return RouteDirectoryListing;
}

bool Route::getRouteGETMethod() const
{
	return RouteGETMethod;
}

bool Route::getRoutePOSTMethod() const
{
	return RoutePOSTMethod;
}

bool Route::getDELETEMethod() const
{
	return RouteDELETEMethod;
}

string Route::getUploadStore() const
{
	return UploadStore;
}

long Route::getClientMaxBodySize() const
{
	return ClientMaxBodySize;
}

string Route::getRouteName() const
{
	return RouteName;
}

Route::Route(const Route &route)
{
	*this = route;
}

pair<string, int> Route::getRedirectUrl() const
{
	return _redirectionUrl;
}

vector<pair<string, string> > Route::getCGIs() const
{
	return CGIs;
}

Route &Route::operator=(const Route &route)
{
	RouteName = route.RouteName;
	RouteIndexFiles = route.RouteIndexFiles;
	RouteDirectoryListing = route.RouteDirectoryListing;
	RouteGETMethod = route.RouteGETMethod;
	RoutePOSTMethod = route.RoutePOSTMethod;
	UploadStore = route.UploadStore;
	ClientMaxBodySize = route.ClientMaxBodySize;
	_redirectionUrl = route._redirectionUrl;
	CGIs = route.CGIs;
	return *this;
}

void Route::setProperty(const string &key, const string &value)
{
	if (key == "name")
		RouteName = value;
	else if (key == "index")
	{
		vector<string> indexes = split(value, ',');
		for (size_t i = 0; i < indexes.size(); i++)
			RouteIndexFiles.push_back(trim(indexes[i]));
	}
	else if (key == "directory_listing")
		RouteDirectoryListing = (value == "on");
	else if (key == "allowed_methods")
	{
		vector<string> methods = split(value, ',');
		for (size_t i = 0; i < methods.size(); i++)
		{
			if (methods[i] == "GET")
				RouteGETMethod = true;
			else if (methods[i] == "POST")
				RoutePOSTMethod = true;
			else if (methods[i] == "DELETE")
				RouteDELETEMethod = true;
			else
				throw runtime_error("\033[31m Invalid method: " + methods[i]);
		}
	}
	else if (key == "upload_store")
		UploadStore = value;
	else if (key == "client_max_body_size")
	{
		string size = value.substr(0, value.length() - 1);
		char unit = value[value.length() - 1];
		ClientMaxBodySize = atol(size.c_str());
		if (unit == 'M')
			ClientMaxBodySize *= 1024 * 1024;
		else if (unit == 'K')
			ClientMaxBodySize *= 1024;
	} else if (key == "return")
	{
		vector<string> parts = split(value, ',');
		if (parts.size() != 2)
			throw runtime_error("\033[31m Invalid return value: " + value + "\nExpected format: \"return <url>, <status_code>\"");
		string url = parts[0];
		int status_code = atoi(parts[1].c_str());
		if (status_code < 100 || status_code > 599)
			throw runtime_error("\033[31m Invalid status code: " + parts[1]);
		if (url.find("http://") != string::npos || url.find("https://") != string::npos)
			throw runtime_error("\033[31m Invalid URL: " + url + "\nRedirection URL should be a route e.g: /...");
		else
			_redirectionUrl = make_pair(url, status_code);

		cout << bold << red << "Redirection URL: " << _redirectionUrl.first << endl;
		cout << bold << red << "Redirection status code: " << _redirectionUrl.second << def << endl;
	}
	else if (key == "cgi_bin")
	{
		vector<string> parts = split(value, ',');
		if (parts.size() != 2)
			throw runtime_error("\033[31m Only 2 CGI access allowed");
		for (size_t i = 0; i < parts.size(); i++)
		{
			vector<string> cgi_parts = split(parts[i], ':');
			if (cgi_parts.size() != 2)
				throw runtime_error("\033[31m Invalid CGI value: " + parts[i] + "\nExpected format: \"cgi_bin <extention>:<path>\"");
			string extention = cgi_parts[0];
			string path = cgi_parts[1];
			if (extention.empty() || path.empty())
				throw runtime_error("\033[31m Invalid CGI value: " + parts[i] + "\nExpected format: \"cgi_bin <extention>:<path>\"");
			if (extention[0] != '.')
				throw runtime_error("\033[31m Invalid CGI value: " + parts[i] + "\nExpected format: \"cgi_bin <extention>:<path>\"\nExtention should start with a dot");
			
			CGIs.push_back(make_pair(extention, path));
		}
	}
	else if (key == "redirection")
	{
		vector<string> parts = split(value, ',');
		if (parts.size() != 2)
			throw runtime_error("\033[31m Invalid redirection value: " + value + "\nExpected format: \"redirection <url>, <status_code>\"");
		string url = parts[0];
		int status_code = atoi(parts[1].c_str());
		if (status_code < 100 || status_code > 599)
			throw runtime_error("\033[31m Invalid status code: " + parts[1]);
		if (url.find("http://") != string::npos || url.find("https://") != string::npos)
			throw runtime_error("\033[31m Invalid URL: " + url + "\nRedirection URL should be a route e.g: /...");
		else
			_redirectionUrl = make_pair(url, status_code);
	}
	else
	{
		throw runtime_error("\033[31m Unknown property: " + key + ",  for Route: " + RouteName);
		return ;
	}
}

void Route::CheckFiles() const
{
	struct stat buffer;
	cerr << blue << " ---------------- Checking Route: " << RouteName << def << endl;
	if (UploadStore.empty() && RouteName == "\"/uploads\"")
		throw runtime_error("\033[31m Route: " + RouteName + " must have an Upload Store");
	if (stat(string("/tmp/www/" + UploadStore).c_str(), &buffer) != 0)
		throw runtime_error("\033[31m Route: " + RouteName + " must have a valid Upload Store");
}

ostream &operator<<(ostream &out, const Route &route)
{
	out << "Route Name: " << route.getRouteName() << endl;
	out << "Index Files: ";
	vector<string> indexFiles = route.getRouteIndexFiles();
	for (size_t i = 0; i < indexFiles.size(); i++)
	{
		out << indexFiles[i];
		if (i < indexFiles.size() - 1)
			out << ", ";
	}
	out << endl;
	out << "Directory Listing: " << (route.getRouteDirectoryListing() ? "on" : "off") << endl;
	out << "Allowed Methods: ";
	if (route.getRouteGETMethod())
		out << "GET ";
	if (route.getRoutePOSTMethod())
		out << "POST";
	out << endl;
	out << "Upload Store: " << route.getUploadStore() << endl;
	out << "Client Max Body Size: " << route.getClientMaxBodySize() << endl;
	out << "Redirection URL: " << route.getRedirectUrl().first << endl;
	out << "Redirection Status Code: " << route.getRedirectUrl().second << endl;
	out << "CGIs: ";
	vector<pair<string, string> > cgis = route.getCGIs();
	for (size_t i = 0; i < cgis.size(); i++)
	{
		out << cgis[i].first << ":" << cgis[i].second;
		if (i < cgis.size() - 1)
			out << ", ";
	}
	cout << endl;
	return out;
}