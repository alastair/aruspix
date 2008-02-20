//------------------------------------------------------------------------------
// Source code generated by wxDesigner from file: axabout.wdr
// Do not modify this file, all changes will be lost!
//------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "axabout_wdr.h"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// Include private header
#include "axabout_wdr.h"

#include <wx/intl.h>

// Euro sign hack of the year
#if wxUSE_UNICODE
    #define __WDR_EURO__ wxT("\u20ac")
#else
    #if defined(__WXMAC__)
        #define __WDR_EURO__ wxT("\xdb")
    #elif defined(__WXMSW__)
        #define __WDR_EURO__ wxT("\x80")
    #else
        #define __WDR_EURO__ wxT("\xa4")
    #endif
#endif

// Implement window functions

wxSizer *AboutDlgFunc( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer *item2 = new wxBoxSizer( wxVERTICAL );

    wxStaticBitmap *item3 = new wxStaticBitmap( parent, ID0_LOGO, Void( 0 ), wxDefaultPosition, wxSize(128,128) );
    item2->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item4 = new wxButton( parent, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    wxFlexGridSizer *item5 = new wxFlexGridSizer( 1, 0, 0 );

    wxBoxSizer *item6 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item7 = new wxStaticText( parent, ID0_TEXT, _("ARUSPIX"), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add( item7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP, 5 );

    wxStaticText *item8 = new wxStaticText( parent, TX_APP_VERSION, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
    item6->Add( item8, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT|wxTOP, 5 );

    item5->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0 );

    wxStaticLine *item9 = new wxStaticLine( parent, ID0_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item5->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );

    wxBoxSizer *item10 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item11 = new wxStaticText( parent, ID0_TEXT, _("Build"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item11, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP, 5 );

    wxStaticText *item12 = new wxStaticText( parent, TX_APP_BUILD, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item10->Add( item12, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT|wxTOP, 5 );

    item5->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0 );

    wxStaticLine *item13 = new wxStaticLine( parent, ID0_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item5->Add( item13, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );

    wxStaticText *item14 = new wxStaticText( parent, ID0_TEXT, _("Credits"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item14, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP, 5 );

    wxWindow *item15 = parent->FindWindow( ID0_WXWIDGETS );
    wxASSERT( item15 );
    item5->Add( item15, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );

    wxWindow *item16 = parent->FindWindow( ID0_TORCH );
    wxASSERT( item16 );
    item5->Add( item16, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );

    wxWindow *item17 = parent->FindWindow( ID0_IM );
    wxASSERT( item17 );
    item5->Add( item17, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );

    wxWindow *item18 = parent->FindWindow( ID0_WG );
    wxASSERT( item18 );
    item5->Add( item18, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );

    item5->Add( 20, 5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item19 = new wxStaticText( parent, ID0_TEXT, _("Copyright 2004-2008 Laurent Pugin"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item19, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );

    wxStaticText *item20 = new wxStaticText( parent, ID0_TEXT, _("All Rights Reserved"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item21 = new wxBoxSizer( wxHORIZONTAL );

    item5->Add( item21, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0 );

    item1->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}

// Implement menubar functions

// Implement toolbar functions

// Implement bitmap functions

wxBitmap Void( size_t index )
{
    if (index == 0)
    {
        /* XPM */
        static const char *xpm_data[] = {
        /* columns rows colors chars-per-pixel */
        "16 15 1 1",
        "a c Black",
        /* pixels */
        "aaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaa"
        };
        wxBitmap bitmap( xpm_data );
        return bitmap;
    }
    return wxNullBitmap;
}


// End of generated file
