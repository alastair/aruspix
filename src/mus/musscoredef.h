/////////////////////////////////////////////////////////////////////////////
// Name:        musscoredef.h
// Author:      Laurent Pugin
// Created:     2013/11/08
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_SCOREDEF_H__
#define __MUS_SCOREDEF_H__

#include "musobject.h"

class MusClef;
class MusKeySig;
class MusMensur;
class MusStaffGrp;
class MusStaffDef;

#define STAFFDEF_DRAW_NONE  0
#define STAFFDEF_DRAW_CLEF  (1<<0)
#define STAFFDEF_DRAW_KEYSIG  (1<<1)
#define STAFFDEF_DRAW_MENSUR  (1<<2)
#define STAFFDEF_DRAW_ALL (1<<3) - 1

//----------------------------------------------------------------------------
// MusScoreOrStaffDefAttrInterface
//----------------------------------------------------------------------------

/**
 * This class is an interface for MEI scoreDef or staffDef attributes clef, keysig and mensur.
 * For simplification, the attributes are stored as MusClef, MusKeySig and MusMensur.
 */
class MusScoreOrStaffDefAttrInterface
{
public:
    // constructors and destructors
    MusScoreOrStaffDefAttrInterface();
    virtual ~MusScoreOrStaffDefAttrInterface();
    MusScoreOrStaffDefAttrInterface( const MusScoreOrStaffDefAttrInterface& interface ); // copy contructor
    MusScoreOrStaffDefAttrInterface& operator=( const MusScoreOrStaffDefAttrInterface& interface ); // copy assignement;
    
    /**
     * Replace the clef (if any) with the newClef (if any).
     */
    void ReplaceClef( MusClef *newClef );
    
    /**
     * Replace the keysig (if any) with the newKeysig (if any).
     */
    void ReplaceKeySig( MusKeySig *newKeySig );
    
    /**
     * Replace the mensur (if any) with the newMensur (if any).
     */
    void ReplaceMensur( MusMensur *newMensur );
    
    /**
     * @name Get  the clef, keysig and mensure.
     */
    ///@{
    MusClef *GetClefAttr() const { return m_clef; };
    MusKeySig *GetKeySigAttr() const { return m_keySig; };
    MusMensur *GetMensurAttr() const { return m_mensur; };
    ///@}
    
protected:
    /** The clef attribute */
    MusClef *m_clef;
    /** The key signature */
    MusKeySig *m_keySig;
    /** The mensure (time signature */
    MusMensur *m_mensur;
    
};



//----------------------------------------------------------------------------
// MusScoreDef
//----------------------------------------------------------------------------

/**
 * This class represents a MEI scoreDef.
 * It contains MusStaffGrp objects.
*/
class MusScoreDef: public MusObject, public MusScoreOrStaffDefAttrInterface
{
public:
    // constructors and destructors
    MusScoreDef();
    virtual ~MusScoreDef();
        
    virtual std::string MusClassName( ) { return "MusScoreDef"; };
    
	void AddStaffGrp( MusStaffGrp *staffGrp );
    
    /**
     * Replace the scoreDef with the content of the newScoreDef.
     */
    void Replace( MusScoreDef *newScoreDef );
    
    /**
     * Replace the corresponding staffDef with the content of the newStaffDef.
     * Looks for the staffDef with the same m_n (@n) and replace the attribute set.
     * Attribute set is provided by the MusScoreOrStaffDefAttrInterface.
     */
    void Replace( MusStaffDef *newStaffDef );
    
    /**
     * Get the staffDef with number n (NULL if not found).
     */
    MusStaffDef *GetStaffDef( int n );
    
    /**
     * Set the redraw flag to all staffDefs.
     * This is necessary at the beginning or when a scoreDef occurs.
     */
    void SetRedraw( bool clef, bool keysig, bool mensur );
    
private:
    
public:

private:
    
};


//----------------------------------------------------------------------------
// MusStaffGrp
//----------------------------------------------------------------------------

/**
 * This class represents a MEI staffGrp.
 * It contains MusStaffDef objects.
 */
class MusStaffGrp: public MusObject
{
public:
    // constructors and destructors
    MusStaffGrp();
    virtual ~MusStaffGrp();
    virtual MusObject* Clone() { return new MusStaffGrp(*this); };
    
    virtual std::string MusClassName( ) { return "MusStaffGrp"; };
	
	void AddStaffDef( MusStaffDef *staffDef );
    
	void AddStaffGrp( MusStaffGrp *staffGrp );
    
    // functors
    virtual int Save( ArrayPtrVoid params );
    
private:
    
public:
    
private:
    
};


//----------------------------------------------------------------------------
// MusStaffDef
//----------------------------------------------------------------------------

/**
 * This class represents a MEI staffDef.
 */
class MusStaffDef: public MusObject, public MusScoreOrStaffDefAttrInterface
{
public:
    // constructors and destructors
    MusStaffDef();
    virtual ~MusStaffDef();
    virtual MusObject* Clone() { return new MusStaffDef(*this); };
    
    virtual std::string MusClassName( ) { return "MusStaffDef"; };
    
    /**
     * @name Set and get  the staff number which is 1-based.
     * This can be different from the index position in the parent measure.
     */
    ///@{
    int GetStaffNo() const { return m_n; };
    void SetStaffNo( int n ) { m_n = n; };
    ///@}
    
    /**
     * @name Set and get the layer drawing flags for clef, keysig and mensur.
     * This will be true when starting a new system or when a scoreDef or staffDef changes occurs
     * This will be true only for the first layer in the staff.
     */
    ///@{
    int DrawClef() const { return m_drawClef; };
    void SetDrawClef( bool drawClef ) { m_drawClef = drawClef; };
    int DrawKeySig() const { return m_drawKeySig; };
    void SetDrawKeySig( bool drawKeySig ) { m_drawKeySig = drawKeySig; };
    int DrawMensur() const { return m_drawMensur; };
    void SetDrawMensur( bool drawMensur ) { m_drawClef = drawMensur; };
    ///@}

    // functors
    virtual int Save( ArrayPtrVoid params );
    
    /**
     * Find the MusStaffDef with number m_n value.
     * param 0: the n we are looking for.
     * param 1: the pointer to pointer to the MusStaffDef retrieved (if found).
     */
    virtual int FindStaffDefByNumber( ArrayPtrVoid params );
    
    /**
     * Replace all the staffDefs in a scoreDef.
     * Calls MusScoreDef::Replace.
     * param 0: a pointer to the scoreDef we are going to replace the staffDefs
     */
    virtual int ReplaceStaffDefsInScoreDef( ArrayPtrVoid params );
    
    /**
     * Set the initial scoreDef of each page.
     * This is necessary for integrating changes that occur within a page.
     * param 0: bool clef flag.
     * param 1: bool keysig flag.
     * param 2: bool the mensur flag.
     */
    virtual int SetStaffDefDraw( ArrayPtrVoid params );
    
private:
    
public:
    
private:
    /** The staff number */
    int m_n;
    
    /**
     *  @name Flags for indicating where the clef, keysig and mensur needs to be drawn or not
     */
    ///@{
    bool m_drawClef;
    bool m_drawKeySig;
    bool m_drawMensur;
    ///@}
    
};

#endif
