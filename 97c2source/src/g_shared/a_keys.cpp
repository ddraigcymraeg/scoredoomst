#include "a_keys.h"
#include "tarray.h"
#include "gi.h"
#include "gstrings.h"
#include "d_player.h"
#include "c_console.h"
#include "s_sound.h"
#include "sc_man.h"
#include "v_palette.h"
#include "w_wad.h"
#include "network.h"
#include "sv_commands.h"
#include "v_text.h"
#include "team.h"
#include "cooperative.h"
#include "scoreboard.h"

// [BC] Ugh. //ghk
void SERVERCONSOLE_UpdatePlayerInfo( LONG lPlayer, ULONG ulUpdateFlags );
void SERVERCONSOLE_UpdateScoreboard( void );


struct OneKey
{
	const PClass * key;
	int count;

	bool check(AActor * owner)
	{
		return !!owner->FindInventory(key);
	}
};

struct Keygroup
{
	TArray<OneKey> anykeylist;

	bool check(AActor * owner)
	{
		for(unsigned int i=0;i<anykeylist.Size();i++)
		{
			if (anykeylist[i].check(owner)) return true;
		}
		return false;
	}
};

struct Lock
{
	TArray<Keygroup *> keylist;
	char * message;
	char * remotemsg;
	int locksound;
	int	rgb;

	Lock()
	{
		message=remotemsg=NULL;
		rgb=0;
	}

	~Lock()
	{
		for(unsigned int i=0;i<keylist.Size();i++) delete keylist[i];
		keylist.Clear();
		if (message) delete [] message;
		if (remotemsg) delete [] remotemsg;
	}

	bool check(AActor * owner)
	{
		// An empty key list means that any key will do
		if (!keylist.Size())
		{
			for (AInventory * item = owner->Inventory; item != NULL; item = item->Inventory)
			{
				if (item->IsKindOf (RUNTIME_CLASS(AKey)))
				{
					return true;
				}
			}
		}
		else for(unsigned int i=0;i<keylist.Size();i++)
		{
			if (!keylist[i]->check(owner)) return false;
		}
		return true;
	}
};


static Lock * locks[256];		// all valid locks
static bool keysdone=false;		// have the locks been initialized?
static int currentnumber;		// number to be assigned to next key
static bool ignorekey;			// set to true when the current lock is not being used

static void ClearLocks();

static const char * keywords_lock[]={
	"ANY",
	"MESSAGE",
	"REMOTEMESSAGE",
	"MAPCOLOR",
	"LOCKEDSOUND",
	NULL
};

//===========================================================================
//
//
//===========================================================================

static void AddOneKey(Keygroup * keygroup, const PClass * mi)
{
	if (mi)
	{
		// Any inventory item can be used to unlock a door
		if (mi->IsDescendantOf(RUNTIME_CLASS(AInventory)))
		{
			OneKey k = {mi,1};
			keygroup->anykeylist.Push (k);

			//... but only keys get key numbers!
			if (mi->IsDescendantOf(RUNTIME_CLASS(AKey)))
			{
				if (!ignorekey &&
					static_cast<AKey*>(GetDefaultByType(mi))->KeyNumber == 0)
				{
					static_cast<AKey*>(GetDefaultByType(mi))->KeyNumber=++currentnumber;
				}
			}
		}
		else
		{
			SC_ScriptError("'%s' is not an inventory item", sc_String);
		}
	}
	else
	{
		SC_ScriptError("Unknown item '%s'", sc_String);
	}
}


//===========================================================================
//
//
//===========================================================================

static Keygroup * ParseKeygroup()
{
	Keygroup * keygroup;
	const PClass * mi;

	SC_MustGetStringName("{");
	keygroup=new Keygroup;
	while (!SC_CheckString("}"))
	{
		SC_MustGetString();
		mi=PClass::FindClass(sc_String);
		AddOneKey(keygroup, mi);
	}
	if (keygroup->anykeylist.Size()==0)
	{
		delete keygroup;
		return NULL;
	}
	keygroup->anykeylist.ShrinkToFit();
	return keygroup;
}

