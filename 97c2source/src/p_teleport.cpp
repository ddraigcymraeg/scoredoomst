// Emacs style mode select	 -*- C++ -*- 
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
// $Log:$
//
// DESCRIPTION:
//		Teleportation.
//
//-----------------------------------------------------------------------------


#include "templates.h"
#include "doomtype.h"
#include "doomdef.h"
#include "s_sound.h"
#include "p_local.h"
#include "p_terrain.h"
#include "r_state.h"
#include "gi.h"
#include "a_sharedglobal.h"
#include "m_random.h"
#include "i_system.h"
#include "sv_commands.h"
#include "network.h"

static FRandom pr_teleport ("Teleport");

extern void P_CalcHeight (player_t *player);

CVAR (Bool, telezoom, true, CVAR_ARCHIVE|CVAR_GLOBALCONFIG);

FState ATeleportFog::States[] =
{
#define S_DTFOG 0
	S_BRIGHT (TFOG, 'A',	6, NULL 						, &States[S_DTFOG+1]),
	S_BRIGHT (TFOG, 'B',	6, NULL 						, &States[S_DTFOG+2]),
	S_BRIGHT (TFOG, 'A',	6, NULL 						, &States[S_DTFOG+3]),
	S_BRIGHT (TFOG, 'B',	6, NULL 						, &States[S_DTFOG+4]),
	S_BRIGHT (TFOG, 'C',	6, NULL 						, &States[S_DTFOG+5]),
	S_BRIGHT (TFOG, 'D',	6, NULL 						, &States[S_DTFOG+6]),
	S_BRIGHT (TFOG, 'E',	6, NULL 						, &States[S_DTFOG+7]),
	S_BRIGHT (TFOG, 'F',	6, NULL 						, &States[S_DTFOG+8]),
	S_BRIGHT (TFOG, 'G',	6, NULL 						, &States[S_DTFOG+9]),
	S_BRIGHT (TFOG, 'H',	6, NULL 						, &States[S_DTFOG+10]),
	S_BRIGHT (TFOG, 'I',	6, NULL 						, &States[S_DTFOG+11]),
	S_BRIGHT (TFOG, 'J',	6, NULL 						, NULL),

#define S_HTFOG (S_DTFOG+12)
	S_BRIGHT (TELE, 'A',    6, NULL                         , &States[S_HTFOG+1]),
	S_BRIGHT (TELE, 'B',    6, NULL                         , &States[S_HTFOG+2]),
	S_BRIGHT (TELE, 'C',    6, NULL                         , &States[S_HTFOG+3]),
	S_BRIGHT (TELE, 'D',    6, NULL                         , &States[S_HTFOG+4]),
	S_BRIGHT (TELE, 'E',    6, NULL                         , &States[S_HTFOG+5]),
	S_BRIGHT (TELE, 'F',    6, NULL                         , &States[S_HTFOG+6]),
	S_BRIGHT (TELE, 'G',    6, NULL                         , &States[S_HTFOG+7]),
	S_BRIGHT (TELE, 'H',    6, NULL                         , &States[S_HTFOG+8]),
	S_BRIGHT (TELE, 'G',    6, NULL                         , &States[S_HTFOG+9]),
	S_BRIGHT (TELE, 'F',    6, NULL                         , &States[S_HTFOG+10]),
	S_BRIGHT (TELE, 'E',    6, NULL                         , &States[S_HTFOG+11]),
	S_BRIGHT (TELE, 'D',    6, NULL                         , &States[S_HTFOG+12]),
	S_BRIGHT (TELE, 'C',    6, NULL                         , NULL),

#define S_STFOG (S_HTFOG+13)
	S_BRIGHT (TFOG, 'A',	6, NULL 						, &States[S_STFOG+1]),
	S_BRIGHT (TFOG, 'B',	6, NULL 						, &States[S_STFOG+2]),
	S_BRIGHT (TFOG, 'C',	6, NULL 						, &States[S_STFOG+3]),
	S_BRIGHT (TFOG, 'D',	6, NULL 						, &States[S_STFOG+4]),
	S_BRIGHT (TFOG, 'E',	6, NULL 						, &States[S_STFOG+5]),
	S_BRIGHT (TFOG, 'F',	6, NULL 						, &States[S_STFOG+6]),
	S_BRIGHT (TFOG, 'E',	6, NULL 						, &States[S_STFOG+7]),
	S_BRIGHT (TFOG, 'D',	6, NULL 						, &States[S_STFOG+8]),
	S_BRIGHT (TFOG, 'C',	6, NULL 						, &States[S_STFOG+9]),
	S_BRIGHT (TFOG, 'B',	6, NULL 						, NULL),
};

