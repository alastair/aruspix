/////////////////////////////////////////////////////////////////////////////
// Name:        musiomlf.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////



#ifdef __GNUG__
    #pragma implementation "musiomlf.h"
#endif

#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/filename.h"
#include "wx/textfile.h"
#include "wx/tokenzr.h"
#include "wx/txtstrm.h"

#include "musiomlf.h"

#include "muspage.h"
#include "mussystem.h"
#include "musstaff.h"
//#include "muselement.h"
//#include "musnote.h"
//#include "mussymbol.h"
#include "musneume.h"

#include "musmlfdic.h"

#include "im/impage.h"
#include "im/imstaff.h"

#include "app/axprocess.h"
#include "app/axprogressdlg.h"
#include "app/axapp.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMLFSymbols );

// TINYXML
#if defined (__WXMSW__)
    #include "tinyxml.h"
#else
    #include "tinyxml/tinyxml.h"
#endif

// utilise pour la transposition de silences

static char _sil1[] = {'A','C','C','E','E','G','G','B','B'};
static int _oct1[] =  { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , -1};

static char _sil0[] = {'B','B','D','D','F','F','A','A','C'};
static int _oct0[] =  { 0 , -1, 0 , 0 , 0 , 0 , 0 , 0 , 0 };

static char _note[] = {'B','C','D','E','F','G','A','B','C'};
//static char *_val[] = {"0","1","2","3","4","5","6","7","8"};

IMPLEMENT_DYNAMIC_CLASS(MusMLFSymbol, wxObject)

//----------------------------------------------------------------------------
// MusMLFSymbol
//----------------------------------------------------------------------------

MusMLFSymbol::MusMLFSymbol( ) :
	wxObject()
{
	m_type = 0;
	m_value = 0;
	m_pitch = 0;
	m_oct = 0;
	m_flag = 0;
	m_position = 0;
}

void MusMLFSymbol::SetValue( char type, wxString subtype, int position, int value, char pitch, int oct, int flag )
{
	m_type = type;
	m_subtype = subtype;
	m_position = position;
	m_value = value;
	m_pitch = pitch;
	m_oct = oct;
	m_flag = flag;
}


wxString MusMLFSymbol::GetLabel( )
{
	if ( m_type == 0 )
	{
		wxLogWarning( _("Uninitialized symbol") );
		return "";
	}

	wxString label, label_width;
	label << m_type;

	if ((m_type == TYPE_NOTE) || (m_type == TYPE_REST))
	{	
		label << "_" << m_value << "_" << m_pitch << "_" << m_oct;
		if ( m_flag )
			label << "_" << m_flag;
	}
	else if (m_type == TYPE_KEY)
	{
		label << "_" << m_subtype << m_value;
	}
	else if  (m_type == TYPE_ALTERATION)
	{
		label << "_" << m_subtype << m_value << "_" << m_pitch << "_" << m_oct;
	}
	else if ((m_type == TYPE_CUSTOS) || (m_type == TYPE_POINT))
	{
		label << "_" << m_pitch << "_" << m_oct;
	}
	else if ((m_type == TYPE_MESURE) || (m_type == TYPE_SYMBOLE))
	{
		label << "_" << m_subtype;
	}
		
	return label;
}


int MusMLFSymbol::GetNbOfStates( )
{
	if ( m_type == 0 )
	{
		wxLogWarning( _("Uninitialized symbol") );
		return 0;
	}
	
	int states = 0;

	if (m_type == TYPE_NOTE)
	{	
		switch (m_value)
		{
		case (0): states = 6; break;
		case (1): states = 6; break;
		case (2): states = 6; break;
		case (3): states = 6; break;
		case (4): states = 6; break;
		case (5): states = 6; break;
		case (6): states = 6; break;
		case (7): states = 6; break;
		}
	}
	else if (m_type == TYPE_REST)
	{
		switch (m_value)
		{
		case (0): states = 4; break;
		case (1): states = 4; break;
		case (2): states = 4; break;
		case (3): states = 4; break;
		case (4): states = 5; break;
		case (5): states = 5; break;
		case (6): states = 5; break;
		}	
	}
	else if (m_type == TYPE_KEY)
	{
		if (m_subtype == "S") states = 7;
		else if (m_subtype == "F") states = 6;
		else if (m_subtype == "U") states = 6;
	}
	else if  (m_type == TYPE_ALTERATION)
	{
		if (m_subtype == "D") states = 6;
		else if (m_subtype == "B") states = 6;
		else if (m_subtype == "H") states = 6;
	}
	else if (m_type == TYPE_CUSTOS)
	{
		states = 6;
	}
	else if (m_type == TYPE_POINT)
	{
		states = 4;
	}
	else if (m_type == TYPE_MESURE)
	{
		states = 6;
	}
	else if (m_type == TYPE_SYMBOLE)
	{
		if (m_subtype == "B") states = 4;
	}
	
	if ( states == 0 )
		wxLogWarning( _("Unknown states number for %s"), this->GetLabel().c_str() );

	return states;
}

