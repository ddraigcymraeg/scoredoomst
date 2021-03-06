/*
** v_text.cpp
** Draws text to a canvas. Also has a text line-breaker thingy.
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
*/

#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "v_text.h"

#include "i_system.h"
#include "v_video.h"
#include "hu_stuff.h"
#include "w_wad.h"
#include "m_swap.h"

#include "doomstat.h"
#include "templates.h"

//
// SetFont
//
// Set the canvas's font
//
void DCanvas::SetFont (FFont *font)
{
	Font = font;
}

void STACK_ARGS DCanvas::DrawChar (int normalcolor, int x, int y, BYTE character, ...)
{
	if (Font == NULL)
		return;

	if (normalcolor >= NumTextColors)
		normalcolor = CR_UNTRANSLATED;

	FTexture *pic;
	int dummy;

	if (NULL != (pic = Font->GetChar (character, &dummy)))
	{
		const BYTE *range = Font->GetColorTranslation ((EColorRange)normalcolor);
		va_list taglist;
		va_start (taglist, character);
		DrawTexture (pic, x, y, DTA_Translation, range, TAG_MORE, &taglist);
		va_end (taglist);
	}
}

//
// DrawText
//
// Write a string using the current font
//
void STACK_ARGS DCanvas::DrawText (int normalcolor, int x, int y, const char *string, ...)
{
	va_list tags;
	DWORD tag;
	INTBOOL boolval;

	int			maxstrlen = INT_MAX;
	int 		w, maxwidth;
	const BYTE *ch;
	int 		c;
	int 		cx;
	int 		cy;
	int			boldcolor;
	const BYTE *range;
	int			height;
	int			scalex, scaley;
	int			kerning;
	FTexture *pic;

	if (Font == NULL || string == NULL)
		return;

	if (normalcolor >= NumTextColors)
		normalcolor = CR_UNTRANSLATED;
	boldcolor = normalcolor ? normalcolor - 1 : NumTextColors - 1;

	range = Font->GetColorTranslation ((EColorRange)normalcolor);
	height = Font->GetHeight () + 1;
	kerning = Font->GetDefaultKerning ();

	ch = (const BYTE *)string;
	cx = x;
	cy = y;

	// Parse the tag list to see if we need to adjust for scaling.
 	maxwidth = Width;
	scalex = scaley = 1;

	va_start (tags, string);
	tag = va_arg (tags, DWORD);

	while (tag != TAG_DONE)
	{
		va_list *more_p;
		DWORD data;
		void *ptrval;

		switch (tag)
		{
		case TAG_IGNORE:
		default:
			data = va_arg (tags, DWORD);
			break;

		case TAG_MORE:
			more_p = va_arg (tags, va_list*);
			va_end (tags);
#ifdef __GNUC__
			__va_copy (tags, *more_p);
#else
			tags = *more_p;
#endif
			break;

		case DTA_DestWidth:
		case DTA_DestHeight:
			*(DWORD *)tags = TAG_IGNORE;
			data = va_arg (tags, DWORD);
			break;

		case DTA_Translation:
			*(DWORD *)tags = TAG_IGNORE;
			ptrval = va_arg (tags, void*);
			break;

		case DTA_CleanNoMove:
			boolval = va_arg (tags, INTBOOL);
			if (boolval)
			{
				scalex = (LONG)CleanXfac;
				scaley = (LONG)CleanYfac;
				maxwidth = Width - (Width % (int)CleanYfac);
			}
			break;

		case DTA_Clean:
		case DTA_320x200:
			boolval = va_arg (tags, INTBOOL);
			if (boolval)
			{
				scalex = scaley = 1;
				maxwidth = 320;
			}
			break;

		case DTA_VirtualWidth:
			maxwidth = va_arg (tags, int);
			scalex = scaley = 1;
			break;

		case DTA_TextLen:
			maxstrlen = va_arg (tags, int);
			break;

		// [BC] Is this text? If so, handle it slightly differently when we draw it.
		case DTA_IsText:

			va_arg( tags, int );
			break;
		}
		tag = va_arg (tags, DWORD);
	}

	height *= scaley;
		
	while ((const char *)ch - string < maxstrlen)
	{
		c = *ch++;
		if (!c)
			break;

		if (c == TEXTCOLOR_ESCAPE)
		{
			int newcolor = *ch++;

			if (newcolor == '\0')
			{
				return;
			}
			else if (newcolor == '-')		// Normal
			{
				newcolor = normalcolor;
			}
			else if (newcolor == '+')		// Bold
			{
				newcolor = boldcolor;
			}
			else if (newcolor == '[')		// Named
			{
				const BYTE *namestart = ch;
				while (*ch != ']' && *ch != '\0')
				{
					ch++;
				}
				FName rangename((const char *)namestart, int(ch - namestart), true);
				if (*ch != '\0')
				{
					ch++;
				}
				newcolor = V_FindFontColor (rangename);
			}
			else if (newcolor >= 'A' && newcolor < NUM_TEXT_COLORS + 'A')	// Standard, uppercase
			{
				newcolor -= 'A';
			}
			else if (newcolor >= 'a' && newcolor < NUM_TEXT_COLORS + 'a')	// Standard, lowercase
			{
				newcolor -= 'a';
			}
			else							// Incomplete!
			{
				continue;
			}
			range = Font->GetColorTranslation ((EColorRange)newcolor);
			continue;
		}
		
		if (c == '\n')
		{
			cx = x;
			cy += height;
			continue;
		}

		if (NULL != (pic = Font->GetChar (c, &w)))
		{
			va_list taglist;
			va_start (taglist, string);
			// [BC] Flag this as being text.
			DrawTexture (pic, cx, cy, DTA_Translation, range, DTA_IsText, true, TAG_MORE, &taglist);
			va_end (taglist);
		}
		cx += (w + kerning) * scalex;
	}
}

