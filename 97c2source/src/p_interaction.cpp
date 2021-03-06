// Emacs style mode select	 -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//		Handling interactions (i.e., collisions).
//
//-----------------------------------------------------------------------------




// Data.
#include "doomdef.h"
#include "gstrings.h"

#include "doomstat.h"

#include "m_random.h"
#include "i_system.h"
#include "announcer.h"

#include "am_map.h"
#include "c_console.h"
#include "c_dispatch.h"

#include "p_local.h"

#include "p_lnspec.h"
#include "p_effect.h"
#include "p_acs.h"

#include "a_doomglobal.h"
#include "a_hereticglobal.h"
#include "ravenshared.h"
#include "a_hexenglobal.h"
#include "a_sharedglobal.h"
#include "a_pickups.h"
#include "gi.h"
#include "templates.h"
#include "sbar.h"
#include "deathmatch.h"
#include "duel.h"
#include "medal.h"
#include "network.h"
#include "sv_commands.h"
#include "g_game.h"
#include "cl_main.h"
#include "joinqueue.h"
#include "lastmanstanding.h"
#include "scoreboard.h"
#include "cooperative.h"
#include "invasion.h"
#include "survival.h"
#include "team.h"
#include "cl_commands.h"

#include "s_sound.h" //GHK
#include "cooperative.h" //GHK


// [BC] Ugh.
void SERVERCONSOLE_UpdatePlayerInfo( LONG lPlayer, ULONG ulUpdateFlags );
void SERVERCONSOLE_UpdateScoreboard( void );

//GHK DOUBLE UGH
//*********


//GHK UGH@@@@*****************************************************************************
// Player list according to rank.
static	int		g_iSortedPlayers[MAXPLAYERS];//ghk
//	PROTOTYPES ghk


static	void			scoreboard_SortPlayers( ); //ghk
static	void			scoreboard_SortPlayersByPoints(); //ghk
static	int	STACK_ARGS	scoreboard_TotalsCompareFunc( const void *arg1, const void *arg2 );//ghk
static	int STACK_ARGS	scoreboard_PointsCompareFunc( const void *arg1, const void *arg2 ); //ghk
/*
bool IsTiedSD( ULONG ulPlayerNum, ULONG state );
//static	void			scoreboard_RenderIndividualPlayer( bool bScale, ULONG ulYPos, ULONG ulColor, char *pszColumn1, char *pszColumn2, char *pszColumn3, char *pszColumn4, bool bIsBot, bool bReadyToGoOn, ULONG ulHandicap );
//leader is passed in to determine if
//they deserve bonus 100 points
bool IsTiedSD( ULONG ulPlayerNum, ULONG state )
{
	ULONG	ulIdx;

	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if ( ulIdx == ulPlayerNum || ( playeringame[ulIdx] == false ) || ( PLAYER_IsTrueSpectator( &players[ulIdx] )))
			continue;

		if ( state==1 )
		{

			if ( players[ulIdx].killcount == players[ulPlayerNum].killcount )
				return ( true );
		}
		else if ( state==2 )
		{
			if ( players[ulIdx].itemcount == players[ulPlayerNum].itemcount )
				return ( true );
		}
		else if ( state==3 )
		{
			if ( players[ulIdx].secretcount == players[ulPlayerNum].secretcount )
				return ( true );
		}
			else if ( state==4 )
		{
			if ( players[ulIdx].barrelcount == players[ulPlayerNum].barrelcount )
				return ( true );
		}
	}

	return ( false );
}
*/

static void scoreboard_SortPlayers( )
{
	ULONG	ulIdx;

	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
		g_iSortedPlayers[ulIdx] = ulIdx;

	//if ( ulSortType == ST_TOTALCOUNT )
		qsort( g_iSortedPlayers, MAXPLAYERS, sizeof( int ), scoreboard_TotalsCompareFunc );
}
//******************************************************************************
static void scoreboard_SortPlayersByPoints( )
{
	ULONG	ulIdx;

	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
		g_iSortedPlayers[ulIdx] = ulIdx;

	//if ( ulSortType == ST_TOTALCOUNT )
		qsort( g_iSortedPlayers, MAXPLAYERS, sizeof( int ), scoreboard_PointsCompareFunc );
}

//*****************************************************************************
//
static int STACK_ARGS scoreboard_TotalsCompareFunc( const void *arg1, const void *arg2 )
{
	return ( (players[*(int *)arg2].killcount + players[*(int *)arg2].itemcount +
	players[*(int *)arg2].secretcount + players[*(int *)arg2].barrelcount)
	-
	(players[*(int *)arg1].killcount + players[*(int *)arg1].itemcount +
	players[*(int *)arg1].secretcount + players[*(int *)arg1].barrelcount) );
}

//*****************************************************************************
//
static int STACK_ARGS scoreboard_PointsCompareFunc( const void *arg1, const void *arg2 )
{
	return ( players[*(int *)arg2].lPointCount - players[*(int *)arg1].lPointCount );
}


//*****************************************************************************
//



//*****

static FRandom pr_obituary ("Obituary");
static FRandom pr_botrespawn ("BotRespawn");
static FRandom pr_killmobj ("ActorDie");
static FRandom pr_damagemobj ("ActorTakeDamage");
static FRandom pr_lightning ("LightningDamage");
static FRandom pr_poison ("PoisonDamage");
static FRandom pr_switcher ("SwitchTarget");

/*static*/	bool		g_bFirstFragAwarded;

CVAR (Bool, cl_showsprees, true, CVAR_ARCHIVE)
CVAR (Bool, cl_showmultikills, true, CVAR_ARCHIVE)

EXTERN_CVAR (Bool, show_obituaries)
EXTERN_CVAR (Bool, sd_alt_con_scoring) //ghk

CUSTOM_CVAR (Int, sv_coopdeath_penalty, 1000, CVAR_SERVERINFO)
{
//ghk
	if (self < 0)
		self=0;

}

//
// GET STUFF
//

//
// P_TouchSpecialThing
//
void P_TouchSpecialThing (AActor *special, AActor *toucher)
{
	fixed_t delta = special->z - toucher->z;

	if (delta > toucher->height || delta < -32*FRACUNIT)
	{ // out of reach
		return;
	}

	// Dead thing touching.
	// Can happen with a sliding player corpse.
	if (toucher->health <= 0)
		return;

	// [BC] Don't allow non-players to touch special things.
	if ( toucher->player == NULL )
		return;

	// [BC] Don't allow spectating players to pickup items.
	if ( toucher->player->bSpectating )
		return;

	special->Touch (toucher);
}


// [RH]
// SexMessage: Replace parts of strings with gender-specific pronouns
//
// The following expansions are performed:
//		%g -> he/she/it
//		%h -> him/her/it
//		%p -> his/her/its
//		%o -> other (victim)
//		%k -> killer
//
void SexMessage (const char *from, char *to, int gender, const char *victim, const char *killer)
{
	static const char *genderstuff[3][3] =
	{
		{ "he",  "him", "his" },
		{ "she", "her", "her" },
		{ "it",  "it",  "its" }
	};
	static const int gendershift[3][3] =
	{
		{ 2, 3, 3 },
		{ 3, 3, 3 },
		{ 2, 2, 3 }
	};
	const char *subst = NULL;

	do
	{
		if (*from != '%')
		{
			*to++ = *from;
		}
		else
		{
			int gendermsg = -1;

			switch (from[1])
			{
			case 'g':	gendermsg = 0;	break;
			case 'h':	gendermsg = 1;	break;
			case 'p':	gendermsg = 2;	break;
			case 'o':	subst = victim;	break;
			case 'k':	subst = killer;	break;
			}
			if (subst != NULL)
			{
				size_t len = strlen (subst);
				memcpy (to, subst, len);
				to += len;
				from++;
				subst = NULL;
			}
			else if (gendermsg < 0)
			{
				*to++ = '%';
			}
			else
			{
				strcpy (to, genderstuff[gender][gendermsg]);
				to += gendershift[gender][gendermsg];
				from++;
			}
		}
	} while (*from++);
}

// [RH]
// ClientObituary: Show a message when a player dies
//
// [BC] Allow passing in of the MOD so clients can use this function too.
void ClientObituary (AActor *self, AActor *inflictor, AActor *attacker, LONG lMeansOfDeath)
{
	int	mod;
	const char *message;
	const char *messagename;
	char gendermessage[1024];
	INTBOOL friendly;
	int  gender;
	bool	bGibbed;
	char	szAttacker[32];
	char	szVictim[32];

	// No obituaries for non-players, voodoo dolls or when not wanted
	if (self->player == NULL || self->player->mo != self || !show_obituaries)
		return;

	bGibbed = self->health <= -100;
	gender = self->player->userinfo.gender;

	// Treat voodoo dolls as unknown deaths
	if (inflictor && inflictor->player == self->player)
		lMeansOfDeath = MOD_UNKNOWN;

	// Must be in cooperative mode.
	if (( NETWORK_GetState( ) != NETSTATE_SINGLE ) && ( deathmatch == false ) && ( teamgame == false ))
		lMeansOfDeath |= MOD_FRIENDLY_FIRE;

	friendly = lMeansOfDeath & MOD_FRIENDLY_FIRE;
	mod = lMeansOfDeath & ~MOD_FRIENDLY_FIRE;
	message = NULL;
	messagename = NULL;

	//GHK ScoreDoom sp & co-op player deaths
		if ((cooperative||teamcoop) && !(deathmatch && (attacker == NULL || attacker->player != NULL)))
		{
			if (mod == MOD_TELEFRAG)
			{
				if ( NETWORK_GetState( ) == NETSTATE_SERVER )
					SERVERCOMMANDS_AnnounceTelefragSD1 (self->player - players,SVCF_ONLYTHISCLIENT);
					//return;
			}
			else
			{
				if ( NETWORK_GetState( ) == NETSTATE_SERVER )
					SERVERCOMMANDS_AnnounceKillSD1(attacker, self, self->player - players,SVCF_ONLYTHISCLIENT);
					//return;
				//GHK: Player dies in co-op, not related to player kills, so give a penalty
				if((attacker == NULL ||attacker->player == NULL||attacker==self)){
					if ( NETWORK_GetState( ) != NETSTATE_CLIENT ){
						//500 point penalty
						if(self->player->lPointCount>=sv_coopdeath_penalty){
							self->player->lPointCount-=sv_coopdeath_penalty;
							if(teamcoop&&self->player->bOnTeam)
								TEAM_SetScore( self->player->ulTeam, TEAM_GetScore( self->player->ulTeam ) - sv_coopdeath_penalty, true );


								if ( NETWORK_GetState( ) == NETSTATE_SERVER ){
									SERVERCOMMANDS_SetPlayerPoints( self->player - players );
													// Also, update the scoreboard.
									SERVERCONSOLE_UpdatePlayerInfo( ULONG( self->player - players ), UDF_FRAGS );
									SERVERCONSOLE_UpdateScoreboard( );
									SERVERCOMMANDS_CheckPointLimit( self->player );
								}


						}else{


							if(teamcoop&&self->player->bOnTeam)
								TEAM_SetScore( self->player->ulTeam, TEAM_GetScore( self->player->ulTeam ) - self->player->lPointCount, true );

							self->player->lPointCount=0;

							if ( NETWORK_GetState( ) == NETSTATE_SERVER ){
									SERVERCOMMANDS_SetPlayerPoints( self->player - players );
									SERVERCONSOLE_UpdatePlayerInfo( ULONG( self->player - players ), UDF_FRAGS );
									SERVERCONSOLE_UpdateScoreboard( );
									SERVERCOMMANDS_CheckPointLimit( self->player );

							}


						}
					}
					SCOREBOARD_RefreshHUD( );

			//sprintf(levelscore,"%d",source->player->level_score);

				if(NETWORK_GetState( ) == NETSTATE_SERVER ){
					//Printf (PRINT_LOW, "- %s\n", "500");
					if(sv_coopdeath_penalty > 0 ){
						char buffer[50];
						sprintf(buffer,"You Died! -%d Penalty!",(int)sv_coopdeath_penalty);
						SERVERCOMMANDS_PrintMid(buffer,self->player - players, SVCF_ONLYTHISCLIENT);
					}else{
						SERVERCOMMANDS_PrintMid("You Died!",self->player - players, SVCF_ONLYTHISCLIENT);
					}
				}

				}
			}
		}
	switch (mod)
	{
	case MOD_SUICIDE:		messagename = "OB_SUICIDE";		break;
	case MOD_FALLING:		messagename = "OB_FALLING";		break;
	case MOD_CRUSH:			messagename = "OB_CRUSH";		break;
	case MOD_EXIT:			messagename = "OB_EXIT";		break;
	case MOD_WATER:			messagename = "OB_WATER";		break;
	case MOD_SLIME:			messagename = "OB_SLIME";		break;
	case MOD_FIRE:			messagename = "OB_LAVA";		break;
	case MOD_BARREL:		messagename = "OB_BARREL";		break;
	case MOD_SPLASH:		messagename = "OB_SPLASH";		break;
	// [BC] Handle Skulltag's reflection rune.
	case MOD_REFLECTION:	messagename = "OB_REFLECTION";	break;
	}

	if (messagename != NULL)
		message = GStrings(messagename);

	if (attacker != NULL && message == NULL)
	{
		if (attacker == self)
		{
			switch (mod)
			{
			case MOD_R_SPLASH:	messagename = "OB_R_SPLASH";		break;
			case MOD_ROCKET:	messagename = "OB_ROCKET";			break;
			// [BC] Obituaries for killing yourself with Skulltag weapons.
			case MOD_GRENADE:	messagename = "OB_GRENADE_SELF";	break;
			case MOD_BFG10K:	messagename = "OB_BFG10K_SELF";		break;
			default:			messagename = "OB_KILLEDSELF";		break;
			}
			message = GStrings(messagename);
		}
		else if (attacker->player == NULL)
		{
			if ((mod == MOD_TELEFRAG) || (mod == MOD_SPAWNTELEFRAG))
			{
				message = GStrings("OB_MONTELEFRAG");
			}
			else if (mod == MOD_HIT)
			{
				message = attacker->GetClass()->Meta.GetMetaString (AMETA_HitObituary);
				if (message == NULL)
				{
					message = attacker->GetClass()->Meta.GetMetaString (AMETA_Obituary);
				}
			}
			else
			{
				message = attacker->GetClass()->Meta.GetMetaString (AMETA_Obituary);
			}
		}
	}

	if (message == NULL && attacker != NULL && attacker->player != NULL)
	{
		if (friendly)
		{
			attacker->player->fragcount -= 2;
			self = attacker;
			gender = self->player->userinfo.gender;
			sprintf (gendermessage, "OB_FRIENDLY%c", '1' + (pr_obituary() & 3));
			message = GStrings(gendermessage);
		}
		else
		{
			// [BC] MOD_SPAWNTELEFRAG, too.
			if ((mod == MOD_TELEFRAG) || (mod == MOD_SPAWNTELEFRAG)) message = GStrings("OB_MPTELEFRAG");
			if (message == NULL)
			{
				if (inflictor != NULL)
				{
					message = inflictor->GetClass()->Meta.GetMetaString (AMETA_Obituary);
				}
				if (message == NULL && attacker->player->ReadyWeapon != NULL)
				{
					message = attacker->player->ReadyWeapon->GetClass()->Meta.GetMetaString (AMETA_Obituary);
				}
				if (message == NULL)
				{
					switch (mod)
					{
					case MOD_R_SPLASH:		messagename = "OB_MPR_SPLASH";		break;
					case MOD_BFG_SPLASH:	messagename = "OB_MPBFG_SPLASH";	break;
					case MOD_RAILGUN:		messagename = "OB_RAILGUN";			break;
					}
					if (messagename != NULL)
						message = GStrings(messagename);
				}
			}
		}
	}
	else attacker = self;	// for the message creation

	if (message != NULL && message[0] == '$')
	{
		message=GStrings[message+1];
	}

	if (message == NULL)
	{
		message = GStrings("OB_DEFAULT");
	}

	// [BC] Stop the color codes after we display a name in the obituary string.
	if ( attacker && attacker->player )
		sprintf( szAttacker, "%s\\c-", attacker->player->userinfo.netname );
	else
		szAttacker[0] = 0;

	if ( self && self->player )
		sprintf( szVictim, "%s\\c-", self->player->userinfo.netname );
	else
		szVictim[0] = 0;

	SexMessage (message, gendermessage, gender,
		szVictim[0] ? szVictim : self->player->userinfo.netname,
		szAttacker[0] ? szAttacker : attacker->player->userinfo.netname);
	Printf (PRINT_MEDIUM, "%s\n", gendermessage);
}


