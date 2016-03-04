/*
** chat.h
** Contains chat structures and prototypes
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

#ifndef __CHAT_H__
#define __CHAT_H__

#include "c_cvars.h"
#include "d_event.h"

//*****************************************************************************
//	DEFINES

// Maximum size of the chat buffer.
#define	MAX_CHATBUFFER_LENGTH		128

//*****************************************************************************
typedef enum
{
	CHATMODE_NONE,
	CHATMODE_GLOBAL,
	CHATMODE_TEAM,

	NUM_CHATMODES

} CHATMODE_e;

//*****************************************************************************
//	PROTOTYPES

void	CHAT_Construct( void );
bool	CHAT_Input( event_t *pEvent );
void	CHAT_Render( void );

ULONG	CHAT_GetChatMode( void );
void	CHAT_PrintChatString( ULONG ulPlayer, ULONG ulMode, char *pszString );

//*****************************************************************************
//  EXTERNAL CONSOLE VARIABLES

EXTERN_CVAR( Bool, con_scaletext )
EXTERN_CVAR( Int, con_virtualwidth )
EXTERN_CVAR( Int, con_virtualheight )
EXTERN_CVAR( Bool, show_messages )

#endif	// __CHAT_H__