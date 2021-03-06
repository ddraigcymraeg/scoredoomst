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
//
//-----------------------------------------------------------------------------


#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <direct.h>
#include <string.h>
#include <process.h>

#include <stdarg.h>
#include <sys/types.h>
#include <sys/timeb.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

#define USE_WINDOWS_DWORD
#include "hardware.h"
#include "doomerrors.h"
#include <math.h>

#include "doomtype.h"
#include "version.h"
#include "doomdef.h"
#include "cmdlib.h"
#include "m_argv.h"
#include "m_misc.h"
#include "i_video.h"
#include "i_sound.h"
#include "i_music.h"
#include "resource.h"

#include "d_main.h"
#include "d_net.h"
#include "g_game.h"
#include "i_input.h"
#include "i_system.h"
#include "c_dispatch.h"
#include "templates.h"
#include "gameconfigfile.h"
// [BC] New #includes.
#include "announcer.h"
#include "network.h"
#include "cl_main.h"
#include "campaign.h"

#include "stats.h"

EXTERN_CVAR (String, language)

#ifdef USEASM
extern "C" void STACK_ARGS CheckMMX (CPUInfo *cpu);
#endif

extern "C"
{
	double		SecondsPerCycle = 1e-8;
	double		CyclesPerSecond = 1e8;		// 100 MHz
	CPUInfo		CPU;
}

extern HWND Window, ConWindow;
extern HINSTANCE g_hInst;

UINT TimerPeriod;
UINT TimerEventID;
UINT MillisecondsPerTic;
HANDLE NewTicArrived;
uint32 LanguageIDs[4];
void CalculateCPUSpeed ();

int (*I_GetTime) (bool saveMS);
int (*I_WaitForTic) (int);

os_t OSPlatform;

void I_Tactile (int on, int off, int total)
{
  // UNUSED.
  on = off = total = 0;
}

ticcmd_t emptycmd;
ticcmd_t *I_BaseTiccmd(void)
{
	return &emptycmd;
}

static DWORD basetime = 0;

// [RH] Returns time in milliseconds
unsigned int I_MSTime (void)
{
	DWORD tm;

	tm = timeGetTime();
	if (!basetime)
		basetime = tm;

	return tm - basetime;
}

static DWORD TicStart;
static DWORD TicNext;

//GHK
void I_SDMessageBox(){
	MessageBox(0, "Please restart ScoreDoomST if you wish to use the add-on pack online.", "ScoreDoomST Configuration", 0);
}

//
// I_GetTime
// returns time in 1/35th second tics
//
int I_GetTimePolled (bool saveMS)
{
	DWORD tm;

	tm = timeGetTime();
	if (!basetime)
		basetime = tm;

	if (saveMS)
	{
		TicStart = tm;
		TicNext = (tm * TICRATE / 1000 + 1) * 1000 / TICRATE;
	}

	return ((tm-basetime)*TICRATE)/1000;
}

float I_GetTimeFloat( void )
{
	DWORD tm;

	tm = timeGetTime();
	if (!basetime)
		basetime = tm;

	return (( (float)tm - (float)basetime ) * (float)TICRATE ) / 1000.0f;
}

int I_GetMSElapsed( void )
{
	DWORD tm;

	tm = timeGetTime();
	if (!basetime)
		basetime = tm;

	return ( tm - basetime );
}

void I_Sleep( int iMS )
{
	Sleep( iMS );
}

int I_WaitForTicPolled (int prevtic)
{
	int time;

	while ((time = I_GetTimePolled(false)) <= prevtic)
		;

	return time;
}


static int tics;
static DWORD ted_start, ted_next;

int I_GetTimeEventDriven (bool saveMS)
{
	if (saveMS)
	{
		TicStart = ted_start;
		TicNext = ted_next;
	}
	return tics;
}

int I_WaitForTicEvent (int prevtic)
{
	while (prevtic >= tics)
	{
		WaitForSingleObject (NewTicArrived, 1000/TICRATE);
	}

	return tics;
}