//
// KillMobj
//
EXTERN_CVAR (Int, fraglimit)

void AActor::Die (AActor *source, AActor *inflictor)
{

	// [BC]
	bool	bPossessedTerminatorArtifact;

	//GHK: added for score calculations when player is giving out damage
	char levelscore[30];

	// [SO] 9/2/02 -- It's rather funny to see an exploded player body with the invuln sparkle active :)
	effects &= ~FX_RESPAWNINVUL;

	// Also kill the alpha flicker cause by the visibility flicker.
	if ( effects & FX_VISIBILITYFLICKER )
	{
		RenderStyle = STYLE_Normal;
		effects &= ~FX_VISIBILITYFLICKER;
	}

	//flags &= ~MF_INVINCIBLE;
/*
	if (debugfile && this->player)
	{
		static int dieticks[MAXPLAYERS];
		int pnum = this->player-players;
		if (dieticks[pnum] == gametic)
			gametic=gametic;
		dieticks[pnum] = gametic;
		fprintf (debugfile, "died (%d) on tic %d (%s)\n", pnum, gametic,
		this->player->cheats&CF_PREDICTING?"predicting":"real");
	}
*/
	// [BC] Since the player loses his terminator status after we tell his inventory
	// that we died, check for it before doing so.
	bPossessedTerminatorArtifact = !!(( player ) && ( player->Powers & PW_TERMINATORARTIFACT ));

	// [RH] Notify this actor's items.
	for (AInventory *item = Inventory; item != NULL; )
	{
		AInventory *next = item->Inventory;
		item->OwnerDied();
		item->Owner=this; //ghk, fix for morphed monsters with inventory

		item = next;
	}

	if (flags & MF_MISSILE)
	{ // [RH] When missiles die, they just explode
		P_ExplodeMissile (this, NULL, NULL);
		return;
	}
	// [RH] Set the target to the thing that killed it. Strife apparently does this.
	if (source != NULL)
	{
		target = source;
	}

	flags &= ~(MF_SHOOTABLE|MF_FLOAT|MF_SKULLFLY);
	if (!(flags4 & MF4_DONTFALL)) flags&=~MF_NOGRAVITY;
	flags |= MF_DROPOFF;
	if ((flags3 & MF3_ISMONSTER) || RaiseState != NULL)
	{	// [RH] Only monsters get to be corpses.
		// Objects with a raise state should get the flag as well so they can
		// be revived by an Arch-Vile. Batman Doom needs this.
		flags |= MF_CORPSE;
	}
	// [RH] Allow the death height to be overridden using metadata.
	fixed_t metaheight = 0;
	if (DamageType == MOD_FIRE)
	{
		metaheight = GetClass()->Meta.GetMetaFixed (AMETA_BurnHeight);
	}
	if (metaheight == 0)
	{
		metaheight = GetClass()->Meta.GetMetaFixed (AMETA_DeathHeight);
	}
	if (metaheight != 0)
	{
		height = MAX<fixed_t> (metaheight, 0);
	}
	else
	{
		height >>= 2;
	}


	// [RH] If the thing has a special, execute and remove it
	//		Note that the thing that killed it is considered
	//		the activator of the script.
	// New: In Hexen, the thing that died is the activator,
	//		so now a level flag selects who the activator gets to be.
	if (special && (!(flags & MF_SPECIAL) || (flags3 & MF3_ISMONSTER)))
	{
		LineSpecials[special] (NULL, level.flags & LEVEL_ACTOWNSPECIAL
			? this : source, false, args[0], args[1], args[2], args[3], args[4]);
		special = 0;
	}

	if (CountsAsKill())
		level.killed_monsters++;

	if(GetClass()->ActorInfo->SpawnID==125){
		level.killed_barrels++; //add players[0].barrelcount++ if !multiplayer here?

			if ( (source && source->player) && ( NETWORK_GetState( ) == NETSTATE_SERVER ) )
			{
				//update barrecoutn
				SERVERCOMMANDS_SetPlayerKillCount( ULONG( source->player - players ),MAXPLAYERS,0, 4 );

				// Also, update the scoreboard.
				//dont this  is needed:
				//SERVERCONSOLE_UpdatePlayerInfo( ULONG( source->player - players ), UDF_FRAGS );
				//SERVERCONSOLE_UpdateScoreboard( );
			}

	}

	//ghk for turrets
	if(source&&source->tracer&&source->tracer->player){
		//if(source->IsKindOf(PClass::FindClass ("TurretA")))
			source=source->tracer; //*SHOULD* BE FINE
	}

	if (source && source->player)
	{

		// [BC] Don't do this in client mode.
		int edamage=0; //ghk used for adding extreme damage bonuses to chain bonuses.
		if (CountsAsKill()&& (NETWORK_GetState( ) != NETSTATE_CLIENT))
		{ // count for intermission

			if(NETWORK_GetState( ) != NETSTATE_CLIENT){
				source->player->killcount++;
			}

			//GHK Calculate 10% OF MAX HEALTH bounty for the kill.
			//playerGHK->level_score+=((AActor*)(type->Defaults))->
			int BonusPoints=(GetDefault()->health)/10;
			if(cooperative||teamcoop){
				source->player->lPointCount+=BonusPoints;
			if(teamcoop&&source->player->bOnTeam)
				TEAM_SetScore( source->player->ulTeam, TEAM_GetScore( source->player->ulTeam ) + BonusPoints, true );


				if( NETWORK_GetState( ) == NETSTATE_SERVER ){
					SERVERCOMMANDS_SetPlayerPoints( source->player - players );
					SERVERCONSOLE_UpdatePlayerInfo( ULONG( source->player - players ), UDF_FRAGS );
					SERVERCONSOLE_UpdateScoreboard( );
					SERVERCOMMANDS_CheckPointLimit(  source->player  );
				}


				//sprintf(levelscore,"%d",source->player->level_score);

				if(NETWORK_GetState( ) == NETSTATE_SERVER ){
						//sprintf(levelscore,"+ %d",BonusPoints);
						//Printf (PRINT_LOW, "%s\n", levelscore);
						//Just use bonus points from above (remember it's default halth /10)
						//GHK REMOVE THIS!!


						if(BonusPoints>=70&&BonusPoints<100){
							SERVERCOMMANDS_AnnounceBigKill(source->player - players, SVCF_ONLYTHISCLIENT);//GHK Announce a 'boss' death
							sprintf(levelscore,TEXTCOLOR_PURPLE"Big Kill! +%d Bonus!",BonusPoints);
							SERVERCOMMANDS_PrintMid(levelscore,source->player - players, SVCF_ONLYTHISCLIENT);


						}else if(BonusPoints>=100&&BonusPoints<=300){
							SERVERCOMMANDS_AnnounceBigKill(source->player - players, SVCF_ONLYTHISCLIENT);//GHK Announce a 'boss' death
							sprintf(levelscore,TEXTCOLOR_PURPLE"Boss Kill! +%d Bonus!",BonusPoints);
							SERVERCOMMANDS_PrintMid(levelscore,source->player - players, SVCF_ONLYTHISCLIENT);


						}else if(BonusPoints>300){
							SERVERCOMMANDS_AnnounceBigKill(source->player - players, SVCF_ONLYTHISCLIENT, true);//GHK Announce a 'boss' death
							sprintf(levelscore,TEXTCOLOR_PURPLE"Mega Boss Kill! +%d Bonus!",BonusPoints);
							SERVERCOMMANDS_PrintMid(levelscore,source->player - players, SVCF_ONLYTHISCLIENT);


						}
						else{
							sprintf(levelscore,"+ %d\n",BonusPoints);
							SERVERCOMMANDS_Print(levelscore,PRINT_LOW, source->player - players, SVCF_ONLYTHISCLIENT);
						}

				}


				int gibhealth = -abs(GetClass()->Meta.GetMetaInt (AMETA_GibHealth,
						gameinfo.gametype == GAME_Doom ? -GetDefault()->health : -GetDefault()->health/2));

					if(health<gibhealth&&GetDefault()->health>=20){ //extreme death bonus, //only for 20 or more hp
						edamage=abs(gibhealth)/10;

							source->player->lPointCount+=edamage;
							if( NETWORK_GetState( ) == NETSTATE_SERVER ){
								SERVERCOMMANDS_SetPlayerPoints( source->player - players );
								SERVERCONSOLE_UpdatePlayerInfo( ULONG( source->player - players ), UDF_FRAGS );
								SERVERCONSOLE_UpdateScoreboard( );
								SERVERCOMMANDS_CheckPointLimit(  source->player  );
								if(teamcoop&&source->player->bOnTeam)
									TEAM_SetScore( source->player->ulTeam, TEAM_GetScore( source->player->ulTeam ) + edamage, true );

							//}


						//if(source->CheckLocalView(consoleplayer)&&(NETWORK_GetState( ) != NETSTATE_CLIENT)){
								sprintf(levelscore,TEXTCOLOR_DARKRED"+ %d Extreme Death Bonus!\n",edamage);
								SERVERCOMMANDS_Print(levelscore,PRINT_LOW, source->player - players, SVCF_ONLYTHISCLIENT);
							}

					}
			}

			if ( invasion )
			{
				INVASION_SetNumMonstersLeft( INVASION_GetNumMonstersLeft( ) - 1 );

				if ( GetClass( ) == RUNTIME_CLASS( AArchvile ))
					INVASION_SetNumArchVilesLeft( INVASION_GetNumArchVilesLeft( ) - 1 );

				// If we're the server, tell the client how many monsters are left.
				if ( NETWORK_GetState( ) == NETSTATE_SERVER )
					SERVERCOMMANDS_SetInvasionNumMonstersLeft( );
			}

			// Update the clients with this player's updated killcount.
			if ( NETWORK_GetState( ) == NETSTATE_SERVER )
			{
				SERVERCOMMANDS_SetPlayerKillCount( ULONG( source->player - players ),MAXPLAYERS,0, 1 );

				// Also, update the scoreboard.
				SERVERCONSOLE_UpdatePlayerInfo( ULONG( source->player - players ), UDF_FRAGS );
				SERVERCONSOLE_UpdateScoreboard( );
			}



		}
		else if((cooperative||teamcoop)&&GetClass()->ActorInfo->SpawnID==125&& (NETWORK_GetState( ) != NETSTATE_CLIENT)){ //GHK Give +5 points for barrel killls
				int BonusPoints=10; //was 5
				source->player->barrelcount++;

				source->player->lPointCount+=BonusPoints;


				if(NETWORK_GetState( ) == NETSTATE_SERVER){
					SERVERCOMMANDS_SetPlayerPoints( source->player - players );
					SERVERCONSOLE_UpdatePlayerInfo( ULONG( source->player - players ), UDF_FRAGS );
					SERVERCONSOLE_UpdateScoreboard( );
					SERVERCOMMANDS_CheckPointLimit(  source->player  );
					if(teamcoop&&source->player->bOnTeam)
						TEAM_SetScore( source->player->ulTeam, TEAM_GetScore( source->player->ulTeam ) + BonusPoints, true );


					//if(source->player->mo->CheckLocalView(consoleplayer)){
						sprintf(levelscore,"+ %d\n",BonusPoints);
						SERVERCOMMANDS_Print(levelscore,PRINT_LOW, source->player - players, SVCF_ONLYTHISCLIENT);


				}
		}else if (cooperative||teamcoop){
			if(!this->player&&GetClass()== RUNTIME_CLASS(ABossBrain)&& (NETWORK_GetState( ) != NETSTATE_CLIENT)){//boss brain Death, CountsAsKill() not set for it.
				int BBBonusPoints=5000;

					source->player->lPointCount+=BBBonusPoints;

					if(NETWORK_GetState( ) == NETSTATE_SERVER){
						if(teamcoop&&source->player->bOnTeam)
							TEAM_SetScore( source->player->ulTeam, TEAM_GetScore( source->player->ulTeam ) + BBBonusPoints, true );

						SERVERCOMMANDS_SetPlayerPoints( source->player - players );
						SERVERCONSOLE_UpdatePlayerInfo( ULONG( source->player - players ), UDF_FRAGS );
						SERVERCONSOLE_UpdateScoreboard( );
						SERVERCOMMANDS_CheckPointLimit(  source->player  );
					}


					//if (!AnnounceMultikillSD (source->player - players)){ //GHK: ALWAYS RETURNS FALSE! AND GIVE A 'GOOD JOB' MESSAGE
						if((NETWORK_GetState( ) == NETSTATE_SERVER)){
							char brainscore[40];
							//Printf (PRINT_MEDIUM, TEXTCOLOR_LIGHTBLUE"+%d"TEXTCOLOR_WHITE"%s\n", BBBonusPoints, " Boss Brain Kill Bonus!");
							sprintf(brainscore,TEXTCOLOR_PURPLE"+%d"TEXTCOLOR_PURPLE"%s\n", BBBonusPoints, " Boss Brain Kill Bonus!");
							SERVERCOMMANDS_PrintMid(brainscore,source->player - players, SVCF_ONLYTHISCLIENT);
							SERVERCOMMANDS_AnnounceMultiKillSD( source->player - players, SVCF_ONLYTHISCLIENT);

						}
					//}
			}

		}

		//if(CountsAsKill())
			//SCOREBOARD_RefreshHUD( );

		// [RH] Multikills
		//GHK: Multikills for Scoredoom
		//Dont count player deaths either!!

		//if(!deathmatch&&!player){
		if(((cooperative||teamcoop)&&!player)&&(CountsAsKill()&&GetDefault()->health>=20)&& (NETWORK_GetState( ) != NETSTATE_CLIENT)){ //only count 20hp or more monsters.
			int mkBonusPoints = 0;
			int sddeathBonus = (GetDefault()->health)/10;
			int BonusPoints=(sddeathBonus>5?sddeathBonus:5)+edamage; //If HPs are greater than 50, give a higher chain bonus based on HP, else give +5, also add extreme damage bonus.
			//int BonusPoints=sddeathBonus+10+edamage;//Chain bonus is +10, then death bonus, also add extreme damage bonus.

			source->player->multicount++;
			players[consoleplayer].multicount=source->player->multicount;

			if (source->player->lastkilltime > 0)
			{

				if (source->player->lastkilltime < level.time - 3*TICRATE)
				{
					players[consoleplayer].multicount=source->player->multicount = 1;
				}

				//if (source->CheckLocalView (consoleplayer) &&
					//cl_showmultikills)
				//{
					char *multimsg;
					char* bonusmsg = new char[30];

					switch (source->player->multicount)
					{
					case 1:
					case 2:
					case 3:
					case 5:
					case 7:
					case 9:
						multimsg = NULL;
						break;
					case 4:
						multimsg = " Multi Kill Bonus!"; //GStrings("MULTI2");
						mkBonusPoints +=BonusPoints;
						break;
					case 6:
						multimsg = " Crazy Kill Bonus!"; //GStrings("MULTI3");
						mkBonusPoints +=BonusPoints; //+=20;
						break;
					case 8:
						multimsg = " Ultra Kill Bonus!";  //GStrings("MULTI4");
						mkBonusPoints +=BonusPoints; //+=30;
						break;
					case 10:
						multimsg = " Monster Kill Bonus!"; //GStrings("MULTI5");
						mkBonusPoints +=BonusPoints; //+=40;
						break;
					default:
						multimsg = " Chain Bonus!"; //GStrings("MULTI5");
						mkBonusPoints +=BonusPoints; //+=50;
						break;
					}
					//if (cl_showmultikills && multimsg != NULL)
					if (multimsg != NULL)
					{
						char buff[256];

						source->player->lPointCount+=mkBonusPoints;


						if ((NETWORK_GetState( ) != NETSTATE_CLIENT))
						{

							if(deathmatch){
								SexMessage (multimsg, buff, player->userinfo.gender,
									player->userinfo.netname, source->player->userinfo.netname);
								StatusBar->AttachMessage (new DHUDMessageFadeOut (buff,
									1.5f, 0.8f, 0, 0, CR_RED, 3.f, 0.5f), MAKE_ID('M','K','I','L'));
							}else{
								if (CountsAsKill()&&(NETWORK_GetState( ) == NETSTATE_SERVER))
								{
										if(teamcoop&&source->player->bOnTeam)
											TEAM_SetScore( source->player->ulTeam, TEAM_GetScore( source->player->ulTeam ) + mkBonusPoints, true );

										SERVERCOMMANDS_SetPlayerPoints( source->player - players );
										SERVERCONSOLE_UpdatePlayerInfo( ULONG( source->player - players ), UDF_FRAGS );
										SERVERCONSOLE_UpdateScoreboard( );
										SERVERCOMMANDS_CheckPointLimit(  source->player  );



									//if (source->CheckLocalView (consoleplayer)){
										sprintf(bonusmsg,TEXTCOLOR_LIGHTBLUE"%s""%d"TEXTCOLOR_GOLD"%s","+ ",mkBonusPoints,multimsg);
									//sprintf(bonusmsg+14,"+ %d",mkBonusPoints);
									//Printf (PRINT_MEDIUM, "%s\n", bonusmsg);
										SERVERCOMMANDS_AnnounceMultiKillSD( source->player - players, SVCF_ONLYTHISCLIENT);
										SERVERCOMMANDS_PrintMid(bonusmsg,source->player - players, SVCF_ONLYTHISCLIENT);
									//}
								}
							}
						}
					}
				//}
			}
			players[consoleplayer].lastkilltime=source->player->lastkilltime = level.time;
		}

		//ghk
		FName mytype = GetClass()->TypeName;

		//FName type = GetClass()->ActorInfo->GetReplacee()->Class->TypeName;

		if(strcmp(mytype.GetChars(),"PointsBunny")==0||strcmp(mytype.GetChars(),"PointsBunnyChain")==0){
			source->player->lPointCount+=500;
			if ((NETWORK_GetState( ) == NETSTATE_SERVER))
			{
					//if(teamcoop&&source->player->bOnTeam)
						//TEAM_SetScore( source->player->ulTeam, TEAM_GetScore( source->player->ulTeam ) + mkBonusPoints, true );
					char* bonusmsg = new char[30];
					SERVERCOMMANDS_SetPlayerPoints( source->player - players );
					SERVERCONSOLE_UpdatePlayerInfo( ULONG( source->player - players ), UDF_FRAGS );
					SERVERCONSOLE_UpdateScoreboard( );
					SERVERCOMMANDS_CheckPointLimit(  source->player );


									//if (source->CheckLocalView (consoleplayer)){
					sprintf(bonusmsg,TEXTCOLOR_GOLD" +500 Bonus Points!");
									//sprintf(bonusmsg+14,"+ %d",mkBonusPoints);
									//Printf (PRINT_MEDIUM, "%s\n", bonusmsg);
					//SERVERCOMMANDS_AnnounceMultiKillSD( source->player - players, SVCF_ONLYTHISCLIENT);
					SERVERCOMMANDS_PrintMid(bonusmsg,source->player - players, SVCF_ONLYTHISCLIENT);
									//}
					SERVERCOMMANDS_SoundPoint( source->player->mo->x , source->player->mo->y, CHAN_AUTO, "ghk/keyscr", 127, ATTN_NORM, (source->player - players), SVCF_ONLYTHISCLIENT );

			}

		}

		//if(cooperative&&NETWORK_GetState( ) != NETSTATE_CLIENT){
			//sprintf(levelscore,"points total: %d\n",source->player->lPointCount);
			//SERVERCOMMANDS_Print(levelscore,PRINT_LOW, source->player - players, SVCF_ONLYTHISCLIENT);
		//}

		if(CountsAsKill()) //ghk
			SCOREBOARD_RefreshHUD( );


		// Don't count any frags at level start, because they're just telefrags
		// resulting from insufficient deathmatch starts, and it wouldn't be
		// fair to count them toward a player's score.
		if (player && (( MeansOfDeath & ~MOD_FRIENDLY_FIRE ) != MOD_SPAWNTELEFRAG ))
		{
			if ( source->player->pSkullBot )
			{
				source->player->pSkullBot->m_ulPlayerKilled = player - players;
				if (( player - players ) == source->player->pSkullBot->m_ulPlayerEnemy )
					source->player->pSkullBot->PostEvent( BOTEVENT_ENEMY_KILLED );
			}

			// [BC] Potentially get rid of some corpses. This isn't necessarily client-only.
			CLIENT_RemoveCorpses( );

			if (player == source->player)	// [RH] Cumulative frag count
			{
				// [BC] Frags are server side.
				if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
					PLAYER_SetFragcount( player, player->fragcount - (( bPossessedTerminatorArtifact ) ? 10 : 1 ), true, true );
			}
			else
			{
				// [BC] Don't award frags here in client mode.
				if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
				{
					if ( bPossessedTerminatorArtifact )
					{
						// [BC] Player receives 10 frags for killing the terminator!
						PLAYER_SetFragcount( source->player, source->player->fragcount + 10, true, true );

						// [BC] Award the fragger with a "Termination!" medal.
						MEDAL_GiveMedal( source->player - players, MEDAL_TERMINATION );

						// [BC] Tell clients about the medal that been given.
						if ( NETWORK_GetState( ) == NETSTATE_SERVER )
							SERVERCOMMANDS_GivePlayerMedal( source->player - players, MEDAL_TERMINATION );
					}
					else
					{
						PLAYER_SetFragcount( source->player, source->player->fragcount + 1, true, true );
					}
				}

				// [BC] Add this frag to the server's statistic module.
				if ( NETWORK_GetState( ) == NETSTATE_SERVER )
					SERVER_STATISTIC_AddToTotalFrags( );

				if (source->player->morphTics)
				{ // Make a super chicken
					source->GiveInventoryType (RUNTIME_CLASS(APowerWeaponLevel2));
				}

				// [BC] Don't award Total/Domination or increment frags w/o death if this is a teammate.
				if ( player->mo->IsTeammate( source ) == false )
				{
					// If this is a deathmatch game, and the "First frag!" medal hasn't been
					// awarded yet, award it here.
					if (( deathmatch ) &&
						( lastmanstanding == false ) &&
						( teamlms == false ) &&
						( possession == false ) &&
						( teampossession == false ) &&
						(( duel == false ) || ( DUEL_GetState( ) == DS_INDUEL )) &&
						( g_bFirstFragAwarded == false ))
					{
						if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
							MEDAL_GiveMedal( source->player - players, MEDAL_FIRSTFRAG );

						// Tell clients about the medal that been given.
						if ( NETWORK_GetState( ) == NETSTATE_SERVER )
							SERVERCOMMANDS_GivePlayerMedal( source->player - players, MEDAL_FIRSTFRAG );

						g_bFirstFragAwarded = true;
					}

					// [BC] Don't do this block in client mode.
					if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
					{
						// [BC] Increment frags without death.
						source->player->ulFragsWithoutDeath++;

						// [BC] If the player has gotten 5 straight frags without dying, award a medal.
						if (( source->player->ulFragsWithoutDeath % 5 ) == 0 )
						{
							if ( source->player->ulFragsWithoutDeath >= 10 )
							{
								// [BC] If the player gets 10+ straight frags without dying, award a "Total Domination" medal.
								MEDAL_GiveMedal( source->player - players, MEDAL_TOTALDOMINATION );

								// [BC] Tell clients about the medal that been given.
								if ( NETWORK_GetState( ) == NETSTATE_SERVER )
									SERVERCOMMANDS_GivePlayerMedal( source->player - players, MEDAL_TOTALDOMINATION );
							}
							else
							{
								// [BC] Otherwise, award a "Domination" medal.
								MEDAL_GiveMedal( source->player - players, MEDAL_DOMINATION );

								// [BC] Tell clients about the medal that been given.
								if ( NETWORK_GetState( ) == NETSTATE_SERVER )
									SERVERCOMMANDS_GivePlayerMedal( source->player - players, MEDAL_DOMINATION );
							}
						}
					}
				}

				// [BC] Don't do this block in client mode.
				if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
				{
					// [BC] Reset deaths without frag.
					source->player->ulDeathsWithoutFrag = 0;

					// Don't award Excellent/Incredible if this is a teammate.
					if ( player->mo->IsTeammate( source ) == false )
					{
						// If the player killed him with this fist, award him a "Fisting!" medal.
						if (( source->player->ReadyWeapon ) && ( source->player->ReadyWeapon->GetClass( ) == PClass::FindClass( "Fist" )))
						{
							// Award the medal.
							MEDAL_GiveMedal( source->player - players, MEDAL_FISTING );

							// Tell clients about the medal that been given.
							if ( NETWORK_GetState( ) == NETSTATE_SERVER )
								SERVERCOMMANDS_GivePlayerMedal( source->player - players, MEDAL_FISTING );
						}

						// If this is the second frag this player has gotten THIS TICK with the
						// BFG9000, award him a "SPAM!" medal.
						if (( source->player->ReadyWeapon ) && ( source->player->ReadyWeapon->GetClass( ) == PClass::FindClass( "BFG9000" )))
						{
							if ( source->player->ulLastBFGFragTick == level.time )
							{
								// Award the medal.
								MEDAL_GiveMedal( source->player - players, MEDAL_SPAM );

								// Tell clients about the medal that been given.
								if ( NETWORK_GetState( ) == NETSTATE_SERVER )
									SERVERCOMMANDS_GivePlayerMedal( source->player - players, MEDAL_SPAM );

								// Also, cancel out the possibility of getting an Excellent/Incredible medal.
								source->player->ulLastExcellentTick = 0;
								source->player->ulLastFragTick = 0;
							}
							else
								source->player->ulLastBFGFragTick = level.time;
						}

						// If the player has gotten two Excelents within two seconds, award an "Incredible" medal.
						if (( source->player->ulLastExcellentTick + ( 2 * TICRATE )) > level.time )
						{
							// Award the incredible.
							MEDAL_GiveMedal( source->player - players, MEDAL_INCREDIBLE );

							// Tell clients about the medal that been given.
							if ( NETWORK_GetState( ) == NETSTATE_SERVER )
								SERVERCOMMANDS_GivePlayerMedal( source->player - players, MEDAL_INCREDIBLE );

							source->player->ulLastExcellentTick = level.time;
							source->player->ulLastFragTick = level.time;
						}
						// If this player has gotten two frags within two seconds, award an "Excellent" medal.
						else if (( source->player->ulLastFragTick + ( 2 * TICRATE )) > level.time )
						{
							// Award the excellent.
							MEDAL_GiveMedal( source->player - players, MEDAL_EXCELLENT );

							// Tell clients about the medal that been given.
							if ( NETWORK_GetState( ) == NETSTATE_SERVER )
								SERVERCOMMANDS_GivePlayerMedal( source->player - players, MEDAL_EXCELLENT );

							source->player->ulLastExcellentTick = level.time;
							source->player->ulLastFragTick = level.time;
						}
					}

					// Bookmark the last time they got a frag.
					source->player->ulLastFragTick = level.time;

					// If the target player had been in the middle of typing, award a "llama" medal.
					// Also do this if the killed player is lagging.
					if ( player->bChatting || player->bLagging )
					{
						MEDAL_GiveMedal( source->player - players, MEDAL_LLAMA );

						// Tell clients about the medal that been given.
						if ( NETWORK_GetState( ) == NETSTATE_SERVER )
							SERVERCOMMANDS_GivePlayerMedal( source->player - players, MEDAL_LLAMA );
					}
				}
			}

			// Play announcer sounds for amount of frags remaining.
			if (( lastmanstanding == false ) && ( teamlms == false ) && ( possession == false ) && ( teampossession == false ) && deathmatch && fraglimit )
			{
				// [RH] Implement fraglimit
				// [BC] Betterized!
				if ( fraglimit <= D_GetFragCount( source->player ))
				{
					if ( NETWORK_GetState( ) == NETSTATE_SERVER )
					{
						SERVER_Printf( PRINT_HIGH, "%s\n", GStrings( "TXT_FRAGLIMIT" ));
						if ( teamplay && ( source->player->bOnTeam ))
							SERVER_Printf( PRINT_HIGH, "%s \\c-wins!\n", TEAM_GetName( source->player->ulTeam ));
						else
							SERVER_Printf( PRINT_HIGH, "%s \\c-wins!\n", source->player->userinfo.netname );
					}
					else
					{
						Printf( "%s\n", GStrings( "TXT_FRAGLIMIT" ));
						if ( teamplay && ( source->player->bOnTeam ))
							Printf( "%s \\c-wins!\n", TEAM_GetName( source->player->ulTeam ));
						else
							Printf( "%s \\c-wins!\n", source->player->userinfo.netname );

						if (( duel == false ) && ( source->player - players == consoleplayer ))
							ANNOUNCER_PlayEntry( cl_announcer, "YouWin" );
					}

					// Pause for five seconds for the win sequence.
					if ( duel )
					{
						// Also, do the win sequence for the player.
						DUEL_SetLoser( player - players );
						DUEL_DoWinSequence( source->player - players );

						// Give the winner a win.
						PLAYER_SetWins( source->player, source->player->ulWins + 1 );

						GAME_SetEndLevelDelay( 5 * TICRATE );
					}
					// End the level after five seconds.
					else
					{
						char				szString[64];
						DHUDMessageFadeOut	*pMsg;

						// Just print "YOU WIN!" in single player.
						if (( NETWORK_GetState( ) == NETSTATE_SINGLE_MULTIPLAYER ) && ( players[consoleplayer].mo->CheckLocalView( source->player - players )))
							sprintf( szString, "\\cGYOU WIN!" );
						else if (( teamplay ) && ( source->player->bOnTeam ))
						{
							if ( source->player->ulTeam == TEAM_BLUE )
								sprintf( szString, "\\cH%s wins!\n", TEAM_GetName( source->player->ulTeam ));
							else
								sprintf( szString, "\\cG%s wins!\n", TEAM_GetName( source->player->ulTeam ));
						}
						else
							sprintf( szString, "%s \\cGWINS!", players[source->player - players].userinfo.netname );
						V_ColorizeString( szString );

						if ( NETWORK_GetState( ) != NETSTATE_SERVER )
						{
							screen->SetFont( BigFont );

							// Display "%s WINS!" HUD message.
							pMsg = new DHUDMessageFadeOut( szString,
								160.4f,
								75.0f,
								320,
								200,
								CR_WHITE,
								3.0f,
								2.0f );

							StatusBar->AttachMessage( pMsg, 'CNTR' );
							screen->SetFont( SmallFont );

							szString[0] = 0;
							pMsg = new DHUDMessageFadeOut( szString,
								0.0f,
								0.0f,
								0,
								0,
								CR_RED,
								3.0f,
								2.0f );

							StatusBar->AttachMessage( pMsg, 'FRAG' );

							pMsg = new DHUDMessageFadeOut( szString,
								0.0f,
								0.0f,
								0,
								0,
								CR_RED,
								3.0f,
								2.0f );

							StatusBar->AttachMessage( pMsg, 'PLAC' );
						}
						else
						{
							SERVERCOMMANDS_PrintHUDMessageFadeOut( szString, 160.4f, 75.0f, 320, 200, CR_WHITE, 3.0f, 2.0f, "BigFont", 'CNTR' );
							szString[0] = 0;
							SERVERCOMMANDS_PrintHUDMessageFadeOut( szString, 0.0f, 0.0f, 0, 0, CR_WHITE, 3.0f, 2.0f, "BigFont", 'FRAG' );
							SERVERCOMMANDS_PrintHUDMessageFadeOut( szString, 0.0f, 0.0f, 0, 0, CR_WHITE, 3.0f, 2.0f, "BigFont", 'PLAC' );
						}

						GAME_SetEndLevelDelay( 5 * TICRATE );
					}
				}
			}
		}

		// If the player got telefragged by a player trying to spawn, allow him to respawn.
		if (( player ) && ( lastmanstanding || teamlms || survival ) && ( MeansOfDeath == MOD_SPAWNTELEFRAG ))
			player->bSpawnTelefragged = true;
	}
	else if ( ( NETWORK_GetState( ) != NETSTATE_CLIENT ) && (CountsAsKill()))
	{

		// count all monster deaths,
		// even those caused by other monsters

			//GHK lets deal with monster infighting & other death bonuses.
		if ((source==NULL || source->player==NULL)){
		//int leaderScore2=level.info->leader_score;
		//int playerScore=0;
		int BonusPoints=(GetDefault()->health)/10;

		BonusPoints=BonusPoints>0?BonusPoints:1; //compensate for creatures with lower than 10 HP
		//int ptsToAdd=0;
		//float fPtsToAdd=0.0;
		//int ptsToAddmod=0;
		char *msg;
		char* bonusmsg = new char[35];

		if((source!=NULL)&&(source!=this)){
			msg=" Infighting Bonus\n";
		}else{
			msg=" Demon Suicide Bonus\n";
		}


		if( teamcoop || cooperative )
		{
			//int leaderScore2=level.info->leader_score;

			ULONG ulIdx;
			ULONG playersingame = 0;
			// need to make sure that there is at least one player in game
			// or the code below will crash server due to infighting after all
			// players exit the game

				for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
				{
					if ( (playeringame[ulIdx] == true) && (players[ulIdx].mo!=NULL) && (PLAYER_IsTrueSpectator( &players[ulIdx] ) == false) ){
						playersingame=1;
						break;
					}
				}

			int leaderScore2=0;
			if(playersingame){
				scoreboard_SortPlayers(); //get sort on total count of all kills, items, secrets, barrels for players

				for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
				{
					if ( (playeringame[g_iSortedPlayers[ ulIdx ]] == true) && (players[g_iSortedPlayers[ ulIdx ]].mo!=NULL) && (PLAYER_IsTrueSpectator( &players[g_iSortedPlayers[ ulIdx ]] ) == false) ){
						leaderScore2=players[ g_iSortedPlayers[ ulIdx ] ].mo->player->killcount + players[ g_iSortedPlayers[ ulIdx ] ].mo->player->itemcount + players[ g_iSortedPlayers[ ulIdx ] ].mo->player->secretcount + players[ g_iSortedPlayers[ ulIdx ] ].mo->player->barrelcount;
						break;
					}
				}


			}
			int playerScore=0;
			int ptsToAdd=0;
			float fPtsToAdd=0.0;
			int ptsToAddmod=0;

			leaderScore2=leaderScore2>0?leaderScore2:1; //GHK, if 0 make it 1.

			//GHK:
			//USED INSTEAD: Iignoring uclient?



// Update each player's points.
			//ULONG ulIdx;
		int finalLeader = -1; //used for determining who won if pointlimit was hit during infighting.
		bool tied = false; //used for determine if tied, if so, let the game go on.
		ULONG totalplayersingame=0; //also need to compensate for when there is only 1 player!
		for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
		{
			if ( (playeringame[ulIdx] == false) || (players[ulIdx].mo==NULL) || (PLAYER_IsTrueSpectator( &players[ulIdx] )) )
				continue;

				totalplayersingame++;
					playerScore=players[ ulIdx ].mo->player->killcount + players[ ulIdx ].mo->player->itemcount + players[ ulIdx ].mo->player->secretcount + players[ ulIdx ].mo->player->barrelcount;
						ptsToAdd=0;

						playerScore=(playerScore>0?playerScore:1); //GHK, if 0 make it 1, give at least 1 point.

						fPtsToAdd=(float(playerScore)/float(leaderScore2))*float(BonusPoints);
						ptsToAdd=int(fPtsToAdd); //Get the LHS of the decimal place.
						//int ptsToAdd2=ptsToAdd;

						//Get the RHS of the decimal place to check to see if we need
						//to round up.
						ptsToAddmod=(int(fPtsToAdd*10.0)%10)>=5?1:0;

						//Never let the runner up have as many bonus points as the leader!
						if(ptsToAddmod==BonusPoints&&leaderScore2>playerScore)
								ptsToAddmod-=1;

						ptsToAdd+=ptsToAddmod;
												//Give at least 1 point? okay.
						ptsToAdd=ptsToAdd>0?ptsToAdd:1;

						players[ ulIdx ].mo->player->lPointCount+=ptsToAdd;

						if(finalLeader==-1)
							finalLeader=ulIdx;
						else if(players[ ulIdx ].mo->player->lPointCount > players[ finalLeader ].mo->player->lPointCount ){
							finalLeader=ulIdx;
							tied=false;
						}
						else if (players[ ulIdx ].mo->player->lPointCount == players[ finalLeader ].mo->player->lPointCount)
							tied=true;

				//SERVERCOMMANDS_SetPlayerPoints( ulIdx, ulClient, SVCF_ONLYTHISCLIENT );


					if(teamcoop&&players[ ulIdx ].mo->player->bOnTeam)
						TEAM_SetScore( players[ ulIdx ].mo->player->ulTeam, TEAM_GetScore(players[ ulIdx ].mo->player->ulTeam ) + players[ ulIdx ].mo->player->lPointCount, true );


				if(( NETWORK_GetState( ) == NETSTATE_SERVER ) && ptsToAdd > 0 ){

						SERVERCOMMANDS_SetPlayerPoints( ULONG(players[ ulIdx ].mo->player - players ) );

						SERVERCONSOLE_UpdatePlayerInfo( ULONG( players[ ulIdx].mo->player - players ), UDF_FRAGS );
						SERVERCONSOLE_UpdateScoreboard( );
						//SERVERCOMMANDS_CheckPointLimit2(  players[ ulIdx ].mo->player );
						//Printf(TEXTCOLOR_GOLD"Objective Reached! +100 Bonus Points!\n");
						sprintf(bonusmsg,TEXTCOLOR_GREEN"%s%d"TEXTCOLOR_WHITE"%s","+ ",ptsToAdd,msg);
						Printf (PRINT_MEDIUM, "%s", bonusmsg);

						SERVERCOMMANDS_Print(bonusmsg,PRINT_MEDIUM, players[ ulIdx ].mo->player - players, SVCF_ONLYTHISCLIENT);
				}

		}

		//check for any pointlimit winners, if pointlimit met/beaten, but tied, keep playing

		if(( NETWORK_GetState( ) == NETSTATE_SERVER ) ){

			if(totalplayersingame>1){
				if(!tied&& finalLeader > -1){ //shouldnt need another .mo check here.
					SERVERCOMMANDS_CheckPointLimit(  players[ finalLeader ].mo->player );
				}
			}else if(totalplayersingame==1&&finalLeader > -1){
					SERVERCOMMANDS_CheckPointLimit(  players[ finalLeader ].mo->player );
			}

		}

		/*
		//ghk, ugh, Points have been added, now need to sort based on points
		// in order to find out if the point limit has been met. The highest
		//scorer (in the case of more than 1 player meeting the point limit
		//is the winner.
		if(( NETWORK_GetState( ) == NETSTATE_SERVER )) {
			scoreboard_SortPlayersByPoints( );


			for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
			{

				// Skip players that aren't playing.
				if ( playeringame[g_iSortedPlayers[ulIdx]] == false && players[g_iSortedPlayers[ulIdx]].mo==NULL )
					continue;

				// Skip spectating players, or players not on a team.
				if ( PLAYER_IsTrueSpectator( &players[g_iSortedPlayers[ulIdx]] ) == false )
					continue;

				if(GAME_GetEndLevelDelay())
					continue; //ignore other point limit checks, since its been hit already.

				SERVERCOMMANDS_CheckPointLimit(  players[ g_iSortedPlayers[ulIdx] ].mo->player );

			}
		}
		*/

/*
		if ( NETWORK_GetState( ) == NETSTATE_SERVER )
	{
		for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
		{
			if ( playeringame[ulIdx] )
				SERVERCONSOLE_UpdatePlayerInfo( ulIdx, UDF_FRAGS );
		}
	}
*/


		}
		}


		// Why give player 0 credit? :P
		// Meh, just do it in single player.
		if ( NETWORK_GetState( ) == NETSTATE_SINGLE )
			players[0].killcount++;  //ghk ignore barrelcounts, since we dont care about singleplayer
		if ( invasion )
		{
			INVASION_SetNumMonstersLeft( INVASION_GetNumMonstersLeft( ) - 1 );

			if ( GetClass( ) == RUNTIME_CLASS( AArchvile ))
				INVASION_SetNumArchVilesLeft( INVASION_GetNumArchVilesLeft( ) - 1 );

			// If we're the server, tell the client how many monsters are left.
			if ( NETWORK_GetState( ) == NETSTATE_SERVER )
				SERVERCOMMANDS_SetInvasionNumMonstersLeft( );
		}

		if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		{
//			SERVER_PlayerKilledMonster( MAXPLAYERS );

			// Also, update the scoreboard.
			SERVERCONSOLE_UpdateScoreboard( );
		}
	}
	// [BC] Don't do this block in client mode.
	if (player && ( NETWORK_GetState( ) != NETSTATE_CLIENT ))
	{
		// [BC] If this is a bot, tell it it died.
		if ( player->pSkullBot )
		{
			if ( source && source->player )
				player->pSkullBot->m_ulPlayerKilledBy = source->player - players;
			else
				player->pSkullBot->m_ulPlayerKilledBy = MAXPLAYERS;

			if ( source && source->player )
			{
				if ( source->player == player )
					player->pSkullBot->PostEvent( BOTEVENT_KILLED_BYSELF );
				else if (( source->player - players ) == player->pSkullBot->m_ulPlayerEnemy )
					player->pSkullBot->PostEvent( BOTEVENT_KILLED_BYENEMY );
				else
					player->pSkullBot->PostEvent( BOTEVENT_KILLED_BYPLAYER );
			}
			else
				player->pSkullBot->PostEvent( BOTEVENT_KILLED_BYENVIORNMENT );
		}

		// [BC] Keep track of where we died for the "same spot respawn" dmflags.
		player->SpawnX = x;
		player->SpawnY = y;
		player->SpawnAngle = angle;
		player->bSpawnOkay = true;

		// Death script execution, care of Skull Tag
		FBehavior::StaticStartTypedScripts (SCRIPT_Death, this, true);

		// [RH] Force a delay between death and respawn
		if (( i_compatflags & COMPATF_INSTANTRESPAWN ) == false )
		{
			player->respawn_time = level.time + TICRATE;

			// [BC] Don't respawn quite so fast on forced respawn. It sounds weird when your
			// scream isn't completed.
			if ( dmflags & DF_FORCE_RESPAWN )
				player->respawn_time += TICRATE/2;
		}

		// count environment kills against you
		if (!source)
		{
			PLAYER_SetFragcount( player, player->fragcount - (( bPossessedTerminatorArtifact ) ? 10 : 1 ), true, true );	// [RH] Cumulative frag count

			// Spawning in nukage or getting crushed is NOT
			// somewhere where you would want to be at when you respawn again
			player->bSpawnOkay = false;
		}

		// [BC] Increment deathcount.
		if (
			((( duel ) && ( DUEL_GetState( ) == DS_COUNTDOWN )) == false ) &&
			((( lastmanstanding || teamlms ) && ( LASTMANSTANDING_GetState( ) == LMSS_COUNTDOWN )) == false )
			)
		{
			player->ulDeathCount++;

			// Increment deaths without frag.
			player->ulDeathsWithoutFrag++;

			// If the player dies TEN times without getting a frag, award a "Your skill is not enough" medal.
			if (( player->ulDeathsWithoutFrag % 10 ) == 0 )
			{
				MEDAL_GiveMedal( player - players, MEDAL_YOURSKILLISNOTENOUGH );

				// Tell clients about the medal that been given.
				if ( NETWORK_GetState( ) == NETSTATE_SERVER )
					SERVERCOMMANDS_GivePlayerMedal( player - players, MEDAL_YOURSKILLISNOTENOUGH );
			}
			// If the player dies five times without getting a frag, award a "You fail it" medal.
			else if (( player->ulDeathsWithoutFrag % 5 ) == 0 )
			{
				MEDAL_GiveMedal( player - players, MEDAL_YOUFAILIT );

				// Tell clients about the medal that been given.
				if ( NETWORK_GetState( ) == NETSTATE_SERVER )
					SERVERCOMMANDS_GivePlayerMedal( player - players, MEDAL_YOUFAILIT );
			}

			// Reset frags without death.
			player->ulFragsWithoutDeath = 0;
		}

		// [BC] Increment team deathcount.
		if (( teamplay || teampossession || teamcoop ) && ( player->bOnTeam ))
			TEAM_SetDeathCount( player->ulTeam, TEAM_GetDeathCount( player->ulTeam ) + 1 );
	}

	// [BC] We can do this stuff in client mode.
	if (player)
	{
		flags &= ~MF_SOLID;
		player->playerstate = PST_DEAD;

		P_DropWeapon (player);

		if (this == players[consoleplayer].camera && automapactive)
		{
			// don't die in auto map, switch view prior to dying
			AM_Stop ();
		}

		// [GRB] Clear extralight. When you killed yourself with weapon that
		// called A_Light1/2 before it called A_Light0, extraligh remained.
		player->extralight = 0;
	}

	// [RH] If this is the unmorphed version of another monster, destroy this
	// actor, because the morphed version is the one that will stick around in
	// the level.
	if (flags & MF_UNMORPHED)
	{
		Destroy ();
		return;
	}

	if (DamageType == MOD_DISINTEGRATE && EDeathState)
	{ // Electrocution death
		SetState (EDeathState);
	}
	else if (DamageType == MOD_FIRE && BDeathState)
	{ // Burn death
		SetState (BDeathState);
	}
	else if (DamageType == MOD_ICE &&
		(IDeathState || (
		(!deh.NoAutofreeze && !(flags4 & MF4_NOICEDEATH)) &&
		(player || (flags3 & MF3_ISMONSTER)))))
	{ // Ice death
		if (IDeathState)
		{
			SetState (IDeathState);
		}
		else
		{
			SetState (&AActor::States[S_GENERICFREEZEDEATH]);
		}
	}
	else
	{
		int flags4 = !inflictor ? 0 : inflictor->player && inflictor->player->ReadyWeapon ?
			inflictor->player->ReadyWeapon->flags4 : inflictor->flags4;

		int gibhealth = -abs(GetClass()->Meta.GetMetaInt (AMETA_GibHealth,
			gameinfo.gametype == GAME_Doom ? -GetDefault()->health : -GetDefault()->health/2));

		DamageType = MOD_UNKNOWN;	// [RH] "Frozen" barrels shouldn't do freezing damage
		if (XDeathState && (health<gibhealth || flags4 & MF4_EXTREMEDEATH) && !(flags4 & MF4_NOEXTREMEDEATH))
		{ // Extreme death
			SetState (XDeathState);
		}
		else
		{ // Normal death
			DamageType = MOD_UNKNOWN;	// [RH] "Frozen" barrels shouldn't do freezing damage
			if (DeathState != NULL)		// [RH] DeathState might be NULL, so try others as needed
			{
				SetState (DeathState);
			}
			else if (EDeathState != NULL)
			{
				SetState (EDeathState);
			}
			else if (BDeathState != NULL)
			{
				SetState (BDeathState);
			}
			else if (IDeathState != NULL)
			{
				SetState (IDeathState);
			}
			else
			{
				Destroy();
			}
		}
	}

	// [BC] Tell clients that this thing died.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
	{
		// If there isn't a player attached to this object, treat it like a normal thing.
		if ( player == NULL )
			SERVERCOMMANDS_KillThing( this );
		else
			SERVERCOMMANDS_KillPlayer( ULONG( player - players ), source, inflictor, MeansOfDeath );
	}

	if (( deathmatch || teamgame ) &&
		( NETWORK_GetState( ) != NETSTATE_SERVER ) &&
		( NETWORK_GetState( ) != NETSTATE_CLIENT ) &&
		( cl_showlargefragmessages ) &&
		((( duel ) && (( DUEL_GetState( ) == DS_WINSEQUENCE ) || ( DUEL_GetState( ) == DS_COUNTDOWN ))) == false ) &&
		((( lastmanstanding || teamlms ) && (( LASTMANSTANDING_GetState( ) == LMSS_WINSEQUENCE ) || ( LASTMANSTANDING_GetState( ) == LMSS_COUNTDOWN ))) == false ))
	{
		if (( player ) && ( source ) && ( source->player ) && ( player != source->player ) && ( MeansOfDeath != MOD_SPAWNTELEFRAG ))
		{
			if ((( deathmatch == false ) || (( fraglimit == 0 ) || ( source->player->fragcount < fraglimit ))) &&
				(( lastmanstanding == false ) || (( winlimit == 0 ) || ( source->player->ulWins < winlimit ))) &&
				(( teamlms == false ) || (( winlimit == 0 ) || ( TEAM_GetWinCount( source->player->ulTeam ) < winlimit ))))
			{
				// Display a large "You were fragged by <name>." message in the middle of the screen.
				if (( player - players ) == consoleplayer )
					SCOREBOARD_DisplayFraggedMessage( source->player );

				// Display a large "You fragged <name>!" message in the middle of the screen.
				else if (( source->player - players ) == consoleplayer )
					SCOREBOARD_DisplayFragMessage( player );
			}
		}
	}

	// [RH] Death messages
	if (( player ) && ( NETWORK_GetState( ) != NETSTATE_CLIENT ))
		ClientObituary (this, inflictor, source, MeansOfDeath);

	tics -= pr_killmobj() & 3;
	if (tics < 1)
		tics = 1;
}




