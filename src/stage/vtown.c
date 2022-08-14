/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "vtown.h"
#include "../stage.h"
#include "../archive.h"
#include "../animation.h"

#include "../mem.h"

//VTown background structure
typedef struct
{
	//Stage background base structure
	StageBack back;

	IO_Data arc_hiro, arc_hiro_ptr[3];

	//Textures
	Gfx_Tex tex_t0, tex_t1, tex_t2; //town
	Gfx_Tex tex_m0, tex_m1, tex_m2; //mountains
	Gfx_Tex tex_g; //ground
	Gfx_Tex tex_yot; //Yotsuba
	Gfx_Tex tex_bait;

	//̶m̶o̶o̶t hiro
	Gfx_Tex tex_hiro;
	u8 hiro_frame, hiro_tex_id;

	Animatable hiro_animatable;
} Back_VTown;

//weird planet thing frame data
static const CharFrame hiro_frame[5] = {
	{0, {  0,   0, 153,105}, {0,  0}},
	{0, {  0, 105, 149,104}, {-2,  -2}},
	{1, {  0,   0, 148,105}, {-2,  1}},
	{1, {  0, 105, 148,105}, {-2,  1}},
	{2, {  0,   0, 148,105}, {-2,  1}},
};

//planet animation
static const Animation hiro_anim[1] = {
	{2, (const u8[]){0, 1, 2, 3, 4, ASCR_BACK, 1}}, //idle
};

//FireL functions
void VTown_Hiro_SetFrame(void *user, u8 frame)
{
	Back_VTown *this = (Back_VTown*)user;
	
	//Check if this is a new frame
	if (frame != this->hiro_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &hiro_frame[this->hiro_frame = frame];
		if (cframe->tex != this->hiro_tex_id)
			Gfx_LoadTex(&this->tex_hiro, this->arc_hiro_ptr[this->hiro_tex_id = cframe->tex], 0);
	}
}

void VTown_Hiro_Draw(Back_VTown *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &hiro_frame[this->hiro_frame];
	
	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Stage_DrawTex(&this->tex_hiro, &src, &dst, stage.camera.bzoom);
}