IMPLEMENT_ACTOR (ATeleportFog, Any, -1, 0)
	PROP_Flags (MF_NOBLOCKMAP|MF_NOGRAVITY)
	PROP_Flags2 (MF2_NOTELEPORT)
	PROP_RenderStyle (STYLE_Add)
END_DEFAULTS

AT_GAME_SET (TeleportFog)
{
	ATeleportFog *def = GetDefault<ATeleportFog>();

	if (gameinfo.gametype == GAME_Doom)
	{
		def->SpawnState = &ATeleportFog::States[S_DTFOG];
	}
	else if (gameinfo.gametype == GAME_Strife)
	{
		def->SpawnState = &ATeleportFog::States[S_STFOG];
	}
	else
	{
		def->SpawnState = &ATeleportFog::States[S_HTFOG];
	}
}

void ATeleportFog::PostBeginPlay ()
{
	Super::PostBeginPlay ();
	S_Sound (this, CHAN_BODY, "misc/teleport", 1, ATTN_NORM);
}

IMPLEMENT_STATELESS_ACTOR (ATeleportDest, Any, 14, 0)
	PROP_Flags (MF_NOBLOCKMAP|MF_NOSECTOR)
	PROP_Flags3(MF3_DONTSPLASH)
END_DEFAULTS

// Teleport dest that can spawn above floor

class ATeleportDest2 : public ATeleportDest
{
	DECLARE_STATELESS_ACTOR (ATeleportDest2, ATeleportDest)
};

IMPLEMENT_STATELESS_ACTOR (ATeleportDest2, Any, 9044, 0)
	PROP_FlagsSet (MF_NOGRAVITY)
END_DEFAULTS

// Z-preserving dest subject to gravity (for TeleportGroup, etc.)

class ATeleportDest3 : public ATeleportDest2
{
	DECLARE_STATELESS_ACTOR (ATeleportDest3, ATeleportDest2)
};

IMPLEMENT_STATELESS_ACTOR (ATeleportDest3, Any, 9043, 0)
	PROP_FlagsClear (MF_NOGRAVITY)
END_DEFAULTS

//
// TELEPORTATION
//