//---------------------------------------------------------------------------
//
// PROC P_AutoUseHealth
//
//---------------------------------------------------------------------------

void P_AutoUseHealth(player_t *player, int saveHealth)
{
	int i;
	int count;
	const PClass *normalType = PClass::FindClass (NAME_ArtiHealth);
	const PClass *superType = PClass::FindClass (NAME_ArtiSuperHealth);
	AInventory *normalItem = player->mo->FindInventory (normalType);
	AInventory *superItem = player->mo->FindInventory (superType);
	int normalAmount, superAmount;

	normalAmount = normalItem != NULL ? normalItem->Amount : 0;
	superAmount = superItem != NULL ? superItem->Amount : 0;

	if ((gameskill == sk_baby) && (normalAmount*25 >= saveHealth))
	{ // Use quartz flasks
		count = (saveHealth+24)/25;
		for(i = 0; i < count; i++)
		{
			player->health += 25;
			if (--normalItem->Amount == 0)
			{
				normalItem->Destroy ();
				break;
			}
		}
	}
	else if (superAmount*100 >= saveHealth)
	{ // Use mystic urns
		count = (saveHealth+99)/100;
		for(i = 0; i < count; i++)
		{
			player->health += 100;
			if (--superItem->Amount == 0)
			{
				superItem->Destroy ();
				break;
			}
		}
	}
	else if ((gameskill == sk_baby)
		&& (superAmount*100+normalAmount*25 >= saveHealth))
	{ // Use mystic urns and quartz flasks
		count = (saveHealth+24)/25;
		saveHealth -= count*25;
		for(i = 0; i < count; i++)
		{
			player->health += 25;
			if (--normalItem->Amount == 0)
			{
				normalItem->Destroy ();
				break;
			}
		}
		count = (saveHealth+99)/100;
		for(i = 0; i < count; i++)
		{
			player->health += 100;
			if (--superItem->Amount == 0)
			{
				superItem->Destroy ();
				break;
			}
		}
	}
	player->mo->health = player->health;
}

