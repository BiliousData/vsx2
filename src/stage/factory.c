/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "factory.h"

#include "../mem.h"

//Factory background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
} Back_Factory;

void Back_Factory_DrawBg(StageBack *back)
{
	Back_Factory *this = (Back_Factory*)back;

	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;

	RECT ground = {stage.gf->x - fx, stage.gf->y - fy, FIXED_DEC(500,1), FIXED_DEC(100,1)};

	Gfx_DrawRect(&ground, 197, 155, 119);
}

//Factory background functions
void Back_Factory_Free(StageBack *back)
{
	Back_Factory *this = (Back_Factory*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Factory_New(void)
{
	//Allocate background structure
	Back_Factory *this = (Back_Factory*)Mem_Alloc(sizeof(Back_Factory));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_overlay = NULL;
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Factory_DrawBg;
	this->back.free = Back_Factory_Free;
	
	//Use non-pitch black background
	Gfx_SetClear(41, 25, 57);
	
	return (StageBack*)this;
}
