/////////////////////////////////////////////////////////////////////////////
// Name:        musobject.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "musobject.h"

//----------------------------------------------------------------------------

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <string>

//----------------------------------------------------------------------------

#include "mus.h"
#include "musbeam.h"
#include "muslayer.h"
#include "muslayerelement.h"
#include "musmeasure.h"
#include "muspage.h"
#include "musrc.h"
#include "musstaff.h"
#include "mussystem.h"
#include "mustie.h"
#include "mustuplet.h"

//----------------------------------------------------------------------------
// MusObject
//----------------------------------------------------------------------------

MusObject::MusObject()
{
    m_parent = NULL;
    m_isModified = true;
    uuid_generate( m_uuid );
}

MusObject::~MusObject()
{
}

void MusObject::SetUuid( uuid_t uuid )
{ 
    uuid_copy( m_uuid, uuid );
};

std::string MusObject::GetUuidStr()
{
    char uuidStr[37];
    uuid_unparse( m_uuid, uuidStr ); 
    return std::string( uuidStr );
}

void MusObject::ClearChildren()
{
    ArrayOfMusObjects::iterator iter;
    for (iter = m_children.begin(); iter != m_children.end(); ++iter)
    {
        delete *iter;
    }
    m_children.clear();
}

void MusObject::InsertChild( MusObject *element, int idx )
{
    if ( idx >= (int)m_children.size() ) {
        m_children.push_back( element );
    }
    ArrayOfMusObjects::iterator iter = m_children.begin();
    m_children.insert( iter+(idx), element );
}

MusObject *MusObject::DetachChild( int idx )
{
    if ( idx >= (int)m_children.size() ) {
        return NULL;
    }
    MusObject *child = m_children[idx];
    child->m_parent = NULL;
    ArrayOfMusObjects::iterator iter = m_children.begin();
    m_children.erase( iter+(idx) );
    return child;
}

void MusObject::RemoveChildAt( int idx )
{
    if ( idx >= (int)m_children.size() ) {
        return;
    }
    delete m_children[idx];
    ArrayOfMusObjects::iterator iter = m_children.begin();
    m_children.erase( iter+(idx) );
}

void MusObject::ResetUuid()
{
    uuid_generate( m_uuid );
}

void MusObject::SetParent( MusObject *parent )
{
    // RZ FIXME just for test
    //assert( !m_parent );
    m_parent = parent;
}


bool MusObject::operator==( MusObject& other )
{
    // This should never happen.
    // The comparison is performed in the CmpFile::Align method.
    // We expect to compare only MusNote, MusRest, etc object for which we have an overwritten method
    Mus::LogError( "Missing comparison operator for '%s'", this->MusClassName().c_str() );
    return false;
}

int MusObject::GetChildIndex( const MusObject *child )
{
    ArrayOfMusObjects::iterator iter;
    int i;
    for (iter = m_children.begin(), i = 0; iter != m_children.end(); ++iter, i++)
    {
        if ( child == *iter ) {
            return i;
        }

    }
    return -1;
}

void MusObject::Modify( bool modified )
{    
    // if we have a parent and a new modification, propagate it
    if ( m_parent && !m_isModified && modified ) {
        m_parent->Modify();
    }
    m_isModified = modified;
}

void MusObject::FillList( ListOfMusObjects *list )
{
    MusFunctor addToList( &MusObject::AddMusLayerElementToList );
    ArrayPtrVoid params;
    params.push_back ( list );
    this->Process( &addToList, params );

    /* // For debuging
    ListOfMusObjects::iterator iter;
    for (iter = list->begin(); iter != list->end(); ++iter)
    {
        MusObject *current = *iter;
        Mus::LogDebug("%s", current->MusClassName().c_str() );
    }
    */
}

void MusObject::AddSameAs( std::string id, std::string filename )
{
    std::string sameAs = filename;
    if ( !filename.empty() ) {
        sameAs += "#";
    }
    sameAs += id;
    
    if ( !m_sameAs.empty() ) {
        m_sameAs += " ";
    }
    m_sameAs += sameAs;
}

