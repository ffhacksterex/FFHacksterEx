#pragma once
#include <vector>

struct DRAW_STRUCT;
class CFFHacksterProject;
class FFH2Project;
struct DRAW_STRUCT;

BYTE Draw_ButtonDown(DRAW_STRUCT* draw, CPoint pt, bool rbutton);
bool Draw_MouseMove(DRAW_STRUCT* draw, CPoint pt);

void Draw_ROM_Buffer(CFFHacksterProject* cart, int offset, DRAW_STRUCT* draw);
void Draw_ROM_Buffer(FFH2Project* proj, int offset, DRAW_STRUCT* draw);

void Draw_Buffer_ROM(CFFHacksterProject* cart, int offset, DRAW_STRUCT* draw);
void Draw_Buffer_ROM(FFH2Project* proj, int offset, DRAW_STRUCT* draw);
//TODO- this is the same as th eoverload above, which passes a pointer to the project
void Draw_Buffer_ROM(std::vector<unsigned char>& rom, int offset, DRAW_STRUCT* draw);

void Draw_DrawGraphic(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal, bool drawbox, bool hideFF);
void Draw_DrawGraphic(CDC* mDC, DRAW_STRUCT* draw, FFH2Project* proj, BYTE* pal, bool drawbox, bool hideFF);

void Draw_DrawBufferTile(CDC* mDC, int pX, int pY, CFFHacksterProject* cart, DRAW_STRUCT* draw, BYTE* bitmap, BYTE* pal, int size);
void Draw_DrawBufferTile(CDC* mDC, int pX, int pY, FFH2Project* proj, DRAW_STRUCT* draw, BYTE* bitmap, BYTE* pal, int size);

void Draw_DrawCloseup(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal);
void Draw_DrawCloseup(CDC* mDC, DRAW_STRUCT* draw, FFH2Project* proj, BYTE* pal);

void Draw_DrawPalette(CDC* mDC, int pX, int pY, CFFHacksterProject* cart, BYTE* pal);
void Draw_DrawPalette(CDC* mDC, int pX, int pY, FFH2Project* proj, BYTE* pal);

void Draw_DrawPatternTables(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal, CRect rc, int max, bool drawbox = 1);
void Draw_DrawPatternTables(CDC* mDC, DRAW_STRUCT* draw, FFH2Project* cart, BYTE* pal, CRect rc, int max, bool drawbox = 1);

void Draw_DrawFinger(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart);
void Draw_DrawFinger(CDC* mDC, DRAW_STRUCT* draw, FFH2Project* proj);

void Draw_DrawAll(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal);

void Draw_ExportToBmp(DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal, CString initialfile = "");
void Draw_ExportToBmp(DRAW_STRUCT* draw, FFH2Project* cart, BYTE* pal, CString initialfile = "");

void Draw_ImportFromBmp(DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* palette, CString initialfile = "");
void Draw_ImportFromBmp(DRAW_STRUCT* draw, FFH2Project* cart, BYTE* palette, CString initialfile = "");

void DrawTile(CDC* dc, int tX, int tY, CFFHacksterProject* cart, int offset, BYTE* palette, BYTE tint = 0);
void DrawTile(CDC* dc, int tX, int tY, FFH2Project* proj, int offset, BYTE* palette, BYTE tint = 0);

void DrawTileScale(CDC* dc, int tX, int tY, CFFHacksterProject* cart, int offset, BYTE* palette, int scale, BYTE tint = 0);
