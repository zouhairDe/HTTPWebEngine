
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
	file.read(this->_data, this->_size);//hm manprotectiwch hna  azeggaf? if (read fails...)
	this->_data[this->_size] = '\0';
}

File::File() {}//huh?

File::File(const File &other) {
	this->_path = other._path;
	this->_size = other._size;
	this->_data = new char[this->_size + 1];
	/* THIS FUNCTION IS (MAYBE) NOT ALLOWED */
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

// void	File::setOffset(size_t offset) {
// 	this->_offset = offset;
// }

// size_t	File::getOffset() const {
// 	return (this->_offset);
// }

void	File::trimOldData(size_t pos)
{
	char	*newData = new char[this->_size - pos + 1];
	memcpy(newData, this->_data + pos, this->_size - pos);
	newData[this->_size - pos] = '\0';
	delete[] this->_data;
	this->_data = newData;
	this->_size -= pos;
	return ;
}

void	File::clear() {
	delete[] this->_data;
	this->_data = nullptr;
}