int MusMLFSymbol::GetWidth( )
{
	if ( m_type == 0 )
	{
		wxLogWarning( _("Uninitialized symbol") );
		return 0;
	}
	
	int width = 0;

	if (m_type == TYPE_NOTE)
	{	
		switch (m_value)
		{
		case (0): width = 30; break;
		case (1): width = 30; break;
		case (2): width = 30; break;
		case (3): width = 30; break;
		case (4): width = 30; break;
		case (5): width = 30; break;
		case (6): width = 30; break;
		case (7): width = 30; break;
		}
	}
	else if (m_type == TYPE_REST)
	{
		switch (m_value)
		{
		case (0): width = 10; break;
		case (1): width = 10; break;
		case (2): width = 10; break;
		case (3): width = 10; break;
		case (4): width = 20; break;
		case (5): width = 20; break;
		case (6): width = 20; break;
		}	
	}
	else if (m_type == TYPE_KEY)
	{
		if (m_subtype == "S") width = 30;
		else if (m_subtype == "F") width = 30;
		else if (m_subtype == "U") width = 30;
	}
	else if  (m_type == TYPE_ALTERATION)
	{
		if (m_subtype == "D") width = 30;
		else if (m_subtype == "B") width = 30;
		else if (m_subtype == "H") width = 30;
	}
	else if (m_type == TYPE_CUSTOS)
	{
		width = 30;
	}
	else if (m_type == TYPE_POINT)
	{
		width = 10;
	}
	else if (m_type == TYPE_MESURE)
	{
		width = 30;
	}
	else if (m_type == TYPE_SYMBOLE)
	{
		if (m_subtype == "B") width = 10;
	}
	
	if ( width == 0 )
		wxLogWarning( _("Unknown width number for %s"), this->GetLabel().c_str() );

	return width;
}

// retourne le label du type

wxString MusMLFSymbol::GetLabelType( )
{
	if ( m_type == 0 )
	{
		wxLogWarning( _("Uninitialized symbol") );
		return "";
	}

	wxString label_width;
	label_width << m_type;

	if ((m_type == TYPE_NOTE) || (m_type == TYPE_REST))
	{	
		label_width << "_" << m_value;
		int flag = m_flag;
		if (flag & NOTE_STEM)
			flag -= NOTE_STEM; // remove not stem flag in type. will be treated in the image
		if ( flag )
			label_width << "_" << flag;
	}
	else if ((m_type == TYPE_KEY) || (m_type == TYPE_ALTERATION) || (m_type == TYPE_SYMBOLE))
	{
		label_width << "_" << m_subtype;
	}
	else if ((m_type == TYPE_MESURE))
	{
		label_width << "_" << m_subtype;
	}
	return label_width;
}


//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

MusMLFOutput::MusMLFOutput( MusDoc *doc, wxString filename, MusMLFDictionary *dict, wxString model_symbol_name ) :
    MusFileOutputStream( doc, filename )
{
    m_filename = filename;
    wxFileName::SplitPath( m_filename, NULL, &m_shortname, NULL );
	m_addPageNo = false;
	m_addHeader = true;
	m_pagePosition = false;
	m_hmmLevel = false;
	m_staff = NULL;
	m_mlf_class_name = model_symbol_name;
	m_dict = dict;
}


MusMLFOutput::MusMLFOutput( MusDoc *doc, int fd, wxString filename, MusMLFDictionary *dict,  wxString model_symbol_name ) :
	MusFileOutputStream( doc, fd )
{
    m_filename = filename;
    wxFileName::SplitPath( m_filename, NULL, &m_shortname, NULL );
	m_addPageNo = false;
	m_addHeader = false;
	m_pagePosition = false;
	m_hmmLevel = false;
	m_staff = NULL;
	m_mlf_class_name = model_symbol_name;
	m_dict = dict;
}

MusMLFOutput::~MusMLFOutput()
{
}

// specific

MusLaidOutStaff *MusMLFOutput::SplitSymboles( MusLaidOutStaff *staff )
{
/*
	unsigned int k;

	MusLaidOutStaff *nstaff = new MusLaidOutStaff();
	staff->CopyAttributes( nstaff );
	MusNote1 *nnote = NULL;
	MusSymbol1 *nsymbol1 = NULL;
	MusSymbol1 *nsymbol2 = NULL;

    for (k = 0;k < staff->GetElementCount() ; k++ )
    {
		if ( staff->m_elements[k].IsNote() )
		{
			nnote = new MusNote1( *(MusNote1*)&staff->m_elements[k] );
			// alteration
			if (nnote->acc != 0)
			{
				nsymbol2 = new MusSymbol1();
				nsymbol2->flag = ALTER;
				nsymbol2->calte = nnote->acc;
				nsymbol2->xrel = nnote->xrel - 10*4; // 10 = pas par defaut
				nsymbol2->dec_y = nnote->dec_y;
				nsymbol2->oct = nnote->oct;
				nsymbol2->code = nnote->code;
				nstaff->m_elements.Add( nsymbol2 );
				nnote->acc = 0;
			}
			nstaff->m_elements.Add( nnote );
			// point
			if (nnote->point == POINTAGE)
			{
				if ( nnote->sil == _SIL )
					wxLogWarning( _("Pointed rest not allowed, point has been removed.") );
				else
				{
					nsymbol2 = new MusSymbol1();
					nsymbol2->flag = PNT;
					nsymbol2->point = 0;
					nsymbol2->xrel = nnote->xrel + 30;
					nsymbol2->dec_y = nnote->dec_y;
					nsymbol2->oct = nnote->oct;
					nsymbol2->code = nnote->code;
					nstaff->m_elements.Add( nsymbol2 );
				}
				nnote->point = 0;
			}				
		}
		if ( staff->m_elements[k].IsSymbol() )
		{
			nsymbol1 = new MusSymbol1( *(MusSymbol1*)&staff->m_elements[k] );
			nstaff->m_elements.Add( nsymbol1 );
			// barre TODO repetion points
			if (nsymbol1->flag == BARRE)
			{			
				if ( nsymbol1->code == 'D' ) // double barre
				{
					nsymbol2 = new MusSymbol1( *nsymbol1 );
					nsymbol2->code = '|';
					nsymbol2->xrel += 10;
					nstaff->m_elements.Add( nsymbol2 );
					nsymbol1->code = '|';
				}
			}
			// indication de mesure - splitter indication avec chiffre ET symbol
			else if (nsymbol1->flag == IND_MES)
			{
				if ((nsymbol1->code & 1) && (nsymbol1->code != 1)) // pas chiffre seuls
				{
					nsymbol2 = new MusSymbol1( *nsymbol1 );
					nsymbol2->code = 1;
					nsymbol2->xrel += 10*5; // 10 = pas par defaut
					nstaff->m_elements.Add( nsymbol2 );
					nsymbol1->code -= 1;
				}
			}
		}
    }
	nstaff->CheckIntegrity( );
	return nstaff;
    */
    wxLogDebug( "SplitSymboles missing in ax2" );
}

