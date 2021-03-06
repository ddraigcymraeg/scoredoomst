/*
** lastmanstanding.cpp
** Contains LMS routines
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

#include "a_action.h"
#include "announcer.h"
#include "c_cvars.h"
#include "cl_main.h"
#include "deathmatch.h"
#include "g_game.h"
#include "joinqueue.h"
#include "lastmanstanding.h"
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
void	SERVERCONSOLE_UpdatePlayerInfo( LONG lPlayer, ULONG ulUpdateFlags );

//*****************************************************************************
//	VARIABLES

static	ULONG		g_ulLMSCountdownTicks = 0;
static	ULONG		g_ulLMSMatches = 0;
static	bool		g_bStartNextMatchOnLevelLoad = false;
static	LMSSTATE_e	g_LMSState;

//*****************************************************************************
//	FUNCTIONS

void LASTMANSTANDING_Construct( void )
{
	g_LMSState = LMSS_WAITINGFORPLAYERS;
}

//*****************************************************************************
//
void LASTMANSTANDING_Tick( void )
{
	// Not in LMS mode.
	if (( lastmanstanding == false ) && ( teamlms == false ))
		return;

	switch ( g_LMSState )
	{
	case LMSS_WAITINGFORPLAYERS:

		if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
			break;

		if ( lastmanstanding )
		{
			// Two players are here now, being the countdown!
			if ( LASTMANSTANDING_CountActivePlayers( ) >= 2 )
			{
				if ( sv_lmscountdowntime > 0 )
					LASTMANSTANDING_StartCountdown(( sv_lmscountdowntime * TICRATE ) - 1 );
				else
					LASTMANSTANDING_StartCountdown(( 10 * TICRATE ) - 1 );
			}
		}

		if ( teamlms )
		{
			if (( LASTMANSTANDING_TeamCountActivePlayers( TEAM_BLUE ) >= 1 ) &&
				( LASTMANSTANDING_TeamCountActivePlayers( TEAM_RED ) >= 1 ))
			{
				if ( sv_lmscountdowntime > 0 )
					LASTMANSTANDING_StartCountdown(( sv_lmscountdowntime * TICRATE ) - 1 );
				else
					LASTMANSTANDING_StartCountdown(( 10 * TICRATE ) - 1 );
			}
		}
		break;
	case LMSS_COUNTDOWN:

		if ( g_ulLMSCountdownTicks )
		{
			g_ulLMSCountdownTicks--;

			// FIGHT!
			if (( g_ulLMSCountdownTicks == 0 ) && ( NETWORK_GetState( ) != NETSTATE_CLIENT ))
				LASTMANSTANDING_DoFight( );
			// Play "3... 2... 1..." sounds.
			else if ( g_ulLMSCountdownTicks == ( 3 * TICRATE ))
				ANNOUNCER_PlayEntry( cl_announcer, "Three" );
			else if ( g_ulLMSCountdownTicks == ( 2 * TICRATE ))
				ANNOUNCER_PlayEntry( cl_announcer, "Two" );
			else if ( g_ulLMSCountdownTicks == ( 1 * TICRATE ))
				ANNOUNCER_PlayEntry( cl_announcer, "One" );
		}
		break;
	case LMSS_INPROGRESS:

		if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
			break;

		// Check to see how many men are left standing.
		if ( lastmanstanding )
		{
			// If only one man is left standing, somebody just won!
			if ( LASTMANSTANDING_CountMenStanding( ) == 1 )
			{
				LONG	lWinner;

				lWinner = LASTMANSTANDING_GetLastManStanding( );
				if ( lWinner != -1 )
				{
					if ( NETWORK_GetState( ) == NETSTATE_SERVER )
						SERVER_Printf( PRINT_HIGH, "%s \\c-wins!\n", players[lWinner].userinfo.netname );
					else
					{
						Printf( "%s \\c-wins!\n", players[lWinner].userinfo.netname );

						if ( lWinner == consoleplayer )
							ANNOUNCER_PlayEntry( cl_announcer, "YouWin" );
					}

					// Give the winner a win.
					PLAYER_SetWins( &players[lWinner], players[lWinner].ulWins + 1 );

					// Pause for five seconds for the win sequence.
					LASTMANSTANDING_DoWinSequence( lWinner );
					GAME_SetEndLevelDelay( 5 * TICRATE );
				}
			}
			// If NOBODY is left standing, it's a draw game!
			else if ( LASTMANSTANDING_CountMenStanding( ) == 0 )
			{
				ULONG	ulIdx;

				for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
				{
					if (( playeringame[ulIdx] ) && ( PLAYER_IsTrueSpectator( &players[ulIdx] ) == false ))
						break;
				}

				if ( ulIdx != MAXPLAYERS )
				{
					if ( NETWORK_GetState( ) == NETSTATE_SERVER )
						SERVER_Printf( PRINT_HIGH, "DRAW GAME!\n" );
					else
						Printf( "DRAW GAME!\n" );

					// Pause for five seconds for the win sequence.
					LASTMANSTANDING_DoWinSequence( MAXPLAYERS );
					GAME_SetEndLevelDelay( 5 * TICRATE );
				}
			}
		}

		// Check to see how many men are left standing on each team.
		if ( teamlms )
		{
			if (( LASTMANSTANDING_TeamCountMenStanding( TEAM_BLUE ) == 0 ) || ( LASTMANSTANDING_TeamCountMenStanding( TEAM_RED ) == 0 ))
			{
				LONG	lWinner;

				lWinner = LASTMANSTANDING_TeamGetLastManStanding( );
				if ( lWinner != -1 )
				{
					if ( NETWORK_GetState( ) == NETSTATE_SERVER )
						SERVER_Printf( PRINT_HIGH, "%s \\c-wins!\n", TEAM_GetName( lWinner ));
					else
					{
						Printf( "%s \\c-wins!\n", TEAM_GetName( lWinner ));

						if ( players[consoleplayer].bOnTeam && ( lWinner == (LONG)players[consoleplayer].ulTeam ))
							ANNOUNCER_PlayEntry( cl_announcer, "YouWin" );
					}

					// Give the team a win.
					TEAM_SetWinCount( lWinner, TEAM_GetWinCount( lWinner ) + 1, false );

					// Pause for five seconds for the win sequence.
					LASTMANSTANDING_DoWinSequence( lWinner );
					GAME_SetEndLevelDelay( 5 * TICRATE );
				}
				// If NOBODY is left standing, it's a draw game!
				else
				{
					ULONG	ulIdx;

					for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
					{
						if (( playeringame[ulIdx] ) && ( PLAYER_IsTrueSpectator( &players[ulIdx] ) == false ))
							break;
					}

					if ( ulIdx != MAXPLAYERS )
					{
						if ( NETWORK_GetState( ) == NETSTATE_SERVER )
							SERVER_Printf( PRINT_HIGH, "DRAW GAME!\n" );
						else
							Printf( "DRAW GAME!\n" );

						// Pause for five seconds for the win sequence.
						LASTMANSTANDING_DoWinSequence( NUM_TEAMS );
						GAME_SetEndLevelDelay( 5 * TICRATE );
					}
				}
			}
		}
		break;
	}
}

//*****************************************************************************
//
ULONG LASTMANSTANDING_CountActivePlayers( void )
{
	ULONG	ulIdx;
	ULONG	ulPlayers;

	ulPlayers = 0;
	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if (( playeringame[ulIdx] ) && ( players[ulIdx].bSpectating == false ))
			ulPlayers++;
	}

	return ( ulPlayers );
}

//*****************************************************************************
//
ULONG LASTMANSTANDING_TeamCountActivePlayers( ULONG ulTeam )
{
	ULONG	ulIdx;
	ULONG	ulPlayers;

	ulPlayers = 0;
	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if (( playeringame[ulIdx] ) && ( players[ulIdx].bSpectating == false ) && ( players[ulIdx].bOnTeam ) && ( players[ulIdx].ulTeam == ulTeam ))
			ulPlayers++;
	}

	return ( ulPlayers );
}

//*****************************************************************************
//
LONG LASTMANSTANDING_CountMenStanding( void )
{
	ULONG	ulIdx;
	ULONG	ulNumMenStanding;

	// Not in lastmanstanding mode.
	if ( lastmanstanding == false )
		return ( -1 );

	ulNumMenStanding = 0;
	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if ( playeringame[ulIdx] && ( players[ulIdx].bSpectating == false ) && ( players[ulIdx].health > 0 ))
			ulNumMenStanding++;
	}

	return ( ulNumMenStanding );
}

//*****************************************************************************
//
LONG LASTMANSTANDING_TeamCountMenStanding( ULONG ulTeam )
{
	ULONG	ulIdx;
	ULONG	ulNumMenStanding;

	// Not in team LMS mode.
	if ( teamlms == false )
		return ( -1 );

	ulNumMenStanding = 0;
	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if ( playeringame[ulIdx] && ( players[ulIdx].bSpectating == false ) && ( players[ulIdx].health > 0 ) && ( players[ulIdx].bOnTeam ) && ( players[ulIdx].ulTeam == ulTeam ))
			ulNumMenStanding++;
	}

	return ( ulNumMenStanding );
}

//*****************************************************************************
//
LONG LASTMANSTANDING_GetLastManStanding( void )
{
	ULONG	ulIdx;

	// Not in lastmanstanding mode.
	if ( lastmanstanding == false )
		return ( -1 );

	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if ( playeringame[ulIdx] && ( players[ulIdx].health > 0 ) && ( players[ulIdx].bSpectating == false ))
			return ( ulIdx );
	}

	return ( -1 );
}

//*****************************************************************************
//
LONG LASTMANSTANDING_TeamGetLastManStanding( void )
{
	ULONG	ulIdx;

	// Not in team lastmanstanding mode.
	if ( teamlms == false )
		return ( -1 );

	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if ( playeringame[ulIdx] && ( players[ulIdx].health > 0 ) && ( players[ulIdx].bOnTeam ) && ( players[ulIdx].bSpectating == false ))
			return ( players[ulIdx].ulTeam );
	}

	return ( -1 );
}

//*****************************************************************************
//
void LASTMANSTANDING_StartCountdown( ULONG ulTicks )
{
	ULONG	ulIdx;

	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if (( playeringame[ulIdx] ) && ( players[ulIdx].pSkullBot ))
			players[ulIdx].pSkullBot->PostEvent( BOTEVENT_LMS_STARTINGCOUNTDOWN );
	}

/*
	// First, reset everyone's fragcount. This must be done before setting the state to LMSS_COUNTDOWN
	// otherwise PLAYER_SetFragcount will ignore our request.
	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if ( playeringame[ulIdx] )
			PLAYER_SetFragcount( &players[ulIdx], 0, false, false );
	}
*/
/*
	TEAM_SetFragCount( TEAM_BLUE, 0, false );
	TEAM_SetFragCount( TEAM_RED, 0, false );
*/
	// Put the game in a countdown state.
	if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
		LASTMANSTANDING_SetState( LMSS_COUNTDOWN );

	// Set the LMS countdown ticks.
	LASTMANSTANDING_SetCountdownTicks( ulTicks );

	// Announce that the fight will soon start.
	ANNOUNCER_PlayEntry( cl_announcer, "PrepareToFight" );

	// Tell clients to start the countdown.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_DoGameModeCountdown( ulTicks );
}

