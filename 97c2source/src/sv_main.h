/*
** sv_commands.h
** Contains variables and routines related to the server portion of the program.
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

#ifndef __SV_MAIN_H__
#define __SV_MAIN_H__

#include "actor.h"
#include "d_player.h"
#include "i_net.h"
#include "s_sndseq.h"

//*****************************************************************************
//	DEFINES

// Interval of time that clients' chat instances are reset at.
#define	CHATINSTANCE_RESET_INTERVAL	( TICRATE * 2 )

// Maximum amount of chat instance times we store.
#define	MAX_CHATINSTANCE_STORAGE	4

// Maximum amount of user info change instance times we store.
#define	MAX_USERINFOINSTANCE_STORAGE	4

// Number of seconds before a client times out.
#define CLIENT_TIMEOUT				65

// Maximum size of the packets sent out by the server.
#define	MAX_UDP_PACKET				8192

// How many commands can be stored at one time?
#define	MAX_STORED_SERVER_COMMANDS	64

// Define this to use one method of the multitick hack fix.
//#define	MULTITICK_HACK_FIX

// Amount of time the client has to report his checksum of the level.
#define	CLIENT_CHECKSUM_WAITTIME	( 15 * TICRATE )

// This is for the server console, but since we normally can't include that (win32 stuff),
// we can just put it here.
#define	UDF_NAME					0x00000001
#define	UDF_FRAGS					0x00000002
#define	UDF_PING					0x00000004
#define	UDF_TIME					0x00000008

#define	MAX_OVERMOVEMENT_LEVEL		( 2 * TICRATE )

#define	KILOBYTE					1024
#define	MEGABYTE					( KILOBYTE * 1024 )
#define	GIGABYTE					( MEGABYTE * 1024 )

#define	MINUTE						60
#define	HOUR						( MINUTE * 60 )
#define	DAY							( HOUR * 24 )

// Server query flags.
#define	SQF_NAME					0x00000001
#define	SQF_URL						0x00000002
#define	SQF_EMAIL					0x00000004
#define	SQF_MAPNAME					0x00000008
#define	SQF_MAXCLIENTS				0x00000010
#define	SQF_MAXPLAYERS				0x00000020
#define	SQF_PWADS					0x00000040
#define	SQF_GAMETYPE				0x00000080
#define	SQF_GAMENAME				0x00000100
#define	SQF_IWAD					0x00000200
#define	SQF_FORCEPASSWORD			0x00000400
#define	SQF_FORCEJOINPASSWORD		0x00000800
#define	SQF_GAMESKILL				0x00001000
#define	SQF_BOTSKILL				0x00002000
#define	SQF_DMFLAGS					0x00004000
#define	SQF_LIMITS					0x00010000
#define	SQF_TEAMDAMAGE				0x00020000
#define	SQF_TEAMSCORES				0x00040000
#define	SQF_NUMPLAYERS				0x00080000
#define	SQF_PLAYERDATA				0x00100000

#define	SQF_ALL						( SQF_NAME|SQF_URL|SQF_EMAIL|SQF_MAPNAME|SQF_MAXCLIENTS|SQF_MAXPLAYERS| \
									  SQF_PWADS|SQF_GAMETYPE|SQF_GAMENAME|SQF_IWAD|SQF_FORCEPASSWORD|SQF_FORCEJOINPASSWORD|SQF_GAMESKILL| \
									  SQF_BOTSKILL|SQF_DMFLAGS|SQF_LIMITS|SQF_TEAMDAMAGE|SQF_TEAMSCORES|SQF_NUMPLAYERS|SQF_PLAYERDATA )

#define	SVFWF_PLAYATTACKING2		0x00000001
#define	SVFWF_FLASHSTATE			0x00000002

#define	MAX_STORED_QUERY_IPS		512

//*****************************************************************************
typedef enum
{
	// Client slot can be used for a new connection.
	CLS_FREE,

	// Client slot has just received a connection signal.
	CLS_CHALLENGE,

	// Client has authenticated his level.
	CLS_AUTHENTICATED,

	// Connection has been received, but has not been spawned in the game yet.
	CLS_CONNECTED,

	// Client is in the game.
	CLS_SPAWNED,

} CLIENTSTATE_e;

//*****************************************************************************
//	STRUCTURES

typedef struct
{
	BYTE    ip[4];
	unsigned short  port;
	unsigned short  pad;
} netadr_t;

//*****************************************************************************
typedef struct sizebuf_s
{
	bool	allowoverflow;	// if false, do a Com_Error
	bool	overflowed;		// set to true if the buffer size failed

	// Unfortunaly, ZDaemon uses two different definitions of sizebuf_t. Attempt
	// to combine the structures here by having two sets of data.
	// Servers use this.
	BYTE	*pbData;

	// Clients use this this.
	BYTE	bData[MAX_UDP_PACKET];

	int		maxsize;
	int		cursize;
	int		readcount;

} sizebuf_t;

//*****************************************************************************
typedef struct
{
	netadr_t		address;

	// Client state (free, in game, etc.)
	CLIENTSTATE_e	State;
	
	sizebuf_t		netbuf;

	sizebuf_t		UnreliablePacketBuffer;

	int				lastcmdtic;

	// Used for calculating pings.
    int				lastgametic;

	// Client needs full update.
	bool			needfullupdate;

	// Can client remotely control server?
	bool			bRCONAccess;

	// Which pair of eyes is this client spying through (spectator)?
	ULONG			ulDisplayPlayer;

	// Client's gametics.
	int				gametics;

	sizebuf_t	relpackets; // save reliable packets here

	LONG		lPacketBeginning[256];

	// This is the packet size for each of the 256 stored packets.
	LONG		lPacketSize[256];

	// This is the packet sequence for each of the 256 stored packets.
	LONG		lPacketSequence[256];

	// Client wants tp start each round as a spectator.
	bool		bWantStartAsSpectator;

	// Client doesn't want his fragcount restored if he is reconnecting to the server.
	bool		bWantNoRestoreFrags;

	// Last packet number sent to this client.
	ULONG		ulPacketSequence;

	// If this client was dead at the end of the last level, he needs to be "reborn" when 
	// he respawns.
	bool		bDeadLastLevel;

	// A record of the gametic the client spoke at. We store the last MAX_CHATINSTANCE_STORAGE
	// times the client chatted. This is used to chat spam protection.
	LONG		lChatInstances[MAX_CHATINSTANCE_STORAGE];
	ULONG		ulLastChatInstance;

	// A record of the gametic the client spoke at. We store the last MAX_CHATINSTANCE_STORAGE
	// times the client chatted. This is used to chat spam protection.
	LONG		lUserInfoInstances[MAX_USERINFOINSTANCE_STORAGE];
	ULONG		ulLastUserInfoInstance;

	// Record the last time this player changed teams, so we can potentially forbid him from
	// doing it again.
	ULONG		ulLastChangeTeamTime;

	// Last tick the client requested missing packets.
	LONG		lLastPacketLossTick;

	// Last tick we received a movement command.
	LONG		lLastMoveTick;

	LONG		lOverMovementLevel;

	// When the client authenticates his level, should enter scripts be run as well?
	bool		bRunEnterScripts;

} CLIENT_t;

//*****************************************************************************
typedef struct
{
	// Address of the person who queried us.
	netadr_t	Address;

	// Gametic when we allow another query.
	LONG		lNextAllowedGametic;

} STORED_QUERY_IP_t;

//*****************************************************************************
//	PROTOTYPES

void	SERVER_Construct( void );
void	SERVER_Tick( void );

void	SERVER_SendOutPackets( void );
LONG	SERVER_FindFreeClientSlot( void );
LONG	SERVER_FindClientByAddress( netadr_t Address );
ULONG	SERVER_CalcNumPlayers( void );
ULONG	SERVER_CalcNumNonSpectatingPlayers( ULONG ulExcludePlayer );
void	SERVER_CheckTimeouts( void );
void	SERVER_GetPackets( void );
void	SERVER_SendChatMessage( ULONG ulPlayer, ULONG ulMode, char *pszString );
void	SERVER_DetermineConnectionType( void );
void	SERVER_SetupNewConnection( bool bNewPlayer );
void	SERVER_AuthenticateClientLevel( void );
bool	SERVER_PerformAuthenticationChecksum( void );
void	SERVER_ConnectNewPlayer( bool bNewPlayer );
bool	SERVER_GetUserInfo( bool bAllowKick );
void	SERVER_ConnectionError( netadr_t Address, char *pszMessage );
void	SERVER_ClientError( ULONG ulClient, ULONG ulErrorCode );
void	SERVER_SendFullUpdate( ULONG ulClient );
void	SERVER_WriteCommands( void );
bool	SERVER_IsValidClient( ULONG ulClient );
bool	SERVER_IsValidPlayer( ULONG ulPlayer );
void	SERVER_DisconnectClient( ULONG ulClient, bool bBroadcast, bool bSaveInfo );
void	SERVER_SendHeartBeat( void );
void	SERVER_UpdateThings( void );
void	STACK_ARGS SERVER_Printf( ULONG ulPrintLevel, const char *pszString, ... );
void	STACK_ARGS SERVER_PrintfPlayer( ULONG ulPrintLevel, ULONG ulPlayer, const char *pszString, ... );
void	SERVER_UpdateSectors( ULONG ulClient );
void	SERVER_UpdateLines( ULONG ulClient );
void	SERVER_ReconnectNewLevel( char *pszMapName );
void	SERVER_LoadNewLevel( char *pszMapName );
void	SERVER_KickPlayer( ULONG ulPlayer, char *pszReason );
void	SERVER_KickPlayerFromGame( ULONG ulPlayer, char *pszReason );
void	SERVER_AddCommand( char *pszCommand );
void	SERVER_DeleteCommand( void );
bool	SERVER_IsEveryoneReadyToGoOn( void );
bool	SERVER_IsPlayerVisible( ULONG ulPlayer, ULONG ulPlayer2 );
bool	SERVER_IsPlayerAllowedToKnowHealth( ULONG ulPlayer, ULONG ulPlayer2 );
char	*SERVER_GetCurrentFont( void );
void	SERVER_SetCurrentFont( char *pszFont );
char	*SERVER_GetScriptActiveFont( void );
void	SERVER_SetScriptActiveFont( const char *pszFont );
LONG	SERVER_AdjustDoorDirection( LONG lDirection );
LONG	SERVER_AdjustFloorDirection( LONG lDirection );
LONG	SERVER_AdjustCeilingDirection( LONG lDirection );
LONG	SERVER_AdjustElevatorDirection( LONG lDirection );
ULONG	SERVER_GetMaxPacketSize( void );
char	*SERVER_GetMapMusic( void );
void	SERVER_SetMapMusic( const char *pszMusic );
void	SERVER_ResetInventory( ULONG ulClient );
void	SERVER_ParseCommands( void );

// From sv_master.cpp
void	SERVER_MASTER_Construct( void );
void	SERVER_MASTER_Tick( void );
void	SERVER_MASTER_Broadcast( void );
void	SERVER_MASTER_SendServerInfo( netadr_t Address, ULONG ulFlags, ULONG ulTime );
char	*SERVER_MASTER_GetGameName( void );

// Statistic functions.
LONG	SERVER_STATISTIC_GetTotalSecondsElapsed( void );
LONG	SERVER_STATISTIC_GetTotalPlayers( void );
LONG	SERVER_STATISTIC_GetMaxNumPlayers( void );
LONG	SERVER_STATISTIC_GetTotalFrags( void );
void	SERVER_STATISTIC_AddToTotalFrags( void );
LONG	SERVER_STATISTIC_GetTotalOutboundDataTransferred( void );
LONG	SERVER_STATISTIC_GetPeakOutboundDataTransfer( void );
void	SERVER_STATISTIC_AddToOutboundDataTransfer( ULONG ulNumBytes );
LONG	SERVER_STATISTIC_GetCurrentOutboundDataTransfer( void );
LONG	SERVER_STATISTIC_GetTotalInboundDataTransferred( void );
LONG	SERVER_STATISTIC_GetPeakInboundDataTransfer( void );
void	SERVER_STATISTIC_AddToInboundDataTransfer( ULONG ulNumBytes );
LONG	SERVER_STATISTIC_GetCurrentInboundDataTransfer( void );

//*****************************************************************************
//	EXTERNAL VARIABLES THAT NEED TO GO AWAY

extern	CLIENT_t	clients[MAXPLAYERS];
extern short parse_cl;

//*****************************************************************************
//	EXTERNAL CONSOLE VARIABLES

EXTERN_CVAR( String, sv_motd );
EXTERN_CVAR( Bool, sv_defaultdmflags );
EXTERN_CVAR( Bool, sv_forcepassword );
EXTERN_CVAR( Bool, sv_forcejoinpassword );
EXTERN_CVAR( Bool, sv_showlauncherqueries );
EXTERN_CVAR( Int, sv_maxclients );
EXTERN_CVAR( Int, sv_maxplayers );
EXTERN_CVAR( String, sv_password );
EXTERN_CVAR( String, sv_joinpassword );
EXTERN_CVAR( String, sv_rconpassword );
EXTERN_CVAR( Int, sv_maxpacketsize );
EXTERN_CVAR( Int, sv_connectiontype );
EXTERN_CVAR( Bool, sv_timestamp );
EXTERN_CVAR( Int, sv_timestampformat );
EXTERN_CVAR( Int, sv_colorstripmethod );
EXTERN_CVAR( Bool, sv_disallowbots );
EXTERN_CVAR( Bool, sv_minimizetosystray )
EXTERN_CVAR( Int, sv_queryignoretime )

// From sv_master.cpp
EXTERN_CVAR( Bool, sv_updatemaster );
EXTERN_CVAR( Bool, sv_broadcast );
EXTERN_CVAR( String, sv_hostname );
EXTERN_CVAR( String, sv_website );
EXTERN_CVAR( String, sv_hostemail );
EXTERN_CVAR( String, sv_masterip );
EXTERN_CVAR( String, sv_masteroverrideip );

#endif	// __SV_MAIN_H__
