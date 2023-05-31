#include "stdafx.h"
#include "draw_functions.h"
#include "FFHacksterProject.h"
#include "DRAW_STRUCT.h"
#include "FFH2Project.h"
#include "common_symbols.h" //TODO - only added for UiTemp::COMMON_OFN
#include "path_functions.h"


BYTE Draw_ButtonDown(DRAW_STRUCT* draw, CPoint pt, bool rbutton)
{
	//return values:
	//  1 = Redraw Graphic/Closeup
	//  2 = Redraw Finger
	//  3 = Redraw all

	if (PtInRect(draw->rcPalette, pt)) {
		draw->curpal = (BYTE)(((pt.x - draw->rcPalette.left) >> 4) + 1);
		return 2;
	}
	else if (PtInRect(draw->rcGraphic, pt)) {
		pt.y = (pt.y - draw->rcGraphic.top) >> 4;
		pt.x = (pt.x - draw->rcGraphic.left) >> 4;
		short temp = draw->PicFormation[(pt.y * draw->Width) + pt.x];
		if (temp == 0xFF) return 0;
		draw->curblock = (BYTE)temp;
		return 1;
	}
	else if (PtInRect(draw->rcCloseup, pt)) {
		draw->mousedown = 1 + rbutton;
		draw->mouseclick.x = (pt.x - draw->rcCloseup.left) >> 4;
		draw->mouseclick.y = (pt.y - draw->rcCloseup.top) >> 4;
		BYTE* temp = &draw->bitmap[(draw->curblock << 6) + (draw->mouseclick.y << 3) + draw->mouseclick.x];
		if (rbutton) *temp = 0;
		else *temp = draw->curpal;
		return 1;
	}

	return 0;
}

bool Draw_MouseMove(DRAW_STRUCT* draw, CPoint pt)
{
	if (draw->mousedown && PtInRect(draw->rcCloseup, pt)) {
		CPoint click;
		click.x = (pt.x - draw->rcCloseup.left) >> 4;
		click.y = (pt.y - draw->rcCloseup.top) >> 4;
		if (click != draw->mouseclick) {
			draw->mouseclick = click;
			BYTE* set = &draw->bitmap[(draw->curblock << 6) + (draw->mouseclick.y << 3) + draw->mouseclick.x];
			if (draw->mousedown == 2) *set = 0;
			else *set = draw->curpal;
			return 1;
		}
	}
	return 0;
}

void DrawTile(CDC* dc, int tX, int tY, CFFHacksterProject* cart, int offset, BYTE* palette, BYTE tint)
{
	BYTE pixel[2][2] = { 0,2,1,3 };
	BYTE coX, coY, line;
	for (coY = 0; coY < 8; coY++, tX -= 8, tY++, offset++) {
		for (coX = 0, line = 0x80; coX < 8; coX++, tX++, line >>= 1) {
			dc->SetPixelV(tX, tY, cart->Palette[tint][palette[pixel
				[(cart->ROM[offset] & line) != 0]
			[(cart->ROM[offset + 8] & line) != 0]]]);
		}
	}
}

void DrawTileScale(CDC* dc, int tX, int tY, CFFHacksterProject* cart, int offset, BYTE* palette, int scale, BYTE tint)
{
	BYTE temp, coX, coY, line;
	BYTE pixel[2][2] = { 0,2,1,3 };
	CRect rc(tX, tY, tX + scale, tY + scale);
	CBrush br[4];
	for (temp = 0; temp < 4; temp++)
		br[temp].CreateSolidBrush(cart->Palette[tint][palette[temp]]);

	for (coY = 0; coY < 8; coY++, offset++, rc.top += scale, rc.bottom += scale, rc.left = tX, rc.right = tX + scale) {
		for (coX = 0, line = 0x80; coX < 8; coX++, line >>= 1, rc.left += scale, rc.right += scale) {
			dc->FillRect(rc, &br[pixel
				[(cart->ROM[offset] & line) != 0]
			[(cart->ROM[offset + 8] & line) != 0]]);
		}
	}

	for (temp = 0; temp < 4; temp++)
		br[temp].DeleteObject();
}

