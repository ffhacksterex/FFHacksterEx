#pragma once

#include <memory>

class FFHImages
{
public:
	FFHImages();
	~FFHImages();
	FFHImages(const FFHImages& rhs);
	FFHImages& operator=(const FFHImages& rhs);
	
	CImageList& get();

	bool Create(int width, int height);
	void Empty();

	BOOL Draw(CDC* pdc, int index, POINT pt, UINT style);
	
private:
	std::unique_ptr<CImageList> images;
	SIZE m_size{};

	void DeleteImageList();
	bool HasList() const;
};