void CALLBACK TimerTicked (UINT id, UINT msg, DWORD user, DWORD dw1, DWORD dw2)
{
	tics++;
	ted_start = timeGetTime ();
	ted_next = ted_start + MillisecondsPerTic;
	SetEvent (NewTicArrived);
}

// Returns the fractional amount of a tic passed since the most recent tic
fixed_t I_GetTimeFrac (uint32 *ms)
{
	DWORD now = timeGetTime();
	if (ms) *ms = TicNext;
	DWORD step = TicNext - TicStart;
	if (step == 0)
	{
		return FRACUNIT;
	}
	else
	{
		fixed_t frac = clamp<fixed_t> ((now - TicStart)*FRACUNIT/step, 0, FRACUNIT);
		return frac;
	}
}

void I_WaitVBL (int count)
{
	// I_WaitVBL is never used to actually synchronize to the
	// vertical blank. Instead, it's used for delay purposes.
	Sleep (1000 * count / 70);
}

// [RH] Detect the OS the game is running under
void			SERVERCONSOLE_UpdateOperatingSystem( char *pszString );
void I_DetectOS (void)
{
	OSVERSIONINFO info;
	const char *osname;

	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&info);

	switch (info.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		OSPlatform = os_Win95;
		if (info.dwMinorVersion < 10)
		{
			osname = "95";
		}
		else if (info.dwMinorVersion < 90)
		{
			osname = "98";
		}
		else
		{
			osname = "Me";
		}
		break;

	case VER_PLATFORM_WIN32_NT:
		OSPlatform = info.dwMajorVersion < 5 ? os_WinNT4 : os_Win2k;
		osname = "NT";
		if (info.dwMajorVersion == 5)
		{
			if (info.dwMinorVersion == 0)
			{
				osname = "2000";
			}
			if (info.dwMinorVersion == 1)
			{
				osname = "XP";
			}
			else if (info.dwMinorVersion == 2)
			{
				osname = "Server 2003";
			}
		}
		else if (info.dwMajorVersion == 6 && info.dwMinorVersion == 0)
		{
			osname = "Vista";
		}
		break;

	default:
		OSPlatform = os_unknown;
		osname = "Unknown OS";
		break;
	}

	Printf ("OS: Windows %s %lu.%lu (Build %lu)\n",
			osname,
			info.dwMajorVersion, info.dwMinorVersion,
			OSPlatform == os_Win95 ? info.dwBuildNumber & 0xffff : info.dwBuildNumber);

	if ( Args.CheckParm( "-host" ))
	{
		char	szString[256];

		sprintf( szString,
			"Windows %s %lu.%lu (Build %lu)",
			osname,
			info.dwMajorVersion, info.dwMinorVersion,
			OSPlatform == os_Win95 ? info.dwBuildNumber & 0xffff : info.dwBuildNumber );

		SERVERCONSOLE_UpdateOperatingSystem( szString );
	}

	if (info.szCSDVersion[0])
	{
		Printf ("    %s\n", info.szCSDVersion);
	}

	if (OSPlatform == os_unknown)
	{
		Printf ("(Assuming Windows 95)\n");
		OSPlatform = os_Win95;
	}
}

//
// SubsetLanguageIDs
//
static void SubsetLanguageIDs (LCID id, LCTYPE type, int idx)
{
	char buf[8];
	LCID langid;
	char *idp;

	if (!GetLocaleInfo (id, type, buf, 8))
		return;
	langid = MAKELCID (strtoul(buf, NULL, 16), SORT_DEFAULT);
	if (!GetLocaleInfo (langid, LOCALE_SABBREVLANGNAME, buf, 8))
		return;
	idp = (char *)(&LanguageIDs[idx]);
	memset (idp, 0, 4);
	idp[0] = tolower(buf[0]);
	idp[1] = tolower(buf[1]);
	idp[2] = tolower(buf[2]);
	idp[3] = 0;
}