// [BC] Added the bHaltMomentum argument.
bool P_Teleport (AActor *thing, fixed_t x, fixed_t y, fixed_t z, angle_t angle,
				 bool useFog, bool sourceFog, bool keepOrientation, bool bHaltMomentum)
{
	fixed_t oldx;
	fixed_t oldy;
	fixed_t oldz;
	fixed_t aboveFloor;
	player_t *player;
	angle_t an;
	sector_t *destsect;
	bool resetpitch = false;
	fixed_t floorheight, ceilingheight;

	oldx = thing->x;
	oldy = thing->y;
	oldz = thing->z;
	aboveFloor = thing->z - thing->floorz;
	destsect = R_PointInSubsector (x, y)->sector;
	// killough 5/12/98: exclude voodoo dolls:
	player = thing->player;
	if (player && player->mo != thing)
		player = NULL;
	floorheight = destsect->floorplane.ZatPoint (x, y);
	ceilingheight = destsect->ceilingplane.ZatPoint (x, y);
	if (z == ONFLOORZ)
	{
		if (player)
		{
			if (thing->flags & MF_NOGRAVITY && aboveFloor)
			{
				z = floorheight + aboveFloor;
				if (z + thing->height > ceilingheight)
				{
					z = ceilingheight - thing->height;
				}
			}
			else
			{
				z = floorheight;
				if (!keepOrientation)
				{
					resetpitch = false;
				}
			}
		}
		else if (thing->flags & MF_MISSILE)
		{
			z = floorheight + aboveFloor;
			if (z + thing->height > ceilingheight)
			{
				z = ceilingheight - thing->height;
			}
		}
		else
		{
			z = floorheight;
		}
	}
	if (!P_TeleportMove (thing, x, y, z, false))
	{
		return false;
	}
	if (player)
	{
		player->viewz = thing->z + player->viewheight;
		if (resetpitch)
		{
			player->mo->pitch = 0;
		}
	}
	if (!keepOrientation)
	{
		thing->angle = angle;
	}
	else
	{
		angle = thing->angle;
	}

	// [BC] Teleporting spectators do not create fog.
	if ( thing && thing->player && thing->player->bSpectating )
	{
		useFog = false;
		sourceFog = false;
	}

	// Spawn teleport fog at source and destination
	if (sourceFog)
	{
		fixed_t fogDelta = thing->flags & MF_MISSILE ? 0 : TELEFOGHEIGHT;
		Spawn<ATeleportFog> (oldx, oldy, oldz + fogDelta, ALLOW_REPLACE);
	}
	if (useFog)
	{
		fixed_t fogDelta = thing->flags & MF_MISSILE ? 0 : TELEFOGHEIGHT;
		an = angle >> ANGLETOFINESHIFT;
		Spawn<ATeleportFog> (x + 20*finecosine[an],
			y + 20*finesine[an], thing->z + fogDelta, ALLOW_REPLACE);
		if (thing->player)
		{
			// [RH] Zoom player's field of vision
			// [BC] && bHaltMomentum.
			if (telezoom && thing->player->mo == thing && bHaltMomentum)
				thing->player->FOV = MIN (175.f, thing->player->DesiredFOV + 45.f);
		}
	}
	// [BC] && bHaltMomentum.
	if (thing->player && (useFog || !keepOrientation) && bHaltMomentum)
	{
		// Freeze player for about .5 sec
		if (!(thing->player->Powers & PW_SPEED))
			thing->reactiontime = 18;
	}
	if (thing->flags & MF_MISSILE)
	{
		angle >>= ANGLETOFINESHIFT;
		thing->momx = FixedMul (thing->Speed, finecosine[angle]);
		thing->momy = FixedMul (thing->Speed, finesine[angle]);
	}
	// [BC] && bHaltMomentum.
	else if (!keepOrientation && bHaltMomentum) // no fog doesn't alter the player's momentum
	{
		thing->momx = thing->momy = thing->momz = 0;
		// killough 10/98: kill all bobbing momentum too
		if (player)
			player->momx = player->momy = 0;
	}

	// [BC] If we're the server, update clients about this teleport.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_TeleportThing( thing, sourceFog, useFog, ( useFog && bHaltMomentum ));

	return true;
}