//*****************************************************************************
//
void LASTMANSTANDING_DoFight( void )
{
	ULONG				ulIdx;
	DHUDMessageFadeOut	*pMsg;

	// The match is now in progress.
	if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
		LASTMANSTANDING_SetState( LMSS_INPROGRESS );

	// Make sure this is 0. Can be non-zero in network games if they're slightly out of sync.
	g_ulLMSCountdownTicks = 0;

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
					players[ulIdx].pSkullBot->PostEvent( BOTEVENT_LMS_FIGHT );
			}
		}
	}

	SCOREBOARD_RefreshHUD( );
}

//*****************************************************************************
//
void LASTMANSTANDING_DoWinSequence( ULONG ulWinner )
{
	ULONG	ulIdx;

	// Put the game state in the win sequence state.
	if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
		LASTMANSTANDING_SetState( LMSS_WINSEQUENCE );

	// Tell clients to do the win sequence.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_DoGameModeWinSequence( ulWinner );

	if ( NETWORK_GetState( ) != NETSTATE_SERVER )
	{
		char				szString[64];
		DHUDMessageFadeOut	*pMsg;

		screen->SetFont( BigFont );

		if ( teamlms )
		{
			if ( ulWinner == NUM_TEAMS )
				sprintf( szString, "\\cdDRAW GAME!" );
			else if ( ulWinner == TEAM_BLUE )
				sprintf( szString, "\\chBLUE WINS!" );
			else
				sprintf( szString, "\\cgRED WINS!" );
		}
		else if ( ulWinner == MAXPLAYERS )
			sprintf( szString, "\\cdDRAW GAME!" );
		else
			sprintf( szString, "%s \\c-WINS!", players[ulWinner].userinfo.netname );
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

		szString[0] = 0;
		pMsg = new DHUDMessageFadeOut( szString,
			0.0f,
			0.0f,
			0,
			0,
			CR_RED,
			3.0f,
			2.0f );

		StatusBar->AttachMessage( pMsg, 'FRAG' );

		pMsg = new DHUDMessageFadeOut( szString,
			0.0f,
			0.0f,
			0,
			0,
			CR_RED,
			3.0f,
			2.0f );

		StatusBar->AttachMessage( pMsg, 'PLAC' );
	}

	// Award a victory or perfect medal to the winner.
	if (( lastmanstanding ) && ( NETWORK_GetState( ) != NETSTATE_CLIENT ))
	{
		LONG	lMedal;

		// If the winner has full health, give him a "Perfect!".
		if ( players[ulWinner].health == deh.MegasphereHealth )
			lMedal = MEDAL_PERFECT;
		else
			lMedal = MEDAL_VICTORY;

		// Give the player the medal.
		MEDAL_GiveMedal( ulWinner, lMedal );
		if ( NETWORK_GetState( ) == NETSTATE_SERVER )
			SERVERCOMMANDS_GivePlayerMedal( ulWinner, lMedal );
	}

	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if (( playeringame[ulIdx] ) && ( players[ulIdx].pSkullBot ))
			players[ulIdx].pSkullBot->PostEvent( BOTEVENT_LMS_WINSEQUENCE );
	}
}