//
// SetLanguageIDs
//
void SetLanguageIDs ()
{
	size_t langlen = strlen (language);

	if (langlen < 2 || langlen > 3)
	{
		memset (LanguageIDs, 0, sizeof(LanguageIDs));
		SubsetLanguageIDs (LOCALE_USER_DEFAULT, LOCALE_ILANGUAGE, 0);
		SubsetLanguageIDs (LOCALE_USER_DEFAULT, LOCALE_IDEFAULTLANGUAGE, 1);
		SubsetLanguageIDs (LOCALE_SYSTEM_DEFAULT, LOCALE_ILANGUAGE, 2);
		SubsetLanguageIDs (LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTLANGUAGE, 3);
	}
	else
	{
		DWORD lang = 0;

		((BYTE *)&lang)[0] = (language)[0];
		((BYTE *)&lang)[1] = (language)[1];
		((BYTE *)&lang)[2] = (language)[2];
		LanguageIDs[0] = lang;
		LanguageIDs[1] = lang;
		LanguageIDs[2] = lang;
		LanguageIDs[3] = lang;
	}
}

//
// I_Init
//
void SERVERCONSOLE_UpdateVendor( char *pszString );
void I_Init (void)
{
#ifndef USEASM
	memset (&CPU, 0, sizeof(CPU));
#else
	CheckMMX (&CPU);
	CalculateCPUSpeed ();

	// Why does Intel right-justify this string?
	char *f = CPU.CPUString, *t = f;

	while (*f == ' ')
	{
		++f;
	}
	if (f != t)
	{
		while (*f != 0)
		{
			*t++ = *f++;
		}
	}

#endif
	if (CPU.VendorID[0])
	{
		Printf ("CPU Vendor ID: %s\n", CPU.VendorID);
		if ( Args.CheckParm( "-host" ))
			SERVERCONSOLE_UpdateVendor( CPU.VendorID );

		if (CPU.CPUString[0])
		{
			Printf ("  Name: %s\n", CPU.CPUString);
		}
		if (CPU.bIsAMD)
		{
			Printf ("  Family %d (%d), Model %d, Stepping %d\n",
				CPU.Family, CPU.AMDFamily, CPU.AMDModel, CPU.AMDStepping);
		}
		else
		{
			Printf ("  Family %d, Model %d, Stepping %d\n",
				CPU.Family, CPU.Model, CPU.Stepping);
		}
		Printf ("  Features:");
		if (CPU.bMMX)		Printf (" MMX");
		if (CPU.bMMXPlus)	Printf (" MMX+");
		if (CPU.bSSE)		Printf (" SSE");
		if (CPU.bSSE2)		Printf (" SSE2");
		if (CPU.bSSE3)		Printf (" SSE3");
		if (CPU.b3DNow)		Printf (" 3DNow!");
		if (CPU.b3DNowPlus)	Printf (" 3DNow!+");
		Printf ("\n");
	}


	// Use a timer event if possible
	NewTicArrived = CreateEvent (NULL, FALSE, FALSE, NULL);
	if (NewTicArrived)
	{
		UINT delay;
		char *cmdDelay;

		cmdDelay = Args.CheckValue ("-timerdelay");
		delay = 0;
		if (cmdDelay != 0)
		{
			delay = atoi (cmdDelay);
		}
		if (delay == 0)
		{
			delay = 1000/TICRATE;
		}
		TimerEventID = timeSetEvent
			(
				delay,
				0,
				TimerTicked,
				0,
				TIME_PERIODIC
			);
		MillisecondsPerTic = delay;
	}
	
	// Server is never a timer event.
	if (( TimerEventID != 0 ) && ( NETWORK_GetState( ) != NETSTATE_SERVER ))
	{
		I_GetTime = I_GetTimeEventDriven;
		I_WaitForTic = I_WaitForTicEvent;
	}
	else
	{	// If no timer event, busy-loop with timeGetTime
		I_GetTime = I_GetTimePolled;
		I_WaitForTic = I_WaitForTicPolled;
	}

	if ( NETWORK_GetState( ) != NETSTATE_SERVER )
	{
		atterm (I_ShutdownSound);
		I_InitSound ();
		I_InitInput (Window);
		I_InitHardware ();
	}
}

