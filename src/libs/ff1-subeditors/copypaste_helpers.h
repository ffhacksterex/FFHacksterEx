#pragma once

#include <vector>

#define ID_FFH_NOCOPYSWAP  0
#define ID_FFH_COPY        ID_EDIT_COPY
#define ID_FFH_PASTE       ID_EDIT_PASTE
#define ID_FFH_SWAP        ID_EDIT_REPLACE

struct PasteTarget
{
	CString caption;
	bool checked;
	CString warning;
};

namespace copypaste_helpers
{
	CString BuildCopyPasteText(int flag, CString srcname, CString destname);

	void CopySwapBytes(bool swap, std::vector<unsigned char>& rom, int srcindex, int dstindex,
		int baseoffset, int recwidth, int start, std::vector<bool> flags);
	void CopySwapBuffer(bool swap, std::vector<unsigned char>& rom, int srcindex, int dstindex,
		int baseoffset, int recwidth, int start, int bytecount);
	void CopySwapItemPermissions(bool swap, std::vector<unsigned char>& rom, size_t srcindex, size_t destindex,
		size_t baseoffset, size_t bits, size_t start, size_t count);
	void CopySwapSpellPermissions(bool swap, std::vector<unsigned char>& rom, size_t srcindex, size_t destindex,
		size_t baseoffset, size_t bits, int recwidth, size_t start, size_t count);

	struct sCopySwapResult
	{
		int selcmd = ID_FFH_NOCOPYSWAP;
		int thisindex = -1;
		int copyindex = -1;
		std::vector<bool> flags;
		CString message;
	};

	sCopySwapResult InvokeCopySwap(CListBox& wnd, CPoint point, int selitem, std::vector<CString> optionnames);
	sCopySwapResult InvokeCopySwap(CListBox& wnd, CPoint point, int selitem, std::vector<PasteTarget> options);
};