//*****************************************************************************
//*****************************************************************************
//
ULONG LASTMANSTANDING_GetCountdownTicks( void )
{
	return ( g_ulLMSCountdownTicks );
}

//*****************************************************************************
//
void LASTMANSTANDING_SetCountdownTicks( ULONG ulTicks )
{
	g_ulLMSCountdownTicks = ulTicks;
}

//*****************************************************************************
//
LMSSTATE_e LASTMANSTANDING_GetState( void )
{
	return ( g_LMSState );
}

//*****************************************************************************
//
void LASTMANSTANDING_SetState( LMSSTATE_e State )
{
	ULONG	ulIdx;

	g_LMSState = State;

	// Tell clients about the state change.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_SetGameModeState( State );

	switch ( State )
	{
	case LMSS_WINSEQUENCE:

		break;
	case LMSS_WAITINGFORPLAYERS:

		// Zero out the countdown ticker.
		LASTMANSTANDING_SetCountdownTicks( 0 );

		if (( NETWORK_GetState( ) != NETSTATE_CLIENT ) && ( lastmanstanding || teamlms ) && ( gamestate != GS_FULLCONSOLE ))
		{
			// Respawn any players who were downed during the previous round.
			for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
			{
				if (( playeringame[ulIdx] == false ) ||
					( PLAYER_IsTrueSpectator( &players[ulIdx] )))
				{
					continue;
				}

				if ( gameaction != ga_worlddone )
				{
					players[ulIdx].bSpectating = false;
					players[ulIdx].bDeadSpectator = false;
					players[ulIdx].playerstate = PST_REBORNNOINVENTORY;

					if (( players[ulIdx].mo ) && ( players[ulIdx].mo->health > 0 ))
					{
						if ( NETWORK_GetState( ) == NETSTATE_SERVER )
							SERVERCOMMANDS_DestroyThing( players[ulIdx].mo );

						players[ulIdx].mo->Destroy( );
						players[ulIdx].mo = NULL;
					}

					G_DeathMatchSpawnPlayer( ulIdx, true );
				}
				else
				{
					if ( players[ulIdx].bDeadSpectator )
						players[ulIdx].bDeadSpectator = false;
				}
			}

			// Let anyone who's been waiting in line join now.
			JOINQUEUE_PopQueue( -1 );
		}
		break;
	}

	// Since some players might have respawned, update the server console window.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
	{
		for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
		{
			if ( playeringame[ulIdx] )
				SERVERCONSOLE_UpdatePlayerInfo( ulIdx, UDF_FRAGS );
		}
	}
}

