/*
** bbannouncer.cpp
** The announcer from Blood (The Voice).
**
**---------------------------------------------------------------------------
** Copyright 1998-2006 Randy Heit
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
** It's been so long since I played a bloodbath, I don't know when all
** these sounds are used, so much of this usage is me guessing. Some of
** it has also obviously been reused for events that were never present
** in bloodbaths.
**
** I should really have a base Announcer class and derive the Bloodbath
** announcer off of that. That way, multiple announcer styles could be
** supported easily.
*/

// HEADER FILES ------------------------------------------------------------
#include <ctype.h> //GHK ADDED
#include "doomstat.h" //ghk
#include "actor.h"
#include "gstrings.h"
#include "s_sound.h"
#include "m_random.h"
#include "d_player.h"
#include "network.h" //GHK
#include "announcer.h" //GHK moved lastannouncetime into announcer.h

//ghk
CVAR (Float, sd_bbvolume, 1.0,CVAR_ARCHIVE) 

// MACROS ------------------------------------------------------------------

// TYPES -------------------------------------------------------------------

struct SoundAndString
{
	const char *Message;
	const char *Sound;
};

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

void SexMessage (const char *from, char *to, int gender,
	const char *victim, const char *killer);

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

CVAR (Bool, cl_bbannounce, true, CVAR_ARCHIVE)
CVAR (Bool, cl_bbannounce_start, false, CVAR_ARCHIVE)

// PRIVATE DATA DEFINITIONS ------------------------------------------------

static FRandom pr_gksannouncechance("bbstartannounce"); //ghk

static const char *BeginSounds[] =
{
	"VO1.SFX",		// Let the bloodbath begin
	"VO2.SFX",		// The festival of blood continues
	"VO3.SFX",		//You will know what to do when the time comes
};

//GHK
static const char *BeginSoundsSDS[] =
{
	"Let the bloodbath begin",		
	"The festival of blood continues",		
	"You will know what to do when the time comes",	
};


static const SoundAndString WorldKillSounds[] =
{
	{ "BBA_EXCREMENT",	"VO7.SFX" },		// Excrement
	{ "BBA_HAMBURGER",	"VO8.SFX" },		// Hamburger
	{ "BBA_SCROTUM",	"VO9.SFX" },		// Scrotum separation
};

static const SoundAndString SuicideSounds[] =
{
	{ "BBA_SUICIDE",	"VO13.SFX" },		// Unassisted death
	{ "BBA_SUICIDE",	"VO5.SFX" },		// Kevorkian approves
	{ "BBA_POPULATION",	"VO12.SFX" },		// Population control
	{ "BBA_DARWIN",		"VO16.SFX" }		// Darwin award
};

static const SoundAndString KillSounds[] =
{
	{ "BBA_BONED",		"BONED.SFX" },		// Boned
	{ "BBA_CREAMED",	"CREAMED.SFX" },	// Creamed
	{ "BBA_DECIMAT",	"DECIMAT.SFX" },	// Decimated
	{ "BBA_DESTRO",		"DESTRO.SFX" },		// Destroyed
	{ "BBA_DICED",		"DICED.SFX" },		// Diced
	{ "BBA_DISEMBO",	"DISEMBO.SFX" },	// Disembowled
	{ "BBA_FLATTE",		"FLATTE.SFX" },		// Flattened
	{ "BBA_JUSTICE",	"JUSTICE.SFX" },	// Justice
	{ "BBA_MADNESS",	"MADNESS.SFX" },	// Madness
	{ "BBA_KILLED",		"KILLED.SFX" },		// Killed
	{ "BBA_MINCMEAT",	"MINCMEAT.SFX" },	// Mincemeat
	{ "BBA_MASSACR",	"MASSACR.SFX" },	// Massacred
	{ "BBA_MUTILA",		"MUTILA.SFX" },		// Mutilated
	{ "BBA_REAMED",		"REAMED.SFX" },		// Reamed
	{ "BBA_RIPPED",		"RIPPED.SFX" },		// Ripped
	{ "BBA_SLAUGHT",	"SLAUGHT.SFX" },	// Slaughtered
	{ "BBA_SMASHED",	"SMASHED.SFX" },	// Smashed
	{ "BBA_SODOMIZ",	"SODOMIZ.SFX" },	// Sodomized
	{ "BBA_SPLATT",		"SPLATT.SFX" },		// Splattered
	{ "BBA_SQUASH",		"SQUASH.SFX" },		// Squashed
	{ "BBA_THROTTL",	"THROTTL.SFX" },	// Throttled
	{ "BBA_WASTED",		"WASTED.SFX" },		// Wasted
	{ "BBA_BODYBAG",	"VO10.SFX" },		// Body bagged
	{ "BBA_HOSED",		"VO25.SFX" },		// Hosed
	{ "BBA_TOAST",		"VO27.SFX" },		// Toasted
	{ "BBA_HELL",		"VO28.SFX" },		// Sent to hell
	{ "BBA_SPRAYED",	"VO35.SFX" },		// Sprayed
	{ "BBA_DOGMEAT",	"VO36.SFX" },		// Dog meat
	{ "BBA_BEATEN",		"VO39.SFX" },		// Beaten like a cur
	{ "BBA_SNUFF",		"VO41.SFX" },		// Snuffed
	{ "BBA_CASTRA",		"CASTRA.SFX" },		// Castrated
};

