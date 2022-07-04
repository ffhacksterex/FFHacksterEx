#pragma once

#include "logging_functions.h"
#include <string>


enum class initflag { dontread, read };

template <typename T>
T* iif(T* pif, T* pifnot) {
	return pif != nullptr ? pif : pifnot;
}

inline CString iif(CString cif, CString cifnot) {
	return !cif.IsEmpty() ? cif : cifnot;
}

inline std::string & iif(std::string & sif, std::string & sifnot)
{
	return !sif.empty() ? sif : sifnot;
}

inline std::string iif(std::string sif, std::string sifnot)
{
	return !sif.empty() ? sif : sifnot;
}

namespace UiTemp { //REFACTOR - move prompts out of these coredefs functions into ff1-utils
	constexpr UINT COMMON_OFN = OFN_NOCHANGEDIR | OFN_DONTADDTORECENT;
}
