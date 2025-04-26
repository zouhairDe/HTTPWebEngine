#ifndef CGI_HPP
# define CGI_HPP

# include "Global.hpp"

class Server;

class CGI {
    private:
        string          _cgiPath;
        ifstream        _inputStream;
        ofstream        _outputStream;
        vector<char*>   _envVars;
        string          _cgiOutputPath;
        string          _cgiInputPath;
        string          _cgiOutput;
        string          _bodyData;
    public:
        CGI();
        ~CGI();
        vector<char*>   getEnvVars() const;
        string          getCgiPath() const;
        string          getCgiOutput() const;
        bool            openInputStream();
        bool            openOutputStream();
        ifstream&       getInputStream();
        ofstream&       getOutputStream();
        void            setCgiOutput(const string &cgiOutput);
        void            setCgiPath(const string &cgiPath);
        void            setEnvVars(vector<char *> &envVars);
        void            addEnvVar(char* envVar);
        void            setBodyData(const string &bodyData);
        string          getBodyData() const;

        void            clean();
        bool            execute();
};

#endif