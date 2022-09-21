/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "valley.h"
#include "../stage.h"
#include "../archive.h"
#include "../animation.h"

#include "../mem.h"

//Valley background structure
typedef struct
{
	//Stage background base structure
	StageBack back;

	//Textures
	Gfx_Tex tex_h0, tex_h1; //Shack
	Gfx_Tex tex_g0, tex_g1; //ground
} Back_Valley;

//Week 1 background functions
void Back_Valley_DrawBG(StageBack *back)
{
	Back_Valley *this = (Back_Valley*)back;
	
	fixed_t fx, fy;

	//draw shack
	fx = stage.camera.x;
	fy = stage.camera.y;

	RECT shack0_src = {0, 0, 256, 251};
	RECT_FIXED shack0_dst = {
		FIXED_DEC(352,1) - fx,
		FIXED_DEC(265,1) - fy,
		FIXED_DEC(shack0_src.w,1),
		FIXED_DEC(shack0_src.h,1)
	};

	RECT shack1_src = {0, 0, 119, 251};
	RECT_FIXED shack1_dst = {
		shack0_dst.x + FIXED_DEC(shack0_src.w,1) - FIXED_DEC(1,1),
		shack0_dst.y,
		FIXED_DEC(shack1_src.w,1),
		FIXED_DEC(shack1_src.h,1)
	};

	Stage_DrawTex(&this->tex_h0, &shack0_src, &shack0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_h1, &shack1_src, &shack1_dst, stage.camera.bzoom);

	//draw ground
	RECT ground0_src = {0, 0, 256, 256};
	RECT_FIXED ground0_dst = {
		FIXED_DEC(298,1) - fx,
		FIXED_DEC(374,1) - fy,
		FIXED_DEC(ground0_src.w,1),
		FIXED_DEC(ground0_src.h,1)
	};

	RECT ground1_src = {0, 0, 247, 256};
	RECT_FIXED ground1_dst = {
		ground0_dst.x + FIXED_DEC(ground0_src.w,1) - FIXED_DEC(1,1),
		ground0_dst.y,
		FIXED_DEC(ground1_src.w,1),
		FIXED_DEC(ground1_src.h,1)
	};

	Stage_DrawTex(&this->tex_g0, &ground0_src, &ground0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_g1, &ground1_src, &ground1_dst, stage.camera.bzoom);
}

//Valley background functions
void Back_Valley_Free(StageBack *back)
{
	Back_Valley *this = (Back_Valley*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Valley_New(void)
{
	//Allocate background structure
	Back_Valley *this = (Back_Valley*)Mem_Alloc(sizeof(Back_Valley));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_overlay = NULL;
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Valley_DrawBG;
	this->back.free = Back_Valley_Free;

	//Load background textures
	IO_Data arc_back = IO_Read("\\MOTHER2\\SHACK.ARC;1");
	Gfx_LoadTex(&this->tex_h0, Archive_Find(arc_back, "h0.tim"), 0);
	Gfx_LoadTex(&this->tex_h1, Archive_Find(arc_back, "h1.tim"), 0);
	Gfx_LoadTex(&this->tex_g0, Archive_Find(arc_back, "g0.tim"), 0);
	Gfx_LoadTex(&this->tex_g1, Archive_Find(arc_back, "g1.tim"), 0);
	Mem_Free(arc_back);
	
	//Use non-pitch black background
	Gfx_SetClear(222, 216, 150);
	
	return (StageBack*)this;
}
