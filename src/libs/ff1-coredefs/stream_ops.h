#pragma once

#include <iostream>
#include <sstream>

struct stream_tag_info {
public:
	char openhead = 0;
	char closehead = 0;
	char openbody = 0;
	char closebody = 0;
	char elemsep = 0;
	std::string tag;

	bool load(std::string separators);

private:
	bool valid = false;
};


template <typename CONTAINER>
std::ostream & stream_out(std::ostream & os, const CONTAINER & c, std::string separators)
{
	stream_tag_info strtg;
	if (!strtg.load(separators)) {
		if (separators.length() < 6) throw std::runtime_error("delimiter format is invalid for " + std::string(typeid(c).name()));
		if (strtg.tag.empty()) throw std::runtime_error("no tag specified for " + std::string(typeid(c).name()));
	}

	os << strtg.tag << strtg.openhead << c.size() << strtg.closehead << strtg.openbody;
	auto it = cbegin(c);
	auto end = cend(c);
	if (it != end) {
		write_element(os, c, it, nullptr);
		//++it;
		for (++it; it != end; ++it)
			write_element(os, c, it, &strtg.elemsep);
	}
	os << strtg.closebody;
	return os;

}

template <typename CONTAINER>
std::istream & stream_in(std::istream & is, CONTAINER & c, std::string separators)
{
	c.clear();

	stream_tag_info strtg;
	if (!strtg.load(separators)) {
		if (separators.length() < 6) throw std::runtime_error("delimiter format is invalid for " + std::string(typeid(c).name()));
		if (strtg.tag.empty()) throw std::runtime_error("no tag specified for " + std::string(typeid(c).name()));
	}

	std::string elemseparators{ strtg.elemsep, strtg.closebody, 0 };
	std::string tag;
	std::getline(is, tag, strtg.openhead);
	if (tag != strtg.tag) throw std::runtime_error("bad type tag");
	size_t count = 0;
	is >> count;
	if (is.get() != strtg.closehead)
		throw std::runtime_error("bad closing head symbol");
	if (is.get() != strtg.openbody)
		throw std::runtime_error("bad opening body symbol");

	for (size_t st = 0; st < count; ++st) {
		if (st > 0) {
			if (is.get() != strtg.elemsep)
				throw std::runtime_error("missing element separator");
			if (is.get() != ' ')
				throw std::runtime_error("missing space after element separator");
		}
		read_element(is, c, elemseparators);
	}

	if (is.get() != strtg.closebody)
		throw std::runtime_error("bad closing body symbol");
	return is;
}


template <typename CONTAINER>
std::string ToString(const CONTAINER & c)
{
	std::ostringstream o;
	o << c;
	return o.str();
}

template <typename CONTAINER>
std::string ToString(std::ostringstream & ostr, const CONTAINER & c)
{
	ostr.str("");
	ostr.seekp(0);
	ostr.clear();
	ostr << c;
	return ostr.str();
}



// GENERAL STREAMING HELPERS

std::string read_until(std::istream & is, const std::string & separators);

void init_stringstream(std::istringstream & istr, const std::string & str);
void init_stringstream(std::ostringstream & ostr);