//Week 1 background functions
void Back_VTown_DrawBG(StageBack *back)
{
	Back_VTown *this = (Back_VTown*)back;
	
	fixed_t fx, fy;

	//draw ground
	fx = stage.camera.x;
	fy = stage.camera.y;

	RECT ground_src = {0, 0, 256, 182};
	RECT_FIXED ground_dst = {
		FIXED_DEC(352,1) - fx,
		FIXED_DEC(265,1) - fy,
		FIXED_DEC(848,1),
		FIXED_DEC(ground_src.h,1)
	};

	Stage_DrawTex(&this->tex_g, &ground_src, &ground_dst, stage.camera.bzoom);

	//Draw town
	fx = stage.camera.x * 2 / 3;
	fy = stage.camera.y * 2 / 3;

	RECT town_src1 = {0, 0, 256, 256};
	RECT town_src2 = {0, 0, 244, 256};
	RECT_FIXED town_dst1 = {
		FIXED_DEC(0,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(town_src1.w,1),
		FIXED_DEC(town_src1.h,1)
	};

	RECT_FIXED town_dst2 = {
		town_dst1.x + FIXED_DEC(town_src1.w,1) - FIXED_DEC(1,1),
		town_dst1.y,
		FIXED_DEC(town_src1.w,1),
		FIXED_DEC(town_src1.h,1)
	};

	RECT_FIXED town_dst3 = {
		town_dst1.x + FIXED_DEC(town_src1.w,1) - FIXED_DEC(1,1) + FIXED_DEC(town_src1.w,1) - FIXED_DEC(1,1),
		town_dst1.y,
		FIXED_DEC(town_src2.w,1),
		FIXED_DEC(town_src2.h,1)
	};

	RECT yot_watch_src = {0, 0, 16, 34};
	RECT_FIXED yot_watch_dst = {
		FIXED_DEC(395,1) - fx,
		FIXED_DEC(128,1) - fy,
		FIXED_DEC(yot_watch_src.w,1),
		FIXED_DEC(yot_watch_src.h,1)
	};

	RECT yot_relax_src = {16, 0, 33, 21};
	RECT_FIXED yot_relax_dst = {
		FIXED_DEC(383,1) - fx,
		FIXED_DEC(148,1) - fy,
		FIXED_DEC(yot_relax_src.w,1),
		FIXED_DEC(yot_relax_src.h,1)
	};


	if (stage.stage_id == StageId_1_1)
		Stage_DrawTex(&this->tex_yot, &yot_watch_src, &yot_watch_dst, stage.camera.bzoom);
	if (stage.stage_id == StageId_1_2)
		Stage_DrawTex(&this->tex_yot, &yot_relax_src, &yot_relax_dst, stage.camera.bzoom);


	Stage_DrawTex(&this->tex_t0, &town_src1, &town_dst1, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_t1, &town_src1, &town_dst2, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_t2, &town_src2, &town_dst3, stage.camera.bzoom);
	
	//Draw stage
	//fx = stage.camera.x;
	//fy = stage.camera.y;
	//
	//RECT stage_src = {0, 0, 256, 59};
	//RECT_FIXED stage_dst = {
	//	FIXED_DEC(-230,1) - FIXED_DEC(SCREEN_WIDEOADD,2) - fx,
	//	FIXED_DEC(50,1) - fy,
	//	FIXED_DEC(410,1) + FIXED_DEC(SCREEN_WIDEOADD,1),
	//	FIXED_DEC(106,1)
	//};
	//
	//Stage_DrawTex(&this->tex_back0, &stage_src, &stage_dst, stage.camera.bzoom);
	
	//Draw mountains
	fx = stage.camera.x * 1 / 3;
	fy = stage.camera.y * 2 / 3;
	
	RECT mount_src1 = {0, 0, 256, 256};
	RECT mount_src2 = {0, 0, 58, 256};
	RECT_FIXED mount_dst1 = {
		FIXED_DEC(-103,1) - fx,
		FIXED_DEC(59,1) - fy,
		FIXED_DEC(mount_src1.w,1),
		FIXED_DEC(mount_src1.h,1)
	};

	RECT_FIXED mount_dst2 = {
		mount_dst1.x + FIXED_DEC(mount_src1.w,1) - FIXED_DEC(1,1),
		mount_dst1.y,
		FIXED_DEC(mount_src1.w,1),
		FIXED_DEC(mount_src1.h,1)
	};

	RECT_FIXED mount_dst3 = {
		mount_dst1.x + FIXED_DEC(mount_src1.w,1) - FIXED_DEC(1,1) + FIXED_DEC(mount_src1.w,1) - FIXED_DEC(1,1),
		mount_dst1.y,
		FIXED_DEC(mount_src2.w,1),
		FIXED_DEC(mount_src2.h,1)
	};

	//draw hiro
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		if (stage.song_step & 0x3)
			Animatable_SetAnim(&this->hiro_animatable, 0);
	}
	Animatable_Animate(&this->hiro_animatable, (void*)this, VTown_Hiro_SetFrame);

	VTown_Hiro_Draw(this, FIXED_DEC(141,1) - fx, FIXED_DEC(-6,1) - fy);

	Stage_DrawTex(&this->tex_m0, &mount_src1, &mount_dst1, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_m1, &mount_src1, &mount_dst2, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_m2, &mount_src2, &mount_dst3, stage.camera.bzoom);
}

