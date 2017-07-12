/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "CLI/Help.h"
//---------------------------------------------------------------------------

//***************************************************************************
//
//***************************************************************************

//---------------------------------------------------------------------------
static string Program_Name = "movmetaedit";

void Set_Program_Name(const char* Name)
{
    Program_Name = Name;
    #if defined(_MSC_VER)
        Program_Name = Program_Name.substr(Program_Name.rfind('\\')+1);
        Program_Name = Program_Name.substr(0, Program_Name.find('.'));
    #else
        Program_Name = Program_Name.substr(Program_Name.rfind('/')+1);
    #endif
}

//---------------------------------------------------------------------------
ReturnValue Help()
{
    STRINGOUT(string("Usage: \" [options] FileNames [options]\"").insert(8, Program_Name));
    TEXTOUT("");
    TEXTOUT("Help:");
    TEXTOUT("  --help, -h");
    TEXTOUT("      Display this help and exit");
    TEXTOUT("  --help-par");
    TEXTOUT("      Display help for modificatons related to pixel aspect ratio");
    TEXTOUT("  --help-adid");
    TEXTOUT("      Display help for modifications related to Ad-iD");
    TEXTOUT("  --version");
    TEXTOUT("      Display version and exit");
    TEXTOUT("");
    TEXTOUT("Global options");
    TEXTOUT("  --simulate, -s");
    TEXTOUT("      Do not modify file (only display of potential modifications)");
    TEXTOUT("");
    TEXTOUT("If no option, aspect ratio information is displayed, no modification");
    TEXTOUT("");
    TEXTOUT("Return value:");
    TEXTOUT(">=0: OK");
    TEXTOUT("1: Warning");
    TEXTOUT("2: Error");

    return ReturnValue_OK;
}

//---------------------------------------------------------------------------
ReturnValue Help_AdID()
{
    TEXTOUT("Options related to Ad-iD:");
    TEXTOUT("  --add-adid Value");
    TEXTOUT("      Add an Id-iD value");
    TEXTOUT("  --add-adid-registry Value");
    TEXTOUT("      Id-iD is stored with the specified value as registry");
    TEXTOUT("      (default is \"ad-id.org\")");
    TEXTOUT("");
    TEXTOUT("Examples:");
    STRINGOUT(string(" --add-adid ADID0000000 FileName.mov").insert(0, Program_Name));
    STRINGOUT(string(" --add-adid-registry example.com --add-adid ADID0000000 FileName.mov").insert(0, Program_Name));

    return ReturnValue_OK;
}

//---------------------------------------------------------------------------
ReturnValue Help_PAR()
{
    TEXTOUT("Options related to NTSC aspect ratio:");
    TEXTOUT("  --par, -p");
    TEXTOUT("      Modify PAR to 9:10 (--> DAR of 4:3)");
    TEXTOUT("  --width-scale, -w");
    TEXTOUT("      Modify width scale to 0.9");
    TEXTOUT("");
    TEXTOUT("Limitation: 720x480 or 720x486 or 720x576");
    TEXTOUT("  --simulate, -s");
    TEXTOUT("      Do not modify file (only display of potential modifications)");

    return ReturnValue_OK;
}

//---------------------------------------------------------------------------
ReturnValue Usage()
{
    STRINGOUT(string("Usage: \" [options] FileNames [options]\"").insert(8, Program_Name));
    STRINGOUT(string("\" --help\" for displaying more information").insert(1, Program_Name));

    return ReturnValue_ERROR;
}
