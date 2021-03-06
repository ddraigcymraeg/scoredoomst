/*
** browser.h
** Contains browser structures and prototypes
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

#ifndef __BROWSER_H__
#define __BROWSER_H__

#include "network.h"

//*****************************************************************************
//  DEFINES

// Maximum number of servers listed in the browser.
#define		MAX_BROWSER_SERVERS		256

//*****************************************************************************
enum
{
	AS_INACTIVE,
	AS_WAITINGFORREPLY,
	AS_ACTIVE,

	NUM_ACTIVESTATES
};

//*****************************************************************************
//	STRUCTURES

//*****************************************************************************
typedef struct
{
	// Player's name.
	char	szName[32];

	// Fragcount.
	LONG	lFragcount;

	// Ping.
	LONG	lPing;

	// Spectating?
	bool	bSpectating;

	// Is a bot?
	bool	bIsBot;

} SERVERPLAYER_t;

//*****************************************************************************
typedef struct
{
	// What's the state of this server's activity?
	ULONG		ulActiveState;

	// IP address of this server.
	netadr_t	Address;

	// Name of the server.
	char		szHostName[256];

	// Website URL of the wad the server is using.
	char		szWadURL[256];

	// Host's email address.
	char		szEmailAddress[128];

	// Mapname of the level the server is currently on.
	char		szMapname[32];

	// Maximum number of players that can join the server.
	LONG		lMaxClients;

	// Number of PWADs the server is using.
	LONG		lNumPWADs;

	// Names of each PWAD the server is using.
	char		szPWADNames[32][32];

	// Name of the IWAD being used.
	char		szIWADName[32];

	// Gametype of the server.
	LONG		lGameType;

	// Number of players on the server.
	LONG		lNumPlayers;

	// Player's playing on the server.
	SERVERPLAYER_t	Players[MAXPLAYERS];

	// Version of the server.
	char		szVersion[32];

	// Was this server broadcasted to us on a LAN?
	bool		bLAN;

	// MS time of when we queried this server.
	LONG		lMSTime;

	// Ping to this server.
	LONG		lPing;

} SERVER_t;

//*****************************************************************************
//	PROTOTYPES

void	BROWSER_Construct( void );

bool		BROWSER_IsActive( ULONG ulServer );
bool		BROWSER_IsLAN( ULONG ulServer );
netadr_t	BROWSER_GetAddress( ULONG ulServer );
char		*BROWSER_GetHostName( ULONG ulServer );
char		*BROWSER_GetWadURL( ULONG ulServer );
char		*BROWSER_GetEmailAddress( ULONG ulServer );
char		*BROWSER_GetMapname( ULONG ulServer );
LONG		BROWSER_GetMaxClients( ULONG ulServer );
LONG		BROWSER_GetNumPWADs( ULONG ulServer );
char		*BROWSER_GetPWADName( ULONG ulServer, ULONG ulWadIdx );
char		*BROWSER_GetIWADName( ULONG ulServer );
LONG		BROWSER_GetGameType( ULONG ulServer );
LONG		BROWSER_GetNumPlayers( ULONG ulServer );
char		*BROWSER_GetPlayerName( ULONG ulServer, ULONG ulPlayer );
LONG		BROWSER_GetPlayerFragcount( ULONG ulServer, ULONG ulPlayer );
LONG		BROWSER_GetPlayerPing( ULONG ulServer, ULONG ulPlayer );
LONG		BROWSER_GetPlayerSpectating( ULONG ulServer, ULONG ulPlayer );
LONG		BROWSER_GetPing( ULONG ulServer );
char		*BROWSER_GetVersion( ULONG ulServer );

void	BROWSER_ClearServerList( void );
void	BROWSER_DeactivateAllServers( void );
void	BROWSER_GetServerList( void );
void	BROWSER_ParseServerQuery( bool bLAN );
void	BROWSER_QueryMasterServer( void );
bool	BROWSER_WaitingForMasterResponse( void );
void	BROWSER_QueryAllServers( void );
LONG	BROWSER_CalcNumServers( void );

//*****************************************************************************
//	EXTERNAL CONSOLE VARIABLES

EXTERN_CVAR( String, cl_masterip );

#endif // __BROWSER_H__