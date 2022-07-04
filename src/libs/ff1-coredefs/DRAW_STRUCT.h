#pragma once

struct DRAW_STRUCT
{
	BYTE* bitmap;		//pointer to the bitmap
	BYTE* PicFormation;	//pointer to the bitmap formation
	BYTE* PalFormation;	//pointer to the palette formation

	BYTE palmax;		//the max number of valid palette entries

	BYTE Width;			//width of the bitmap formation
	BYTE Height;		//height
	short Max;			//last tile used in the bitmap formation

	CRect rcPalette;	//the RECT for the palette to be drawn
	CRect rcGraphic;	//the RECT for the graphic
	CRect rcCloseup;	//the closeup
	CRect rcFinger;		//the "finger" cursor

	BYTE curblock;		//the block we are currently editing
	BYTE curpal;		//the current palette selection

	BYTE mousedown;		//is the mouse button down?
	CPoint mouseclick;	//where was the last place clicked (used for drag-drawing)
};