static const char *GoodJobSounds[] =
{
	"VO22.SFX",		// Fine work
	"VO23.SFX",		// Well done
	"VO44.SFX",		// Excellent
};

static const char *TooBadSounds[] =
{
	"VO17.SFX",		// Go play Mario
	"VO18.SFX",		// Need a tricycle?
	"VO37.SFX",		// Bye bye now
};

static const char *TelefragSounds[] =
{
	"VO29.SFX",		// Pass the jelly
	"VO34.SFX",		// Spillage
	"VO40.SFX",		// Whipped and creamed
	"VO42.SFX",		// Spleen vented
	"VO43.SFX",		// Vaporized
	"VO38.SFX",		// Ripped him loose
	"VO14.SFX",		// Shat upon
};
//GHK
static const char *TelefragSoundsSDS[] =
{
	"Pass the jelly",		// Pass the jelly
	"Spillage",		// Spillage
	"Whipped and creamed",		// Whipped and creamed
	"Spleen vented",		// Spleen vented
	"Vaporized",		// Vaporized
	"Ripped him loose",		// Ripped him loose
	"Shat upon",		// Shat upon
};

//GHK SD Variants are for Score Doom
static const char *GoodJobSoundsSD[] =
{
	"VO22.SFX",		// Fine work
	"VO23.SFX",		// Well done
	"VO44.SFX",		// Excellent
	"VO20.SFX",		// Good one
	"VO33.SFX",		// He shoots... He scores
	"VO19.SFX",		// Talented

};

static const char *GoodJobSoundsSDIntermiss[] =
{
	"VO22.SFX",		// Fine work
	"VO23.SFX",		// Well done
	"VO44.SFX",		// Excellent
	"VO20.SFX",		// Good one
	"VO33.SFX",		// He shoots... He scores
	"VO19.SFX",		// Talented
	"VO30.SFX",		//Punishment Delivered
	"VO38.SFX",		//Ripped Em Loose

};

static const char *GoodJobSoundsSDSIntermiss[] =
{
	"Fine work!",		// Fine work
	"Well done!",		// Well done
	"Excellent!",		// Excellent
	"Good one!",		// Good one
	"He shoots... He scores!",		// He shoots... He scores
	"Talented!",		// Talented
	"Punishment delivered!",
	"Ripped em loose!",
};

static const char *GoodJobSoundsSDS[] =
{
	"Fine work!",		// Fine work
	"Well done!",		// Well done
	"Excellent!",		// Excellent
	"Good one!",		// Good one
	"He shoots... He scores!",		// He shoots... He scores
	"Talented!",		// Talented

};

static const char *TooBadSoundsSDIntermiss[] =
{
	"VO17.SFX",		// Go play Mario
	"VO18.SFX",		// Need a tricycle?
	"VO37.SFX",		// Bye bye now
	"VO40.SFX",		// Whipped and creamed
	"VO31.SFX",		// Bobbit-ized
	"VO26.SFX",		// Humiliated
	"VO39.SFX",	//Beaten like a cur 

};

