#include "simp.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"

//Week 1 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_shop; //Sneed's Feed & Seed (Formely Chuck's)
	Gfx_Tex tex_field; //field
} Back_Simp;

//Week 1 background functions
void Back_Simp_DrawBG(StageBack *back)
{
	Back_Simp *this = (Back_Simp*)back;
	
	fixed_t fx, fy;
	
	//Draw the shop
	fx = stage.camera.x;
	fy = stage.camera.y;

	RECT shop_src = {0, 0, 256, 206};
	RECT_FIXED shop_dst = {
		FIXED_DEC(-250,1) - fx,
		FIXED_DEC(-200,1) - fy,
		FIXED_DEC(312,1),
		FIXED_DEC(216,1)
	};
	
	Stage_DrawTex(&this->tex_shop, &shop_src, &shop_dst, stage.camera.bzoom);

	//Draw field
	RECT field_src = {0, 0, 256, 206};
	RECT_FIXED field_dst = {
		FIXED_DEC(-155,1) - fx,
		shop_dst.y,
		FIXED_DEC(256,1),
		FIXED_DEC(216,1)
	};

	Stage_DrawTex(&this->tex_field, &field_src, &field_dst, stage.camera.bzoom);
}

void Back_Simp_Free(StageBack *back)
{
	Back_Simp *this = (Back_Simp*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Simp_New(void)
{
	//Allocate background structure
	Back_Simp *this = (Back_Simp*)Mem_Alloc(sizeof(Back_Simp));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_overlay = NULL;
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Simp_DrawBG;
	this->back.free = Back_Simp_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\SNEED\\SIMP.ARC;1");
	Gfx_LoadTex(&this->tex_shop, Archive_Find(arc_back, "shop.tim"), 0);
	Gfx_LoadTex(&this->tex_field, Archive_Find(arc_back, "field.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