void MusMLFOutput::StartLabel( )
{
	wxString label;
	if ( !m_addPageNo )
		label = wxString::Format("\"*/%s_%d.0.lab\"\n", m_shortname.c_str(), m_staff_i );
	else
		label = wxString::Format("\"*/%s_%03d.%d.0.lab\"\n", m_shortname.c_str(), m_page_i + 1, m_staff_i );
	Write( label, label.Length() );
	
	m_symbols.Clear();

}

void MusMLFOutput::EndLabel( int offset, int end_point )
{
	for (int i = 0; i < (int)m_symbols.GetCount(); i++ )
	{
		wxString word;
		wxString word_label = m_symbols[i].GetLabel();
		if ( word_label == "" ) // skip empty labels
			continue;
		int pos;
		
		// si verifier dans le dictionnaire, tenir jour la liste dans 
		// The dictionnary is now ready to handle several hmm (and states)
		// per word. To enable this, the MusMLFSymbol class would have to be modified,
		// as for now a word has has only one hmm and we assume that the hmm name
		// is identical to the word (but lowercase)
		if ( m_dict && m_dict->Index( word_label ) == wxNOT_FOUND )
		{
			
			MusMLFWord *w = new MusMLFWord();
			w->m_word = word_label;
			wxString hmm = word_label;
			hmm.MakeLower();
			w->m_hmms.Add( hmm );
			w->m_states.Add( m_symbols[i].GetNbOfStates() );
			m_dict->m_dict.Add( w );
			m_dict->m_dict.Sort( SortMLFWords );
		}
		
		
		// here we write the symbols rather than the word, with staff position. Valid only if we 
		// have the imPage and offsets. Maybe would be better to have this in a child class?
		// currently does not use the dictionnary as it should. It just convert the word to lower case
		// See comment above
		if ( m_hmmLevel )
		{
			wxASSERT( offset != -1 );
			wxASSERT( end_point != -1 );
			
			int fact = 100;
		
			wxString hmm;
			wxString hmm_label = word_label;
			hmm_label.MakeLower();
			pos = m_symbols[i].GetPosition() - offset;
			if ( pos < 0 )
				pos = 0;
			
			int next = end_point;
			int next_symbol = next;
			// position du symbol suivant, mais seulement si pas hors portee
			if ( i < (int)m_symbols.GetCount() - 1 )
				next_symbol = m_symbols[i+1].GetPosition() - offset;
			if ( next_symbol < next )
				next = next_symbol;
			
			int width = pos + m_symbols[i].GetWidth();
			// width
			if ( width > next )
			{
				//wxLogDebug( _("Label position %s out of range (symbol truncated)"), subsymbol_label.c_str() );
				width = next;
			}
			if ( pos < width ) // else skip symbol
				hmm << ( pos * fact) << " " << ( width * fact) << " " << hmm_label<< "\n";
			else
				wxLogDebug( _("Label position %s out of range (symbol skipped)"), hmm_label.c_str() );
	
			if ( width + WIN_WIDTH <  next ) // add sp
				hmm << ( width * fact) << " " << ( next * fact) << " {s}\n";
			 
			Write( hmm, hmm.Length() );		
		}
		
		else
		{
			if ( m_pagePosition )  // this is a basic position over the page, without checking overlaps 
									//between symbols nor overflow
			{
				pos = m_symbols[i].GetPosition();
				// really ???? this is not page position anymore.... 
				// I think that offset are used in CmpFile... has to be checked...
				if ( offset != -1 )
					pos -= offset;
				if ( pos < 0 )
					pos = 0;
				
				int width = pos + m_symbols[i].GetWidth();
				word << ( pos ) << " " << ( width ) << " "; // !!!!!!  VERIFIER !!!!!
			}
			word << word_label << "\n";
			Write( word, word.Length() );
		}
	}
	m_symbols.Clear();

	wxString end_label = ".\n";
	Write( end_label, end_label.Length() );
}
	
// copie le portee en convertissant les symbols de la clef courante vers Ut1
// si inPlace, directment dans staff

MusLaidOutStaff *MusMLFOutput::GetUt1( MusLaidOutStaff *staff , bool inPlace )
{
/*
	if ( !staff )
		return NULL;

	int code, oct;

	MusLaidOutStaff *nstaff = staff;
	if ( !inPlace )
		nstaff = new MusLaidOutStaff( *staff );
	
	for (int i = 0; i < (int)nstaff->m_elements.GetCount(); i++ )
	{
		if ( nstaff->m_elements[i].IsNote() )
		{
			MusNote1 *note = (MusNote1*)&nstaff->m_elements[i];
			{
				GetUt1( nstaff, note, &code, &oct );
				note->code = code;
				note->oct = oct;
			}
		}
		else if ( nstaff->m_elements[i].IsSymbol() )
		{
			MusSymbol1 *symbol = (MusSymbol1*)&nstaff->m_elements[i];
			if ((symbol->flag == ALTER) || (symbol->flag == PNT))
			{
				GetUt1( nstaff, symbol, &code, &oct );
				symbol->code = code;
				symbol->oct = oct;
			}
		}
	}

	if ( inPlace )
		return NULL;
	else
		return nstaff;
*/
    wxLogDebug( "GetUt1 missing in ax2" );
    return NULL;
}


