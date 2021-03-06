// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
//
// $Log:$
//
// DESCRIPTION:  the automap code
//
//-----------------------------------------------------------------------------

#include <stdio.h>

#include "doomdef.h"
#include "templates.h"
#include "g_level.h"
#include "doomdef.h"
#include "st_stuff.h"
#include "p_local.h"
#include "p_lnspec.h"
#include "w_wad.h"
#include "a_sharedglobal.h"
#include "statnums.h"

#include "m_cheat.h"
#include "i_system.h"
#include "c_dispatch.h"

// Needs access to LFB.
#include "v_video.h"

#include "v_text.h"

// State.
#include "doomstat.h"
#include "r_state.h"

// Data.
#include "gstrings.h"

#include "am_map.h"
#include "a_artifacts.h"

// [BC] New #includes.
#include "team.h"
#include "deathmatch.h"
#include "network.h"
#include "scoreboard.h"
#include "gl_main.h"

static int Background, YourColor, WallColor, TSWallColor,
		   FDWallColor, CDWallColor, ThingColor,
		   ThingColor_Item, ThingColor_Monster, ThingColor_Friend,
		   SecretWallColor, GridColor, XHairColor,
		   NotSeenColor,
		   LockedColor,
		   AlmostBackground,
		   IntraTeleportColor, InterTeleportColor,
		   SecretSectorColor;

static int DoomColors[11];
static BYTE DoomPaletteVals[11*3] =
{
	0x00,0x00,0x00, 0xff,0xff,0xff, 0x10,0x10,0x10,
	0xfc,0x00,0x00, 0x80,0x80,0x80, 0xbc,0x78,0x48,
	0xfc,0xfc,0x00, 0x74,0xfc,0x6c, 0x4c,0x4c,0x4c,
	0x80,0x80,0x80, 0x6c,0x6c,0x6c
};

#define MAPBITS 12
#define MapDiv SafeDivScale12
#define MapMul MulScale12
#define MAPUNIT (1<<MAPBITS)
#define FRACTOMAPBITS (FRACBITS-MAPBITS)

// scale on entry
#define INITSCALEMTOF (.2*MAPUNIT)
// used by MTOF to scale from map-to-frame-buffer coords
fixed_t scale_mtof = (fixed_t)INITSCALEMTOF; // [ZDoomGL]: was static
// used by FTOM to scale from frame-buffer-to-map coords (=1/scale_mtof)
static fixed_t scale_ftom;

// translates between frame-buffer and map distances
inline fixed_t FTOM(fixed_t x)
{
	return x * scale_ftom;
}

inline fixed_t MTOF(fixed_t x)
{
	return MulScale24 (x, scale_mtof);
}

static int WeightingScale;

CVAR (Int,   am_rotate,				0,			CVAR_ARCHIVE);
CVAR (Int,   am_overlay,			0,			CVAR_ARCHIVE);
CVAR (Bool,  am_showsecrets,		true,		CVAR_ARCHIVE);
CVAR (Bool,  am_showmonsters,		true,		CVAR_ARCHIVE);
CVAR (Bool,  am_showbarrels,		true,		CVAR_ARCHIVE);
CVAR (Bool,  am_showitems,			true,		CVAR_ARCHIVE);
CVAR (Bool,  am_showtime,			true,		CVAR_ARCHIVE);
CVAR (Bool,  am_showtotaltime,		false,		CVAR_ARCHIVE);
CVAR (Bool,  am_usecustomcolors,	true,		CVAR_ARCHIVE);
CVAR (Float, am_ovtrans,			1.f,		CVAR_ARCHIVE);
CVAR (Color, am_backcolor,			0x6c5440,	CVAR_ARCHIVE);
CVAR (Color, am_yourcolor,			0xfce8d8,	CVAR_ARCHIVE);
CVAR (Color, am_wallcolor,			0x2c1808,	CVAR_ARCHIVE);
CVAR (Color, am_secretwallcolor,	0x000000,	CVAR_ARCHIVE);
CVAR (Color, am_tswallcolor,		0x888888,	CVAR_ARCHIVE);
CVAR (Color, am_fdwallcolor,		0x887058,	CVAR_ARCHIVE);
CVAR (Color, am_cdwallcolor,		0x4c3820,	CVAR_ARCHIVE);
CVAR (Color, am_thingcolor,			0xfcfcfc,	CVAR_ARCHIVE);
CVAR (Color, am_gridcolor,			0x8b5a2b,	CVAR_ARCHIVE);
CVAR (Color, am_xhaircolor,			0x808080,	CVAR_ARCHIVE);
CVAR (Color, am_notseencolor,		0x6c6c6c,	CVAR_ARCHIVE);
CVAR (Color, am_lockedcolor,		0x007800,	CVAR_ARCHIVE);
CVAR (Color, am_ovyourcolor,		0xfce8d8,	CVAR_ARCHIVE);
CVAR (Color, am_ovwallcolor,		0x00ff00,	CVAR_ARCHIVE);
CVAR (Color, am_ovthingcolor,		0xe88800,	CVAR_ARCHIVE);
CVAR (Color, am_ovotherwallscolor,	0x008844,	CVAR_ARCHIVE);
CVAR (Color, am_ovunseencolor,		0x00226e,	CVAR_ARCHIVE);
CVAR (Color, am_ovtelecolor,		0xffff00,	CVAR_ARCHIVE);
CVAR (Color, am_intralevelcolor,	0x0000ff,	CVAR_ARCHIVE);
CVAR (Color, am_interlevelcolor,	0xff0000,	CVAR_ARCHIVE);
CVAR (Color, am_secretsectorcolor,	0xff00ff,	CVAR_ARCHIVE);
CVAR (Int,   am_map_secrets,		1,			CVAR_ARCHIVE);
CVAR (Bool,  am_drawmapback,		true,		CVAR_ARCHIVE);
CVAR (Color, am_thingcolor_friend,		0xfcfcfc,	CVAR_ARCHIVE);
CVAR (Color, am_thingcolor_monster,		0xfcfcfc,	CVAR_ARCHIVE);
CVAR (Color, am_thingcolor_item,		0xfcfcfc,	CVAR_ARCHIVE);
CVAR (Color, am_ovthingcolor_friend,	0xe88800,	CVAR_ARCHIVE);
CVAR (Color, am_ovthingcolor_monster,	0xe88800,	CVAR_ARCHIVE);
CVAR (Color, am_ovthingcolor_item,		0xe88800,	CVAR_ARCHIVE);

EXTERN_CVAR(Bool, gl_automap_dukestyle); // [ZDoomGL]

// drawing stuff
#define AM_PANDOWNKEY	KEY_DOWNARROW
#define AM_PANUPKEY		KEY_UPARROW
#define AM_PANRIGHTKEY	KEY_RIGHTARROW
#define AM_PANLEFTKEY	KEY_LEFTARROW
#define AM_ZOOMINKEY	KEY_EQUALS
#define AM_ZOOMINKEY2	0x4e	// DIK_ADD
#define AM_ZOOMOUTKEY	KEY_MINUS
#define AM_ZOOMOUTKEY2	0x4a	// DIK_SUBTRACT
#define AM_GOBIGKEY		0x0b	// DIK_0
#define AM_FOLLOWKEY	'f'
#define AM_GRIDKEY		'g'
#define AM_MARKKEY		'm'
#define AM_CLEARMARKKEY	'c'

#define AM_NUMMARKPOINTS 10

// player radius for automap checking
#define PLAYERRADIUS	16*MAPUNIT

// how much the automap moves window per tic in frame-buffer coordinates
// moves 140 pixels at 320x200 in 1 second
#define F_PANINC		(140/TICRATE)
// how much zoom-in per tic
// goes to 2x in 1 second
#define M_ZOOMIN        ((int) (1.02*MAPUNIT))
// how much zoom-out per tic
// pulls out to 0.5x in 1 second
#define M_ZOOMOUT       ((int) (MAPUNIT/1.02))

// translates between frame-buffer and map coordinates
#define CXMTOF(x)  (MTOF((x)-m_x)/* - f_x*/)
#define CYMTOF(y)  (f_h - MTOF((y)-m_y)/* + f_y*/)

typedef struct {
	int x, y;
} fpoint_t;

typedef struct {
	fpoint_t a, b;
} fline_t;

typedef struct {
	fixed_t x,y;
} mpoint_t;

typedef struct {
	mpoint_t a, b;
} mline_t;

typedef struct {
	fixed_t slp, islp;
} islope_t;



//
// The vector graphics for the automap.
//  A line drawing of the player pointing right,
//   starting from the middle.
//
#define R ((8*PLAYERRADIUS)/7)
mline_t player_arrow[] = {
	{ { -R+R/8, 0 }, { R, 0 } }, // -----
	{ { R, 0 }, { R-R/2, R/4 } },  // ----->
	{ { R, 0 }, { R-R/2, -R/4 } },
	{ { -R+R/8, 0 }, { -R-R/8, R/4 } }, // >---->
	{ { -R+R/8, 0 }, { -R-R/8, -R/4 } },
	{ { -R+3*R/8, 0 }, { -R+R/8, R/4 } }, // >>--->
	{ { -R+3*R/8, 0 }, { -R+R/8, -R/4 } }
};
#undef R
#define NUMPLYRLINES (sizeof(player_arrow)/sizeof(mline_t))

