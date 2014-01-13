/////////////////////////////////////////////////////////////////////////////
// Name:        musbeam.cpp
// Author:      Rodolfo Zitellini
// Created:     26/06/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "mus.h"
#include "musbeam.h"

//----------------------------------------------------------------------------
// MusBeam
//----------------------------------------------------------------------------

MusBeam::MusBeam():
    LayerElement("beam-"), MusObjectListInterface()
{
}


MusBeam::~MusBeam()
{
}

void MusBeam::AddElement(LayerElement *element) {
   
    element->SetParent( this );
    m_children.push_back(element);
    Modify();
}

void MusBeam::FilterList()
{
    // We want to keep only notes and rest
    // Eventually, we also need to filter out grace notes properly (e.g., with sub-beams)
    ListOfMusObjects::iterator iter = m_list.begin();
    
    while ( iter != m_list.end()) {
        LayerElement *currentElement = dynamic_cast<LayerElement*>(*iter);
        if ( currentElement && !currentElement->HasDurationInterface() )
        {
            //Mus::LogDebug("KILLED!!! %s", currentElement->MusClassName().c_str() );
            iter = m_list.erase( iter );
        } else {
            iter++;
        }
    }
    
}


