#include "stdafx.h"
#include "FFHImages.h"

FFHImages::FFHImages()
	: images(std::make_unique<CImageList>())
{
}

FFHImages::~FFHImages()
{
	DeleteImageList();
}

FFHImages::FFHImages(const FFHImages& rhs)
	: FFHImages()
{
	this->operator=(rhs);
}

FFHImages& FFHImages::operator=(const FFHImages& rhs)
{
	if (rhs.HasList()) {
		// Copying an image llst isn't directly suported,
		// so use the Read/Write support provided via CArchive.
		// The CArchives are in scopes to flush and close without making explicit calls to do so.
		CMemFile m;
		{
			CArchive arstore(&m, CArchive::store);
			if (!rhs.images->Write(&arstore))
				throw std::runtime_error("FFHImages assignment failed during store phase.");
		}

		DeleteImageList();
		m.SeekToBegin();
		{
			CArchive arload(&m, CArchive::load);
			if (!images->Read(&arload))
				throw std::runtime_error("FFHImages assignment failed during read phase.");
		}
	}
	return *this;
}

CImageList& FFHImages::get()
{
	return *images;
}

//TODO - remove and use the other overload
//bool FFHImages::Create(int width, int height)
//{
//	DeleteImageList();
//	m_size = { 0,0 };
//	bool created = images->Create(width, height, ILC_COLOR16 + ILC_MASK, 2, 0) == TRUE;
//	if (created)
//		m_size = { width, height };
//	return created;
//}

bool FFHImages::Create(int width, int height, int flags, int initialCount, int growBy)
{
	DeleteImageList();
	m_size = { 0,0 };
	bool created = images->Create(width, height, flags, initialCount, growBy) == TRUE;
	if (created)
		m_size = { width, height };
	return created;
}

void FFHImages::Empty()
{
	while (images->GetImageCount() > 0) images->Remove(0);
}

BOOL FFHImages::Draw(CDC* pdc, int index, POINT pt, UINT style)
{
	return images->Draw(pdc, index, pt, style);
}

void FFHImages::DeleteImageList()
{
	if (HasList())
		images->DeleteImageList();
}

bool FFHImages::HasList() const
{
	return images != nullptr && images->m_hImageList != NULL;
}