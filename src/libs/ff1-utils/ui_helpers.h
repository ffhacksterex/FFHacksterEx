
#pragma once

#include <afxwin.h>
#include <atlimage.h>
#include <vector>
#include "datanode.h"
#include "StrikeCheck.h"
#include "InplaceEdit.h"
#include "InplaceCombo.h"
#include "InplaceCheck.h"
#include <function_types.h>
#include <rc_functions.h>

namespace Ui
{

	class RedrawScope //REFACTOR - MOVE TO ITS OWN H/CPP FILES, KEEP IN THE Ui NAMESPACE
	{
	public:
		RedrawScope(CWnd * pwnd);
		~RedrawScope();
	private:
		CWnd* m_pwnd;
	};


	class ui_exception : public std::exception //REFACTOR - MOVE TO ITS OWN H/CPP FILES, KEEP IN THE Ui NAMESPACE
	{
	public:
		ui_exception(const char* file, int line, const char* function, std::string message);
		virtual ~ui_exception();
		virtual const char* what() const override;
	private:
		std::string m_message;
	};


	// ################################################################
	// Methods

	const CRect  & GetMouseOverImgRect();
	CImage & GetMouseOverSelImg();
	CImage & GetMouseOverHotImg();

	CFont & GetSubBannerFont();

	CWnd* GetControlByName(CWnd* parent, const Rc::dialogmappings & dlgmap, CString ctlname);

	void SetCheckValue(CButton & check, bool set); 
	bool GetCheckValue(CButton & check);
	void SetCheckFlags(int flags, std::vector<CButton*> vchecks);
	int GetCheckFlags(std::vector<CButton*> vchecks);

	void SetCheckValue(CStrikeCheck & check, bool set);
	bool GetCheckValue(CStrikeCheck & check);

	void SetEditValue(CEdit & edit, LPCSTR newvalue);
	void SetEditValue(CEdit & edit, std::string newvalue);
	CString GetEditValue(CEdit & edit);

	CString GetStaticText(CStatic & stat);
	void SetStaticText(CStatic & stat, CString text);

	void SetCheckFlags(int flags, std::vector<CStrikeCheck*> vchecks);
	int GetCheckFlags(std::vector<CStrikeCheck*> vchecks);

	void OffsetWindow(CWnd & wnd, int dx, int dy);
	void InflateWindowRect(CWnd & wnd, int cx, int cy);

	void ShowWindow(CWnd & wnd, bool show);
	void EnableWindow(CWnd & wnd, bool enabled);

	CString GetControlText(CWnd & wnd);
	int GetControlInt(CWnd& wnd, int radix = 10);
	void SetControlInt(CWnd& wnd, int value, int radix = 10);

	void AddEntry(CComboBox & box, CString string, DWORD_PTR data);
	void AddEntry(CListBox & box, CString string, DWORD_PTR data);
	void InsertEntry(CComboBox & box, int index,  CString string, DWORD_PTR data);
	void InsertEntry(CListBox & box, int index, CString string, DWORD_PTR data);

	int FindIndexByData(CComboBox & box, DWORD_PTR data);
	int FindIndexByPrefix(CComboBox & box, int matchvalue);
	DWORD_PTR GetSelectedItemData(CComboBox & box, DWORD_PTR defvalue = -1);
	void RetrieveSelectedItemData(CComboBox & box, DWORD_PTR & value);

	void SelectFirstItem(CComboBox & box);
	bool SelectItemByData(CComboBox & box, DWORD_PTR data);
	DWORD_PTR GetSelectedItemData(CListBox & box, DWORD_PTR defvalue = -1);

	int LoadCombo(CComboBox & combo, dataintnodevector dvec);
	int LoadListBox(CListBox & list, dataintnodevector dvec);
	int LoadCaptions(std::vector<CWnd*> checks, dataintnodevector dvec);

