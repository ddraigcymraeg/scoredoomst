/* Generated by re2c 0.10.5 */
#line 1 "src/sc_man_scanner.re"
#define	YYCTYPE		char
#define	YYCURSOR	cursor
#define	YYLIMIT		limit
#define	YYMARKER	marker

	// This buffer must be as large as the largest YYFILL call
	YYCTYPE eofbuf[3];
#define	YYFILL(n)	\
	{ if(!sc_End) { \
	   if(n == 2) { eofbuf[0] = *cursor; } \
	   else if(n == 3) { eofbuf[0] = *cursor; eofbuf[1] = *(cursor + 1); } \
	   eofbuf[n-1] = '\n'; \
	   cursor = eofbuf; \
	   limit = eofbuf + n - 1; \
	   sc_End = true; } \
	} \
	assert(n <= 3)	// Semicolon intentionally omitted

//#define YYDEBUG(s,c) { Printf ("%d: %02x\n", s, c); }
#define YYDEBUG(s,c)

	char *cursor = ScriptPtr;
	char *limit = ScriptEndPtr;

std1:
	tok = YYCURSOR;
std2:
#line 43 "src/sc_man_scanner.re"

	if (!CMode)
	{
	
#line 36 "src/sc_man_scanner.h"
{
	YYCTYPE yych;
	unsigned int yyaccept = 0;

	if((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
	yych = *YYCURSOR;
	if(yych <= '/') {
		if(yych <= ' ') {
			if(yych == 0x0A) goto yy8;
			goto yy6;
		} else {
			if(yych == '"') goto yy10;
			if(yych <= '.') goto yy14;
			goto yy3;
		}
	} else {
		if(yych <= '<') {
			if(yych == ';') goto yy5;
			goto yy14;
		} else {
			if(yych <= '=') goto yy12;
			if(yych <= 'z') goto yy14;
			if(yych <= '}') goto yy12;
			goto yy14;
		}
	}
yy2:
#line 57 "src/sc_man_scanner.re"
	{ goto normal_token; }
#line 66 "src/sc_man_scanner.h"
yy3:
	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	if(yych <= '/') {
		if(yych <= '"') {
			if(yych == '!') goto yy19;
		} else {
			if(yych == '*') goto yy27;
			if(yych <= '.') goto yy19;
			goto yy23;
		}
	} else {
		if(yych <= '<') {
			if(yych != ';') goto yy19;
		} else {
			if(yych <= '=') goto yy4;
			if(yych <= 'z') goto yy19;
			if(yych >= '~') goto yy19;
		}
	}
yy4:
#line 59 "src/sc_man_scanner.re"
	{ goto normal_token; }
#line 90 "src/sc_man_scanner.h"
yy5:
	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	goto yy24;
yy6:
	++YYCURSOR;
	yych = *YYCURSOR;
	goto yy22;
yy7:
#line 50 "src/sc_man_scanner.re"
	{ goto std1; }
#line 102 "src/sc_man_scanner.h"
yy8:
	++YYCURSOR;
#line 51 "src/sc_man_scanner.re"
	{ goto newline; }
#line 107 "src/sc_man_scanner.h"
yy10:
	++YYCURSOR;
#line 52 "src/sc_man_scanner.re"
	{ goto string; }
#line 112 "src/sc_man_scanner.h"
yy12:
	++YYCURSOR;
#line 54 "src/sc_man_scanner.re"
	{ goto normal_token; }
#line 117 "src/sc_man_scanner.h"
yy14:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	goto yy18;
yy15:
	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	if(yych <= '/') {
		if(yych <= '"') {
			if(yych == '!') goto yy19;
		} else {
			if(yych == '*') goto yy16;
			if(yych <= '.') goto yy19;
		}
	} else {
		if(yych <= '<') {
			if(yych != ';') goto yy19;
		} else {
			if(yych <= '=') goto yy16;
			if(yych <= 'z') goto yy19;
			if(yych >= '~') goto yy19;
		}
	}
yy16:
	YYCURSOR = YYMARKER;
	if(yyaccept <= 0) {
		goto yy4;
	} else {
		goto yy2;
	}
yy17:
	yyaccept = 1;
	YYMARKER = ++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
yy18:
	if(yych <= ':') {
		if(yych <= '"') {
			if(yych == '!') goto yy17;
			goto yy2;
		} else {
			if(yych == '/') goto yy15;
			goto yy17;
		}
	} else {
		if(yych <= '=') {
			if(yych == '<') goto yy17;
			goto yy2;
		} else {
			if(yych <= 'z') goto yy17;
			if(yych <= '}') goto yy2;
			goto yy17;
		}
	}
yy19:
	yyaccept = 1;
	YYMARKER = ++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	if(yych <= ':') {
		if(yych <= '"') {
			if(yych == '!') goto yy19;
			goto yy2;
		} else {
			if(yych == '/') goto yy15;
			goto yy19;
		}
	} else {
		if(yych <= '=') {
			if(yych == '<') goto yy19;
			goto yy2;
		} else {
			if(yych <= 'z') goto yy19;
			if(yych <= '}') goto yy2;
			goto yy19;
		}
	}
yy21:
	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
yy22:
	if(yych == 0x0A) goto yy7;
	if(yych <= ' ') goto yy21;
	goto yy7;
yy23:
	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
yy24:
	if(yych != 0x0A) goto yy23;
	++YYCURSOR;
#line 48 "src/sc_man_scanner.re"
	{ goto newline; }
#line 213 "src/sc_man_scanner.h"
yy27:
	++YYCURSOR;
#line 47 "src/sc_man_scanner.re"
	{ goto comment; }
#line 218 "src/sc_man_scanner.h"
}
#line 60 "src/sc_man_scanner.re"

	}
	else
	{
	
#line 226 "src/sc_man_scanner.h"
{
	YYCTYPE yych;
	if((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
	yych = *YYCURSOR;
	if(yych <= '9') {
		if(yych <= '%') {
			if(yych <= ' ') {
				if(yych == 0x0A) goto yy35;
				goto yy33;
			} else {
				if(yych == '"') goto yy37;
				goto yy49;
			}
		} else {
			if(yych <= ',') {
				if(yych <= '&') goto yy44;
				if(yych <= '\'') goto yy50;
				goto yy49;
			} else {
				if(yych <= '-') goto yy39;
				if(yych <= '.') goto yy43;
				if(yych >= '0') goto yy41;
			}
		}
	} else {
		if(yych <= '^') {
			if(yych <= '=') {
				if(yych <= ';') goto yy49;
				if(yych <= '<') goto yy47;
				goto yy45;
			} else {
				if(yych <= '>') goto yy48;
				if(yych <= '@') goto yy49;
				if(yych <= 'Z') goto yy50;
				goto yy49;
			}
		} else {
			if(yych <= 'z') {
				if(yych == '`') goto yy49;
				goto yy50;
			} else {
				if(yych == '|') goto yy46;
				if(yych <= '~') goto yy49;
				goto yy50;
			}
		}
	}
	yych = *(YYMARKER = ++YYCURSOR);
	if(yych == '*') goto yy74;
	if(yych == '/') goto yy72;
yy32:
#line 79 "src/sc_man_scanner.re"
	{ goto normal_token; }
#line 280 "src/sc_man_scanner.h"
yy33:
	++YYCURSOR;
	yych = *YYCURSOR;
	goto yy71;
yy34:
#line 68 "src/sc_man_scanner.re"
	{ goto std1; }
#line 288 "src/sc_man_scanner.h"
yy35:
	++YYCURSOR;
#line 69 "src/sc_man_scanner.re"
	{ goto newline; }
#line 293 "src/sc_man_scanner.h"
yy37:
	++YYCURSOR;
#line 70 "src/sc_man_scanner.re"
	{ goto string; }
#line 298 "src/sc_man_scanner.h"
yy39:
	++YYCURSOR;
#line 72 "src/sc_man_scanner.re"
	{ goto negative_check; }
#line 303 "src/sc_man_scanner.h"
yy41:
	++YYCURSOR;
	if((yych = *YYCURSOR) == '.') goto yy68;
	if(yych <= '/') goto yy52;
	if(yych <= '9') goto yy66;
	goto yy52;
yy42:
#line 80 "src/sc_man_scanner.re"
	{ goto normal_token; }
#line 313 "src/sc_man_scanner.h"
yy43:
	yych = *++YYCURSOR;
	if(yych <= '/') goto yy32;
	if(yych <= '9') goto yy63;
	goto yy32;
yy44:
	yych = *++YYCURSOR;
	if(yych == '&') goto yy61;
	goto yy32;
yy45:
	yych = *++YYCURSOR;
	if(yych == '=') goto yy59;
	goto yy32;
yy46:
	yych = *++YYCURSOR;
	if(yych == '|') goto yy57;
	goto yy32;
yy47:
	yych = *++YYCURSOR;
	if(yych == '<') goto yy55;
	goto yy32;
yy48:
	yych = *++YYCURSOR;
	if(yych == '>') goto yy53;
	goto yy32;
yy49:
	yych = *++YYCURSOR;
	goto yy32;
yy50:
	yych = *++YYCURSOR;
	goto yy52;
yy51:
	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
yy52:
	if(yych <= 'Z') {
		if(yych <= '/') {
			if(yych == '\'') goto yy51;
			goto yy42;
		} else {
			if(yych <= '9') goto yy51;
			if(yych <= '@') goto yy42;
			goto yy51;
		}
	} else {
		if(yych <= '`') {
			if(yych == '_') goto yy51;
			goto yy42;
		} else {
			if(yych <= 'z') goto yy51;
			if(yych <= '~') goto yy42;
			goto yy51;
		}
	}
yy53:
	++YYCURSOR;
#line 78 "src/sc_man_scanner.re"
	{ goto normal_token; }
#line 373 "src/sc_man_scanner.h"
yy55:
	++YYCURSOR;
#line 77 "src/sc_man_scanner.re"
	{ goto normal_token; }
#line 378 "src/sc_man_scanner.h"
yy57:
	++YYCURSOR;
#line 76 "src/sc_man_scanner.re"
	{ goto normal_token; }
#line 383 "src/sc_man_scanner.h"
yy59:
	++YYCURSOR;
#line 75 "src/sc_man_scanner.re"
	{ goto normal_token; }
#line 388 "src/sc_man_scanner.h"
yy61:
	++YYCURSOR;
#line 74 "src/sc_man_scanner.re"
	{ goto normal_token; }
#line 393 "src/sc_man_scanner.h"
yy63:
	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	if(yych <= '/') goto yy65;
	if(yych <= '9') goto yy63;
yy65:
#line 73 "src/sc_man_scanner.re"
	{ goto normal_token; }
#line 403 "src/sc_man_scanner.h"
yy66:
	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	if(yych <= '@') {
		if(yych <= '-') {
			if(yych == '\'') goto yy51;
			goto yy42;
		} else {
			if(yych <= '.') goto yy68;
			if(yych <= '/') goto yy42;
			if(yych <= '9') goto yy66;
			goto yy42;
		}
	} else {
		if(yych <= '_') {
			if(yych <= 'Z') goto yy51;
			if(yych <= '^') goto yy42;
			goto yy51;
		} else {
			if(yych <= '`') goto yy42;
			if(yych <= 'z') goto yy51;
			if(yych <= '~') goto yy42;
			goto yy51;
		}
	}
yy68:
	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	if(yych <= '/') goto yy65;
	if(yych <= '9') goto yy68;
	goto yy65;
yy70:
	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
yy71:
	if(yych == 0x0A) goto yy34;
	if(yych <= ' ') goto yy70;
	goto yy34;
yy72:
	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	if(yych == 0x0A) goto yy76;
	goto yy72;
yy74:
	++YYCURSOR;
#line 65 "src/sc_man_scanner.re"
	{ goto comment; }
#line 455 "src/sc_man_scanner.h"
yy76:
	++YYCURSOR;
#line 66 "src/sc_man_scanner.re"
	{ goto newline; }
#line 460 "src/sc_man_scanner.h"
}
#line 83 "src/sc_man_scanner.re"

	}

negative_check:
	// re2c doesn't have enough state to handle '-' as the start of a negative number
	// and as its own token, so help it out a little.
	if (YYCURSOR >= YYLIMIT)
	{
		goto normal_token;
	}
	if (*YYCURSOR >= '0' && *YYCURSOR <= '9')
	{
		goto std2;
	}
	if (*YYCURSOR != '.' || YYCURSOR+1 >= YYLIMIT)
	{
		goto normal_token;
	}
	if (*(YYCURSOR+1) >= '0' && *YYCURSOR <= '9')
	{
		goto std2;
	}
	goto normal_token;

comment:

#line 489 "src/sc_man_scanner.h"
{
	YYCTYPE yych;
	if((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
	yych = *YYCURSOR;
	if(yych == 0x0A) goto yy82;
	if(yych != '*') goto yy84;
	++YYCURSOR;
	if((yych = *YYCURSOR) == '/') goto yy85;
yy81:
#line 129 "src/sc_man_scanner.re"
	{ goto comment; }
#line 501 "src/sc_man_scanner.h"
yy82:
	++YYCURSOR;
#line 119 "src/sc_man_scanner.re"
	{
			if (YYCURSOR >= YYLIMIT)
			{
				ScriptPtr = ScriptEndPtr;
				return false;
			}
			sc_Line++;
			sc_Crossed = true;
			goto comment;
		}
#line 515 "src/sc_man_scanner.h"
yy84:
	yych = *++YYCURSOR;
	goto yy81;
yy85:
	++YYCURSOR;
#line 110 "src/sc_man_scanner.re"
	{
			if (YYCURSOR >= YYLIMIT)
			{
				ScriptPtr = ScriptEndPtr;
				return false;
			}
			goto std1;
		}
#line 530 "src/sc_man_scanner.h"
}
#line 130 "src/sc_man_scanner.re"


newline:
	if (YYCURSOR >= YYLIMIT)
	{
		ScriptPtr = ScriptEndPtr;
		return false;
	}
	sc_Line++;
	sc_Crossed = true;
	goto std1;

normal_token:
	ScriptPtr = (YYCURSOR >= YYLIMIT) ? ScriptEndPtr : cursor;
	sc_StringLen = MIN (ScriptPtr - tok, MAX_STRING_SIZE-1);
	memcpy (sc_String, tok, sc_StringLen);
	sc_String[sc_StringLen] = '\0';
	return true;

string:
	if (YYLIMIT != ScriptEndPtr)
	{
		ScriptPtr = ScriptEndPtr;
		return false;
	}
	ScriptPtr = cursor;
	for (sc_StringLen = 0; cursor < YYLIMIT; ++cursor)
	{
		if (Escape && *cursor == '\\' && *(cursor + 1) == '"')
		{
			cursor++;
		}
		else if (*cursor == '\r' && *(cursor + 1) == '\n')
		{
			cursor++;	// convert CR-LF to simply LF
		}
		else if (*cursor == '"')
		{
			break;
		}
		if (*cursor == '\n')
		{
			if (CMode)
			{
				if (!Escape || sc_StringLen == 0 || sc_String[sc_StringLen - 1] != '\\')
				{
					SC_ScriptError ("Unterminated string constant");
				}
				else
				{
					sc_StringLen--;		// overwrite the \ character with \n
				}
			}
			sc_Line++;
			sc_Crossed = true;
		}
		if (sc_StringLen < MAX_STRING_SIZE-1)
		{
			sc_String[sc_StringLen++] = *cursor;
		}
	}
	ScriptPtr = cursor + 1;
	sc_String[sc_StringLen] = '\0';
	return true;