/* Calcule l'offset de la cle; doit etre separe de clefId pour pouvoir
  etre appele depuis rd_symbol() en cas de changement de definition.
	La valeur attribuee est un facteur de multiplication d'espaces
  (espace[staffSize]) decrivant l'eloignement de la clef par rapport a
  la ligne du bas.
*/

void MusMLFOutput::GetUt1( MusLaidOutStaff *staff, MusLaidOutLayerElement *pelement, int *code, int *oct)
{
/*
	if (!pelement || !code || !oct) return;

	char valeur = 0;
	int offs;

	if (!staff || (staff->GetClef(pelement,&valeur, 1 )==0))
		valeur = 5;
	MusSymbol1::calcoffs(&offs, valeur);

	*oct = pelement->oct;
	*code = 0;
	if (pelement->IsSymbol())
		*code = ((MusSymbol1*)pelement)->code + offs;
	else if (pelement->IsNote())
		*code = ((MusNote1*)pelement)->code + offs;

	while (*code < 1)
	{
		*code += 7;
		(*oct)--;
	};
	while (*code > 7)
	{
		*code -= 7;
		(*oct)++;
	};
*/
    wxLogDebug( "GetUt1 missing in ax2" );
}

bool MusMLFOutput::WritePage( const MusPage *page, bool write_header )
{
	if ( write_header && m_addHeader )
	{
		Write("#!MLF!#\n",8);
		m_addHeader = false;
	}
    
    /*

    m_staff = NULL;
    for (m_staff_i = 0; m_staff_i < page->GetStaffCount(); m_staff_i++) 
    {
        MusLaidOutStaff *staff = &page->m_staves[m_staff_i];
		m_staff = MusMLFOutput::SplitSymboles( staff );
        WriteStaff( m_staff );
		delete m_staff;
		m_staff = NULL;
    }

    */
    wxLogError( "MusMLFOutput::WritePage missing in ax2" );
    return true;

}

// idem ExportFile() puis WritePage(), mais gere la position des portee de imPage et les portee selon staff numbers
bool MusMLFOutput::WritePage( const MusPage *page, wxString filename, ImPage *imPage, wxArrayInt *staff_numbers )
{
    /*
	wxASSERT_MSG( page, "MusPage cannot be NULL" );
	wxASSERT_MSG( imPage, "ImPage cannot be NULL" );
	
    m_filename = filename;
	m_shortname = filename;
    //wxFileName::SplitPath( m_filename, NULL, &m_shortname, NULL );
	
	if ( m_addHeader )
	{
		Write("#!MLF!#\n",8);
		m_addHeader = false;
	}
	
	int offset;
	int end_point;

    m_staff = NULL;
    for (m_staff_i = 0; m_staff_i < page->GetStaffCount(); m_staff_i++) 
    {
		if ( staff_numbers && ( staff_numbers->Index( m_staff_i ) == wxNOT_FOUND ) )
			continue;

        MusLaidOutStaff *staff = &page->m_staves[m_staff_i];
		imPage->m_staves[m_staff_i].GetMinMax( &offset, &end_point );
		m_staff = MusMLFOutput::SplitSymboles( staff );
        WriteStaff( m_staff, offset, end_point );
		delete m_staff;
		m_staff = NULL;
    }
    */
    wxLogError( "MusMLFOutput::WritePage missing in ax2" );
    return true;

}


bool MusMLFOutput::WriteStaff( const MusLaidOutStaff *staff, int offset,  int end_point )
{
/*
	if (staff->GetElementCount() == 0)
		return true;

    unsigned int k;

	StartLabel();

    for (k = 0;k < staff->GetElementCount() ; k++ )
    {
        if ( staff->m_elements[k].IsNote() )
        {
            WriteNote( (MusNote1*)&staff->m_elements[k] );
        }
        else if ( staff->m_elements[k].IsSymbol() )
        {
            WriteSymbol( (MusSymbol1*)&staff->m_elements[k] );
        }
    }
	EndLabel( offset, end_point );
*/
    wxLogDebug( "WriteStaff missing in ax2" );

    return true;
}

/*
bool MusMLFOutput::WriteNote(  MusNote1 *note )
{
	int  code, oct;
	GetUt1( m_staff, note, &code, &oct);

	MusMLFSymbol *mlfsb = NULL;

	// custos 
	if (note->val == CUSTOS)
	{
		mlfsb = (MusMLFSymbol*)wxCreateDynamicObject( m_mlf_class_name );
		mlfsb->SetValue( TYPE_CUSTOS, "", note->xrel, 0, _note[code],oct);
		m_symbols.Add( mlfsb );

	}
	// note ou silence
	else 
	{
		mlfsb = (MusMLFSymbol*)wxCreateDynamicObject( m_mlf_class_name );
		int flag = 0;
		// hampe
		if ((note->q_auto == false) && (note->sil != _SIL) && ((note->val ==  DUR_LG) || (note->val > DUR_1 )))
			flag += NOTE_STEM;
		
		// ligature
		if ((note->ligat == true) && (note->sil != _SIL) && (note->val >  DUR_LG) && (note->val < DUR_2 ))
			flag += NOTE_LIGATURE;
			
		// coloration
		if ((note->inv_val == true) && (note->sil != _SIL) && (note->val < DUR_2 ))
			flag += NOTE_COLORATION;
		else if ((note->oblique == true) && (note->sil != _SIL) && (note->val > DUR_4 ))
			flag += NOTE_COLORATION;

		//silence
		if (note->sil == _SIL)
		{
			if ( oct % 2 )
				mlfsb->SetValue( TYPE_REST, "", note->xrel, note->val, _sil0[code],oct + _oct0[code]);	
			else
				mlfsb->SetValue( TYPE_REST, "", note->xrel, note->val, _sil1[code],oct + _oct1[code]);
		}
		else
			mlfsb->SetValue( TYPE_NOTE, "", note->xrel, note->val, _note[code],oct, flag);
		m_symbols.Add( mlfsb );
	}
	return true;
}
*/

