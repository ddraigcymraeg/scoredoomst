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
//		Ticker.
//
//-----------------------------------------------------------------------------


#include "p_local.h"
#include "p_effect.h"
#include "p_acs.h"
#include "c_console.h"
#include "s_sound.h"
#include "doomstat.h"
#include "sbar.h"
#include "g_game.h"
#include "team.h"
#include "network.h"
#include "sv_commands.h"
#include "cl_main.h"
#include "astar.h"
#include "botpath.h"

extern gamestate_t wipegamestate;

//==========================================================================
//
// P_CheckTickerPaused
//
// Returns true if the ticker should be paused. In that cause, it also
// pauses sound effects and possibly music. If the ticker should not be
// paused, then it returns false but does not unpause anything.
//
//==========================================================================

bool P_CheckTickerPaused ()
{
	// pause if in menu or console and at least one tic has been run
	if (( NETWORK_GetState( ) != NETSTATE_CLIENT )
		 && gamestate != GS_TITLELEVEL
		 && ((menuactive != MENU_Off && menuactive != MENU_OnNoPause) ||
			 ConsoleState == c_down || ConsoleState == c_falling)
		 && !demoplayback
		 && !demorecording
		 && players[consoleplayer].viewz != 1
		 && wipegamestate == gamestate)
	{
		S_PauseSound (!(level.flags & LEVEL_PAUSE_MUSIC_IN_MENUS));
		return true;
	}
	return false;
}