void DrawTileScale(CDC* dc, int tX, int tY, FFH2Project* proj, int offset, BYTE* palette, int scale, BYTE tint)
{
	BYTE temp, coX, coY, line;
	BYTE pixel[2][2] = { 0,2,1,3 };
	CRect rc(tX, tY, tX + scale, tY + scale);
	CBrush br[4];
	for (temp = 0; temp < 4; temp++)
		br[temp].CreateSolidBrush(proj->palette[tint][palette[temp]]);

	for (coY = 0; coY < 8; coY++, offset++, rc.top += scale, rc.bottom += scale, rc.left = tX, rc.right = tX + scale) {
		for (coX = 0, line = 0x80; coX < 8; coX++, line >>= 1, rc.left += scale, rc.right += scale) {
			dc->FillRect(rc, &br[pixel
				[(proj->ROM[offset] & line) != 0]
			[(proj->ROM[offset + 8] & line) != 0]]);
		}
	}

	for (temp = 0; temp < 4; temp++)
		br[temp].DeleteObject();
}

void Draw_ROM_Buffer(CFFHacksterProject* cart, int offset, DRAW_STRUCT* draw)
{
	BYTE coY, coX, line;
	int co;
	BYTE pixel[2][2] = { 0,2,1,3 };
	int bmpco = 0;

	for (co = 0; co < draw->Max; co++, offset += 8) {
		for (coY = 0; coY < 8; coY++, offset++) {
			for (coX = 0, line = 0x80; coX < 8; coX++, line >>= 1, bmpco++)
				draw->bitmap[bmpco] = pixel[(cart->ROM[offset] & line) != 0][(cart->ROM[offset + 8] & line) != 0];
		}
	}
}

void Draw_ROM_Buffer(FFH2Project* proj, int offset, DRAW_STRUCT* draw)
{
	BYTE coY, coX, line;
	int co;
	BYTE pixel[2][2] = { 0,2,1,3 };
	int bmpco = 0;

	for (co = 0; co < draw->Max; co++, offset += 8) {
		for (coY = 0; coY < 8; coY++, offset++) {
			for (coX = 0, line = 0x80; coX < 8; coX++, line >>= 1, bmpco++)
				draw->bitmap[bmpco] = pixel[(proj->ROM[offset] & line) != 0][(proj->ROM[offset + 8] & line) != 0];
		}
	}
}

void Draw_Buffer_ROM(CFFHacksterProject* cart, int offset, DRAW_STRUCT* draw)
{
	BYTE coY, coX, line;
	int bmpco = 0, co;

	for (co = 0; co < draw->Max; co++, offset += 8) {
		for (coY = 0; coY < 8; coY++, offset++) {
			cart->ROM[offset] = 0;
			cart->ROM[offset + 8] = 0;
			for (coX = 0, line = 0x80; coX < 8; coX++, line >>= 1, bmpco++) {
				if (draw->bitmap[bmpco] & 0x01) cart->ROM[offset] |= line;
				if (draw->bitmap[bmpco] & 0x02) cart->ROM[offset + 8] |= line;
			}
		}
	}
}

void Draw_Buffer_ROM(FFH2Project* proj, int offset, DRAW_STRUCT* draw)
{
	BYTE coY, coX, line;
	int bmpco = 0, co;

	for (co = 0; co < draw->Max; co++, offset += 8) {
		for (coY = 0; coY < 8; coY++, offset++) {
			proj->ROM[offset] = 0;
			proj->ROM[offset + 8] = 0;
			for (coX = 0, line = 0x80; coX < 8; coX++, line >>= 1, bmpco++) {
				if (draw->bitmap[bmpco] & 0x01) proj->ROM[offset] |= line;
				if (draw->bitmap[bmpco] & 0x02) proj->ROM[offset + 8] |= line;
			}
		}
	}
}

void Draw_Buffer_ROM(std::vector<unsigned char>& rom, int offset, DRAW_STRUCT* draw)
{
	BYTE coY, coX, line;
	int bmpco = 0, co;

	for (co = 0; co < draw->Max; co++, offset += 8) {
		for (coY = 0; coY < 8; coY++, offset++) {
			rom[offset] = 0;
			rom[offset + 8] = 0;
			for (coX = 0, line = 0x80; coX < 8; coX++, line >>= 1, bmpco++) {
				if (draw->bitmap[bmpco] & 0x01) rom[offset] |= line;
				if (draw->bitmap[bmpco] & 0x02) rom[offset + 8] |= line;
			}
		}
	}
}

