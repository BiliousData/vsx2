/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "homer.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Homer character structure
enum
{
	Homer_ArcMain_Idle0,
	Homer_ArcMain_Idle1,
	Homer_ArcMain_Left,
	Homer_ArcMain_Down,
	Homer_ArcMain_Up,
	Homer_ArcMain_Right,
	
	Homer_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Homer_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Homer;

//Homer character definitions
static const CharFrame char_homer_frame[] = {
	{Homer_ArcMain_Idle0, {  0,   0,  69, 150}, {  0,   0}}, //0 idle 1
	{Homer_ArcMain_Idle0, { 69,   0,  70, 151}, {  2,   1}}, //1 idle 1
	{Homer_ArcMain_Idle0, {139,   0,  69, 151}, {  1,   1}}, //2 idle 1
	{Homer_ArcMain_Idle1, {  0,   0,  70, 151}, {  1,   1}}, //3 idle 1
	{Homer_ArcMain_Idle1, { 70,   0,  68, 151}, {  0,   1}}, //4 idle 1

	{Homer_ArcMain_Left,  {  0,   0,  73, 150}, {  4,  -1}}, //5 idle 1
	{Homer_ArcMain_Left,  { 73,   0,  71, 151}, {  2,   0}}, //6 idle 1

	{Homer_ArcMain_Down,  {  0,   0,  70, 150}, {  0,   0}}, //7 idle 1
	{Homer_ArcMain_Down,  { 70,   0,  70, 151}, {  0,   1}}, //8 idle 1

	{Homer_ArcMain_Up,    {  0,   0,  72, 154}, {  2,   3}}, //9 idle 1
	{Homer_ArcMain_Up,    { 72,   0,  71, 154}, {  1,   3}}, //10 idle 1

	{Homer_ArcMain_Right, {  0,   0,  86, 152}, {  0,   2}}, //11 idle 1
	{Homer_ArcMain_Right, { 86,   0,  85, 152}, {  1,   2}}, //12 idle 1
};

static const Animation char_homer_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){11, 12, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Homer character functions
void Char_Homer_SetFrame(void *user, u8 frame)
{
	Char_Homer *this = (Char_Homer*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_homer_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Homer_Tick(Character *character)
{
	Char_Homer *this = (Char_Homer*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Homer_SetFrame);
	Character_Draw(character, &this->tex, &char_homer_frame[this->frame]);
}

void Char_Homer_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Homer_Free(Character *character)
{
	Char_Homer *this = (Char_Homer*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Homer_New(fixed_t x, fixed_t y)
{
	//Allocate homer object
	Char_Homer *this = Mem_Alloc(sizeof(Char_Homer));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Homer_New] Failed to allocate homer object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Homer_Tick;
	this->character.set_anim = Char_Homer_SetAnim;
	this->character.free = Char_Homer_Free;
	
	Animatable_Init(&this->character.animatable, char_homer_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(-10,1);
	this->character.focus_y = FIXED_DEC(82,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\HOMER.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Homer_ArcMain_Idle0
		"idle1.tim", //Homer_ArcMain_Idle1
		"left.tim",  //Homer_ArcMain_Left
		"down.tim",  //Homer_ArcMain_Down
		"up.tim",    //Homer_ArcMain_Up
		"right.tim", //Homer_ArcMain_Right
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
