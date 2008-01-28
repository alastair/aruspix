/////////////////////////////////////////////////////////////////////////////
// Name:        mustoolpanel.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_TOOLPANEL_H__
#define __MUS_TOOLPANEL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "mustoolpanel.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//#include "wx/bmpbuttn.h"

#include "mus_wdr.h"

#define MUS_TOOLS_NOTES 0
#define MUS_TOOLS_CLEFS 1
#define MUS_TOOLS_SIGNS 2
#define MUS_TOOLS_OTHER 3

#define MUS_TOOLS_NUMBER 4 // Nombre de type de symbols

#define WG_MODE_EDIT 0
#define WG_MODE_INSERT 1

class MusWindow;

// WDR: class declarations


//----------------------------------------------------------------------------
// MusToolRow
//----------------------------------------------------------------------------

class MusToolRow: public wxPanel
{
public:
    // constructors and destructors
    MusToolRow( wxWindow *parent, wxWindowID id, int type );

    void UpdateTools( bool edition );
    
    // WDR: method declarations for MusToolRow
private:
    // WDR: member variable declarations for MusToolRow
	int m_type; // row type;
	bool m_previous_edition;
	wxBitmapButton *m_buttons[MUS_TOOLS_NUMBER + 1]; // +1 pour le bouton insert

public:
    
private:
    // WDR: handler declarations for MusToolRow

private:
};


//----------------------------------------------------------------------------
// MusToolPanel
//----------------------------------------------------------------------------

class MusToolPanel: public wxPanel
{
public:
    // constructors and destructors
    MusToolPanel( wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL );

    void SetWgWindow( MusWindow *w );
    void SetTools( int tools, bool edition );
    
    // WDR: method declarations for MusToolPanel

private:
    void SendEvent( wxKeyEvent event );
    
private:
    // WDR: member variable declarations for MusToolPanel
    MusWindow *m_w;
	// panels
	MusToolRow *m_rows[4];
    // sizers used to vertical / horizontal change
	bool m_previous_edition;
	int m_previous_tools;


public:
    
private:
    // WDR: handler declarations for MusToolPanel
	void OnChangeMode( wxCommandEvent &event );
	void OnChangeTool( wxCommandEvent &event );
    void OnSymbole( wxCommandEvent &event );
    void OnSign( wxCommandEvent &event );
    void OnKey( wxCommandEvent &event );
    void OnNote( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};


#endif