//============================================================================
//
// P_AutoUseStrifeHealth
//
//============================================================================

void P_AutoUseStrifeHealth (player_t *player)
{
	static const ENamedName healthnames[2] = { NAME_MedicalKit, NAME_MedPatch };

	for (int i = 0; i < 2; ++i)
	{
		const PClass *type = PClass::FindClass (healthnames[i]);

		while (player->health < 50)
		{
			AInventory *item = player->mo->FindInventory (type);
			if (item == NULL)
				break;
			if (!player->mo->UseInventory (item))
				break;
		}
	}
}

/*
=================
=
= P_DamageMobj
=
= Damages both enemies and players
= inflictor is the thing that caused the damage
= 		creature or missile, can be NULL (slime, etc)
= source is the thing to target after taking damage
=		creature or NULL
= Source and inflictor are the same for melee attacks
= source can be null for barrel explosions and other environmental stuff
==================
*/

int MeansOfDeath;

void P_DamageMobj (AActor *target, AActor *inflictor, AActor *source, int damage, int mod, int flags)
{
	unsigned ang;
	player_t *player;
	fixed_t thrust;
	int temp;

	//GHK: added for score calculations when player is giving out damage
	player_t *playerGHK;
	char levelscore[10];
	int oldhealth;
	oldhealth=0;


	// [BC] Game is currently in a suspended state; don't hurt anyone.
	if ( GAME_GetEndLevelDelay( ))
		return;

	if (target == NULL || !(target->flags & MF_SHOOTABLE))
	{ // Shouldn't happen
		return;
	}

	// Spectral targets only take damage from spectral projectiles.
	if (target->flags4 & MF4_SPECTRAL && damage < 1000000)
	{
		if (inflictor == NULL || !(inflictor->flags4 & MF4_SPECTRAL))
		{
			/*
			if (target->MissileState != NULL)
			{
				target->SetState (target->MissileState);
			}
			*/
			return;
		}
	}
	if (target->health <= 0)
	{
		if (inflictor && mod == MOD_ICE)
		{
			return;
		}
		else if (target->flags & MF_ICECORPSE) // frozen
		{
			target->tics = 1;
			target->momx = target->momy = target->momz = 0;
		}
		return;
	}
	if ((target->flags2 & MF2_INVULNERABLE) && damage < 1000000)
	{ // actor is invulnerable
		if (!target->player)
		{
			if (!inflictor || !(inflictor->flags3 & MF3_FOILINVUL))
			{
				return;
			}
		}
		else
		{
			// Only in Hexen invulnerable players are excluded from getting
			// thrust by damage.
			if (gameinfo.gametype == GAME_Hexen) return;
		}

	}
	if (inflictor != NULL)
	{
		if (inflictor->flags5 & MF5_PIERCEARMOR) flags |= DMG_NO_ARMOR;
	}

	MeansOfDeath = mod;
	// [RH] Andy Baker's Stealth monsters
	if (target->flags & MF_STEALTH)
	{
		target->alpha = OPAQUE;
		target->visdir = -1;
	}
	if (target->flags & MF_SKULLFLY)
	{
		target->momx = target->momy = target->momz = 0;
	}
	if (target->flags2 & MF2_DORMANT)
	{
		// Invulnerable, and won't wake up
		return;
	}
	player = target->player;
	if (player && gameskill == sk_baby)
	{
		// Take half damage in trainer mode
		if (damage > 1)
		{
			damage >>= 1;
		}
	}
	// Special damage types
	if (inflictor)
	{
		if (inflictor->flags4 & MF4_SPECTRAL)
		{
			if (player != NULL)
			{
				if (inflictor->health == -1)
					return;
			}
			else if (target->flags4 & MF4_SPECTRAL)
			{
				if (inflictor->health == -2)
					return;
			}
		}
		if (mod == MOD_FIRE && target->flags4 & MF4_FIRERESIST)
		{
			damage /= 2;
		}
		damage = inflictor->DoSpecialDamage (target, damage);
		if (damage == -1)
		{
			return;
		}
		//GHK add quaddamage & double damage multipliers here for
		//inflictor!=source for Player: I.E. rockets, plasmaballs etc...
		if(source && source->player && (source != inflictor)){
			damage = source->DoSpecialDamage (target, damage);

		}
	}
	damage = target->TakeSpecialDamage (inflictor, source, damage, mod);
	if (damage == -1)
	{
		return;
	}

	// [BC] If the target player has the reflection rune, damage the source with 50% of the
	// this player is being damaged with.
	if ( target->player && ( target->player->Powers & PW_REFLECTION ) && source && ( mod != MOD_REFLECTION ) && ( NETWORK_GetState( ) != NETSTATE_CLIENT ))
	{
		if ( target != source )
		{
			//GHK, in SDST, lets do full damage
			//P_DamageMobj( source, NULL, target, (( damage * 3 ) / 4 ), MOD_REFLECTION );
			P_DamageMobj( source, NULL, target, ( damage * 4), MOD_REFLECTION );


			// Reset means of death flag.
			MeansOfDeath = mod;
		}
	}

	// Push the target unless the source's weapon's kickback is 0.
	// (i.e. Guantlets/Chainsaw)
	if (inflictor && inflictor != target	// [RH] Not if hurting own self
		&& !(target->flags & MF_NOCLIP)
		&& !(inflictor->flags2 & MF2_NODMGTHRUST))
	{
		int kickback;

		if (!source || !source->player || !source->player->ReadyWeapon)
			kickback = gameinfo.defKickback;
		else
			kickback = source->player->ReadyWeapon->Kickback;

		if (kickback)
		{
			ang = R_PointToAngle2 (inflictor->x, inflictor->y,
				target->x, target->y);
			thrust = damage*(FRACUNIT>>3)*kickback / target->Mass;
			// [RH] If thrust overflows, use a more reasonable amount
			if (thrust < 0 || thrust > 10*FRACUNIT)
			{
				thrust = 10*FRACUNIT;
			}
			// make fall forwards sometimes
			if ((damage < 40) && (damage > target->health)
				 && (target->z - inflictor->z > 64*FRACUNIT)
				 && (pr_damagemobj()&1)
				 // [RH] But only if not too fast and not flying
				 && thrust < 10*FRACUNIT
				 && !(target->flags & MF_NOGRAVITY))
			{
				ang += ANG180;
				thrust *= 4;
			}
			ang >>= ANGLETOFINESHIFT;
			if (source && source->player && (source == inflictor)
				&& source->player->ReadyWeapon != NULL &&
				(source->player->ReadyWeapon->WeaponFlags & WIF_STAFF2_KICKBACK))
			{
				// Staff power level 2
				target->momx += FixedMul (10*FRACUNIT, finecosine[ang]);
				target->momy += FixedMul (10*FRACUNIT, finesine[ang]);
				if (!(target->flags & MF_NOGRAVITY))
				{
					target->momz += 5*FRACUNIT;
				}
			}
			else
			{
				target->momx += FixedMul (thrust, finecosine[ang]);
				target->momy += FixedMul (thrust, finesine[ang]);
			}
		}
	}

	//
	// player specific
	//
	if (player)
	{
		if ((target->flags2 & MF2_INVULNERABLE) && damage < 1000000)
		{ // player is invulnerable, so don't hurt him
			return;
		}

		// end of game hell hack
		if ((target->Sector->special & 255) == dDamage_End
			&& damage >= target->health)
		{
			damage = target->health - 1;
		}

		if (damage < 1000 && ((target->player->cheats & CF_GODMODE)
			|| (target->player->mo->flags2 & MF2_INVULNERABLE)))
		{
			return;
		}

		// [RH] Avoid friendly fire if enabled
		if (source != NULL && player != source->player && target->IsTeammate (source))
		{
			if ((( teamlms || survival ) && ( MeansOfDeath == MOD_SPAWNTELEFRAG )) == false )
				MeansOfDeath |= MOD_FRIENDLY_FIRE;
			if (damage < 1000000)
			{ // Still allow telefragging :-(
				damage = (int)((float)damage * teamdamage);
				if (damage <= 0)
					return;
			}
		}else if(source != NULL && source->tracer != NULL){ //GHK Added 'else if' check for damage from minotaur friend & Sentry Turret
			//GHK, Need an extra check for source->tracer->player->mo!=NULL below??
			//IsTeamate already does it.
			if(source->tracer->player != NULL && target->IsTeammate(source->tracer->player->mo)){
				 MeansOfDeath |= MOD_FRIENDLY_FIRE; //FriendlyFire = true;
                if (damage < 1000000){ // Still allow telefragging :-(
					damage = (int)((float)damage * teamdamage);
                    if (damage <= 0)
                          return;
                    }
			}
		}


		if (!(flags & DMG_NO_ARMOR) && player->mo->Inventory != NULL)
		{
			int newdam = damage;
			player->mo->Inventory->AbsorbDamage (damage, mod, newdam);
			damage = newdam;
			if (damage <= 0)
			{
				return;
			}
		}

		// [BC] For the red armor's special fire resistance, potentially reduce the amount
		// of damage taken AFTER the player's armor has been depleted.
		if (( player->Powers & PW_FIRERESISTANT ) &&
			(( mod == MOD_FIRE ) ||
			( mod == MOD_GRENADE ) ||
			( mod == MOD_ROCKET )))
		{
			damage /= 8;
		}

		if (damage >= player->health
			&& ((gameskill == sk_baby) || deathmatch)
			&& !player->morphTics)
		{ // Try to use some inventory health
			P_AutoUseHealth (player, damage - player->health + 1);
		}
		player->health -= damage;		// mirror mobj health here for Dave
		// [RH] Make voodoo dolls and real players record the same health
		target->health = player->mo->health -= damage;
		if (player->health < 50 && !deathmatch)
		{
			P_AutoUseStrifeHealth (player);
			player->mo->health = player->health;
		}
		if (player->health < 0)
		{
			player->health = 0;
		}

		if ( player->pSkullBot )
		{
			// Tell the bot he's been damaged by a player.
			if ( source && source->player && ( source->player != player ))
			{
				player->pSkullBot->m_ulLastPlayerDamagedBy = source->player - players;
				player->pSkullBot->PostEvent( BOTEVENT_DAMAGEDBY_PLAYER );
			}
		}

		player->attacker = source;
		player->damagecount += damage;	// add damage after armor / invuln
		if (player->damagecount > 100)
		{
			player->damagecount = 100;	// teleport stomp does 10k points...
		}
		if ( player->damagecount < 0 )
			player->damagecount = 0;
		temp = damage < 100 ? damage : 100;
		if (player == &players[consoleplayer])
		{
			I_Tactile (40,10,40+temp*2);
		}
	}
	else
	{
		//GHK: OLDHealth for telefragging points.
		oldhealth=target->health;
		target->health -= damage;
	}

	//
	// the damage has been dealt; now deal with the consequences
	//

		//GHK: calculate player points based on damage given out.

	if(source!=NULL&&source->player!=NULL){
		playerGHK=source->player;

	}else if (inflictor!=NULL&&inflictor->player!=NULL){
		playerGHK=inflictor->player;

	}else{
		//ghk: for minotaurfiend damage
		if (source!=NULL&&source->tracer!=NULL&&source->tracer->player != NULL){
			playerGHK=source->tracer->player;
		}else{
			playerGHK=NULL;
		}
	}


	bool scoredPoints = false; //ghk for quad damage sounds.

	int realdamage=damage;
	if( ( teamcoop || cooperative) && ( NETWORK_GetState( ) != NETSTATE_CLIENT ) ){
		if(playerGHK!=NULL && target->CountsAsKill()){

			if(damage>(target->GetDefault()->health)){
						   //telefragging ...
						   //get remaining health of the creature.

				if(oldhealth<0)
					oldhealth=0;

					playerGHK->lPointCount+=oldhealth;
					scoredPoints=true;

					realdamage=oldhealth;

			}else{
						   //normal..
						   //playerGHK->level_score+=damage;
						   //scoredPoints=true;

				if(damage>oldhealth)
					realdamage=oldhealth;

					playerGHK->lPointCount+=realdamage;
					scoredPoints=true;

			 }


			if(true){ //GHK: sd-altconsole fix for sshotty lag
				if(target->lastpnumdmg==playerGHK->mo->id){
					 playerGHK->mo->multidmgbuffer+=realdamage; //if repeated damage per tic, queue it up
														//gets called on player's aactor::tick call


				}else{
					 //regular behaviour
					  if(( NETWORK_GetState( ) == NETSTATE_SERVER )&& realdamage > 0){
							SERVERCOMMANDS_SetPlayerPoints( playerGHK - players );
							// Also, update the scoreboard.
							SERVERCONSOLE_UpdatePlayerInfo( ULONG( playerGHK - players ), UDF_FRAGS );
							SERVERCONSOLE_UpdateScoreboard( );
							SERVERCOMMANDS_CheckPointLimit(  playerGHK  );

							sprintf(levelscore,"+ %d\n",realdamage);
							SERVERCOMMANDS_Print(levelscore,PRINT_MEDIUM, playerGHK - players, SVCF_ONLYTHISCLIENT);



					  }

				 }
				//set this after calculations
				target->lastpnumdmg=playerGHK->mo->id;
			  }else{ //*never gets here*
				if(playerGHK->mo->CheckLocalView(consoleplayer)&&realdamage>0){
					sprintf(levelscore,"+ %d\n",realdamage);
					Printf (PRINT_MEDIUM, "%s\n", levelscore);
				}

			 }



		}else{ //GHK newly added else clause FOR BOSS BRAIN

				if(playerGHK!=NULL&&!target->player&&target->GetClass()== RUNTIME_CLASS(ABossBrain)){//boss brain damage, CountsAsKill() not set for it.
					if(damage>(target->GetDefault()->health)){
							//telefragging ...
							//get remaining health of the creature.
							if(oldhealth<0)
								oldhealth=0;

							playerGHK->lPointCount+=oldhealth;
							scoredPoints=true;
							if(( NETWORK_GetState( ) == NETSTATE_SERVER )){
								SERVERCOMMANDS_SetPlayerPoints( playerGHK - players );
								// Also, update the scoreboard.
								SERVERCONSOLE_UpdatePlayerInfo( ULONG( playerGHK - players ), UDF_FRAGS );
								SERVERCONSOLE_UpdateScoreboard( );
								SERVERCOMMANDS_CheckPointLimit(  playerGHK  );
								sprintf(levelscore,"+ %d\n",oldhealth);
								SERVERCOMMANDS_Print(levelscore,PRINT_MEDIUM, playerGHK - players, SVCF_ONLYTHISCLIENT);

							}


					}else{
							//normal..
							playerGHK->lPointCount+=damage;
							scoredPoints=true;

							if( ( NETWORK_GetState( ) == NETSTATE_SERVER )&&damage>0){
								SERVERCOMMANDS_SetPlayerPoints( playerGHK - players );
								// Also, update the scoreboard.
								SERVERCONSOLE_UpdatePlayerInfo( ULONG( playerGHK - players ), UDF_FRAGS );
								SERVERCONSOLE_UpdateScoreboard( );
								sprintf(levelscore,"+ %d\n",damage);
								SERVERCOMMANDS_Print(levelscore,PRINT_MEDIUM, playerGHK - players, SVCF_ONLYTHISCLIENT);

							}
						}
				}


		}
		if(scoredPoints&&teamcoop&&player->bOnTeam)
						TEAM_SetScore( playerGHK->ulTeam, TEAM_GetScore(playerGHK->ulTeam ) + playerGHK->lPointCount, true );

	}
	if(scoredPoints)
		SCOREBOARD_RefreshHUD( );

	// If the damaging player has the power of drain, give the player 50% of the damage
	// done in health.
	if ( source && source->player && ( source->player->Powers & PW_DRAIN ) && ( NETWORK_GetState( ) != NETSTATE_CLIENT ))
	{
		if (( target->player == false ) || ( target->player != source->player ))
		{
			if ( P_GiveBody( source, damage / 2 )&&scoredPoints)
			{
				// [BC] If we're the server, send out the health change, and play the
				// health sound.
				if ( NETWORK_GetState( ) == NETSTATE_SERVER )
				{
					SERVERCOMMANDS_SetPlayerHealth( source->player - players );
					SERVERCOMMANDS_SoundActor( source, CHAN_ITEM, "ghk/qdrain", 127, ATTN_NORM );
				}
				//was: "misc/i_pkup"

				S_Sound( source, CHAN_ITEM, "ghk/qdrain", 1, ATTN_NORM );
			}
		}
	}
	//GHK SAME FOR QUAD/DOUBLE damage
	if ( source && source->player && ( source->player->Powers & (PW_QUADDAMAGE|PW_DOUBLEDAMAGE) ) && ( NETWORK_GetState( ) != NETSTATE_CLIENT ))
	{
		if (( target->player == false ) || ( target->player != source->player ))
		{

				// [BC] If we're the server, send out the health change, and play the
				// health sound.
				if ( NETWORK_GetState( ) == NETSTATE_SERVER )
				{
					//SERVERCOMMANDS_SetPlayerHealth( source->player - players );
					SERVERCOMMANDS_SoundActor( source, CHAN_ITEM, "ghk/qdam", 127, ATTN_NORM );
				}

				S_Sound( source, CHAN_ITEM, "ghk/qdam", 1, ATTN_NORM );

		}
	}

	// [BC] Tell clients that this thing was damaged.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
	{
		if ( player )
			SERVERCOMMANDS_DamagePlayer( ULONG( player - players ));
		else
			// Is this even necessary?
			SERVERCOMMANDS_DamageThing( target );
	}

	if (target->health <= 0)
	{ // Death
		target->special1 = damage;
		if (source && target->IsKindOf (RUNTIME_CLASS(AExplosiveBarrel))
			&& !source->IsKindOf (RUNTIME_CLASS(AExplosiveBarrel)))
		{ // Make sure players get frags for chain-reaction kills
			target->target = source;
		}
		// check for special fire damage or ice damage deaths
		if (mod == MOD_FIRE)
		{
			if (player && !player->morphTics)
			{ // Check for flame death
				if (!inflictor ||
					((target->health > -50) && (damage > 25)) ||
					!inflictor->IsKindOf (RUNTIME_CLASS(APhoenixFX1)))
				{
					target->DamageType = MOD_FIRE;
				}
			}
			else
			{
				target->DamageType = MOD_FIRE;
			}
		}
		else
		{
			target->DamageType = mod;
		}
		if (source && source->tracer && source->IsKindOf (RUNTIME_CLASS (AMinotaur)))
		{ // Minotaur's kills go to his master
			// Make sure still alive and not a pointer to fighter head
			if (source->tracer->player && (source->tracer->player->mo == source->tracer))
			{
				source = source->tracer;
			}
		}
		if (source && (source->player) && source->player->ReadyWeapon != NULL &&
			(source->player->ReadyWeapon->WeaponFlags & WIF_EXTREME_DEATH))
		{
			// Always extreme death from fourth weapon
			target->health = -target->GetDefault()->health * 3;
		}

		// Deaths are server side.
		if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
			target->Die (source, inflictor);
		return;
	}
	if (target->WoundState != NULL)
	{
		int woundhealth = RUNTIME_TYPE(target)->Meta.GetMetaInt (AMETA_WoundHealth, 6);

		if (target->health <= woundhealth)
		{
			target->SetState (target->WoundState);
			return;
		}
	}
	if ((pr_damagemobj() < target->PainChance) && target->PainState != NULL
		 && !(target->flags & MF_SKULLFLY) &&
		 ( NETWORK_GetState( ) != NETSTATE_CLIENT ))
	{
		if (inflictor && inflictor->IsKindOf (RUNTIME_CLASS(ALightning)))
		{
			if (pr_lightning() < 96)
			{
				target->flags |= MF_JUSTHIT; // fight back!
				if (target->PainState != NULL)
				{
					target->SetState (target->PainState);

					// If we are the server, tell clients about the state change.
					if ( NETWORK_GetState( ) == NETSTATE_SERVER )
						SERVERCOMMANDS_SetThingState( target, STATE_PAIN );
				}
			}
			else
			{ // "electrocute" the target
				target->renderflags |= RF_FULLBRIGHT;
				if ((target->flags3 & MF3_ISMONSTER) && pr_lightning() < 128)
				{
					target->Howl ();
				}
			}
		}
		else
		{
			target->flags |= MF_JUSTHIT; // fight back!
			target->SetState (target->PainState);

			// If we are the server, tell clients about the state change.
			if ( NETWORK_GetState( ) == NETSTATE_SERVER )
				SERVERCOMMANDS_SetThingState( target, STATE_PAIN );

			if (inflictor && inflictor->IsKindOf (RUNTIME_CLASS(APoisonCloud)))
			{
				if ((target->flags3 & MF3_ISMONSTER) && pr_poison() < 128)
				{
					target->Howl ();
				}
			}
		}
	}

	// Nothing more to do!
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	target->reactiontime = 0;			// we're awake now...
	if (source)
	{
		if (source == target->target)
		{
			target->threshold = BASETHRESHOLD;
			if (target->state == target->SpawnState && target->SeeState != NULL)
			{
				target->SetState (target->SeeState);
			}
		}
		else if (source != target->target && target->OkayToSwitchTarget (source))
		{
			// Target actor is not intent on another actor,
			// so make him chase after source

			// killough 2/15/98: remember last enemy, to prevent
			// sleeping early; 2/21/98: Place priority on players

			if (target->lastenemy == NULL ||
				(target->lastenemy->player == NULL && target->TIDtoHate == 0) ||
				target->lastenemy->health <= 0)
			{
				target->lastenemy = target->target; // remember last enemy - killough
			}
			target->target = source;
			target->threshold = BASETHRESHOLD;
			if (target->state == target->SpawnState && target->SeeState != NULL)
			{
				target->SetState (target->SeeState);

				// If we are the server, tell clients about the state change.
				if ( NETWORK_GetState( ) == NETSTATE_SERVER )
					SERVERCOMMANDS_SetThingState( target, STATE_SEE );
			}
		}
	}
}