MusObject *MusObject::GetFirstParent( const std::type_info *elementType, int maxSteps )
{
    if ( (maxSteps == 0) || !m_parent ) {
        return NULL;
    }
    
    if ( typeid(*m_parent) == *elementType ) {
        return m_parent;
    }
    else {
        return ( m_parent->GetFirstParent( elementType, maxSteps - 1 ) );
    }
}
bool MusObject::GetSameAs( std::string *id, std::string *filename, int idx )
{
    int i = 0;
    
    std::istringstream iss( m_sameAs );
    std::string token;
    while( getline( iss, token, ' '))
    {
        if ( i == idx ) {
            size_t pos = token.find( "#" );
            if (pos != std::string::npos) {
                (*filename) = token.substr( 0, pos );
                (*id) = token.substr( pos + 1, std::string::npos );
            }
            else {
                (*filename) = "";
                (*id) = token;
            }
            return true;
        }
        i++;
    }
    return false;
}

void MusObject::Process(MusFunctor *functor, ArrayPtrVoid params )
{
    if (functor->m_stopIt) {
        return;
    }
    
    functor->Call( this, params );

    MusObject *obj;
	int i;
    for (i = 0; i < (int)m_children.size(); i++)
	{
        obj = m_children[i];
        obj->Process( functor, params );
	}
}

//----------------------------------------------------------------------------
// MusDocObject
//----------------------------------------------------------------------------

MusDocObject::MusDocObject() :
	MusObject()
{
	//m_doc = NULL;
    ResetBB();
}

MusDocObject::~MusDocObject()
{
}

void MusDocObject::Refresh() 
{
    // if we have a parent MusDocObject, propagate it
    if ( dynamic_cast<MusDocObject*>(m_parent) ) {
        ((MusDocObject*)m_parent)->Refresh();
    }
}


void MusDocObject::UpdateContentBB( int x1, int y1, int x2, int y2) 
{
    //printf("CB Was: %i %i %i %i\n", m_contentBB_x1, m_contentBB_y1, m_contentBB_x2 ,m_contentBB_y2);
    
    int min_x = std::min( x1, x2 );
    int max_x = std::max( x1, x2 );
    int min_y = std::min( y1, y2 );
    int max_y = std::max( y1, y2 );
    
    if (m_contentBB_x1 > min_x) m_contentBB_x1 = min_x;
    if (m_contentBB_y1 > min_y) m_contentBB_y1 = min_y;
    if (m_contentBB_x2 < max_x) m_contentBB_x2 = max_x;
    if (m_contentBB_y2 < max_y) m_contentBB_y2 = max_y;
    
    m_updatedBB = true;
    //printf("CB Is:  %i %i %i %i\n", m_contentBB_x1,m_contentBB_y1, m_contentBB_x2, m_contentBB_y2);
}

void MusDocObject::UpdateSelfBB( int x1, int y1, int x2, int y2 ) 
{
    //printf("SB Was: %i %i %i %i\n", m_selfBB_x1,m_selfBB_y1, m_selfBB_x2 ,m_selfBB_y2);
    
    int min_x = std::min( x1, x2 );
    int max_x = std::max( x1, x2 );
    int min_y = std::min( y1, y2 );
    int max_y = std::max( y1, y2 );
    
    if (m_selfBB_x1 > min_x) m_selfBB_x1 = min_x;
    if (m_selfBB_y1 > min_y) m_selfBB_y1 = min_y;
    if (m_selfBB_x2 < max_x) m_selfBB_x2 = max_x;
    if (m_selfBB_y2 < max_y) m_selfBB_y2 = max_y;
    
    m_updatedBB = true;
    
    //printf("SB Is:  %i %i %i %i\n", m_selfBB_x1,m_selfBB_y1, m_selfBB_x2 ,m_selfBB_y2);
    
}

void MusDocObject::ResetBB() 
{
    m_contentBB_x1 = 0xFFFF;
    m_contentBB_y1 = 0xFFFF;
    m_contentBB_x2 = -0xFFFF;
    m_contentBB_y2 = -0xFFFF;
    m_selfBB_x1 = 0xFFFF;
    m_selfBB_y1 = 0xFFFF; 
    m_selfBB_x2 = -0xFFFF;
    m_selfBB_y2 = -0xFFFF;
    
    m_updatedBB = false;
}

