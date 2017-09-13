// BWF MetaEdit Riff - RIFF stuff for BWF MetaEdit
//
// This code was created in 2010 for the Library of Congress and the
// other federal government agencies participating in the Federal Agencies
// Digitization Guidelines Initiative and it is in the public domain.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#include "Common/mp4_Handler.h"
#include "Common/mp4/mp4_.h"
#include <sstream>
#include <iostream>
#include "ZenLib/File.h"
#include "ZenLib/Dir.h"
using namespace std;
using namespace ZenLib;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
const string mp4_Handler_EmptyZtring_Const; //Use it when we can't return a reference to a true Ztring, const version
//---------------------------------------------------------------------------

//***************************************************************************
// Const
//***************************************************************************

enum xxxx_Fields
{
    Fields_Tech,
    Fields_Bext,
    Fields_Info,
    Fields_Max,
};

size_t xxxx_Strings_Size[]=
{
    4,  //Tech
    15,  //Bext
    17, //Info
};

const char* xxxx_Strings[][17]=
{
    {
        "XMP",
        "aXML",
        "iXML",
        "MD5Stored",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
    },
    {
        "Description",
        "Originator",
        "OriginatorReference",
        "OriginationDate",
        "OriginationTime",
        "TimeReference (translated)",
        "TimeReference",
        "BextVersion",
        "UMID",
        "LoudnessValue",
        "LoudnessRange",
        "MaxTruePeakLevel",
        "MaxMomentaryLoudness",
        "MaxShortTermLoudness",
        "CodingHistory",
        "",
        "",
    },
    {
        //Note: there is a duplicate in mp4_Chunks_INFO_xxxx
        "IARL", //Archival Location
        "IART", //Artist
        "ICMS", //Commissioned
        "ICMT", //Comment
        "ICOP", //Copyright
        "ICRD", //Date Created
        "IENG", //Engineer
        "IGNR", //Genre
        "IKEY", //Keywords
        "IMED", //Medium
        "INAM", //Title
        "IPRD", //Product
        "ISBJ", //Subject
        "ISFT", //Software
        "ISRC", //Source
        "ISRF", //Source Form
        "ITCH", //Technician
    },
};

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
mp4_Handler::mp4_Handler ()
{
    //Configuration
    riff2rf64_Reject=false;
    Overwrite_Reject=false;
    NoPadding_Accept=false;
    NewChunksAtTheEnd=false;
    GenerateMD5=false;
    VerifyMD5=false;
    EmbedMD5=false;
    EmbedMD5_AuthorizeOverWritting=false;
    Bext_DefaultVersion=0;
    Bext_MaxVersion=2;

    //Internal
    Chunks=NULL;
    File_IsValid=false;
    File_IsCanceled=false;
}

//---------------------------------------------------------------------------
mp4_Handler::~mp4_Handler ()
{
    delete Chunks; //Chunks=NULL;
}

//***************************************************************************
// I/O
//***************************************************************************

