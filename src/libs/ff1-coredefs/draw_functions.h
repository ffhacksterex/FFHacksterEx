#pragma once

struct DRAW_STRUCT;
class CFFHacksterProject;
struct DRAW_STRUCT;

BYTE Draw_ButtonDown(DRAW_STRUCT* draw, CPoint pt, bool rbutton);
bool Draw_MouseMove(DRAW_STRUCT* draw, CPoint pt);

void Draw_ROM_Buffer(CFFHacksterProject* cart, int offset, DRAW_STRUCT* draw);
void Draw_Buffer_ROM(CFFHacksterProject* cart, int offset, DRAW_STRUCT* draw);
void Draw_DrawGraphic(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal, bool drawbox, bool hideFF);
void Draw_DrawBufferTile(CDC* mDC, int pX, int pY, CFFHacksterProject* cart, DRAW_STRUCT* draw, BYTE* bitmap, BYTE* pal, int size);
void Draw_DrawCloseup(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal);
void Draw_DrawPalette(CDC* mDC, int pX, int pY, CFFHacksterProject* cart, BYTE* pal);
void Draw_DrawPatternTables(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal, CRect rc, int max, bool drawbox = 1);
void Draw_DrawFinger(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart);
void Draw_DrawAll(CDC* mDC, DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal);

void Draw_ExportToBmp(DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* pal);
void Draw_ImportFromBmp(DRAW_STRUCT* draw, CFFHacksterProject* cart, BYTE* palette);

void DrawTile(CDC* dc, int tX, int tY, CFFHacksterProject* cart, int offset, BYTE* palette, BYTE tint = 0);
void DrawTileScale(CDC* dc, int tX, int tY, CFFHacksterProject* cart, int offset, BYTE* palette, int scale, BYTE tint = 0);