bool AActor::OkayToSwitchTarget (AActor *other)
{
	if (other == this)
		return false;		// [RH] Don't hate self (can happen when shooting barrels)

	if (!(other->flags & MF_SHOOTABLE))
		return false;		// Don't attack things that can't be hurt

	if ((flags4 & MF4_NOTARGETSWITCH) && target != NULL)
		return false;		// Don't switch target if not allowed

	if ((master != NULL && other->IsA(master->GetClass())) ||		// don't attack your master (or others of its type)
		(other->master != NULL && IsA(other->master->GetClass())))	// don't attack your minion (or those of others of your type)
	{
		if (!IsHostile (other) &&								// allow target switch if other is considered hostile
			(other->tid != TIDtoHate || TIDtoHate == 0) &&		// or has the tid we hate
			other->TIDtoHate == TIDtoHate)						// or has different hate information
		{
			return false;
		}
	}

	if ((other->flags3 & MF3_NOTARGET) &&
		(other->tid != TIDtoHate || TIDtoHate == 0) &&
		!IsHostile (other))
		return false;
	if (threshold != 0 && !(flags4 & MF4_QUICKTORETALIATE))
		return false;
	if (IsFriend (other))
	{ // [RH] Friendlies don't target other friendlies
		return false;
	}
	// [BC] No infighting during invasion mode.
	if ((gameinfo.gametype == GAME_Strife || infighting < 0 || invasion ) &&
		other->player == NULL && !IsHostile (other))
	{
		return false;	// Strife & infighting off: Non-friendlies don't target other non-friendlies
	}
	if (TIDtoHate != 0 && TIDtoHate == other->TIDtoHate)
		return false;		// [RH] Don't target "teammates"
	if (other->player != NULL && (flags4 & MF4_NOHATEPLAYERS))
		return false;		// [RH] Don't target players
	if (target != NULL && target->health > 0 &&
		TIDtoHate != 0 && target->tid == TIDtoHate && pr_switcher() < 128 &&
		P_CheckSight (this, target))
		return false;		// [RH] Don't be too quick to give up things we hate

	return true;
}