static AActor *SelectTeleDest (int tid, int tag)
{
	AActor *searcher;

	// If tid is non-zero, select a destination from a matching actor at random.
	// If tag is also non-zero, the selection is restricted to actors in sectors
	// with a matching tag. If tid is zero and tag is non-zero, then the old Doom
	// behavior is used instead (return the first teleport dest found in a tagged
	// sector).

	if (tid != 0)
	{
		TActorIterator<ATeleportDest> iterator (tid);
		int count = 0;
		while ( (searcher = iterator.Next ()) )
		{
			if (tag == 0 || searcher->Sector->tag == tag)
			{
				count++;
			}
		}

		// If teleport dests were not found, the sector tag is ignored for the
		// following compatibility searches.
		if (count == 0)
		{
			// Try to find a matching map spot (fixes Hexen MAP10)
			TActorIterator<AMapSpot> it2 (tid);
			searcher = it2.Next ();
			if (searcher == NULL)
			{
				// Try to find a matching non-blocking spot of any type (fixes Caldera MAP13)
				FActorIterator it3 (tid);
				searcher = it3.Next ();
				while (searcher != NULL && (searcher->flags & MF_SOLID))
				{
					searcher = it3.Next ();
				}
				return searcher;
			}
		}
		else
		{
			if (count != 1)
			{
				count = 1 + (pr_teleport() % count);
			}
			searcher = NULL;
			while (count > 0)
			{
				searcher = iterator.Next ();
				if (tag == 0 || searcher->Sector->tag == tag)
				{
					count--;
				}
			}
		}
		return searcher;
	}

	if (tag != 0)
	{
		int secnum = -1;

		while ((secnum = P_FindSectorFromTag (tag, secnum)) >= 0)
		{
			// Scanning the snext links of things in the sector will not work, because
			// TeleportDests have MF_NOSECTOR set. So you have to search *everything*.
			// If there is more than one sector with a matching tag, then the destination
			// in the lowest-numbered sector is returned, rather than the earliest placed
			// teleport destination. This means if 50 sectors have a matching tag and
			// only the last one has a destination, *every* actor is scanned at least 49
			// times. Yuck.
			TThinkerIterator<ATeleportDest> it2;
			while ((searcher = it2.Next()) != NULL)
			{
				if (searcher->Sector == sectors + secnum)
				{
					return searcher;
				}
			}
		}
	}

	return NULL;
}

// [BC] Added the bHaltMomentum argument.
bool EV_Teleport (int tid, int tag, line_t *line, int side, AActor *thing, bool fog,
				  bool sourceFog, bool keepOrientation, bool bHaltMomentum)
{
	AActor *searcher;
	fixed_t z;
	angle_t angle = 0;
	fixed_t s = 0, c = 0;
	fixed_t momx = 0, momy = 0;
	angle_t	OldAngle;

	if (thing == NULL)
	{ // Teleport function called with an invalid actor
		return false;
	}
	if (thing->flags2 & MF2_NOTELEPORT)
	{
		return false;
	}
	if (side != 0)
	{ // Don't teleport if hit back of line, so you can get out of teleporter.
		return 0;
	}
	searcher = SelectTeleDest (tid, tag);
	if (searcher == NULL)
	{
		return false;
	}

	// [BC]
	OldAngle = thing->angle;

	// [RH] Lee Killough's changes for silent teleporters from BOOM
	if (keepOrientation && line)
	{
		// Get the angle between the exit thing and source linedef.
		// Rotate 90 degrees, so that walking perpendicularly across
		// teleporter linedef causes thing to exit in the direction
		// indicated by the exit thing.
		angle = R_PointToAngle2 (0, 0, line->dx, line->dy) - searcher->angle + ANG90;

		// Sine, cosine of angle adjustment
		s = finesine[angle>>ANGLETOFINESHIFT];
		c = finecosine[angle>>ANGLETOFINESHIFT];

		// Momentum of thing crossing teleporter linedef
		momx = thing->momx;
		momy = thing->momy;

		z = searcher->z;
	}
	else if (searcher->IsKindOf (RUNTIME_CLASS(ATeleportDest2)))
	{
		z = searcher->z;
	}
	else
	{
		z = ONFLOORZ;
	}
	if (P_Teleport (thing, searcher->x, searcher->y, z, searcher->angle, fog, sourceFog, keepOrientation, bHaltMomentum))
	{
		// [RH] Lee Killough's changes for silent teleporters from BOOM
		if (!fog && line && keepOrientation)
		{
			// Rotate thing according to difference in angles
			thing->angle += angle;

			// Rotate thing's momentum to come out of exit just like it entered
			thing->momx = FixedMul(momx, c) - FixedMul(momy, s);
			thing->momy = FixedMul(momy, c) + FixedMul(momx, s);
		}

		// [BC] Adjust the thing's momentum if we didn't halt it.
		if ( bHaltMomentum == false )
		{
			// Get the angle between the exit thing and source linedef.
			// Rotate 90 degrees, so that walking perpendicularly across
			// teleporter linedef causes thing to exit in the direction
			// indicated by the exit thing.
//			angle = R_PointToAngle2 (0, 0, line->dx, line->dy) - searcher->angle + ANG90;
			angle = thing->angle - OldAngle;

			// Sine, cosine of angle adjustment
			s = finesine[angle>>ANGLETOFINESHIFT];
			c = finecosine[angle>>ANGLETOFINESHIFT];

			// Momentum of thing crossing teleporter linedef
			momx = thing->momx;
			momy = thing->momy;

			// Rotate thing's momentum to come out of exit just like it entered
			thing->momx = FixedMul(momx, c) - FixedMul(momy, s);
			thing->momy = FixedMul(momy, c) + FixedMul(momx, s);
		}
		return true;
	}
	return false;
}

