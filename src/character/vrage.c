/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "vrage.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

boolean fuck;

//vRage character structure
enum
{
	vRage_ArcMain_Idle0,
	vRage_ArcMain_Idle1,
	vRage_ArcMain_Idle2,
	vRage_ArcMain_Idle3,

	vRage_ArcMain_Left0,
	vRage_ArcMain_Left1,
	vRage_ArcMain_Left2,

	vRage_ArcMain_Down0,
	vRage_ArcMain_Down1,
	vRage_ArcMain_Down2,

	vRage_ArcMain_Up0,
	vRage_ArcMain_Up1,
	vRage_ArcMain_Up2,

	vRage_ArcMain_Right0,
	vRage_ArcMain_Right1,
	vRage_ArcMain_Right2,

	vRage_ArcMain_Row0,
	vRage_ArcMain_Row1,
	vRage_ArcMain_Row2,

	vRage_ArcMain_FUCK0,
	vRage_ArcMain_FUCK1,
	vRage_ArcMain_FUCK2,
	vRage_ArcMain_FUCK3,
	vRage_ArcMain_FUCK4,
	vRage_ArcMain_FUCK5,
	vRage_ArcMain_FUCK6,
	vRage_ArcMain_FUCK7,
	vRage_ArcMain_FUCK8,
	vRage_ArcMain_FUCK9,
	
	vRage_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[vRage_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_vRage;

//vRage character definitions
static const CharFrame char_vrage_frame[] = {
	{vRage_ArcMain_Idle0,  {  0,   0, 136, 106}, {  0,   0}}, //0
	{vRage_ArcMain_Idle0,  {  0, 107, 136, 106}, {  0,   0}}, //1
	{vRage_ArcMain_Idle1,  {  0,   0, 136, 106}, {  0,   0}}, //2
	{vRage_ArcMain_Idle1,  {  0, 107, 136, 107}, {  0,   1}}, //3
	{vRage_ArcMain_Idle2,  {  0,   0, 136, 107}, {  0,   1}}, //4
	{vRage_ArcMain_Idle2,  {  0, 108, 136, 107}, {  0,   1}}, //5
	{vRage_ArcMain_Idle3,  {  0,   0, 136, 107}, {  0,   1}}, //6
	{vRage_ArcMain_Idle3,  {  0, 108, 136, 107}, {  0,   1}}, //7

	{vRage_ArcMain_Left0,  {  0,   0, 135, 106}, {  0,   0}}, //8
	{vRage_ArcMain_Left0,  {  0, 107, 135, 106}, {  0,   0}}, //9
	{vRage_ArcMain_Left1,  {  0,   0, 135, 107}, {  0,   2}}, //10
	{vRage_ArcMain_Left1,  {  0, 108, 135, 107}, {  0,   2}}, //11
	{vRage_ArcMain_Left2,  {  0,   0, 135, 107}, {  0,   2}}, //12
	{vRage_ArcMain_Left2,  {  0, 108, 135, 107}, {  0,   2}}, //13

	{vRage_ArcMain_Down0,  {  0,   0, 135,  98}, {  0,  -8}}, //14
	{vRage_ArcMain_Down0,  {  0,  99, 135,  98}, {  0,  -8}}, //15
	{vRage_ArcMain_Down1,  {  0,   0, 136, 100}, {  0,  -6}}, //16
	{vRage_ArcMain_Down1,  {  0, 100, 136, 100}, {  0,  -6}}, //17
	{vRage_ArcMain_Down2,  {  0,   0, 136,  99}, {  0,  -7}}, //18

	{vRage_ArcMain_Up0,    {  0,   0, 136, 105}, {  0,  -1}}, //19
	{vRage_ArcMain_Up0,    {  0, 106, 136, 105}, {  0,  -1}}, //20
	{vRage_ArcMain_Up1,    {  0,   0, 136, 107}, {  0,   2}}, //21
	{vRage_ArcMain_Up1,    {  0, 108, 136, 107}, {  0,   2}}, //22
	{vRage_ArcMain_Up2,    {  0,   0, 136, 107}, {  0,   2}}, //23

	{vRage_ArcMain_Right0, {  0,   0, 136,  96}, {  0,  -9}}, //24
	{vRage_ArcMain_Right0, {  0,  97, 136,  96}, {  0,  -9}}, //25
	{vRage_ArcMain_Right1, {  0,   0, 136,  97}, {  0,  -8}}, //26
	{vRage_ArcMain_Right1, {  0,  98, 136,  97}, {  0,  -8}}, //27
	{vRage_ArcMain_Right2, {  0,   0, 136,  97}, {  0,  -8}}, //28

	{vRage_ArcMain_Row0,  {  0,   0, 136, 109}, {  0,   3}}, //29
	{vRage_ArcMain_Row0,  {  0, 110, 136, 109}, {  0,   3}}, //30
	{vRage_ArcMain_Row1,  {  0,   0, 136, 106}, {  0,   0}}, //31
	{vRage_ArcMain_Row1,  {  0, 107, 137,  81}, {  1, -26}}, //32
	{vRage_ArcMain_Row2,  {  0,   0, 136,  81}, {  0, -25}}, //33
	{vRage_ArcMain_Row2,  {  0,  82, 136,  84}, {  0, -22}}, //34

	{vRage_ArcMain_FUCK0,  {  0,   0, 137, 106}, {  1,   1}}, //35
	{vRage_ArcMain_FUCK0,  {  0, 107, 137, 107}, {  1,   2}}, //36
	{vRage_ArcMain_FUCK1,  {  0,   0, 137,  95}, {  1, -10}}, //37
	{vRage_ArcMain_FUCK1,  {  0,  96, 137,  97}, {  1,  -8}}, //38
	{vRage_ArcMain_FUCK2,  {  0,   0, 137,  98}, {  1,  -7}}, //39
	{vRage_ArcMain_FUCK2,  {  0,  99, 137,  98}, {  1,  -7}}, //40
	{vRage_ArcMain_FUCK3,  {  0,   0, 137,  98}, {  1,  -7}}, //41
	{vRage_ArcMain_FUCK3,  {  0,  99, 137,  99}, {  1,  -6}}, //42
	{vRage_ArcMain_FUCK4,  {  0,   0, 137,  99}, {  1,  -6}}, //43
	{vRage_ArcMain_FUCK4,  {  0, 100, 137,  97}, {  1,  -8}}, //44
	{vRage_ArcMain_FUCK5,  {  0,   0, 137,  97}, {  1,  -8}}, //45
	{vRage_ArcMain_FUCK5,  {  0,  98, 137,  97}, {  1,  -8}}, //46
	{vRage_ArcMain_FUCK6,  {  0,   0, 137,  97}, {  1,  -8}}, //47
	{vRage_ArcMain_FUCK6,  {  0,  98, 137,  97}, {  1,  -8}}, //48
	{vRage_ArcMain_FUCK7,  {  0,   0, 137,  97}, {  1,  -8}}, //49
	{vRage_ArcMain_FUCK7,  {  0,  98, 137,  97}, {  1,  -8}}, //50
	{vRage_ArcMain_FUCK8,  {  0,   0, 137,  97}, {  1,  -8}}, //51
	{vRage_ArcMain_FUCK8,  {  0,  98, 137,  97}, {  1,  -8}}, //52
	{vRage_ArcMain_FUCK9,  {  0,   0, 137,  97}, {  1,  -8}}, //53
	{vRage_ArcMain_FUCK9,  {  0,  98, 137,  97}, {  1,  -8}}, //54
};

static const Animation char_vrage_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, ASCR_BACK, 2}}, //CharAnim_Idle
	{1, (const u8[]){ 8, 9, 10, 11, 12, 13, ASCR_BACK, 1}},         //CharAnim_Left
	{1, (const u8[]){ 8, 9, 10, 11, 12, 13, ASCR_BACK, 1}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 14, 15, 16, 17, 18, ASCR_BACK, 1}},         //CharAnim_Down
	{1, (const u8[]){ 14, 15, 16, 17, 18, ASCR_BACK, 1}},   //CharAnim_DownAlt
	{1, (const u8[]){ 19, 20, 21, 22, 23, ASCR_BACK, 1}},         //CharAnim_Up
	{1, (const u8[]){ 19, 20, 21, 22, 23, ASCR_BACK, 1}},   //CharAnim_UpAlt
	{1, (const u8[]){24, 25, 26, 27, 28, ASCR_BACK, 1}},         //CharAnim_Right
	{1, (const u8[]){29, 30, 31, 32, 33, 34, ASCR_BACK, 1}},   //ROW ROW FIGHT DA POWAH
	{1, (const u8[]){35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, ASCR_BACK, 10}},
};

