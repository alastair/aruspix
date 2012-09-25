/////////////////////////////////////////////////////////////////////////////
// Name:        muslaidoutstaff.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musio.h"
#include "muslaidoutstaff.h"
#include "muslaidoutlayer.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusLaidOutStaves );

//----------------------------------------------------------------------------
// MusLaidOutStaff
//----------------------------------------------------------------------------

MusLaidOutStaff::MusLaidOutStaff( int logStaffNb ):
	MusLayoutObject()
{
	Clear( );
    wxASSERT ( logStaffNb > 0 );
    m_logStaffNb = logStaffNb;
}

bool MusLaidOutStaff::Check()
{
    wxASSERT( m_system );
    return ( m_system && MusLayoutObject::Check());
}

MusLaidOutStaff::MusLaidOutStaff( const MusLaidOutStaff& staff )
{
    m_system = NULL;
	totGrp = staff.totGrp;
	//noLigne = staff.noLigne;
	armTyp = staff.armTyp;
	armNbr = staff.armNbr;
	notAnc = staff.notAnc;
	grise = staff.grise;
	invisible = staff.invisible;
	vertBarre = staff.vertBarre;
	brace = staff.brace;
	staffSize = staff.staffSize;
	portNbLine = staff.portNbLine;
	accol = staff.accol;
	accessoire = staff.accessoire;
	m_y_abs = staff.m_y_abs;
	m_y_drawing = staff.m_y_drawing;

    int i;
	for (i = 0; i < staff.GetLayerCount(); i++)
	{
        MusLaidOutLayer *nlayer = new MusLaidOutLayer( *&staff.m_layers[i] );
        nlayer->SetStaff( this );
        this->m_layers.Add( nlayer );
	}
}

MusLaidOutStaff::~MusLaidOutStaff()
{
}

void MusLaidOutStaff::Clear()
{
	m_layers.Clear();
    m_system = NULL;
	noGrp = 0;
	totGrp = 0;
	//noLigne = 0; // ax2
	armTyp = 0;
	armNbr = 0;
	notAnc = false; // LP we want modern notation :))
	grise = false;
	invisible = false;
	vertBarre = 0;
	brace = 0;
	staffSize = 0; 
	portNbLine = 5;
	accol = 0;
	accessoire = 0;
	m_y_abs = 0;
	m_y_drawing = 0;
    
    //
    //beamListPremier = NULL;
}

void MusLaidOutStaff::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];         
    output->WriteLaidOutStaff( this );
    
    // save layers
    MusLaidOutLayerFunctor layer( &MusLaidOutLayer::Save );
    this->Process( &layer, params );
}

void MusLaidOutStaff::AddLayer( MusLaidOutLayer *layer )
{
	layer->SetStaff( this );
	m_layers.Add( layer );
}

void MusLaidOutStaff::CopyAttributes( MusLaidOutStaff *nstaff )
{
	if ( !nstaff )
		return;

	nstaff->Clear();
	nstaff->noGrp = noGrp;
	nstaff->totGrp = totGrp;
	//nstaff->noLigne = noLigne;
	nstaff->armTyp = armTyp;
	nstaff->armNbr = armNbr;
	nstaff->notAnc = notAnc;
	nstaff->grise = grise;
	nstaff->invisible = invisible;
	nstaff->vertBarre = vertBarre;
	nstaff->brace = brace;
	nstaff->staffSize = staffSize;
	nstaff->portNbLine = portNbLine;
	nstaff->accol = accol;
	nstaff->accessoire = accessoire;
	nstaff->m_y_abs = m_y_abs;
	nstaff->m_y_drawing = m_y_drawing;
}

int MusLaidOutStaff::GetStaffNo() const
{
    wxASSERT_MSG( m_system, "System cannot be NULL");
    
    return m_system->m_staves.Index( *this );
}

MusLaidOutLayer *MusLaidOutStaff::GetFirst( )
{
	if ( m_layers.IsEmpty() )
		return NULL;
	return &m_layers[0];
}

MusLaidOutLayer *MusLaidOutStaff::GetLast( )
{
	if ( m_layers.IsEmpty() )
		return NULL;
	int i = GetLayerCount() - 1;
	return &m_layers[i];
}

MusLaidOutLayer *MusLaidOutStaff::GetNext( MusLaidOutLayer *layer )
{	
    if ( !layer || m_layers.IsEmpty())
        return NULL;
        
	int i = m_layers.Index( *layer );

	if ((i == wxNOT_FOUND ) || ( i >= GetLayerCount() - 1 )) 
		return NULL;

	return &m_layers[i + 1];
}

MusLaidOutLayer *MusLaidOutStaff::GetPrevious( MusLaidOutLayer *layer )
{
    if ( !layer || m_layers.IsEmpty())
        return NULL;
        
	int i = m_layers.Index( *layer );

	if ((i == wxNOT_FOUND ) || ( i <= 0 )) 
        return NULL;
	
    return &m_layers[i - 1];
}


MusLaidOutLayer *MusLaidOutStaff::GetLayer( int LayerNo )
{
    if ( LayerNo > (int)m_layers.GetCount() - 1 )
        return NULL;
	
	return &m_layers[LayerNo];
}

void MusLaidOutStaff::GetPosOnPage( wxArrayPtrVoid params )
{
    // param 0: the MusLaidOutStaff we are looking for
    // param 1: the position on the page (int)
    // param 2; the success flag (bool)
    MusLaidOutStaff *staff = (MusLaidOutStaff*)params[0];
	int *position = (int*)params[1];
    bool *success = (bool*)params[2];
    
    if ( (*success) ) {
        return;
    } 
    (*position)++;
    if ( this == staff ) {
        (*success) = true;
        return;
    }
}

void MusLaidOutStaff::UpdateYPosition( wxArrayPtrVoid params )
{
    // param 0: the MusLayerElement we point to
	int *current_y_shift = (int*)params[0];  
    
    int negative_offset = this->m_y_abs - this->m_contentBB_y2;
    this->m_y_abs = (*current_y_shift) + negative_offset;
    (*current_y_shift) -= (this->m_contentBB_y2 - this->m_contentBB_y1);
}

// functors for MusLaidOutStaff

void MusLaidOutStaff::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    MusLaidOutLayerFunctor *layerFunctor = dynamic_cast<MusLaidOutLayerFunctor*>(functor);
    MusLaidOutLayer *layer;
	int i;
    for (i = 0; i < GetLayerCount(); i++) 
	{
        layer = &m_layers[i];
        functor->Call( layer, params );
        if (layerFunctor) { // is is a MusLaidOutLayerFunctor, call it
            layerFunctor->Call( layer, params );
        }
        else { // process it further
            layer->Process( functor, params );
        }
	}
}