void Draw_DrawGraphic(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal, bool drawbox, bool hideFF)
{
	CPen redpen; redpen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* oldpen = mDC->SelectObject(&redpen);
	BYTE coY, coX, paltemp;
	short temp;
	int co = 0;
	int drawX, drawY;
	for (coY = 0, drawY = draw->rcGraphic.top; coY < draw->Height; coY++, drawY += 16) {
		for (coX = 0, drawX = draw->rcGraphic.left; coX < draw->Width; coX++, co++, drawX += 16) {
			temp = draw->PicFormation[co];
			if (temp == 0xFF && hideFF) continue;

			if (draw->PalFormation == nullptr) paltemp = 0;
			else paltemp = draw->PalFormation[(coY * draw->Width) + coX] << 2;
			Draw_DrawBufferTile(mDC, drawX, drawY, cart, draw, &draw->bitmap[temp << 6], &pal[paltemp], 2);
			if (drawbox && draw->curblock == temp) {
				mDC->MoveTo(drawX, drawY);
				mDC->LineTo(drawX + 15, drawY);
				mDC->LineTo(drawX + 15, drawY + 15);
				mDC->LineTo(drawX, drawY + 15);
				mDC->LineTo(drawX, drawY);
			}
		}
	}
	mDC->SelectObject(oldpen);
	redpen.DeleteObject();
}

void Draw_DrawGraphic(CDC* mDC, DRAW_STRUCT* draw, FFH2Project* proj, BYTE* pal, bool drawbox, bool hideFF)
{
	CPen redpen; redpen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* oldpen = mDC->SelectObject(&redpen);
	BYTE coY, coX, paltemp;
	short temp;
	int co = 0;
	int drawX, drawY;
	for (coY = 0, drawY = draw->rcGraphic.top; coY < draw->Height; coY++, drawY += 16) {
		for (coX = 0, drawX = draw->rcGraphic.left; coX < draw->Width; coX++, co++, drawX += 16) {
			temp = draw->PicFormation[co];
			if (temp == 0xFF && hideFF) continue;

			if (draw->PalFormation == nullptr) paltemp = 0;
			else paltemp = draw->PalFormation[(coY * draw->Width) + coX] << 2;
			Draw_DrawBufferTile(mDC, drawX, drawY, proj, draw, &draw->bitmap[temp << 6], &pal[paltemp], 2);
			if (drawbox && draw->curblock == temp) {
				mDC->MoveTo(drawX, drawY);
				mDC->LineTo(drawX + 15, drawY);
				mDC->LineTo(drawX + 15, drawY + 15);
				mDC->LineTo(drawX, drawY + 15);
				mDC->LineTo(drawX, drawY);
			}
		}
	}
	mDC->SelectObject(oldpen);
	redpen.DeleteObject();
}

void Draw_DrawAll(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal)
{
	Draw_DrawPalette(mDC, draw->rcPalette.left, draw->rcPalette.top, cart, pal);
	Draw_DrawFinger(mDC, draw, cart);
	Draw_DrawGraphic(mDC, draw, cart, pal, 1, 1);
	Draw_DrawCloseup(mDC, draw, cart, pal);
}

void Draw_DrawAll(CDC* mDC, DRAW_STRUCT* draw, FFH2Project* proj, BYTE* pal)
{
	Draw_DrawPalette(mDC, draw->rcPalette.left, draw->rcPalette.top, proj, pal);
	Draw_DrawFinger(mDC, draw, proj);
	Draw_DrawGraphic(mDC, draw, proj, pal, 1, 1);
	Draw_DrawCloseup(mDC, draw, proj, pal);
}

void Draw_DrawFinger(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart)
{
	CPoint pt;
	pt.x = (draw->rcFinger.left - 16) + (draw->curpal << 4);
	pt.y = draw->rcFinger.top;
	cart->Finger.Draw(mDC, 1, pt, ILD_TRANSPARENT);
}

void Draw_DrawFinger(CDC* mDC, DRAW_STRUCT* draw, FFH2Project* proj)
{
	CPoint pt;
	pt.x = (draw->rcFinger.left - 16) + (draw->curpal << 4);
	pt.y = draw->rcFinger.top;
	proj->Finger.Draw(mDC, 1, pt, ILD_TRANSPARENT);
}

void Draw_DrawPalette(CDC* mDC, int pX, int pY, CFFHacksterProject* cart, BYTE* pal)
{
	CBrush br[3];
	BYTE co;
	for (co = 0; co < 3; co++)
		br[co].CreateSolidBrush(cart->Palette[0][pal[co + 1]]);

	CRect rc(pX, pY, pX + 16, pY + 16);
	mDC->FillRect(rc, &br[0]); rc.left += 16; rc.right += 16;
	mDC->FillRect(rc, &br[1]); rc.left += 16; rc.right += 16;
	mDC->FillRect(rc, &br[2]);

	for (co = 0; co < 3; co++)
		br[co].DeleteObject();
}