/*
  flag
	B = BARRE
	C = CLE
	A = ALTER
	P = PNT
	I = IND_MES
  autre
  */

/*
bool MusMLFOutput::WriteSymbol(  MusSymbol1 *symbol )
{
	// gestion des segment de portees (pas actif ?????)
	if ((symbol->flag == BARRE) && (symbol->code == 'I'))
	{	
		wxASSERT_MSG( false, "Should not happen..." );
		EndLabel();
		StartLabel();
		return true;
	}

	MusMLFSymbol *mlfsb = (MusMLFSymbol*)wxCreateDynamicObject( m_mlf_class_name );

	if (symbol->flag == BARRE)
	{
		mlfsb->SetValue( TYPE_SYMBOLE, "B", symbol->xrel );
		m_symbols.Add( mlfsb );
	}
	else if (symbol->flag == CLE)
	{
		switch(symbol->code)
		{	case SOL2 : mlfsb->SetValue( TYPE_KEY, "S",  symbol->xrel, 2 ); break;
			case SOL1 : mlfsb->SetValue( TYPE_KEY, "S",  symbol->xrel, 1 ); break;
			case SOLva : mlfsb->SetValue( TYPE_KEY, "S",  symbol->xrel, 8 ); break;
			case FA5 : mlfsb->SetValue( TYPE_KEY, "F",  symbol->xrel, 5 ); break;
			case FA4 : mlfsb->SetValue( TYPE_KEY, "F",  symbol->xrel, 4 ); break;
			case FA3 : mlfsb->SetValue( TYPE_KEY, "F",  symbol->xrel, 3 ); break;
			case UT1 : mlfsb->SetValue( TYPE_KEY, "U",  symbol->xrel, 1 ); break;
			case UT2 : mlfsb->SetValue( TYPE_KEY, "U",  symbol->xrel, 2 ); break;
			case UT3 : mlfsb->SetValue( TYPE_KEY, "U",  symbol->xrel, 3 ); break;
			case UT5 : mlfsb->SetValue( TYPE_KEY, "U",  symbol->xrel, 5); break;
			case UT4 : mlfsb->SetValue( TYPE_KEY, "U",  symbol->xrel, 4 ); break;
			default: break;
		}
		m_symbols.Add( mlfsb );
	}
	else if (symbol->flag == ALTER)
	{
		int  code = 0, oct = 0;
		GetUt1( m_staff, symbol, &code, &oct);
		if (symbol->calte == ACCID_SHARP)
			mlfsb->SetValue( TYPE_ALTERATION, "D",  symbol->xrel, 0, _note[code], oct );
		else if (symbol->calte == ACCID_FLAT)
			mlfsb->SetValue( TYPE_ALTERATION, "B",  symbol->xrel, 0, _note[code], oct );
		else if (symbol->calte == ACCID_NATURAL)
			mlfsb->SetValue( TYPE_ALTERATION, "H",  symbol->xrel, 0, _note[code], oct );
		else if (symbol->calte == ACCID_DOUBLE_SHARP)
			mlfsb->SetValue( TYPE_ALTERATION, "D",  symbol->xrel, 1, _note[code], oct );
		else if (symbol->calte == ACCID_DOUBLE_FLAT)
			mlfsb->SetValue( TYPE_ALTERATION, "B",  symbol->xrel, 1, _note[code], oct );
		m_symbols.Add( mlfsb );
	}
	else if (symbol->flag == PNT)
	{
		int  code = 0, oct = 0;
		GetUt1( m_staff, symbol, &code, &oct);
		mlfsb->SetValue( TYPE_POINT, "", symbol->xrel, 0, _note[code + ((code+oct) % 2)], ((code + ((code+oct) % 2)) == 8) ? (oct+1) : oct );
		//str += wxString::Format("P_%s_%d\n",
		//	_note[code + ((code+oct) % 2)],((code + ((code+oct) % 2)) == 8) ? (oct+1) : oct,symbol->code,code,oct);
		m_symbols.Add( mlfsb );
	}
	else if (symbol->flag == IND_MES)
	{
		// signes standard
		if ((int)symbol->code & 64)
		{
			switch (symbol->calte)
			{	
				case 0: mlfsb->SetValue( TYPE_MESURE, "S_C", symbol->xrel ); break;
				case 1: mlfsb->SetValue( TYPE_MESURE, "S_CB", symbol->xrel ); break;
				case 2: mlfsb->SetValue( TYPE_MESURE, "S_2", symbol->xrel ); break;
				case 3: mlfsb->SetValue( TYPE_MESURE, "S_3", symbol->xrel ); break;
				case 4: mlfsb->SetValue( TYPE_MESURE, "S_2B", symbol->xrel ); break;
				case 5: mlfsb->SetValue( TYPE_MESURE, "S_3B", symbol->xrel ); break;
			}
			m_symbols.Add( mlfsb );
		}
		else if (symbol->code != 1)
		{
			wxString subtype;
			// temps parfait
			if ((int)symbol->code & 32)
				subtype += wxString::Format("TP");
			// temps imparfait	
			else if ((int)symbol->code & 16)
				subtype += wxString::Format("TI");
			// temps imparfait double
			else if ((int)symbol->code & 8)
				subtype += wxString::Format("TID");
			// barre
			if ((int)symbol->code & 4)
				subtype += wxString::Format("_B");
			// prolation parfaite
			if ((int)symbol->code & 2)
				subtype += wxString::Format("_P");

			mlfsb->SetValue( TYPE_MESURE, subtype, symbol->xrel );
			m_symbols.Add( mlfsb );
		}
		// chiffres
		else
		{
			wxString subtype = wxString::Format("CH_%d_%d", max (symbol->durNum, (unsigned short)1), max(symbol->durDen, (unsigned short)1));
			mlfsb->SetValue( TYPE_MESURE, subtype, symbol->xrel );
			m_symbols.Add( mlfsb );
		}
	}
	//else if ((symbol->flag == CHAINE ) && (symbol->calte == NUMMES))
	//{}
	else
	{
		delete mlfsb;
		return false;
	}

	return true;
}
*/