//==========================================================================
//
// P_PoisonPlayer - Sets up all data concerning poisoning
//
//==========================================================================

void P_PoisonPlayer (player_t *player, AActor *poisoner, AActor *source, int poison)
{
	// This is handled server side.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	if((player->cheats&CF_GODMODE) || (player->mo->flags2 & MF2_INVULNERABLE))
	{
		return;
	}
	if (source != NULL && source->player != player && player->mo->IsTeammate (source))
	{
		poison = (int)((float)poison * teamdamage);
	}
	if (poison > 0)
	{
		player->poisoncount += poison;
		player->poisoner = poisoner;
		if(player->poisoncount > 100)
		{
			player->poisoncount = 100;
		}
	}
}

//==========================================================================
//
// P_PoisonDamage - Similar to P_DamageMobj
//
//==========================================================================

void P_PoisonDamage (player_t *player, AActor *source, int damage,
	bool playPainSound)
{
	AActor *target;
	AActor *inflictor;

	target = player->mo;
	inflictor = source;
	if (target->health <= 0)
	{
		return;
	}
	if (target->flags2&MF2_INVULNERABLE && damage < 1000000)
	{ // target is invulnerable
		return;
	}
	if (player && gameskill == sk_baby)
	{
		// Take half damage in trainer mode
		damage >>= 1;
	}
	if(damage < 1000 && ((player->cheats&CF_GODMODE)
		|| (player->mo->flags2 & MF2_INVULNERABLE)))
	{
		return;
	}
	if (damage >= player->health
		&& ((gameskill == sk_baby) || deathmatch)
		&& !player->morphTics)
	{ // Try to use some inventory health
		P_AutoUseHealth (player, damage - player->health+1);
	}
	player->health -= damage; // mirror mobj health here for Dave
	if (player->health < 50 && !deathmatch)
	{
		P_AutoUseStrifeHealth (player);
	}
	if (player->health < 0)
	{
		player->health = 0;
	}
	player->attacker = source;

	//
	// do the damage
	//
	target->health -= damage;
	if (target->health <= 0)
	{ // Death
		target->special1 = damage;
		if (player && inflictor && !player->morphTics)
		{ // Check for flame death
			if ((inflictor->DamageType == MOD_FIRE)
				&& (target->health > -50) && (damage > 25))
			{
				target->DamageType = MOD_FIRE;
			}
			if (inflictor->DamageType == MOD_ICE)
			{
				target->DamageType = MOD_ICE;
			}
		}
		target->Die (source, source);
		return;
	}
	if (!(level.time&63) && playPainSound && target->PainState != NULL)
	{
		target->SetState (target->PainState);

		// If we are the server, tell clients about the state change.
		if ( NETWORK_GetState( ) == NETSTATE_SERVER )
			SERVERCOMMANDS_SetThingState( target, STATE_PAIN );
	}
/*
	if((P_Random() < target->info->painchance)
		&& !(target->flags&MF_SKULLFLY))
	{
		target->flags |= MF_JUSTHIT; // fight back!
		P_SetMobjState(target, target->info->painstate);
	}
*/
}