//
// Find string width using this font
//
int FFont::StringWidth (const BYTE *string) const
{
	int w = 0;
	int maxw = 0;
		
	while (*string)
	{
		if (*string == TEXTCOLOR_ESCAPE)
		{
			++string;
			if (*string == '[')
			{
				while (*string != '\0' && *string != ']')
				{
					++string;
				}
			}
			else if (*string != '\0')
			{
				++string;
			}
			continue;
		}
		else if (*string == '\n')
		{
			if (w > maxw)
				maxw = w;
			w = 0;
			++string;
		}
		else
		{
			w += GetCharWidth (*string++) + GlobalKerning;
		}
	}
				
	return MAX (maxw, w);
}

// [BC] This is essentially strbin() from the original ZDoom code. It's been made a public
// function so that we can colorize lots of things.
void V_ColorizeString( char *pszString )
{
	char *p = pszString, c;
	int i;

	while ( (c = *p++) )
	{
		// If we don't encounter a slash, keep parsing.
		if (c != '\\')
			*pszString++ = c;
		else
		{
			switch (*p)
			{
				case 'c':
					*pszString++ = TEXTCOLOR_ESCAPE;
					break;
				case 'n':
					*pszString++ = '\n';
					break;
				case 't':
					*pszString++ = '\t';
					break;
				case 'r':
					*pszString++ = '\r';
					break;
//				case '\n':
//					break;
				case 'x':
				case 'X':
					c = 0;
					p++;
					for (i = 0; i < 2; i++) {
						c <<= 4;
						if (*p >= '0' && *p <= '9')
							c += *p-'0';
						else if (*p >= 'a' && *p <= 'f')
							c += 10 + *p-'a';
						else if (*p >= 'A' && *p <= 'F')
							c += 10 + *p-'A';
						else
							break;
						p++;
					}
					*pszString++ = c;
					break;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					c = 0;
					for (i = 0; i < 3; i++) {
						c <<= 3;
						if (*p >= '0' && *p <= '7')
							c += *p-'0';
						else
							break;
						p++;
					}
					*pszString++ = c;
					break;
				default:
					*pszString++ = c;
					p--;
					break;
			}
			p++;
		}
	}
	*pszString = 0;
}