//Sage bait
void Back_VTown_DrawBait(StageBack *back)
{
	Back_VTown *this = (Back_VTown*)back;
	
	fixed_t fx, fy;

	RECT fnfkek_src = {0, 0 + 72, 75, 60};
	RECT kingdude_src = {76, 0 + 72, 70, 60};
	RECT you_src = {147, 0 + 72, 70, 60};
	RECT ninja_src = {0, 61 + 72, 64, 60};
	RECT madness_src = {65, 61 + 72, 64, 60};
	RECT psx_src = {130, 61 + 72, 75, 60};
	RECT fnfsucks_src = {0, 122 + 72, 56, 56};
	RECT kade_src = {57, 122 + 72, 70, 60};
	//yes the whitty one is missing
	//no im not adding it

	RECT_FIXED fnfkek_dst = {
		FIXED_DEC(0,1),
		FIXED_DEC(1,1),
		FIXED_DEC(fnfkek_src.w,1),
		FIXED_DEC(fnfkek_src.h,1),
	};

	RECT_FIXED kingdude_dst = {
		FIXED_DEC(89,1),
		FIXED_DEC(42,1),
		FIXED_DEC(kingdude_src.w,1),
		FIXED_DEC(kingdude_src.h,1),
	};

	RECT_FIXED you_dst = {
		FIXED_DEC(8,1),
		FIXED_DEC(-112,1),
		FIXED_DEC(you_src.w,1),
		FIXED_DEC(you_src.h,1),
	};

	RECT_FIXED ninja_dst = {
		FIXED_DEC(43,1),
		FIXED_DEC(-1,1),
		FIXED_DEC(ninja_src.w,1),
		FIXED_DEC(ninja_src.h,1),
	};

	RECT_FIXED madness_dst = {
		FIXED_DEC(80,1),
		FIXED_DEC(-114,1),
		FIXED_DEC(madness_src.w,1),
		FIXED_DEC(madness_src.h,1),
	};

	RECT_FIXED psx_dst = {
		FIXED_DEC(-6,1),
		FIXED_DEC(-43,1),
		FIXED_DEC(psx_src.w,1),
		FIXED_DEC(psx_src.h,1),
	};

	RECT_FIXED fnfsucks_dst = {
		FIXED_DEC(13,1),
		FIXED_DEC(35,1),
		FIXED_DEC(fnfsucks_src.w,1),
		FIXED_DEC(fnfsucks_src.h,1),
	};

	RECT_FIXED kade_dst = {
		FIXED_DEC(86,1),
		FIXED_DEC(-8,1),
		FIXED_DEC(kade_src.w,1),
		FIXED_DEC(kade_src.h,1),
	};

	if (stage.sagecount >= 8)
		Stage_DrawTex(&this->tex_bait, &kade_src, &kade_dst, stage.camera.bzoom);
	if (stage.sagecount >= 7)
		Stage_DrawTex(&this->tex_bait, &fnfsucks_src, &fnfsucks_dst, stage.camera.bzoom);
	if (stage.sagecount >= 6)
		Stage_DrawTex(&this->tex_bait, &psx_src, &psx_dst, stage.camera.bzoom);
	if (stage.sagecount >= 5)
		Stage_DrawTex(&this->tex_bait, &madness_src, &madness_dst, stage.camera.bzoom);
	if (stage.sagecount >= 4)
		Stage_DrawTex(&this->tex_bait, &ninja_src, &ninja_dst, stage.camera.bzoom);
	if (stage.sagecount >= 3)
		Stage_DrawTex(&this->tex_bait, &you_src, &you_dst, stage.camera.bzoom);
	if (stage.sagecount >= 2)
		Stage_DrawTex(&this->tex_bait, &kingdude_src, &kingdude_dst, stage.camera.bzoom);
	if (stage.sagecount >= 1)
		Stage_DrawTex(&this->tex_bait, &fnfkek_src, &fnfkek_dst, stage.camera.bzoom);

	
	
}

//VTown background functions
void Back_VTown_Free(StageBack *back)
{
	Back_VTown *this = (Back_VTown*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_VTown_New(void)
{
	//Allocate background structure
	Back_VTown *this = (Back_VTown*)Mem_Alloc(sizeof(Back_VTown));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_overlay = Back_VTown_DrawBait;
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_VTown_DrawBG;
	this->back.free = Back_VTown_Free;

	//Load background textures
	IO_Data arc_back = IO_Read("\\V\\TOWN.ARC;1");
	Gfx_LoadTex(&this->tex_t0, Archive_Find(arc_back, "t0.tim"), 0);
	Gfx_LoadTex(&this->tex_t1, Archive_Find(arc_back, "t1.tim"), 0);
	Gfx_LoadTex(&this->tex_t2, Archive_Find(arc_back, "t2.tim"), 0);
	Gfx_LoadTex(&this->tex_m0, Archive_Find(arc_back, "m0.tim"), 0);
	Gfx_LoadTex(&this->tex_m1, Archive_Find(arc_back, "m1.tim"), 0);
	Gfx_LoadTex(&this->tex_m2, Archive_Find(arc_back, "m2.tim"), 0);
	Gfx_LoadTex(&this->tex_g, Archive_Find(arc_back, "g.tim"), 0);
	Gfx_LoadTex(&this->tex_yot, Archive_Find(arc_back, "yot.tim"), 0);
	Gfx_LoadTex(&this->tex_bait, Archive_Find(arc_back, "bait.tim"), 0);
	Mem_Free(arc_back);

	//load moot
	this->arc_hiro = IO_Read("\\V\\HIRO.ARC;1");
	this->arc_hiro_ptr[0] = Archive_Find(this->arc_hiro, "hiro0.tim");
	this->arc_hiro_ptr[1] = Archive_Find(this->arc_hiro, "hiro1.tim");
	this->arc_hiro_ptr[2] = Archive_Find(this->arc_hiro, "hiro2.tim");
	
	//Use white background
	Gfx_SetClear(255, 255, 255);

	Animatable_Init(&this->hiro_animatable, hiro_anim);
	Animatable_SetAnim(&this->hiro_animatable, 0);
	this->hiro_frame = this->hiro_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
