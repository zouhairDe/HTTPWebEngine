
#include "File.hpp"

File::File(std::string path) : _path(path), _size(0), _data(nullptr) {
    std::ifstream file(this->_path, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Failed to get file size.\n";
        return ;
    }

    this->_size = file.tellg();
	file.seekg(0, std::ios::beg);

	_data = new char[this->_size + 1];
	if (!file) {
		std::cerr << "Failed to open the file.\n";
		return ;
	}
	file.read(this->_data, this->_size);
}

File::File() {}//huh?

File::~File() {
	delete[] this->_data;
}

bool	File::exists() const {
	return (this->_data != nullptr);
}

size_t	File::getSize() const {
	return (this->_size);
}

char	*File::getData() const {
	return (this->_data);
}

std::string	File::getPath() const {
	return (this->_path);
}