void SERVERCONSOLE_UpdateCPUSpeed( char *pszString );
void CalculateCPUSpeed ()
{
	LARGE_INTEGER freq;

	QueryPerformanceFrequency (&freq);

	if (freq.QuadPart != 0 && CPU.bRDTSC)
	{
		LARGE_INTEGER count1, count2;
		DWORD minDiff;
		cycle_t ClockCalibration = 0;

		// Count cycles for at least 55 milliseconds.
		// The performance counter is very low resolution compared to CPU
		// speeds today, so the longer we count, the more accurate our estimate.
		// On the other hand, we don't want to count too long, because we don't
		// want the user to notice us spend time here, since most users will
		// probably never use the performance statistics.
		minDiff = freq.LowPart * 11 / 200;

		// Minimize the chance of task switching during the testing by going very
		// high priority. This is another reason to avoid timing for too long.
		SetPriorityClass (GetCurrentProcess (), REALTIME_PRIORITY_CLASS);
		SetThreadPriority (GetCurrentThread (), THREAD_PRIORITY_TIME_CRITICAL);
		clock (ClockCalibration);
		QueryPerformanceCounter (&count1);
		do
		{
			QueryPerformanceCounter (&count2);
		} while ((DWORD)((unsigned __int64)count2.QuadPart - (unsigned __int64)count1.QuadPart) < minDiff);
		unclock (ClockCalibration);
		QueryPerformanceCounter (&count2);
		SetPriorityClass (GetCurrentProcess (), NORMAL_PRIORITY_CLASS);
		SetThreadPriority (GetCurrentThread (), THREAD_PRIORITY_NORMAL);

		CyclesPerSecond = (double)ClockCalibration *
			(double)freq.QuadPart /
			(double)((__int64)count2.QuadPart - (__int64)count1.QuadPart);
		SecondsPerCycle = 1.0 / CyclesPerSecond;
	}
	else
	{
		Printf ("Can't determine CPU speed, so pretending.\n");

		if ( Args.CheckParm( "-host" ))
		{
			char	szString[256];

			sprintf( szString, "Can't determine CPU speed, so pretending.", CyclesPerSecond / 1e6 );
			SERVERCONSOLE_UpdateCPUSpeed( szString );
		}
	}

	Printf ("CPU Speed: %f MHz\n", CyclesPerSecond / 1e6);

	if ( Args.CheckParm( "-host" ))
	{
		char	szString[256];

		sprintf( szString, "%f MHz", CyclesPerSecond / 1e6 );
		SERVERCONSOLE_UpdateCPUSpeed( szString );
	}
}

//
// I_Quit
//
static int has_exited;

void I_Quit (void)
{
	has_exited = 1;		/* Prevent infinitely recursive exits -- killough */

	if (TimerEventID)
		timeKillEvent (TimerEventID);
	if (NewTicArrived)
		CloseHandle (NewTicArrived);

	timeEndPeriod (TimerPeriod);

	// [BC] Tell the server we're leaving the game.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		CLIENT_QuitNetworkGame( );

	if (demorecording)
		G_CheckDemoStatus();
	G_ClearSnapshots ();
}


//
// I_Error
//
extern FILE *Logfile;
bool gameisdead;

void STACK_ARGS I_FatalError (const char *error, ...)
{
	static BOOL alreadyThrown = false;
	gameisdead = true;

	if (!alreadyThrown)		// ignore all but the first message -- killough
	{
		char errortext[MAX_ERRORTEXT];
		int index;
		va_list argptr;
		va_start (argptr, error);
		index = vsprintf (errortext, error, argptr);
// GetLastError() is usually useless because we don't do a lot of Win32 stuff
//		sprintf (errortext + index, "\nGetLastError = %ld", GetLastError());
		va_end (argptr);

		// Record error to log (if logging)
		if (Logfile)
			fprintf (Logfile, "\n**** DIED WITH FATAL ERROR:\n%s\n", errortext);

		throw CFatalError (errortext);
	}

	if (!has_exited)	// If it hasn't exited yet, exit now -- killough
	{
		has_exited = 1;	// Prevent infinitely recursive exits -- killough
		exit(-1);
	}
}