//
// Silent linedef-based TELEPORTATION, by Lee Killough
// Primarily for rooms-over-rooms etc.
// This is the complete player-preserving kind of teleporter.
// It has advantages over the teleporter with thing exits.
//

// [RH] Modified to support different source and destination ids.
// [RH] Modified some more to be accurate.
bool EV_SilentLineTeleport (line_t *line, int side, AActor *thing, int id, INTBOOL reverse)
{
	int i;
	line_t *l;

	if (side || thing->flags2 & MF2_NOTELEPORT || !line || line->sidenum[1] == NO_SIDE)
		return false;

	for (i = -1; (i = P_FindLineFromID (id, i)) >= 0; )
	{
		if (line-lines == i)
			continue;

		if ((l=lines+i) != line && l->backsector)
		{
			// Get the thing's position along the source linedef
			SDWORD pos;				// 30.2 fixed
			fixed_t nposx, nposy;	// offsets from line
			{
				SQWORD den;

				den = (SQWORD)line->dx*line->dx + (SQWORD)line->dy*line->dy;
				if (den == 0)
				{
					pos = 0;
					nposx = 0;
					nposy = 0;
				}
				else
				{
					SQWORD num = (SQWORD)(thing->x-line->v1->x)*line->dx + 
								 (SQWORD)(thing->y-line->v1->y)*line->dy;
					if (num <= 0)
					{
						pos = 0;
					}
					else if (num >= den)
					{
						pos = 1<<30;
					}
					else
					{
						pos = (SDWORD)(num / (den>>30));
					}
					nposx = thing->x - line->v1->x - MulScale30 (line->dx, pos);
					nposy = thing->y - line->v1->y - MulScale30 (line->dy, pos);
				}
			}

			// Get the angle between the two linedefs, for rotating
			// orientation and momentum. Rotate 180 degrees, and flip
			// the position across the exit linedef, if reversed.
			angle_t angle =
				R_PointToAngle2(0, 0, l->dx, l->dy) -
				R_PointToAngle2(0, 0, line->dx, line->dy);

			if (!reverse)
			{
				angle += ANGLE_180;
				pos = (1<<30) - pos;
			}

			// Sine, cosine of angle adjustment
			fixed_t s = finesine[angle>>ANGLETOFINESHIFT];
			fixed_t c = finecosine[angle>>ANGLETOFINESHIFT];

			fixed_t x, y;

			// Rotate position along normal to match exit linedef
			x = DMulScale16 (nposx, c, -nposy, s);
			y = DMulScale16 (nposy, c,  nposx, s);

			// Interpolate position across the exit linedef
			x += l->v1->x + MulScale30 (pos, l->dx);
			y += l->v1->y + MulScale30 (pos, l->dy);

			// Whether this is a player, and if so, a pointer to its player_t.
			// Voodoo dolls are excluded by making sure thing->player->mo==thing.
			player_t *player = thing->player && thing->player->mo == thing ?
				thing->player : NULL;

			// Height of thing above ground
			fixed_t z;
			
			z = thing->z - sides[line->sidenum[1]].sector->floorplane.ZatPoint (thing->x, thing->y)
				+ sides[l->sidenum[0]].sector->floorplane.ZatPoint (x, y);

			// Attempt to teleport, aborting if blocked
			// Adjust z position to be same height above ground as before.
			// Ground level at the exit is measured as the higher of the
			// two floor heights at the exit linedef.
			if (!P_TeleportMove (thing, x, y, z, false))
			{
				return false;
			}

			if (thing == players[consoleplayer].camera)
			{
				R_ResetViewInterpolation ();
			}

			// Rotate thing's orientation according to difference in linedef angles
			thing->angle += angle;

			// Momentum of thing crossing teleporter linedef
			x = thing->momx;
			y = thing->momy;

			// Rotate thing's momentum to come out of exit just like it entered
			thing->momx = DMulScale16 (x, c, -y, s);
			thing->momy = DMulScale16 (y, c,  x, s);

			// Adjust a player's view, in case there has been a height change
			if (player && player->mo == thing)
			{
				// Adjust player's local copy of momentum
				x = player->momx;
				y = player->momy;
				player->momx = DMulScale16 (x, c, -y, s);
				player->momy = DMulScale16 (y, c,  x, s);

				// Save the current deltaviewheight, used in stepping
				fixed_t deltaviewheight = player->deltaviewheight;

				// Clear deltaviewheight, since we don't want any changes now
				player->deltaviewheight = 0;

				// Set player's view according to the newly set parameters
				P_CalcHeight(player);

				// Reset the delta to have the same dynamics as before
				player->deltaviewheight = deltaviewheight;
			}

			// [BC] If we're the server, send the message that this thing has been tele-
			// ported.
			if ( NETWORK_GetState( ) == NETSTATE_SERVER )
				SERVERCOMMANDS_TeleportThing( thing, false, false, false );

			return true;
		}
	}
	return false;
}