void Draw_DrawPalette(CDC* mDC, int pX, int pY, FFH2Project* proj, BYTE* pal)
{
	CBrush br[3];
	BYTE co;
	for (co = 0; co < 3; co++)
		br[co].CreateSolidBrush(proj->palette[0][pal[co + 1]]);

	CRect rc(pX, pY, pX + 16, pY + 16);
	mDC->FillRect(rc, &br[0]); rc.left += 16; rc.right += 16;
	mDC->FillRect(rc, &br[1]); rc.left += 16; rc.right += 16;
	mDC->FillRect(rc, &br[2]);

	for (co = 0; co < 3; co++)
		br[co].DeleteObject();
}

void Draw_DrawPatternTables(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal, CRect rc, int max, bool drawbox)
{
	CPen redpen; redpen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* oldpen = mDC->SelectObject(&redpen);
	BYTE coY, coX;
	int co = 0;
	int drawX, drawY;
	for (coY = 0, drawY = rc.top; co < max; coY++, drawY += 16) {
		for (coX = 0, drawX = rc.left; coX < 16 && co < max; coX++, co++, drawX += 16)
			Draw_DrawBufferTile(mDC, drawX, drawY, cart, draw, &draw->bitmap[co << 6], pal, 2);
	}

	if (drawbox && draw->curblock != 0xFF) {
		drawX = rc.left + ((draw->curblock & 0x0F) << 4);
		drawY = rc.top + (draw->curblock & 0xF0);
		mDC->MoveTo(drawX, drawY); mDC->LineTo(drawX + 15, drawY);
		mDC->LineTo(drawX + 15, drawY + 15); mDC->LineTo(drawX, drawY + 15);
		mDC->LineTo(drawX, drawY);
	}

	mDC->SelectObject(oldpen);
	redpen.DeleteObject();
}

void Draw_DrawPatternTables(CDC* mDC, DRAW_STRUCT* draw, FFH2Project* proj, BYTE* pal, CRect rc, int max, bool drawbox)
{
	CPen redpen; redpen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* oldpen = mDC->SelectObject(&redpen);
	BYTE coY, coX;
	int co = 0;
	int drawX, drawY;
	for (coY = 0, drawY = rc.top; co < max; coY++, drawY += 16) {
		for (coX = 0, drawX = rc.left; coX < 16 && co < max; coX++, co++, drawX += 16)
			Draw_DrawBufferTile(mDC, drawX, drawY, proj, draw, &draw->bitmap[co << 6], pal, 2);
	}

	if (drawbox && draw->curblock != 0xFF) {
		drawX = rc.left + ((draw->curblock & 0x0F) << 4);
		drawY = rc.top + (draw->curblock & 0xF0);
		mDC->MoveTo(drawX, drawY); mDC->LineTo(drawX + 15, drawY);
		mDC->LineTo(drawX + 15, drawY + 15); mDC->LineTo(drawX, drawY + 15);
		mDC->LineTo(drawX, drawY);
	}

	mDC->SelectObject(oldpen);
	redpen.DeleteObject();
}

void Draw_DrawCloseup(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal)
{
	Draw_DrawBufferTile(mDC, draw->rcCloseup.left, draw->rcCloseup.top, cart, draw, &draw->bitmap[draw->curblock << 6], pal, 16);
}

void Draw_DrawCloseup(CDC* mDC, DRAW_STRUCT* draw, FFH2Project* proj, BYTE* pal)
{
	Draw_DrawBufferTile(mDC, draw->rcCloseup.left, draw->rcCloseup.top, proj, draw, &draw->bitmap[draw->curblock << 6], pal, 16);
}

void Draw_DrawBufferTile(CDC* mDC, int pX, int pY, CFFHacksterProject* cart, DRAW_STRUCT* draw, BYTE* bitmap, BYTE* pal, int size)
{
	CBrush br[16];
	BYTE coX, coY;
	int co;
	for (co = 0; co < draw->palmax; co++)
		br[co].CreateSolidBrush(cart->Palette[0][pal[co]]);

	CRect rc;
	for (coY = 0, co = 0, rc.top = pY, rc.bottom = pY + size; coY < 8; coY++, rc.top += size, rc.bottom += size) {
		for (coX = 0, rc.left = pX, rc.right = pX + size; coX < 8; coX++, co++, rc.left += size, rc.right += size)
			mDC->FillRect(rc, &br[bitmap[co]]);
	}

	for (co = 0; co < draw->palmax; co++)
		br[co].DeleteObject();
}

