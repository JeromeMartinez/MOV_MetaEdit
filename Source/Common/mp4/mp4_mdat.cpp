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
// WAVE data
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_mdat::Read_Internal ()
{
    //Filling
    Global->mdat=new mp4_Base::global::chunk_data;
    Global->mdat->File_Offset=Global->In.Position_Get();
    Global->mdat->Size=Chunk.Content.Size;

    //MD5
    try
    {
        Chunk.Content.Buffer=new int8u[65536];
    }
    catch(...)
    {
        throw exception_read_chunk("Problem during memory allocation");
    }
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_mdat::Modify_Internal ()
{
    //No modification is possible
}

//***************************************************************************
// Write
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_mdat::Write_Internal ()
{
    if (Global->Out_Buffer_File_TryModification)
        Global->Out_Buffer_WriteAtEnd=true;
    else
        mp4_Base::Write_Internal();
}

