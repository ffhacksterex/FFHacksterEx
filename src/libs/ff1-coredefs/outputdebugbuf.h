#pragma once

#include <ostream>

template <typename TCH, typename TRAITS = std::char_traits<TCH>>
class basic_outputdebugbuf : std::basic_streambuf<TCH, TRAITS>
{
public:
	using streambuf_t = std::basic_streambuf<TCH, TRAITS>;
	using ostream_t = std::basic_ostream<TCH, TRAITS>;

	basic_outputdebugbuf(ostream_t& os)
		: m_os(os)
	{
		// Store the original streambuf, the destructor will restore it.
		// Implies that the object passed via os must outlive this object.
		m_porigbuf = m_os.rdbuf(this);
	}

	~basic_outputdebugbuf()
	{
		m_os.rdbuf(m_porigbuf);
	}

	virtual int underflow() {
		return EOF; // fail on any attempt to read from this output buffer
	}

	virtual int sync() {
		m_porigbuf->pubsync();
	}

	virtual streambuf_t* setbuf(TCH* p, std::streamsize len) {
		return m_porigbuf->pubsetbuf(p, len);
	}

	// Sends the character to output. This is where we write it to the debugger via OutputDebugString.
	virtual int overflow(int_type ch = EOF) {
		//REFACTOR - std::cerr is not buffered, but can we find a way to buffer this to reduce the
		//		number of calls to OutputDebugString() without losing any info?
		if (ch != EOF) {
			TCH sz[] = { (TCH)ch, 0 };
			OutputDebugString(sz);
		}
		return m_porigbuf->sputc((TCH)ch);
	}

private:
	ostream_t& m_os;
	streambuf_t* m_porigbuf = nullptr;
};


using outputdebugbuf = basic_outputdebugbuf<char>;
using woutputdebugbuf = basic_outputdebugbuf<wchar_t>;
