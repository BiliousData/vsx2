/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfz.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"
#include "hand.h"

//BFZ character structure
enum
{
	BFZ_ArcMain_Hit,
	
	BFZ_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFZ_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	Character *hand;
} Char_BFZ;

boolean visible;

//BFZ character definitions
static const CharFrame char_bfz_frame[] = {
	{BFZ_ArcMain_Hit, {  0,   0,  50,  49}, {  0,   0}}, //0
	{BFZ_ArcMain_Hit, { 51,   0,  51,  49}, {  0,   0}}, //1
	{BFZ_ArcMain_Hit, {103,   0,  50,  50}, {  0,   1}}, //2
	{BFZ_ArcMain_Hit, {154,   0,  50,  50}, {  0,   1}}, //3
	{BFZ_ArcMain_Hit, {205,   0,  49,  50}, {  0,   1}}, //4
	{BFZ_ArcMain_Hit, {  0,  51,  49,  50}, {  0,   1}}, //5

	{BFZ_ArcMain_Hit, { 50,  51,  47,  51}, {  2,   2}}, //6
	{BFZ_ArcMain_Hit, { 98,  51,  48,  51}, {  2,   2}}, //7

	{BFZ_ArcMain_Hit, {147,  51,  49,  50}, {  1,   1}}, //8
	{BFZ_ArcMain_Hit, {197,  51,  47,  50}, {  0,   1}}, //9

	{BFZ_ArcMain_Hit, {  0, 103,  47,  51}, { -5,   2}}, //10
	{BFZ_ArcMain_Hit, { 48, 103,  47,  51}, { -5,   2}}, //11

	{BFZ_ArcMain_Hit, { 96, 103,  49,  50}, { -5,   1}}, //12
	{BFZ_ArcMain_Hit, {146, 103,  50,  51}, { -4,   2}}, //13
	
};

static const Animation char_bfz_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 7, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 8, 9, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 10, 11, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){12, 13, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//BFZ character functions
void Char_BFZ_SetFrame(void *user, u8 frame)
{
	Char_BFZ *this = (Char_BFZ*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfz_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFZ_Tick(Character *character)
{
	Char_BFZ *this = (Char_BFZ*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_BFZ_SetFrame);
	if (visible == true)
		Character_DrawScale(character, &this->tex, &char_bfz_frame[this->frame], 2, 2);
	//Tick hand
	this->hand->tick(this->hand);

	//hand animation shit
	if (stage.player->animatable.anim == CharAnim_Idle)
	{
		if (stage.opponent->animatable.anim == CharAnim_Down || stage.opponent->animatable.anim == CharAnim_DownAlt)
		{
			this->hand->set_anim(this->hand, CharAnim_Down);
			visible = false;
		}
		else if (stage.opponent->animatable.anim == CharAnim_Left || stage.opponent->animatable.anim == CharAnim_LeftAlt)
		{
			this->hand->set_anim(this->hand, CharAnim_Left);
			visible = false;
		}
		else if (stage.opponent->animatable.anim == CharAnim_Up || stage.opponent->animatable.anim == CharAnim_UpAlt)
		{
			this->hand->set_anim(this->hand, CharAnim_Up);
			visible = false;
		}
		else if (stage.opponent->animatable.anim == CharAnim_Right || stage.opponent->animatable.anim == CharAnim_RightAlt)
		{
			this->hand->set_anim(this->hand, CharAnim_Right);
			visible = false;
		}
		else if (stage.opponent->animatable.anim == CharAnim_Idle && this->hand->animatable.anim != CharAnim_Idle)
		{
			this->hand->set_anim(this->hand, CharAnim_Idle);
			visible = true;
		}
	}
	else
	{
		if (stage.opponent->animatable.anim == CharAnim_Down || stage.opponent->animatable.anim == CharAnim_DownAlt)
		{
			this->hand->set_anim(this->hand, CharAnim_DownAlt);
			visible = true;
		}
		else if (stage.opponent->animatable.anim == CharAnim_Left || stage.opponent->animatable.anim == CharAnim_LeftAlt)
		{
			this->hand->set_anim(this->hand, CharAnim_LeftAlt);
			visible = true;
		}
		else if (stage.opponent->animatable.anim == CharAnim_Up || stage.opponent->animatable.anim == CharAnim_UpAlt)
		{
			this->hand->set_anim(this->hand, CharAnim_UpAlt);
			visible = true;
		}
		else if (stage.opponent->animatable.anim == CharAnim_Right || stage.opponent->animatable.anim == CharAnim_RightAlt)
		{
			this->hand->set_anim(this->hand, CharAnim_RightAlt);
			visible = true;
		}
	}
}

void Char_BFZ_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFZ_Free(Character *character)
{
	Char_BFZ *this = (Char_BFZ*)character;
	
	//Free art
	Mem_Free(this->arc_main);
	//free hand
	Character_Free(this->hand);
}

Character *Char_BFZ_New(fixed_t x, fixed_t y)
{
	//Allocate bfz object
	Char_BFZ *this = Mem_Alloc(sizeof(Char_BFZ));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFZ_New] Failed to allocate bfz object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFZ_Tick;
	this->character.set_anim = Char_BFZ_SetAnim;
	this->character.free = Char_BFZ_Free;
	
	Animatable_Init(&this->character.animatable, char_bfz_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 0;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);

	//HAND GOES TO X 140 Y 163
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BFZ.ARC;1");
	
	const char **pathp = (const char *[]){
		"hit.tim", //BFZ_ArcMain_Idle0
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	//Start loading Hand
	this->hand = Char_Hand_New(FIXED_DEC(140,1), FIXED_DEC(163,1));

	this->character.hr = 175;
	this->character.hg = 102;
	this->character.hb = 206;
	visible = true;
	
	return (Character*)this;
}
