#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File file;
	u32 length;
} XA_TrackDef;

static const XA_TrackDef xa_tracks[] = {
	//MENU.XA
	{XA_Menu, XA_LENGTH(11295)}, //XA_GettinFreaky
	{XA_Menu, XA_LENGTH(3840)},  //XA_GameOver
	//VA.XA
	{XA_Va, XA_LENGTH(11100)}, //XA_Vidyagaems
	{XA_Va, XA_LENGTH(12800)}, //XA_Sage
	//VB.XA
	{XA_Vb, XA_LENGTH(10400)}, //XA_Harmony
	{XA_Vb, XA_LENGTH(19600)}, //XA_Infinitrigger
	//VC.XA
	{XA_Vc, XA_LENGTH(18100)}, //XA_Nogames
	{XA_Vc, XA_LENGTH(13100)}, //XA_Sneed
	//VD.XA
	{XA_Vd, XA_LENGTH(12900)}, //XA_Birthday
	//GAMER.XA
	{XA_Gamer, XA_LENGTH(2200)}, //XA_Intro
	{XA_Gamer, XA_LENGTH(6000)}, //XA_Breakfast
	//BIKER.XA
	{XA_Biker, XA_LENGTH(10100)},
	//EARTHBOUND.XA
	{XA_EarthBound, XA_LENGTH(11700)},
	{XA_EarthBound, XA_LENGTH(2100)},
	{XA_EarthBound, XA_LENGTH(3900)},
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\VA.XA;1",     //XA_Va
	"\\MUSIC\\VB.XA;1",     //XA_Vb
	"\\MUSIC\\VC.XA;1",     //XA_Vc
	"\\MUSIC\\VD.XA;1",     //XA_Vd
	"\\MUSIC\\GAMER.XA;1",
	"\\MUSIC\\BIKER.XA;1",
	"\\MUSIC\\PORK.XA;1",
	NULL,
};

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3;

static const XA_Mp3 xa_mp3s[] = {
	//MENU.XA
	{"freaky", false},   //XA_GettinFreaky
	{"gameover", false}, //XA_GameOver
	//VA.XA
	{"vidyagaems", true},
	{"sage", true},
	//VB.XA
	{"harmony", true},
	{"infinitrigger", true},
	//VC.XA
	{"nogames", true},
	{"sneed", true},
	//VD.XA
    {"birthday", true},
	//GAMER.XA
	{"intro", false},
	{"breakfast", false},
	//BIKER.XA
	{"blaster", true},
	//EARTHBOUND.XA
	{"konbanwa", true},
	{"title", false},
	{"boundmenu", false},
	
	{NULL, false}
};
