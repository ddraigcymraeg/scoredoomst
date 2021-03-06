/*
** sv_master.cpp
** Contains variables and routines related to the master server
** of the program.
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

#include <winsock2.h>

#define USE_WINDOWS_DWORD
#include "c_dispatch.h"
#include "cooperative.h"
#include "deathmatch.h"
#include "d_player.h"
#include "duel.h"
#include "g_game.h"
#include "gi.h"
#include "lastmanstanding.h"
#include "team.h"
#include "network.h"
#include "sv_main.h"
#include "sv_ban.h"
#include "version.h"

//*****************************************************************************
//	VARIABLES

// Address of master server.
static	netadr_t			g_AddressMasterServer;

// Message buffer for sending messages to the master server.
static	sizebuf_t			g_MasterServerBuffer;

// Port the master server is located on.
static	LONG				g_lMasterPort;

// List of IP address that this server has been queried by recently.
static	STORED_QUERY_IP_t	g_StoredQueryIPs[MAX_STORED_QUERY_IPS];

static	LONG				g_lStoredQueryIPHead;
static	LONG				g_lStoredQueryIPTail;

//*****************************************************************************
//	FUNCTIONS

void SERVER_MASTER_Construct( void )
{
	char	*pszPort;

	// Setup our message buffer.
	NETWORK_InitBuffer( &g_MasterServerBuffer, MAX_UDP_PACKET );
	NETWORK_ClearBuffer( &g_MasterServerBuffer );

	// Allow the user to specify which port the master server is on.
	pszPort = Args.CheckValue( "-masterport" );
    if ( pszPort )
    {
       g_lMasterPort = atoi( pszPort );
       Printf( PRINT_HIGH, "Alternate master server port: %d.\n", g_lMasterPort );
    }
	else
	   g_lMasterPort = DEFAULT_MASTER_PORT;

	g_lStoredQueryIPHead = 0;
	g_lStoredQueryIPTail = 0;
}

//*****************************************************************************
//
void SERVER_MASTER_Tick( void )
{
	UCVarValue	Val;

	while (( g_lStoredQueryIPHead != g_lStoredQueryIPTail ) && ( gametic >= g_StoredQueryIPs[g_lStoredQueryIPHead].lNextAllowedGametic ))
	{
		g_lStoredQueryIPHead++;
		g_lStoredQueryIPHead = g_lStoredQueryIPHead % MAX_STORED_QUERY_IPS;
	}

	// Send an update to the master server every 30 seconds.
	if ( gametic % ( TICRATE * 30 ))
		return;

	// User doesn't wish to update the master server.
	if ( sv_updatemaster == false )
		return;

	NETWORK_ClearBuffer( &g_MasterServerBuffer );

	Val = sv_masterip.GetGenericRep( CVAR_String );
	NETWORK_StringToAddress( Val.String, &g_AddressMasterServer );
	I_SetPort( g_AddressMasterServer, g_lMasterPort );

	// Write to our packet a challenge to the master server.
	Val = sv_masteroverrideip.GetGenericRep( CVAR_String );
	if ( Val.String[0] == '\0' )
		NETWORK_WriteLong( &g_MasterServerBuffer, SERVER_MASTER_CHALLENGE );
	else
	{
		netadr_t	OverrideIP;

		NETWORK_WriteLong( &g_MasterServerBuffer, SERVER_MASTER_CHALLENGE_OVERRIDE );

		NETWORK_StringToAddress( Val.String, &OverrideIP );
		NETWORK_WriteByte( &g_MasterServerBuffer, OverrideIP.ip[0] );
		NETWORK_WriteByte( &g_MasterServerBuffer, OverrideIP.ip[1] );
		NETWORK_WriteByte( &g_MasterServerBuffer, OverrideIP.ip[2] );
		NETWORK_WriteByte( &g_MasterServerBuffer, OverrideIP.ip[3] );
		NETWORK_WriteShort( &g_MasterServerBuffer, NETWORK_GetLocalPort( ));
	}

	// Send the master server our packet.
//	NETWORK_LaunchPacket( &g_MasterServerBuffer, g_AddressMasterServer, true );
	NETWORK_LaunchPacket( &g_MasterServerBuffer, g_AddressMasterServer );
}

//*****************************************************************************
//
void SERVER_MASTER_Broadcast( void )
{
	netadr_t	AddressBroadcast;
	sockaddr_in	broadcast_addr;

	// Send an update to the master server every second.
	if ( gametic % TICRATE )
		return;

	// User doesn't wish to broadcast this server.
	if (( sv_broadcast == false ) || Args.CheckParm( "-nobroadcast" ))
		return;

//	NETWORK_ClearBuffer( &g_MasterServerBuffer );

	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_addr.s_addr = INADDR_BROADCAST;
	broadcast_addr.sin_port = htons( DEFAULT_BROADCAST_PORT );
	NETWORK_SocketAddressToNetAddress( &broadcast_addr, &AddressBroadcast );

	// Broadcast our packet.
	SERVER_MASTER_SendServerInfo( AddressBroadcast, SQF_ALL, 0 );
//	NETWORK_WriteLong( &g_MasterServerBuffer, MASTER_CHALLENGE );
//	NETWORK_LaunchPacket( g_MasterServerBuffer, AddressBroadcast, true );
}

//*****************************************************************************
//
void SERVER_MASTER_SendServerInfo( netadr_t Address, ULONG ulFlags, ULONG ulTime )
{
	UCVarValue	Val;
	char		szAddress[4][4];
	ULONG		ulIdx;
	ULONG		ulBits;
	ULONG		ulNumPWADs;

	// Let's just use the master server buffer! It gets cleared again when we need it anyway!
	NETWORK_ClearBuffer( &g_MasterServerBuffer );

	// First, check to see if we've been queried by this address recently.
	if ( g_lStoredQueryIPHead != g_lStoredQueryIPTail )
	{
		ulIdx = g_lStoredQueryIPHead;
		while ( ulIdx != (ULONG)g_lStoredQueryIPTail )
		{
			// Check to see if this IP exists in our stored query IP list. If it does, then
			// ignore it, since it queried us less than 10 seconds ago.
			if ( NETWORK_CompareAddress( Address, g_StoredQueryIPs[ulIdx].Address, true ))
			{
				// Write our header.
				NETWORK_WriteLong( &g_MasterServerBuffer, SERVER_LAUNCHER_IGNORING );

				// Send the time the launcher sent to us.
				NETWORK_WriteLong( &g_MasterServerBuffer, ulTime );

				// Send the packet.
//				NETWORK_LaunchPacket( &g_MasterServerBuffer, Address, true );
				NETWORK_LaunchPacket( &g_MasterServerBuffer, Address );

				if ( sv_showlauncherqueries )
					Printf( "Ignored IP launcher challenge.\n" );

				// Nothing more to do here.
				return;
			}

			ulIdx++;
			ulIdx = ulIdx % MAX_STORED_QUERY_IPS;
		}
	}

	// Now, check to see if this IP has been banend from this server.
	itoa( Address.ip[0], szAddress[0], 10 );
	itoa( Address.ip[1], szAddress[1], 10 );
	itoa( Address.ip[2], szAddress[2], 10 );
	itoa( Address.ip[3], szAddress[3], 10 );
	if (( sv_enforcebans ) && ( SERVERBAN_IsIPBanned( szAddress[0], szAddress[1], szAddress[2], szAddress[3] )))
	{
		// Write our header.
		NETWORK_WriteLong( &g_MasterServerBuffer, SERVER_LAUNCHER_BANNED );

		// Send the time the launcher sent to us.
		NETWORK_WriteLong( &g_MasterServerBuffer, ulTime );

		// Send the packet.
		NETWORK_LaunchPacket( &g_MasterServerBuffer, Address );

		if ( sv_showlauncherqueries )
			Printf( "Denied BANNED IP launcher challenge.\n" );

		// Nothing more to do here.
		return;
	}

	// This IP didn't exist in the list. and it wasn't banned.
	// So, add it, and keep it there for 10 seconds.
	g_StoredQueryIPs[g_lStoredQueryIPTail].Address = Address;
	g_StoredQueryIPs[g_lStoredQueryIPTail].lNextAllowedGametic = gametic + ( TICRATE * ( sv_queryignoretime ));

	g_lStoredQueryIPTail++;
	g_lStoredQueryIPTail = g_lStoredQueryIPTail % MAX_STORED_QUERY_IPS;
	if ( g_lStoredQueryIPTail == g_lStoredQueryIPHead )
		Printf( "SERVER_MASTER_SendServerInfo: WARNING! g_lStoredQueryIPTail == g_lStoredQueryIPHead\n" );

	// Write our header.
	NETWORK_WriteLong( &g_MasterServerBuffer, SERVER_LAUNCHER_CHALLENGE );

	// Send the time the launcher sent to us.
	NETWORK_WriteLong( &g_MasterServerBuffer, ulTime );

	// Send our version.
	NETWORK_WriteString( &g_MasterServerBuffer, DOTVERSIONSTR );

	// Send the information about the data that will be sent.
	ulBits = ulFlags;

	// If the launcher desires to know the team damage, but we're not in a game mode where
	// team damage applies, then don't send back team damage information.
	if (( teamplay || teamgame || teamlms || teampossession || teamcoop || (( deathmatch == false ) && ( teamgame == false ))) == false )
	{
		if ( ulBits & SQF_TEAMDAMAGE )
			ulBits &= ~SQF_TEAMDAMAGE;
	}

	// If the launcher desires to know the team score, but we're not in a game mode where
	// teams have scores, then don't send back team score information.
	if (( teamplay || teamgame || teamlms || teampossession || teamcoop ) == false )
	{
		if ( ulBits & SQF_TEAMSCORES )
			ulBits &= ~SQF_TEAMSCORES;
	}

	// If the launcher wants to know player data, then we have to tell them how many players
	// are in the server.
	if ( ulBits & SQF_PLAYERDATA )
		ulBits |= SQF_NUMPLAYERS;

	NETWORK_WriteLong( &g_MasterServerBuffer, ulBits );

	// Send the server name.
	if ( ulBits & SQF_NAME )
	{
		Val = sv_hostname.GetGenericRep( CVAR_String );
		NETWORK_WriteString( &g_MasterServerBuffer, Val.String );
	}

	// Send the website URL.
	if ( ulBits & SQF_URL )
	{
		Val = sv_website.GetGenericRep( CVAR_String );
		NETWORK_WriteString( &g_MasterServerBuffer, Val.String );
	}

	// Send the host's e-mail address.
	if ( ulBits & SQF_EMAIL )
	{
		Val = sv_hostemail.GetGenericRep( CVAR_String );
		NETWORK_WriteString( &g_MasterServerBuffer, Val.String );
	}

	if ( ulBits & SQF_MAPNAME )
		NETWORK_WriteString( &g_MasterServerBuffer, level.mapname );

	if ( ulBits & SQF_MAXCLIENTS )
		NETWORK_WriteByte( &g_MasterServerBuffer, sv_maxclients );

	if ( ulBits & SQF_MAXPLAYERS )
		NETWORK_WriteByte( &g_MasterServerBuffer, sv_maxplayers );

	// Send out the PWAD information.
	if ( ulBits & SQF_PWADS )
	{
		ulNumPWADs = 0;
		for ( ulIdx = 0; Wads.GetWadName( ulIdx ) != NULL; ulIdx++ )
		{
			// Skip the IWAD file index, skulltag.wad/pk3, and files that were automatically
			// loaded from subdirectories (such as skin files).
			if (( ulIdx == FWadCollection::IWAD_FILENUM ) ||
				( stricmp( Wads.GetWadName( ulIdx ), "scoredoomst.wad" ) == 0 ) ||
				( stricmp( Wads.GetWadName( ulIdx ), "scoredoomst.pk3" ) == 0 ) ||
				( Wads.GetLoadedAutomatically( ulIdx )))
			{
				continue;
			}

			ulNumPWADs++;
		}

		NETWORK_WriteByte( &g_MasterServerBuffer, ulNumPWADs );
		for ( ulIdx = 0; Wads.GetWadName( ulIdx ) != NULL; ulIdx++ )
		{
			// Skip the IWAD file index, skulltag.wad/pk3, and files that were automatically
			// loaded from subdirectories (such as skin files).
			if (( ulIdx == FWadCollection::IWAD_FILENUM ) ||
				( stricmp( Wads.GetWadName( ulIdx ), "scoredoomst.wad" ) == 0 ) ||
				( stricmp( Wads.GetWadName( ulIdx ), "scoredoomst.pk3" ) == 0 ) ||
				( Wads.GetLoadedAutomatically( ulIdx )))
			{
				continue;
			}

			NETWORK_WriteString( &g_MasterServerBuffer, (char *)Wads.GetWadName( ulIdx ));
		}
	}

	if ( ulBits & SQF_GAMETYPE )
	{
		NETWORK_WriteByte( &g_MasterServerBuffer, GAME_GetGameType( ));
		NETWORK_WriteByte( &g_MasterServerBuffer, instagib );
		NETWORK_WriteByte( &g_MasterServerBuffer, buckshot );
	}

	if ( ulBits & SQF_GAMENAME )
		NETWORK_WriteString( &g_MasterServerBuffer, SERVER_MASTER_GetGameName( ));

	if ( ulBits & SQF_IWAD )
		NETWORK_WriteString( &g_MasterServerBuffer, (char *)Wads.GetWadName( FWadCollection::IWAD_FILENUM ));

	if ( ulBits & SQF_FORCEPASSWORD )
		NETWORK_WriteByte( &g_MasterServerBuffer, sv_forcepassword );

	if ( ulBits & SQF_FORCEJOINPASSWORD )
		NETWORK_WriteByte( &g_MasterServerBuffer, sv_forcejoinpassword );

	if ( ulBits & SQF_GAMESKILL )
		NETWORK_WriteByte( &g_MasterServerBuffer, gameskill );

	if ( ulBits & SQF_BOTSKILL )
		NETWORK_WriteByte( &g_MasterServerBuffer, botskill );

	if ( ulBits & SQF_DMFLAGS )
	{
		NETWORK_WriteLong( &g_MasterServerBuffer, dmflags );
		NETWORK_WriteLong( &g_MasterServerBuffer, dmflags2 );
		NETWORK_WriteLong( &g_MasterServerBuffer, compatflags );
	}

	if ( ulBits & SQF_LIMITS )
	{
		NETWORK_WriteShort( &g_MasterServerBuffer, fraglimit );
		NETWORK_WriteShort( &g_MasterServerBuffer, (SHORT)timelimit );
		if ( timelimit )
		{
			LONG	lTimeLeft;

			lTimeLeft = (LONG)( timelimit - ( level.totaltime / ( TICRATE * 60 ))); //ghk
			if ( lTimeLeft < 0 )
				lTimeLeft = 0;
			NETWORK_WriteShort( &g_MasterServerBuffer, lTimeLeft );
		}
		NETWORK_WriteShort( &g_MasterServerBuffer, duellimit );
		NETWORK_WriteLong( &g_MasterServerBuffer, pointlimit ); //ghk writelong
		NETWORK_WriteShort( &g_MasterServerBuffer, winlimit );
	}

	// Send the team damage scale.
	if ( teamplay || teamgame || teamlms || teampossession || teamcoop || (( deathmatch == false ) && ( teamgame == false )))
	{
		if ( ulBits & SQF_TEAMDAMAGE )
			NETWORK_WriteFloat( &g_MasterServerBuffer, teamdamage );
	}

	// Send the team scores.
	if ( teamplay || teamgame || teamlms || teampossession || teamcoop )
	{
		if ( ulBits & SQF_TEAMSCORES )
		{
			for ( ulIdx = 0; ulIdx < NUM_TEAMS; ulIdx++ )
			{
				if ( teamplay )
					NETWORK_WriteShort( &g_MasterServerBuffer, TEAM_GetFragCount( ulIdx ));
				else if ( teamlms )
					NETWORK_WriteShort( &g_MasterServerBuffer, TEAM_GetWinCount( ulIdx ));
				else
					NETWORK_WriteShort( &g_MasterServerBuffer, TEAM_GetScore( ulIdx )); //ghk writelong?
			}
		}
	}

	if ( ulBits & SQF_NUMPLAYERS )
		NETWORK_WriteByte( &g_MasterServerBuffer, SERVER_CalcNumPlayers( ));

	if ( ulBits & SQF_PLAYERDATA )
	{
		for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
		{
			if ( playeringame[ulIdx] == false )
				continue;

			NETWORK_WriteString( &g_MasterServerBuffer, players[ulIdx].userinfo.netname );
			if ( teamgame || possession || teampossession || cooperative || teamcoop )
				NETWORK_WriteLong( &g_MasterServerBuffer, players[ulIdx].lPointCount );//ghk writelong
			else if ( deathmatch )
				NETWORK_WriteLong( &g_MasterServerBuffer, players[ulIdx].fragcount ); //ghk writelong
			else
				NETWORK_WriteLong( &g_MasterServerBuffer, players[ulIdx].killcount ); //ghk writelong

			NETWORK_WriteShort( &g_MasterServerBuffer, players[ulIdx].ulPing );
			NETWORK_WriteByte( &g_MasterServerBuffer, PLAYER_IsTrueSpectator( &players[ulIdx] ));
			NETWORK_WriteByte( &g_MasterServerBuffer, players[ulIdx].bIsBot );

			if ( teamplay || teamgame || teamlms || teampossession || teamcoop )
			{
				if ( players[ulIdx].bOnTeam == false )
					NETWORK_WriteByte( &g_MasterServerBuffer, 255 );
				else
					NETWORK_WriteByte( &g_MasterServerBuffer, players[ulIdx].ulTeam );
			}

			NETWORK_WriteByte( &g_MasterServerBuffer, players[ulIdx].ulTime / ( TICRATE * 60 ));
		}
	}

//	NETWORK_LaunchPacket( &g_MasterServerBuffer, Address, true );
	NETWORK_LaunchPacket( &g_MasterServerBuffer, Address );
}

//*****************************************************************************
//
char *SERVER_MASTER_GetGameName( void )
{
	switch ( gameinfo.gametype )
	{
	case GAME_Doom:

		if ( gamemode != commercial )
			return ( "DOOM" );
		else
			return ( "DOOM II" );
		break;
	case GAME_Heretic:

		return ( "Heretic" );
		break;
	case GAME_Hexen:

		return ( "Hexen" );
		break;
	default:

		return ( "ERROR!" );
		break;
	}
}

//*****************************************************************************
//	CONSOLE VARIABLES/COMMANDS

// Should the server inform the master server of its existance?
CVAR( Bool, sv_updatemaster, true, CVAR_SERVERINFO )

// Should the server broadcast so LAN clients can hear it?
CVAR( Bool, sv_broadcast, true, CVAR_ARCHIVE )

// Name of this server on launchers.
void	SERVERCONSOLE_UpdateTitleString( char *pszString );
CUSTOM_CVAR( String, sv_hostname, "Unnamed ScoreDoomST server", CVAR_ARCHIVE )
{
	SERVERCONSOLE_UpdateTitleString( (char *)(const char *)self );
}

// Website that has the wad this server is using, possibly with other info.
CVAR( String, sv_website, "http://www.scoredoom.com/", CVAR_ARCHIVE )

// E-mail address of the person running this server.
CVAR( String, sv_hostemail, "", CVAR_ARCHIVE )

// IP address of the master server.
CVAR( String, sv_masterip, "scoredoom.dyndns.com", CVAR_ARCHIVE )

// IP that the master server should use for this server.
CVAR( String, sv_masteroverrideip, "", CVAR_ARCHIVE )

CCMD( wads )
{
	ULONG		ulIdx;
	ULONG		ulNumPWADs;

	Printf( "IWAD: %s\n", Wads.GetWadName( FWadCollection::IWAD_FILENUM ));

	ulNumPWADs = 0;
	for ( ulIdx = 0; Wads.GetWadName( ulIdx ) != NULL; ulIdx++ )
	{
		// Skip the IWAD file index, skulltag.wad/pk3, and files that were automatically
		// loaded from subdirectories (such as skin files).
		if (( ulIdx == FWadCollection::IWAD_FILENUM ) ||
			( stricmp( Wads.GetWadName( ulIdx ), "scoredoomst.wad" ) == 0 ) ||
			( stricmp( Wads.GetWadName( ulIdx ), "scoredoomst.pk3" ) == 0 ) ||
			( Wads.GetLoadedAutomatically( ulIdx )))
		{
			continue;
		}

		ulNumPWADs++;
	}

	Printf( "Num PWADs: %d\n", ulNumPWADs );
	for ( ulIdx = 0; Wads.GetWadName( ulIdx ) != NULL; ulIdx++ )
	{
		// Skip the IWAD file index, skulltag.wad/pk3, and files that were automatically
		// loaded from subdirectories (such as skin files).
		if (( ulIdx == FWadCollection::IWAD_FILENUM ) ||
			( stricmp( Wads.GetWadName( ulIdx ), "scoredoomst.wad" ) == 0 ) ||
			( stricmp( Wads.GetWadName( ulIdx ), "scoredoomst.pk3" ) == 0 ) ||
			( Wads.GetLoadedAutomatically( ulIdx )))
		{
			continue;
		}

		Printf( "PWAD: %s\n", Wads.GetWadName( ulIdx ));
	}
}