//vRage character functions
void Char_vRage_SetFrame(void *user, u8 frame)
{
	Char_vRage *this = (Char_vRage*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_vrage_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_vRage_Tick(Character *character)
{
	Char_vRage *this = (Char_vRage*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 && fuck != 1)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_vRage_SetFrame);
	Character_Draw(character, &this->tex, &char_vrage_frame[this->frame]);

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		if (stage.stage_id == StageId_1_2 && stage.song_step == 1660)
		{
			character->set_anim(character, CharAnim_Extra1);
			fuck = 1;
		}
	}
}

void Char_vRage_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_vRage_Free(Character *character)
{
	Char_vRage *this = (Char_vRage*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_vRage_New(fixed_t x, fixed_t y)
{
	//Allocate vrage object
	Char_vRage *this = Mem_Alloc(sizeof(Char_vRage));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_vRage_New] Failed to allocate vrage object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_vRage_Tick;
	this->character.set_anim = Char_vRage_SetAnim;
	this->character.free = Char_vRage_Free;
	
	Animatable_Init(&this->character.animatable, char_vrage_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(85,1);
	this->character.focus_y = FIXED_DEC(20,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\VRAGE.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
		"idle1.tim",
		"idle2.tim",
		"idle3.tim",
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
		"row0.tim",
		"row1.tim",
		"row2.tim",
		"fuck0.tim",
		"fuck1.tim",
		"fuck2.tim",
		"fuck3.tim",
		"fuck4.tim",
		"fuck5.tim",
		"fuck6.tim",
		"fuck7.tim",
		"fuck8.tim",
		"fuck9.tim",
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

	fuck = 0;
	
	return (Character*)this;
}