//---------------------------------------------------------------------------
bool mp4_Handler::Open(const string &FileName)
{
    //Init
    PerFile_Error.str(string());
    File_IsValid=false;
    File_IsCanceled=false;
    bool ReturnValue=true;
    
    //Global info
    delete Chunks; Chunks=new mp4();
    Chunks->Global->File_Name.From_Local(FileName);

    //Opening file
    if (!File::Exists(Chunks->Global->File_Name) || !Chunks->Global->In.Open(Chunks->Global->File_Name))
    {
        Errors<<FileName<<": File does not exist"<<endl;
        PerFile_Error<<"File does not exist"<<endl;
        return false;
    }
    Chunks->Global->File_Size=Chunks->Global->In.Size_Get();
    Chunks->Global->File_Date=Chunks->Global->In.Created_Local_Get();
    if (Chunks->Global->File_Date.empty())
        Chunks->Global->File_Date=Chunks->Global->In.Modified_Local_Get();

    //Base
    mp4_Base::chunk Chunk;
    Chunk.Content.Size=Chunks->Global->File_Size;
    Options_Update();

    //Parsing
    try
    {
        Chunks->Read(Chunk);
        File_IsValid=true;
    }
    catch (exception_canceled &)
    {
        CriticalSectionLocker(Chunks->Global->CS);
        File_IsCanceled=true;
        Chunks->Global->Canceling=false;
        ReturnValue=false;
    }
    catch (exception_read_chunk &e)
    {
        Errors<<Chunks->Global->File_Name.To_Local()<<": "<<Ztring().From_CC4(Chunk.Header.Name).To_Local()<<" "<<e.what()<<endl;
        PerFile_Error<<Ztring().From_CC4(Chunk.Header.Name).To_Local()<<" "<<e.what()<<endl;
    }
    catch (exception &e)
    {
        Errors<<Chunks->Global->File_Name.To_Local() <<": "<<e.what()<<endl;
        PerFile_Error<<e.what()<<endl;
        ReturnValue=false;
    }

    //Cleanup
    Chunks->Global->In.Close();

    CriticalSectionLocker(Chunks->Global->CS);
    Chunks->Global->Progress=1;
    
    return ReturnValue;
}

//---------------------------------------------------------------------------
bool mp4_Handler::Save()
{
    Chunks->Global->CS.Enter();
    Chunks->Global->Progress=(float)0.05;
    Chunks->Global->CS.Leave();
    
    //Init
    PerFile_Error.str(string());

    //Integrity
    if (Chunks==NULL)
    {
        Errors<<"(No file name): Internal error"<<endl;
        return false;
    }

    //Write only if modified
    if (!IsModified_Get())
    {
        Information<<Chunks->Global->File_Name.To_Local()<<": Nothing to do"<<endl;
        return false;
    }

    //Modifying the chunks in memory
    /*
    for (size_t Fields_Pos=0; Fields_Pos<Fields_Max; Fields_Pos++)
        for (size_t Pos=0; Pos<xxxx_Strings_Size[Fields_Pos]; Pos++)
        {
            if (!IsOriginal(xxxx_Strings[Fields_Pos][Pos], Get(xxxx_Strings[Fields_Pos][Pos])))
                Chunks->Modify(Elements::WAVE, Chunk_Name2_Get(xxxx_Strings[Fields_Pos][Pos]), Chunk_Name3_Get(xxxx_Strings[Fields_Pos][Pos]));
        }
    */
    Chunks->Modify(Elements::moov, Elements::moov_meta, Elements::moov_meta_hdlr);
    Chunks->Modify(Elements::moov, Elements::moov_meta, Elements::moov_meta_keys);
    Chunks->Modify(Elements::moov, Elements::moov_meta, Elements::moov_meta_ilst);

    //Opening files
    if (!Chunks->Global->In.Open(Chunks->Global->File_Name))
    {
        Errors<<Chunks->Global->File_Name.To_Local()<<": File does not exist anymore"<<endl;
        PerFile_Error<<"File does not exist anymore"<<endl;
        return false;
    }

    //Old temporary file
    if (File::Exists(Chunks->Global->File_Name+__T(".tmp")) && !File::Delete(Chunks->Global->File_Name+__T(".tmp")))
    {
        Errors<<Chunks->Global->File_Name.To_Local()<<": Old temporary file can't be deleted"<<endl;
        PerFile_Error<<"Old temporary file can't be deleted"<<endl;
        return false;
    }
    
    //Parsing
    try
    {
        Chunks->Write();
    }
    catch (exception_canceled &)
    {
        Chunks->Global->Out.Close();
        File::Delete(Chunks->Global->File_Name+__T(".tmp"));
        CriticalSectionLocker(Chunks->Global->CS);
        File_IsCanceled=true;
        Chunks->Global->Canceling=false;
        return false;
    }
    catch (exception &e)
    {
        Errors<<Chunks->Global->File_Name.To_Local() <<": "<<e.what()<<endl;
        return false;
    }

    //Log
    Information<<(Chunks?Chunks->Global->File_Name.To_Local() :"")<<": Is modified"<<endl;

    //Loading the new file (we are verifying the integraty of the generated file)
    string FileName=Chunks->Global->File_Name.To_Local();
    bool GenerateMD5_Temp=Chunks->Global->GenerateMD5;
    Chunks->Global->GenerateMD5=false;
    if (!Open(FileName) && Chunks==NULL) //There may be an error but file is open (eg MD5 error)
    {
        Errors<<FileName<<": WARNING, the resulting file can not be validated, file may be CORRUPTED"<<endl;
        PerFile_Error<<"WARNING, the resulting file can not be validated, file may be CORRUPTED"<<endl;
        Chunks->Global->GenerateMD5=GenerateMD5_Temp;
        return false;
    }
    Chunks->Global->GenerateMD5=GenerateMD5_Temp;

    CriticalSectionLocker(Chunks->Global->CS);
    Chunks->Global->Progress=1;
    
    return true;
}

