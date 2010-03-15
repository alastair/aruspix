/////////////////////////////////////////////////////////////////////////////
// Name:        muspage.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "muspage.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/tokenzr.h"


#include "muspage.h"
#include "musstaff.h"
#include "muselement.h"
#include "mussymbol.h"
#include "musnote.h"
#include "muswindow.h"
#include "mus.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusStaves );


// WDR: class implementations

//----------------------------------------------------------------------------
// MusPage
//----------------------------------------------------------------------------

MusPage::MusPage() :
	MusObject()
{
	Clear( );
}

MusPage::~MusPage()
{
}

void MusPage::Clear( )
{
	m_staves.Clear( );
	nbrePortees = 0;
	noMasqueFixe = false;
	noMasqueVar = false;
	reserve = 0;
	defin = 20;
	indent = 0;
	indentDroite = 0;
	lrg_lign = 190;
	nbrePortees = 0;
	npage = 0;
}

void MusPage::CheckIntegrity()
{
	this->nbrePortees = (int)this->m_staves.GetCount();

	MusStaff *staff;
	int i;
	int system = 1;
    for (i = 0; i < nbrePortees; i++) 
	{
		staff = &m_staves[i];
		staff->no = i;
		staff->noLigne = system;
		if ((staff->vertBarre == 0) || (staff->vertBarre == FIN))
			system++;
        staff->CheckIntegrity();
	}
}

/*
void MusPage::ClearStaves( wxDC *dc, MusStaff *start )
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;

	int j;
	for(j = 0; j < nbrePortees; j++)
	{
		if (start && (start != &this->m_staves[j]))
			continue;
		else
			start = NULL;
		(&this->m_staves[j])->ClearElements( dc );
	}
}
*/

MusStaff *MusPage::GetFirst( )
{
	if ( m_staves.IsEmpty() )
		return NULL;
	return &m_staves[0];
}

MusStaff *MusPage::GetLast( )
{
	if ( m_staves.IsEmpty() )
		return NULL;
	int i = (int)m_staves.GetCount() - 1;
	return &m_staves[i];
}

MusStaff *MusPage::GetNext( MusStaff *staff )
{
	if ( !staff || m_staves.IsEmpty() || ( staff->no >= (int)m_staves.GetCount() - 1 ) )
		return NULL;

	//wxASSERT_MSG( m_staves.Index( *staff) != wxNOT_FOUND, 
	//	"\nLa portee recherchee n'appartient pas � la page" );

	return &m_staves[staff->no + 1];
}

MusStaff *MusPage::GetPrevious( MusStaff *staff  )
{
	if ( !staff || m_staves.IsEmpty() || ( staff->no <= 0 ) )
		return NULL;

	//wxASSERT_MSG( m_staves.Index( *staff) != wxNOT_FOUND, 
	//	"\nLa portee recherchee n'appartient pas � la page" );

	return &m_staves[staff->no - 1];
}


MusStaff *MusPage::GetAtPos( int y )
{
	y += ( STAFF_OFFSET / 2 );
	MusStaff *staff = this->GetFirst();
	if ( !staff )
		return NULL;
	
	int dif =  abs( staff->yrel - y );
	while ( this->GetNext(staff) )
	{
		if ( (int)staff->yrel < y )
		{
			// one too much
			if ( this->GetPrevious( staff ) ){
				staff = this->GetPrevious( staff );
				if ( dif < abs( staff->yrel - y ) )
					staff = this->GetNext( staff );
			}
				
			return staff;
		}
		staff = this->GetNext( staff );
		dif = abs( staff->yrel - y );
	}

	if ( ( (int)staff->yrel < y )  && this->GetPrevious( staff ) )
		staff = this->GetPrevious( staff );

	return staff;
}

void MusPage::UpdateStavesPosition( ) 
{
	if ( !Check() )
		return;

	int i, yy, orgx;
	MusStaff *staff = NULL;

	yy = m_w->wymax;
    for (i = 0; i < nbrePortees; i++) 
	{
		staff = &m_staves[i];
        yy -= staff->ecart * m_w->_interl[ staff->pTaille ];
        m_w->kPos[i].compte = 0;

		// Calcul du TAB initial, s'il y a lieu 
		orgx = staff->indent ? staff->indent*10 : 0;
         
		// calcul du point d'ancrage des curseurs au-dessus de la ligne superieure
		m_w->kPos[i].yp = yy + m_w->_portee[ staff->pTaille ];
		staff->yrel = (int)(m_w->kPos[i].yp);
        // portees � 1 ou 4 lignes
        if (staff->portNbLine == 1)
			m_w->kPos[i].yp  += m_w->_interl[ staff->pTaille ]*2;
        else if (staff->portNbLine == 4)
			m_w->kPos[i].yp  += m_w->_interl[ staff->pTaille ];		
	}
}