void Draw_DrawBufferTile(CDC* mDC, int pX, int pY, FFH2Project* proj, DRAW_STRUCT* draw, BYTE* bitmap, BYTE* pal, int size)
{
	CBrush br[16];
	BYTE coX, coY;
	int co;
	for (co = 0; co < draw->palmax; co++)
		br[co].CreateSolidBrush(proj->palette[0][pal[co]]);

	CRect rc;
	for (coY = 0, co = 0, rc.top = pY, rc.bottom = pY + size; coY < 8; coY++, rc.top += size, rc.bottom += size) {
		for (coX = 0, rc.left = pX, rc.right = pX + size; coX < 8; coX++, co++, rc.left += size, rc.right += size)
			mDC->FillRect(rc, &br[bitmap[co]]);
	}

	for (co = 0; co < draw->palmax; co++)
		br[co].DeleteObject();
}

void Draw_ExportToBmp(DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal, CString initialfile)
{
	CFileDialog dlg(0, "bmp", nullptr, OFN_OVERWRITEPROMPT | UiTemp::COMMON_OFN,
		"16/256 Color Bitmaps (*.bmp)|*bmp|All Files (*.*)|*.*||");
	char initpath[_MAX_PATH + 1] = { 0 };
	if (!initialfile.IsEmpty()) {
		dlg.m_pOFN->lpstrFile = initpath;
		if (Paths::IsDir(initialfile)) initialfile += "\\"; // this sets it as initial dir instead
		strncpy(initpath, initialfile, _MAX_PATH);
	}
	if (dlg.DoModal() != IDOK) return;

	FILE* file = fopen(dlg.GetPathName(), "w+b");
	if (file == nullptr) {
		AfxMessageBox("Unable to create bitmap.", MB_ICONERROR);
		return;
	}

	BYTE header[118];
	int height = draw->Height << 3;
	int width = draw->Width << 3;

	for (int co = 0; co < 118; co++)
		header[co] = 0;

	header[0] = 'B';
	header[1] = 'M';		//file type

	header[10] = 118;		//offset to the data
	header[14] = 40;		//size of rest of header

	header[18] = width & 0xFF;
	header[19] = (width >> 8) & 0xFF;
	header[20] = (width >> 16) & 0xFF;
	header[21] = (width >> 24) & 0xFF;	//bitmap's width

	header[22] = height & 0xFF;
	header[23] = (height >> 8) & 0xFF;
	header[24] = (height >> 16) & 0xFF;
	header[25] = (height >> 24) & 0xFF;	//bitmap's height

	header[26] = 1;		//# of planes

	header[28] = 4;		//bits per pixel  (4 = 16 colors)

	int temp, co2 = 54;	//palette
	for (int co = 0; co < draw->palmax; co++, co2 += 4) {
		temp = cart->Palette[0][pal[co]];
		header[co2] = (BYTE)(temp >> 16);
		header[co2 + 1] = (temp >> 8) & 0xFF;
		header[co2 + 2] = temp & 0xFF;
	}

	fwrite(header, 1, 118, file);

	BYTE bmtemp[4];
	BYTE bigY, smallY;
	BYTE palshot = 0;

	for (height = (draw->Height << 3) - 1; height >= 0; height--) {
		for (width = 0; width < draw->Width; width++) {
			bigY = (byte)(height >> 3);
			smallY = height & 0x07;

			auto co = draw->PicFormation[(bigY * draw->Width) + width];
			if (draw->PalFormation != nullptr) palshot = draw->PalFormation[(bigY * draw->Width) + width] << 2;
			if (co != 0xFF) {
				co = (co << 6) + (smallY << 3);
				for (co2 = 0; co2 < 4; co2++, co += 2)
					bmtemp[co2] = ((draw->bitmap[co] + palshot) << 4) + draw->bitmap[co + 1] + palshot;
			}
			else {
				for (co = 0; co < 4; co++)
					bmtemp[co] = 0;
			}

			fwrite(bmtemp, 1, 4, file);

		}
	}

	fclose(file);
}

