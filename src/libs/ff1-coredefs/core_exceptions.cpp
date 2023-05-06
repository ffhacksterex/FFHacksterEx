#include "stdafx.h"
#include "core_exceptions.h"
#include "FFHacksterProject.h"
#include "FFH2Project.h"
#include "std_assembly.h"

// ################################################################
// in FFHacksterProject.h

// bad_ffhtype_exception

bad_ffhtype_exception::bad_ffhtype_exception(std::string file, int line, std::string function, exceptop op, std::string projtypename, std::string additionalmsg)
{
	TRACEEXCEPTIONPOINT(*this, file, line, function);

	m_message = "Unknown project type " + projtypename + " does NOT support " + get_exceptop_text(op);
	if (!additionalmsg.empty()) m_message += ", " + additionalmsg;

	LogMsg << m_message << std::endl;
}

bad_ffhtype_exception::~bad_ffhtype_exception()
{
}

const char* bad_ffhtype_exception::what() const
{ 
	return m_message.c_str();
}


// wrongprojectype_exception

wrongprojectype_exception::wrongprojectype_exception(std::string file, int line, std::string function, CFFHacksterProject & proj, std::string expectedtype)
{
	TRACEEXCEPTIONPOINT(*this, file, line, function);

	m_message = "Wrong project type: expected " + expectedtype + ", but the project is " + std::string((LPCSTR)proj.ProjectTypeName);

	LogMsg << m_message << std::endl;
}

wrongprojectype_exception::wrongprojectype_exception(std::string file, int line, std::string function, FFH2Project& proj, std::string expectedtype)
{
	TRACEEXCEPTIONPOINT(*this, file, line, function);

	m_message = "Wrong project type: expected " + expectedtype + ", but the project is " + proj.info.type;

	LogMsg << m_message << std::endl;
}

wrongprojectype_exception::~wrongprojectype_exception()
{
}

const char* wrongprojectype_exception::what() const
{
	return m_message.c_str();
}



// ################################################################
// in assembly_functions.h

// Assembly exceptions

asminline_unusedmappingexception::asminline_unusedmappingexception(std::string file, int line, std::string function, exceptop op, const asmsourcemappingvector & mappings, const uintvector & hits)
	: m_matchcount(0)
	, m_total(hits.size())
{
	TRACEEXCEPTIONPOINT(*this, file, line, function);

	auto failcount = count_if(cbegin(hits), cend(hits), [](auto hitcount) { return hitcount == 0; });
	auto strexcept = get_exceptop_text(op);
	CString msg;
	msg.Format("failed while %s to find a match for %lu of the %lu mapping(s):",
		strexcept.c_str(), failcount, m_total);
	for (size_t st = 0; st < hits.size(); ++st) {
		if (hits[st] == 0) msg.AppendFormat("\n - %s (type %ld)", mappings[st].name.c_str(), mappings[st].type);
	}
	m_message = (LPCSTR)msg;
	m_total = hits.size();
	m_matchcount = m_total - failcount;

	LogMsg << m_message << std::endl;
}

asminline_unusedmappingexception::~asminline_unusedmappingexception()
{
}

const char* asminline_unusedmappingexception::what() const
{ 
	return m_message.c_str();
}