// [BC] This essentially does the reverse of V_ColorizeString(). It takes a string with
// color codes and puts it back in \c<color code> format.
void V_UnColorizeString( char *pszString, ULONG ulMaxStringLength )
{
	char	*p;
	char	c;
	ULONG	ulCurStringLength;

	ulCurStringLength = strlen( pszString );

	p = pszString;
	while ( c = *p++ )
	{
		if ( c == TEXTCOLOR_ESCAPE )
		{
			ULONG	ulPos;

			ulCurStringLength++;
			if ( ulCurStringLength > ulMaxStringLength )
			{
				pszString++;
				continue;
			}

			for ( ulPos = strlen( pszString ) + 1; ulPos > 0; ulPos-- )
				pszString[ulPos] = pszString[ulPos - 1];

			pszString[0] = '\\';
			pszString[1] = 'c';
		}

		pszString++;
	}
	*pszString = 0;
}

// [BC] This strips color codes from a string.
void V_RemoveColorCodes( char *pszString )
{
	char *p;
	char c;

	p = pszString;
	while ( c = *p++ )
	{
		if ( c == TEXTCOLOR_ESCAPE )
		{
			ULONG	ulPos;
			ULONG	ulStringLength;

			ulStringLength = strlen( pszString );

			// If there aren't 3 characters left (the color character, the color code,
			// and the new text), just terminate the string where the color character is.
			if ( ulStringLength < 3 )
			{
				pszString[0] = 0;
				return;
			}

			for ( ulPos = 0; ulPos < ulStringLength; ulPos++ )
				pszString[ulPos] = pszString[ulPos + 2];

			p--;
		}
		else
			pszString++;
	}
	*pszString = 0;
}

//
// Break long lines of text into multiple lines no longer than maxwidth pixels
//
static void breakit (FBrokenLines *line, FFont *font, const BYTE *start, const BYTE *stop, FString &linecolor)
{
	if (!linecolor.IsEmpty())
	{
		line->Text = TEXTCOLOR_ESCAPE;
		line->Text += linecolor;
	}
	line->Text.AppendCStrPart ((const char *)start, stop - start);
	line->Width = font->StringWidth (line->Text);
}

FBrokenLines *V_BreakLines (FFont *font, int maxwidth, const BYTE *string)
{
	FBrokenLines lines[128];	// Support up to 128 lines (should be plenty)

	const BYTE *space = NULL, *start = string;
	int i, c, w, nw;
	FString lastcolor, linecolor;
	bool lastWasSpace = false;
	int kerning = font->GetDefaultKerning ();

	i = w = 0;

	while ( (c = *string++) && i < 128 )
	{
		if (c == TEXTCOLOR_ESCAPE)
		{
			if (*string)
			{
				if (*string == '[')
				{
					const BYTE *start = string;
					while (*string != ']' && *string != '\0')
					{
						string++;
					}
					if (*string != '\0')
					{
						string++;
					}
					lastcolor = FString((const char *)start, string - start);
				}
				else
				{
					lastcolor = *string++;
				}
			}
			continue;
		}

		if (isspace(c)) 
		{
			if (!lastWasSpace)
			{
				space = string - 1;
				lastWasSpace = true;
			}
		}
		else
		{
			lastWasSpace = false;
		}

		nw = font->GetCharWidth (c);

		if ((w > 0 && w + nw > maxwidth) || c == '\n')
		{ // Time to break the line
			if (!space)
				space = string - 1;

			breakit (&lines[i], font, start, space, linecolor);
			if (c == '\n')
			{
				lastcolor = "";		// Why, oh why, did I do it like this?
			}
			linecolor = lastcolor;

			i++;
			w = 0;
			lastWasSpace = false;
			start = space;
			space = NULL;

			while (*start && isspace (*start) && *start != '\n')
				start++;
			if (*start == '\n')
				start++;
			else
				while (*start && isspace (*start))
					start++;
			string = start;
		}
		else
		{
			w += nw + kerning;
		}
	}

	// String here is pointing one character after the '\0'
	if (i < 128 && --string - start >= 1)
	{
		const BYTE *s = start;

		while (s < string)
		{
			// If there is any non-white space in the remainder of the string, add it.
			if (!isspace (*s++))
			{
				breakit (&lines[i++], font, start, string, linecolor);
				break;
			}
		}
	}

	// Make a copy of the broken lines and return them
	FBrokenLines *broken = new FBrokenLines[i+1];

	for (c = 0; c < i; ++c)
	{
		broken[c] = lines[c];
	}
	broken[c].Width = -1;

	return broken;
}

void V_FreeBrokenLines (FBrokenLines *lines)
{
	if (lines)
	{
		delete[] lines;
	}
}
