// BWF MetaEdit Riff - RIFF stuff for BWF MetaEdit
//
// This code was created in 2010 for the Library of Congress and the
// other federal government agencies participating in the Federal Agencies
// Digitization Guidelines Initiative and it is in the public domain.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#include "Common/mp4/mp4_.h"
//---------------------------------------------------------------------------

//***************************************************************************
// Top level element
//***************************************************************************

//---------------------------------------------------------------------------
mp4::mp4()
    :mp4_Base(NULL, 0)
{
    Global=new global();
}

//---------------------------------------------------------------------------
mp4::~mp4()
{
    delete Global; //Global=NULL
}

//---------------------------------------------------------------------------
void mp4::Read_Internal()
{
    SUBS_BEGIN();
        SUB_ELEMENT(mdat);
        SUB_ELEMENT(moov);
    SUBS_END();
}

