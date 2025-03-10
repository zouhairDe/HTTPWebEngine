#include "Route.hpp"
#include "File.hpp"
#include "RequestProccessor.hpp"

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

File* Route::handleFile(const string& path) const {
	cout << "In HandleFile" << endl;
    if (access(path.c_str(), F_OK) == -1 || access(path.c_str(), R_OK) == -1)
        return NULL;
	else
    	return new File(path);
}

string Route::createDirectoryListing(const string& path) const {
	cout << "In Directory Listing" << endl;
    string htmlContent = "<html><head><title>Directory Listing</title></head><body><h1>Directory Listing</h1><ul>";
    DIR* dir = opendir(path.c_str());
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            string name = entry->d_name;
            if (name != "." && name != "..") {
                htmlContent += "<li><a href=\"";
                htmlContent += name;
                htmlContent += "\">";
                htmlContent += name;
                htmlContent += "</a></li>";
            }
        }
        closedir(dir);
    }
    htmlContent += "</ul></body></html>";
    return htmlContent;
}

File* Route::handleDirectory(const string& path) const {
	cout << "In HandleDirectory" << endl;
    if (!RouteDirectoryListing)
        return NULL;

    string htmlContent = createDirectoryListing(path);
    string tempPath = "/tmp/dirlist_" + cpp11_toString(time(NULL)) + ".html";
    ofstream tempFile(tempPath.c_str());
    if (!tempFile.is_open())
        return NULL;
    
    tempFile << htmlContent;
    tempFile.close();
    return new File(tempPath);
}

File* Route::getGETResponse(RequestProccessor req, string root, string fullPath) const {
	cout << "In GetGETResponse" << endl;
	(void)req;
	(void)root;
    string routeName = RouteName;
    // string originalPath = "/tmp/www/" + root;// + cpp11_replace(routeName, "\"", "").substr(1);
    // string fullPath = originalPath;
    
    // if (!fileFromUri.empty())
    //     fullPath += "/" + fileFromUri;

	cout << "Full Path: " << fullPath << endl;
    struct stat pathStat;
    if (stat(fullPath.c_str(), &pathStat) == -1)
        return NULL;

    // Check if it's a directory
    if (S_ISDIR(pathStat.st_mode)) {
        return handleDirectory(fullPath);
    }
    
    // Handle as regular file
    return handleFile(fullPath);
}