/////////////////////////////////////////////////////////////////////////////
// Name:        wgviewer.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_WGVIEWER

#ifdef __GNUG__
    #pragma implementation "wgviewer.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/dirctrl.h"
#include "wx/calctrl.h"

#include "wgviewer.h"
#include "app/axframe.h"
#include "app/axapp.h"

#include "mus/muswindow.h"
#include "mus/musfile.h"
#include "mus/mustoolpanel.h"
#include "mus/musiowwg.h"
#include "mus/musiomlf.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// WgvPanel
//----------------------------------------------------------------------------

// WDR: event table for WgvPanel

BEGIN_EVENT_TABLE(WgvPanel,wxPanel)
    //EVT_TREE_SEL_CHANGED( ID0_DIRECTORIES, WgvPanel::OnTree )
    EVT_SIZE( WgvPanel::OnSize )
END_EVENT_TABLE()

WgvPanel::WgvPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{

}

// WDR: handler implementations for WgvPanel

void WgvPanel::OnTree( wxTreeEvent &event )
{
    wxMessageBox("TREE");
}

void WgvPanel::OnSize( wxSizeEvent &event )
{
	this->Layout();
    wxWindow *w = FindWindowById( ID5_WGWINDOW );
    if ( w && w->IsKindOf( CLASSINFO( MusWindow ) ))
        ((MusWindow*)w)->Resize( );

    event.Skip();
}

//----------------------------------------------------------------------------
// WgvEnv
//----------------------------------------------------------------------------

// WDR: event table for WgvEnv

BEGIN_EVENT_TABLE(WgvEnv,AxEnv)
    EVT_MENU( ID_OPEN, WgvEnv::OnOpen )
    EVT_MENU( ID_SAVE, WgvEnv::OnSave )
    EVT_UPDATE_UI_RANGE( wxID_LOWEST, ENV_IDS_MAX, WgvEnv::OnUpdateUI )
	EVT_MENU( ID_UNDO, WgvEnv::OnUndo )
	EVT_MENU( ID_REDO, WgvEnv::OnRedo )
    EVT_MENU( ID5_NEXT, WgvEnv::OnNext )
    EVT_MENU( ID5_PREVIOUS, WgvEnv::OnPrevious )
    EVT_MENU( ID5_GOTO, WgvEnv::OnGoto )
    EVT_MENU( ID5_ZOOMOUT, WgvEnv::OnZoomOut )
    EVT_MENU( ID5_ZOOMIN, WgvEnv::OnZoomIn )
	EVT_MENU( ID5_SAVE_MODEL, WgvEnv::OnSaveModel )
    EVT_MENU( ID5_SAVE_MLF, WgvEnv::OnSaveMLF )
    EVT_MENU( ID5_OPEN_MLF, WgvEnv::OnOpenMLF )
    EVT_MENU_RANGE( ID5_INSERT_MODE, ID5_SYMBOLES, WgvEnv::OnTools )
	//EVT_SIZE( WgvEnv::OnSize )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(WgvEnv,AxEnv)

WgvEnv::WgvEnv():
    AxEnv()
{
    m_panelPtr = NULL;
    m_filePtr = NULL;
    m_musViewPtr = NULL;

    this->m_envMenuBarFuncPtr = MenuBarFunc5;
}

WgvEnv::~WgvEnv()
{
    if (m_envWindowPtr) m_envWindowPtr->Destroy();
    m_envWindowPtr = NULL;

    if ( m_filePtr )
        delete m_filePtr;
}

void WgvEnv::LoadWindow()
{
    this->m_envWindowPtr = new wxPanel(m_framePtr,-1);

    //m_panelPtr = ((WgvPanel*)m_envWindowPtr)->GetWgPanel();
	//
	WindowFunc5( m_envWindowPtr, true );
	m_panelPtr = (WgvPanel*)m_envWindowPtr->FindWindowById( ID5_MUSPANEL );
    wxASSERT_MSG( m_panelPtr, "WG Panel cannot be NULL ");
    
    if ( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) == *wxWHITE )
        m_panelPtr->SetBackgroundColour( *wxLIGHT_GREY );
    else
        m_panelPtr->SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) );
}


void WgvEnv::RealizeToolbar( )
{
	ToolBarFunc5( m_framePtr->GetToolBar() ); // function generated by wxDesigner
}


// WDR: handler implementations for WgvEnv

void WgvEnv::OnSaveModel( wxCommandEvent &event )
{
	wxLogMessage(" doesn't exists anymore.... pfff");
}

