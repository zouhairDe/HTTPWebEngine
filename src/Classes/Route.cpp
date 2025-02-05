
#include "Route.hpp"

Route::Route()
	: RouteDirectoryListing(false), RouteGETMethod(false), 
	RoutePOSTMethod(false), ClientMaxBodySize(0)
{ }

Route::~Route() { }
			
string Route::getRouteRoot() const {
	return RouteRoot;
}

string Route::getRouteIndex() const {
	return RouteIndex;
}

bool Route::getRouteDirectoryListing() const {
	return RouteDirectoryListing;
}

bool Route::getRouteGETMethod() const {
	return RouteGETMethod;
}

bool Route::getRoutePOSTMethod() const {
	return RoutePOSTMethod;
}

string Route::getUploadStore() const {
	return UploadStore;
}

long Route::getClientMaxBodySize() const {
	return ClientMaxBodySize;
}

string Route::getRouteName() const {
	return RouteName;
}

// ma3rftch ila chi rouote ma3ndouch root/ wach n3tiwh default ("/var/www/") wla la

void	Route::setProperty(const string& key, const string& value) {
	if (key == "root") RouteRoot = value;
	else if (key == "name") RouteName = value;
	else if (key == "index") RouteIndex = value;
	else if (key == "directory_listing") 
		RouteDirectoryListing = (value == "on");
	else if (key == "allowed_methods") {
		RouteGETMethod = (value.find("GET") != string::npos);
		RoutePOSTMethod = (value.find("POST") != string::npos);
	}
	else if (key == "upload_store") UploadStore = value;
	else if (key == "client_max_body_size") {
		string size = value.substr(0, value.length()-1);
		char unit = value[value.length()-1];
		ClientMaxBodySize = atol(size.c_str());
		if (unit == 'M') ClientMaxBodySize *= 1024 * 1024;
		else if (unit == 'K') ClientMaxBodySize *= 1024;
	}
}
