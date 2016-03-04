/*
** OpenGLVideo.h
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

#ifndef __OPENGLVIDEO_H__
#define __OPENGLVIDEO_H__

#include "win32iface.h"
#include <gl/gl.h>
#include <gl/glu.h>

//*****************************************************************************
//	STRUCTURES

typedef struct _VIDEOMODEINFO_s
{
	// Pointer to the next mode in the list.
	struct _VIDEOMODEINFO_s	*pNext;

	// Width, height, bit, and hz information for this mode.
	ULONG	ulWidth;
	ULONG	ulHeight;
	ULONG	ulBPP;
	ULONG	ulHz;

} VIDEOMODEINFO_s;

//*****************************************************************************
class OpenGLVideo : public IVideo
{
public:

	OpenGLVideo( );
	~OpenGLVideo( );

	// Inherited functions from IVideo.
	EDisplayType	GetDisplayType( ) { return ( DISPLAY_Both ); }
	void			SetWindowedScale( float fScale );
	
	DFrameBuffer	*CreateFrameBuffer( int iWidth, int iHeight, bool bFullScreen, DFrameBuffer *pOldFrameBuffer );
	
	bool			FullscreenChanged( bool bFullScreen );
	int				GetModeCount( void );
	void			StartModeIterator( int iBPP );
	bool			NextMode( int *piWidth, int *piHeight, bool *bFullScreen );

	// New functions.
	void			BlankForGDI( void );

	bool			GoFullscreen( bool bYes );
	bool			PaintToWindow( void );
	ULONG			GetRefreshRate( ULONG ulWidth, ULONG ulHeight, ULONG ulBPP );

private:

	// Why is there a pointer back at this class?
	OpenGLVideo			*m_pSelf;

	// List of video modes.
	VIDEOMODEINFO_s		*m_pVideoModeList;

	// Iterators for NextMode().
	VIDEOMODEINFO_s		*m_pIteratorVideoMode;
	ULONG				m_ulIteratorBPP;

	// How wide is our current screen?
	ULONG				m_ulDisplayWidth;

	// How tall is our current screen?
	ULONG				m_ulDisplayHeight;

	// What is our current bits per pixel?
	ULONG				m_ulDisplayBPP;

	// Are we in fullscreen?
	bool				m_bFullScreen;

	// How many times have we tried to create the frame buffer?
	ULONG				m_ulFBCreateAttemptCounter;

	// Create the list of possible video modes.
	void MakeModesList( void );

	// Free that list.
	void FreeModesList( void );

	// Add a possible video mode to the list.
	void AddMode( ULONG ulWidth, ULONG ulHeight, ULONG ulBPP, ULONG ulHz );
};

//*****************************************************************************
class OpenGLFrameBuffer : public BaseWinFB
{
public:

	OpenGLFrameBuffer( ULONG ulWidth, ULONG ulHeight, bool bFullScreen );
	~OpenGLFrameBuffer( );

	// Inherited functions from BaseWinFB.
	bool	IsFullscreen( );
	void	Blank( );
	bool	PaintToWindow( );

	// New functions.
	void	Update( );
	void	GetFlashedPalette( PalEntry aPal[256] );
	void	UpdatePalette( );
	void	GetFlash( PalEntry &pRGB, int &piAmount );

	virtual void Dim( ) const;
	virtual void Clear( int iLeft, int iTop, int iRight, int iBottom, int iColor ) const;

	bool	Lock( );
	bool	Lock( bool bBuffer );
	bool	Relock( );

	bool	SetGamma( float fGamma );
	bool	GetGammaRamp( void *pv );

   bool SetFlash(PalEntry rgb, int amount);
   bool MultiTexture() { return m_useMultiTexture; }
   int NumTexelUnits() { return m_maxTexelUnits; }
   bool UseVBO();
   bool SupportsFragmentPrograms() { return m_supportsFragmentProgram; }

   int GetPageCount();
	void	PaletteChanged( );
	int		QueryNewPalette( );
   int GetBitdepth();

   PalEntry *GetPalette();
   HRESULT GetHR();

protected:
   bool CreateResources() { return true; }
   void ReleaseResources() {}

private:

	// Handle to the screen's device context.
	HDC m_hDC;

	// Handle to the screen's OpenGL rendering context.
	HGLRC m_hRC;

   HRESULT m_lastHR;
	PalEntry		m_SourcePalette[256];
	PALETTEENTRY m_palEntries[256];
	PalEntry			m_Flash;

	bool		m_bNeedPalUpdate;

	bool		m_bSupportsGamma;
	
	bool		m_supportsFragmentProgram;

	bool		m_supportsVBO;

	LONG		m_lFlashAmount;
	
	int			m_maxTexelUnits;

	bool		m_useMultiTexture;

	// How wide is our current screen?
	ULONG		m_ulDisplayWidth;

	// How tall is our current screen?
	ULONG		m_ulDisplayHeight;

	// What is our current bits per pixel?
	ULONG		m_ulDisplayBPP;

	// Are we in fullscreen?
	bool		m_bFullScreen;

	BYTE		m_GammaTable[256];
	WORD		m_OriginalGamma[768];

	void		SetupPixelFormat( );
   void loadLightInfo();

   // Let the OpenGLVideo class access member variables of this class.
   friend OpenGLVideo;
};

#endif //__OPENGLVIDEO_H__