// [RH] Teleport anything matching other_tid to dest_tid
bool EV_TeleportOther (int other_tid, int dest_tid, bool fog)
{
	bool didSomething = false;

	if (other_tid != 0 && dest_tid != 0)
	{
		AActor *victim;
		FActorIterator iterator (other_tid);

		while ( (victim = iterator.Next ()) )
		{
			didSomething |= EV_Teleport (dest_tid, 0, NULL, 0, victim, fog, fog, !fog);
		}
	}

	return didSomething;
}

static bool DoGroupForOne (AActor *victim, AActor *source, AActor *dest, bool floorz, bool fog)
{
	int an = (dest->angle - source->angle) >> ANGLETOFINESHIFT;
	fixed_t offX = victim->x - source->x;
	fixed_t offY = victim->y - source->y;
	angle_t offAngle = victim->angle - source->angle;
	fixed_t newX = DMulScale16 (offX, finecosine[an], -offY, finesine[an]);
	fixed_t newY = DMulScale16 (offX, finesine[an], offY, finecosine[an]);

	bool res =
		P_Teleport (victim, dest->x + newX,
							dest->y + newY,
							floorz ? ONFLOORZ : dest->z + victim->z - source->z,
							0, fog, fog, !fog);
	// P_Teleport only changes angle if fog is true
	victim->angle = dest->angle + offAngle;

	return res;
}

#if 0
static void MoveTheDecal (DBaseDecal *decal, fixed_t z, AActor *source, AActor *dest)
{
	int an = (dest->angle - source->angle) >> ANGLETOFINESHIFT;
	fixed_t offX = decal->x - source->x;
	fixed_t offY = decal->y - source->y;
	fixed_t newX = DMulScale16 (offX, finecosine[an], -offY, finesine[an]);
	fixed_t newY = DMulScale16 (offX, finesine[an], offY, finecosine[an]);

	decal->Relocate (dest->x + newX, dest->y + newY, dest->z + z - source->z);
}
#endif

