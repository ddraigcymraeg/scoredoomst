/*
** botpath.h
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

#ifndef __BOTPATH_H__
#define __BOTPATH_H__

#include "actor.h"
#include "r_defs.h"

//*****************************************************************************
//	DEFINES

#define	BOTPATH_OBSTRUCTED			1
#define	BOTPATH_STAIRS				2
#define	BOTPATH_JUMPABLELEDGE		4
#define	BOTPATH_DAMAGINGSECTOR		8
#define	BOTPATH_DROPOFF				16
#define	BOTPATH_TELEPORT			32
#define	BOTPATH_DOOR				64

//*****************************************************************************
//	FUNCTIONS

bool		BOTPATH_IsPositionBlocked( AActor *pActor, fixed_t DestX, fixed_t DestY );
ULONG		BOTPATH_TryWalk( AActor *pActor, fixed_t StartX, fixed_t StartY, fixed_t StartZ, fixed_t DestX, fixed_t DestY );
void		BOTPATH_LineOpening( line_t *pLine, fixed_t X, fixed_t Y, fixed_t RefX, fixed_t RefY );
sector_t	*BOTPATH_GetDoorSector( void );

#endif	// __BOTPATH_H__