#pragma once
class CFFHacksterProject;
class FFH2Project;

class bad_ffhtype_exception : public std::exception
{
public:
	bad_ffhtype_exception(std::string file, int line, std::string function, exceptop op, std::string projtypename, std::string additionalmsg = "");
	virtual ~bad_ffhtype_exception();
	virtual const char* what() const;
private:
	std::string m_message;
};

class wrongprojectype_exception : public std::exception
{
public:
	wrongprojectype_exception(std::string file, int line, std::string function, CFFHacksterProject& proj, std::string expectedtype);
	wrongprojectype_exception(std::string file, int line, std::string function, FFH2Project& proj, std::string expectedtype);
	virtual ~wrongprojectype_exception();
	virtual const char* what() const;
private:
	std::string m_message;
};
