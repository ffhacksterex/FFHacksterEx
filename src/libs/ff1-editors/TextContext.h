////DEVNOTE - EXPERIMENTAL WIP, NOT CURRENTLY USED
//
//#pragma once
//
//#include <vector>
//#include <map>
//
//enum TEXTCONTEXTTYPE {
//	ITEMS, WEAPONS, ARMOR, GOLD, STDMAGIC, CLASSES, ENEMYATTACKS, ENEMIES, DIALOGUE, BATTLEMESSAGES,
//	INTROTEXT, SHOPTEXT, STATUSMENUTEXT, STORY,
//	TEXTMODE_COUNT
//};
//
//enum TEXTSEMANTICS { AS_NORMAL = 0, AS_DIALOGUE, AS_STORY, AS_INTRO };
//
//class TextContext
//{
//public:
//	TextContext();
//	TextContext(CFFHacksterProject& proj);
//	TextContext(CFFHacksterProject& proj, int context, CString displayname, int aptroffset, int aptradd, int acount,
//		int atextstart, int atextend, int aptrtblstart);
//	TextContext(CFFHacksterProject& proj, int context, CString displayname, int aptroffset, int aptradd, int acount,
//		int atextstart, int atextend, int aptrtblstart, int adteindex);
//	virtual ~TextContext() {}
//
//public:
//	TextContext & SetKabValues(int akabptrset, int akabcount);
//	TextContext & SetPtrTableCount(int aptrtblcount);
//	TextContext & SetSemantics(TEXTSEMANTICS sem);
//	TextContext & SetDteIndex(int adteindex);
//	TextContext & UseFullTextEditor(bool afulltext);
//	TextContext & Show(bool show);
//
//	TextContext StepCopyAs(int acontext, CString adisplayname, int newcount) const;
//
//	bool Visible() const;
//	bool IsSemantic(TEXTSEMANTICS ts) const;
//	TEXTSEMANTICS GetSemantic() const;
//
//	bool ViewWithDte() const;
//	void SetViewWithDte(bool view);
//
//	int texttype = -1; // TEXTCONTEXTTYPE
//	CString displayname;
//
//	int ptroffset = -1;
//	int ptradd = -1;
//	int count = -1;
//	int textstart = -1;
//	int textend = -1;
//	int ptrtblstart = -1;
//	int ptrtblcount = -1;
//
//	int kabptrset = -1;
//	int kabcount = -1;
//
//	int dteindex = -1;		// ignore DTE if -1; read from project settings with this index if >= 0
//	bool isfulltext = false;
//
//private:
//	CFFHacksterProject * m_proj;
//	TEXTSEMANTICS semantics = AS_NORMAL;
//	CString keyname;
//	bool visible = true;
//};
//
//using contextentry = TextContext;
//using contextlist = std::vector<contextentry>;
//using contextmap = std::map<int, contextentry>;
