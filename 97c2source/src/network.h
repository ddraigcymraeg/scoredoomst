/*
** network.h
** Contains network definitions and functions not specifically
** related to the server or client.
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

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "c_cvars.h"
#include "d_player.h"
#include "i_net.h"
#include "sv_main.h"

//*****************************************************************************
//	DEFINES

enum
{
	// Server is letting master server of its existance.
	SERVER_MASTER_CHALLENGE = 5660020,

	// Server is letting master server of its existance, along with sending an IP the master server
	// should use for this server.
	SERVER_MASTER_CHALLENGE_OVERRIDE,

	// Server is sending some statistics to the master server.
	SERVER_MASTER_STATISTICS,

	// Server is sending its info to the launcher.
	SERVER_LAUNCHER_CHALLENGE,

	// Server is telling a launcher that it's ignoring it.
	SERVER_LAUNCHER_IGNORING,

	// Server is telling a launcher that his IP is banned from the server.
	SERVER_LAUNCHER_BANNED,

	// Client is trying to create a new account with the master server.
	CLIENT_MASTER_NEWACCOUNT,

	// Client is trying to log in with the master server.
	CLIENT_MASTER_LOGIN,

};

// Launcher is querying the server, or master server.
#define	LAUNCHER_SERVER_CHALLENGE	199

#define	DEFAULT_SERVER_PORT		10666
#define	DEFAULT_CLIENT_PORT		10667
#define	DEFAULT_MASTER_PORT		15300
#define	DEFAULT_BROADCAST_PORT	15101

// Connection messages.
#define CONNECT_CHALLENGE		200
#define	CONNECT_READY			201
#define	CONNECT_GETDATA			202
#define	CONNECT_QUIT			203
#define	CONNECT_AUTHENTICATED	204
#define	CONNECT_AUTHENTICATING	205

// Network messages (universal)
#define	NETWORK_ERROR			254

// Movement stuff.
#define CM_X			1
#define CM_Y			2
#define CM_Z			4
#define CM_ANGLE		8
#define CM_MOMX			16
#define CM_MOMY			32
#define CM_MOMZ			64
#define CM_COLORMAP		128
#define	CM_WATERLEVEL	256
#define	CM_NOTVISIBLE	512

// Should we use huffman compression?
#define	USE_HUFFMAN_COMPRESSION

// Extra player update info for spectators.
#define	PLAYER_UPDATE_WEAPON	1
#define	PLAYER_UPDATE_PITCH		2

// Movement flags being sent by the client.
#define	CLIENT_UPDATE_BUTTONS			( 1 << 0 )
#define	CLIENT_UPDATE_FORWARDMOVE		( 1 << 1 )
#define	CLIENT_UPDATE_SIDEMOVE			( 1 << 2 )
#define	CLIENT_UPDATE_UPMOVE			( 1 << 3 )

// Identifying states (the cheap & easy way out)
#define	STATE_SPAWN				1
#define	STATE_SEE				2
#define	STATE_PAIN				3
#define	STATE_MELEE				4
#define	STATE_MISSILE			5
#define	STATE_DEATH				6
#define	STATE_XDEATH			7
#define	STATE_RAISE				8
#define	STATE_HEAL				9

// Identifying player states (again, cheap & easy)
#define	STATE_PLAYER_IDLE		1
#define	STATE_PLAYER_SEE		2
#define	STATE_PLAYER_ATTACK		3
#define	STATE_PLAYER_ATTACK2	4

// HUD message types.
#define	HUDMESSAGETYPE_NORMAL			1
#define	HUDMESSAGETYPE_FADEOUT			2
#define	HUDMESSAGETYPE_TYPEONFADEOUT	3
#define	HUDMESSAGETYPE_FADEINOUT		4

// Different levels of network messages.
#define	NETMSG_LITE		0
#define	NETMSG_MEDIUM	1
#define	NETMSG_HIGH		2

// Which actor flags are being updated?
#define	FLAGSET_FLAGS		1
#define	FLAGSET_FLAGS2		2
#define	FLAGSET_FLAGS3		3
#define	FLAGSET_FLAGS4		4
#define	FLAGSET_FLAGS5		5
#define	FLAGSET_FLAGSST		6

// Which actor sound is being updated?
#define	ACTORSOUND_SEESOUND			1
#define	ACTORSOUND_ATTACKSOUND		2
#define	ACTORSOUND_PAINSOUND		3
#define	ACTORSOUND_DEATHSOUND		4
#define	ACTORSOUND_ACTIVESOUND		5

// Which userinfo categories are being updated?
#define	USERINFO_NAME				1
#define	USERINFO_GENDER				2
#define	USERINFO_COLOR				4
#define	USERINFO_AIMDISTANCE		8
#define	USERINFO_SKIN				16
#define	USERINFO_RAILCOLOR			32
#define	USERINFO_HANDICAP			64
#define	USERINFO_CONNECTIONTYPE		128
#define	USERINFO_PLAYERCLASS		256

#define	USERINFO_ALL				( USERINFO_NAME | USERINFO_GENDER | USERINFO_COLOR | \
									USERINFO_AIMDISTANCE | USERINFO_SKIN | USERINFO_RAILCOLOR | \
									USERINFO_HANDICAP | USERINFO_CONNECTIONTYPE | USERINFO_PLAYERCLASS )

//*****************************************************************************
typedef enum
{
	POLYSOUND_STOPSEQUENCE,
	POLYSOUND_SEQ_DOOR,

	NUM_NETWORK_POLYOBJSOUNDS,

} NETWORK_POLYOBJSOUND_e;

//*****************************************************************************
enum
{
	// Client has the wrong password.
	NETWORK_ERRORCODE_WRONGPASSWORD,

	// Client has the wrong version.
	NETWORK_ERRORCODE_WRONGVERSION,

	// Client is using a version with different network protocol.
	NETWORK_ERRORCODE_WRONGPROTOCOLVERSION,

	// Client has been banned.
	NETWORK_ERRORCODE_BANNED,

	// The server is full.
	NETWORK_ERRORCODE_SERVERISFULL,

	// Client has the wrong version of the current level.
	NETWORK_ERRORCODE_AUTHENTICATIONFAILED,

	// Client failed to send userinfo when connecting.
	NETWORK_ERRORCODE_FAILEDTOSENDUSERINFO,

	// Client has the wrong version of the current ADDONPACK. //GHK
	//NETWORK_ERRORCODE_ADDONPACKFAILED,
	//NETWORK_ERRORCODE_ADDONPACKFAILED2,
	//NETWORK_ERRORCODE_ADDONPACKFAILED3,
	//NETWORK_ERRORCODE_ADDONPACKFAILED4,


	NUM_NETWORK_ERRORCODES
};

//*****************************************************************************
enum
{
	// Program is being run in single player mode.
	NETSTATE_SINGLE,

	// Program is being run in single player mode, emulating a network game (bots, etc).
	NETSTATE_SINGLE_MULTIPLAYER,

	// Program is a client playing a network game.
	NETSTATE_CLIENT,

	// Program is a server, hosting a game.
	NETSTATE_SERVER,

	NUM_NETSTATES
};

//*****************************************************************************
// Note: If the number of enumerated messages goes beyond 255, commands will need
// to be changed to a short. Hopefully that won't have to happen.
typedef enum
{
	SVC_HEADER,							// GENERAL PROTOCOL COMMANDS
	SVC_UNRELIABLEPACKET,
	SVC_RESETSEQUENCE,
	SVC_PING,
	SVC_NOTHING,
	SVC_BEGINSNAPSHOT,
	SVC_ENDSNAPSHOT,
	SVC_SPAWNPLAYER,					// PLAYER COMMANDS
	SVC_MOVEPLAYER,
	SVC_DAMAGEPLAYER,
	SVC_KILLPLAYER,
	SVC_SETPLAYERHEALTH,
	SVC_SETPLAYERSTATE,
	SVC_SETPLAYERUSERINFO,
	SVC_SETPLAYERFRAGS,
	SVC_SETPLAYERPOINTS,
	SVC_SETPLAYERWINS,
	SVC_SETPLAYERKILLCOUNT,
	SVC_SETPLAYERCHATSTATUS,
	SVC_SETPLAYERLAGGINGSTATUS,
	SVC_SETPLAYERREADYTOGOONSTATUS,
	SVC_SETPLAYERTEAM,
	SVC_SETPLAYERCAMERA,
	SVC_UPDATEPLAYERPING,
	SVC_UPDATEPLAYEREXTRADATA,
	SVC_UPDATEPLAYERTIME,
	SVC_MOVELOCALPLAYER,
	SVC_DISCONNECTPLAYER,
	SVC_SETCONSOLEPLAYER,
	SVC_CONSOLEPLAYERKICKED,
	SVC_GIVEPLAYERMEDAL,
	SVC_RESETALLPLAYERSFRAGCOUNT,
	SVC_PLAYERISSPECTATOR,
	SVC_PLAYERSAY,
	SVC_PLAYERTAUNT,
	SVC_PLAYERRESPAWNINVULNERABILITY,
	SVC_SPAWNTHING,						// THING COMMANDS
	SVC_SPAWNTHINGNONETID,
	SVC_SPAWNTHINGEXACT,
	SVC_SPAWNTHINGEXACTNONETID,
	SVC_MOVETHING,
	SVC_DAMAGETHING,
	SVC_KILLTHING,
	SVC_SETTHINGSTATE,
	SVC_DESTROYTHING,
	SVC_SETTHINGANGLE,
	SVC_SETTHINGWATERLEVEL,
	SVC_SETTHINGFLAGS,
	SVC_SETTHINGARGUMENTS,
	SVC_SETTHINGTRANSLATION,
	SVC_SETTHINGPROPERTY,
	SVC_SETTHINGSOUND,
	SVC_SETWEAPONAMMOGIVE,
	SVC_THINGISCORPSE,
	SVC_HIDETHING,
	SVC_TELEPORTTHING,
	SVC_THINGACTIVATE,
	SVC_THINGDEACTIVATE,
	SVC_RESPAWNTHING,
	SVC_PRINT,							// PRINT COMMANDS
	SVC_PRINTMID,
	SVC_PRINTMOTD,
	SVC_PRINTHUDMESSAGE,
	SVC_PRINTHUDMESSAGEFADEOUT,
	SVC_PRINTHUDMESSAGEFADEINOUT,
	SVC_PRINTHUDMESSAGETYPEONFADEOUT,
	SVC_SETGAMEMODE,					// GAME COMMANDS
	SVC_SETGAMESKILL,
	SVC_SETGAMEDMFLAGS,
	SVC_SETGAMEMODELIMITS,
	SVC_SETGAMEENDLEVELDELAY,
	SVC_SETGAMEMODESTATE,
	SVC_SETDUELNUMDUELS,
	SVC_SETLMSSPECTATORSETTINGS,
	SVC_SETLMSALLOWEDWEAPONS,
	SVC_SETINVASIONNUMMONSTERSLEFT,
	SVC_SETINVASIONWAVE,
	SVC_DOPOSSESSIONARTIFACTPICKEDUP,
	SVC_DOPOSSESSIONARTIFACTDROPPED,
	SVC_DOGAMEMODEFIGHT,
	SVC_DOGAMEMODECOUNTDOWN,
	SVC_DOGAMEMODEWINSEQUENCE,
	SVC_SETTEAMFRAGS,					// TEAM COMMANDS
	SVC_SETTEAMSCORE,
	SVC_SETTEAMWINS,
	SVC_SETTEAMRETURNTICKS,
	SVC_TEAMFLAGRETURNED,
	SVC_SPAWNMISSILE,					// MISSILE COMMANDS
	SVC_SPAWNMISSILEEXACT,
	SVC_MISSILEEXPLODE,
	SVC_WEAPONSOUND,					// WEAPON COMMANDS
	SVC_WEAPONCHANGE,
	SVC_WEAPONRAILGUN,
	SVC_SETSECTORFLOORPLANE,			// SECTOR COMMANDS
	SVC_SETSECTORCEILINGPLANE,
	SVC_SETSECTORLIGHTLEVEL,
	SVC_SETSECTORCOLOR,
	SVC_SETSECTORFADE,
	SVC_SETSECTORFLAT,
	SVC_SETSECTORPANNING,
	SVC_SETSECTORROTATION,
	SVC_SETSECTORSCALE,
	SVC_SETSECTORFRICTION,
	SVC_SETSECTORANGLEYOFFSET,
	SVC_SETSECTORGRAVITY,
	SVC_STOPSECTORLIGHTEFFECT,
	SVC_DESTROYALLSECTORMOVERS,
	SVC_DOSECTORLIGHTFIREFLICKER,		// SECTOR LIGHT COMMANDS
	SVC_DOSECTORLIGHTFLICKER,
	SVC_DOSECTORLIGHTLIGHTFLASH,
	SVC_DOSECTORLIGHTSTROBE,
	SVC_DOSECTORLIGHTGLOW,
	SVC_DOSECTORLIGHTGLOW2,
	SVC_DOSECTORLIGHTPHASED,
	SVC_SETLINEALPHA,					// LINE COMMANDS
	SVC_SETLINETEXTURE,
	SVC_SETLINEBLOCKING,
	SVC_SOUND,							// SOUND COMMANDS
	SVC_SOUNDID,
	SVC_SOUNDACTOR,
	SVC_SOUNDIDACTOR,
	SVC_SOUNDPOINT,
	SVC_STARTSECTORSEQUENCE,			// SECTOR SEQUENCE COMMANDS
	SVC_STOPSECTORSEQUENCE,
	SVC_CALLVOTE,						// VOTING COMMANDS
	SVC_PLAYERVOTE,
	SVC_VOTEENDED,
	SVC_MAPLOAD,						// MAP COMMANDS
	SVC_MAPNEW,
	SVC_MAPEXIT,
	SVC_MAPAUTHENTICATE,
	SVC_SETMAPTIME,
	SVC_SETMAPNUMKILLEDMONSTERS,
	SVC_SETMAPNUMFOUNDITEMS,
	SVC_SETMAPNUMFOUNDSECRETS,
	SVC_SETMAPNUMTOTALMONSTERS,
	SVC_SETMAPNUMTOTALITEMS,
	SVC_SETMAPMUSIC,
	SVC_SETMAPSKY,
	SVC_GIVEINVENTORY,					// INVENTORY COMMANDS
	SVC_TAKEINVENTORY,
	SVC_GIVEPOWERUP,
	SVC_DOINVENTORYPICKUP,
	SVC_DESTROYALLINVENTORY,
	SVC_DODOOR,							// DOOR COMMANDS
	SVC_DESTROYDOOR,
	SVC_CHANGEDOORDIRECTION,
	SVC_DOFLOOR,						// FLOOR COMMANDS
	SVC_DESTROYFLOOR,
	SVC_CHANGEFLOORDIRECTION,
	SVC_CHANGEFLOORTYPE,
	SVC_CHANGEFLOORDESTDIST,
	SVC_STARTFLOORSOUND,
	SVC_DOCEILING,						// CEILING COMMANDS
	SVC_DESTROYCEILING,
	SVC_CHANGECEILINGDIRECTION,
	SVC_CHANGECEILINGSPEED,
	SVC_PLAYCEILINGSOUND,
	SVC_DOPLAT,							// PLAT COMMANDS
	SVC_DESTROYPLAT,
	SVC_CHANGEPLATSTATUS,
	SVC_PLAYPLATSOUND,
	SVC_DOELEVATOR,						// ELEVATOR COMMANDS
	SVC_DESTROYELEVATOR,
	SVC_STARTELEVATORSOUND,
	SVC_DOPILLAR,						// PILLAR COMMANDS
	SVC_DESTROYPILLAR,
	SVC_DOWAGGLE,						// WAGGLE COMMANDS
	SVC_DESTROYWAGGLE,
	SVC_UPDATEWAGGLE,
	SVC_DOROTATEPOLY,					// ROTATEPOLY COMMANDS
	SVC_DESTROYROTATEPOLY,
	SVC_DOMOVEPOLY,						// MOVEPOLY COMMANDS
	SVC_DESTROYMOVEPOLY,
	SVC_DOPOLYDOOR,						// POLYDOOR COMMANDS
	SVC_DESTROYPOLYDOOR,
	SVC_SETPOLYDOORSPEEDPOSITION,
	SVC_PLAYPOLYOBJSOUND,				// GENERIC POLYOBJECT COMMANDS
	SVC_SETPOLYOBJPOSITION,
	SVC_SETPOLYOBJROTATION,
	SVC_EARTHQUAKE,						// MISC. COMMANDS
	SVC_SETQUEUEPOSITION,
	SVC_DOSCROLLER,
	SVC_GENERICCHEAT,
	SVC_SETCAMERATOTEXTURE,
	SVC_UPDATEPLAYEREPENDINGWEAPON,
	SVC_SETANNOUNCEBIGKILL, //GHK
	SVC_SETANNOUNCEKILLSD1, //GHK
	SVC_SETANNOUNCEMULTIKILLSD, //GHK
	SVC_SETANNOUNCESDGAMESTART, //GHK
	SVC_ANNOUNCETELEFRAGSD1, //GHK
	SVC_SETSECTORSECRETFOUND, //GHK
	SVC_SETSSDSOUND, //GHK
	SVC_SETSDLIMITHIT, //GHK
	SVC_DOGIVEINVENTORY, //GHK
	SVC_DOTAKEINVENTORY, //GHK
	SVC_SETTHINGSTATEOFFSET, //GHK

	NUM_SERVER_COMMANDS
};

//*****************************************************************************
enum
{
	CLC_USERINFO,
	CLC_STARTCHAT,
	CLC_ENDCHAT,
	CLC_SAY,
	CLC_CLIENTMOVE,
	CLC_MISSINGPACKET,
	CLC_PONG,
	CLC_WEAPONSELECT,
	CLC_TAUNT,
	CLC_SPECTATE,
	CLC_REQUESTJOIN,
	CLC_REQUESTRCON,
	CLC_RCONCOMMAND,
	CLC_SUICIDE,
	CLC_CHANGETEAM,
	CLC_SPECTATEINFO,
	CLC_GENERICCHEAT,
	CLC_GIVECHEAT,
	CLC_SUMMONCHEAT,
	CLC_READYTOGOON,
	CLC_CHANGEDISPLAYPLAYER,
	CLC_AUTHENTICATELEVEL,
	CLC_CALLVOTE,
	CLC_VOTEYES,
	CLC_VOTENO,
	CLC_WARPTOCHEAT, //ghk

	NUM_CLIENT_COMMANDS

};

//*****************************************************************************
enum
{
	MSC_BEGINSERVERLIST,
	MSC_SERVER,
	MSC_ENDSERVERLIST,
	MSC_IPISBANNED,
	MSC_REQUESTIGNORED,
	MSC_AUTHENTICATEUSER,
	MSC_INVALIDUSERNAMEORPASSWORD,
	MSC_ACCOUNTALREADYEXISTS,

};

//*****************************************************************************
//	PROTOTYPES

void		NETWORK_Construct( void );

LONG		NETWORK_GetState( void );
void		NETWORK_SetState( LONG lState );

USHORT		NETWORK_GetLocalPort( void );
void		NETWORK_SetLocalPort( USHORT usPort );

void		NETWORK_Initialize( void );

int			NETWORK_ReadChar( void );
void		NETWORK_WriteChar( sizebuf_t *pBuffer, int Char );

int			NETWORK_ReadByte( void );
void		NETWORK_WriteByte( sizebuf_t *pBuffer, int Byte );

int			NETWORK_ReadShort( void );
void		NETWORK_WriteShort( sizebuf_t *pBuffer, int Short );

int			NETWORK_ReadLong( void );
void		NETWORK_WriteLong( sizebuf_t *pBuffer, int Long );

float		NETWORK_ReadFloat( void );
void		NETWORK_WriteFloat( sizebuf_t *pBuffer, float Float );

char		*NETWORK_ReadString( void );
void		NETWORK_WriteString( sizebuf_t *pBuffer, char *pszString );

// Debugging function.
void		NETWORK_WriteHeader( sizebuf_t *pBuffer, int Byte );

void		NETWORK_CheckBuffer( ULONG ulClient, ULONG ulSize );
int			NETWORK_GetPackets( void );
int			NETWORK_GetLANPackets( void );
//void		NETWORK_LaunchPacket( sizebuf_t netbuf, netadr_t to, bool bCompression );
void		NETWORK_LaunchPacket( sizebuf_t *netbuf, netadr_t to );
void		NETWORK_SendPacket( ULONG ulClient );
void		NETWORK_SendUnreliablePacket( ULONG ulClient );
void		NETWORK_InitBuffer( sizebuf_t *pBuffer, ULONG ulLength );
void		NETWORK_FreeBuffer( sizebuf_t *pBuffer );
void		NETWORK_ClearBuffer( sizebuf_t *pBuffer );
BYTE		*NETWORK_GetSpace( sizebuf_t *pBuffer, ULONG ulLength );
void		NETWORK_Write( sizebuf_t *pBuffer, void *pvData, int nLength );
void		NETWORK_Write( sizebuf_t *pBuffer, BYTE *pbData, int nStartPos, int nLength );
void		NETWORK_Print( sizebuf_t *pBuffer, char *pszData );	// strcats onto the sizebuf
char		*NETWORK_AddressToString( netadr_t Address );
char		*NETWORK_AddressToStringIgnorePort( netadr_t Address );
bool		NETWORK_StringToAddress( char *pszString, netadr_t *pAddress );
bool		NETWORK_CompareAddress( netadr_t a, netadr_t b, bool bIgnorePort );
void		NETWORK_SocketAddressToNetAddress( struct sockaddr_in *s, netadr_t *a );
void		NETWORK_NetAddressToSocketAddress( netadr_t *a, struct sockaddr_in *s );
AActor		*NETWORK_FindThingByNetID( LONG lID );
netadr_t	NETWORK_GetLocalAddress( void );
LONG		NETWORK_GetPacketSize( void );
sizebuf_t	*NETWORK_GetNetworkMessageBuffer( void );
ULONG		NETWORK_ntohs( ULONG ul );

void	I_DoSelect( void );
void	I_SetPort( netadr_t &addr, int port );

void convertWeaponNameToKeyLetter( const char *&pszName );
void convertWeaponKeyLetterToFullString( const char *&pszName );


// DEBUG FUNCTION!
void	NETWORK_FillBufferWithShit( sizebuf_t *pBuffer, ULONG ulSize );

//*****************************************************************************
//	EXTERNAL VARIABLES THAT MUST BE FIXED

extern	netadr_t	g_AddressFrom;


#endif	// __NETWORK_H__