// [RH] Teleport a group of actors centered around source_tid so
// that they become centered around dest_tid instead.
bool EV_TeleportGroup (int group_tid, AActor *victim, int source_tid, int dest_tid, bool moveSource, bool fog)
{
	AActor *sourceOrigin, *destOrigin;
	{
		FActorIterator iterator (source_tid);
		sourceOrigin = iterator.Next ();
	}
	if (sourceOrigin == NULL)
	{ // If there is no source origin, behave like TeleportOther
		return EV_TeleportOther (group_tid, dest_tid, fog);
	}

	{
		TActorIterator<ATeleportDest> iterator (dest_tid);
		destOrigin = iterator.Next ();
	}
	if (destOrigin == NULL)
	{
		return false;
	}

	bool didSomething = false;
	bool floorz = !destOrigin->IsKindOf (RUNTIME_CLASS(ATeleportDest2));

	// Use the passed victim if group_tid is 0
	if (group_tid == 0 && victim != NULL)
	{
		didSomething = DoGroupForOne (victim, sourceOrigin, destOrigin, floorz, fog);
	}
	else
	{
		FActorIterator iterator (group_tid);

		// For each actor with tid matching arg0, move it to the same
		// position relative to destOrigin as it is relative to sourceOrigin
		// before the teleport.
		while ( (victim = iterator.Next ()) )
		{
			didSomething |= DoGroupForOne (victim, sourceOrigin, destOrigin, floorz, fog);
		}
	}

	if (moveSource && didSomething)
	{
		didSomething |=
			P_Teleport (sourceOrigin, destOrigin->x, destOrigin->y,
				floorz ? ONFLOORZ : destOrigin->z, 0, false, false, true);
		sourceOrigin->angle = destOrigin->angle;
	}

	return didSomething;
}

// [RH] Teleport a group of actors in a sector. Source_tid is used as a
// reference point so that they end up in the same position relative to
// dest_tid. Group_tid can be used to not teleport all actors in the sector.
bool EV_TeleportSector (int tag, int source_tid, int dest_tid, bool fog, int group_tid)
{
	AActor *sourceOrigin, *destOrigin;
	{
		FActorIterator iterator (source_tid);
		sourceOrigin = iterator.Next ();
	}
	if (sourceOrigin == NULL)
	{
		return false;
	}
	{
		TActorIterator<ATeleportDest> iterator (dest_tid);
		destOrigin = iterator.Next ();
	}
	if (destOrigin == NULL)
	{
		return false;
	}

	bool didSomething = false;
	bool floorz = !destOrigin->IsKindOf (RUNTIME_CLASS(ATeleportDest2));
	int secnum;

	secnum = -1;
	while ((secnum = P_FindSectorFromTag (tag, secnum)) >= 0)
	{
		msecnode_t *node;
		const sector_t * const sec = &sectors[secnum];

		for (node = sec->touching_thinglist; node; )
		{
			AActor *actor = node->m_thing;
			msecnode_t *next = node->m_snext;

			// possibly limit actors by group
			if (actor != NULL && (group_tid == 0 || actor->tid == group_tid))
			{
				didSomething |= DoGroupForOne (actor, sourceOrigin, destOrigin, floorz, fog);
			}
			node = next;
		}

#if 0
		if (group_tid == 0 && !fog)
		{
			int lineindex;
			for (lineindex = sec->linecount-1; lineindex >= 0; --lineindex)
			{
				line_t *line = sec->lines[lineindex];
				int wallnum;

				wallnum = line->sidenum[(line->backsector == sec)];
				if (wallnum != -1)
				{
					side_t *wall = &sides[wallnum];
					ADecal *decal = wall->BoundActors;

					while (decal != NULL)
					{
						ADecal *next = (ADecal *)decal->snext;
						MoveTheDecal (decal, decal->GetRealZ (wall), sourceOrigin, destOrigin);	
						decal = next;
					}
				}
			}
		}
#endif
	}
	return didSomething;
}