#define R ((8*PLAYERRADIUS)/7)
mline_t cheat_player_arrow[] = {
	{ { -R+R/8, 0 }, { R, 0 } }, // -----
	{ { R, 0 }, { R-R/2, R/6 } },  // ----->
	{ { R, 0 }, { R-R/2, -R/6 } },
	{ { -R+R/8, 0 }, { -R-R/8, R/6 } }, // >----->
	{ { -R+R/8, 0 }, { -R-R/8, -R/6 } },
	{ { -R+3*R/8, 0 }, { -R+R/8, R/6 } }, // >>----->
	{ { -R+3*R/8, 0 }, { -R+R/8, -R/6 } },
	{ { -R/2, 0 }, { -R/2, -R/6 } }, // >>-d--->
	{ { -R/2, -R/6 }, { -R/2+R/6, -R/6 } },
	{ { -R/2+R/6, -R/6 }, { -R/2+R/6, R/4 } },
	{ { -R/6, 0 }, { -R/6, -R/6 } }, // >>-dd-->
	{ { -R/6, -R/6 }, { 0, -R/6 } },
	{ { 0, -R/6 }, { 0, R/4 } },
	{ { R/6, R/4 }, { R/6, -R/7 } }, // >>-ddt->
	{ { R/6, -R/7 }, { R/6+R/32, -R/7-R/32 } },
	{ { R/6+R/32, -R/7-R/32 }, { R/6+R/10, -R/7 } }
};
#undef R
#define NUMCHEATPLYRLINES (sizeof(cheat_player_arrow)/sizeof(mline_t))

#define R (MAPUNIT)
// [RH] Avoid lots of warnings without compiler-specific #pragmas
#define L(a,b,c,d) { {(fixed_t)((a)*R),(fixed_t)((b)*R)}, {(fixed_t)((c)*R),(fixed_t)((d)*R)} }
mline_t triangle_guy[] = {
	L (-.867,-.5, .867,-.5),
	L (.867,-.5, 0,1),
	L (0,1, -.867,-.5)
};
#define NUMTRIANGLEGUYLINES (sizeof(triangle_guy)/sizeof(mline_t))

mline_t thintriangle_guy[] = {
	L (-.5,-.7, 1,0),
	L (1,0, -.5,.7),
	L (-.5,.7, -.5,-.7)
};
#undef L
#undef R
#define NUMTHINTRIANGLEGUYLINES (sizeof(thintriangle_guy)/sizeof(mline_t))



EXTERN_CVAR (Bool, sv_cheats)
CUSTOM_CVAR (Int, am_cheat, 0, 0)
{
	// No automap cheat in net games when cheats are disabled!
	if (( NETWORK_GetState( ) == NETSTATE_CLIENT ) && !sv_cheats && self != 0)
	{
		self = 0;
	}
}

static int 	grid = 0;

static int 	leveljuststarted = 1; 	// kluge until AM_LevelInit() is called

bool		automapactive = false;

// location of window on screen
// [ZDoomGL]: was static
int	f_x;
int	f_y;

// size of window on screen
// [ZDoomGL]: was static
int	f_w;
int	f_h;
static int	f_p;				// [RH] # of bytes from start of a line to start of next

static BYTE *fb;				// pseudo-frame buffer
static int	amclock;

static mpoint_t	m_paninc;		// how far the window pans each tic (map coords)
static fixed_t	mtof_zoommul;	// how far the window zooms in each tic (map coords)
static fixed_t	ftom_zoommul;	// how far the window zooms in each tic (fb coords)

// [ZDoomGL]: these were static
fixed_t	m_x, m_y;		// LL x,y where the window is on the map (map coords)
fixed_t	m_x2, m_y2;		// UR x,y where the window is on the map (map coords)

//
// width/height of window on map (map coords)
//
// [ZDoomGL]: was static
fixed_t	m_w;
fixed_t	m_h;

// based on level size
static fixed_t	min_x, min_y, max_x, max_y;

static fixed_t	max_w; // max_x-min_x,
static fixed_t	max_h; // max_y-min_y

// based on player size
static fixed_t	min_w;
static fixed_t	min_h;


static fixed_t	min_scale_mtof; // used to tell when to stop zooming out
static fixed_t	max_scale_mtof; // used to tell when to stop zooming in

// old stuff for recovery later
static fixed_t old_m_w, old_m_h;
static fixed_t old_m_x, old_m_y;

// old location used by the Follower routine
static mpoint_t f_oldloc;

static int marknums[10]; // numbers used for marking by the automap
static mpoint_t markpoints[AM_NUMMARKPOINTS]; // where the points are
static int markpointnum = 0; // next point to be assigned

static int followplayer = 1; // specifies whether to follow the player around

static FTexture *mapback;	// the automap background
static fixed_t mapystart=0; // y-value for the start of the map bitmap...used in the parallax stuff.
static fixed_t mapxstart=0; //x-value for the bitmap.

static bool stopped = true;


#define NUMALIASES		3
#define WALLCOLORS		-1
#define FDWALLCOLORS	-2
#define CDWALLCOLORS	-3

#define WEIGHTBITS		6
#define WEIGHTSHIFT		(16-WEIGHTBITS)
#define NUMWEIGHTS		(1<<WEIGHTBITS)
#define WEIGHTMASK		(NUMWEIGHTS-1)
static BYTE antialias[NUMALIASES][NUMWEIGHTS];



void AM_rotatePoint (fixed_t *x, fixed_t *y);
void AM_rotate (fixed_t *x, fixed_t *y, angle_t an);

void DrawWuLine (int X0, int Y0, int X1, int Y1, BYTE *BaseColor);
void DrawTransWuLine (int X0, int Y0, int X1, int Y1, BYTE BaseColor);

// Calculates the slope and slope according to the x-axis of a line
// segment in map coordinates (with the upright y-axis n' all) so
// that it can be used with the brain-dead drawing stuff.

// Ripped out for Heretic
/*
void AM_getIslope (mline_t *ml, islope_t *is)
{
	int dx, dy;

	dy = ml->a.y - ml->b.y;
	dx = ml->b.x - ml->a.x;
	if (!dy) is->islp = (dx<0?-MAXINT:MAXINT);
		else is->islp = FixedDiv(dx, dy);
	if (!dx) is->slp = (dy<0?-MAXINT:MAXINT);
		else is->slp = FixedDiv(dy, dx);
}
*/

void AM_GetPosition(fixed_t & x, fixed_t & y)
{
	x = (m_x + m_w/2) << FRACTOMAPBITS;
	y = (m_y + m_h/2) << FRACTOMAPBITS;
}
//
//
//
void AM_activateNewScale ()
{
	m_x += m_w/2;
	m_y += m_h/2;
	m_w = FTOM(f_w);
	m_h = FTOM(f_h);
	m_x -= m_w/2;
	m_y -= m_h/2;
	m_x2 = m_x + m_w;
	m_y2 = m_y + m_h;
}

//
//
//
void AM_saveScaleAndLoc ()
{
	old_m_x = m_x;
	old_m_y = m_y;
	old_m_w = m_w;
	old_m_h = m_h;
}

//
//
//
void AM_restoreScaleAndLoc ()
{
	m_w = old_m_w;
	m_h = old_m_h;
	if (!followplayer)
	{
		m_x = old_m_x;
		m_y = old_m_y;
    }
	else
	{
		m_x = (players[consoleplayer].camera->x >> FRACTOMAPBITS) - m_w/2;
		m_y = (players[consoleplayer].camera->y >> FRACTOMAPBITS)- m_h/2;
    }
	m_x2 = m_x + m_w;
	m_y2 = m_y + m_h;

	// Change the scaling multipliers
	scale_mtof = MapDiv(f_w<<MAPBITS, m_w);
	scale_ftom = MapDiv(MAPUNIT, scale_mtof);
}

//
// adds a marker at the current location
//
bool AM_addMark ()
{
	if (marknums[0] != -1)
	{
		markpoints[markpointnum].x = m_x + m_w/2;
		markpoints[markpointnum].y = m_y + m_h/2;
		markpointnum = (markpointnum + 1) % AM_NUMMARKPOINTS;
		return true;
	}
	return false;
}

//
// Determines bounding box of all vertices,
// sets global variables controlling zoom range.
//
static void AM_findMinMaxBoundaries ()
{
	int i;
	fixed_t a;
	fixed_t b;

	min_x = min_y = FIXED_MAX;
	max_x = max_y = FIXED_MIN;

	for (i = 0; i < numvertexes; i++)
	{
		if (vertexes[i].x < min_x)
			min_x = vertexes[i].x;
		else if (vertexes[i].x > max_x)
			max_x = vertexes[i].x;

		if (vertexes[i].y < min_y)
			min_y = vertexes[i].y;
		else if (vertexes[i].y > max_y)
			max_y = vertexes[i].y;
	}

	max_w = (max_x >>= FRACTOMAPBITS) - (min_x >>= FRACTOMAPBITS);
	max_h = (max_y >>= FRACTOMAPBITS) - (min_y >>= FRACTOMAPBITS);

	min_w = 2*PLAYERRADIUS; // const? never changed?
	min_h = 2*PLAYERRADIUS;

	a = MapDiv (SCREENWIDTH << MAPBITS, max_w);
	b = MapDiv (::ST_Y << MAPBITS, max_h);

	min_scale_mtof = a < b ? a : b;
	max_scale_mtof = MapDiv (SCREENHEIGHT << MAPBITS, 2*PLAYERRADIUS);
}

