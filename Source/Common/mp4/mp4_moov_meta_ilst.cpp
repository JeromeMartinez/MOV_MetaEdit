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
void mp4_moov_meta_ilst::Read_Internal ()
{
    /*
    SUBS_BEGIN();
        SUB_ELEMENT_DEFAULT(moov_meta_ilst_xxxx);
    SUBS_END_DEFAULT();
    */

    //Integrity
    if (Global->moov_meta_ilst_AlreadyPresent)
        throw exception_valid("2 moov meta ilst chunks");
    Global->moov_meta_ilst_AlreadyPresent++; //Global, we don't read ilst

    //Reading
    Read_Internal_ReadAllInBuffer();
}

/*
//---------------------------------------------------------------------------
size_t mp4_moov_meta_ilst::Insert_Internal (int32u Chunk_Name_Insert)
{
    mp4_Base* NewChunk;
    switch (Chunk_Name_Insert)
    {
        default                  :  return Subs.size();
    }

    size_t Subs_Pos;
    switch (Chunk_Name_Insert)
    {
        default                  :  return Subs.size();
    }

    NewChunk->Modify();
    if (!NewChunk->IsRemovable())
    {
        if (Subs_Pos<Subs.size())
        {
            Subs.insert(Subs.begin()+Subs_Pos+1, NewChunk); //First place after fmt, always
            return Subs_Pos+1;
        }
        else
        {
            Subs.push_back(NewChunk); //At the end
            return Subs.size()-1;
        }
    }
    else
    {
        delete NewChunk; //NewChunk=NULL;
        return Subs.size();
    }
}
*/

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_meta_ilst::Modify_Internal()
{
    if (!Global->moov_meta_ilst_AlreadyPresent && Global->moov_meta_ilst_NewValues.empty())
    {
        Chunk.Content.IsRemovable = true;
        return;
    }

    if (Global->moov_meta_ilst_NewValues.empty())
        return; //No change

    //Creating buffer
    Chunk.Content.Buffer_Offset=Chunk.Content.Size;
    size_t Size_ToAdd=0;
    for (size_t i=0; i<Global->moov_meta_ilst_NewValues.size(); i++)
        Size_ToAdd+=8+8+8+Global->moov_meta_ilst_NewValues[i].size();
    int8u* t=new int8u[Chunk.Content.Size+Size_ToAdd];
    memcpy(t, Chunk.Content.Buffer, Chunk.Content.Size);
    delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer=t;
    Chunk.Content.Size+=Size_ToAdd;

    for (size_t i=0; i<Global->moov_meta_ilst_NewValues.size(); i++)
    {
        Put_B4(8+8+8+Global->moov_meta_ilst_NewValues[i].size());
        Put_B4(1+Global->moov_meta_keys_AlreadyPresent-Global->moov_meta_ilst_NewValues.size()+i); //1-ordered key position
        Put_B4(8+8+Global->moov_meta_ilst_NewValues[i].size());
        Put_B4(0x64617461); //data
        Put_B4(0x00000001); //UTF-8
        Put_B4(0x00000000); //No locale
        Put_String(Global->moov_meta_ilst_NewValues[i].size(), Global->moov_meta_ilst_NewValues[i]);
    }
    Global->moov_meta_ilst_AlreadyPresent+=Global->moov_meta_ilst_NewValues.size();
    Global->moov_meta_ilst_NewValues.clear();

    Chunk.Content.IsModified = true;
    Chunk.Content.Size_IsModified = true;
}

//***************************************************************************
// Write
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_meta_ilst::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