//----------------------------------------------------------------------------
// MusMLFInput
//----------------------------------------------------------------------------


bool MusMLFInput::IsElement( bool *note, wxString *line, int *pos )
{
	if (!note || !line || !pos)
		return false;

	if (line->IsEmpty() || (line->GetChar(0) == '"') || (line->GetChar(0) =='.'))
		return false;

	wxString str;
	wxStringTokenizer tkz( *line , " ");
	if ( !tkz.HasMoreTokens() )
		return false;

	str = tkz.GetNextToken();
	if ( !tkz.HasMoreTokens() ) // no position
	{
		*line = str;
		(*pos) += 45; // defaut step
	}
	else // position is given
	{
		*pos = atoi( str.c_str() );
		str = tkz.GetNextToken(); // skip end position
		if ( !tkz.HasMoreTokens() )
			return false;
		*line = tkz.GetNextToken();
	}
	if ((line->GetChar(0) == 'N') || (line->GetChar(0) =='R')  || (line->GetChar(0) =='C'))
		*note = true;
	else
		*note = false;

	return true;
}


/*
MusNote1 *MusMLFInput::ConvertNote( wxString line )
{
	wxStringTokenizer tkz( line , "_");
	if ( !tkz.HasMoreTokens() )
		return NULL;

	wxString str, str1, str2;
	int code, val, flag = 0;
	
	str = tkz.GetNextToken();

	if ( str == "C" )
	{
		if ( !tkz.HasMoreTokens() )
			return NULL;
		str1 = tkz.GetNextToken();
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		str2 = tkz.GetNextToken();
		
		val = CUSTOS;
	}
	else
	{
		if ( !tkz.HasMoreTokens() )
			return NULL;
		val = atoi (tkz.GetNextToken().c_str() );

		if ( !tkz.HasMoreTokens() )
			return NULL;
		str1 = tkz.GetNextToken();

		if ( ! tkz.HasMoreTokens() )
			return NULL;
		str2 = tkz.GetNextToken();

		if ( tkz.HasMoreTokens() )
			flag = atoi (tkz.GetNextToken().c_str() );
	}

	MusNote1 *note = new MusNote1();
	note->val = val;
	GetPitchWWG( str1.GetChar(0), &code );
	note->code = code;
	note->oct = atoi( str2.c_str() );
	if ( str != "N" ) // silence AND CUSTOS
		note->sil = _SIL;
	if ( flag != 0 )
	{
		// hampe
		if ( (flag & NOTE_STEM) && (note->sil != _SIL) && ((note->val ==  DUR_LG) || (note->val > DUR_1 )) )
			note->q_auto = false;
		
		// ligature
		if ( (flag & NOTE_LIGATURE)  && (note->sil != _SIL) && (note->val >  DUR_LG) && (note->val < DUR_2 ) )
			note->ligat = true;
			
		// coloration
		if (  (flag & NOTE_COLORATION) && (note->sil != _SIL) && (note->val < DUR_2 ) )
			note->inv_val = true;
		else if ( (flag & NOTE_COLORATION) && (note->sil != _SIL) && (note->val > DUR_4 ) )
			note->oblique = true;
	}
	
	return note;
}
*/