void Draw_ExportToBmp(DRAW_STRUCT* draw, FFH2Project* cart, BYTE* pal, CString initialfile)
{
	CFileDialog dlg(0, "bmp", nullptr, OFN_OVERWRITEPROMPT | UiTemp::COMMON_OFN,
		"16/256 Color Bitmaps (*.bmp)|*bmp|All Files (*.*)|*.*||");
	char initpath[_MAX_PATH + 1] = { 0 };
	if (!initialfile.IsEmpty()) {
		dlg.m_pOFN->lpstrFile = initpath;
		if (Paths::IsDir(initialfile)) initialfile += "\\"; // this sets it as initial dir instead
		strncpy(initpath, initialfile, _MAX_PATH);
	}
	if (dlg.DoModal() != IDOK) return;

	FILE* file = fopen(dlg.GetPathName(), "w+b");
	if (file == nullptr) {
		AfxMessageBox("Unable to create bitmap.", MB_ICONERROR);
		return;
	}

	BYTE header[118];
	int height = draw->Height << 3;
	int width = draw->Width << 3;

	for (int co = 0; co < 118; co++)
		header[co] = 0;

	header[0] = 'B';
	header[1] = 'M';		//file type

	header[10] = 118;		//offset to the data
	header[14] = 40;		//size of rest of header

	header[18] = width & 0xFF;
	header[19] = (width >> 8) & 0xFF;
	header[20] = (width >> 16) & 0xFF;
	header[21] = (width >> 24) & 0xFF;	//bitmap's width

	header[22] = height & 0xFF;
	header[23] = (height >> 8) & 0xFF;
	header[24] = (height >> 16) & 0xFF;
	header[25] = (height >> 24) & 0xFF;	//bitmap's height

	header[26] = 1;		//# of planes

	header[28] = 4;		//bits per pixel  (4 = 16 colors)

	int temp, co2 = 54;	//palette
	for (int co = 0; co < draw->palmax; co++, co2 += 4) {
		temp = cart->palette[0][pal[co]];
		header[co2] = (BYTE)(temp >> 16);
		header[co2 + 1] = (temp >> 8) & 0xFF;
		header[co2 + 2] = temp & 0xFF;
	}

	fwrite(header, 1, 118, file);

	BYTE bmtemp[4];
	BYTE bigY, smallY;
	BYTE palshot = 0;

	for (height = (draw->Height << 3) - 1; height >= 0; height--) {
		for (width = 0; width < draw->Width; width++) {
			bigY = (byte)(height >> 3);
			smallY = height & 0x07;

			auto co = draw->PicFormation[(bigY * draw->Width) + width];
			if (draw->PalFormation != nullptr) palshot = draw->PalFormation[(bigY * draw->Width) + width] << 2;
			if (co != 0xFF) {
				co = (co << 6) + (smallY << 3);
				for (co2 = 0; co2 < 4; co2++, co += 2)
					bmtemp[co2] = ((draw->bitmap[co] + palshot) << 4) + draw->bitmap[co + 1] + palshot;
			}
			else {
				for (co = 0; co < 4; co++)
					bmtemp[co] = 0;
			}

			fwrite(bmtemp, 1, 4, file);

		}
	}

	fclose(file);
}