void WgvEnv::OnSize( wxSizeEvent &event )
{

    event.Skip();
}

void WgvEnv::OnTools( wxCommandEvent &event )
{
	if ( !m_musViewPtr )
		return;

	if ( event.GetId() == ID5_INSERT_MODE )
		m_musViewPtr->SetInsertMode( event.IsChecked() );
 	else if ( event.GetId() == ID5_NOTES )
		m_musViewPtr->SetToolType( MUS_TOOLS_NOTES );
	else if ( event.GetId() == ID5_KEYS)
		m_musViewPtr->SetToolType( MUS_TOOLS_CLEFS );
	else if ( event.GetId() == ID5_SIGNS )
		m_musViewPtr->SetToolType( MUS_TOOLS_SIGNS );
	else if ( event.GetId() == ID5_SYMBOLES )
		m_musViewPtr->SetToolType( MUS_TOOLS_OTHER );
}

void WgvEnv::OnUndo( wxCommandEvent &event )
{
    if ( !m_musViewPtr || !m_musViewPtr->CanUndo() )
		return;
		
	m_musViewPtr->Undo();
}

void WgvEnv::OnRedo( wxCommandEvent &event )
{
    if ( !m_musViewPtr || !m_musViewPtr->CanRedo() )
		return;
		
	m_musViewPtr->Redo();
}


void WgvEnv::OnSave( wxCommandEvent &event )
{
    if (  !m_panelPtr || !m_filePtr )
        return;

    wxString wwg = wxFileSelector( _("Save"), wxGetApp().m_lastDir, _T(""), NULL, "WWG|*.wwg", wxSAVE);
    if ( wwg.IsEmpty() )
        return;
    //wxGetApp().m_lastDirWWG_out = wxPathOnly( wwg );   
    
    m_filePtr->m_fname = wwg;
    MusWWGOutput *wwgoutput = new MusWWGOutput( m_filePtr, wwg );
    wwgoutput->ExportFile();
    delete wwgoutput;
}

void WgvEnv::OnOpenMLF( wxCommandEvent &event )
{
    wxString mlf = wxFileSelector( _("Open"), wxGetApp().m_lastDir, _T(""), NULL, "MLF|*.mlf", wxOPEN);
    if ( mlf.IsEmpty() )
        return;
    //wxGetApp().m_lastDirMLF_in = wxPathOnly( mlf );

    if ( m_musViewPtr )
    {
        m_musViewPtr->Destroy();
        m_musViewPtr = NULL;
    }

    if ( m_filePtr )
    {
        delete m_filePtr;
        m_filePtr = NULL;
    }
    
    m_filePtr = new MusFile();
	//MusPage *page = new MusPage();
	//MusStaff *staff = new MusStaff();
	//page->m_staves.Add( staff );
	//m_filePtr->m_pages.Add( page );
    MusMLFInput *mlfinput = new MusMLFInput( m_filePtr, mlf );
    bool import = mlfinput->ImportFile( 12 );
    delete mlfinput;

    if ( !import )
    {   
        wxLogError( _("Importation of file '%s' failed") , mlf.c_str() );
        return;
    }

    m_musViewPtr = new MusWindow( m_panelPtr, ID5_WGWINDOW, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL|wxSIMPLE_BORDER );
    m_musViewPtr->SetFile( m_filePtr );
    m_musViewPtr->Resize( );
}

void WgvEnv::OnSaveMLF( wxCommandEvent &event )
{
    wxASSERT_MSG( m_panelPtr, "Panel cannot be NULL ");

	wxFileName fn( m_filePtr->m_fname );
	fn.SetExt( "mlf" );
	fn.SetPath( wxGetApp().m_lastDir );

    wxString filename = wxFileSelector( _("Save"), wxGetApp().m_lastDir, fn.GetFullPath() , NULL, "MLF|*.mlf", wxSAVE);
    if ( filename.IsEmpty() )
        return;
    //wxGetApp().m_lastDirMLF_out = wxPathOnly( filename );

    wxString dic = wxFileSelector( _("Open dictionnay"), wxGetApp().m_lastDir, _T(""), NULL, "DIC|*.dic", wxOPEN);
    if ( dic.IsEmpty() )
        return;
    //wxGetApp().m_lastDirDIC_in = wxPathOnly( dic );

    bool positions = ( wxMessageBox("Output symboles positions?", wxGetApp().GetAppName() ,
                            wxYES | wxNO | wxICON_QUESTION ) == wxYES );

    MusMLFOutput *mlfoutput = new MusMLFOutput( m_filePtr, filename, NULL, "MusMLFSymbol" );
	//mlfoutput->LoadTypes( dic );
	mlfoutput->m_pagePosition = positions;
    mlfoutput->m_addPageNo = true;
    
    mlfoutput->ExportFile();
    delete mlfoutput; 
}


