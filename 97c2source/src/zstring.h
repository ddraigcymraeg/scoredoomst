#ifndef ZSTRING_H
#define ZSTRING_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include "tarray.h"

#ifdef __GNUC__
#define PRINTFISH(x) __attribute__((format(printf, 2, x)))
#else
#define PRINTFISH(x)
#endif

struct FStringData
{
	unsigned int Len;		// Length of string, excluding terminating null
	unsigned int AllocLen;	// Amount of memory allocated for string
	int RefCount;			// < 0 means it's locked
	// char StrData[xxx];

	char *Chars()
	{
		return (char *)(this + 1);
	}

	const char *Chars() const
	{
		return (const char *)(this + 1);
	}

	char *AddRef()
	{
		if (RefCount < 0)
		{
			return (char *)(MakeCopy() + 1);
		}
		else
		{
			RefCount++;
			return (char *)(this + 1);
		}
	}

	void Release()
	{
		assert (RefCount != 0);

		if (--RefCount <= 0)
		{
			Dealloc();
		}
	}

	FStringData *MakeCopy();

	static FStringData *Alloc (size_t strlen);
	FStringData *Realloc (size_t newstrlen);
	void Dealloc ();
};

struct FNullStringData
{
	unsigned int Len;
	unsigned int AllocLen;
	int RefCount;
	char Nothing[2];
};

class FString
{
public:
	FString () : Chars(&NullString.Nothing[0]) { NullString.RefCount++; }

	// Copy constructors
	FString (const FString &other) { AttachToOther (other); }
	FString (const char *copyStr);
	FString (const char *copyStr, size_t copyLen);
	FString (char oneChar);

	// Concatenation constructors
	FString (const FString &head, const FString &tail);
	FString (const FString &head, const char *tail);
	FString (const FString &head, char tail);
	FString (const char *head, const FString &tail);
	FString (const char *head, const char *tail);
	FString (char head, const FString &tail);

	~FString ();

	char *LockBuffer();		// Obtain write access to the character buffer
	void UnlockBuffer();	// Allow shared access to the character buffer

	operator const char *() const { return Chars; }

	const char *GetChars() const { return Chars; }
	const char &operator[] (int index) const { return Chars[index]; }
	const char &operator[] (size_t index) const { return Chars[index]; }

	FString &operator = (const FString &other);
	FString &operator = (const char *copyStr);

	FString operator + (const FString &tail) const;
	FString operator + (const char *tail) const;
	FString operator + (char tail) const;
	friend FString operator + (const char *head, const FString &tail);
	friend FString operator + (char head, const FString &tail);

	FString &operator += (const FString &tail);
	FString &operator += (const char *tail);
	FString &operator += (char tail);
	FString &AppendCStrPart (const char *tail, size_t tailLen);

	FString Left (size_t numChars) const;
	FString Right (size_t numChars) const;
	FString Mid (size_t pos, size_t numChars) const;

	long IndexOf (const FString &substr, long startIndex=0) const;
	long IndexOf (const char *substr, long startIndex=0) const;
	long IndexOf (char subchar, long startIndex=0) const;

	long IndexOfAny (const FString &charset, long startIndex=0) const;
	long IndexOfAny (const char *charset, long startIndex=0) const;

	long LastIndexOf (const FString &substr) const;
	long LastIndexOf (const char *substr) const;
	long LastIndexOf (char subchar) const;
	long LastIndexOf (const FString &substr, long endIndex) const;
	long LastIndexOf (const char *substr, long endIndex) const;
	long LastIndexOf (char subchar, long endIndex) const;
	long LastIndexOf (const char *substr, long endIndex, size_t substrlen) const;

	long LastIndexOfAny (const FString &charset) const;
	long LastIndexOfAny (const char *charset) const;
	long LastIndexOfAny (const FString &charset, long endIndex) const;
	long LastIndexOfAny (const char *charset, long endIndex) const;

	void ToUpper ();
	void ToLower ();
	void SwapCase ();

	void StripLeft ();
	void StripLeft (const FString &charset);
	void StripLeft (const char *charset);

	void StripRight ();
	void StripRight (const FString &charset);
	void StripRight (const char *charset);

	void StripLeftRight ();
	void StripLeftRight (const FString &charset);
	void StripLeftRight (const char *charset);

	void Insert (size_t index, const FString &instr);
	void Insert (size_t index, const char *instr);
	void Insert (size_t index, const char *instr, size_t instrlen);

	void ReplaceChars (char oldchar, char newchar);
	void ReplaceChars (const char *oldcharset, char newchar);

	void StripChars (char killchar);
	void StripChars (const char *killchars);

	void MergeChars (char merger);
	void MergeChars (char merger, char newchar);
	void MergeChars (const char *charset, char newchar);

	void Substitute (const FString &oldstr, const FString &newstr);
	void Substitute (const char *oldstr, const FString &newstr);
	void Substitute (const FString &oldstr, const char *newstr);
	void Substitute (const char *oldstr, const char *newstr);
	void Substitute (const char *oldstr, const char *newstr, size_t oldstrlen, size_t newstrlen);

	void Format (const char *fmt, ...) PRINTFISH(3);
	void AppendFormat (const char *fmt, ...) PRINTFISH(3);
	void VFormat (const char *fmt, va_list arglist) PRINTFISH(0);
	void VAppendFormat (const char *fmt, va_list arglist) PRINTFISH(0);

	bool IsInt () const;
	bool IsFloat () const;
	long ToLong (int base=0) const;
	unsigned long ToULong (int base=0) const;
	double ToDouble () const;

	size_t Len() const { return Data()->Len; }
	bool IsEmpty() const { return Len() == 0; }

	void Truncate (long newlen);

	int Compare (const FString &other) const { return strcmp (Chars, other.Chars); }
	int Compare (const char *other) const { return strcmp (Chars, other); }

	int CompareNoCase (const FString &other) const { return stricmp (Chars, other.Chars); }
	int CompareNoCase (const char *other) const { return stricmp (Chars, other); }

protected:
	const FStringData *Data() const { return (FStringData *)Chars - 1; }
	FStringData *Data() { return (FStringData *)Chars - 1; }

	void AttachToOther (const FString &other);
	void AllocBuffer (size_t len);
	void ReallocBuffer (size_t newlen);

	static int FormatHelper (void *data, const char *str, int len);
	static void StrCopy (char *to, const char *from, size_t len);
	static void StrCopy (char *to, const FString &from);

	char *Chars;

	static FNullStringData NullString;

	friend struct FStringData;
};

namespace StringFormat
{
	enum
	{
		// Format specification flags
		F_MINUS		= 1,
		F_PLUS		= 2,
		F_ZERO		= 4,
		F_BLANK		= 8,
		F_HASH		= 16,

		F_SIGNED	= 32,
		F_NEGATIVE	= 64,
		F_ZEROVALUE	= 128,

		// Format specification size prefixes
		F_HALFHALF	= 0x1000,	// hh
		F_HALF		= 0x2000,	// h
		F_LONG		= 0x3000,	// l
		F_LONGLONG	= 0x4000,	// ll or I64
		F_BIGI		= 0x5000	// I
	};
	typedef int (*OutputFunc)(void *data, const char *str, int len);

	int VWorker (OutputFunc output, void *outputData, const char *fmt, va_list arglist);
	int Worker (OutputFunc output, void *outputData, const char *fmt, ...);
};

#undef PRINTFISH

#endif
