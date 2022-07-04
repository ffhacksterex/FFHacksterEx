#include "stdafx.h"
#include "assembly_types.h"
#include "string_functions.h"

using namespace Strings;

bool operator==(const asm_sourcemapping & left, const asm_sourcemapping & right)
{
	return left.type == right.type &&
		left.name == right.name &&
		left.offsets == right.offsets &&
		left.varnamesforvalues == right.varnamesforvalues;
}

bool operator!=(const asm_sourcemapping & left, const asm_sourcemapping & right)
{
	return !(left == right);
}


std::string asmsrcmapvec(const asmsourcemappingvector & v)
{
	std::ostringstream o;
	o << v;
	return o.str();
}

asmsourcemappingvector asmsrcmapvec(std::string text)
{
	asmsourcemappingvector vec;
	std::istringstream i(text);
	i >> vec;
	return vec;
}

std::ostream & operator<<(std::ostream & os, const asm_sourcemapping & srcmap)
{
	os << (int)srcmap.type << ' ';
	os << srcmap.name << ' ';
	os << srcmap.offsets << ' ';
	os << srcmap.varnamesforvalues;
	return os;
}

std::istream & operator >> (std::istream & is, asm_sourcemapping & srcmap)
{
	is >> (int&)srcmap.type;
	if (is.get() != ' ') throw std::runtime_error(std::string(typeid(srcmap).name()) + " missing space after srcmap at position " + std::to_string(is.tellg()));

	is >> srcmap.name;
	if (is.get() != ' ') throw std::runtime_error(std::string(typeid(srcmap).name()) + " missing space after srcmap at position " + std::to_string(is.tellg()));

	is >> srcmap.offsets;
	if (is.get() != ' ') throw std::runtime_error(std::string(typeid(srcmap).name()) + " missing space after srcmap at position " + std::to_string(is.tellg()));

	is >> srcmap.varnamesforvalues;
	return is;
}

std::ostream & operator << (std::ostream & os, const talkelement & elem)
{
	os << elem.marker << " ";
	os << elem.bits << " ";
	os << backquoted(elem.element) << " ";
	os << backquoted(elem.value) << " ";
	os << backquoted(elem.comment);
	return os;
}

std::istream & operator >> (std::istream & is, talkelement & elem)
{
	is >> elem.marker >> std::ws;
	is >> elem.bits >> std::ws;
	is >> backquoted(elem.element) >> std::ws;
	is >> backquoted(elem.value) >> std::ws;
	is >> backquoted(elem.comment);
	return is;
}

std::ostream & operator << (std::ostream & os, const asmsourcemappingvector & vec)
{
	return stream_out(os, vec, VECTOR_DELIMITERS);
}

std::istream & operator >> (std::istream & is, asmsourcemappingvector & vec)
{
	return stream_in(is, vec, VECTOR_DELIMITERS);
}

std::ostream & operator<<(std::ostream & os, const talkelementvector & vec)
{
	return stream_out(os, vec, VECTOR_DELIMITERS);
}

std::istream & operator >> (std::istream & is, talkelementvector & vec)
{
	return stream_in(is, vec, VECTOR_DELIMITERS);
}

std::ostream & operator<<(std::ostream & os, const talknamevectormap & cmap)
{
	return stream_out(os, cmap, MAP_DELIMITERS);
}

std::istream & operator >> (std::istream & is, talknamevectormap & cmap)
{
	return stream_in(is, cmap, MAP_DELIMITERS);
}