static const char *TooBadSoundsSDSIntermiss[] =
{
	"Go play Mario",		// Go play Mario
	"Need a tricycle?",		// Need a tricycle?
	"Bye bye now",		// Bye bye now
	"Whipped and creamed",		// Whipped and creamed
	"Bobbit-ized",		// Bobbit-ized
	"Humiliated",		// Humiliated
	"Beaten like a cur"

};

//GHK SD Variants are for Score Doom
static const char *TooBadSoundsSD[] =
{
	"VO17.SFX",		// Go play Mario
	"VO18.SFX",		// Need a tricycle?
	"VO37.SFX",		// Bye bye now
	"VO31.SFX",		// Bobbit-ized
	//"VO6.SFX",		// Asshole
	"VO26.SFX",		// Humiliated
	"VO32.SFX",		// Stiffed
	"VO21.SFX",		// Lunch meat
	"VO29.SFX",		// Pass the jelly
	"VO34.SFX",		// Spillage
	"VO40.SFX",		// Whipped and creamed
	"VO42.SFX",		// Spleen vented
	"VO7.SFX", //excrement
	"VO8.SFX", //hamburger
	"VO9.SFX", //scrotum separation
	"CASTRA.SFX", //castrated
	"VO6.SFX", //asshole
	"SLICED.SFX", //sliced
	"VO14.SFX",		// Shat upon

};

static const char *TooBadSoundsSDS[] =
{
	"Go play Mario",		// Go play Mario
	"Need a tricycle?",		// Need a tricycle?
	"Bye bye now",		// Bye bye now
	"Bobbit-ized",		// Bobbit-ized
	//"Asshole!",		// Asshole
	"Humiliated",		// Humiliated
	"Stiffed",		// Stiffed
	"Lunch meat",		// Lunch meat
	"Pass the jelly",		// Pass the jelly
	"Spillage",		// Spillage
	"Whipped and creamed",		// Whipped and creamed
	"Spleen vented",		// Spleen vented
	"Excrement",
	"Hamburger",
	"Scrotum separation",
	"Castrated",
	"Asshole",
	"Sliced",
	"Shat upon",		// Shat upon
};

#if 0	// Sounds I don't know what to do with
	"VO6.SFX",		// Asshole
	"VO15.SFX",		// Finish him
	"VO19.SFX",		// Talented
	"VO20.SFX",		// Good one
	"VO21.SFX",		// Lunch meat
	"VO26.SFX",		// Humiliated
	"VO30.SFX",		// Punishment delivered
	"VO31.SFX",		// Bobbit-ized
	"VO32.SFX",		// Stiffed
	"VO33.SFX",		// He shoots... He scores
#endif

// static int LastAnnounceTime;
static FRandom pr_bbannounce ("BBAnnounce");

// CODE --------------------------------------------------------------------

//==========================================================================
//
// DoVoiceAnnounce
//
//==========================================================================

void DoVoiceAnnounce (const char *sound)
{
	// Don't play announcements too close together
	if (LastAnnounceTime == 0 || LastAnnounceTime <= level.time-5)
	{
		LastAnnounceTime = level.time;
		S_Sound (CHAN_VOICE, sound, sd_bbvolume, ATTN_SURROUND);
	}
}

//ghk for client-server SD sounds
void DoSDSound (ULONG ulPlayer, ULONG ulState)
{
	// Don't play announcements too close together
	//if (LastAnnounceTime == 0 || LastAnnounceTime <= level.time-5)
	//{

		//LastAnnounceTime = level.time;
	switch(ulState){
			case 1: //secret found!
				S_Sound (CHAN_AUTO, "misc/secret", 1, ATTN_NORM);
				break;
			case 2: //keyscore
				S_Sound (CHAN_AUTO, "ghk/keyscr", 1, ATTN_NORM);
				break;

		}
		//S_Sound (CHAN_VOICE, sound, 1, ATTN_NONE);
	//}
}

