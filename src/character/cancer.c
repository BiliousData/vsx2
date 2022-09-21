/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "cancer.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Cancer character structure
enum
{
	Cancer_ArcMain_Idle0,
	Cancer_ArcMain_Idle1,
	Cancer_ArcMain_Idle2,
	Cancer_ArcMain_Idle3,
	Cancer_ArcMain_Left0,
	Cancer_ArcMain_Left1,
	Cancer_ArcMain_Down0,
	Cancer_ArcMain_Down1,
	Cancer_ArcMain_Up0,
	Cancer_ArcMain_Up1,
	Cancer_ArcMain_Up2,
	Cancer_ArcMain_Right0,
	Cancer_ArcMain_Right1,
	Cancer_ArcMain_Right2,
	
	Cancer_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Cancer_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Cancer;

//Cancer character definitions
static const CharFrame char_cancer_frame[] = {
	{Cancer_ArcMain_Idle0,  {  0,   0, 116, 125}, {  0,   0}}, //0
	{Cancer_ArcMain_Idle0,  {117,   0, 113, 133}, {  0,   6}}, //1
	{Cancer_ArcMain_Idle1,  {  0,   0, 112, 135}, {  0,   8}}, //2
	{Cancer_ArcMain_Idle1,  {113,   0, 112, 135}, {  0,   8}}, //3
	{Cancer_ArcMain_Idle2,  {  0,   0, 111, 135}, {  0,   8}}, //4
	{Cancer_ArcMain_Idle2,  {112,   0, 111, 135}, {  0,   8}}, //5
	{Cancer_ArcMain_Idle3,  {  0,   0, 111, 135}, {  0,   8}}, //6

	{Cancer_ArcMain_Left0,  {  0,   0, 125, 169}, {  2,  44}}, //7
	{Cancer_ArcMain_Left0,  {126,   0, 127, 166}, {  4,  41}}, //8
	{Cancer_ArcMain_Left1,  {  0,   0, 127, 165}, {  4,  40}}, //9

	{Cancer_ArcMain_Down0,  {  0,   0, 134, 120}, {  3,  -3}}, //10
	{Cancer_ArcMain_Down0,  {  0, 121, 130, 129}, {  2,   6}}, //11
	{Cancer_ArcMain_Down1,  {  0,   0, 130, 129}, {  2,   6}}, //12

	{Cancer_ArcMain_Up0,    {  0,   0, 131, 178}, {  6,  51}}, //13
	{Cancer_ArcMain_Up1,    {  0,   0, 136, 180}, {  6,  52}}, //14
	{Cancer_ArcMain_Up2,    {  0,   0, 137, 180}, {  7,  52}}, //15

	{Cancer_ArcMain_Right0, {  0,   0, 152, 144}, {  1,  19}}, //16
	{Cancer_ArcMain_Right1, {  0,   0, 146, 143}, {  0,  19}}, //17
	{Cancer_ArcMain_Right2, {  0,   0, 144, 143}, {  0,  19}}, //18
};

static const Animation char_cancer_anim[CharAnim_Max] = {
	{2, (const u8[]){0, 1, 2, 3, 4, 5, 6, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){7, 8, 9, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){10, 11, 12, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){13, 14, 15, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){16, 17, 18, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Cancer character functions
void Char_Cancer_SetFrame(void *user, u8 frame)
{
	Char_Cancer *this = (Char_Cancer*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_cancer_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Cancer_Tick(Character *character)
{
	Char_Cancer *this = (Char_Cancer*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Cancer_SetFrame);
	Character_Draw(character, &this->tex, &char_cancer_frame[this->frame]);
}

void Char_Cancer_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Cancer_Free(Character *character)
{
	Char_Cancer *this = (Char_Cancer*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_CancerLord_New(fixed_t x, fixed_t y)
{
	//Allocate cancer object
	Char_Cancer *this = Mem_Alloc(sizeof(Char_Cancer));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Cancer_New] Failed to allocate cancer object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Cancer_Tick;
	this->character.set_anim = Char_Cancer_SetAnim;
	this->character.free = Char_Cancer_Free;
	
	Animatable_Init(&this->character.animatable, char_cancer_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(0,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\CANCER.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
		"idle1.tim",
		"idle2.tim",
		"idle3.tim",
		"left0.tim",
		"left1.tim",
		"down0.tim",
		"down1.tim",
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