//***************************************************************************
// Per Item
//***************************************************************************

//---------------------------------------------------------------------------
string mp4_Handler::Get(const string &Field)
{
    mp4_Base::global::chunk_strings** Chunk_Strings=chunk_strings_Get(Field);
    if (!Chunk_Strings || !*Chunk_Strings)
        return string();
    
    return Get(Field_Get(Field), *Chunk_Strings);
}

//---------------------------------------------------------------------------
bool mp4_Handler::Set(const string &Field, const string &Value, rules Rules)
{
    //Integrity
    if (Chunks==NULL)
    {
        Errors<<"(No file name): Internal error"<<endl;
        return false;
    }

    //Setting it
    if (Field=="com.universaladid.idregistry" || Field == "com.universaladid.idvalue")
    {
        Chunks->Global->moov_meta_keys_NewKeys.push_back(Field);
        Chunks->Global->moov_meta_ilst_NewValues.push_back(Value);
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
bool mp4_Handler::Remove(const string &Field)
{
    //Integrity
    if (Chunks==NULL)
    {
        Errors<<"(No file name): Internal error"<<endl;
        return false;
    }

    return Set(Field, string(), *chunk_strings_Get(Field), Chunk_Name2_Get(Field), Chunk_Name3_Get(Field)); 
}

//---------------------------------------------------------------------------
bool mp4_Handler::IsModified(const string &Field)
{
    mp4_Base::global::chunk_strings** Chunk_Strings=chunk_strings_Get(Field);
    if (!Chunk_Strings || !*Chunk_Strings)
        return false;
    
    return IsModified(Field_Get(Field=="timereference (translated)"?string("timereference"):Field), *Chunk_Strings);
}

//---------------------------------------------------------------------------
bool mp4_Handler::IsValid(const string &Field_, const string &Value_, rules Rules)
{
    if (!IsValid_Errors.str().empty())
    {
        Errors<<(Chunks?Chunks->Global->File_Name.To_Local():"")<<": "<<IsValid_Errors.str()<<endl;
        return false;
    }
    else
        return true;
}

//---------------------------------------------------------------------------
bool mp4_Handler::IsOriginal(const string &Field, const string &Value)
{
    mp4_Base::global::chunk_strings** Chunk_Strings=chunk_strings_Get(Field);
    if (!Chunk_Strings || !*Chunk_Strings)
        return true;
    
    return IsOriginal(Field_Get(Field=="timereference (translated)"?string("timereference"):Field), Value, *Chunk_Strings);
}

//---------------------------------------------------------------------------
string mp4_Handler::History(const string &Field)
{
    mp4_Base::global::chunk_strings** Chunk_Strings=chunk_strings_Get(Field);
    if (!Chunk_Strings || !*Chunk_Strings)
        return string();
    
    return History(Field_Get(Field), *Chunk_Strings);
}

//***************************************************************************
// Info
//***************************************************************************

//---------------------------------------------------------------------------
string mp4_Handler::Trace_Get()
{
    return Chunks->Global->Trace.str();
}

//---------------------------------------------------------------------------
string mp4_Handler::FileName_Get()
{
    return Chunks->Global->File_Name.To_Local();
}

//---------------------------------------------------------------------------
string mp4_Handler::FileDate_Get()
{
    return Chunks->Global->File_Date.To_Local();
}

//---------------------------------------------------------------------------
float mp4_Handler::Progress_Get()
{
    if (Chunks==NULL || Chunks->Global==NULL)
        return 0;    
    CriticalSectionLocker(Chunks->Global->CS);
    if (Chunks->Global->Progress==1)
        return (float)0.99; //Must finish opening, see Open()
    return Chunks->Global->Progress;
}

//---------------------------------------------------------------------------
void mp4_Handler::Progress_Clear()
{
    if (Chunks==NULL || Chunks->Global==NULL)
        return;

    CriticalSectionLocker(Chunks->Global->CS);
    Chunks->Global->Progress=0;
}

//---------------------------------------------------------------------------
bool mp4_Handler::Canceled_Get()
{
    if (Chunks==NULL || Chunks->Global==NULL)
        return false;    
    CriticalSectionLocker(Chunks->Global->CS);
    return File_IsCanceled;
}

//---------------------------------------------------------------------------
void mp4_Handler::Cancel()
{
    if (Chunks==NULL || Chunks->Global==NULL)
        return;    
    CriticalSectionLocker(Chunks->Global->CS);
    Chunks->Global->Canceling=true;
}

//---------------------------------------------------------------------------
bool mp4_Handler::IsModified_Get()
{
    return true;

    bool ToReturn=false;
    for (size_t Fields_Pos=0; Fields_Pos<Fields_Max; Fields_Pos++)
        for (size_t Pos=0; Pos<xxxx_Strings_Size[Fields_Pos]; Pos++)
            if (IsModified(xxxx_Strings[Fields_Pos][Pos]))
                ToReturn=true;

    if (!ToReturn && Chunks)
        ToReturn=Chunks->IsModified();

    return ToReturn;
}

//---------------------------------------------------------------------------
bool mp4_Handler::IsValid_Get()
{
    return File_IsValid;
}

//***************************************************************************
// Helpers - Per item
//***************************************************************************

//---------------------------------------------------------------------------
string mp4_Handler::Get(const string &Field, mp4_Base::global::chunk_strings* &Chunk_Strings)
{
    if (!File_IsValid)
        return string();

    return mp4_Handler_EmptyZtring_Const;
}

//---------------------------------------------------------------------------
bool mp4_Handler::Set(const string &Field, const string &Value, mp4_Base::global::chunk_strings* &Chunk_Strings, int32u Chunk_Name2, int32u Chunk_Name3)
{
    if (!File_IsValid
     || &Chunk_Strings==NULL) 
        return false;
    
    if ((Chunk_Strings!=NULL && Value==Chunk_Strings->Strings[Field])
     || (Chunk_Strings==NULL && Value.empty()))
        return true; //Nothing to do

    //Overwrite_Reject
    if (Overwrite_Reject && Chunk_Strings!=NULL && !Chunk_Strings->Strings[Field].empty())
    {
        Errors<<(Chunks?Chunks->Global->File_Name.To_Local():"")<<": overwriting is not authorized ("<<Field<<")"<<endl;
        return false;
    }

    //Filling
    bool Alreadyexists=false;
    for (size_t Pos=0; Pos<Chunk_Strings->Histories[Field].size(); Pos++)
        if (Chunk_Strings->Histories[Field][Pos]==Chunk_Strings->Strings[Field])
            Alreadyexists=true;
    if (!Alreadyexists)
        Chunk_Strings->Histories[Field].push_back(Chunk_Strings->Strings[Field]);
    Chunk_Strings->Strings[Field]=Value;

    return true;
}

//---------------------------------------------------------------------------
bool mp4_Handler::IsOriginal(const string &Field, const string &Value, mp4_Base::global::chunk_strings* &Chunk_Strings)
{
    if (!File_IsValid || &Chunk_Strings==NULL || Chunk_Strings==NULL)
        return Value.empty();
   
    //Special cases
    if (Field=="timereference (translated)" && &Chunk_Strings && Chunk_Strings && Chunk_Strings->Strings.find("timereference")!=Chunk_Strings->Strings.end())
        return IsOriginal("timereference", Get("timereference"));

    if (Chunk_Strings->Histories[Field].empty())
        return Value==Chunk_Strings->Strings[Field];
   
    return Value==Chunk_Strings->Histories[Field][0];
}

//---------------------------------------------------------------------------
bool mp4_Handler::IsModified(const string &Field, mp4_Base::global::chunk_strings* &Chunk_Strings)
{
    if (!File_IsValid)
        return false;

    //Special cases
    if (Field=="timereference (translated)" && &Chunk_Strings && Chunk_Strings && Chunk_Strings->Strings.find("timereference")!=Chunk_Strings->Strings.end())
        return IsModified("timereference");

    if (&Chunk_Strings!=NULL && Chunk_Strings && Chunk_Strings->Histories.find(Field)!=Chunk_Strings->Histories.end())
    {
        //Special cases
        if (Field=="bextversion")
            return !Chunk_Strings->Histories["bextversion"].empty() && !(Chunk_Strings->Strings["bextversion"]=="0" || Chunk_Strings->Histories["bextversion"][0]==Chunk_Strings->Strings["bextversion"]);

        return !Chunk_Strings->Histories[Field].empty() && Chunk_Strings->Histories[Field][0]!=Chunk_Strings->Strings[Field];
    }
    else
        return false;
}

//---------------------------------------------------------------------------
string mp4_Handler::History(const string &Field, mp4_Base::global::chunk_strings* &Chunk_Strings)
{
    if (!File_IsValid)
        return string();

    return mp4_Handler_EmptyZtring_Const;
}

//***************************************************************************
// Configuration
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_Handler::Options_Update()
{
    if (Chunks==NULL || Chunks->Global==NULL)
        return;

    Chunks->Global->NoPadding_Accept=NoPadding_Accept;
    Chunks->Global->NewChunksAtTheEnd=NewChunksAtTheEnd;
    Chunks->Global->GenerateMD5=GenerateMD5;
    Chunks->Global->VerifyMD5=VerifyMD5;
    Chunks->Global->EmbedMD5=EmbedMD5;
    Chunks->Global->EmbedMD5_AuthorizeOverWritting=EmbedMD5_AuthorizeOverWritting;
}

//***************************************************************************
// Helpers - Retrieval of chunks info
//***************************************************************************

//---------------------------------------------------------------------------
mp4_Base::global::chunk_strings** mp4_Handler::chunk_strings_Get(const string &Field)
{
    if (Chunks==NULL || Chunks->Global==NULL)
        return NULL;    

    //INFO
    //if (Field.size()==4)
    //    return &Chunks->Global->INFO;
    
    //Unknown
    return NULL;
}

//---------------------------------------------------------------------------
string mp4_Handler::Field_Get(const string &Field)
{
    //Unknown
    return 0x00000000;
}

//---------------------------------------------------------------------------
int32u mp4_Handler::Chunk_Name2_Get(const string &Field)
{  
    //Unknown
    return 0x00000000;
}

//---------------------------------------------------------------------------
int32u mp4_Handler::Chunk_Name3_Get(const string &Field)
{
    //Unknown / not needed
    return 0x00000000;
}