//GHK
void DoVoiceAnnounceSD (const char *sound)
{
	// Don't play announcements too close together
	//if (LastAnnounceTime == 0 || LastAnnounceTime <= level.time-5)
	//{
		LastAnnounceTime = level.time;
		S_Sound (CHAN_VOICE, sound, 1, ATTN_NONE);
	//}
}
//GHK
void DoVoiceAnnounceIntermission (bool win)
{
	int rannum = pr_bbannounce();
	if (cl_bbannounce){
		if(win){
			DoVoiceAnnounceSD(GoodJobSoundsSDIntermiss[rannum % 8]);
		}else{
			DoVoiceAnnounceSD(TooBadSoundsSDIntermiss[rannum % 7]);
		}
	}
}

//==========================================================================
//
// AnnounceGameStart
//
// Called when a new map is entered.
//
//==========================================================================

bool AnnounceGameStart ()
{
	LastAnnounceTime = 0;
	if (cl_bbannounce)
	{
		DoVoiceAnnounce (BeginSounds[pr_bbannounce() & 1]);
	}
	return false;
}

//GHK
bool AnnounceSDGameStart (const char *levelname)
{
	//if(level.flags & LEVEL_VISITED) //for hubs, where you can re-enter levels
		//return false; //remove this check? yes, since skulltag does not support hubs

	bool firstlevel=false;
	LastAnnounceTime = 0;
	int rannum = pr_bbannounce();

	if(pr_gksannouncechance(11)>6)
		return false;

	if(tolower(levelname[0])=='e'){
		//DOOM check for eXm1 X=1->4
		if(levelname[3]=='1')
			firstlevel=true;
	
	}else if((levelname[3]=='0'&&levelname[4]=='1')||(levelname[3]=='3'&&levelname[4]=='0')){
		//DOOM2+ check for MAP01 & MAP30
		firstlevel=true;
	}

	//also added cl_bbannounce_start in case it is irritating for players
	if (cl_bbannounce&&cl_bbannounce_start&&(firstlevel))
	{
		
		if(!sd_startannounce_didonce){
			DoVoiceAnnounce (BeginSounds[pr_bbannounce() & 1]);
			sd_startannounce_didonce=true;
		}
	
	}else if (cl_bbannounce&&cl_bbannounce_start&&(level.flags & (LEVEL_MAP07SPECIAL|LEVEL_CYBORGSPECIAL|
							LEVEL_SPIDERSPECIAL|LEVEL_BRUISERSPECIAL))){
		
		if(!sd_startannounce_didonce){
			DoVoiceAnnounce (BeginSounds[rannum % 3]);
			sd_startannounce_didonce=true;
		}
	}else{
		sd_startannounce_didonce=false;
	}
	return false;
}

//==========================================================================
//
// AnnounceKill
//
// Called when somebody dies.
//
//==========================================================================

bool AnnounceKill (AActor *killer, AActor *killee)
{
	const char *killerName;
	const SoundAndString *choice;
	const char *message;
	int rannum = pr_bbannounce();

	if (cl_bbannounce) //&& deathmatch)
	{
		bool playSound = killee->CheckLocalView (consoleplayer);

		if (killer == NULL)
		{ // The world killed the player
			if (killee->player->userinfo.gender == GENDER_MALE)
			{ // Only males have scrotums to separate
				choice = &WorldKillSounds[rannum % 3];
			}
			else
			{
				choice = &WorldKillSounds[rannum & 1];
			}
			killerName = NULL;
		}
		else if (killer == killee)
		{ // The player killed self
			choice = &SuicideSounds[rannum & 3];
			killerName = killer->player->userinfo.netname;
		}
		else
		{ // Another player did the killing
			if (killee->player->userinfo.gender == GENDER_MALE)
			{ // Only males can be castrated
				choice = &KillSounds[rannum % countof(KillSounds)];
			}
			else
			{
				choice = &KillSounds[rannum % (countof(KillSounds) - 1)];
			}
			killerName = killer->player->userinfo.netname;

			// Blood only plays the announcement sound on the killer's
			// computer. I think it sounds neater to also hear it on
			// the killee's machine.
			playSound |= killer->CheckLocalView (consoleplayer);
		}

		message = GStrings(choice->Message);
		if (message != NULL)
		{
			char assembled[1024];

			SexMessage (message, assembled, killee->player->userinfo.gender,
				killee->player->userinfo.netname, killerName);
			Printf (PRINT_MEDIUM, "%s\n", assembled);
		}
		if (playSound)
		{
			DoVoiceAnnounce (choice->Sound);
		}
		return message != NULL;
	}
	return false;
}