static void AM_ClipRotatedExtents ()
{
	fixed_t rmin_x, rmin_y, rmax_x, rmax_y;

	if (am_rotate == 0 || (am_rotate == 2 && !viewactive))
	{
		rmin_x = min_x;
		rmin_y = min_y;
		rmax_x = max_x;
		rmax_y = max_y;
	}
	else
	{
		fixed_t xs[4], ys[4];
		int i;

		xs[0] = min_x;	ys[0] = min_y;
		xs[1] = max_x;	ys[1] = min_y;
		xs[2] = max_x;	ys[2] = max_y;
		xs[3] = min_x;	ys[3] = max_y;

		for (i = 0; i < 4; ++i)
		{
			AM_rotatePoint (&xs[i], &ys[i]);
		}
		rmin_x = rmin_y = FIXED_MAX;
		rmax_x = rmax_y = FIXED_MIN;
		for (i = 0; i < 4; ++i)
		{
			if (xs[i] < rmin_x)	rmin_x = xs[i];
			if (xs[i] > rmax_x) rmax_x = xs[i];
			if (ys[i] < rmin_y) rmin_y = ys[i];
			if (ys[i] > rmax_y) rmax_y = ys[i];
		}
	}
	if (m_x + m_w/2 > rmax_x)
		m_x = rmax_x - m_w/2;
	else if (m_x + m_w/2 < rmin_x)
		m_x = rmin_x - m_w/2;

	if (m_y + m_h/2 > rmax_y)
		m_y = rmax_y - m_h/2;
	else if (m_y + m_h/2 < rmin_y)
		m_y = rmin_y - m_h/2;

	m_x2 = m_x + m_w;
	m_y2 = m_y + m_h;
}

static void AM_ScrollParchment (fixed_t dmapx, fixed_t dmapy)
{
	mapxstart -= MulScale12 (dmapx, scale_mtof);
	mapystart -= MulScale12 (dmapy, scale_mtof);

	if (mapback != NULL)
	{
		int pwidth = mapback->GetWidth() << MAPBITS;
		int pheight = mapback->GetHeight() << MAPBITS;

		while(mapxstart > 0)
			mapxstart -= pwidth;
		while(mapxstart <= -pwidth)
			mapxstart += pwidth;
		while(mapystart > 0)
			mapystart -= pheight;
		while(mapystart <= -pheight)
			mapystart += pheight;
	}
}

//
//
//
void AM_changeWindowLoc ()
{
	if (0 != (m_paninc.x | m_paninc.y))
	{
		followplayer = 0;
		f_oldloc.x = FIXED_MAX;
	}

	int oldmx = m_x, oldmy = m_y;

	m_x += Scale (m_paninc.x, SCREENWIDTH, 320);
	m_y += Scale (m_paninc.y, SCREENHEIGHT, 200);

	AM_ClipRotatedExtents ();
	AM_ScrollParchment (m_x-oldmx, oldmy-m_y);
}


//
//
//
void AM_initVariables ()
{
	int pnum;

	automapactive = true;

	f_oldloc.x = FIXED_MAX;
	amclock = 0;

	m_paninc.x = m_paninc.y = 0;
	ftom_zoommul = MAPUNIT;
	mtof_zoommul = MAPUNIT;

	m_w = FTOM(SCREENWIDTH);
	m_h = FTOM(SCREENHEIGHT);

	// find player to center on initially
	if (!playeringame[pnum = consoleplayer])
		for (pnum=0;pnum<MAXPLAYERS;pnum++)
			if (playeringame[pnum])
				break;

	m_x = (players[pnum].camera->x >> FRACTOMAPBITS) - m_w/2;
	m_y = (players[pnum].camera->y >> FRACTOMAPBITS) - m_h/2;
	AM_changeWindowLoc();

	// for saving & restoring
	old_m_x = m_x;
	old_m_y = m_y;
	old_m_w = m_w;
	old_m_h = m_h;
}

static void GetComponents (int color, DWORD *palette, float &r, float &g, float &b)
{
	if (palette)
		color = palette[color];

	r = (float)RPART(color);
	g = (float)GPART(color);
	b = (float)BPART(color);
}

static void AM_initColors (bool overlayed)
{
	static DWORD *lastpal = NULL;
	static int lastback = -1;
	DWORD *palette;

	palette = (DWORD *)GPalette.BaseColors;

	if (lastpal != palette)
	{
		int i, j;

		for (i = j = 0; i < 11; i++, j += 3)
		{
			DoomColors[i] = palette
				? ColorMatcher.Pick (DoomPaletteVals[j], DoomPaletteVals[j+1], DoomPaletteVals[j+2])
				: MAKERGB(DoomPaletteVals[j], DoomPaletteVals[j+1], DoomPaletteVals[j+2]);
		}
	}

	if (overlayed)
	{
		YourColor = am_ovyourcolor.GetIndex ();
		SecretSectorColor = SecretWallColor = WallColor = am_ovwallcolor.GetIndex ();
		ThingColor_Item = am_ovthingcolor_item.GetIndex();
		ThingColor_Friend = am_ovthingcolor_friend.GetIndex();
		ThingColor_Monster = am_ovthingcolor_monster.GetIndex();
		ThingColor = am_ovthingcolor.GetIndex ();
		FDWallColor = CDWallColor = LockedColor = am_ovotherwallscolor.GetIndex ();
		NotSeenColor = TSWallColor = am_ovunseencolor.GetIndex ();
		IntraTeleportColor = InterTeleportColor = am_ovtelecolor.GetIndex ();
	}
	else if (am_usecustomcolors)
	{
		/* Use the custom colors in the am_* cvars */
		Background = am_backcolor.GetIndex ();
		YourColor = am_yourcolor.GetIndex ();
		SecretWallColor = am_secretwallcolor.GetIndex ();
		WallColor = am_wallcolor.GetIndex ();
		TSWallColor = am_tswallcolor.GetIndex ();
		FDWallColor = am_fdwallcolor.GetIndex ();
		CDWallColor = am_cdwallcolor.GetIndex ();
		ThingColor_Item = am_thingcolor_item.GetIndex();
		ThingColor_Friend = am_thingcolor_friend.GetIndex();
		ThingColor_Monster = am_thingcolor_monster.GetIndex();
		ThingColor = am_thingcolor.GetIndex ();
		GridColor = am_gridcolor.GetIndex ();
		XHairColor = am_xhaircolor.GetIndex ();
		NotSeenColor = am_notseencolor.GetIndex ();
		LockedColor = am_lockedcolor.GetIndex ();
		InterTeleportColor = am_interlevelcolor.GetIndex ();
		IntraTeleportColor = am_intralevelcolor.GetIndex ();
		SecretSectorColor = am_secretsectorcolor.GetIndex ();

		DWORD ba = am_backcolor;

		int r = RPART(ba) - 16;
		int g = GPART(ba) - 16;
		int b = BPART(ba) - 16;

		if (r < 0)
			r += 32;
		if (g < 0)
			g += 32;
		if (b < 0)
			b += 32;

		AlmostBackground = ColorMatcher.Pick (r, g, b);
	}
	else
	{ // Use colors corresponding to the original Doom's
		Background = DoomColors[0];
		YourColor = DoomColors[1];
		AlmostBackground = DoomColors[2];
		SecretSectorColor =
			SecretWallColor =
			WallColor = DoomColors[3];
		TSWallColor = DoomColors[4];
		FDWallColor = DoomColors[5];
		LockedColor =
			CDWallColor = DoomColors[6];
		ThingColor_Item =
			ThingColor_Friend =
			ThingColor_Monster =
			ThingColor = DoomColors[7];
		GridColor = DoomColors[8];
		XHairColor = DoomColors[9];
		NotSeenColor = DoomColors[10];
	}

	// initialize the anti-aliased lines
	static struct
	{
		int *color;
		int prevcolor;
		int falseColor;
	} aliasedLines[3] = {
		{ &WallColor, -1, WALLCOLORS },
		{ &FDWallColor, -1, FDWALLCOLORS },
		{ &CDWallColor, -1, CDWALLCOLORS }
	};
	float backRed, backGreen, backBlue;

	GetComponents (Background, palette, backRed, backGreen, backBlue);

	for (int alias = 0; alias < NUMALIASES; alias++)
	{
		if (aliasedLines[alias].prevcolor != *(aliasedLines[alias].color) ||
			lastpal != palette || lastback != Background)
		{
			float foreRed, foreGreen, foreBlue;

			aliasedLines[alias].prevcolor = *(aliasedLines[alias].color);
			GetComponents (*(aliasedLines[alias].color), palette, foreRed, foreGreen, foreBlue);

			for (int i = 0; i < NUMWEIGHTS; i++)
			{
				float step = (float)i;
				float fore = (NUMWEIGHTS-1 - step) / (NUMWEIGHTS-1);
				float back = step / (NUMWEIGHTS-1);
				int red = (int)(backRed * back + foreRed * fore);
				int green = (int)(backGreen * back + foreGreen * fore);
				int blue = (int)(backGreen * back + foreBlue * fore);
// [RH] What was I thinking here?
//				if (palette)
					antialias[alias][i] = ColorMatcher.Pick (red, green, blue);
//				else
//					antialias[alias][i] = MAKERGB(red, green, blue);
			}
			*(aliasedLines[alias].color) = aliasedLines[alias].falseColor;
		}
	}
	lastpal = palette;
	lastback = Background;
}

