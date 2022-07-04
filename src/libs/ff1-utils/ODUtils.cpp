#include "stdafx.h"
#include "ODUtils.h"
#include "BackgroundMixin.h"
#include "ui_helpers.h"

using namespace Ui;

namespace ODUtils
{

	bool HasAllStyles(UINT objectstyles, UINT checkfor) //REFACTOR - move to coredefs, maybe create a Flags class to generalize it?
	{
		return (objectstyles & checkfor) == checkfor;
	}

	bool HasAnyStyles(UINT objectstyles, UINT checkfor)
	{
		return (objectstyles & checkfor) != 0;
	}

	bool HasAtLeastOneOf(UINT flags, std::initializer_list<UINT> flagstocheck)
	{
		for (auto fcheck : flagstocheck) {
			if ((flags & fcheck) == fcheck)
				return true;
		}
		return false;
	}

	void UpdateParentRect(CWnd * pwnd)
	{
		if (pwnd == nullptr) return;

		auto parent = pwnd->GetParent();
		if (parent == nullptr) return;

		CRect rect;
		pwnd->GetWindowRect(&rect);
		parent->ScreenToClient(&rect);
		parent->InvalidateRect(&rect);
	}

	POINT SetBrushOrigin(CDC & dc, CWnd * pwnd)
	{
		CRect rc;
		pwnd->GetWindowRect(&rc);
		pwnd->GetParent()->ScreenToClient(&rc);

		pwnd->GetClientRect(&rc);

		auto old = dc.SetBrushOrg(-rc.left, -rc.top);
		return old;
	}

	// Return he brush to use, or a non-NULL brush if the parent's handler should not be called.
	HBRUSH GetODCtlColorBrush(CWnd * pwnd, CDC * pDC)
	{
		ASSERT(pwnd != nullptr);
		ASSERT(pDC != nullptr);

		auto bgmixin = dynamic_cast<CBackgroundMixin*>(pwnd->GetParent()); //REFACTOR - if this works, wrap it into an OD helper function
		if (bgmixin != nullptr) {
			auto exstyle = pwnd->GetExStyle();
			bool transparent = (exstyle & WS_EX_TRANSPARENT) != 0;
			auto transbrush = (HBRUSH)(transparent ? bgmixin->GetBackgroundBrush() : NULL);
			if (transbrush) {
				CRect rcwin;
				ClientToParentRect(pwnd, &rcwin);
				pDC->SetBrushOrg(-rcwin.left, -rcwin.top);
				pDC->SetBkMode(TRANSPARENT);
				return transbrush;
			}
		}
		
		return NULL;
	}

}