//===========================================================================
//
//
//===========================================================================

static void PrintMessage (const char *str)
{
	if (str != NULL)
	{
		if (str[0]=='$')
		{
			str=GStrings(str+1);
		}
		C_MidPrint (str);
	}
}

//===========================================================================
//
//
//===========================================================================

static void ParseLock()
{
	int i,r,g,b;
	int keynum;
	Lock sink;
	Lock * lock=&sink;
	Keygroup * keygroup;
	const PClass * mi;

	SC_MustGetNumber();
	keynum=sc_Number;

	SC_MustGetString();
	if (SC_Compare("DOOM"))
	{
		if (gameinfo.gametype != GAME_Doom) keynum=-1;
	}
	else if (SC_Compare("HERETIC"))
	{
		if (gameinfo.gametype != GAME_Heretic) keynum=-1;
	}
	else if (SC_Compare("HEXEN"))
	{
		if (gameinfo.gametype != GAME_Hexen) keynum=-1;
	}
	else if (SC_Compare("STRIFE"))
	{
		if (gameinfo.gametype != GAME_Strife) keynum=-1;
	}
	else SC_UnGet();

	ignorekey=true;
	if (keynum>0 && keynum<255)
	{
		lock=new Lock;
		if (locks[keynum]) delete locks[keynum];
		locks[keynum]=lock;
		locks[keynum]->locksound = S_FindSound("misc/keytry");
		ignorekey=false;
	}
	else if (keynum!=-1)
	{
		SC_ScriptError("Lock index %d out of range", keynum);
	}

	SC_MustGetStringName("{");
	while (!SC_CheckString("}"))
	{
		SC_MustGetString();
		switch(i=SC_MatchString(keywords_lock))
		{
		case 0:	// Any
			keygroup=ParseKeygroup();
			if (keygroup) lock->keylist.Push(keygroup);
			break;

		case 1:	// message
			SC_MustGetString();
			lock->message=copystring(sc_String);
			break;

		case 2: // remotemsg
			SC_MustGetString();
			lock->remotemsg=copystring(sc_String);
			break;

		case 3:	// mapcolor
			SC_MustGetNumber();
			r=sc_Number;
			SC_MustGetNumber();
			g=sc_Number;
			SC_MustGetNumber();
			b=sc_Number;
			lock->rgb=MAKERGB(r,g,b);
			break;

		case 4:	// locksound
			SC_MustGetString();
			lock->locksound = S_FindSound(sc_String);
			break;

		default:
			mi=PClass::FindClass(sc_String);
			if (mi)
			{
				keygroup=new Keygroup;
				AddOneKey(keygroup, mi);
				if (keygroup)
				{
					keygroup->anykeylist.ShrinkToFit();
					lock->keylist.Push(keygroup);
				}
			}
			break;
		}
	}
	// copy the messages if the other one does not exist
	if (!lock->remotemsg && lock->message) lock->remotemsg = copystring(lock->message);
	if (!lock->message && lock->remotemsg) lock->message = copystring(lock->remotemsg);
	lock->keylist.ShrinkToFit();
}

//===========================================================================
//
// Clears all key numbers so the parser can assign its own ones
// This ensures that only valid keys are considered by the key cheats
//
//===========================================================================

static void ClearLocks()
{
	unsigned int i;
	for(i=0;i<PClass::m_Types.Size();i++)
	{
		if (PClass::m_Types[i]->IsDescendantOf(RUNTIME_CLASS(AKey)))
		{
			AKey *key = static_cast<AKey*>(GetDefaultByType(PClass::m_Types[i]));
			if (key != NULL)
			{
				key->KeyNumber = 0;
			}
		}
	}
	for(i=0;i<256;i++)
	{
		if (locks[i]!=NULL)
		{
			delete locks[i];
			locks[i]=NULL;
		}
	}
	currentnumber=0;
	keysdone=false;
}

//===========================================================================
//
// P_InitKeyMessages
//
//===========================================================================