//
//
//
void AM_loadPics ()
{
	int i;
	char namebuf[9];

	for (i = 0; i < 10; i++)
	{
		sprintf (namebuf, "AMMNUM%d", i);
		marknums[i] = TexMan.CheckForTexture (namebuf, FTexture::TEX_MiscPatch);
	}

	if (mapback == NULL)
	{
		i = Wads.CheckNumForName ("AUTOPAGE");
		if (i >= 0)
		{
			mapback = FTexture::CreateTexture(i, FTexture::TEX_Autopage);
		}
	}
}

void AM_unloadPics ()
{
	if (mapback != NULL)
	{
		delete mapback;
		mapback = NULL;
	}
}

bool AM_clearMarks ()
{
	for (int i = AM_NUMMARKPOINTS-1; i >= 0; i--)
		markpoints[i].x = -1; // means empty
	markpointnum = 0;
	return marknums[0] != -1;
}

//
// should be called at the start of every level
// right now, i figure it out myself
//
void AM_LevelInit ()
{
	leveljuststarted = 0;

	AM_clearMarks();

	AM_findMinMaxBoundaries();
	scale_mtof = MapDiv(min_scale_mtof, (int) (0.7*MAPUNIT));
	if (scale_mtof > max_scale_mtof)
		scale_mtof = min_scale_mtof;
	scale_ftom = MapDiv(MAPUNIT, scale_mtof);
}




//
//
//
void AM_Stop ()
{
	AM_unloadPics ();
	automapactive = false;
	stopped = true;
	BorderNeedRefresh = screen->GetPageCount ();
	viewactive = true;
}

//
//
//
void AM_Start ()
{
	static char lastmap[sizeof(level.mapname)] = "";

	if (!stopped) AM_Stop();
	stopped = false;
	if (strcmp (lastmap, level.mapname))
	{
		AM_LevelInit();
		strcpy (lastmap, level.mapname);
	}
	AM_initVariables();
	AM_loadPics();
}

//
// set the window scale to the maximum size
//
void AM_minOutWindowScale ()
{
	scale_mtof = min_scale_mtof;
	scale_ftom = MapDiv(MAPUNIT, scale_mtof);
}

//
// set the window scale to the minimum size
//
void AM_maxOutWindowScale ()
{
	scale_mtof = max_scale_mtof;
	scale_ftom = MapDiv(MAPUNIT, scale_mtof);
}


CCMD (togglemap)
{
	gameaction = ga_togglemap;
}

void AM_ToggleMap ()
{
	if (gamestate != GS_LEVEL)
		return;

	SB_state = screen->GetPageCount ();
	if (!automapactive)
	{
		AM_Start ();
		viewactive = (am_overlay != 0.f);
	}
	else
	{
		if (am_overlay==1 && viewactive)
		{
			viewactive = false;
			SB_state = screen->GetPageCount ();
		}
		else
		{
			AM_Stop ();
		}
	}
}

//
// Handle events (user inputs) in automap mode
//
bool AM_Responder (event_t *ev)
{
	bool rc;
	static int cheatstate = 0;
	static int bigstate = 0;

	rc = false;

	if (automapactive && ev->type == EV_KeyDown)
	{
		rc = true;
		switch (ev->data1)
		{
		case AM_PANRIGHTKEY: // pan right
			if (!followplayer)
				m_paninc.x = FTOM(F_PANINC);
			else
				rc = false;
			break;
		case AM_PANLEFTKEY: // pan left
			if (!followplayer)
				m_paninc.x = -FTOM(F_PANINC);
			else
				rc = false;
			break;
		case AM_PANUPKEY: // pan up
			if (!followplayer)
				m_paninc.y = FTOM(F_PANINC);
			else
				rc = false;
			break;
		case AM_PANDOWNKEY: // pan down
			if (!followplayer)
				m_paninc.y = -FTOM(F_PANINC);
			else
				rc = false;
			break;
		case AM_ZOOMOUTKEY: // zoom out
		case AM_ZOOMOUTKEY2:
			mtof_zoommul = M_ZOOMOUT;
			ftom_zoommul = M_ZOOMIN;
			break;
		case AM_ZOOMINKEY: // zoom in
		case AM_ZOOMINKEY2:
			mtof_zoommul = M_ZOOMIN;
			ftom_zoommul = M_ZOOMOUT;
			break;
		case AM_GOBIGKEY:
			bigstate = !bigstate;
			if (bigstate)
			{
				AM_saveScaleAndLoc();
				AM_minOutWindowScale();
			}
			else
				AM_restoreScaleAndLoc();
			break;
		default:
			switch (ev->data2)
			{
			case AM_FOLLOWKEY:
				followplayer = !followplayer;
				f_oldloc.x = FIXED_MAX;
				Printf ("%s\n", GStrings(followplayer ? "AMSTR_FOLLOWON" : "AMSTR_FOLLOWOFF"));
				break;
			case AM_GRIDKEY:
				grid = !grid;
				Printf ("%s\n", GStrings(grid ? "AMSTR_GRIDON" : "AMSTR_GRIDOFF"));
				break;
			case AM_MARKKEY:
				if (AM_addMark())
				{
					Printf ("%s %d\n", GStrings("AMSTR_MARKEDSPOT"), markpointnum);
				}
				else
				{
					rc = false;
				}
				break;
			case AM_CLEARMARKKEY:
				if (AM_clearMarks())
				{
					Printf ("%s\n", GStrings("AMSTR_MARKSCLEARED"));
				}
				else
				{
					rc = false;
				}
				break;
			default:
				cheatstate = 0;
				rc = false;
			}
		}
	}
	else if (ev->type == EV_KeyUp)
	{
		rc = false;
		switch (ev->data1)
		{
		case AM_PANRIGHTKEY:
			if (!followplayer) m_paninc.x = 0;
			break;
		case AM_PANLEFTKEY:
			if (!followplayer) m_paninc.x = 0;
			break;
		case AM_PANUPKEY:
			if (!followplayer) m_paninc.y = 0;
			break;
		case AM_PANDOWNKEY:
			if (!followplayer) m_paninc.y = 0;
			break;
		case AM_ZOOMOUTKEY:
		case AM_ZOOMOUTKEY2:
		case AM_ZOOMINKEY:
		case AM_ZOOMINKEY2:
			mtof_zoommul = MAPUNIT;
			ftom_zoommul = MAPUNIT;
			break;
		}
	}

	return rc;
}


//
// Zooming
//
void AM_changeWindowScale ()
{
	// Change the scaling multipliers
	scale_mtof = MapMul(scale_mtof, mtof_zoommul);
	scale_ftom = MapDiv(MAPUNIT, scale_mtof);

	if (scale_mtof < min_scale_mtof)
		AM_minOutWindowScale();
	else if (scale_mtof > max_scale_mtof)
		AM_maxOutWindowScale();
}


//
//
//
void AM_doFollowPlayer ()
{
	fixed_t sx, sy;

    if (players[consoleplayer].camera != NULL &&
		(f_oldloc.x != players[consoleplayer].camera->x ||
		 f_oldloc.y != players[consoleplayer].camera->y))
	{
		m_x = (players[consoleplayer].camera->x >> FRACTOMAPBITS) - m_w/2;
		m_y = (players[consoleplayer].camera->y >> FRACTOMAPBITS) - m_h/2;
		m_x2 = m_x + m_w;
		m_y2 = m_y + m_h;

  		// do the parallax parchment scrolling.
		sx = (players[consoleplayer].camera->x - f_oldloc.x) >> FRACTOMAPBITS;
		sy = (f_oldloc.y - players[consoleplayer].camera->y) >> FRACTOMAPBITS;
		if (am_rotate == 1 || (am_rotate == 2 && viewactive))
		{
			AM_rotate (&sx, &sy, players[consoleplayer].camera->angle - ANG90);
		}
		AM_ScrollParchment (sx, sy);

		f_oldloc.x = players[consoleplayer].camera->x;
		f_oldloc.y = players[consoleplayer].camera->y;
	}
}

//
// Updates on Game Tick
//
void AM_Ticker ()
{
	if (!automapactive)
		return;

	amclock++;

	if (followplayer)
		AM_doFollowPlayer();

	// Change the zoom if necessary
	if (ftom_zoommul != MAPUNIT)
		AM_changeWindowScale();

	// Change x,y location
	//if (m_paninc.x || m_paninc.y)
		AM_changeWindowLoc();
}


//
// Clear automap frame buffer.
//
void AM_clearFB (int color)
{
	if (mapback == NULL || !am_drawmapback)
	{
		screen->Clear (0, 0, f_w, f_h, color);
	}
	else
	{
		int pwidth = mapback->GetWidth();
		int pheight = mapback->GetHeight();
		int x, y;

		//blit the automap background to the screen.
		for (y = mapystart >> MAPBITS; y < f_h; y += pheight)
		{
			for (x = mapxstart >> MAPBITS; x < f_w; x += pwidth)
			{
				screen->DrawTexture (mapback, x, y, DTA_ClipBottom, f_h, TAG_DONE);
			}
		}
	}
}


