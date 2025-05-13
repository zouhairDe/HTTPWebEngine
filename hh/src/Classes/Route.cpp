#include "Route.hpp"
#include "File.hpp"
#include "RequestProcessor.hpp"

string			cpp11_replace(string &str, string toReplace, string replacement);
string			trim(const string &s);
vector<string>	split(string str, char delimiter);
string			cpp11_toString(int n);

Route::Route()
	: RouteDirectoryListing(false), RouteGETMethod(false),
	  RoutePOSTMethod(false), ClientMaxBodySize(0)
{
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

Route &Route::operator=(const Route &route)
{
	RouteName = route.RouteName;
	RouteIndexFiles = route.RouteIndexFiles;
	RouteDirectoryListing = route.RouteDirectoryListing;
	RouteGETMethod = route.RouteGETMethod;
	RoutePOSTMethod = route.RoutePOSTMethod;
	UploadStore = route.UploadStore;
	ClientMaxBodySize = route.ClientMaxBodySize;
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
		RouteGETMethod = (value.find("GET") != string::npos);
		RoutePOSTMethod = (value.find("POST") != string::npos);
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
	}
	else
	{
		throw runtime_error("\033[31m Unknown property: " + key + ",  for Route: " + RouteName);
		exit(1);
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
	return out;
}