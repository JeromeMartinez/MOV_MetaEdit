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
// WAVE
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_meta_ilst_xxxx_data::Read_Internal ()
{
    if (!Global->moov_meta_ilst_xxxxCurrent)
        throw exception_valid("moov meta ilst data index is 0");
    if (Global->moov_meta_ilst_xxxxCurrent>Global->moov_meta_keys_AlreadyPresent)
        throw exception_valid("moov meta ilst data without corresponding key");

    //Reading
    Read_Internal_ReadAllInBuffer();

    int32u Type, Locale;
    Get_B4(Type);
    Get_B4(Locale);
    /*const string &Key_value=Global->moov_meta_keys[Global->moov_meta_ilst_xxxxCurrent-1];
    if (Key_value=="com.universaladid.idregistry" || Key_value=="com.universaladid.idvalue")
    {
        if (Type!=1)
            throw exception_valid(Key_value+" must have Type of 1 (UTF-8)");
        if (Locale)
            throw exception_valid(Key_value+" must have no Locale");
        string Value;
        Get_String(Chunk.Content.Size-8, Value);
        Global->moov_meta_values[Global->moov_meta_ilst_xxxxCurrent]=Value;
    }
    else*/
        Skip_XX(Chunk.Content.Size-8); //Value
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_meta_ilst_xxxx_data::Modify_Internal()
{
    /*string Field = Ztring().From_CC4(Chunk.Header.Name).MakeUpperCase();
    if (Global->INFO->Strings[Field].empty())
    {
        Chunk.Content.IsRemovable = true;
        return;
    }

    //Calculating size
    if (Global->INFO->Strings[Field].size() >= 0xFFFFFFFF)
        return; //TODO: error

                //Creating buffer
    Chunk.Content.Buffer_Offset = 0;
    Chunk.Content.Size = Global->INFO->Strings[Field].size();
    delete[] Chunk.Content.Buffer; Chunk.Content.Buffer = new int8u[Global->INFO->Strings[Field].size()];

    Put_String(Global->INFO->Strings[Field].size(), Global->INFO->Strings[Field]);

    Chunk.Content.IsModified = true;
    Chunk.Content.Size_IsModified = true;*/
}

//***************************************************************************
// Write
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_meta_ilst_xxxx_data::Write_Internal()
{
    //Riff_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