//
// Automap clipping of lines.
//
// Based on Cohen-Sutherland clipping algorithm but with a slightly
// faster reject and precalculated slopes.  If the speed is needed,
// use a hash algorithm to handle the common cases.
//
bool AM_clipMline (mline_t *ml, fline_t *fl)
{
	enum {
		LEFT	=1,
		RIGHT	=2,
		BOTTOM	=4,
		TOP		=8
	};

	register int outcode1 = 0;
	register int outcode2 = 0;
	register int outside;

	fpoint_t tmp;
	int dx;
	int dy;

#define DOOUTCODE(oc, mx, my) \
	(oc) = 0; \
	if ((my) < 0) (oc) |= TOP; \
	else if ((my) >= f_h) (oc) |= BOTTOM; \
	if ((mx) < 0) (oc) |= LEFT; \
	else if ((mx) >= f_w) (oc) |= RIGHT;

	// do trivial rejects and outcodes
	if (ml->a.y > m_y2)
		outcode1 = TOP;
	else if (ml->a.y < m_y)
		outcode1 = BOTTOM;

	if (ml->b.y > m_y2)
		outcode2 = TOP;
	else if (ml->b.y < m_y)
		outcode2 = BOTTOM;

	if (outcode1 & outcode2)
		return false; // trivially outside

	if (ml->a.x < m_x)
		outcode1 |= LEFT;
	else if (ml->a.x > m_x2)
		outcode1 |= RIGHT;

	if (ml->b.x < m_x)
		outcode2 |= LEFT;
	else if (ml->b.x > m_x2)
		outcode2 |= RIGHT;

	if (outcode1 & outcode2)
		return false; // trivially outside

	// transform to frame-buffer coordinates.
	fl->a.x = CXMTOF(ml->a.x);
	fl->a.y = CYMTOF(ml->a.y);
	fl->b.x = CXMTOF(ml->b.x);
	fl->b.y = CYMTOF(ml->b.y);

	DOOUTCODE(outcode1, fl->a.x, fl->a.y);
	DOOUTCODE(outcode2, fl->b.x, fl->b.y);

	if (outcode1 & outcode2)
		return false;

	while (outcode1 | outcode2) {
		// may be partially inside box
		// find an outside point
		if (outcode1)
			outside = outcode1;
		else
			outside = outcode2;

		// clip to each side
		if (outside & TOP)
		{
			dy = fl->a.y - fl->b.y;
			dx = fl->b.x - fl->a.x;
			tmp.x = fl->a.x + (dx*(fl->a.y))/dy;
			tmp.y = 0;
		}
		else if (outside & BOTTOM)
		{
			dy = fl->a.y - fl->b.y;
			dx = fl->b.x - fl->a.x;
			tmp.x = fl->a.x + (dx*(fl->a.y-f_h))/dy;
			tmp.y = f_h-1;
		}
		else if (outside & RIGHT)
		{
			dy = fl->b.y - fl->a.y;
			dx = fl->b.x - fl->a.x;
			tmp.y = fl->a.y + (dy*(f_w-1 - fl->a.x))/dx;
			tmp.x = f_w-1;
		}
		else if (outside & LEFT)
		{
			dy = fl->b.y - fl->a.y;
			dx = fl->b.x - fl->a.x;
			tmp.y = fl->a.y + (dy*(-fl->a.x))/dx;
			tmp.x = 0;
		}

		if (outside == outcode1)
		{
			fl->a = tmp;
			DOOUTCODE(outcode1, fl->a.x, fl->a.y);
		}
		else
		{
			fl->b = tmp;
			DOOUTCODE(outcode2, fl->b.x, fl->b.y);
		}

		if (outcode1 & outcode2)
			return false; // trivially outside
	}

	return true;
}
#undef DOOUTCODE


//
// Classic Bresenham w/ whatever optimizations needed for speed
//
void AM_drawFline (fline_t *fl, int color)
{
	fl->a.x += f_x;
	fl->b.x += f_x;
	fl->a.y += f_y;
	fl->b.y += f_y;

	// [BC/ZDoomGL] Just let the OpenGL renderer do everything.
	if ( OPENGL_GetCurrentRenderer( ) == RENDERER_OPENGL )
	{
		GL_DrawLine(fl->a.x, fl->a.y, fl->b.x, fl->b.y, color);
		return;
	}

	switch (color)
	{
		case WALLCOLORS:
			DrawWuLine (fl->a.x, fl->a.y, fl->b.x, fl->b.y, &antialias[0][0]);
			break;
		case FDWALLCOLORS:
			DrawWuLine (fl->a.x, fl->a.y, fl->b.x, fl->b.y, &antialias[1][0]);
			break;
		case CDWALLCOLORS:
			DrawWuLine (fl->a.x, fl->a.y, fl->b.x, fl->b.y, &antialias[2][0]);
			break;
		default:
			DrawTransWuLine (fl->a.x, fl->a.y, fl->b.x, fl->b.y, color);
			break;
#if 0
  		{
			register int x;
			register int y;
			register int dx;
			register int dy;
			register int sx;
			register int sy;
			register int ax;
			register int ay;
			register int d;

#define PUTDOTP(xx,yy,cc) fb[(yy)*f_p+(xx)]=(cc)

			dx = fl->b.x - fl->a.x;
			ax = 2 * (dx<0 ? -dx : dx);
			sx = dx<0 ? -1 : 1;

			dy = fl->b.y - fl->a.y;
			ay = 2 * (dy<0 ? -dy : dy);
			sy = dy<0 ? -1 : 1;

			x = fl->a.x;
			y = fl->a.y;

			if (ax > ay) {
				d = ay - ax/2;
				for (;;) {
					PUTDOTP(x,y,(BYTE)color);
					if (x == fl->b.x)
						return;
					if (d>=0) {
						y += sy;
						d -= ax;
					}
					x += sx;
					d += ay;
				}
			} else {
				d = ax - ay/2;
				for (;;) {
					PUTDOTP(x, y, (BYTE)color);
					if (y == fl->b.y)
						return;
					if (d >= 0) {
						x += sx;
						d -= ay;
					}
					y += sy;
					d += ax;
				}
			}
		}
#endif
	}
}

/* Wu antialiased line drawer.
 * (X0,Y0),(X1,Y1) = line to draw
 * BaseColor = color # of first color in block used for antialiasing, the
 *          100% intensity version of the drawing color
 * NumLevels = size of color block, with BaseColor+NumLevels-1 being the
 *          0% intensity version of the drawing color
 * IntensityBits = log base 2 of NumLevels; the # of bits used to describe
 *          the intensity of the drawing color. 2**IntensityBits==NumLevels
 */
void PUTDOT (int xx, int yy,BYTE *cc, BYTE *cm)
{
	static int oldyy;
	static int oldyyshifted;
	BYTE *oldcc=cc;

#if 0
	if(xx < 32)
		cc += 7-(xx>>2);
	else if(xx > (finit_width - 32))
		cc += 7-((finit_width-xx) >> 2);
//	if(cc==oldcc) //make sure that we don't double fade the corners.
//	{
		if(yy < 32)
			cc += 7-(yy>>2);
		else if(yy > (finit_height - 32))
			cc += 7-((finit_height-yy) >> 2);
//	}
#endif
	if (cm != NULL && cc > cm)
	{
		cc = cm;
	}
	else if (cc > oldcc+6) // don't let the color escape from the fade table...
	{
		cc=oldcc+6;
	}
	if (yy == oldyy+1)
	{
		oldyy++;
		oldyyshifted += SCREENPITCH;
	}
	else if (yy == oldyy-1)
	{
		oldyy--;
		oldyyshifted -= SCREENPITCH;
	}
	else if (yy != oldyy)
	{
		oldyy = yy;
		oldyyshifted = yy*SCREENPITCH;
	}
	fb[oldyyshifted+xx] = *(cc);
}

