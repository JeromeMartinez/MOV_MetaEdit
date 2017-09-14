/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

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
    mp4_Base::global::block_mdat* mdat=new mp4_Base::global::block_mdat;
    mdat->File_Offset=Global->In.Position_Get();
    mdat->Size=Chunk.Content.Size;
    Global->mdat=mdat;
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

