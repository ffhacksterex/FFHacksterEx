#pragma once

#include <afx.h>

class IProgress
{
public:
	IProgress() {}
	virtual ~IProgress() {}

	void AddSteps(int numtoadd) { DoAddSteps(numtoadd); }
	void Step() { DoStep(); }
	void SetProgressText(LPCSTR text) { DoSetProgressText(text); }
	void StepAndProgressText(LPCSTR text) { DoSetProgressText(text); DoStep(); }
	void SetStatusText(LPCSTR text) { DoSetStatusText(text); }

private:
	virtual void DoAddSteps(int numtoadd) = 0;
	virtual void DoStep() = 0;
	virtual void DoSetProgressText(LPCSTR text) = 0;
	virtual void DoSetStatusText(LPCSTR text) = 0;
};
