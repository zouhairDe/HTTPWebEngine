# include "CGI.hpp"
# include "Functions.hpp"

CGI::CGI() {
    srand(time(0));
    _cgiInputPath = "/tmp/.cgi_input_" + cpp11_toString(rand()) + ".tmp";
    _cgiOutputPath = "/tmp/.cgi_output_" + cpp11_toString(rand()) + ".tmp";
}

CGI::~CGI() {
    _inputStream.close();
    _outputStream.close();
    unlink(_cgiInputPath.c_str());
    unlink(_cgiOutputPath.c_str());
}

CGI& CGI::operator=(const CGI &cgi) {
    if (this != &cgi) {
        _cgiPath = cgi._cgiPath;
        // _inputStream = cgi._inputStream;
        // _outputStream = cgi._outputStream;
        _envVars = cgi._envVars;
        _cgiOutputPath = cgi._cgiOutputPath;
        _cgiInputPath = cgi._cgiInputPath;
        _cgiOutput = cgi._cgiOutput;
        _bodyData = cgi._bodyData;
    }
    return *this;
}

CGI::CGI(const CGI &cgi) {
    *this = cgi;
}

void CGI::setUri(string uri) {
    _uri = uri;
}

void CGI::setCgiPath(const string &cgiPath) {
    _cgiPath = cgiPath;
}

string CGI::getCgiPath() const {
    return _cgiPath;
}

bool CGI::openInputStream() {
    std::ofstream tempStream(_cgiInputPath.c_str(), std::ios::out | std::ios::trunc);
    if (!tempStream.is_open()) {
        std::cerr << "Failed to create CGI input file" << std::endl;
        return false;
    }
    if (!_bodyData.empty()) {
        tempStream << _bodyData;
    }
    tempStream.close();
    _inputStream.open(_cgiInputPath.c_str(), std::ios::in);
    if (!_inputStream.is_open()) {
        std::cerr << "Failed to open CGI input file for reading" << std::endl;
        return false;
    }
    return true;
}

bool CGI::openOutputStream() {
    _outputStream.open(_cgiOutputPath.c_str(), std::ios::out | std::ios::trunc);
    if (!_outputStream.is_open()) {
        std::cerr << "Failed to open CGI output file" << std::endl;
        return false;
    }
    return true;
}

ifstream& CGI::getInputStream() {
    return _inputStream;
}

ofstream& CGI::getOutputStream() {
    return _outputStream;
}

string CGI::getCgiOutput() const {
    return _cgiOutput;
}

vector<char*> CGI::getEnvVars() const {
    return _envVars;
}

void CGI::setCgiOutput(const string &cgiOutput) {
    _cgiOutput = cgiOutput;
}

void CGI::setEnvVars(vector<char*> &envVars) {
    _envVars = envVars;
}

void CGI::addEnvVar(char* envVar) {
    _envVars.push_back(envVar);
}


void CGI::clean() {
    _envVars.clear();
    _inputStream.close();
    _outputStream.close();
    unlink(_cgiInputPath.c_str());
    unlink(_cgiOutputPath.c_str());
}

void CGI::setBodyData(const string &bodyData) {
    _bodyData = bodyData;
}

string CGI::getBodyData() const {
    return _bodyData;
}

bool CGI::execute() {
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Fork failed" << std::endl;
        return false;
    } else if (pid == 0) { // Child process
        // Redirect input and output
        int inputFd = open(_cgiInputPath.c_str(), O_RDONLY);
        if (inputFd == -1) {
            std::cerr << "Failed to open input file in child process: " << std::endl;
            exit(1);
        }
        
        int outputFd = open(_cgiOutputPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (outputFd == -1) {
            std::cerr << "Failed to open output file in child process: " << std::endl;
            close(inputFd);
            exit(1);
        }
        
        if (dup2(inputFd, STDIN_FILENO) == -1) {
            std::cerr << "Failed to duplicate stdin: " << std::endl;
            close(inputFd);
            close(outputFd);
            exit(1);
        }
        
        if (dup2(outputFd, STDOUT_FILENO) == -1) {
            std::cerr << "Failed to duplicate stdout: " << std::endl;
            close(inputFd);
            close(outputFd);
            exit(1);
        }
        
        close(inputFd);
        close(outputFd);

        // Execute the CGI script
        char* const* envp = _envVars.data();
        if (_uri.find(".js") != std::string::npos) {
            const char* nodePath = "/usr/bin/node";
            char* const argv[] = {(char*)"node", const_cast<char*>(_cgiPath.c_str()), NULL};
            execve(nodePath, argv, envp);
        } else{
            char* const argv[] = {const_cast<char*>(_cgiPath.c_str()), NULL};
            execve(_cgiPath.c_str(), argv, envp);
        }
        
        std::cerr << "Execve failed: " << std::endl;
        exit(1);
    } else { // Parent process
        time_t start = time(NULL);
        int status = 0;
        while (true) {
            status = waitpid(pid, &status, WNOHANG);
            if (status == -1) {
                std::cerr << "Waitpid failed: " << std::endl;
                return false;
            }
            if (status == 0) {
                if (difftime(time(NULL), start) >= CGI_TIMEOUT) {
                    kill(pid, SIGKILL);
                    std::cerr << "CGI script timed out" << std::endl;
                    return false;
                }
            } else {
                break;
            }
        }
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 0) {
                std::cerr << "CGI script exited with status: " << WEXITSTATUS(status) << std::endl;
                return false;
            }
        } else {
            std::cerr << "CGI script did not exit normally" << std::endl;
            return false;
        }
        
        // Read the output from the CGI script
        _inputStream.close(); // Close if still open
        _inputStream.open(_cgiOutputPath.c_str(), std::ios::in);
        if (!_inputStream.is_open()) {
            std::cerr << "Failed to open CGI output file for reading" << std::endl;
            return false;
        }
        
        std::string res;
        std::string line;
        while (std::getline(_inputStream, line)) {
            res += line + "\n";
        }
        
        _inputStream.close();
        _cgiOutput = res;
        return true;
    }
}