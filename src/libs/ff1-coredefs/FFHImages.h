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

	//bool Create(int width, int height);
	bool Create(int width, int height, int flags, int initialCount, int growBy);
	void Empty();                  // removes images, but doesn't delete the image list
	void DeleteImageList();        // deletes the image list

	BOOL Draw(CDC* pdc, int index, POINT pt, UINT style);
	
private:
	std::unique_ptr<CImageList> images;
	SIZE m_size{};

	bool HasList() const;
};
