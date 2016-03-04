/*
** sv_ban.h
** Contains variables and routines related to the server ban of the program.
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

#ifndef __SV_BAN_H__
#define __SV_BAN_H__

#include "sv_main.h"

//*****************************************************************************
//	DEFINES

#define	MAX_SERVER_BANS			64
#define	BANFILE_REPARSE_TIME	( TICRATE * 60 * 10 )

//*****************************************************************************
//	STRUCTURES

typedef struct
{
	// The IP address that is banned in char form. Can be a number, or a wildcards.
	char		szBannedIP[4][4];

	// Comment regarding the banned address.
	char		szComment[128];

} BAN_t;

//*****************************************************************************
//	PROTOTYPES

void	SERVERBAN_Tick( void );
bool	SERVERBAN_IsIPBanned( char *pszIP0, char *pszIP1, char *pszIP2, char *pszIP3 );
ULONG	SERVERBAN_DoesBanExist( char *pszIP0, char *pszIP1, char *pszIP2, char *pszIP3 );
void	SERVERBAN_AddBan( char *pszIP0, char *pszIP1, char *pszIP2, char *pszIP3, char *pszPlayerName, char *pszComment );
bool	SERVERBAN_StringToBan( char *pszAddress, char *pszIP0, char *pszIP1, char *pszIP2, char *pszIP3 );
void	SERVERBAN_ClearBans( void );
//LONG	SERVERBAN_GetNumBans( void );
BAN_t	SERVERBAN_GetBan( ULONG ulIdx );

//*****************************************************************************
//	EXTERNAL CONSOLE VARIABLES

EXTERN_CVAR( Bool, sv_enforcebans );
EXTERN_CVAR( String, sv_banfile );

#endif	// __SV_BAN_H__