bool CheckCheatmode ();

//GHK
void PLAYER_SetPoints( player_s *pPlayer, LONG Points, bool bAnnounce, bool bUpdateTeamFrags )
{
	// Don't bother with fragcount during warm-ups.
	if ((( duel ) && ( DUEL_GetState( ) == DS_COUNTDOWN )) ||
		(( lastmanstanding || teamlms ) && ( LASTMANSTANDING_GetState( ) == LMSS_COUNTDOWN )))
	{
		return;
	}

	// Don't announce events related to frag changes during teamplay, LMS,
	// or possession games.
	/*if (( bAnnounce ) &&
		( deathmatch ) &&
		( teamplay == false ) &&
		( lastmanstanding == false ) &&
		( teamlms == false ) &&
		( possession == false ) &&
		( teampossession == false ))
	{
		ANNOUNCER_PlayFragSounds( pPlayer - players, pPlayer->fragcount, lFragCount );
	}*/

	// If this is a teamplay deathmatch, update the team frags.
	if ( bUpdateTeamFrags )
	{
		if (( teamplay || teamlms || teamgame || teampossession || teamcoop ) && ( pPlayer->bOnTeam ))
			TEAM_SetScore( pPlayer->ulTeam, TEAM_GetScore( pPlayer->ulTeam ) + Points, bAnnounce );
	}

	// Set the player's fragcount.
	pPlayer->lPointCount += Points;

	// If we're the server, notify the clients of the fragcount change.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
	{
		SERVERCOMMANDS_SetPlayerPoints( pPlayer - players );

		// Also, update the scoreboard.
		SERVERCONSOLE_UpdatePlayerInfo( pPlayer - players, UDF_FRAGS );
		SERVERCONSOLE_UpdateScoreboard( );
		SERVERCOMMANDS_CheckPointLimit(  pPlayer  );
	}

	// Refresh the HUD since a score has changed.
	SCOREBOARD_RefreshHUD( );
}
//*****************************************************************************
//
void PLAYER_SetFragcount( player_s *pPlayer, LONG lFragCount, bool bAnnounce, bool bUpdateTeamFrags )
{
	// Don't bother with fragcount during warm-ups.
	if ((( duel ) && ( DUEL_GetState( ) == DS_COUNTDOWN )) ||
		(( lastmanstanding || teamlms ) && ( LASTMANSTANDING_GetState( ) == LMSS_COUNTDOWN )))
	{
		return;
	}

	// Don't announce events related to frag changes during teamplay, LMS,
	// or possession games.
	if (( bAnnounce ) &&
		( deathmatch ) &&
		( teamplay == false ) &&
		( lastmanstanding == false ) &&
		( teamlms == false ) &&
		( possession == false ) &&
		( teampossession == false ))
	{
		ANNOUNCER_PlayFragSounds( pPlayer - players, pPlayer->fragcount, lFragCount );
	}

	// If this is a teamplay deathmatch, update the team frags.
	if ( bUpdateTeamFrags )
	{
		if (( teamplay || teamlms || teamgame || teampossession ) && ( pPlayer->bOnTeam ))
			TEAM_SetFragCount( pPlayer->ulTeam, TEAM_GetFragCount( pPlayer->ulTeam ) + ( lFragCount - pPlayer->fragcount ), bAnnounce );
	}

	// Set the player's fragcount.
	pPlayer->fragcount = lFragCount;

	// If we're the server, notify the clients of the fragcount change.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
	{
		SERVERCOMMANDS_SetPlayerFrags( pPlayer - players );

		// Also, update the scoreboard.
		SERVERCONSOLE_UpdatePlayerInfo( pPlayer - players, UDF_FRAGS );
		SERVERCONSOLE_UpdateScoreboard( );
	}

	// Refresh the HUD since a score has changed.
	SCOREBOARD_RefreshHUD( );
}

//*****************************************************************************
//
void PLAYER_ResetAllPlayersFragcount( void )
{
	ULONG	ulIdx;

	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if ( playeringame[ulIdx] == false )
			continue;

		players[ulIdx].fragcount = 0;

		// If we're the server,
		if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		{
			SERVERCONSOLE_UpdatePlayerInfo( ulIdx, UDF_FRAGS );
			SERVERCONSOLE_UpdateScoreboard( );
		}
	}

	// Refresh the HUD since a score has changed.
	SCOREBOARD_RefreshHUD( );
}

//*****************************************************************************
//
void PLAYER_GivePossessionPoint( player_s *pPlayer )
{
	char				szString[64];
	DHUDMessageFadeOut	*pMsg;

//	if ( possession )
//		ANNOUNCER_PlayScoreSounds( pPlayer - players, pPlayer->lPointCount, pPlayer->lPointCount + 1 );

	// If we're in possession mode, give the player a point. Also, determine if the player's lead
	// state changed. If it did, announce it.
	if ( possession )
	{
		pPlayer->lPointCount++;

		// If we're the server, notify the clients of the pointcount change.
		if ( NETWORK_GetState( ) == NETSTATE_SERVER )
			SERVERCOMMANDS_SetPlayerPoints( pPlayer - players );
	}
	// If this is team possession, also give the player's team a point.
	else if ( teampossession && pPlayer->bOnTeam )
	{
		TEAM_SetScore( pPlayer->ulTeam, TEAM_GetScore( pPlayer->ulTeam ) + 1, true );

		pPlayer->lPointCount++;

		// If we're the server, notify the clients of the pointcount change.
		if ( NETWORK_GetState( ) == NETSTATE_SERVER )
			SERVERCOMMANDS_SetPlayerPoints( pPlayer - players );
	}
	else
		return;

	SCOREBOARD_RefreshHUD( );

	// If a pointlimit has been set, determine if the game has been won.
	if ( pointlimit )
	{
		if ( cooperative || possession && ( pPlayer->lPointCount >= pointlimit )) //ghk
		{
			if ( NETWORK_GetState( ) == NETSTATE_SERVER )
			{
				SERVER_Printf( PRINT_HIGH, "Pointlimit hit.\n" );
				SERVER_Printf( PRINT_HIGH, "%s \\c-wins!\n", pPlayer->userinfo.netname );
			}
			else
			{
				Printf( "Pointlimit hit.\n" );
				Printf( "%s \\c-wins!\n", pPlayer->userinfo.netname );

				if ( pPlayer->mo->CheckLocalView( consoleplayer ))
					ANNOUNCER_PlayEntry( cl_announcer, "YouWin" );
			}

			// Just print "YOU WIN!" in single player.
			if (( NETWORK_GetState( ) == NETSTATE_SINGLE_MULTIPLAYER ) && ( pPlayer->mo->CheckLocalView( consoleplayer )))
				sprintf( szString, "YOU WIN!" );
			else
				sprintf( szString, "%s \\c-WINS!", pPlayer->userinfo.netname );
			V_ColorizeString( szString );

			if ( NETWORK_GetState( ) != NETSTATE_SERVER )
			{
				screen->SetFont( BigFont );

				// Display "%s WINS!" HUD message.
				pMsg = new DHUDMessageFadeOut( szString,
					160.4f,
					75.0f,
					320,
					200,
					CR_RED,
					3.0f,
					2.0f );

				StatusBar->AttachMessage( pMsg, 'CNTR' );
				screen->SetFont( SmallFont );
				//g_SDLimitHit=1; //GHK
			}
			else
			{
				SERVERCOMMANDS_PrintHUDMessageFadeOut( szString, 160.4f, 75.0f, 320, 200, CR_RED, 3.0f, 2.0f, "BigFont", 'CNTR' );
				//g_SDLimitHit=1;
				SERVERCOMMANDS_SetSDLimitHit(1); //GHK
			}

			// End the level after five seconds.
			GAME_SetEndLevelDelay( 5 * TICRATE );
		}
		else if ( teamcoop || teampossession && ( TEAM_GetScore( pPlayer->ulTeam ) >= pointlimit )) //ghk
		{
			if ( NETWORK_GetState( ) == NETSTATE_SERVER )
			{
				SERVER_Printf( PRINT_HIGH, "Pointlimit hit.\n" );
				SERVER_Printf( PRINT_HIGH, "%s \\c-wins!\n", TEAM_GetName( pPlayer->ulTeam ));
			}
			else
			{
				Printf( "Pointlimit hit.\n" );
				Printf( "%s \\c-wins!\n", TEAM_GetName( pPlayer->ulTeam ));

				if ( pPlayer->mo->IsTeammate( players[consoleplayer].camera ))
					ANNOUNCER_PlayEntry( cl_announcer, "YouWin" );
			}

			if ( pPlayer->ulTeam == TEAM_BLUE )
				sprintf( szString, "\\chBLUE WINS!" );
			else
				sprintf( szString, "\\cgRED WINS!" );
			V_ColorizeString( szString );

			if ( NETWORK_GetState( ) != NETSTATE_SERVER )
			{
				screen->SetFont( BigFont );

				// Display "%s WINS!" HUD message.
				pMsg = new DHUDMessageFadeOut( szString,
					1.5f,
					0.375f,
					320,
					200,
					CR_RED,
					3.0f,
					2.0f );

				StatusBar->AttachMessage( pMsg, 'CNTR' );
				screen->SetFont( SmallFont );
			}
			else
			{
				SERVERCOMMANDS_PrintHUDMessageFadeOut( szString, 1.5f, 0.375f, 320, 200, CR_RED, 3.0f, 2.0f, "BigFont", 'CNTR' );
			}

			// End the level after five seconds.
			GAME_SetEndLevelDelay( 5 * TICRATE );
		}
	}
}

