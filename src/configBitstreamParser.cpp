#include <iostream>
#include <stdexcept>
#include <string>
#include <stdint.h>
#include <strings.h>
#include <unistd.h>

#include "display.hpp"

#include "configBitstreamParser.hpp"

using namespace std;

ConfigBitstreamParser::ConfigBitstreamParser(const string &filename, int mode,
			bool verbose): _filename(filename), _bit_length(0),
			_file_size(0), _verbose(verbose),
			_bit_data(), _raw_data(), _hdr()
{
	if (!filename.empty()) {
		ifstream _fd(filename, ifstream::in | (ios_base::openmode)mode);
		if (!_fd.is_open())
			throw std::runtime_error("Error: fail to open " + _filename);

		_fd.seekg(0, _fd.end);
		_file_size = _fd.tellg();
		_fd.seekg(0, _fd.beg);

		_raw_data.resize(_file_size);
		_bit_data.reserve(_file_size);

		_fd.read((char *)&_raw_data[0], sizeof(char) * _file_size);
		if (_fd.gcount() != _file_size)
			throw std::runtime_error("Error: fail to read " + _filename);
		_fd.close();
	} else if (!isatty(fileno(stdin))) {
		_file_size = 0;
		string tmp;
		tmp.resize(4096);
		size_t size;

		do {
			cin.read((char *)&tmp[0], 4096);
			size = cin.gcount();
			_raw_data.append(tmp, 0, size);
			_file_size += size;
		} while (size > 0);
	} else {
		throw std::runtime_error("Error: fail to parse. No filename or pipe\n");
	}
}

ConfigBitstreamParser::~ConfigBitstreamParser()
{
}

string ConfigBitstreamParser::getHeaderVal(string key)
{
	auto val = _hdr.find(key);
	if (val == _hdr.end())
		throw std::runtime_error("Error key " + key + " not found");
	return val->second;
}

void ConfigBitstreamParser::displayHeader()
{
	if (_hdr.empty())
		return;
	cout << "bitstream header infos" << endl;
	for (auto it = _hdr.begin(); it != _hdr.end(); it++) {
		printInfo((*it).first + ": ", false);
		printSuccess((*it).second);
	}
}

uint8_t ConfigBitstreamParser::reverseByte(uint8_t src)
{
	uint8_t dst = 0;
	for (int i=0; i < 8; i++) {
		dst = (dst << 1) | (src & 0x01);
		src >>= 1;
	}
	return dst;
}
