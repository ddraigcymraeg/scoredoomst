/*
** cl_commands.cpp
** Contains a set of functions that correspond to each message a client
** can send out. Each functions handles the send out of each message.
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

#include "cl_commands.h"
#include "cl_main.h"
#include "gi.h"
#include "network.h"
#include "r_state.h"

//*****************************************************************************
//	FUNCTIONS

static	ULONG	g_ulLastChangeTeamTime = 0;

//*****************************************************************************
//	FUNCTIONS

void CLIENTCOMMANDS_UserInfo( ULONG ulFlags )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_USERINFO );

	// Tell the server which items are being updated.
	NETWORK_WriteShort( CLIENT_GetLocalBuffer( ), ulFlags );

	if ( ulFlags & USERINFO_NAME )
		NETWORK_WriteString( CLIENT_GetLocalBuffer( ), players[consoleplayer].userinfo.netname );
	if ( ulFlags & USERINFO_GENDER )
		NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), players[consoleplayer].userinfo.gender );
	if ( ulFlags & USERINFO_COLOR )
		NETWORK_WriteLong( CLIENT_GetLocalBuffer( ), players[consoleplayer].userinfo.color );
	if ( ulFlags & USERINFO_AIMDISTANCE )
		NETWORK_WriteLong( CLIENT_GetLocalBuffer( ), players[consoleplayer].userinfo.aimdist );
	if ( ulFlags & USERINFO_SKIN )
		NETWORK_WriteString( CLIENT_GetLocalBuffer( ), skins[players[consoleplayer].userinfo.skin].name );
	if ( ulFlags & USERINFO_RAILCOLOR )
		NETWORK_WriteLong( CLIENT_GetLocalBuffer( ), players[consoleplayer].userinfo.lRailgunTrailColor );
	if ( ulFlags & USERINFO_HANDICAP )
		NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), players[consoleplayer].userinfo.lHandicap );
	if ( ulFlags & USERINFO_CONNECTIONTYPE )
		NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), players[consoleplayer].userinfo.lConnectionType );
	if (( gameinfo.gametype == GAME_Hexen ) && ( ulFlags & USERINFO_PLAYERCLASS ))
		NETWORK_WriteString( CLIENT_GetLocalBuffer( ), (char *)PlayerClasses[players[consoleplayer].userinfo.PlayerClass].Type->TypeName.GetChars( ));
}

//*****************************************************************************
//
void CLIENTCOMMANDS_StartChat( void )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_STARTCHAT );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_EndChat( void )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_ENDCHAT );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_Say( ULONG ulMode, char *pszString )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_SAY );
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), ulMode );
	NETWORK_WriteString( CLIENT_GetLocalBuffer( ), (char *)pszString );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_ClientMove( void )
{
}

//*****************************************************************************
//
void CLIENTCOMMANDS_MissingPacket( void )
{
}

//*****************************************************************************
//
void CLIENTCOMMANDS_Pong( ULONG ulTime )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_PONG );
	NETWORK_WriteLong( CLIENT_GetLocalBuffer( ), ulTime );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_WeaponSelect( char *pszWeapon )
{

	//char *pszWeapon2;//ghk
	// Some optimization. For standard Doom weapons, to reduce the size of the string
	// that's sent out, just send some key character that identifies the weapon, instead
	// of the full name.

	if ( stricmp( pszWeapon, "Fist" ) == 0 )
		pszWeapon = "1";
	else if ( stricmp( pszWeapon, "Pistol" ) == 0 )
		pszWeapon = "2";
	else if ( stricmp( pszWeapon, "Shotgun" ) == 0 )
		pszWeapon = "3";
	else if ( stricmp( pszWeapon, "SuperShotgun" ) == 0 )
		pszWeapon = "4";
	else if ( stricmp( pszWeapon, "RocketLauncher" ) == 0 )
		pszWeapon = "5";
	else if ( stricmp( pszWeapon, "GrenadeLauncher" ) == 0 )
		pszWeapon = "6";
	else if ( stricmp( pszWeapon, "PlasmaRifle" ) == 0 )
		pszWeapon = "7";
	else if ( stricmp( pszWeapon, "Railgun" ) == 0 )
		pszWeapon = "8";
	else if ( stricmp( pszWeapon, "BFG9000" ) == 0 )
		pszWeapon = "9";
	else if ( stricmp( pszWeapon, "BFG10K" ) == 0 )
		pszWeapon = "0";

	//convertWeaponNameToKeyLetter( pszWeapon );
	//memcpy(pszWeapon2,pszWeapon,2); //ghk
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_WEAPONSELECT );
	NETWORK_WriteString( CLIENT_GetLocalBuffer( ), pszWeapon ); //ghk
}

//*****************************************************************************
//
void CLIENTCOMMANDS_Taunt( void )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_TAUNT );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_Spectate( void )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_SPECTATE );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_RequestJoin( char *pszJoinPassword )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_REQUESTJOIN );
	NETWORK_WriteString( CLIENT_GetLocalBuffer( ), pszJoinPassword );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_RequestRCON( char *pszRCONPassword )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_REQUESTRCON );
	NETWORK_WriteString( CLIENT_GetLocalBuffer( ), pszRCONPassword );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_RCONCommand( char *pszCommand )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_RCONCOMMAND );
	NETWORK_WriteString( CLIENT_GetLocalBuffer( ), pszCommand );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_Suicide( void )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_SUICIDE );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_ChangeTeam( char *pszJoinPassword, LONG lDesiredTeam )
{
	if (( g_ulLastChangeTeamTime > 0 ) && ( (ULONG)gametic < ( g_ulLastChangeTeamTime + ( TICRATE * 10 ))))
	{
		Printf( "You must wait at least 10 seconds before changing teams again.\n" );
		return;
	}

	g_ulLastChangeTeamTime = gametic;
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_CHANGETEAM );
	NETWORK_WriteString( CLIENT_GetLocalBuffer( ), pszJoinPassword );
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), lDesiredTeam );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_SpectateInfo( void )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_SPECTATEINFO );
	NETWORK_WriteLong( CLIENT_GetLocalBuffer( ), gametic );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_GenericCheat( LONG lCheat )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_GENERICCHEAT );
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), lCheat );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_GiveCheat( char *pszItem, LONG lAmount )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_GIVECHEAT );
	NETWORK_WriteString( CLIENT_GetLocalBuffer( ), pszItem );
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), lAmount );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_SummonCheat( char *pszItem )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_SUMMONCHEAT );
	NETWORK_WriteString( CLIENT_GetLocalBuffer( ), pszItem );
}

//*****************************************************************************
// GHK
void CLIENTCOMMANDS_WarpToCheat( char *SpiedPlayerName, LONG lx, LONG ly)
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_WARPTOCHEAT );
	NETWORK_WriteString( CLIENT_GetLocalBuffer( ), SpiedPlayerName );
	NETWORK_WriteLong( CLIENT_GetLocalBuffer( ), lx );
	NETWORK_WriteLong( CLIENT_GetLocalBuffer( ), ly );

}

//*****************************************************************************
//
void CLIENTCOMMANDS_ReadyToGoOn( void )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_READYTOGOON );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_ChangeDisplayPlayer( LONG lDisplayPlayer )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_CHANGEDISPLAYPLAYER );
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), lDisplayPlayer );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_AuthenticateLevel( void )
{
}

//*****************************************************************************
//
void CLIENTCOMMANDS_CallVote( LONG lVoteCommand, char *pszArgument )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_CALLVOTE );
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), lVoteCommand );
	NETWORK_WriteString( CLIENT_GetLocalBuffer( ), pszArgument );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_VoteYes( void )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_VOTEYES );
}

//*****************************************************************************
//
void CLIENTCOMMANDS_VoteNo( void )
{
	NETWORK_WriteByte( CLIENT_GetLocalBuffer( ), CLC_VOTENO );
}