//*****************************************************************************
//
void PLAYER_SetTeam( player_s *pPlayer, ULONG ulTeam, bool bNoBroadcast )
{
	bool	bBroadcastChange = false;

	if (
		// Player was removed from a team.
		( pPlayer->bOnTeam && ulTeam == NUM_TEAMS ) ||

		// Player was put on a team after not being on one.
		( pPlayer->bOnTeam == false && ulTeam < NUM_TEAMS ) ||

		// Player is on a team, but is being put on a different team.
		( pPlayer->bOnTeam && ( ulTeam < NUM_TEAMS ) && ( ulTeam != pPlayer->ulTeam ))
		)
	{
		bBroadcastChange = true;
	}

	// We don't want to broadcast a print message.
	if ( bNoBroadcast )
		bBroadcastChange = false;

	// Set whether or not this player is on a team.
	if ( ulTeam == NUM_TEAMS )
		pPlayer->bOnTeam = false;
	else
		pPlayer->bOnTeam = true;

	// Set the team.
	pPlayer->ulTeam = ulTeam;

	// If we're the server, tell clients about this team change.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
	{
		SERVERCOMMANDS_SetPlayerTeam( pPlayer - players );

		// Player has changed his team! Tell clients.
		if ( bBroadcastChange )
		{
			if ( ulTeam == TEAM_BLUE )
				SERVER_Printf( PRINT_HIGH, "%s \\c-joined the \\ch%s \\c-team.\n", pPlayer->userinfo.netname, TEAM_GetName( ulTeam ));
			else
				SERVER_Printf( PRINT_HIGH, "%s \\c-joined the \\cg%s \\c-team.\n", pPlayer->userinfo.netname, TEAM_GetName( ulTeam ));
		}
	}

	// Finally, update the player's color.
	R_BuildPlayerTranslation( pPlayer - players );
	if ( StatusBar && ( pPlayer->mo->CheckLocalView( consoleplayer )))
		StatusBar->AttachToPlayer( pPlayer );

	// Update this player's info on the scoreboard.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCONSOLE_UpdatePlayerInfo( pPlayer - players, UDF_FRAGS );
}

//*****************************************************************************
//
void PLAYER_SetSpectator( player_s *pPlayer, bool bBroadcast, bool bDeadSpectator )
{
	// Already a spectator. Check if their spectating state is changing.
	if ( pPlayer->bSpectating == true )
	{
		// If they're becoming a true spectator after being a dead spectator, do all the
		// special spectator stuff we didn't do before.
		if (( bDeadSpectator == false ) && ( pPlayer->bDeadSpectator ))
		{
			pPlayer->bDeadSpectator = false;

			if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
			{
				// Tell the join queue module that a player is leaving the game.
				JOINQUEUE_PlayerLeftGame( true );
			}

			pPlayer->health = pPlayer->mo->health = deh.StartHealth;

			if ( bBroadcast )
			{
				// Send out a message saying this player joined the spectators.
				if ( NETWORK_GetState( ) == NETSTATE_SERVER )
					SERVER_Printf( PRINT_HIGH, "%s \\c-joined the spectators.\n", pPlayer->userinfo.netname );
				else
					Printf( "%s \\c-joined the spectators.\n", pPlayer->userinfo.netname );
			}
		}

		// This player no longer has a team affiliation.
		pPlayer->bOnTeam = false;

		if (( pPlayer->fragcount > 0 ) && ( NETWORK_GetState( ) != NETSTATE_CLIENT ))
			PLAYER_SetFragcount( pPlayer, 0, false, false );

		return;
	}

	// Flag this player as being a spectator.
	pPlayer->bSpectating = true;
	pPlayer->bDeadSpectator = bDeadSpectator;

	// If this player was eligible to get an assist, cancel that.
	if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
	{
		if ( TEAM_GetAssistPlayer( TEAM_BLUE ) == pPlayer - players )
			TEAM_SetAssistPlayer( TEAM_BLUE, MAXPLAYERS );
		if ( TEAM_GetAssistPlayer( TEAM_RED ) == pPlayer - players )
			TEAM_SetAssistPlayer( TEAM_RED, MAXPLAYERS );
	}

	if ( pPlayer->mo )
	{
		// Before we start fucking with the player's body, drop important items
		// like flags, etc.
		if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
			pPlayer->mo->DropImportantItems( false );

		// Is this player tagged as a dead spectator, give him life.
		pPlayer->playerstate = PST_LIVE;
		if ( bDeadSpectator == false )
			pPlayer->health = pPlayer->mo->health = deh.StartHealth;

		// Make the player unshootable, etc.
		pPlayer->mo->flags2 |= (MF2_CANNOTPUSH);
		pPlayer->mo->flags &= ~(MF_SOLID|MF_SHOOTABLE|MF_PICKUP);
		pPlayer->mo->flags2 &= ~(MF2_PASSMOBJ);
		pPlayer->mo->RenderStyle = STYLE_None;

		// Make the player flat, so he can travel under doors and such.
		pPlayer->mo->height = 0;

		// Make monsters unable to "see" this player.
		pPlayer->cheats |= CF_NOTARGET;

		// Take away all of the player's inventory.
		pPlayer->mo->DestroyAllInventory( );

		// Reset a bunch of other stuff.
		pPlayer->extralight = 0;
		pPlayer->fixedcolormap = 0;
		pPlayer->damagecount = 0;
		pPlayer->bonuscount = 0;
		pPlayer->poisoncount = 0;
		pPlayer->inventorytics = 0;

		if (( bDeadSpectator == false ) && bBroadcast )
		{
			// Send out a message saying this player joined the spectators.
			if ( NETWORK_GetState( ) == NETSTATE_SERVER )
				SERVER_Printf( PRINT_HIGH, "%s \\c-joined the spectators.\n", pPlayer->userinfo.netname );
			else
				Printf( "%s \\c-joined the spectators.\n", pPlayer->userinfo.netname );
		}
	}

	// This player no longer has a team affiliation.
	if ( bDeadSpectator == false )
		pPlayer->bOnTeam = false;

	// Player's lose all their frags when they become a spectator.
	if ( bDeadSpectator == false )
	{
		if (( pPlayer->fragcount > 0 ) && ( NETWORK_GetState( ) != NETSTATE_CLIENT ))
			PLAYER_SetFragcount( pPlayer, 0, false, false );

		// Also, tell the joinqueue module that a player has left the game.
		if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
		{
			// Tell the join queue module that a player is leaving the game.
			JOINQUEUE_PlayerLeftGame( true );
		}

		if ( pPlayer->pSkullBot )
			pPlayer->pSkullBot->PostEvent( BOTEVENT_SPECTATING );
	}

	// Player's also lose all of their wins in duel mode.
	if (( duel || ( lastmanstanding && ( bDeadSpectator == false ))) && ( pPlayer->ulWins > 0 ) && ( NETWORK_GetState( ) != NETSTATE_CLIENT ))
		PLAYER_SetWins( pPlayer, 0 );

	// Update this player's info on the scoreboard.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCONSOLE_UpdatePlayerInfo( pPlayer - players, UDF_FRAGS );
}

//*****************************************************************************
//
void PLAYER_SetWins( player_s *pPlayer, ULONG ulWins )
{
	// Set the player's fragcount.
	pPlayer->ulWins = ulWins;

	// Refresh the HUD since a score has changed.
	SCOREBOARD_RefreshHUD( );

	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
	{
		// If we're the server, notify the clients of the win count change.
		SERVERCOMMANDS_SetPlayerWins( pPlayer - players );

		// Also, update the scoreboard.
		SERVERCONSOLE_UpdatePlayerInfo( pPlayer - players, UDF_FRAGS );
		SERVERCONSOLE_UpdateScoreboard( );
	}
}

//*****************************************************************************
//
void PLAYER_GetName( player_s *pPlayer, char *pszOutBuf )
{
	// Build the buffer, which has a "remove color code" tag at the end of it.
	sprintf( pszOutBuf, "%s\\c-", pPlayer->userinfo.netname );
}

//*****************************************************************************
//
bool PLAYER_IsTrueSpectator( player_s *pPlayer )
{
	if ( lastmanstanding || teamlms || survival )
		return (( pPlayer->bSpectating ) && ( pPlayer->bDeadSpectator == false ));

	return ( pPlayer->bSpectating );
}

//*****************************************************************************
//
void PLAYER_StruckPlayer( player_s *pPlayer )
{
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	pPlayer->ulConsecutiveHits++;

	// If the player has made 5 straight consecutive hits with a weapon, award a medal.
	if (( pPlayer->ulConsecutiveHits % 5 ) == 0 )
	{
		// If this player has made 10+ consecuvite hits, award a "Precision" medal.
		if ( pPlayer->ulConsecutiveHits >= 10 )
		{
			MEDAL_GiveMedal( pPlayer - players, MEDAL_PRECISION );

			// Tell clients about the medal that been given.
			if ( NETWORK_GetState( ) == NETSTATE_SERVER )
				SERVERCOMMANDS_GivePlayerMedal( pPlayer - players, MEDAL_PRECISION );
		}
		// Otherwise, award an "Accuracy" medal.
		else
		{
			MEDAL_GiveMedal( pPlayer - players, MEDAL_ACCURACY );

			// Tell clients about the medal that been given.
			if ( NETWORK_GetState( ) == NETSTATE_SERVER )
				SERVERCOMMANDS_GivePlayerMedal( pPlayer - players, MEDAL_ACCURACY );
		}
	}

	// Reset the struck player flag.
	pPlayer->bStruckPlayer = false;
}

//*****************************************************************************
//
bool PLAYER_ShouldSpawnAsSpectator( player_s *pPlayer )
{
	UCVarValue	Val;

	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
	{
		// If there's a join password, the player should start as a spectator.
		Val = sv_joinpassword.GetGenericRep( CVAR_String );
		if (( sv_forcejoinpassword ) && ( strlen( Val.String )))
			return ( true );

		// If the number of players in the game exceeds sv_maxplayers, spawn as a spectator.
		Val = sv_maxplayers.GetGenericRep( CVAR_Int );
		if ( SERVER_CalcNumNonSpectatingPlayers( pPlayer - players ) >= Val.Int )
			return ( true );
	}

	// If an LMS game is in progress, the player should start as a spectator.
	if (( lastmanstanding || teamlms ) &&
		(( LASTMANSTANDING_GetState( ) == LMSS_INPROGRESS ) || ( LASTMANSTANDING_GetState( ) == LMSS_WINSEQUENCE )) &&
		( gameaction != ga_worlddone ))
	{
		return ( true );
	}

	// If the duel isn't in the "waiting for players" state, the player should start as a spectator.
	if ( duel )
	{
		if (( DUEL_GetState( ) != DS_WAITINGFORPLAYERS ) ||
			( SERVER_CalcNumNonSpectatingPlayers( pPlayer - players ) >= 2 ))
		{
			return ( true );
		}
	}

	// If a survival game is in progress, the player should start as a spectator.
	if (( survival ) &&
		(( SURVIVAL_GetState( ) == SURVS_INPROGRESS ) || ( SURVIVAL_GetState( ) == SURVS_MISSIONFAILED )) &&
		( gameaction != ga_worlddone ))
	{
		return ( true );
	}

	// Players entering a teamplay game must choose a team first before joining the fray.
	if (( pPlayer->bOnTeam == false ) || ( playeringame[pPlayer - players] == false ))
	{
		if (( teamplay && (( dmflags2 & DF2_NO_TEAM_SELECT ) == false )) ||
			( teamlms && (( dmflags2 & DF2_NO_TEAM_SELECT ) == false )) ||
			( ( teampossession || teamcoop ) && (( dmflags2 & DF2_NO_TEAM_SELECT ) == false )) ||
			( (teamgame) && (( dmflags2 & DF2_NO_TEAM_SELECT ) == false ) && ( TemporaryTeamStarts.Size( ) == 0 )))
		{
			return ( true );
		}
	}

	// Passed all checks!
	return ( false );
}

//*****************************************************************************
//
bool PLAYER_Taunt( player_s *pPlayer )
{
	// Don't taunt if we're not in a level!
	if ( gamestate != GS_LEVEL )
		return ( false );

	// Spectators or dead people can't taunt!
	if (( pPlayer->bSpectating ) ||
		( pPlayer->health <= 0 ) ||
		( pPlayer->mo == NULL ) ||
		( i_compatflags & COMPATF_DISABLETAUNTS ))
	{
		return ( false );
	}

	if ( cl_taunts )
		S_Sound( pPlayer->mo, CHAN_VOICE, "*taunt", 1, ATTN_NORM );

	return ( true );
}

CCMD (kill)
{
	// Only allow it in a level.
	if ( gamestate != GS_LEVEL )
		return;

	// [BC] Don't let spectators kill themselves.
	if ( players[consoleplayer].bSpectating == true )
		return;

	// [BC] Don't allow suiciding during a duel.
	if ( duel && ( DUEL_GetState( ) == DS_INDUEL ))
	{
		Printf( "You cannot suicide during a duel.\n" );
		return;
	}

	if (argv.argc() > 1)
	{
		if (!stricmp (argv[1], "monsters"))
		{
			// Kill all the monsters
			if (CheckCheatmode ())
				return;

			Net_WriteByte (DEM_GENERICCHEAT);
			Net_WriteByte (CHT_MASSACRE);
		}
		else
		{
			Printf("cannot kill '%s'\n", argv[1]);
			return;
		}
	}
	else
	{
		// [BC] Tell the server we wish to suicide.
		if (( NETWORK_GetState( ) == NETSTATE_CLIENT ) && ( players[consoleplayer].bSpectating == false ))
			CLIENTCOMMANDS_Suicide( );

		// Kill the player
		Net_WriteByte (DEM_SUICIDE);
	}
	C_HideConsole ();
}

//*****************************************************************************
//
CCMD( taunt )
{
	if ( PLAYER_Taunt( &players[consoleplayer] ))
	{
		// Tell the server we taunted!
		if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
			CLIENTCOMMANDS_Taunt( );
	}
}