void DrawWuLine (int x0, int y0, int x1, int y1, BYTE *baseColor)
{
	int deltaX, deltaY, xDir;

	if (viewactive)
	{
		// If the map is overlayed, use the translucent line drawer
		// code to avoid nasty discolored spots along the edges of
		// the lines. Otherwise, use this one to avoid reading from
		// the framebuffer.
		DrawTransWuLine (x0, y0, x1, y1, *baseColor);
		return;
	}

	// Make sure the line runs top to bottom
	if (y0 > y1)
	{
		int temp = y0; y0 = y1; y1 = temp;
		temp = x0; x0 = x1; x1 = temp;
	}

	// Draw the initial pixel, which is always exactly intersected by
	// the line and so needs no weighting
	PUTDOT (x0, y0, &baseColor[0], NULL);

	if ((deltaX = x1 - x0) >= 0)
	{
		xDir = 1;
	}
	else
	{
		xDir = -1;
		deltaX = -deltaX;	// make deltaX positive
	}
	// Special-case horizontal, vertical, and diagonal lines, which
	// require no weighting because they go right through the center of
    // every pixel
	if ((deltaY = y1 - y0) == 0)
	{ // horizontal line
		while (deltaX-- != 0)
		{
			x0 += xDir;
			PUTDOT (x0, y0, &baseColor[0], NULL);
		}
		return;
	}
	if (deltaX == 0)
	{ // vertical line
		do
		{
			y0++;
			PUTDOT (x0, y0, &baseColor[0], NULL);
		} while (--deltaY != 0);
		return;
	}
	if (deltaX == deltaY)
	{ // diagonal line.
		do
		{
			x0 += xDir;
			y0++;
			PUTDOT (x0, y0, &baseColor[0], NULL);
		} while (--deltaY != 0);
		return;
	}

	// Line is not horizontal, diagonal, or vertical
	fixed_t errorAcc = 0;	// initialize the line error accumulator to 0

	// Is this an X-major or Y-major line?
	if (deltaY > deltaX)
	{
		// Y-major line; calculate 16-bit fixed-point fractional part of a
		// pixel that X advances each time Y advances 1 pixel, truncating the
		// result so that we won't overrun the endpoint along the X axis
		fixed_t errorAdj = ((DWORD) deltaX << 16) / (DWORD) deltaY & 0xffff;

		// Draw all pixels other than the first and last
		if (xDir < 0)
		{
			while (--deltaY)
			{
				errorAcc += errorAdj;
				y0++;	// Y-major, so always advance Y

				// The most significant bits of ErrorAcc give us the intensity
				// weighting for this pixel, and the complement of the weighting
				// for the paired pixel
				int weighting = (errorAcc >> WEIGHTSHIFT) & WEIGHTMASK;
				PUTDOT (x0 - (errorAcc >> 16), y0, &baseColor[weighting], &baseColor[NUMWEIGHTS-1]);
				PUTDOT (x0 - (errorAcc >> 16) - 1, y0,
						&baseColor[WEIGHTMASK - weighting], &baseColor[NUMWEIGHTS-1]);
			}
		}
		else
		{
			while (--deltaY)
			{
				errorAcc += errorAdj;
				y0++;	// Y-major, so always advance Y
				int weighting = (errorAcc >> WEIGHTSHIFT) & WEIGHTMASK;
				PUTDOT (x0 + (errorAcc >> 16), y0, &baseColor[weighting], &baseColor[NUMWEIGHTS-1]);
				PUTDOT (x0 + (errorAcc >> 16) + 1, y0,
						&baseColor[WEIGHTMASK - weighting], &baseColor[NUMWEIGHTS-1]);
			}
		}
	}
	else
	{
		// It's an X-major line; calculate 16-bit fixed-point fractional part of a
		// pixel that Y advances each time X advances 1 pixel, truncating the
		// result to avoid overrunning the endpoint along the X axis
		fixed_t errorAdj = ((DWORD) deltaY << 16) / (DWORD) deltaX;

		// Draw all pixels other than the first and last
		while (--deltaX)
		{
			errorAcc += errorAdj;
			x0 += xDir;	// X-major, so always advance X
			int weighting = (errorAcc >> WEIGHTSHIFT) & WEIGHTMASK;
			PUTDOT (x0, y0 + (errorAcc >> 16), &baseColor[weighting], &baseColor[NUMWEIGHTS-1]);
			PUTDOT (x0, y0 + (errorAcc >> 16) + 1,
					&baseColor[WEIGHTMASK - weighting], &baseColor[NUMWEIGHTS-1]);
		}
	}

	// Draw the final pixel, which is always exactly intersected by the line
	// and so needs no weighting
	PUTDOT (x1, y1, &baseColor[0], NULL);
}

void PUTTRANSDOT (int xx, int yy, int basecolor, int level)
{
	static int oldyy;
	static int oldyyshifted;

#if 0
	if(xx < 32)
		cc += 7-(xx>>2);
	else if(xx > (finit_width - 32))
		cc += 7-((finit_width-xx) >> 2);
//	if(cc==oldcc) //make sure that we don't double fade the corners.
//	{
		if(yy < 32)
			cc += 7-(yy>>2);
		else if(yy > (finit_height - 32))
			cc += 7-((finit_height-yy) >> 2);
//	}
	if(cc > cm && cm != NULL)
	{
		cc = cm;
	}
	else if(cc > oldcc+6) // don't let the color escape from the fade table...
	{
		cc=oldcc+6;
	}
#endif
	if (yy == oldyy+1)
	{
		oldyy++;
		oldyyshifted += SCREENPITCH;
	}
	else if (yy == oldyy-1)
	{
		oldyy--;
		oldyyshifted -= SCREENPITCH;
	}
	else if (yy != oldyy)
	{
		oldyy = yy;
		oldyyshifted = yy*SCREENPITCH;
	}

	BYTE *spot = fb + oldyyshifted + xx;
	DWORD *bg2rgb = Col2RGB8[1+level];
	DWORD *fg2rgb = Col2RGB8[63-level];
	DWORD fg = fg2rgb[basecolor];
	DWORD bg = bg2rgb[*spot];
	bg = (fg+bg) | 0x1f07c1f;
	*spot = RGB32k[0][0][bg&(bg>>15)];
}

void DrawTransWuLine (int x0, int y0, int x1, int y1, BYTE baseColor)
{
	int deltaX, deltaY, xDir;

	if (y0 > y1)
	{
		int temp = y0; y0 = y1; y1 = temp;
		temp = x0; x0 = x1; x1 = temp;
	}

	PUTTRANSDOT (x0, y0, baseColor, 0);

	if ((deltaX = x1 - x0) >= 0)
	{
		xDir = 1;
	}
	else
	{
		xDir = -1;
		deltaX = -deltaX;
	}

	if ((deltaY = y1 - y0) == 0)
	{ // horizontal line
		if (x0 > x1)
		{
			swap (x0, x1);
		}
		memset (screen->GetBuffer() + y0*screen->GetPitch() + x0, baseColor, deltaX+1);
		return;
	}
	if (deltaX == 0)
	{ // vertical line
		BYTE *spot = screen->GetBuffer() + y0*screen->GetPitch() + x0;
		int pitch = screen->GetPitch ();
		do
		{
			*spot = baseColor;
			spot += pitch;
		} while (--deltaY != 0);
		return;
	}
	if (deltaX == deltaY)
	{ // diagonal line.
		BYTE *spot = screen->GetBuffer() + y0*screen->GetPitch() + x0;
		int advance = screen->GetPitch() + xDir;
		do
		{
			*spot = baseColor;
			spot += advance;
		} while (--deltaY != 0);
		return;
	}

	// line is not horizontal, diagonal, or vertical
	fixed_t errorAcc = 0;

	if (deltaY > deltaX)
	{ // y-major line
		fixed_t errorAdj = (((unsigned)deltaX << 16) / (unsigned)deltaY) & 0xffff;
		if (xDir < 0)
		{
			if (WeightingScale == 0)
			{
				while (--deltaY)
				{
					errorAcc += errorAdj;
					y0++;
					int weighting = (errorAcc >> WEIGHTSHIFT) & WEIGHTMASK;
					PUTTRANSDOT (x0 - (errorAcc >> 16), y0, baseColor, weighting);
					PUTTRANSDOT (x0 - (errorAcc >> 16) - 1, y0,
							baseColor, WEIGHTMASK - weighting);
				}
			}
			else
			{
				while (--deltaY)
				{
					errorAcc += errorAdj;
					y0++;
					int weighting = ((errorAcc * WeightingScale) >> (WEIGHTSHIFT+8)) & WEIGHTMASK;
					PUTTRANSDOT (x0 - (errorAcc >> 16), y0, baseColor, weighting);
					PUTTRANSDOT (x0 - (errorAcc >> 16) - 1, y0,
							baseColor, WEIGHTMASK - weighting);
				}
			}
		}
		else
		{
			if (WeightingScale == 0)
			{
				while (--deltaY)
				{
					errorAcc += errorAdj;
					y0++;
					int weighting = (errorAcc >> WEIGHTSHIFT) & WEIGHTMASK;
					PUTTRANSDOT (x0 + (errorAcc >> 16), y0, baseColor, weighting);
					PUTTRANSDOT (x0 + (errorAcc >> 16) + xDir, y0,
							baseColor, WEIGHTMASK - weighting);
				}
			}
			else
			{
				while (--deltaY)
				{
					errorAcc += errorAdj;
					y0++;
					int weighting = ((errorAcc * WeightingScale) >> (WEIGHTSHIFT+8)) & WEIGHTMASK;
					PUTTRANSDOT (x0 + (errorAcc >> 16), y0, baseColor, weighting);
					PUTTRANSDOT (x0 + (errorAcc >> 16) + xDir, y0,
							baseColor, WEIGHTMASK - weighting);
				}
			}
		}
	}
	else
	{ // x-major line
		fixed_t errorAdj = (((DWORD) deltaY << 16) / (DWORD) deltaX) & 0xffff;

		if (WeightingScale == 0)
		{
			while (--deltaX)
			{
				errorAcc += errorAdj;
				x0 += xDir;
				int weighting = (errorAcc >> WEIGHTSHIFT) & WEIGHTMASK;
				PUTTRANSDOT (x0, y0 + (errorAcc >> 16), baseColor, weighting);
				PUTTRANSDOT (x0, y0 + (errorAcc >> 16) + 1,
						baseColor, WEIGHTMASK - weighting);
			}
		}
		else
		{
			while (--deltaX)
			{
				errorAcc += errorAdj;
				x0 += xDir;
				int weighting = ((errorAcc * WeightingScale) >> (WEIGHTSHIFT+8)) & WEIGHTMASK;
				PUTTRANSDOT (x0, y0 + (errorAcc >> 16), baseColor, weighting);
				PUTTRANSDOT (x0, y0 + (errorAcc >> 16) + 1,
						baseColor, WEIGHTMASK - weighting);
			}
		}
	}

	PUTTRANSDOT (x1, y1, baseColor, 0);
}