bool MusDocObject::HasContentBB() 
{
    return ( (m_contentBB_x1 != 0xFFFF) && (m_contentBB_y1 != 0xFFFF) && (m_contentBB_x2 != -0xFFFF) && (m_contentBB_y2 != -0xFFFF) );
}

bool MusDocObject::HasSelfBB() 
{
    return ( (m_selfBB_x1 != 0xFFFF) && (m_selfBB_y1 != 0xFFFF) && (m_selfBB_x2 != -0xFFFF) && (m_selfBB_y2 != -0xFFFF) );
}


//----------------------------------------------------------------------------
// MusObjectListInterface
//----------------------------------------------------------------------------


void MusObjectListInterface::ResetList( MusObject *node )
{
    // nothing to do, the list if up to date
    if ( !node->IsModified() ) {
        return;
    }
    
    m_list.clear();
    node->FillList( &m_list );
    this->FilterList();
    node->Modify( false );
}

ListOfMusObjects *MusObjectListInterface::GetList( MusObject *node )
{   
    ResetList( node );
    return &m_list;
}


int MusObjectListInterface::GetListIndex( const MusObject *listElement )
{
    ListOfMusObjects::iterator iter;
    int i;
    for (iter = m_list.begin(), i = 0; iter != m_list.end(); ++iter, i++)
    {
        if ( listElement == *iter ) {
            return i;
        }
        
    }
    return -1;
}

MusObject *MusObjectListInterface::GetListPrevious( const MusObject *listElement )
{
    ListOfMusObjects::iterator iter;
    int i;
    for (iter = m_list.begin(), i = 0; iter != m_list.end(); ++iter, i++)
    {
        if (listElement == *iter) {
            if (i > 0) {
                return *(--iter);
            }
            else {
                return NULL;
            }
        }
        
    }
    return NULL;
}

MusObject *MusObjectListInterface::GetListNext( const MusObject *listElement )
{
    ListOfMusObjects::reverse_iterator iter;
    int i;
    for (iter = m_list.rbegin(), i = 0; iter != m_list.rend(); ++iter, i++)
    {
        if ( listElement == *iter ) {
            if (i > 0) {
                return *(--iter);
            }
            else {
                return NULL;
            }
        }
        
    }
    return NULL;
}

//----------------------------------------------------------------------------
// MusFunctor
//----------------------------------------------------------------------------

MusFunctor::MusFunctor( )
{ 
    m_stopIt=false;
    m_reverse=false;
    obj_fpt=NULL; 
}

MusFunctor::MusFunctor( bool(MusObject::*_obj_fpt)( ArrayPtrVoid ))
{ 
    m_stopIt=false;
    m_reverse=false;
    obj_fpt=_obj_fpt; 
}

void MusFunctor::Call( MusObject *ptr, ArrayPtrVoid params )
{
    // we should have return codes (not just bool) for avoiding to go further down the tree in some cases
    m_stopIt = (*ptr.*obj_fpt)( params );
}

//----------------------------------------------------------------------------
// MusObject functor methods
//----------------------------------------------------------------------------

bool MusObject::AddMusLayerElementToList( ArrayPtrVoid params )
{
    // param 0: the ListOfMusObjects
    ListOfMusObjects *list = (ListOfMusObjects*)params[0];
    if ( dynamic_cast<MusLayerElement*>(this ) ) {
        list->push_back( this );
    }
    return false;
}

bool MusObject::FindByUuid( ArrayPtrVoid params )
{
    // param 0: the uuid we are looking for
    // parma 1: the pointer to the element
    uuid_t *uuid = (uuid_t*)params[0];  
    MusObject **element = (MusObject**)params[1];  
    
    if ( (*element) ) {
        return true;
    }
    
    if ( uuid_compare( *uuid, *this->GetUuid() ) == 0 ) {
        (*element) = this;
        //Mus::LogDebug("Found it!");
        return true;
    }
    //Mus::LogDebug("Still looking for uuid...");
    return false;
}