//
// P_Ticker
//
void SERVERCONSOLE_UpdatePlayerInfo( LONG lPlayer, ULONG ulUpdateFlags );
void P_Ticker (void)
{
	ULONG	ulIdx;

	// [BC] Don't run this if the server is lagging.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
	{
		if (( CLIENT_GetServerLagging( ) == true ) ||
			( players[consoleplayer].mo == NULL ))
		{
			return;
		}
	}

	// [BC] Server doesn't need any of this.
	if ( NETWORK_GetState( ) != NETSTATE_SERVER )
	{
		updateinterpolations ();
		r_NoInterpolate = true;

		// run the tic
		if (paused || P_CheckTickerPaused())
			return;

		// [BC] Do a quick check to see if anyone has the freeze time power. If they do,
		// then don't resume the sound, since one of the effects of that power is to shut
		// off the music.
		for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
		{
			if ( playeringame[ulIdx] == false )
				continue;

			if ( players[ulIdx].Powers & PW_TIMEFREEZE )
				break;
		}

		if ( ulIdx == MAXPLAYERS )
			S_ResumeSound ();
		P_ResetSightCounters (false);

		// Since things will be moving, it's okay to interpolate them in the renderer.
		r_NoInterpolate = false;

		P_ResetSpawnCounters( );

		// Since things will be moving, it's okay to interpolate them in the renderer.
		r_NoInterpolate = false;

		// Don't run particles while in freeze mode.
		if ( GAME_GetFreezeMode( ) == false )
		{
			P_ThinkParticles ();	// [RH] make the particles think
		}
		StatusBar->Tick ();		// [RH] moved this here
	}

	// Predict the console player's position.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
	{
		if (( CLIENT_GetServerLagging( ) == false ) && ( CLIENT_GetClientLagging( ) == false ))
			CLIENT_PREDICT_PlayerPredict( );
	}

	if (( botdebug_showcosts ) && ( players[consoleplayer].camera ))
	{
		POS_t	Position;

		Position.x = players[consoleplayer].camera->x;
		Position.y = players[consoleplayer].camera->y;
		ASTAR_ShowCosts( Position );
	}

	if (( NETWORK_GetState( ) != NETSTATE_SERVER ) && ( players[consoleplayer].camera ))
	{
		if ( botdebug_walktest > 0 )
		{
			char				szString[256];
			ULONG				ulTextColor;
			fixed_t				DestX;
			fixed_t				DestY;
			ULONG				ulFlags;
			DHUDMessageFadeOut	*pMsg;

			DestX = players[consoleplayer].camera->x + ( botdebug_walktest * finecosine[players[consoleplayer].camera->angle >> ANGLETOFINESHIFT] );
			DestY = players[consoleplayer].camera->y + ( botdebug_walktest * finesine[players[consoleplayer].camera->angle >> ANGLETOFINESHIFT] );

			szString[0] = 0;
			ulFlags = BOTPATH_TryWalk( players[consoleplayer].camera, players[consoleplayer].camera->x, players[consoleplayer].camera->y, players[consoleplayer].camera->z, DestX, DestY );
			if ( ulFlags > 0 )
			{
				bool	bNeedMark;

				bNeedMark = false;
				if ( ulFlags & BOTPATH_OBSTRUCTED )
				{
					if ( bNeedMark )
						sprintf( szString, "%s ", szString );
					
					sprintf( szString, "%sOBSTRUCTED", szString );
					bNeedMark = true;
				}
				if ( ulFlags & BOTPATH_STAIRS )
				{
					if ( bNeedMark )
						sprintf( szString, "%s ", szString );
					
					sprintf( szString, "%sSTAIRS", szString );
					bNeedMark = true;
				}
				if ( ulFlags & BOTPATH_JUMPABLELEDGE )
				{
					if ( bNeedMark )
						sprintf( szString, "%s ", szString );
					
					sprintf( szString, "%sJUMPABLELEDGE", szString );
					bNeedMark = true;
				}
				if ( ulFlags & BOTPATH_DAMAGINGSECTOR )
				{
					if ( bNeedMark )
						sprintf( szString, "%s ", szString );
					
					sprintf( szString, "%sDAMAGINGSECTOR", szString );
					bNeedMark = true;
				}
				if ( ulFlags & BOTPATH_DROPOFF )
				{
					if ( bNeedMark )
						sprintf( szString, "%s ", szString );
					
					sprintf( szString, "%sDROPOFF", szString );
					bNeedMark = true;
				}
				if ( ulFlags & BOTPATH_TELEPORT )
				{
					if ( bNeedMark )
						sprintf( szString, "%s ", szString );
					
					sprintf( szString, "%sTELEPORT", szString );
					bNeedMark = true;
				}
				if ( ulFlags & BOTPATH_DOOR )
				{
					if ( bNeedMark )
						sprintf( szString, "%s ", szString );
					
					sprintf( szString, "%sDOOR", szString );
					bNeedMark = true;
				}

				ulTextColor = CR_RED;
			}
			else
			{
				ulTextColor = CR_GREEN;
				sprintf( szString, "ALL CLEAR!" );
			}

			pMsg = new DHUDMessageFadeOut( szString,
				1.5f,
				0.9f,
				0,
				0,
				(EColorRange)ulTextColor,
				0.f,
				0.35f );

			StatusBar->AttachMessage( pMsg, 'OBST' );
		}

		if ( botdebug_obstructiontest > 0 )
		{
			char				szString[64];
			ULONG				ulTextColor;
			DHUDMessageFadeOut	*pMsg;

			if ( BOTS_IsPathObstructed( botdebug_obstructiontest, players[consoleplayer].camera ))
			{
				ulTextColor = CR_RED;
				sprintf( szString, "PATH OBSTRUCTED!" );
			}
			else
			{
				ulTextColor = CR_GREEN;
				sprintf( szString, "ALL CLEAR!" );
			}

			pMsg = new DHUDMessageFadeOut( szString,
				1.5f,
				0.9f,
				0,
				0,
				(EColorRange)ulTextColor,
				0.f,
				0.35f );

			StatusBar->AttachMessage( pMsg, 'OBST' );
		}
	}

	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		// Increment individual player time.
		if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
		{
			if ( playeringame[ulIdx] )
			{
				players[ulIdx].ulTime++;

				// Potentially update the scoreboard or send out an update.
				if ( NETWORK_GetState( ) == NETSTATE_SERVER )
				{
					if (( players[ulIdx].ulTime % ( TICRATE * 60 )) == 0 )
					{
						// Send out the updated time field to all clients.
						SERVERCOMMANDS_UpdatePlayerTime( ulIdx );

						// Update the console as well.
						SERVERCONSOLE_UpdatePlayerInfo( ulIdx, UDF_TIME );
					}
				}
			}
		}

		// Client's "think" every time we get a cmd.
#ifndef	MULTITICK_HACK_FIX
		if (( NETWORK_GetState( ) == NETSTATE_SERVER ) && ( players[ulIdx].bIsBot == false ))
			continue;
#endif

		// Console player thinking is handled by player prediction.
		if (( ulIdx == consoleplayer ) &&  ( NETWORK_GetState( ) == NETSTATE_CLIENT ))
			continue;

		if ( playeringame[ulIdx] )
			P_PlayerThink( &players[ulIdx] );
	}

	level.Tick ();			// [RH] let the level tick
	DThinker::RunThinkers ();

	// Don't do this stuff while in freeze mode.
	if ( GAME_GetFreezeMode( ) == false )
	{
		for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
		{
			if (( playeringame[ulIdx] ) && ( players[ulIdx].pSkullBot ))
			{
				// Also, if they have an enemy, and can see it, update their known enemy position.
				if (( players[ulIdx].pSkullBot->m_ulPlayerEnemy != MAXPLAYERS ) && ( P_CheckSight( players[ulIdx].mo, players[players[ulIdx].pSkullBot->m_ulPlayerEnemy].mo, 2 )))
					players[ulIdx].pSkullBot->SetEnemyPosition( players[players[ulIdx].pSkullBot->m_ulPlayerEnemy].mo->x, players[players[ulIdx].pSkullBot->m_ulPlayerEnemy].mo->y, players[players[ulIdx].pSkullBot->m_ulPlayerEnemy].mo->z );

				// Now that all the players have moved to their final location for this tick,
				// we can properly aim at them.
				players[ulIdx].pSkullBot->HandleAiming( );
			}
		}

		P_UpdateSpecials ();

		if ( NETWORK_GetState( ) != NETSTATE_SERVER )
			P_RunEffects ();	// [RH] Run particle effects
	}

	// for par times
	level.time++;
	level.maptime++;
	level.totaltime++;

	// Tick the team module. The handles returning dropped flags/skulls.
	if ( teamgame )
	{
		if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
			TEAM_Tick( );
	}
}
