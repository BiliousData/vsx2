/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "chuck.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Chuck character structure
enum
{
	Chuck_ArcMain_Idle0,
	Chuck_ArcMain_Idle1,
	Chuck_ArcMain_Idle2,
	Chuck_ArcMain_Idle3,
	Chuck_ArcMain_Idle4,
	Chuck_ArcMain_Idle5,
	Chuck_ArcMain_Left0,
	Chuck_ArcMain_Left1,
	Chuck_ArcMain_Left2,
	Chuck_ArcMain_Down0,
	Chuck_ArcMain_Down1,
	Chuck_ArcMain_Down2,
	Chuck_ArcMain_Up0,
	Chuck_ArcMain_Up1,
	Chuck_ArcMain_Up2,
	Chuck_ArcMain_Right0,
	Chuck_ArcMain_Right1,
	Chuck_ArcMain_Right2,
	
	Chuck_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Chuck_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Chuck;

//Chuck character definitions
static const CharFrame char_chuck_frame[] = {
	{Chuck_ArcMain_Idle0, {  0,   0, 150, 197}, {  0,   0}}, //0
	{Chuck_ArcMain_Idle1, {  0,   0, 150, 198}, {  0,   1}}, //1
	{Chuck_ArcMain_Idle2, {  0,   0, 151, 200}, {  1,   3}}, //2
	{Chuck_ArcMain_Idle3, {  0,   0, 152, 200}, {  2,   3}}, //3
	{Chuck_ArcMain_Idle4, {  0,   0, 152, 201}, {  2,   4}}, //4
	{Chuck_ArcMain_Idle5, {  0,   0, 152, 201}, {  2,   4}}, //5

	{Chuck_ArcMain_Left0, {  0,   0, 153, 201}, {  4,   4}}, //6
	{Chuck_ArcMain_Left1, {  0,   0, 153, 201}, {  4,   4}}, //7
	{Chuck_ArcMain_Left2, {  0,   0, 152, 201}, {  3,   4}}, //8

	{Chuck_ArcMain_Down0, {  0,   0, 149, 196}, { -1,  -1}}, //9
	{Chuck_ArcMain_Down1, {  0,   0, 149, 198}, { -1,   1}}, //10
	{Chuck_ArcMain_Down2, {  0,   0, 148, 198}, { -2,   1}}, //11

	{Chuck_ArcMain_Up0,   {  0,   0, 152, 202}, {  2,   4}}, //12
	{Chuck_ArcMain_Up1,   {  0,   0, 151, 200}, {  1,   3}}, //13
	{Chuck_ArcMain_Up2,   {  0,   0, 151, 199}, {  1,   2}}, //14

	{Chuck_ArcMain_Right0, {  0,   0, 150, 199}, {  0,   2}}, //15
	{Chuck_ArcMain_Right1, {  0,   0, 151, 200}, {  1,   3}}, //16
	{Chuck_ArcMain_Right2, {  0,   0, 152, 200}, {  2,   3}}, //17
};

static const Animation char_chuck_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 8, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 9, 10, 11, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 12, 13, 14, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){15, 16, 17, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Chuck character functions
void Char_Chuck_SetFrame(void *user, u8 frame)
{
	Char_Chuck *this = (Char_Chuck*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_chuck_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Chuck_Tick(Character *character)
{
	Char_Chuck *this = (Char_Chuck*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Chuck_SetFrame);
	Character_DrawWide(character, &this->tex, &char_chuck_frame[this->frame], 2);
}

void Char_Chuck_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Chuck_Free(Character *character)
{
	Char_Chuck *this = (Char_Chuck*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Chuck_New(fixed_t x, fixed_t y)
{
	//Allocate chuck object
	Char_Chuck *this = Mem_Alloc(sizeof(Char_Chuck));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Chuck_New] Failed to allocate chuck object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Chuck_Tick;
	this->character.set_anim = Char_Chuck_SetAnim;
	this->character.free = Char_Chuck_Free;
	
	Animatable_Init(&this->character.animatable, char_chuck_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(115,1);
	this->character.focus_y = FIXED_DEC(79,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\CHUCK.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Chuck_ArcMain_Idle0
		"idle1.tim",
		"idle2.tim",
		"idle3.tim",
		"idle4.tim",
		"idle5.tim",
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
