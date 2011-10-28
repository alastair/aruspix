/////////////////////////////////////////////////////////////////////////////
// Name:        musdurationinterface.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_DURATION_INTERFACE_H__
#define __MUS_DURATION_INTERFACE_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#define BEAM_INITIAL  (1<<1)
#define BEAM_MEDIAL  (1<<2) 
#define BEAM_TERMINAL  (1<<3) 

#define TUPLET_INITIAL  (1<<1)
#define TUPLET_MEDIAL  (1<<2) 
#define TUPLET_TERMINAL  (1<<3)


//----------------------------------------------------------------------------
// MusDurationInterface
//----------------------------------------------------------------------------

/** 
 * This class is an interface for elements with duration, such as notes and rests.
 * It is not an abstract class but should not be instanciate directly.
 */
class MusDurationInterface
{
public:
    // constructors and destructors
    MusDurationInterface();
    virtual ~MusDurationInterface();
    
private:
    
public:
    /** Indicates if intial, medial or termial part of a beam for up to 6 nesting/overlaping beams */
    unsigned char m_beam[6];
    /** Indicates a break in the beaming */
    unsigned char m_breakSec;
    /** Indicates the number of augmentation dots */
    unsigned char m_dots;
    /** Indicates the duration */
    int m_dur;
    /** Indicates the numerator of the duration ratio */
    int m_num;
    /** Indicates the denominator of the duration ratio */
    int m_numBase;
    /** Indicates if intial, medial or termial part of a tuplet for up to 6 nesting/overlaping tuplets */
    unsigned char m_tuplet[6];

    

private:
    
};


#endif