bool MusObject::TrimSystem( ArrayPtrVoid params )
{
    MusSystem *current = dynamic_cast<MusSystem*>(this);
    if ( !current  ) {
        return false;
    }
    
    if ( !m_parent ) {
        return false;
    }
    MusPage *page = (MusPage*)m_parent;
    
    int system_length = (current->m_contentBB_x2 - current->m_contentBB_x1) + page->m_pageRightMar;
    if ( page->m_pageWidth < system_length ) {
        page->m_pageWidth = system_length;
    }
    return false;
}

bool MusObject::LayOutLayerElementXPos( ArrayPtrVoid params )
{
    // param 0: the current x measure shift
    // param 1: the current x element shift
	int *current_x_measure_shift = (int*)params[0];
    int *current_x_element_shift = (int*)params[1];
    
    // starting a new staff
    MusStaff *current_staff = dynamic_cast<MusStaff*>(this);
    if ( current_staff  ) {
        (*current_x_measure_shift) = 0;
    }

    // starting a new measure
    MusMeasure *current_measure = dynamic_cast<MusMeasure*>(this);
    if ( current_measure ) {
        current_measure->m_x_rel = (*current_x_measure_shift);
    }
    
    // starting an new layer
    MusLayer *current_layer = dynamic_cast<MusLayer*>(this);
    if ( current_layer  ) {
        (*current_x_element_shift) = 0;
    }

    MusLayerElement *current = dynamic_cast<MusLayerElement*>(this);
    if ( !current  ) {
        return false;
    }

    if ( !current->HasUpdatedBB() ) {
        // this is all we need for empty elements
        current->m_x_rel = (*current_x_element_shift);
        return false;
    }
    
    if ( dynamic_cast<MusBeam*>(current) ) {
        return false;
    }
    
    if ( dynamic_cast<MusTie*>(current) ) {
        return false;
    }
    
    if ( dynamic_cast<MusTuplet*>(current) ) {
        return false;
    }
    
    int negative_offset = current->m_x_rel - current->m_contentBB_x1;
    current->m_x_rel = (*current_x_element_shift) + negative_offset;
    
    int shift = (current->m_contentBB_x2 - current->m_contentBB_x1) + current->GetHorizontalSpacing();
    (*current_x_element_shift) += shift;
    (*current_x_measure_shift) += shift; // this will not work with more than one layer because we are cumulating
    
    return false;
}


bool MusObject::LayOutSystemAndStaffYPos( ArrayPtrVoid params )
{
    // param 0: the current y system shift
    // param 1: the current y staff shift
	int *current_y_system_shift = (int*)params[0];
	int *current_y_staff_shift = (int*)params[1];
    
    MusStaff *current = dynamic_cast<MusStaff*>(this);
    if ( !current  ) {
        return false;
    }
    
    // This is the value that need to be added to fit everything
    int negative_offset = current->m_y_rel - current->m_contentBB_y2;
    
    // reset the x position if we are starting a new system
    if ( current->m_parent->GetChildIndex( this ) == 0 ) {
        MusSystem *system = dynamic_cast<MusSystem*>( current->m_parent );
        // The parent is a MusSystem, we need to reset the y staff shift
        if ( system ) {
            // the staff position is the same as the one of the system
            (*current_y_staff_shift) = 0;
            current->m_y_rel = 0;
            // move the system down to fit the content
            system->m_y_rel = (*current_y_system_shift)  + negative_offset;
            // spacing for the next system
            (*current_y_system_shift) -= system->GetVerticalSpacing();
        }
    }
    else
    {
        // just more the staff down
        current->m_y_rel = (*current_y_staff_shift)  + negative_offset;
    }
    
    int shift = (current->m_contentBB_y2 - current->m_contentBB_y1) + current->GetVerticalSpacing();
    (*current_y_staff_shift) -= shift;
    (*current_y_system_shift) -= shift;
    // we should have return codes for avoiding to go further down the tree in such cases
    return false;
}





