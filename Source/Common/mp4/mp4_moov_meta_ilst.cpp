/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "Common/mp4/mp4_.h"
//---------------------------------------------------------------------------

//***************************************************************************
// WAVE
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_meta_ilst::Read_Internal ()
{
    //Integrity
    if (Global->moov_meta_ilst)
        throw exception_read_block("2 moov meta ilst blocks");
    if (!Global->moov_meta_keys)
        throw exception_read_block("moov meta ilst is expected after moov meta keys");
    Global->moov_meta_ilst=new mp4_Base::global::block_moov_meta_ilst;

    //Reading
    Read_Internal_ReadAllInBuffer();

    //Parsing
    while (Chunk.Content.Buffer_Offset < Chunk.Content.Size)
    {
        int32u Key_size, Key_Pos;
        Get_B4(Key_size);
        if (Key_size<8)
            throw exception_read_block("moov meta ilst atom size invalid");
        Get_B4(Key_Pos);
        if (!Key_Pos || Key_Pos>Global->moov_meta_keys->Keys.size())
            throw exception_read_block("moov meta ilst atom name invalid");
        if (Global->moov_meta_keys->Keys[Key_Pos-1]=="com.universaladid.idregistry" || Global->moov_meta_keys->Keys[Key_Pos-1]=="com.universaladid.idvalue")
        {
            int64u SizeMax=Chunk.Content.Buffer_Offset+Key_size-8;
            while (Chunk.Content.Buffer_Offset<SizeMax)
            {
                int32u size, name;
                Get_B4(size);
                if (size<8)
                    throw exception_read_block("moov meta ilst atom size invalid");
                Get_B4(name);
                if (name==0x64617461) //data
                {
                    if (size<16)
                        throw exception_read_block("moov meta ilst data atom size invalid");
                    int32u format, locale;
                    Get_B4(format);
                    if (format!=1)
                        throw exception_read_block("universaladid format out of specs");
                    Get_B4(locale);
                    if (locale)
                        throw exception_read_block("universaladid locale out of specs");
                    string Value;
                    Get_String(size-16, Value);
                    Global->moov_meta_ilst->KnownValues[Global->moov_meta_keys->Keys[Key_Pos - 1]] = Value;
                }
                else
                    Skip_XX(size-8);
            }
        }
        else
        {
            //Skip
            Skip_XX(Key_size-8);
        }

    }
}

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
        Put_B4((int32u)(8+8+8+Global->moov_meta_ilst_NewValues[i].size()));
        Put_B4((int32u)(1+Global->moov_meta_keys_AlreadyPresent-Global->moov_meta_ilst_NewValues.size()+i)); //1-ordered key position
        Put_B4((int32u)(8+8+Global->moov_meta_ilst_NewValues[i].size()));
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