void Draw_ImportFromBmp(DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* palette, CString initialfile)
{
	CFileDialog dlg(1, "bmp", nullptr, OFN_HIDEREADONLY | UiTemp::COMMON_OFN,
		"16/256 Color Bitmaps (*.bmp)|*bmp|All Files (*.*)|*.*||");
	TCHAR initpath[_MAX_PATH] = { 0 };
	if (!initialfile.IsEmpty()) {
		if (Paths::FileExists(initialfile)) {
			strncpy(initpath, initialfile, _MAX_PATH);
			dlg.m_pOFN->lpstrFile = initpath;
		}
		else if (Paths::DirExists(initialfile)) {
			strncpy(initpath, initialfile, _MAX_PATH);
			dlg.m_pOFN->lpstrInitialDir = initpath;
		}
	}
	if (dlg.DoModal() != IDOK) return;

	FILE* file = fopen(dlg.GetPathName(), "rb");
	if (file == nullptr) {
		AfxMessageBox("Unable to load bitmap.", MB_ICONERROR);
		return;
	}
	BYTE header[54];

	fread(header, 1, 54, file);

	if (header[0] != 'B' || header[1] != 'M') {
		AfxMessageBox("Not a valid bitmap.", MB_ICONERROR);
		return;
	}

	if (header[28] != 4 && header[28] != 8) {
		AfxMessageBox("Only 16 and 256 color bitmaps are supported.", MB_ICONERROR);
		return;
	}

	int height = header[22] + (header[23] << 8) + (header[24] << 16) + (header[25] << 24);
	int width = header[18] + (header[19] << 8) + (header[20] << 16) + (header[21] << 24);
	int numcolors = 1 << header[28];

	BYTE bmppalette[256][4];
	fread(bmppalette, 1, numcolors << 2, file);

	BYTE PaletteExchange[4][256];
	int co;
	BYTE pal;
	int palentry;
	int dif, lowestdif;
	for (int co2 = 0; co2 < draw->palmax; co2 += 4) {
		for (co = 0; co < numcolors; co++) {
			lowestdif = 0x1000000;
			for (pal = 0; pal < 4; pal++) {
				palentry = cart->Palette[0][palette[co2 + pal]];
				dif = 0;
				dif += abs((palentry & 0xFF) - bmppalette[co][2]);
				dif += abs(((palentry >> 8) & 0xFF) - bmppalette[co][1]);
				dif += abs(((palentry >> 16) & 0xFF) - bmppalette[co][0]);
				if (dif < lowestdif) {
					lowestdif = dif;
					PaletteExchange[co2 >> 2][co] = pal;
				}
				if (!dif) break;
			}
		}
	}

	//Okay... the palette is loaded... and all the best palette conversions are found... now lets
	// actually LOAD the bitmap

	int readpixels = draw->Width << 3;
	if (width < readpixels) readpixels = width;

	BYTE pixelsperblock = 4;
	if (numcolors == 16) pixelsperblock = 8;

	int blockcount = width / pixelsperblock;
	if (width % pixelsperblock) blockcount += 1;

	int goodblocks = readpixels / pixelsperblock;
	int skippixels = readpixels % pixelsperblock;
	if (skippixels) {
		goodblocks += 1;
		skippixels = pixelsperblock - skippixels;
	}

	int readlines = draw->Height << 3;
	if (readlines > height) readlines = height;
	if (readlines < height)
		fseek(file, (height - readlines) * (blockcount << 2), SEEK_CUR);

	int coY, coX;
	BYTE PixelLine[112];
	int arid;
	BYTE usepal = 0;
	for (coY = readlines - 1; coY >= 0; coY--) {

		fread(PixelLine, 1, goodblocks << 2, file);
		fseek(file, (blockcount - goodblocks) << 2, SEEK_CUR);

		if (numcolors == 256) {
			for (coX = (goodblocks << 2) - skippixels - 1; coX >= 0; coX--) {
				arid = ((coY >> 3) * draw->Width) + (coX >> 3);
				if (draw->PalFormation != nullptr) usepal = draw->PalFormation[arid];
				arid = draw->PicFormation[arid];
				if (arid == 0xFF) continue;
				draw->bitmap[(arid << 6) + ((coY & 7) << 3) + (coX & 7)] = PaletteExchange[usepal][PixelLine[coX]];
			}
		}
		else {
			for (coX = (goodblocks << 2) - (skippixels >> 1) - 1; coX >= 0; coX--) {
				arid = ((coY >> 3) * draw->Width) + (coX >> 2);
				if (draw->PalFormation != nullptr) usepal = draw->PalFormation[arid];
				arid = draw->PicFormation[arid];
				if (arid == 0xFF) continue;
				draw->bitmap[(arid << 6) + ((coY & 7) << 3) + ((coX << 1) & 7)] = PaletteExchange[usepal][PixelLine[coX] >> 4];
				draw->bitmap[(arid << 6) + ((coY & 7) << 3) + (((coX << 1) + 1) & 7)] = PaletteExchange[usepal][PixelLine[coX] & 0x0F];
			}
		}
	}

	fclose(file);
}