void STACK_ARGS I_Error (const char *error, ...)
{
	va_list argptr;
	char errortext[MAX_ERRORTEXT];

	va_start (argptr, error);
	vsprintf (errortext, error, argptr);
	va_end (argptr);

	throw CRecoverableError (errortext);
}

char DoomStartupTitle[256] = { 0 };

void I_SetTitleString (const char *title)
{
	int i;

	for (i = 0; title[i]; i++)
		DoomStartupTitle[i] = title[i];
}

void I_PrintStr (const char *cp, bool lineBreak)
{
	if (ConWindow == NULL)
		return;

	static bool newLine = true;
	HWND edit = (HWND)(LONG_PTR)GetWindowLongPtr (ConWindow, GWLP_USERDATA);
	char buf[256];
	int bpos = 0;

	int selstart, selend;
	SendMessage (edit, EM_GETSEL, (WPARAM)&selstart, (LPARAM)&selend);

//	SendMessage (edit, EM_SETSEL, (WPARAM)-1, 0);
	SendMessage (edit, EM_SETSEL, INT_MAX, INT_MAX);

	if (lineBreak && !newLine)
	{
		buf[0] = '\r';
		buf[1] = '\n';
		bpos = 2;
	}
	while (*cp != 0)
	{
		if (*cp == 28)
		{ // Skip color changes
			if (*++cp != 0)
				cp++;
			continue;
		}
		if (bpos < 253)
		{
			// Stupid edit controls need CR-LF pairs
			if (*cp == '\n')
			{
				buf[bpos++] = '\r';
			}
		}
		else
		{
			buf[bpos] = 0;
			SendMessage (edit, EM_REPLACESEL, FALSE, (LPARAM)buf);
			newLine = buf[bpos-1] == '\n';
			bpos = 0;
		}
		buf[bpos++] = *cp++;
	}
	if (bpos != 0)
	{
		buf[bpos] = 0;
		SendMessage (edit, EM_REPLACESEL, FALSE, (LPARAM)buf);
		newLine = buf[bpos-1] == '\n';
	}
	SendMessage (edit, EM_SETSEL, selstart, selend);
}

EXTERN_CVAR (Bool, queryiwad);
CVAR (String, queryiwad_key, "shift", CVAR_GLOBALCONFIG|CVAR_ARCHIVE);
static WadStuff *WadList;
static int NumWads;
static int DefaultWad;

static void SetQueryIWad (HWND dialog)
{
	HWND checkbox = GetDlgItem (dialog, IDC_DONTASKIWAD);
	int state = SendMessage (checkbox, BM_GETCHECK, 0, 0);
	bool query = (state != BST_CHECKED);

	if (!query && queryiwad)
	{
		MessageBox (dialog,
			"You have chosen not to show this dialog box in the future.\n"
			"If you wish to see it again, hold down SHIFT while starting " GAMENAME ".",
			"Don't ask me this again",
			MB_OK | MB_ICONINFORMATION);
	}

	queryiwad = query;
}

