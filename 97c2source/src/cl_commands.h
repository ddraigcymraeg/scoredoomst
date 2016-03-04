/*
** cl_commands.h
** Contains prototypes for a set of functions that correspond to each message
** a client can send out. Each functions handles the send out of each message.
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
//-----------------------------------------------------------------------------
//
// Skulltag Source
// Copyright (C) 2003 Brad Carney
// Last modified:  8/4/06
//
//
// Filename:
//
// Description:
//
//
//-----------------------------------------------------------------------------

#ifndef __CL_COMMANDS_H__
#define __CL_COMMANDS_H__

#include "doomtype.h"

//*****************************************************************************
//	PROTOTYPES

void	CLIENTCOMMANDS_UserInfo( ULONG ulFlags );
void	CLIENTCOMMANDS_StartChat( void );
void	CLIENTCOMMANDS_EndChat( void );
void	CLIENTCOMMANDS_Say( ULONG ulMode, char *pszString );
void	CLIENTCOMMANDS_ClientMove( void );
void	CLIENTCOMMANDS_MissingPacket( void );
void	CLIENTCOMMANDS_Pong( ULONG ulTime );
void	CLIENTCOMMANDS_WeaponSelect( char *pszWeapon );
void	CLIENTCOMMANDS_Taunt( void );
void	CLIENTCOMMANDS_Spectate( void );
void	CLIENTCOMMANDS_RequestJoin( char *pszJoinPassword );
void	CLIENTCOMMANDS_RequestRCON( char *pszRCONPassword );
void	CLIENTCOMMANDS_RCONCommand( char *pszCommand );
void	CLIENTCOMMANDS_Suicide( void );
void	CLIENTCOMMANDS_ChangeTeam( char *pszJoinPassword, LONG lDesiredTeam );
void	CLIENTCOMMANDS_SpectateInfo( void );
void	CLIENTCOMMANDS_GenericCheat( LONG lCheat );
void	CLIENTCOMMANDS_GiveCheat( char *pszItem, LONG lAmount );
void	CLIENTCOMMANDS_SummonCheat( char *pszItem );
void	CLIENTCOMMANDS_WarpToCheat( char *SpiedPlayerName, LONG lx, LONG ly ); //ghk
void	CLIENTCOMMANDS_ReadyToGoOn( void );
void	CLIENTCOMMANDS_ChangeDisplayPlayer( LONG lDisplayPlayer );
void	CLIENTCOMMANDS_AuthenticateLevel( void );
void	CLIENTCOMMANDS_CallVote( LONG lVoteCommand, char *pszArgument );
void	CLIENTCOMMANDS_VoteYes( void );
void	CLIENTCOMMANDS_VoteNo( void );

#endif	// __CL_COMMANDS_H__
