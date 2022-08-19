/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "green.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Green character structure
enum
{
	Green_ArcMain_Hit0,
	Green_ArcMain_Hit1,
	Green_ArcMain_Hit2,
	
	Green_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Green_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	//Bike
	Gfx_Tex tex_brear, tex_bfront, tex_sandblast;

	//sandblast
	u8 sand_frame, sand_tex_id;
	Animatable sand_animatable;
} Char_Green;

static const CharFrame sand_frame[3] = {
	{0, {  0,   0 + 72,  61, 65}, {0,  0}},
	{0, { 62,   0 + 72,  59, 52}, {7,-15}},
	{0, {122,   0 + 72,  60, 46}, {9,-21}},
};

static const Animation sand_anim[1] = {
	{1, (const u8[]){0, 1, 2, ASCR_REPEAT}},
};

void Bike_Sand_Draw(Char_Green *this, fixed_t x, fixed_t y)
{
	//Draw animated object
	const CharFrame *cframe = &sand_frame[this->sand_frame];
	
	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Stage_DrawTex(&this->tex_sandblast, &src, &dst, stage.camera.bzoom);
}

void Bike_Sand_SetFrame(void *user, u8 frame)
{
	Char_Green *this = (Char_Green*)user;
	
	//Check if this is a new frame
	if (frame != this->sand_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &sand_frame[this->sand_frame = frame];
	}
}

//Green character definitions
static const CharFrame char_green_frame[] = {
	{Green_ArcMain_Hit0, {  0,   0,  67,  98}, {  0,   0}}, //0
	{Green_ArcMain_Hit0, { 68,   0,  67, 100}, {  0,   2}}, //1
	{Green_ArcMain_Hit0, {136,   0,  67, 101}, {  0,   3}}, //2
	{Green_ArcMain_Hit0, {  0, 102,  67, 101}, {  0,   3}}, //3
	{Green_ArcMain_Hit0, { 68, 102,  67, 101}, {  0,   3}}, //4

	{Green_ArcMain_Hit0, {136, 102,  62, 104}, {  0,   4}}, //5
	{Green_ArcMain_Hit1, {  0,   0,  63, 105}, {  0,   5}}, //6
	{Green_ArcMain_Hit1, { 64,   0,  63, 105}, {  0,   5}}, //7

	{Green_ArcMain_Hit1, {128,   0,  69,  95}, {  1,  -3}}, //8
	{Green_ArcMain_Hit1, {  0, 106,  69,  97}, {  1,  -1}}, //9
	{Green_ArcMain_Hit1, { 70, 106,  69,  97}, {  1,  -1}}, //10

	{Green_ArcMain_Hit1, {140, 106,  63, 106}, { -1,   7}}, //11
	{Green_ArcMain_Hit2, {  0,   0,  64, 105}, { -1,   6}}, //12
	{Green_ArcMain_Hit2, { 65,   0,  65, 105}, { -1,   6}}, //13

	{Green_ArcMain_Hit2, {131,   0,  68, 103}, {  0,   3}}, //14
	{Green_ArcMain_Hit2, {  0, 106,  67, 103}, {  0,   3}}, //15
};

static const Animation char_green_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 7, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 8, 9, 10, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 11, 12, 13, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){14, 15, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Green character functions
void Char_Green_SetFrame(void *user, u8 frame)
{
	Char_Green *this = (Char_Green*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_green_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Green_Tick(Character *character)
{
	Char_Green *this = (Char_Green*)character;

	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	//Draw Bike Front
	RECT bfront_src = {0, 0, 81, 68};
	RECT_FIXED bfront_dst = {
		this->character.x + FIXED_DEC(39,1) - fx,
		this->character.y + FIXED_DEC(39,1) - fy,
		FIXED_DEC(bfront_src.w,1),
		FIXED_DEC(bfront_src.h,1)
	};
	Stage_DrawTex(&this->tex_bfront, &bfront_src, &bfront_dst, stage.camera.bzoom);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Green_SetFrame);
	Character_Draw(character, &this->tex, &char_green_frame[this->frame]);

	//Draw Bike Rear
	RECT brear_src = {0, 0, 124, 68};
	RECT_FIXED brear_dst = {
		this->character.x + FIXED_DEC(-50,1) - fx,
		this->character.y + FIXED_DEC(30,1) - fy,
		FIXED_DEC(brear_src.w,1),
		FIXED_DEC(brear_src.h,1)
	};
	Stage_DrawTex(&this->tex_brear, &brear_src, &brear_dst, stage.camera.bzoom);

	//Draw Bike Sandblast
	Animatable_Animate(&this->sand_animatable, (void*)this, Bike_Sand_SetFrame);
	Bike_Sand_Draw(this, this->character.x + FIXED_DEC(-70,1) - fx, this->character.y + FIXED_DEC(30,1) - fy);
}

void Char_Green_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Green_Free(Character *character)
{
	Char_Green *this = (Char_Green*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GreenBikerDude_New(fixed_t x, fixed_t y)
{
	//Allocate green object
	Char_Green *this = Mem_Alloc(sizeof(Char_Green));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Green_New] Failed to allocate green object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Green_Tick;
	this->character.set_anim = Char_Green_SetAnim;
	this->character.free = Char_Green_Free;
	
	Animatable_Init(&this->character.animatable, char_green_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(14,1);
	this->character.focus_y = FIXED_DEC(35,1);
	this->character.focus_zoom = 1534;
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GREEN.ARC;1");
	
	const char **pathp = (const char *[]){
		"hit0.tim",
		"hit1.tim",
		"hit2.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);

	Gfx_LoadTex(&this->tex_brear, IO_Read("\\CHAR\\BREAR.TIM;1"), GFX_LOADTEX_FREE);
	Gfx_LoadTex(&this->tex_bfront, IO_Read("\\CHAR\\BFRONT.TIM;1"), GFX_LOADTEX_FREE);
	Gfx_LoadTex(&this->tex_sandblast, IO_Read("\\CHAR\\SBLAST.TIM;1"), GFX_LOADTEX_FREE);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;

	this->character.hr = 204;
	this->character.hg = 204;
	this->character.hb = 51;

	Animatable_Init(&this->sand_animatable, sand_anim);
	Animatable_SetAnim(&this->sand_animatable, 0);
	
	return (Character*)this;
}
