/*
** duel.cpp
** Contains duel routines
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
#include "cl_main.h"
#include "deathmatch.h"
#include "duel.h"
#include "g_game.h"
#include "network.h"
#include "p_effect.h"
#include "sbar.h"
#include "scoreboard.h"
#include "sv_commands.h"
#include "team.h"
#include "v_video.h"

//*****************************************************************************
//	MISC CRAP THAT SHOULDN'T BE HERE BUT HAS TO BE BECAUSE OF SLOPPY CODING

void	G_PlayerReborn( int player );
void	SERVERCONSOLE_UpdateScoreboard( );

EXTERN_CVAR( Int,  cl_respawninvuleffect )

extern	bool	g_bFirstFragAwarded;

//*****************************************************************************
//	VARIABLES

static	ULONG		g_ulDuelCountdownTicks = 0;
static	ULONG		g_ulDuelLoser = 0;
static	ULONG		g_ulNumDuels = 0;
static	bool		g_bStartNextDuelOnLevelLoad = false;
static	DUELSTATE_e	g_DuelState;

//*****************************************************************************
//	FUNCTIONS

void DUEL_Construct( void )
{
	g_DuelState = DS_WAITINGFORPLAYERS;
}

//*****************************************************************************
//
void DUEL_Tick( void )
{
	// Not in duel mode.
	if ( duel == false )
		return;

	switch ( g_DuelState )
	{
	case DS_WAITINGFORPLAYERS:

		if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
			break;

		// Two players are here now, begin the countdown!
		if ( DUEL_CountActiveDuelers( ) == 2 )
		{
			if ( sv_duelcountdowntime > 0 )
				DUEL_StartCountdown(( sv_duelcountdowntime * TICRATE ) - 1 );
			else
				DUEL_StartCountdown(( 10 * TICRATE ) - 1 );
		}
		break;
	case DS_COUNTDOWN:

		if ( g_ulDuelCountdownTicks )
		{
			g_ulDuelCountdownTicks--;

			// FIGHT!
			if (( g_ulDuelCountdownTicks == 0 ) && ( NETWORK_GetState( ) != NETSTATE_CLIENT ))
				DUEL_DoFight( );
			// Play "3... 2... 1..." sounds.
			else if ( g_ulDuelCountdownTicks == ( 3 * TICRATE ))
				ANNOUNCER_PlayEntry( cl_announcer, "Three" );
			else if ( g_ulDuelCountdownTicks == ( 2 * TICRATE ))
				ANNOUNCER_PlayEntry( cl_announcer, "Two" );
			else if ( g_ulDuelCountdownTicks == ( 1 * TICRATE ))
				ANNOUNCER_PlayEntry( cl_announcer, "One" );
		}
		break;
	}
}

//*****************************************************************************
//
ULONG DUEL_CountActiveDuelers( void )
{
	ULONG	ulIdx;
	ULONG	ulDuelers;

	ulDuelers = 0;
	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if (( playeringame[ulIdx] ) && ( players[ulIdx].bSpectating == false ))
			ulDuelers++;
	}

	return ( ulDuelers );
}

//*****************************************************************************
//
void DUEL_StartCountdown( ULONG ulTicks )
{
	ULONG	ulIdx;

	if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
	{
		// First, reset everyone's fragcount.
		PLAYER_ResetAllPlayersFragcount( );

		// If we're the server, tell clients to reset everyone's fragcount.
		if ( NETWORK_GetState( ) == NETSTATE_SERVER )
			SERVERCOMMANDS_ResetAllPlayersFragcount( );

		// Also, tell bots that a duel countdown is starting.
		for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
		{
			if ( playeringame[ulIdx] )
			{
				if ( players[ulIdx].pSkullBot )
					players[ulIdx].pSkullBot->PostEvent( BOTEVENT_DUEL_STARTINGCOUNTDOWN );
			}
		}

		// Put the duel in a countdown state.
		DUEL_SetState( DS_COUNTDOWN );
	}

	// Set the duel countdown ticks.
	DUEL_SetCountdownTicks( ulTicks );

	// Announce that the fight will soon start.
	ANNOUNCER_PlayEntry( cl_announcer, "PrepareToFight" );

	// Reset announcer "frags left" variables.
	ANNOUNCER_AllowNumFragsAndPointsLeftSounds( );

	// Reset the first frag awarded flag.
	g_bFirstFragAwarded = false;

	// Tell clients to start the countdown.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_DoGameModeCountdown( ulTicks );
}

//*****************************************************************************
//
void DUEL_DoFight( void )
{
	ULONG				ulIdx;
	DHUDMessageFadeOut	*pMsg;

	// No longer waiting to duel.
	if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
		DUEL_SetState( DS_INDUEL );

	// Make sure this is 0. Can be non-zero in network games if they're slightly out of sync.
	g_ulDuelCountdownTicks = 0;

	// Reset level time to 0.
	level.time = 0;

	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		// Since the level time is being reset, also reset the last frag/excellent time for
		// each player.
		players[ulIdx].ulLastExcellentTick = 0;
		players[ulIdx].ulLastFragTick = 0;
		players[ulIdx].ulLastBFGFragTick = 0;

		players[ulIdx].ulDeathsWithoutFrag = 0;
		players[ulIdx].ulFragsWithoutDeath = 0;
		players[ulIdx].ulRailgunShots = 0;
	}

	// Tell clients to "fight!".
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_DoGameModeFight( 0 );

	if ( NETWORK_GetState( ) != NETSTATE_SERVER )
	{
		// Play fight sound.
		ANNOUNCER_PlayEntry( cl_announcer, "Fight" );

		screen->SetFont( BigFont );

		// Display "FIGHT!" HUD message.
		pMsg = new DHUDMessageFadeOut( "FIGHT!",
			160.4f,
			75.0f,
			320,
			200,
			CR_RED,
			2.0f,
			1.0f );

		StatusBar->AttachMessage( pMsg, 'CNTR' );
		screen->SetFont( SmallFont );
	}
	// Display a little thing in the server window so servers can know when matches begin.
	else
		Printf( "FIGHT!\n" );

	// Reset the map.
	if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
		GAME_ResetMap( );

	if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
	{
		// Respawn the players.
		for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
		{
			if (( playeringame[ulIdx] ) && ( PLAYER_IsTrueSpectator( &players[ulIdx] ) == false ))
			{
				if ( players[ulIdx].mo )
				{
					if ( NETWORK_GetState( ) == NETSTATE_SERVER )
						SERVERCOMMANDS_DestroyThing( players[ulIdx].mo );

					players[ulIdx].mo->Destroy( );
					players[ulIdx].mo = NULL;
				}

				// Set the player's state to PST_REBORNNOINVENTORY so they everything is cleared (weapons, etc.)
				players[ulIdx].playerstate = PST_REBORNNOINVENTORY;
				G_DeathMatchSpawnPlayer( ulIdx, true );

				if ( players[ulIdx].pSkullBot )
					players[ulIdx].pSkullBot->PostEvent( BOTEVENT_DUEL_FIGHT );
			}
		}
	}

	SCOREBOARD_RefreshHUD( );
}

//*****************************************************************************
//
void DUEL_DoWinSequence( ULONG ulPlayer )
{
	ULONG	ulIdx;

	// Put the duel state in the win sequence state.
	if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
		DUEL_SetState( DS_WINSEQUENCE );

	// Tell clients to do the win sequence.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_DoGameModeWinSequence( ulPlayer );

	if ( NETWORK_GetState( ) != NETSTATE_SERVER )
	{
		char				szString[64];
		DHUDMessageFadeOut	*pMsg;

		screen->SetFont( BigFont );

		sprintf( szString, "%s \\c-WINS!", players[ulPlayer].userinfo.netname );
		V_ColorizeString( szString );

		// Display "%s WINS!" HUD message.
		pMsg = new DHUDMessageFadeOut( szString,
			160.4f,
			75.0f,
			320,
			200,
			CR_RED,
			3.0f,
			2.0f );

		StatusBar->AttachMessage( pMsg, 'CNTR' );
		screen->SetFont( SmallFont );
	}

	// Award a victory or perfect medal to the winner.
	if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
	{
		LONG	lMedal;

		// If the duel loser doesn't have any frags, give the winner a "Perfect!".
		if ( players[g_ulDuelLoser].fragcount <= 0 )
			lMedal = MEDAL_PERFECT;
		else
			lMedal = MEDAL_VICTORY;

		// Give the player the medal.
		MEDAL_GiveMedal( ulPlayer, lMedal );
		if ( NETWORK_GetState( ) == NETSTATE_SERVER )
			SERVERCOMMANDS_GivePlayerMedal( ulPlayer, lMedal );
	}

	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if (( playeringame[ulIdx] ) && ( players[ulIdx].pSkullBot ))
			players[ulIdx].pSkullBot->PostEvent( BOTEVENT_DUEL_WINSEQUENCE );
	}
}

//*****************************************************************************
//
void DUEL_SendLoserToSpectators( void )
{
	// Losing dueler must have left.
	if ( playeringame[g_ulDuelLoser] == false )
		return;

	// Make this player a spectator.
	PLAYER_SetSpectator( &players[g_ulDuelLoser], true, false );

	// Tell the other players to mark this player as a spectator.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_PlayerIsSpectator( g_ulDuelLoser );
}

//*****************************************************************************
//
bool DUEL_IsDueler( ULONG ulPlayer )
{
	if ( ulPlayer >= MAXPLAYERS )
		return ( false );

	return (( playeringame[ulPlayer] ) && ( players[ulPlayer].bSpectating == false ));
}

//*****************************************************************************
//*****************************************************************************
//
ULONG DUEL_GetCountdownTicks( void )
{
	return ( g_ulDuelCountdownTicks );
}

//*****************************************************************************
//
void DUEL_SetCountdownTicks( ULONG ulTicks )
{
	g_ulDuelCountdownTicks = ulTicks;
}

//*****************************************************************************
//
void DUEL_SetLoser( ULONG ulPlayer )
{
	g_ulDuelLoser = ulPlayer;
}

//*****************************************************************************
//
ULONG DUEL_GetLoser( void )
{
	return ( g_ulDuelLoser );
}

//*****************************************************************************
//
DUELSTATE_e DUEL_GetState( void )
{
	return ( g_DuelState );
}

//*****************************************************************************
//
void DUEL_SetState( DUELSTATE_e State )
{
	if ( g_DuelState == State )
		return;

	g_DuelState = State;

	// Tell clients about the state change.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_SetGameModeState( State );

	switch ( State )
	{
	case DS_WINSEQUENCE:

		// If we've gotten to a win sequence, we've completed a duel.
		if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
			DUEL_SetNumDuels( g_ulNumDuels + 1 );
		break;
	case DS_WAITINGFORPLAYERS:

		// Zero out the countdown ticker.
		DUEL_SetCountdownTicks( 0 );
		break;
	}
}

//*****************************************************************************
//
ULONG DUEL_GetNumDuels( void )
{
	return ( g_ulNumDuels );
}

//*****************************************************************************
//
void DUEL_SetNumDuels( ULONG ulNumDuels )
{
	g_ulNumDuels = ulNumDuels;

	// If we're the server, tell the clients that the number of duels has changed.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_SetDuelNumDuels( ulNumDuels );
}

//*****************************************************************************
//
bool DUEL_GetStartNextDuelOnLevelLoad( void )
{
	return ( g_bStartNextDuelOnLevelLoad );
}

//*****************************************************************************
//
void DUEL_SetStartNextDuelOnLevelLoad( bool bStart )
{
	g_bStartNextDuelOnLevelLoad = bStart;
}

//*****************************************************************************
//	CONSOLE COMMANDS/VARIABLES

CVAR( Int, sv_duelcountdowntime, 10, CVAR_ARCHIVE );
CUSTOM_CVAR( Int, duellimit, 0, CVAR_CAMPAIGNLOCK )
{
	if (( NETWORK_GetState( ) == NETSTATE_SERVER ) && ( gamestate != GS_STARTUP ))
	{
		SERVER_Printf( PRINT_HIGH, "%s changed to: %d\n", self.GetName( ), (LONG)self );
		SERVERCOMMANDS_SetGameModeLimits( );

		// Update the scoreboard.
		SERVERCONSOLE_UpdateScoreboard( );
	}
}
