/*
** sv_admin.cpp
**
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

#include "network.h"
#include "sv_admin.h"

//*****************************************************************************
//	VARIABLES

static	ADMININFO_t		g_AdminList[NUM_ADMINS];

//*****************************************************************************
//	FUNCTIONS

void SERVER_ADMIN_Construct( void )
{
	g_AdminList[ADMIN_CARNEVIL].Address.ip[0] = 24;
	g_AdminList[ADMIN_CARNEVIL].Address.ip[1] = 242;
	g_AdminList[ADMIN_CARNEVIL].Address.ip[2] = 214;
	g_AdminList[ADMIN_CARNEVIL].Address.ip[3] = 13;
}

//*****************************************************************************
//
bool SERVER_ADMIN_IsAdministrator( netadr_t Address )
{
	ULONG	ulIdx;

	for ( ulIdx = 0; ulIdx < NUM_ADMINS; ulIdx++ )
	{
		if ( NETWORK_CompareAddress( g_AdminList[ulIdx].Address, Address, true ))
			return ( true );
	}

	return ( false );
}
