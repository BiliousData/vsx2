/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "sneed.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Sneed character structure
enum
{
	Sneed_ArcMain_Idle0,
	Sneed_ArcMain_Idle1,
	Sneed_ArcMain_Idle2,
	Sneed_ArcMain_Left,
	Sneed_ArcMain_Down0,
	Sneed_ArcMain_Down1,
	Sneed_ArcMain_Up0,
	Sneed_ArcMain_Up1,
	Sneed_ArcMain_Right,
	
	Sneed_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Sneed_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Sneed;

//Sneed character definitions
static const CharFrame char_sneed_frame[] = {
	{Sneed_ArcMain_Idle0, {  0,   0, 108, 154}, {  0, 0}}, //0 idle 1
	{Sneed_ArcMain_Idle0, {108,   0, 109, 156}, {  0, 2}}, //1 idle 1
	{Sneed_ArcMain_Idle1, {  0,   0, 110, 156}, {  0, 2}}, //2 idle 1
	{Sneed_ArcMain_Idle1, {110,   0, 110, 156}, {  0, 2}}, //3 idle 1
	{Sneed_ArcMain_Idle2, {  0,   0, 111, 156}, {  0, 2}}, //4 idle 1

	{Sneed_ArcMain_Left, {  0,   0, 112, 157}, {  0, 3}}, //5 left 1
	{Sneed_ArcMain_Left, {112,   0, 111, 157}, {  0, 3}}, //6 left 1

	{Sneed_ArcMain_Down0, {  0,   0, 113, 153}, {  0,-1}}, //7 down 1
	{Sneed_ArcMain_Down0, {113,   0, 114, 154}, {  0, 0}}, //8 down 1
	{Sneed_ArcMain_Down1, {  0,   0, 114, 154}, {  0, 0}}, //9 down 1

	{Sneed_ArcMain_Up0, {  0,   0, 114, 158}, {  0, 4}}, //10 up 1
	{Sneed_ArcMain_Up0, {114,   0, 113, 157}, {  0, 3}}, //11 up 1
	{Sneed_ArcMain_Up1, {  0,   0, 113, 157}, {  0, 3}}, //12 up 1

	{Sneed_ArcMain_Right, {  0,   0, 125, 158}, {  0, 4}}, //13 right 1
	{Sneed_ArcMain_Right, {125,   0, 122, 158}, {  0, 4}}, //14 right 1
};

static const Animation char_sneed_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1,  2,  3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, ASCR_BACK, 1}},         //CharAnim_Left
	{2, (const u8[]){ 5, 6, ASCR_BACK, 1}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 9, ASCR_BACK, 1}},         //CharAnim_Down
	{2, (const u8[]){ 7, 8, 9, ASCR_BACK, 1}},         //CharAnim_Down
	{2, (const u8[]){ 10, 11, 12, ASCR_BACK, 1}},         //CharAnim_Up
	{2, (const u8[]){ 10, 11, 12, ASCR_BACK, 1}},         //CharAnim_Up
	{2, (const u8[]){13, 14, ASCR_BACK, 1}},         //CharAnim_Right
	{2, (const u8[]){13, 14, ASCR_BACK, 1}},         //CharAnim_Right
};

//Sneed character functions
void Char_Sneed_SetFrame(void *user, u8 frame)
{
	Char_Sneed *this = (Char_Sneed*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_sneed_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Sneed_Tick(Character *character)
{
	Char_Sneed *this = (Char_Sneed*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Sneed_SetFrame);
	Character_Draw(character, &this->tex, &char_sneed_frame[this->frame]);
}

void Char_Sneed_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Sneed_Free(Character *character)
{
	Char_Sneed *this = (Char_Sneed*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Sneed_New(fixed_t x, fixed_t y)
{
	//Allocate sneed object
	Char_Sneed *this = Mem_Alloc(sizeof(Char_Sneed));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Sneed_New] Failed to allocate sneed object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Sneed_Tick;
	this->character.set_anim = Char_Sneed_SetAnim;
	this->character.free = Char_Sneed_Free;
	
	Animatable_Init(&this->character.animatable, char_sneed_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(30,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SNEED.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Sneed_ArcMain_Idle0
		"idle1.tim", //Sneed_ArcMain_Idle1
		"idle2.tim",  //Sneed_ArcMain_Left
		"left.tim",  //Sneed_ArcMain_Down
		"down0.tim",    //Sneed_ArcMain_Up
		"down1.tim", //Sneed_ArcMain_Right
		"up0.tim",
		"up1.tim",
		"right.tim",
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
