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

namespace ffh
{
	namespace uti
	{
		class bounds_error : std::exception
		{
		public:
			bounds_error(size_t index, size_t bounds);
			virtual ~bounds_error();
			virtual const char* what() const;
		private:
			size_t m_index = (size_t)-1;
			size_t m_bounds = (size_t)-1;
			std::string m_message;
		};
	}
}