//GHK announce 'boss' (>=700hp) kill.
bool AnnounceBigKill (ULONG ulPlayer, bool bOnlyGood){
	int rannum = pr_bbannounce();
	const SoundAndString *choice;
	const char* message;
	//bool playSound = killer->player->mo->CheckLocalView (consoleplayer);

	if (!cl_bbannounce)
		return false;

	if(rannum>128&&!bOnlyGood){
		choice = &KillSounds[rannum % (countof(KillSounds)-1)];
//		if (playSound)
	//	{
			char assembled[1024];
			const char *killerName;
			killerName = players[ulPlayer].userinfo.netname;
			DoVoiceAnnounce (choice->Sound);
			message = GStrings(choice->Message);
			//Printf (PRINT_HIGH, "%s\n", message);
			//monster gender =2 other?
			SexMessage (message, assembled, 2,
				"the boss demon", killerName);
			Printf (PRINT_MEDIUM, "%s\n", assembled);
				
		//}

	}else{
		//if (killer->player->mo->CheckLocalView (consoleplayer))
		//{
			//DoVoiceAnnounce (GoodJobSoundsSD[rannum % 7]);
			//Printf (PRINT_HIGH, "%s\n", GoodJobSoundsSDS[rannum % 7]);
			DoVoiceAnnounce (GoodJobSoundsSD[rannum % 6]);
			Printf (PRINT_MEDIUM, "%s\n", GoodJobSoundsSDS[rannum % 6]);
		//}

	}

return false;
}

//GHK For when a player is killed by a monster in Score Doom
bool AnnounceKillSD1 (ULONG ulPlayer, ULONG ulState)
{
	const char *killerName;
	const SoundAndString *choice;
	int rannum = pr_bbannounce();
	const char *message;

	//ok, make sure that player on player kills never get in here
	//in co-op
	//if (killer!=NULL&&((killer != killee)&&killer->player != NULL)){
		//return false;
	//}
	killerName = "A monster";

	if (cl_bbannounce)
	{
		//bool playSound = killee->CheckLocalView (consoleplayer);

		if (ulState==0)
		{ // The world killed the player
			//if (killee->player->userinfo.gender == GENDER_MALE)
			//{ // Only males have scrotums to separate
				choice = &WorldKillSounds[rannum % 3];
			//}
			//else
			//{
			//	choice = &WorldKillSounds[rannum & 1];
			//}
			killerName = NULL;
		}
		else if (ulState==2)
		{ // The player killed self
			choice = &SuicideSounds[rannum % 4];
			killerName = players[ulPlayer].userinfo.netname;
		}
		else if (ulState==1)
		{
			// A non-player (i.e. monster) did the killing
			
			choice = &KillSounds[rannum % countof(KillSounds)];
			
			
			killerName = "A monster"; //killer->player->userinfo.netname;

			// Blood only plays the announcement sound on the killer's
			// computer. I think it sounds neater to also hear it on
			// the killee's machine.
			//playSound |= killer->CheckLocalView (consoleplayer);
			
		}

		if(rannum>=180){
			DoVoiceAnnounce (TooBadSoundsSD[rannum % 18]);
				Printf (PRINT_LOW,"%s\n",TooBadSoundsSDS[rannum % 18]);
		}else{
			DoVoiceAnnounce (choice->Sound);

				message = GStrings(choice->Message);
				if (message != NULL)
				{
					char assembled[1024];

					SexMessage (message, assembled, players[ulPlayer].userinfo.gender,
					players[ulPlayer].userinfo.netname, killerName);
						Printf (PRINT_LOW,"%s\n", assembled);
				}


		}

		
		//return message != NULL;
	}
	return false;
}

