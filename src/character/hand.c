/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "hand.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Hand character structure
enum
{
	Hand_ArcMain_Idle0,
	Hand_ArcMain_Idle1,
	Hand_ArcMain_Idle2,

	Hand_ArcMain_Leftb0,
	Hand_ArcMain_Leftb1,
	Hand_ArcMain_Leftb2,

	Hand_ArcMain_Downb0,
	Hand_ArcMain_Downb1,
	Hand_ArcMain_Downb2,

	Hand_ArcMain_Upb0,
	Hand_ArcMain_Upb1,
	Hand_ArcMain_Upb2,

	Hand_ArcMain_Rightb0,
	Hand_ArcMain_Rightb1,
	Hand_ArcMain_Rightb2,

	Hand_ArcMain_Left0,
	Hand_ArcMain_Left1,
	Hand_ArcMain_Left2,

	Hand_ArcMain_Down0,
	Hand_ArcMain_Down1,
	Hand_ArcMain_Down2,

	Hand_ArcMain_Up0,
	Hand_ArcMain_Up1,
	Hand_ArcMain_Up2,

	Hand_ArcMain_Right0,
	Hand_ArcMain_Right1,
	Hand_ArcMain_Right2,
	
	
	Hand_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Hand_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Hand;

//Hand character definitions
static const CharFrame char_hand_frame[] = {
	{Hand_ArcMain_Idle0, {  0,   0, 244, 104}, {   0,   0}}, //0
	{Hand_ArcMain_Idle1, {  0,   0, 244, 104}, {   0,   0}}, //1
	{Hand_ArcMain_Idle2, {  0,   0, 244, 104}, {   0,   0}}, //2

	{Hand_ArcMain_Leftb0, {  0,   0, 244, 104}, {   0,   0}}, //3
	{Hand_ArcMain_Leftb1, {  0,   0, 244, 104}, {   0,   0}}, //4
	{Hand_ArcMain_Leftb2, {  0,   0, 244, 104}, {   0,   0}}, //5

	{Hand_ArcMain_Downb0, {  0,   0, 244, 104}, {   0,   0}}, //6
	{Hand_ArcMain_Downb1, {  0,   0, 244, 104}, {   0,   0}}, //7
	{Hand_ArcMain_Downb2, {  0,   0, 244, 104}, {   0,   0}}, //8

	{Hand_ArcMain_Upb0, {  0,   0, 244, 104}, {   0,   0}}, //9
	{Hand_ArcMain_Upb1, {  0,   0, 244, 104}, {   0,   0}}, //10
	{Hand_ArcMain_Upb2, {  0,   0, 244, 104}, {   0,   0}}, //11

	{Hand_ArcMain_Rightb0, {  0,   0, 244, 104}, {   0,   0}}, //12
	{Hand_ArcMain_Rightb1, {  0,   0, 244, 104}, {   0,   0}}, //13
	{Hand_ArcMain_Rightb2, {  0,   0, 244, 104}, {   0,   0}}, //14

	{Hand_ArcMain_Left0, {  0,   0, 244, 104}, {   0,   0}}, //15
	{Hand_ArcMain_Left1, {  0,   0, 244, 104}, {   0,   0}}, //16
	{Hand_ArcMain_Left2, {  0,   0, 244, 104}, {   0,   0}}, //17

	{Hand_ArcMain_Down0, {  0,   0, 244, 104}, {   0,   0}}, //18
	{Hand_ArcMain_Down1, {  0,   0, 244, 104}, {   0,   0}}, //19
	{Hand_ArcMain_Down2, {  0,   0, 244, 104}, {   0,   0}}, //20

	{Hand_ArcMain_Up0, {  0,   0, 244, 104}, {   0,   0}}, //21
	{Hand_ArcMain_Up1, {  0,   0, 244, 104}, {   0,   0}}, //22
	{Hand_ArcMain_Up2, {  0,   0, 244, 104}, {   0,   0}}, //23

	{Hand_ArcMain_Right0, {  0,   0, 244, 104}, {   0,   0}}, //24
	{Hand_ArcMain_Right1, {  0,   0, 244, 104}, {   0,   0}}, //25
	{Hand_ArcMain_Right2, {  0,   0, 244, 104}, {   0,   0}}, //26
};

static const Animation char_hand_anim[CharAnim_Max] = {
	{3, (const u8[]){ 0, 1, 2, 2, 1, 0, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 3, 4, 5, ASCR_BACK, 1}},         //CharAnim_Left
	{2, (const u8[]){15, 16, 17, ASCR_BACK, 1}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, 8, ASCR_BACK, 1}},         //CharAnim_Down
	{2, (const u8[]){18, 19, 20, ASCR_BACK, 1}},   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 11, ASCR_BACK, 1}},         //CharAnim_Up
	{2, (const u8[]){21, 22, 23, ASCR_BACK, 1}},   //CharAnim_UpAlt
	{2, (const u8[]){12, 13, 14, ASCR_BACK, 1}},         //CharAnim_Right
	{2, (const u8[]){24, 25, 26, ASCR_BACK, 1}},   //CharAnim_RightAlt
};

//Hand character functions
void Char_Hand_SetFrame(void *user, u8 frame)
{
	Char_Hand *this = (Char_Hand*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_hand_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Hand_Tick(Character *character)
{
	Char_Hand *this = (Char_Hand*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Hand_SetFrame);
	Character_DrawScale(character, &this->tex, &char_hand_frame[this->frame], 2, 2);
}

void Char_Hand_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Hand_Free(Character *character)
{
	Char_Hand *this = (Char_Hand*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Hand_New(fixed_t x, fixed_t y)
{
	//Allocate hand object
	Char_Hand *this = Mem_Alloc(sizeof(Char_Hand));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Hand_New] Failed to allocate hand object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Hand_Tick;
	this->character.set_anim = Char_Hand_SetAnim;
	this->character.free = Char_Hand_Free;
	
	Animatable_Init(&this->character.animatable, char_hand_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(0,1);
	this->character.focus_y = FIXED_DEC(0,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\HAND.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
		"idle1.tim",
		"idle2.tim",
		"leftb0.tim",
		"leftb1.tim",
		"leftb2.tim",
		"downb0.tim",
		"downb1.tim",
		"downb2.tim",
		"upb0.tim",
		"upb1.tim",
		"upb2.tim",
		"rightb0.tim",
		"rightb1.tim",
		"rightb2.tim",
		"left0.tim",
		"left1.tim",
		"left2.tim",
		"down0.tim",
		"down1.tim",
		"down2.tim",
		"up0.tim",
		"up1.tim",
		"up2.tim",
		"right0.tim",
		"right1.tim",
		"right2.tim",
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