/*
MusSymbol1 *MusMLFInput::ConvertSymbole( wxString line )
{
	wxStringTokenizer tkz( line , "_");
	if ( !tkz.HasMoreTokens() )
		return NULL;

	int code;

	wxString str = tkz.GetNextToken();
	if ( str == "S" )
	{
		if ( !tkz.HasMoreTokens() )
			return NULL;
		str = tkz.GetNextToken();
		if ( str == "B" )
		{
			MusSymbol1 *symbol = new MusSymbol1();
			symbol->flag = BARRE;
			symbol->code = '|';

			return symbol;
		}
	}
	else if ( str == "K" )
	{
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		str = tkz.GetNextToken(); 
		MusSymbol1 *symbol = new MusSymbol1();
		symbol->flag = CLE;
		if ( str == "S2" )
			symbol->code = SOL2;
		else if ( str == "S1" )
			symbol->code = SOL1;
		else if ( str == "S8" )
			symbol->code = SOLva;
		else if ( str == "PITCH_E" )
			symbol->code = FA4;
		else if ( str == "PITCH_D" )
			symbol->code = FA3;
		else if ( str == "U1" )
			symbol->code = UT1;
		else if ( str == "U2" )
			symbol->code = UT2;
		else if ( str == "U3" )
			symbol->code = UT3;
		else if ( str == "U5" )
			symbol->code = UT5;
		else if ( str == "U4" )
			symbol->code = UT4;
		else
			wxLogWarning( _("Unkown key") );

		return symbol;
	}
	else if ( str == "A" )
	{
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		str = tkz.GetNextToken();
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		wxString str1 = tkz.GetNextToken();
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		wxString str2 = tkz.GetNextToken(); 
		MusSymbol1 *symbol = new MusSymbol1();
		symbol->flag = ALTER;
		GetPitchWWG( str1.GetChar(0), &code );
		symbol->code = code;
		symbol->oct = atoi( str2.c_str() );
		if ( str == "D0" )
			symbol->calte = ACCID_SHARP;
		else if ( str == "B0" )
			symbol->calte = ACCID_FLAT;
		else if ( str == "H0" )
			symbol->calte = ACCID_NATURAL;
		else if ( str == "D1" )
			symbol->calte = ACCID_DOUBLE_SHARP;
		else if ( str == "B1" )
			symbol->calte = ACCID_DOUBLE_FLAT;
		else
			wxLogWarning( _("Unkown alteration") );

		return symbol;
	}
	else if ( str == "P" )
	{
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		wxString str1 = tkz.GetNextToken();
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		wxString str2 = tkz.GetNextToken();
		MusSymbol1 *symbol = new MusSymbol1();
		symbol->flag = PNT;
		GetPitchWWG( str1.GetChar(0), &code );
		symbol->code = code;
		symbol->oct = atoi( str2.c_str() );

		return symbol;
	}
	else if ( str == "M" )
	{
		if ( ! tkz.HasMoreTokens() )
			return NULL;

		MusSymbol1 *symbol = new MusSymbol1();
		symbol->flag = IND_MES;

		wxString str = tkz.GetNextToken();
		wxString option1;
		wxString option2;
		if ( tkz.HasMoreTokens() )
			option1 = tkz.GetNextToken();
		if ( tkz.HasMoreTokens() )
			option2 = tkz.GetNextToken();

		if ( str == "S" )
		{
			symbol->code = 64;
			if ( option1 == "C" )
				symbol->calte = 0;
			else if ( option1 == "CB" )
				symbol->calte = 1;
			else if ( option1 == "2" )
				symbol->calte = 2;
			else if ( option1 == "3" )
				symbol->calte = 3;
			else if ( option1 == "2B" )
				symbol->calte = 4;
			else if ( option1 == "3B" )
				symbol->calte = 5;
			else if ( option1 == "C" )
				symbol->calte = 0;
			else
				wxLogWarning( _("Unkown mesure signe indication") );
		}
		else if ( str == "CH" )
		{
			symbol->code = 1;
			symbol->durNum = atoi( option1.c_str() );
			symbol->durDen = atoi( option2.c_str() );
		}
		else 
		{
			if ( str == "TP" )
				symbol->code = 32;
			else if ( str == "TI" )
				symbol->code = 16;
			else if ( str == "TID" )
				symbol->code = 8;
			else
				wxLogWarning( _("Unkown mesure indication") );
			if ( option1 == "B" )
				symbol->code += 4;
			if (( option1 == "P" ) || ( option2 == "P" ))
				symbol->code += 2;
		}

		return symbol;
	}
	else if ( str != "SP" )
		wxLogWarning( _("Unknown symbol '%s'"), str.c_str() );

	return NULL;
}
*/

MusMLFInput::MusMLFInput( MusDoc *file, wxString filename ) :
    MusFileInputStream( file, filename )
{
	m_staff_i = m_staff_label = -1;
}

MusMLFInput::~MusMLFInput()
{
}


// copie le portee en convertissant les symbols de Ut vers Clef courrant
// si inPlace, directment dans staff

MusLaidOutStaff *MusMLFInput::GetNotUt1( MusLaidOutStaff *staff , bool inPlace )
{
/*
	if ( !staff )
		return NULL;

	int code, oct;

	MusLaidOutStaff *nstaff = staff;
	if ( !inPlace )
		nstaff = new MusLaidOutStaff( *staff );
	
	for (int i = 0; i < (int)nstaff->m_elements.GetCount(); i++ )
	{
		if ( nstaff->m_elements[i].IsNote() )
		{
			MusNote1 *note = (MusNote1*)&nstaff->m_elements[i];
			{
				GetNotUt1( nstaff, note, &code, &oct );
				note->code = code;
				note->oct = oct;
			}
		}
		else if ( nstaff->m_elements[i].IsSymbol() )
		{
			MusSymbol1 *symbol = (MusSymbol1*)&nstaff->m_elements[i];
			if ((symbol->flag == ALTER) || (symbol->flag == PNT))
			{
				GetNotUt1( nstaff, symbol, &code, &oct );
				symbol->code = code;
				symbol->oct = oct;
			}
		}
	}

	if ( inPlace )
		return NULL;
	else
		return nstaff;
    */
    wxLogDebug( "GetNotUt1 missing in ax2" );
    return NULL;
}