//GHK For when a player is telefragged by a monster in Score Doom
//I believe this is redundant, since I dont care if other players telefrag atm
//in ScoreDoom. Actually Monsters cant telefrag players iirc.
bool AnnounceTelefragSD1 (ULONG ulPlayer)
{
	int rannum = pr_bbannounce();

	//ok, make sure that player on player kills never get in here
	//in co-op
	//if (killer!=NULL&&((killer != killee)&&killer->player != NULL)){
		//return false;
	//}

	if (cl_bbannounce)
	{
		const char *message = GStrings("OB_MPTELEFRAG");
		if (message != NULL)
		{
			//char assembled[1024];

			//SexMessage (message, assembled, killee->player->userinfo.gender,
				//killee->player->userinfo.netname, killer->player->userinfo.netname);
			//Printf (PRINT_MEDIUM, "%s\n", assembled);
		}
		//if (killee->CheckLocalView (consoleplayer))
		//{
			if(rannum>128){
				DoVoiceAnnounce (TooBadSoundsSD[rannum % 18]);
				Printf (PRINT_MEDIUM, "%s\n", TooBadSoundsSDS[rannum % 18]);
			}else{
				DoVoiceAnnounce (TelefragSounds[rannum % 7]);
				Printf (PRINT_MEDIUM, "%s\n", TelefragSoundsSDS[rannum % 7]);
			}
		//}
		//return message != NULL;
	}
	return false;
}

//==========================================================================
//
// AnnounceTelefrag
//
// Called when somebody dies by telefragging.
//
//==========================================================================

bool AnnounceTelefrag (AActor *killer, AActor *killee)
{
	int rannum = pr_bbannounce();

	if (cl_bbannounce) // && multiplayer)
	{
		const char *message = GStrings("OB_MPTELEFRAG");
		if (message != NULL)
		{
			char assembled[1024];

			SexMessage (message, assembled, killee->player->userinfo.gender,
				killee->player->userinfo.netname, killer->player->userinfo.netname);
			Printf (PRINT_MEDIUM, "%s\n", assembled);
		}
		if (killee->CheckLocalView (consoleplayer) ||
			killer->CheckLocalView (consoleplayer))
		{
			DoVoiceAnnounce (TelefragSounds[rannum % 7]);
		}
		return message != NULL;
	}
	return false;
}

//==========================================================================
//
// AnnounceSpree
//
// Called when somebody is on a spree.
//
//==========================================================================

bool AnnounceSpree (AActor *who)
{
	return false;
}

//==========================================================================
//
// AnnounceSpreeLoss
//
// Called when somebody on a spree gets killed.
//
//==========================================================================

bool AnnounceSpreeLoss (AActor *who)
{
	if (cl_bbannounce)
	{
		if (who->CheckLocalView (consoleplayer))
		{
			DoVoiceAnnounce (TooBadSounds[M_Random() % 3]);
		}
	}
	return false;
}

//==========================================================================
//
// AnnounceMultikill
//
// Called when somebody is quickly raking in kills.
//
//==========================================================================

bool AnnounceMultikill (AActor *who)
{
	if (cl_bbannounce)
	{
		if (who->CheckLocalView (consoleplayer))
		{
			DoVoiceAnnounce (GoodJobSounds[M_Random() % 3]);
		}
	}
	return false;
}

//Only for the player making the multikill ghk
bool AnnounceMultikillSD (ULONG ulPlayer)
{
	int rannum = pr_bbannounce();
	if (cl_bbannounce)
	{
		//if (( players[consoleplayer].mo != NULL ) &&
		//( players[consoleplayer].camera != NULL ) &&
		//( players[consoleplayer].camera->player != NULL ) &&
		//( players[consoleplayer].camera->player->bSpectating == false ))
		//{
			if(rannum>64){
				//DoVoiceAnnounce (GoodJobSoundsSD[rannum % 7]);
			//DoVoiceAnnounce (GoodJobSounds[rannum % 3]);
			DoVoiceAnnounce (GoodJobSoundsSD[rannum % 6]);
			Printf (PRINT_MEDIUM, "%s\n", GoodJobSoundsSDS[rannum % 6]);
			}else{
				char assembled[1024];
				const char* message;
				const SoundAndString *choice;
				choice = &KillSounds[rannum % (countof(KillSounds) - 1)];
				message = GStrings(choice->Message);
				SexMessage (message, assembled, 2,
				"", "");
				DoVoiceAnnounce (choice->Sound);
				Printf (PRINT_MEDIUM, "%s\n", assembled);

			}
		//}
	}
	return false;
}