//
// Clip lines, draw visible parts of lines.
//
void AM_drawMline (mline_t *ml, int color)
{
	static fline_t fl;

	if (AM_clipMline (ml, &fl))
		AM_drawFline (&fl, color); // draws it on frame buffer using fb coords
}



//
// Draws flat (floor/ceiling tile) aligned grid lines.
//
void AM_drawGrid (int color)
{
	fixed_t x, y;
	fixed_t start, end;
	mline_t ml;
	fixed_t minlen, extx, exty;
	fixed_t minx, miny;

	// [RH] Calculate a minimum for how long the grid lines should be so that
	// they cover the screen at any rotation.
	minlen = (fixed_t)sqrtf ((float)m_w*(float)m_w + (float)m_h*(float)m_h);
	extx = (minlen - m_w) / 2;
	exty = (minlen - m_h) / 2;

	minx = m_x;
	miny = m_y;

	// Figure out start of vertical gridlines
	start = minx - extx;
	if ((start-bmaporgx)%(MAPBLOCKUNITS<<MAPBITS))
		start += (MAPBLOCKUNITS<<MAPBITS)
			- ((start-bmaporgx)%(MAPBLOCKUNITS<<MAPBITS));
	end = minx + minlen - extx;

	// draw vertical gridlines
	for (x = start; x < end; x += (MAPBLOCKUNITS<<MAPBITS))
	{
		ml.a.x = x;
		ml.b.x = x;
		ml.a.y = miny - exty;
		ml.b.y = ml.a.y + minlen;
		if (am_rotate == 1 || (am_rotate == 2 && viewactive))
		{
			AM_rotatePoint (&ml.a.x, &ml.a.y);
			AM_rotatePoint (&ml.b.x, &ml.b.y);
		}
		AM_drawMline(&ml, color);
	}

	// Figure out start of horizontal gridlines
	start = miny - exty;
	if ((start-bmaporgy)%(MAPBLOCKUNITS<<MAPBITS))
		start += (MAPBLOCKUNITS<<MAPBITS)
			- ((start-bmaporgy)%(MAPBLOCKUNITS<<MAPBITS));
	end = miny + minlen - exty;

	// draw horizontal gridlines
	for (y=start; y<end; y+=(MAPBLOCKUNITS<<MAPBITS))
	{
		ml.a.x = minx - extx;
		ml.b.x = ml.a.x + minlen;
		ml.a.y = y;
		ml.b.y = y;
		if (am_rotate == 1 || (am_rotate == 2 && viewactive))
		{
			AM_rotatePoint (&ml.a.x, &ml.a.y);
			AM_rotatePoint (&ml.b.x, &ml.b.y);
		}
		AM_drawMline (&ml, color);
	}
}

//
// Determines visible lines, draws them.
// This is LineDef based, not LineSeg based.
//
void AM_drawWalls (bool allmap)
{
	int i;
	static mline_t l;

	for (i = 0; i < numlines; i++)
	{
		l.a.x = lines[i].v1->x >> FRACTOMAPBITS;
		l.a.y = lines[i].v1->y >> FRACTOMAPBITS;
		l.b.x = lines[i].v2->x >> FRACTOMAPBITS;
		l.b.y = lines[i].v2->y >> FRACTOMAPBITS;

		if (am_rotate == 1 || (am_rotate == 2 && viewactive))
		{
			AM_rotatePoint (&l.a.x, &l.a.y);
			AM_rotatePoint (&l.b.x, &l.b.y);
		}

		if (am_cheat != 0 || (lines[i].flags & ML_MAPPED))
		{
			if ((lines[i].flags & ML_DONTDRAW) && am_cheat == 0)
				continue;

			if (!lines[i].backsector)
			{
				if (lines[i].frontsector->oldspecial &&
					(am_map_secrets==2 || (am_map_secrets==1 && !(lines[i].frontsector->special&SECRET_MASK))))
				{
					// map secret sectors like Boom
					AM_drawMline(&l, SecretSectorColor);
				}
				else
				{
					AM_drawMline(&l, WallColor);
				}
			}
			else
			{
				if ((lines[i].special == Teleport ||
					lines[i].special == Teleport_NoFog ||
					lines[i].special == Teleport_Line) &&
					GET_SPAC(lines[i].flags) != SPAC_MCROSS &&
					am_usecustomcolors)
				{ // intra-level teleporters
					AM_drawMline(&l, IntraTeleportColor);
				}
				else if ((lines[i].special == Teleport_NewMap ||
						 lines[i].special == Teleport_EndGame ||
						 lines[i].special == Exit_Normal ||
						 lines[i].special == Exit_Secret) &&
						 am_usecustomcolors)
				{ // inter-level/game-ending teleporters
					AM_drawMline(&l, InterTeleportColor);
				}
				else if (lines[i].flags & ML_SECRET)
				{ // secret door
					if (am_cheat != 0)
						AM_drawMline(&l, SecretWallColor);
				    else
						AM_drawMline(&l, WallColor);
				}
				else if (lines[i].special == Door_LockedRaise ||
						 lines[i].special == ACS_LockedExecute ||
						 (lines[i].special == Generic_Door && lines[i].args[4]!=0))
				{
					if (am_usecustomcolors)
					{
						int P_GetMapColorForLock(int lock);
						int lock;


						if (lines[i].special==Door_LockedRaise) lock=lines[i].args[3];
						else lock=lines[i].args[4];

						int color = P_GetMapColorForLock(lock);

						if (color > 0)
						{
							color = ColorMatcher.Pick(RPART(color), GPART(color), BPART(color));
						}
						else color = LockedColor;

						AM_drawMline (&l, color);
					}
					else
						AM_drawMline (&l, LockedColor);  // locked special
				}
				else if (lines[i].backsector->floorplane
					  != lines[i].frontsector->floorplane)
				{
					AM_drawMline(&l, FDWallColor); // floor level change
				}
				else if (lines[i].backsector->ceilingplane
					  != lines[i].frontsector->ceilingplane)
				{
					AM_drawMline(&l, CDWallColor); // ceiling level change
				}
				else if (am_cheat != 0)
				{
					AM_drawMline(&l, TSWallColor);
				}
			}
		}
		else if (allmap)
		{
			if (!(lines[i].flags & ML_DONTDRAW))
				AM_drawMline(&l, NotSeenColor);
		}
    }
}


//
// Rotation in 2D.
// Used to rotate player arrow line character.
//
void AM_rotate (fixed_t *x, fixed_t *y, angle_t a)
{
	fixed_t tmpx;

	a >>= ANGLETOFINESHIFT;
	tmpx = DMulScale16 (*x,finecosine[a],*y,-finesine[a]);
	*y = DMulScale16 (*x,finesine[a],*y,finecosine[a]);
	*x = tmpx;
}

void AM_rotatePoint (fixed_t *x, fixed_t *y)
{
	*x -= players[consoleplayer].camera->x >> FRACTOMAPBITS;
	*y -= players[consoleplayer].camera->y >> FRACTOMAPBITS;
	AM_rotate (x, y, ANG90 - players[consoleplayer].camera->angle);
	*x += players[consoleplayer].camera->x >> FRACTOMAPBITS;
	*y += players[consoleplayer].camera->y >> FRACTOMAPBITS;
}

void
AM_drawLineCharacter
( const mline_t *lineguy,
  int		lineguylines,
  fixed_t	scale,
  angle_t	angle,
  int		color,
  fixed_t	x,
  fixed_t	y )
{
	int		i;
	mline_t	l;

	for (i=0;i<lineguylines;i++) {
		l.a.x = lineguy[i].a.x;
		l.a.y = lineguy[i].a.y;

		if (scale) {
			l.a.x = MapMul(scale, l.a.x);
			l.a.y = MapMul(scale, l.a.y);
		}

		if (angle)
			AM_rotate(&l.a.x, &l.a.y, angle);

		l.a.x += x;
		l.a.y += y;

		l.b.x = lineguy[i].b.x;
		l.b.y = lineguy[i].b.y;

		if (scale) {
			l.b.x = MapMul(scale, l.b.x);
			l.b.y = MapMul(scale, l.b.y);
		}

		if (angle)
			AM_rotate(&l.b.x, &l.b.y, angle);

		l.b.x += x;
		l.b.y += y;

		AM_drawMline(&l, color);
	}
}