	BOOL SetTrackMouseEvent(CWnd* pwnd, UINT eventtype);
	void HandleLbuttonDrag(CWnd* pwnd);
	void ClientToParentRect(CWnd* pwnd, RECT * prc);
	void WindowToParentRect(CWnd* pwnd, RECT * prc);
	void GetControlRect(CWnd * pcontrol, RECT * prc);
	CRect GetControlRect(CWnd * pcontrol);
	CRect GetClientRect(CWnd * pcontrol);
	CRect GetNonClientRect(CWnd& wnd);
	CRect GetWindowRect(CWnd * pwnd);

	CRect GetSubitemRect(CListCtrl & list, int item, int subitem);
	void RenumberList(CListCtrl & list, int subitem, int base, int startat);

	void AutosizeAllColumns(CListCtrl & list);
	void AutosizeAllColumns(CListCtrl & list, CFont * overridingfont);
	void AutosizeColumn(CListCtrl & list, int column, CFont * overridingfont = nullptr);

	void PrepareInplaceEdit(CListCtrl & list, int item, int subitem, CInplaceEdit & edit, std::function<InplaceEditOKFunc> func);
	void PrepareInplaceCheck(CListCtrl & list, int item, int subitem, CInplaceCheck & check, std::function<InplaceCheckOKFunc> func);
	void PrepareInplaceCombo(CListCtrl & list, int item, int subitem, CInplaceCombo & combo, std::function<InplaceComboOKFunc> func);

	void UpdateBackground(CWnd * wnd, CDC * dc, CRect rect, CBrush* optionalbrush = nullptr);
	void DrawSelectionRect(CListCtrl & list, CDC * dc, int item, CRect rect, bool focused);

	void CenterToParent(CWnd * pwnd);

	CSize GetDistance(CWnd * pwnd, CWnd * pother);
	void MoveControlBy(CWnd * pwnd, CSize delta);
	void MoveControlBy(CWnd * pwnd, int cx, int cy);
	void ShrinkWindow(CWnd * pwnd, int cx, int cy);
	void MoveWindowBy(CWnd* pwnd, int cx, int cy);
	void MoveWindowTo(CWnd* pwnd, int x, int y);

	int AlignToTopLeft(CWnd * pwnd, std::initializer_list<CWnd*> windows);

	int ConvertStyleToAlignment(int align);
	void SizeToText(CButton & btn);

	void SetImage(CWnd * wnd, CString imagepath);
	bool LoadImageResource(CImage & img, HINSTANCE hinst, CString type, UINT image);
	HBITMAP LoadImageHandle(HINSTANCE hinst, CString type, UINT image);
	bool LoadPNG(CBitmap & bmp, HINSTANCE hinst, UINT image);

	void AddThinBorder(CWnd* wnd);
	void InvalidateThinBorder(CWnd & wnd);
	void InvalidateChild(CWnd * pwnd);

	bool IsKeyDown(int vkey);

	void NotifyParent(CWnd * control, UINT uNotifyMessage);
	void CommandParent(CWnd * control, UINT uNotifyMessage);

	CString LoadStringResource(UINT id, CString defaulttext);

	CString GetFontFace(CFont * font);
	bool CreateResizedFont(CFont* origfont, CFont& newfont, float sizefactor);
	BOOL MakePointFont(CString name, int pointsize, CFont & font);
	BOOL GetResizedFont(CFont* origfont, int pointsize, CFont & newfont);

	int PointsToHeight(int points); // uses the screen, i.e. GetDC(NULL)
	int PointsToHeight(int points, HDC dc);
	int HeightToPoints(int height);
	int HeightToPoints(int height, HDC dc);

	HMODULE GetExternalModule(CString editorpath);

	BOOL AbortInitDialog(CDialog * dlg, CString message); // ends the dialog, returns TRUE
	bool AbortFail(CDialog * dlg, CString message);      // ends the dialog, returns false

	int Done(int code, CString message);
	bool Fail(CString message);

	LRESULT SendNotificationToParent(CWnd* wnd, UINT notifyid);
	LRESULT SendBnClickedToParent(CWnd* wnd);
	LRESULT SendLbnSelchangeToParent(CWnd* wnd);
}