void MusMLFInput::GetNotUt1( MusLaidOutStaff *staff, MusLaidOutLayerElement *pelement, int *code, int *oct)
{
/*
	if (!pelement || !code || !oct) return;

	char valeur = 0;
	int offs;

	if ( !staff || ( staff->GetClef( pelement, &valeur, 1 ) == 0 ) )
		valeur = 5;
	MusSymbol1::calcoffs( &offs, valeur );

	*oct = pelement->oct;
	*code = 0;

	if (pelement->IsSymbol())
		*code = ((MusSymbol1*)pelement)->code;
	else if (pelement->IsNote())
		*code = ((MusNote1*)pelement)->code;

	while ( (*code) - offs < 1 )
	{
		(*code) += 7;
		(*oct)--;
	}
	while ( (*code) - offs > 7 )
	{
		(*code) -= 7;
		(*oct)++;
	}
	(*code) -= offs;
    */
    wxLogDebug( "GetNotUt1 missing in ax2" );
    
}

void MusMLFInput::GetPitchWWG( char code , int *code1 )
{
	if ( !code1 ) return;

	switch (code)
	{
		case 'C': *code1 = 1; break;
		case 'D': *code1 = 2; break;
		case 'E': *code1 = 3; break;
		case 'F': *code1 = 4; break;
		case 'G': *code1 = 5; break;
		case 'A': *code1 = 6; break;
		case 'B': *code1 = 7; break;
	}
}
	
bool MusMLFInput::ReadLine( wxString *line )
{
	if ( !line )
		return false;

	*line = "";
	
	char c = GetC();
	
	if ( !LastRead() )
		return false;

	while ( LastRead() && (c != '\n' ))
	{
		(*line) += c;
		c = GetC();
	}
	return true;
}


bool MusMLFInput::ReadLabelStr( wxString label )
{
	if ( label[0] != '"')
		return false;

	if ( label.Freq( '.' ) < 2 )
		return false;

	if ( label.Freq( '_' ) < 1 )
		return false;

	wxString str = label.BeforeLast('.'); // remove .lab"
	//m_segment_label = atoi( str.AfterLast('.').c_str() );
	str = str.BeforeLast('.'); // remove .seg"
	m_staff_label = atoi ( str.AfterLast('_').c_str() );

	return true;
}


bool MusMLFInput::ImportFile( int staff_per_page )
{
    /*
	m_doc->m_pages.Clear();

	wxString line;
	if ( !ReadLine( &line )  || (line != "#!MLF!#" ))
		return false;

	MusPage *page = NULL;
	MusLaidOutStaff *staff = NULL;
	int offset;

	if ( staff_per_page != -1 ) 
	// staff per page is given
	// just fill page one after the othe
	{
		int n_staff = 0;
		while ( ReadLine( &line ) && ReadLabelStr( line ) )
		{
			if ( !page ) // new page
			{
				page = new MusPage();
				n_staff = 0;
				offset = 0;
			}
			staff = new MusLaidOutStaff();
			ReadLabel( staff, offset );
			MusLaidOutStaff *staff2 = GetNotUt1( staff );
			delete staff;
			staff = staff2;
			page->m_staves.Add( staff );
			staff = NULL;
			n_staff++;
			if ( n_staff == staff_per_page )
			{
				m_doc->m_pages.Add( page );
				m_doc->m_parameters.nbpage++;
				page = NULL;
			}
		}

	}
	else
	{
		wxASSERT_MSG( false, "Staff per page should be specified in MFLInput ImportFile" );
	}

	if ( page && staff )
	{
		GetNotUt1( staff, true );
		page->m_staves.Add( staff );
	}
	if ( page )
		m_doc->m_pages.Add( page );
	m_doc->CheckIntegrity();

    */
    wxLogError( "MusMLFInput::ImportFile missing in ax2" );
    return true;
}


// permet d'importer un fichier par page
// dans ce cas la premiere ligne == #!MLF!#
// Si imPage, ajustera les position en fonction des position x dans imPage (staff)

bool MusMLFInput::ReadPage( MusPage *page , bool firstLineMLF, ImPage *imPage )
{
    /*
	wxString line;
	if ( firstLineMLF  && ( !ReadLine( &line )  || ( line != "#!MLF!#" )))
		return false;

	MusLaidOutStaff *staff = NULL;
	int offset;

	while ( ReadLine( &line ) && ReadLabelStr( line ) )
	{
		if ( m_staff_i < m_staff_label ) // new staff
		{
			if ( staff )
				GetNotUt1( staff, true ); // convert pitches

			if ( m_staff_label < (int)page->m_staves.GetCount() )
			{
				staff = &page->m_staves[ m_staff_label ];
				m_staff_i = m_staff_label; //m_staff_i++;
			}
			offset = 0;
		}

		if ( staff )
		{
			if ( imPage )
				offset = imPage->m_staves[m_staff_i].m_x1;
			ReadLabel( staff, offset );
		}
	}

	if ( staff )
		GetNotUt1( staff, true ); // convert pitches

	page->CheckIntegrity();
    */
    wxLogError( "MusMLFInput::ReadPage missing in ax2" );
    return true;
}

// offset est la position x relative du label
// normalement donne par imPage si present

bool MusMLFInput::ReadLabel( MusLaidOutStaff *staff, int offset )
{
/*
	bool is_note;
	int pos = 0;
	wxString line;

	while ( ReadLine( &line ) &&  MusMLFInput::IsElement( &is_note, &line, &pos ) )
	{
		if ( !is_note )
		{
			MusSymbol1 *s = MusMLFInput::ConvertSymbole( line );
			if ( s )
			{
				s->xrel = pos + offset;
				staff->m_elements.Add( s );
			}
		}
		else
		{
			MusNote1 *n = MusMLFInput::ConvertNote( line );
			if ( n )
			{
				n->xrel = pos + offset;
				staff->m_elements.Add( n );
			}
		}
	} 
*/
    wxLogDebug( "ReadLabel missing in ax2" );
    return true;
}



