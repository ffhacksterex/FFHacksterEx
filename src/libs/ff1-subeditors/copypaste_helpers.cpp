#include "stdafx.h"
#include "copypaste_helpers.h"
#include <ui_helpers.h>
#include "DlgPasteTargets.h"
#include <algorithm>

namespace copypaste_helpers
{

	CString BuildCopyPasteText(int flag, CString srcname, CString destname)
	{
		CString cs;
		switch (flag) {
		case ID_FFH_COPY:
			cs.Format("&Copy %s", (LPCSTR)srcname);
			break;
		case ID_FFH_PASTE:
			cs.Format("Paste from %s to %s", (LPCSTR)srcname, (LPCSTR)destname);
			break;
		case ID_FFH_SWAP:
			cs.Format("Swap %s with %s", (LPCSTR)srcname, (LPCSTR)destname);
			break;
		default:
			cs.Format("Unknown operation '%d'", flag);
			throw std::runtime_error((LPCSTR)cs);
			break;
		}
		return cs;
	}

	void CopySwapBytes(bool swap, std::vector<unsigned char>& rom, int srcindex, int dstindex,
		int baseoffset, int recwidth, int start, std::vector<bool> flags)
	{
		int srcoffset = baseoffset + (srcindex * recwidth);
		int destoffset = baseoffset + (dstindex * recwidth);
		size_t bytecount = flags.size();
		for (size_t i = start, end = start + bytecount; i < end; ++i) {
			if (!flags[i])
				continue;

			auto src = srcoffset + i;
			auto dst = destoffset + i;
			if (swap) {
				auto temp = rom[dst];
				rom[dst] = rom[src];
				rom[src] = temp;
			}
			else {
				rom[dst] = rom[src];
			}
		}
	}

	void CopySwapBuffer(bool swap, std::vector<unsigned char>& rom, int srcindex, int dstindex,
		int baseoffset, int recwidth, int start, int bytecount)
	{
		int srcoffset = baseoffset + (srcindex * recwidth);
		int destoffset = baseoffset + (dstindex * recwidth);
		for (int i = start, end = start + bytecount; i < end; ++i) {
			auto src = srcoffset + i;
			auto dst = destoffset + i;
			if (swap) {
				auto temp = rom[dst];
				rom[dst] = rom[src];
				rom[src] = temp;
			}
			else {
				rom[dst] = rom[src];
			}
		}
	}

	void CopySwapPermissions(bool swap, std::vector<unsigned char>& rom, size_t srcindex, size_t destindex,
		size_t baseoffset, size_t bits, size_t start, size_t count)
	{
		const unsigned short valuemask = (1 << bits) - 1;
		const unsigned short srcmask = (1 << ((bits - 1) - srcindex));
		const unsigned short dstmask = (1 << ((bits - 1) - destindex));
		const size_t step = bits > 8 ? 2 : 1; // handles lists of BYTEs and WORDs

		for (size_t i = start, end = start + count; i < end; ++i) {
			size_t offset = baseoffset + (i * step);
			unsigned short origoctet = rom[offset];
			unsigned short octet = rom[offset] + (rom[offset + 1] << 8);

			// I invert the byte to make it a little more intuitive.
			// So now, the class can use the item if the bit is SET.
			auto invoct = (unsigned short)(~octet & valuemask);

			// Set dest if source is set, clear it otherwise
			bool srcon = (octet & srcmask) != srcmask;
			unsigned short newdst = srcon ? dstmask : 0;

			if (swap) {
				// We're swapping, so get the bit states,
				// then swap them and OR the swapped states back into invoct.
				bool dston = (octet & dstmask) != dstmask;
				unsigned short newsrc = dston ? srcmask : 0;
				invoct &= ~(srcmask | dstmask);
				invoct |= (newsrc | newdst);
			}
			else {
				// We're copying, so if the source bit was set,
				// turn on the dest mask bit; otherwise turn it off.
				invoct &= ~dstmask;
				invoct |= newdst;
			}

			// Reverse the inversion and store the change
			auto newoctet = (unsigned short)(~invoct & valuemask);
			rom[offset] = newoctet & 0xFF;
			if (bits > 8) rom[offset + 1] = (newoctet >> 8) & 0xFF;
		}
	}

	sCopySwapResult InvokeCopySwap(CListBox& wnd, CPoint point, int selitem, std::vector<CString> optionnames)
	{
		std::vector<PasteTarget> options(optionnames.size());
		std::transform(cbegin(optionnames), cend(optionnames), begin(options), [](auto name) -> PasteTarget { return {name,false}; });
		return InvokeCopySwap(wnd, point, selitem, options);
	}

	sCopySwapResult InvokeCopySwap(CListBox& wnd, CPoint point, int selitem, std::vector<PasteTarget> options)
	{
		sCopySwapResult result;
		auto thisitem = Ui::ItemFromPoint(wnd, point);
		if (thisitem == -1) return result;

		result.thisindex = thisitem;

		CString thisname = Ui::GetItemText(wnd, thisitem);
		CMenu menu;
		VERIFY(menu.CreatePopupMenu());
		menu.AppendMenu(MF_BYCOMMAND, ID_FFH_COPY, copypaste_helpers::BuildCopyPasteText(ID_FFH_COPY, thisname, ""));
		if (selitem != -1 && selitem != thisitem) {
			CString selname = Ui::GetItemText(wnd, selitem);
			menu.AppendMenu(MF_BYCOMMAND, ID_FFH_PASTE, copypaste_helpers::BuildCopyPasteText(ID_FFH_PASTE, selname, thisname));
			menu.AppendMenu(MF_BYCOMMAND, ID_FFH_SWAP, copypaste_helpers::BuildCopyPasteText(ID_FFH_SWAP, selname, thisname));
		}
		auto selcmd = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, point.x, point.y, &wnd);
		switch (selcmd) {
		case ID_FFH_COPY:
			result.selcmd = selcmd;
			result.copyindex = thisitem;
			break;
		case ID_FFH_PASTE:
		case ID_FFH_SWAP:
		{
			CDlgPasteTargets dlgtargets(wnd.GetParent());
			menu.GetMenuString(selcmd, dlgtargets.Title, MF_BYCOMMAND);
			dlgtargets.SetTargets(options);
			if (dlgtargets.DoModal() == IDOK) {
				if (dlgtargets.IsAnyChecked()) {
					result.selcmd = selcmd;
					result.flags = dlgtargets.GetResults();
				}
				else {
					result.message = "You must select at least one item for paste/swap to have an effect.";
				}
			}
			break;
		}
		}
		return result;
	}

} // end namespace