void WgvEnv::OnZoomOut( wxCommandEvent &event )
{
    if ( !m_musViewPtr )
        return;
    if ( m_musViewPtr->CanZoom( false ) )
        m_musViewPtr->Zoom( false );
}

void WgvEnv::OnZoomIn( wxCommandEvent &event )
{
    if ( !m_musViewPtr )
        return;
    if (m_musViewPtr->CanZoom( true ) )
        m_musViewPtr->Zoom( true );    
}

void WgvEnv::OnGoto( wxCommandEvent &event )
{
    if ( !m_musViewPtr )
        return;
    if (m_musViewPtr->CanGoto(  ) )
        m_musViewPtr->Goto(  );    
}

void WgvEnv::OnPrevious( wxCommandEvent &event )
{
    if ( !m_musViewPtr )
        return;
    if (m_musViewPtr->HasNext( false ) )
        m_musViewPtr->Next( false );    
}

void WgvEnv::OnNext( wxCommandEvent &event )
{
    if ( !m_musViewPtr )
        return;
    if (m_musViewPtr->HasNext( true ) )
        m_musViewPtr->Next( true );    
}

void WgvEnv::OnUpdateUI( wxUpdateUIEvent &event )
{
    if ( event.GetId() == ID_OPEN )
        event.Enable(true);
    else if ( event.GetId() == ID5_OPEN_MLF )
        event.Enable( true );

    else if ( !m_musViewPtr )
        event.Enable(false);
    else if ( event.GetId() == ID_UNDO )
        event.Enable( m_musViewPtr->CanUndo() );
    else if ( event.GetId() == ID_REDO )
        event.Enable( m_musViewPtr->CanRedo() );
    else if ( event.GetId() == ID5_GOTO )
        event.Enable( m_musViewPtr->CanGoto() );
    else if ( event.GetId() == ID5_PREVIOUS )
        event.Enable( m_musViewPtr->HasNext( false ) );
    else if ( event.GetId() == ID5_NEXT )
        event.Enable( m_musViewPtr->HasNext( true ) );
    else if ( event.GetId() == ID5_ZOOMOUT )
        event.Enable( m_musViewPtr->CanZoom( false ) );
    else if ( event.GetId() == ID5_ZOOMIN )
        event.Enable( m_musViewPtr->CanZoom( true ) );
    else if ( event.GetId() == ID5_SAVE_MLF )
        event.Enable( m_musViewPtr != NULL );
	else if ( event.GetId() == ID5_INSERT_MODE )
	{
		event.Enable( true );
		event.Check( m_musViewPtr && !m_musViewPtr->m_editElement );
	}
    else
        event.Enable(true);
}

void WgvEnv::OnOpen( wxCommandEvent &event )
{
    wxASSERT_MSG( m_panelPtr, "Panel cannot be NULL ");


    wxString filename = wxFileSelector( _("Open"), wxGetApp().m_lastDir, _T(""), NULL, "WWG|*.wwg", wxOPEN);
    if ( filename.IsEmpty() )
        return;
    //wxGetApp().m_lastDirWWG_in = wxPathOnly( filename );
    
    if ( m_filePtr )
    {
        delete m_filePtr;
        m_filePtr = NULL;
    }
    m_filePtr = new MusFile();
    m_filePtr->m_fname = filename;
    MusWWGInput wwginput( m_filePtr, filename );
	if ( !wwginput.ImportFile() )
		return;

    /*
    MusWWGOutput *wwgoutput = new MusWWGOutput( m_filePtr, filename );
    wwgoutput->ExportFile();
    delete wwgoutput;*/
    

    if ( m_musViewPtr )
    {
        m_musViewPtr->Destroy();
        m_musViewPtr = NULL;
    }
    m_musViewPtr = new MusWindow( m_panelPtr, ID5_WGWINDOW, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL|wxSIMPLE_BORDER );
    m_musViewPtr->SetToolPanel( ((WgvPanel*)m_envWindowPtr)->GetWgToolPanel() );
    m_musViewPtr->SetFile( m_filePtr );
    m_musViewPtr->Resize( );
}

#endif // AX_WGVIEWER

