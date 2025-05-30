#include "File.hpp"
#include <fstream>
#include <cstring>

File::File(string path) : _path(path), _size(0), _data(nullptr) {
    ifstream file(this->_path.c_str(), ios::binary | ios::ate);
    if (!file) {
        cerr << "Failed to get file size.\n";
        return ;
    }

    this->_size = file.tellg();
	file.seekg(0, ios::beg);

	_data = new char[this->_size + 1];
	if (!file) {
		cerr << "Failed to open the file.\n";
		return ;
	}
	file.read(this->_data, this->_size);
	this->_data[this->_size] = '\0';

}

File::File() {
	this->_path = "";
	this->_size = 0;
	this->_data = nullptr;

}

File::File(const File &other) {
	this->_path = other._path;
	this->_size = other._size;
	this->_data = new char[this->_size + 1];

	memcpy(this->_data, other._data, this->_size);
	this->_data[this->_size] = '\0';
}

File	&File::operator=(const File &other) {
	if (this != &other) {
		this->_path = other._path;
		this->_size = other._size;
		delete[] this->_data;
		this->_data = new char[this->_size + 1];
		memcpy(this->_data, other._data, this->_size);
		this->_data[this->_size] = '\0';
	}
	return (*this);
}

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

string	File::getPath() const {
	return (this->_path);
}

void	File::setData(string Data, size_t size) {
	if (this->_data != nullptr)
		delete[] this->_data;
	this->_data = new char[size + 1];
	memcpy(this->_data, Data.c_str(), size);
	this->_data[size] = '\0';
	this->_size = size;
}

ostream &operator<<(ostream &os, const File &file) {
	os << "File path: " << file.getPath() << endl;
	os << "File size: " << file.getSize() << endl;
	os << "File data: " << file.getData() << endl;
	return (os);
}