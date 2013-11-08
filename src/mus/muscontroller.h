/////////////////////////////////////////////////////////////////////////////
// Name:        muscontroller.h
// Author:      Laurent Pugin
// Created:     17/10/2013
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_CONTROLLER_H__
#define __MUS_CONTROLLER_H__

#include "musdoc.h"
#include "musrc.h"

#include <string>

typedef enum _file_formats {
    mei_file = 0,
    pae_file,
    darms_file
} ConvertFileFormat;

//----------------------------------------------------------------------------
// MusController
//----------------------------------------------------------------------------

class MusController
{
public:
    // constructors and destructors
    MusController();
    virtual ~MusController();
    
    /**
     * Load a file with the specified type.
     */
    bool LoadFile( std::string filename );
    
    /**
     * Load a string data witht he specified type.
     */
    bool LoadString( std::string data );
    
    /**
     * Parse the options passed as JSON string
     * only available for Emscripten based compiles
     **/
    bool ParseOptions( std::string json_options );
    
    /**
     * Render the page in SVG and returns it as a string
     */
    std::string RenderToSvg( int pageNo =  1, bool xml_tag = false );

    /**
     * Render the page in SVG and save it to the file.
     */
    bool RenderToSvgFile( std::string filename, int pageNo =  1 );
    
    /**
     * @name Set and get the border
     */
    ///@{
    void SetBorder( int border ) { m_border = border; };
    int GetBorder() { return m_border; };
    ///@}
    
    /**
     * @name Set and get the scale
     */
    ///@{
    void SetScale( int scale ) { m_scale = scale; };
    int GetScale() { return m_scale; };
    ///@}
    
    /**
     * @name Get the input file format (defined as ConvertFileFormat)
     */
    ///@{
    void SetFormat( ConvertFileFormat format ) { m_format = format; };
    int GetFormat() { return m_format; };
    ///@}
    
private:
    
public:
    
private:
    MusDoc m_doc;
    MusRC m_rc;
    int m_border;
    int m_scale;
    ConvertFileFormat m_format;
};

#endif