void MusPage::DrawPage( wxDC *dc, bool background ) 
{
	wxASSERT_MSG( dc , "DC cannot be NULL");

	if ( !Check() )
		return;

	int i;
    MusStaff *staff;

	if (m_p->orientation)
		i =  m_w->ToZoom( m_w->pageFormatHor ); //+mrgG; marge comprise dans SetDeviceOrigine
	else
		i =  m_w->ToZoom( m_w->pageFormatHor + 50); //+mrgG;

	if ( background )
		dc->DrawRectangle( m_w->mrgG, m_w->ToZoomY(( m_w->pageFormatVer-50) + m_p->MargeSOMMET*10), i,
			m_w->ToZoomY (-50)+m_p->MargeSOMMET*10);


	// position des portees
	UpdateStavesPosition();
	// barres
	DrawBarres( dc );

    for (i = 0; i < nbrePortees; i++) 
	{
		staff = &m_staves[i];
		staff->Init( m_w );
		//wxLogDebug("staff %d yrel=%d", i, staff->yrel);
		//if (( m_w->drawRect.y > (int)staff->yrel) && ( m_w->drawRect.GetBottom() < (int)staff->yrel) )
			staff->DrawStaffLines( dc , i );		
	}

    for (i = 0; i < nbrePortees; i++) 
	{
		staff = &m_staves[i];
			staff->DrawStaff( dc , i );		
	}
}


void MusPage::SetValues( int type )
{
    int i;
    wxString values;
    for (i = 0; i < nbrePortees; i++) 
	{
        switch ( type ) {
            case PAGE_VALUES_VOICES: values += wxString::Format("%d;", (&m_staves[i])->voix ); break;
            case PAGE_VALUES_INDENT: values += wxString::Format("%d;", (&m_staves[i])->indent ); break;
        }
	}
    values = wxGetTextFromUser( "Enter values for the pages", "", values );
    if (values.Length() == 0 ) {
        return;
    }
    wxArrayString values_arr = wxStringTokenize(values, ";");
    for (i = 0; (i < nbrePortees) && (i < (int)values_arr.GetCount()) ; i++) 
	{
        switch ( type ) {
            case PAGE_VALUES_VOICES: (&m_staves[i])->voix = atoi( values_arr[i].c_str() ); break;
            case PAGE_VALUES_INDENT: (&m_staves[i])->indent = atoi( values_arr[i].c_str() ); break;
        }	
	}
}

// functors for MusPage

void MusPage::ProcessStaves( wxArrayPtrVoid params )
{
    // param 0: MusStaffFunctor
    // param 1; wxArrayPtrVoid
    
    MusStaffFunctor *staffFunctor = (MusStaffFunctor*)params[0];
    wxArrayPtrVoid *staffParams = (wxArrayPtrVoid*)params[1];
    
	int i;
    MusStaff *staff;

    for (i = 0; i < nbrePortees; i++) 
	{
		staff = &m_staves[i];
		staffFunctor->Call( staff, *staffParams );	
	}

}


void MusPage::ProcessVoices( wxArrayPtrVoid params )
{
    // param 0: MusStaffFunctor
    // param 1; wxArrayPtrVoid 
    // param 2; int (voice number)
    
    MusStaffFunctor *staffFunctor = (MusStaffFunctor*)params[0];
    wxArrayPtrVoid *staffParams = (wxArrayPtrVoid*)params[1];
    int *voice = (int*)params[2];
    
	int i;
    MusStaff *staff;

    for (i = 0; i < nbrePortees; i++) 
	{
		staff = &m_staves[i];
        if (staff->voix == (*voice)) {
            staffFunctor->Call( staff, *staffParams );
        }
	}

}

void MusPage::CountVoices( wxArrayPtrVoid params )
{
    // param 0; int (min number of voice number)
    // param 1; int (max number of voice number)
    
    int *min_voice = (int*)params[0];
    int *max_voice = (int*)params[1];
    
	int i;
    MusStaff *staff;

    for (i = 0; i < nbrePortees; i++) 
	{
		staff = &m_staves[i];
        if (staff->voix > (*max_voice)) {
           (*max_voice) = staff->voix;
        }
        if (staff->voix < (*min_voice)) {
           (*min_voice) = staff->voix;
        }
	}

}

// WDR: handler implementations for MusPage


