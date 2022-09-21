/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfv.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//BFV character structure
enum
{
	BFV_ArcMain_Hit,
	
	BFV_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFV_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BFV;

//BFV character definitions
static const CharFrame char_bfv_frame[] = {
	{BFV_ArcMain_Hit, {  0,   0,  57,  59}, {  0,   0}}, //0
	{BFV_ArcMain_Hit, { 58,   0,  57,  59}, {  0,   0}}, //1
	{BFV_ArcMain_Hit, {116,   0,  57,  60}, {  0,   1}}, //2
	{BFV_ArcMain_Hit, {174,   0,  57,  62}, {  0,   3}}, //3
	{BFV_ArcMain_Hit, {  0,  63,  57,  62}, {  0,   3}}, //4

	{BFV_ArcMain_Hit, { 58,  63,  57,  61}, {  0,   1}}, //5
	{BFV_ArcMain_Hit, {116,  63,  56,  61}, { -1,   1}}, //6

	{BFV_ArcMain_Hit, {173,  63,  58,  54}, { -2,  -5}}, //7
	{BFV_ArcMain_Hit, {  0, 126,  57,  56}, { -2,  -4}}, //8

	{BFV_ArcMain_Hit, { 58, 126,  50,  65}, { -8,   6}}, //9
	{BFV_ArcMain_Hit, {109, 126,  50,  64}, { -7,   5}}, //10

	{BFV_ArcMain_Hit, {160, 126,  55,  60}, { -8,   1}}, //11
	{BFV_ArcMain_Hit, {  0, 192,  54,  60}, { -8,   1}}, //12
};

static const Animation char_bfv_anim[CharAnim_Max] = {
	{2, (const u8[]){0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){5, 6, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){7, 8, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){9, 10, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){11, 12, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//BFV character functions
void Char_BFV_SetFrame(void *user, u8 frame)
{
	Char_BFV *this = (Char_BFV*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfv_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFV_Tick(Character *character)
{
	Char_BFV *this = (Char_BFV*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_BFV_SetFrame);
	Character_Draw(character, &this->tex, &char_bfv_frame[this->frame]);
}

void Char_BFV_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFV_Free(Character *character)
{
	Char_BFV *this = (Char_BFV*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFV_New(fixed_t x, fixed_t y)
{
	//Allocate bfv object
	Char_BFV *this = Mem_Alloc(sizeof(Char_BFV));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFV_New] Failed to allocate bfv object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFV_Tick;
	this->character.set_anim = Char_BFV_SetAnim;
	this->character.free = Char_BFV_Free;
	
	Animatable_Init(&this->character.animatable, char_bfv_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(0,1);
	this->character.focus_y = FIXED_DEC(0,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BFV.ARC;1");
	
	const char **pathp = (const char *[]){
		"hit.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;

	this->character.hr = 175;
	this->character.hg = 102;
	this->character.hb = 206;
	
	return (Character*)this;
}
