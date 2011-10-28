/////////////////////////////////////////////////////////////////////////////
// Name:        musbarline.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musbarline.h"

//----------------------------------------------------------------------------
// MusBarline
//----------------------------------------------------------------------------

MusBarline::MusBarline():
	MusLayerElement()
{
    m_barlineType = BARLINE_SINGLE;    
}


MusBarline::~MusBarline()
{
}
