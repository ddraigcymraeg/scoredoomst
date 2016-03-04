/*
** deathmatch.cpp
** Contains deathmatch routines
**
**---------------------------------------------------------------------------
** Copyright 2000-2010 Brad Carney
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/

#include "announcer.h"
#include "c_cvars.h"
#include "c_dispatch.h"
#include "cl_commands.h"
#include "cl_main.h"
#include "cooperative.h"
#include "deathmatch.h"
#include "g_game.h"
#include "network.h"
#include "p_effect.h"
#include "r_state.h"
#include "s_sound.h"
#include "sbar.h"
#include "sv_commands.h"
#include "team.h"
#include "v_video.h"

//*****************************************************************************
//	MISC CRAP THAT SHOULDN'T BE HERE BUT HAS TO BE BECAUSE OF SLOPPY CODING

void	G_PlayerReborn( int player );
void	SERVERCONSOLE_UpdateScoreboard( );

//*****************************************************************************
//	CONSOLE COMMANDS/VARIABLES

CUSTOM_CVAR( Bool, deathmatch, false, CVAR_SERVERINFO | CVAR_LATCH | CVAR_CAMPAIGNLOCK )
{
	UCVarValue	Val;

	if ( self == true )
	{
		Val.Bool = false;

		// Disable teamgame if we're playing deathmatch.
		teamgame.ForceSet( Val, CVAR_Bool );

		// Disable cooperative if we're playing deathmatch.
		cooperative.ForceSet( Val, CVAR_Bool );
	}
	else
	{
		Val.Bool = false;
		// Deathmatch has been disabled, so disable all related modes.
		teamplay.ForceSet( Val, CVAR_Bool );
		duel.ForceSet( Val, CVAR_Bool );
		terminator.ForceSet( Val, CVAR_Bool );
		lastmanstanding.ForceSet( Val, CVAR_Bool );
		teamlms.ForceSet( Val, CVAR_Bool );
		possession.ForceSet( Val, CVAR_Bool );
		teampossession.ForceSet( Val, CVAR_Bool );

		// If teamgame is also disabled, enable cooperative mode.
		if ( teamgame == false )
		{
			Val.Bool = true;

			if ( cooperative != true )
				cooperative.ForceSet( Val, CVAR_Bool );
		}
	}
}

//*****************************************************************************
//
CUSTOM_CVAR( Bool, teamplay, false, CVAR_SERVERINFO | CVAR_LATCH | CVAR_CAMPAIGNLOCK )
{
	UCVarValue	Val;

	if ( self == true )
	{
		Val.Bool = true;
		// Enable deathmatch.
		deathmatch.ForceSet( Val, CVAR_Bool );

		Val.Bool = false;
		// Disable other modes.
		duel.ForceSet( Val, CVAR_Bool );
		terminator.ForceSet( Val, CVAR_Bool );
		lastmanstanding.ForceSet( Val, CVAR_Bool );
		teamlms.ForceSet( Val, CVAR_Bool );
		possession.ForceSet( Val, CVAR_Bool );
		teampossession.ForceSet( Val, CVAR_Bool );
	}
}

//*****************************************************************************
//
CUSTOM_CVAR( Bool, duel, false, CVAR_SERVERINFO | CVAR_LATCH | CVAR_CAMPAIGNLOCK )
{
	UCVarValue	Val;

	if ( self == true )
	{
		Val.Bool = true;
		// Enable deathmatch.
		deathmatch.ForceSet( Val, CVAR_Bool );

		Val.Bool = false;
		// Disable other modes.
		teamplay.ForceSet( Val, CVAR_Bool );
		terminator.ForceSet( Val, CVAR_Bool );
		lastmanstanding.ForceSet( Val, CVAR_Bool );
		teamlms.ForceSet( Val, CVAR_Bool );
		possession.ForceSet( Val, CVAR_Bool );
		teampossession.ForceSet( Val, CVAR_Bool );
	}
}

//*****************************************************************************
//
CUSTOM_CVAR( Bool, terminator, false, CVAR_SERVERINFO | CVAR_LATCH | CVAR_CAMPAIGNLOCK )
{
	UCVarValue	Val;

	if ( self == true )
	{
		Val.Bool = true;
		// Enable deathmatch.
		deathmatch.ForceSet( Val, CVAR_Bool );

		Val.Bool = false;
		// Disable other modes.
		teamplay.ForceSet( Val, CVAR_Bool );
		duel.ForceSet( Val, CVAR_Bool );
		lastmanstanding.ForceSet( Val, CVAR_Bool );
		teamlms.ForceSet( Val, CVAR_Bool );
		possession.ForceSet( Val, CVAR_Bool );
		teampossession.ForceSet( Val, CVAR_Bool );
	}
}

//*****************************************************************************
//
CUSTOM_CVAR( Bool, lastmanstanding, false, CVAR_SERVERINFO | CVAR_LATCH | CVAR_CAMPAIGNLOCK )
{
	UCVarValue	Val;

	if ( self == true )
	{
		Val.Bool = true;
		// Enable deathmatch.
		deathmatch.ForceSet( Val, CVAR_Bool );

		Val.Bool = false;
		// Disable other modes.
		teamplay.ForceSet( Val, CVAR_Bool );
		duel.ForceSet( Val, CVAR_Bool );
		terminator.ForceSet( Val, CVAR_Bool );
		teamlms.ForceSet( Val, CVAR_Bool );
		possession.ForceSet( Val, CVAR_Bool );
		teampossession.ForceSet( Val, CVAR_Bool );
	}
}

//*****************************************************************************
//
CUSTOM_CVAR( Bool, teamlms, false, CVAR_SERVERINFO | CVAR_LATCH | CVAR_CAMPAIGNLOCK )
{
	UCVarValue	Val;

	if ( self == true )
	{
		Val.Bool = true;
		// Enable deathmatch.
		deathmatch.ForceSet( Val, CVAR_Bool );

		Val.Bool = false;
		// Disable other modes.
		teamplay.ForceSet( Val, CVAR_Bool );
		duel.ForceSet( Val, CVAR_Bool );
		terminator.ForceSet( Val, CVAR_Bool );
		lastmanstanding.ForceSet( Val, CVAR_Bool );
		possession.ForceSet( Val, CVAR_Bool );
		teampossession.ForceSet( Val, CVAR_Bool );
	}
}

//*****************************************************************************
//
CUSTOM_CVAR( Bool, possession, false, CVAR_SERVERINFO | CVAR_LATCH | CVAR_CAMPAIGNLOCK )
{
	UCVarValue	Val;

	if ( self == true )
	{
		Val.Bool = true;
		// Enable deathmatch.
		deathmatch.ForceSet( Val, CVAR_Bool );

		Val.Bool = false;
		// Disable other modes.
		teamplay.ForceSet( Val, CVAR_Bool );
		duel.ForceSet( Val, CVAR_Bool );
		terminator.ForceSet( Val, CVAR_Bool );
		lastmanstanding.ForceSet( Val, CVAR_Bool );
		teamlms.ForceSet( Val, CVAR_Bool );
		teampossession.ForceSet( Val, CVAR_Bool );
	}
}

//*****************************************************************************
//
CUSTOM_CVAR( Bool, teampossession, false, CVAR_SERVERINFO | CVAR_LATCH | CVAR_CAMPAIGNLOCK )
{
	UCVarValue	Val;

	if ( self == true )
	{
		Val.Bool = true;
		// Enable deathmatch.
		deathmatch.ForceSet( Val, CVAR_Bool );

		Val.Bool = false;
		// Disable other modes.
		teamplay.ForceSet( Val, CVAR_Bool );
		duel.ForceSet( Val, CVAR_Bool );
		terminator.ForceSet( Val, CVAR_Bool );
		lastmanstanding.ForceSet( Val, CVAR_Bool );
		teamlms.ForceSet( Val, CVAR_Bool );
		possession.ForceSet( Val, CVAR_Bool );
	}
}

//*****************************************************************************
//
CCMD( spectate )
{
	// Already a spectator!
	if ( PLAYER_IsTrueSpectator( &players[consoleplayer] ))
		return;

	// If we're a client, inform the server that we wish to spectate.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
	{
		CLIENTCOMMANDS_Spectate( );
		return;
	}

	// Make the player a spectator.
	PLAYER_SetSpectator( &players[consoleplayer], true, false );
}

//*****************************************************************************
//*****************************************************************************
//
CUSTOM_CVAR( Int, fraglimit, 0, CVAR_SERVERINFO | CVAR_CAMPAIGNLOCK )
{
	if (( NETWORK_GetState( ) == NETSTATE_SERVER ) && ( gamestate != GS_STARTUP ))
	{
		SERVER_Printf( PRINT_HIGH, "%s changed to: %d\n", self.GetName( ), (LONG)self );
		SERVERCOMMANDS_SetGameModeLimits( );

		// Update the scoreboard.
		SERVERCONSOLE_UpdateScoreboard( );
	}
}

//*****************************************************************************
//
CUSTOM_CVAR( Float, timelimit, 0.0f, CVAR_SERVERINFO | CVAR_CAMPAIGNLOCK )
{
	//ghk, disable timelimit
	if (self<0){
		self=0;
	}else if(self > 0){
		self=0;
	}
	return;
	if (( NETWORK_GetState( ) == NETSTATE_SERVER ) && ( gamestate != GS_STARTUP ))
	{

		SERVER_Printf( PRINT_HIGH, "%s changed to: %d\n", self.GetName( ), (LONG)self );
		SERVERCOMMANDS_SetGameModeLimits( );

		// Update the scoreboard.
		SERVERCONSOLE_UpdateScoreboard( );
	}
}

CVAR( Bool, instagib, false, CVAR_SERVERINFO | CVAR_LATCH | CVAR_CAMPAIGNLOCK );
CVAR( Bool, buckshot, false, CVAR_SERVERINFO | CVAR_LATCH | CVAR_CAMPAIGNLOCK );

// Is this a good place for these variables?
CVAR( Bool, cl_noammoswitch, true, CVAR_ARCHIVE );
CVAR( Bool, cl_useoriginalweaponorder, false, CVAR_ARCHIVE );

// Allow the display of large frag messages.
CVAR( Bool, cl_showlargefragmessages, true, CVAR_ARCHIVE );

// Silly Seismos :>
CCMD( seismos )
{
	Printf( "SEISMOS IS GAY!\n" );
}