void P_InitKeyMessages()
{
	int lastlump, lump;

	lastlump = 0;

	ClearLocks();
	while ((lump = Wads.FindLump ("LOCKDEFS", &lastlump)) != -1)
	{
		SC_OpenLumpNum (lump, "LOCKDEFS");
		while (SC_GetString ())
		{
			if (SC_Compare("LOCK"))
			{
				ParseLock();
			}
			else if (SC_Compare("CLEARLOCKS"))
			{
				// clear all existing lock defintions and key numbers
				ClearLocks();
			}
			else
				SC_ScriptError("Unknown command %s in LockDef", sc_String);
		}
		SC_Close();
	}
	keysdone=true;
}

//===========================================================================
//
// P_DeinitKeyMessages
//
//===========================================================================

void P_DeinitKeyMessages()
{
	ClearLocks();
}

//===========================================================================
//
// P_CheckKeys
//
// Returns true if the actor has the required key. If not, a message is
// shown if the actor is also the consoleplayer's camarea, and false is
// returned.
//
//===========================================================================

bool P_CheckKeys (AActor *owner, int keynum, bool remote)
{
	const char *failtext = NULL;
	int failsound = 0;

	failtext = NULL;
	failsound = 0;

	if (keynum<=0 || keynum>255) return true;
	// Just a safety precaution. The messages should have been initialized upon game start.
	if (!keysdone) P_InitKeyMessages();

	if (!locks[keynum])
	{
		if (keynum==103 && gameinfo.gametype == GAME_Strife)
			failtext = "THIS AREA IS ONLY AVAILABLE IN THE RETAIL VERSION OF STRIFE";
		else
			failtext = "That doesn't seem to work";

		failsound = S_FindSound("misc/keytry");
	}
	else
	{
		if (locks[keynum]->check(owner)){
			//ghk, if the owner is the first player to use this color key
			//on a door on the level, then they get a bonus. Doesnt differentiate between
			// card key or skull key of the same color, so it 2 of these of the same color are
			//used, then it will only score once based on color used.

			///if ( NETWORK_GetState( ) != NETSTATE_CLIENT )
			if ( ( teamcoop || cooperative ) && ( NETWORK_GetState( ) != NETSTATE_CLIENT ) ){
				int keyBonus = 250;
				bool keyUsedScore = false;
				char levelscore[40];
				switch (locks[keynum]->rgb){
					//RedKey
					case 16711680:
						if(!level.rkeyused){
							//level.info->
							if(owner->player!=NULL){


								owner->player->lPointCount+=keyBonus;
								//if(owner->CheckLocalView (consoleplayer)){
									//char levelscore[40];
									sprintf(levelscore,TEXTCOLOR_RED"Objective Reached! "TEXTCOLOR_WHITE"+ %d"TEXTCOLOR_RED"%s", keyBonus, " Bonus!");
								//C_MidPrintSD(levelscore);
								//Printf (PRINT_MEDIUM, TEXTCOLOR_WHITE"+ %d"TEXTCOLOR_RED"%s\n", keyBonus, " Red Key Bonus!");
								//}

								level.rkeyused=!level.rkeyused;

								keyUsedScore = true;
							}
						}
						break;
					//BlueKey
					case 255:
						if(!level.bkeyused){
							//level.info->
							if(owner->player!=NULL){

								owner->player->lPointCount+=keyBonus;
								//if(owner->CheckLocalView (consoleplayer)){
									//char levelscore[40];
									sprintf(levelscore,TEXTCOLOR_LIGHTBLUE"Objective Reached! "TEXTCOLOR_WHITE"+ %d"TEXTCOLOR_LIGHTBLUE"%s", keyBonus, " Bonus!");
									//C_MidPrintSD(levelscore);
							//Printf (PRINT_MEDIUM, TEXTCOLOR_WHITE"+ %d"TEXTCOLOR_RED"%s\n", keyBonus, " Red Key Bonus!");
								//}
								level.bkeyused=!level.bkeyused;
								keyUsedScore = true;
							}
						}
						break;
					//YellowKey
					case 16776960:
						if(!level.ykeyused){
							//level.info->
							if(owner->player!=NULL){

								owner->player->lPointCount+=keyBonus;
								//if(owner->CheckLocalView (consoleplayer)){
									//char levelscore[40];
									sprintf(levelscore,TEXTCOLOR_GOLD"Objective Reached! "TEXTCOLOR_WHITE"+ %d"TEXTCOLOR_GOLD"%s", keyBonus, " Bonus!");
								//C_MidPrintSD(levelscore);
							//Printf (PRINT_MEDIUM, TEXTCOLOR_WHITE"+ %d"TEXTCOLOR_RED"%s\n", keyBonus, " Red Key Bonus!");
								//}
								level.ykeyused=!level.ykeyused;
								keyUsedScore = true;
							}
						}
						break;
					//GreenKey (Heretic)..replaces red key
					case 65280:
						if(!level.rkeyused&&gameinfo.gametype == GAME_Heretic){
							//level.info->
							if(owner->player!=NULL){

								owner->player->lPointCount+=keyBonus;
								//if(owner->CheckLocalView (consoleplayer)){
									//char levelscore[40];
									sprintf(levelscore,TEXTCOLOR_GREEN"Objective Reached! "TEXTCOLOR_WHITE"+ %d"TEXTCOLOR_GREEN"%s", keyBonus, " Bonus!");
								//C_MidPrintSD(levelscore);
							//Printf (PRINT_MEDIUM, TEXTCOLOR_WHITE"+ %d"TEXTCOLOR_RED"%s\n", keyBonus, " Red Key Bonus!");
								//}
								level.rkeyused=!level.rkeyused;
								keyUsedScore = true;
							}
						}
						break;
					case 8421631: //any key
						if(!level.nkeyused){
							//level.info->
							if(owner->player!=NULL){

								owner->player->lPointCount+=keyBonus;
								//if(owner->CheckLocalView (consoleplayer)){
									//char levelscore[40];
									sprintf(levelscore,TEXTCOLOR_GREEN"Objective Reached! "TEXTCOLOR_WHITE"+ %d"TEXTCOLOR_GREEN"%s", keyBonus, " Bonus!");
									C_MidPrintSD(levelscore);
							//Printf (PRINT_MEDIUM, TEXTCOLOR_WHITE"+ %d"TEXTCOLOR_RED"%s\n", keyBonus, " Red Key Bonus!");
								//}
								level.nkeyused=!level.nkeyused;
								keyUsedScore = true;
							}
						}
						break;
					case 0: //all 6 keys & all 3 keys
						//If there are levels which allow red only doors with all 3 or 6, this poses a problem
						//due to flags. Need a 3-door, 6 -door flag, but if rgb is the same for
						//both 3 & 6 , its impossible!
						//need new keyused variables.
						if(locks[keynum]->keylist.Size()==3){
							if(!level.m3keyused){
								//level.info->
								if(owner->player!=NULL){

								owner->player->lPointCount+=keyBonus;
								//if(owner->CheckLocalView (consoleplayer)){
										//char levelscore[40];
										sprintf(levelscore,TEXTCOLOR_GREEN"Objective Reached! "TEXTCOLOR_WHITE"+ %d"TEXTCOLOR_GREEN"%s", 3*keyBonus, " Bonus!");
										//C_MidPrintSD(levelscore);
								//Printf (PRINT_MEDIUM, TEXTCOLOR_WHITE"+ %d"TEXTCOLOR_RED"%s\n", keyBonus, " Red Key Bonus!");
									//}
									level.m3keyused=!level.m3keyused;
									keyUsedScore = true;
								}
							}
						}else if(locks[keynum]->keylist.Size()==6){
							if(!level.m6keyused){
								//level.info->
								if(owner->player!=NULL){

								owner->player->lPointCount+=keyBonus;
								//if(owner->CheckLocalView (consoleplayer)){
										//char levelscore[40];
										sprintf(levelscore,TEXTCOLOR_GREEN"Objective Reached! "TEXTCOLOR_WHITE"+ %d"TEXTCOLOR_GREEN"%s", 6*keyBonus, " Bonus!");
										//C_MidPrintSD(levelscore);
								//Printf (PRINT_MEDIUM, TEXTCOLOR_WHITE"+ %d"TEXTCOLOR_RED"%s\n", keyBonus, " Red Key Bonus!");
									//}
									level.m6keyused=!level.m6keyused;
									keyUsedScore = true;
								}
							}

						}
						break;


				}

				if(teamcoop&&owner->player->bOnTeam)
					TEAM_SetScore( owner->player->ulTeam, TEAM_GetScore( owner->player->ulTeam ) + owner->player->lPointCount, true );


				if(keyUsedScore && (NETWORK_GetState( ) == NETSTATE_SERVER ) ){

					SERVERCOMMANDS_SetPlayerPoints( owner->player - players );
					// Also, update the scoreboard.
					SERVERCONSOLE_UpdatePlayerInfo( ULONG( owner->player - players ), UDF_FRAGS );
					SERVERCONSOLE_UpdateScoreboard( );
					SERVERCOMMANDS_CheckPointLimit(  owner->player );
					//SERVERCOMMANDS_DoSDSound(1, ULONG( owner->player - players ), SVCF_ONLYTHISCLIENT);
					//SERVERCOMMANDS_SetSectorSecretFound( LONG( sector - sectors ) );

					SERVERCOMMANDS_PrintMid(levelscore, owner->player - players, SVCF_ONLYTHISCLIENT);
					SERVERCOMMANDS_DoSDSound(2, ULONG( owner->player - players ), SVCF_ONLYTHISCLIENT);
					//
					//S_Sound (CHAN_AUTO, "ghk/keyscr", 1, ATTN_NORM);

				}

			}
			SCOREBOARD_RefreshHUD( );

			//
			return true;}

		failtext = remote? locks[keynum]->remotemsg : locks[keynum]->message;
		failsound = locks[keynum]->locksound;
	}

	// If we get here, that means the actor isn't holding an appropriate key.

	if (owner == players[consoleplayer].camera)
	{
		PrintMessage(failtext);
		S_SoundID (owner, CHAN_VOICE, failsound, 1, ATTN_NORM);

		// [BC] If we're the server, print the message to
		if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		{
			if ( failtext[0] == '$' )
				failtext = GStrings( failtext + 1 );

			SERVERCOMMANDS_PrintMid( (char *)failtext, consoleplayer, SVCF_ONLYTHISCLIENT );
			SERVERCOMMANDS_SoundIDActor( owner, CHAN_VOICE, failsound, 127, ATTN_NORM );
		}
	}

	return false;
}

