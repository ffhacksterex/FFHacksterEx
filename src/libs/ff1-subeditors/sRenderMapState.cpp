#include "stdafx.h"
#include "sRenderMapState.h"

bool sRenderMapState::IsValid() const
{
	return
		(project != nullptr) &&
		(owner != nullptr) &&
		(showrooms != nullptr) &&
		(ptLastClick != nullptr) &&
		(rcToolRect != nullptr) &&
		(cur_map != nullptr) &&
		(cur_tile != nullptr) &&
		(cur_tool != nullptr) &&
		(DecompressedMap != nullptr) &&
		mapdims.cx > 0 && mapdims.cy > 0 &&
		tiledims.cx > 0 && tiledims.cy > 0 &&
		//DEVNOTE - if the mapdims and/or tiledims must be square, 
		//	then enforce that in the client class's is_valid() instead.

		m_sprites != nullptr &&
		SPRITE_COUNT > 0 &&
		SPRITE_PICASSIGNMENT > 0 &&

		(Sprite_Coords != nullptr &&
			Sprite_Coords->size() == SPRITE_COUNT) &&
		(Sprite_InRoom != nullptr &&
			Sprite_InRoom->size() == SPRITE_COUNT) &&
		(Sprite_StandStill != nullptr &&
			Sprite_StandStill->size() == SPRITE_COUNT) &&
		(Sprite_Value != nullptr &&
			Sprite_Value->size() == SPRITE_COUNT);
}