/////////////////////////////////////////////////////////////////////////////
// Name:        musapp.cpp
// Author:      Laurent Pugin
// Created:     2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musapp.h"

//----------------------------------------------------------------------------

#include "musio.h"

//----------------------------------------------------------------------------
// MusLayerApp
//----------------------------------------------------------------------------

MusLayerApp::MusLayerApp():
    LayerElement()
{
}

MusLayerApp::~MusLayerApp()
{
    
}

void MusLayerApp::AddLayerRdg( MusLayerRdg *layerRdg )
{
    //assert(this->m_layer->m_staff); // Layer staff cannot be NULL when adding and <rdg>" 
    // The the to the parent staff will be NULL!
    //layerRdg->SetStaff( this->m_layer->m_staff );
    
	layerRdg->SetParent( this );
    m_children.push_back( layerRdg );
    Modify();
}

int MusLayerApp::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];         
    if (!output->WriteLayerApp( this )) {
        return FUNCTOR_STOP;
    }
    return FUNCTOR_CONTINUE;
}

//----------------------------------------------------------------------------
// MusLayerRdg
//----------------------------------------------------------------------------

MusLayerRdg::MusLayerRdg( ):
    MusObject()
{

}


MusLayerRdg::~MusLayerRdg()
{
    
}


void MusLayerRdg::AddElement( LayerElement *element, int idx )
{
	element->SetParent( this );
    if ( idx == -1 ) {
        m_children.push_back( element );
    }
    else {
        InsertChild( element, idx );
    }
}


int MusLayerRdg::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    if (!output->WriteLayerRdg( this )) {
        return FUNCTOR_STOP;
    }
    return FUNCTOR_CONTINUE;
}