void AM_drawPlayers ()
{
	angle_t angle;
	int i;

	if ( NETWORK_GetState( ) == NETSTATE_SINGLE )
	{
		if (am_rotate == 1 || (am_rotate == 2 && viewactive))
			angle = ANG90;
		else
			angle = players[consoleplayer].camera->angle;

		if (am_cheat != 0)
			AM_drawLineCharacter
			(cheat_player_arrow, NUMCHEATPLYRLINES, 0,
			 angle, YourColor, players[consoleplayer].camera->x >> FRACTOMAPBITS, players[consoleplayer].camera->y >> FRACTOMAPBITS);
		else
			AM_drawLineCharacter
			(player_arrow, NUMPLYRLINES, 0, angle,
			 YourColor, players[consoleplayer].camera->x >> FRACTOMAPBITS, players[consoleplayer].camera->y >> FRACTOMAPBITS);
		return;
	}

	for (i = 0; i < MAXPLAYERS; i++)
	{
		player_t *p = &players[i];
		int color;
		mpoint_t pt;

		if (!playeringame[i] || p->mo == NULL)
		{
			continue;
		}

		// [BC] Do this in teamgame mode too.
		if (( deathmatch || teamgame ) && !demoplayback &&
			!p->mo->IsTeammate (players[consoleplayer].mo) &&
			p != players[consoleplayer].camera->player)
		{
			continue;
		}

		if (p->mo->alpha < OPAQUE)
		{
			color = AlmostBackground;
		}
		else
		{
			float h, s, v, r, g, b;

			D_GetPlayerColor (i, &h, &s, &v);
			HSVtoRGB (&r, &g, &b, h, s, v);

			color = ColorMatcher.Pick (clamp (int(r*255.f),0,255),
				clamp (int(g*255.f),0,255), clamp (int(b*255.f),0,255));
		}

		if (p->mo != NULL)
		{
			pt.x = p->mo->x >> FRACTOMAPBITS;
			pt.y = p->mo->y >> FRACTOMAPBITS;
			angle = p->mo->angle;

			if (am_rotate == 1 || (am_rotate == 2 && viewactive))
			{
				AM_rotatePoint (&pt.x, &pt.y);
				angle -= players[consoleplayer].camera->angle - ANG90;
			}

			AM_drawLineCharacter
				(player_arrow, NUMPLYRLINES, 0, angle,
				color, pt.x, pt.y);
		}
    }
}

void AM_drawThings (int _color)
{
	int color;
	int		 i;
	AActor*	 t;
	mpoint_t p;
	angle_t	 angle;

	for (i=0;i<numsectors;i++)
	{
		t = sectors[i].thinglist;
		while (t)
		{
			p.x = t->x >> FRACTOMAPBITS;
			p.y = t->y >> FRACTOMAPBITS;
			angle = t->angle;

			if (am_rotate == 1 || (am_rotate == 2 && viewactive))
			{
				AM_rotatePoint (&p.x, &p.y);
				angle += ANG90 - players[consoleplayer].camera->angle;
			}

			color = ThingColor;

			// use separate colors for special thing types
			if (t->flags3&MF3_ISMONSTER && !(t->flags&MF_CORPSE))
			{
				if (t->flags & MF_FRIENDLY || !(t->flags & MF_COUNTKILL)) color = ThingColor_Friend;
				else color = ThingColor_Monster;
			}
			else if (t->flags&MF_SPECIAL) color = ThingColor_Item;

			AM_drawLineCharacter
			(thintriangle_guy, NUMTHINTRIANGLEGUYLINES,
			 16<<MAPBITS, angle, color, p.x, p.y);

			if (am_cheat >= 3)
			{
				static const mline_t box[4] =
				{
					{ { -MAPUNIT, -MAPUNIT }, {  MAPUNIT, -MAPUNIT } },
					{ {  MAPUNIT, -MAPUNIT }, {  MAPUNIT,  MAPUNIT } },
					{ {  MAPUNIT,  MAPUNIT }, { -MAPUNIT,  MAPUNIT } },
					{ { -MAPUNIT,  MAPUNIT }, { -MAPUNIT, -MAPUNIT } },
				};

				AM_drawLineCharacter (box, 4, t->radius >> FRACTOMAPBITS, angle - t->angle, color, p.x, p.y);
			}
			t = t->snext;
		}
	}
}

static void DrawMarker (FTexture *tex, fixed_t x, fixed_t y, int yadjust,
	INTBOOL flip, int xscale, int yscale, int translation, fixed_t alpha, DWORD alphacolor, int renderstyle)
{
	if (tex == NULL || tex->UseType == FTexture::TEX_Null)
	{
		return;
	}
	if (am_rotate == 1 || (am_rotate == 2 && viewactive))
	{
		AM_rotatePoint (&x, &y);
	}
	screen->DrawTexture (tex, CXMTOF(x) + f_x, CYMTOF(y) + yadjust + f_y,
		DTA_DestWidth, MulScale6 (tex->GetScaledWidth() * CleanXfac, xscale),
		DTA_DestHeight, MulScale6 (tex->GetScaledHeight() * CleanYfac, yscale),
		DTA_ClipTop, f_y,
		DTA_ClipBottom, f_y + f_h,
		DTA_ClipLeft, f_x,
		DTA_ClipRight, f_x + f_w,
		DTA_FlipX, flip,
		DTA_Translation, translation != 0 ? translationtables[(translation&0xff00)>>8] + (translation&0x00ff)*256 : NULL,
		DTA_Alpha, alpha,
		DTA_FillColor, alphacolor,
		DTA_RenderStyle, renderstyle,
		TAG_DONE);
}

void AM_drawMarks ()
{
	for (int i = 0; i < AM_NUMMARKPOINTS; i++)
	{
		if (markpoints[i].x != -1)
		{
			DrawMarker (TexMan(marknums[i]), markpoints[i].x, markpoints[i].y, -3, 0, 64, 64, 0, FRACUNIT, 0, STYLE_Normal);
		}
	}
}

void AM_drawAuthorMarkers ()
{
	// [RH] Draw any actors derived from AMapMarker on the automap.
	// If args[0] is 0, then the actor's sprite is drawn at its own location.
	// Otherwise, its sprite is drawn at the location of any actors whose TIDs match args[0].
	TThinkerIterator<AMapMarker> it (STAT_MAPMARKER);
	AMapMarker *mark;

	while ((mark = it.Next()) != NULL)
	{
		if (mark->flags2 & MF2_DORMANT)
		{
			continue;
		}

		int picnum;
		FTexture *tex;
		WORD flip = 0;

		if (mark->picnum != 0xFFFF)
		{
			tex = TexMan(mark->picnum);
			if (tex->Rotations != 0xFFFF)
			{
				spriteframe_t *sprframe = &SpriteFrames[tex->Rotations];
				picnum = sprframe->Texture[0];
				flip = sprframe->Flip & 1;
				tex = TexMan[picnum];
			}
		}
		else
		{
			spritedef_t *sprdef = &sprites[mark->sprite];
			if (mark->frame >= sprdef->numframes)
			{
				continue;
			}
			else
			{
				spriteframe_t *sprframe = &SpriteFrames[sprdef->spriteframes + mark->frame];
				picnum = sprframe->Texture[0];
				flip = sprframe->Flip & 1;
				tex = TexMan[picnum];
			}
		}
		FActorIterator it (mark->args[0]);
		AActor *marked = mark->args[0] == 0 ? mark : it.Next();

		while (marked != NULL)
		{
			if (mark->args[1] == 0 || (mark->args[1] == 1 && marked->Sector->MoreFlags & SECF_DRAWN))
			{
				DrawMarker (tex, marked->x >> FRACTOMAPBITS, marked->y >> FRACTOMAPBITS, 0,
					flip, mark->xscale+1, mark->yscale+1, mark->Translation,
					mark->alpha, mark->alphacolor, mark->RenderStyle);
			}
			marked = mark->args[0] != 0 ? it.Next() : NULL;
		}
	}
}

void AM_drawCrosshair (int color)
{
	fb[f_p*((f_h+1)/2)+(f_w/2)] = (BYTE)color; // single point for now
}

void AM_Drawer ()
{
	if (!automapactive)
		return;

	bool allmap = (level.flags & LEVEL_ALLMAP) != 0;
	bool allthings = allmap && players[consoleplayer].mo->FindInventory<APowerScanner>() != NULL;

	AM_initColors (viewactive);

	fb = screen->GetBuffer ();
	if (!viewactive)
	{
		// [RH] Set f_? here now to handle automap overlaying
		// and view size adjustments.
		f_x = f_y = 0;
		f_w = screen->GetWidth ();
		f_h = ST_Y;
		f_p = screen->GetPitch ();
		WeightingScale = 0;

		AM_clearFB(Background);
	}
	else
	{
		f_x = viewwindowx;
		f_y = viewwindowy;
		f_w = realviewwidth;
		f_h = realviewheight;
		f_p = screen->GetPitch ();
		WeightingScale = (int)(am_ovtrans * 256.f);
		if (WeightingScale < 0 || WeightingScale >= 256)
		{
			WeightingScale = 0;
		}
	}
	AM_activateNewScale();

	if (grid)
		AM_drawGrid(GridColor);

	// [BC/ZDoomGL] Potentially render the automap in DUKESTYLE!
	if (( OPENGL_GetCurrentRenderer( ) == RENDERER_OPENGL ) && ( gl_automap_dukestyle ))
		GL_DrawDukeAutomap();

	AM_drawWalls(allmap);
	AM_drawPlayers();
	if (am_cheat >= 2 || allthings)
		AM_drawThings(ThingColor);
	AM_drawAuthorMarkers ();

	if (!viewactive)
		AM_drawCrosshair(XHairColor);

	AM_drawMarks();
}