BOOL CALLBACK IWADBoxCallback (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND ctrl;
	int i;

	switch (message)
	{
	case WM_INITDIALOG:
		// Add our program name to the window title
		{
			TCHAR label[256];
			FString newlabel;

			GetWindowText (hDlg, label, countof(label));
			newlabel.Format (GAMESIG " " DOTVERSIONSTR_NOREV ": %s", label);
			SetWindowText (hDlg, newlabel.GetChars());
		}
		// Populate the list with all the IWADs found
		ctrl = GetDlgItem (hDlg, IDC_IWADLIST);
		for (i = 0; i < NumWads; i++)
		{
			FString work;
			const char *filepart = strrchr (WadList[i].Path, '/');
			if (filepart == NULL)
				filepart = WadList[i].Path;
			else
				filepart++;
			work.Format ("%s (%s)", IWADTypeNames[WadList[i].Type], filepart);
			SendMessage (ctrl, LB_ADDSTRING, 0, (LPARAM)work.GetChars());
			SendMessage (ctrl, LB_SETITEMDATA, i, (LPARAM)i);
		}
		SendMessage (ctrl, LB_SETCURSEL, DefaultWad, 0);
		SetFocus (ctrl);
		// Set the state of the "Don't ask me again" checkbox
		ctrl = GetDlgItem (hDlg, IDC_DONTASKIWAD);
		SendMessage (ctrl, BM_SETCHECK, queryiwad ? BST_UNCHECKED : BST_CHECKED, 0);
		// Make sure the dialog is in front. If SHIFT was pressed to force it visible,
		// then the other window will normally be on top.
		SetForegroundWindow (hDlg);
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog (hDlg, -1);
		}
		else if (LOWORD(wParam) == IDOK ||
			(LOWORD(wParam) == IDC_IWADLIST && HIWORD(wParam) == LBN_DBLCLK))
		{
			SetQueryIWad (hDlg);
			ctrl = GetDlgItem (hDlg, IDC_IWADLIST);
			EndDialog (hDlg, SendMessage (ctrl, LB_GETCURSEL, 0, 0));
		}
		break;
	}
	return FALSE;
}

int I_PickIWad (WadStuff *wads, int numwads, bool showwin, int defaultiwad)
{
	int vkey;

	if (stricmp (queryiwad_key, "shift") == 0)
	{
		vkey = VK_SHIFT;
	}
	else if (stricmp (queryiwad_key, "control") == 0 || stricmp (queryiwad_key, "ctrl") == 0)
	{
		vkey = VK_CONTROL;
	}
	else
	{
		vkey = 0;
	}
	if (showwin || (vkey != 0 && GetAsyncKeyState(vkey)))
	{
		WadList = wads;
		NumWads = numwads;
		DefaultWad = defaultiwad;

		return DialogBox (g_hInst, MAKEINTRESOURCE(IDD_IWADDIALOG),
			(HWND)Window, (DLGPROC)IWADBoxCallback);
	}
	return defaultiwad;
}

bool I_WriteIniFailed ()
{
	char *lpMsgBuf;
	FString errortext;

	FormatMessageA (FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPSTR)&lpMsgBuf,
		0,
		NULL 
	);
	errortext.Format ("The config file %s could not be written:\n%s", GameConfig->GetPathName(), lpMsgBuf);
	LocalFree (lpMsgBuf);
	return MessageBox (Window, errortext.GetChars(), GAMENAME " configuration not saved", MB_ICONEXCLAMATION | MB_RETRYCANCEL) == IDRETRY;
}

void *I_FindFirst (const char *filespec, findstate_t *fileinfo)
{
	return FindFirstFileA (filespec, (LPWIN32_FIND_DATAA)fileinfo);
}
int I_FindNext (void *handle, findstate_t *fileinfo)
{
	return !FindNextFileA ((HANDLE)handle, (LPWIN32_FIND_DATAA)fileinfo);
}

int I_FindClose (void *handle)
{
	return FindClose ((HANDLE)handle);
}

//
// I_ConsoleInput - [NightFang] - pulled from the old 0.99 code
//
char *I_ConsoleInput (void)
{
#ifndef	WIN32
	static 	char text[256];
	int	len;
	if (!stdin_ready || !do_stdin)
	{ return NULL; }

	stdin_ready = 0;

	len = read(0, text, sizeof(text));
	if (len < 1)
	{ return NULL; }

	text[len-1] = 0;

	return text;
#else
	
// Windows code
	static char     text[256];
    static int              len;
    int             c;

    // read a line out
    while (_kbhit())
    {
		c = _getch();
        putch (c);
        if (c == '\r')
        {
			text[len] = 0;
            putch ('\n');
            len = 0;
            return text;
        }
        
		if (c == 8)
        {
			if (len)
            {
				putch (' ');
                putch (c);
                len--;
                text[len] = 0;
            }
            continue;
        }
    
		text[len] = c;
        len++;
        text[len] = 0;
        if (len == sizeof(text))
		    len = 0;
	}

    return NULL;
#endif
}