//*****************************************************************************
//
bool LASTMANSTANDING_GetStartNextMatchOnLevelLoad( void )
{
	return ( g_bStartNextMatchOnLevelLoad );
}

//*****************************************************************************
//
void LASTMANSTANDING_SetStartNextMatchOnLevelLoad( bool bStart )
{
	g_bStartNextMatchOnLevelLoad = bStart;
}

//*****************************************************************************
//	CONSOLE COMMANDS/VARIABLES

CVAR( Int, sv_lmscountdowntime, 10, CVAR_ARCHIVE );
CUSTOM_CVAR( Int, winlimit, 0, CVAR_SERVERINFO | CVAR_CAMPAIGNLOCK )
{
	if (( NETWORK_GetState( ) == NETSTATE_SERVER ) && ( gamestate != GS_STARTUP ))
	{
		SERVER_Printf( PRINT_HIGH, "%s changed to: %d\n", self.GetName( ), (LONG)self );
		SERVERCOMMANDS_SetGameModeLimits( );

		// Update the scoreboard.
		SERVERCONSOLE_UpdateScoreboard( );
	}
}

CUSTOM_CVAR( Int, lmsallowedweapons, LMS_AWF_ALLALLOWED, CVAR_SERVERINFO )
{
	if (( NETWORK_GetState( ) == NETSTATE_SERVER ) && ( gamestate != GS_STARTUP ))
	{
		SERVER_Printf( PRINT_HIGH, "%s changed to: %d\n", self.GetName( ), (LONG)self );
		if ( lastmanstanding || teamlms )
			SERVERCOMMANDS_SetLMSAllowedWeapons( );
	}
}
CVAR( Flag, lms_allowpistol, lmsallowedweapons, LMS_AWF_PISTOL );
CVAR( Flag, lms_allowshotgun, lmsallowedweapons, LMS_AWF_SHOTGUN );
CVAR( Flag, lms_allowssg, lmsallowedweapons, LMS_AWF_SSG );
CVAR( Flag, lms_allowchaingun, lmsallowedweapons, LMS_AWF_CHAINGUN );
CVAR( Flag, lms_allowminigun, lmsallowedweapons, LMS_AWF_MINIGUN );
CVAR( Flag, lms_allowrocketlauncher, lmsallowedweapons, LMS_AWF_ROCKETLAUNCHER );
CVAR( Flag, lms_allowgrenadelauncher, lmsallowedweapons, LMS_AWF_GRENADELAUNCHER );
CVAR( Flag, lms_allowplasma, lmsallowedweapons, LMS_AWF_PLASMA );
CVAR( Flag, lms_allowrailgun, lmsallowedweapons, LMS_AWF_RAILGUN );
CVAR( Flag, lms_allowchainsaw, lmsallowedweapons, LMS_AWF_CHAINSAW );

CUSTOM_CVAR( Int, lmsspectatorsettings, LMS_SPF_VIEW, CVAR_SERVERINFO )
{
	if (( NETWORK_GetState( ) == NETSTATE_SERVER ) && ( gamestate != GS_STARTUP ))
	{
		SERVER_Printf( PRINT_HIGH, "%s changed to: %d\n", self.GetName( ), (LONG)self );
		if ( lastmanstanding || teamlms )
			SERVERCOMMANDS_SetLMSSpectatorSettings( );
	}
}
CVAR( Flag, lms_spectatorchat, lmsspectatorsettings, LMS_SPF_CHAT );
CVAR( Flag, lms_spectatorview, lmsspectatorsettings, LMS_SPF_VIEW );