void Draw_ImportFromBmp(DRAW_STRUCT* draw, FFH2Project* cart, BYTE* palette, CString initialfile)
{
	CFileDialog dlg(1, "bmp", nullptr, OFN_HIDEREADONLY | UiTemp::COMMON_OFN,
		"16/256 Color Bitmaps (*.bmp)|*bmp|All Files (*.*)|*.*||");
	TCHAR initpath[_MAX_PATH] = { 0 };
	if (!initialfile.IsEmpty()) {
		if (Paths::FileExists(initialfile)) {
			strncpy(initpath, initialfile, _MAX_PATH);
			dlg.m_pOFN->lpstrFile = initpath;
		}
		else if (Paths::DirExists(initialfile)) {
			strncpy(initpath, initialfile, _MAX_PATH);
			dlg.m_pOFN->lpstrInitialDir = initpath;
		}
	}
	if (dlg.DoModal() != IDOK) return;

	FILE* file = fopen(dlg.GetPathName(), "rb");
	if (file == nullptr) {
		AfxMessageBox("Unable to load bitmap.", MB_ICONERROR);
		return;
	}
	BYTE header[54];

	fread(header, 1, 54, file);

	if (header[0] != 'B' || header[1] != 'M') {
		AfxMessageBox("Not a valid bitmap.", MB_ICONERROR);
		return;
	}

	if (header[28] != 4 && header[28] != 8) {
		AfxMessageBox("Only 16 and 256 color bitmaps are supported.", MB_ICONERROR);
		return;
	}

	int height = header[22] + (header[23] << 8) + (header[24] << 16) + (header[25] << 24);
	int width = header[18] + (header[19] << 8) + (header[20] << 16) + (header[21] << 24);
	int numcolors = 1 << header[28];

	BYTE bmppalette[256][4];
	fread(bmppalette, 1, numcolors << 2, file);

	BYTE PaletteExchange[4][256];
	int co;
	BYTE pal;
	int palentry;
	int dif, lowestdif;
	for (int co2 = 0; co2 < draw->palmax; co2 += 4) {
		for (co = 0; co < numcolors; co++) {
			lowestdif = 0x1000000;
			for (pal = 0; pal < 4; pal++) {
				palentry = cart->palette[0][palette[co2 + pal]];
				dif = 0;
				dif += abs((palentry & 0xFF) - bmppalette[co][2]);
				dif += abs(((palentry >> 8) & 0xFF) - bmppalette[co][1]);
				dif += abs(((palentry >> 16) & 0xFF) - bmppalette[co][0]);
				if (dif < lowestdif) {
					lowestdif = dif;
					PaletteExchange[co2 >> 2][co] = pal;
				}
				if (!dif) break;
			}
		}
	}

	//Okay... the palette is loaded... and all the best palette conversions are found... now lets
	// actually LOAD the bitmap

	int readpixels = draw->Width << 3;
	if (width < readpixels) readpixels = width;

	BYTE pixelsperblock = 4;
	if (numcolors == 16) pixelsperblock = 8;

	int blockcount = width / pixelsperblock;
	if (width % pixelsperblock) blockcount += 1;

	int goodblocks = readpixels / pixelsperblock;
	int skippixels = readpixels % pixelsperblock;
	if (skippixels) {
		goodblocks += 1;
		skippixels = pixelsperblock - skippixels;
	}

	int readlines = draw->Height << 3;
	if (readlines > height) readlines = height;
	if (readlines < height)
		fseek(file, (height - readlines) * (blockcount << 2), SEEK_CUR);

	int coY, coX;
	BYTE PixelLine[112];
	int arid;
	BYTE usepal = 0;
	for (coY = readlines - 1; coY >= 0; coY--) {

		fread(PixelLine, 1, goodblocks << 2, file);
		fseek(file, (blockcount - goodblocks) << 2, SEEK_CUR);

		if (numcolors == 256) {
			for (coX = (goodblocks << 2) - skippixels - 1; coX >= 0; coX--) {
				arid = ((coY >> 3) * draw->Width) + (coX >> 3);
				if (draw->PalFormation != nullptr) usepal = draw->PalFormation[arid];
				arid = draw->PicFormation[arid];
				if (arid == 0xFF) continue;
				draw->bitmap[(arid << 6) + ((coY & 7) << 3) + (coX & 7)] = PaletteExchange[usepal][PixelLine[coX]];
			}
		}
		else {
			for (coX = (goodblocks << 2) - (skippixels >> 1) - 1; coX >= 0; coX--) {
				arid = ((coY >> 3) * draw->Width) + (coX >> 2);
				if (draw->PalFormation != nullptr) usepal = draw->PalFormation[arid];
				arid = draw->PicFormation[arid];
				if (arid == 0xFF) continue;
				draw->bitmap[(arid << 6) + ((coY & 7) << 3) + ((coX << 1) & 7)] = PaletteExchange[usepal][PixelLine[coX] >> 4];
				draw->bitmap[(arid << 6) + ((coY & 7) << 3) + (((coX << 1) + 1) & 7)] = PaletteExchange[usepal][PixelLine[coX] & 0x0F];
			}
		}
	}

	fclose(file);
}



// === FFH2Project versions

void DrawTile(CDC* dc, int tX, int tY, FFH2Project* proj, int offset, BYTE* palette, BYTE tint)
{
	BYTE pixel[2][2] = { 0,2,1,3 };
	BYTE coX, coY, line;
	for (coY = 0; coY < 8; coY++, tX -= 8, tY++, offset++) {
		for (coX = 0, line = 0x80; coX < 8; coX++, tX++, line >>= 1) {
			dc->SetPixelV(tX, tY, proj->palette[tint][palette[pixel
				[(proj->ROM[offset] & line) != 0]
			[(proj->ROM[offset + 8] & line) != 0]]]);
		}
	}
}