//==========================================================================
//
// AKey implementation
//
//==========================================================================

IMPLEMENT_STATELESS_ACTOR (AKey, Any, -1, 0)
 PROP_Inventory_FlagsSet (IF_INTERHUBSTRIP)
 PROP_Inventory_PickupSound ("misc/k_pkup")
END_DEFAULTS

bool AKey::HandlePickup (AInventory *item)
{
	// In single player, you can pick up an infinite number of keys
	// even though you can only hold one of each.
	if ( NETWORK_GetState( ) != NETSTATE_SINGLE )
	{
		return Super::HandlePickup (item);
	}
	if (GetClass() == item->GetClass())
	{
		item->ItemFlags |= IF_PICKUPGOOD;
		return true;
	}
	if (Inventory != NULL)
	{
		return Inventory->HandlePickup (item);
	}
	return false;
}

bool AKey::ShouldStay ()
{
	return ( NETWORK_GetState( ) != NETSTATE_SINGLE );
}

//==========================================================================
//
// These functions can be used to get color information for
// automap display of keys and locked doors
//
//==========================================================================

int P_GetMapColorForLock (int lock)
{
	if (lock > 0 && lock < 256)
	{
		if (locks[lock]) return locks[lock]->rgb;
	}
	return 0;
}

//==========================================================================
//
//
//
//==========================================================================

int P_GetMapColorForKey (AInventory * key)
{
	int i;

	for (i = 0; i < 256; i++)
	{
		if (locks[i] && locks[i]->check(key)) return locks[i]->rgb;
	}
	return 0;
}
