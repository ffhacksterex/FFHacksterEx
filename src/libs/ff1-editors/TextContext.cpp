//DEVNOTE - EXPERIMENTAL WIP, NOT CURRENTLY USED

#include "stdafx.h"
//#include "TextContext.h"
//#include <FFHacksterProject.h>
//#include "TextEditorSettings.h"
//#include <string_functions.h>
//
//using namespace Strings;
//
//
//TextContext::TextContext()
//	: m_proj(nullptr)
//{
//}
//
//TextContext::TextContext(CFFHacksterProject& proj)
//	: m_proj(&proj)
//{
//}
//
//TextContext::TextContext(CFFHacksterProject& proj, int acontext, CString adisplayname, int aptroffset, int aptradd, int acount,
//	int atextstart, int atextend, int aptrtblstart)
//	: TextContext(proj, acontext, adisplayname, aptroffset, aptradd, acount, atextstart, atextend, aptrtblstart, acontext)
//{
//	// delegate to the other constructor, passing the context as the dteindex.
//}
//
//TextContext::TextContext(CFFHacksterProject& proj, int acontext, CString adisplayname, int aptroffset, int aptradd, int acount,
//	int atextstart, int atextend, int aptrtblstart, int adteindex)
//	: m_proj(&proj)
//{
//	texttype = acontext;
//	displayname = adisplayname;
//	ptroffset = aptroffset;
//	ptradd = aptradd;
//	count = acount;
//	textstart = atextstart;
//	textend = atextend;
//	ptrtblstart = aptrtblstart;
//	ptrtblcount = acount;
//	kabptrset = ptroffset;
//	kabcount = acount; // << 1;
//	dteindex = adteindex;
//	keyname = MakeVarName(displayname);
//}
//
//TextContext & TextContext::SetKabValues(int akabptrset, int akabcount)
//{
//	kabptrset = akabptrset;
//	kabcount = akabcount;
//	ptrtblstart = akabptrset;
//	ptrtblcount = akabcount;
//	return *this;
//}
//
////TODO - REMOVE? might not use this method
//TextContext & TextContext::SetPtrTableCount(int otblcount)
//{
//	UNREFERENCED_PARAMETER(otblcount);
//	//ptrtblcount = otblcount;
//	//kabcount = otblcount;
//	return *this;
//}
//
//TextContext & TextContext::SetSemantics(TEXTSEMANTICS sem)
//{
//	semantics = sem;
//	return *this;
//}
//
//TextContext & TextContext::UseFullTextEditor(bool afulltext)
//{
//	isfulltext = afulltext;
//	return *this;
//}
//
//TextContext & TextContext::Show(bool show)
//{
//	visible = show;
//	return *this;
//}
//
//TextContext & TextContext::SetDteIndex(int adteindex)
//{
//	dteindex = adteindex;
//	return *this;
//}
//
//TextContext TextContext::StepCopyAs(int acontext, CString adisplayname, int newcount) const
//{
//	auto that = *this;
//	that.texttype = acontext;
//	that.displayname = adisplayname;
//	that.ptroffset += count << 1; // the next context steps forward by (count << 1) bytes
//	that.count = newcount;
//	//that.kabcount = newcount << 1;
//	return that;
//}
//
//bool TextContext::Visible() const
//{
//	return visible;
//}
//
//bool TextContext::IsSemantic(TEXTSEMANTICS ts) const
//{
//	return semantics == ts;
//}
//
//TEXTSEMANTICS TextContext::GetSemantic() const
//{
//	return semantics;
//}
//
//bool TextContext::ViewWithDte() const
//{
//	if (m_proj == nullptr)
//		throw std::runtime_error("TextContext is not linked to a project");
//
//	// If it has a DTE index, it's a default stored in project settings
//	//FUTURE - this originally kept compatibility with FFHackster, but that's no longer possible,
//	//		so move the TextViewInDTE[] settings into the text editor settings per-project.
//	if (dteindex != -1)
//		return m_proj->TextViewInDTE[dteindex];
//
//	// otherwise, use the native setting
//	TextEditorSettings stgs(*m_proj);
//	return stgs.ReadDteSetting(keyname, true);
//
//}
//
//void TextContext::SetViewWithDte(bool view)
//{
//	if (m_proj == nullptr)
//		throw std::runtime_error("TextContext is not linked to a project");
//
//	// If it has a DTE index, it's a default stored in project settings
//	//FUTURE - move these to the text editor settings for this project.
//	if (dteindex != -1) {
//		m_proj->TextViewInDTE[dteindex] = view;
//	}
//	else {
//		TextEditorSettings stgs(*m_proj);
//		stgs.WriteDteSetting(keyname, view);
//	}
//}