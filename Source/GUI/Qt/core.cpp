/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include <QDirIterator>
#include <QStringList>
#include <QFileInfo>
#include <QString>
#include <QPair>
#include <QDir>

#include "tablewidget.h"
#include "core.h"
#include "ZenLib/Ztring.h"
using namespace ZenLib;

//---------------------------------------------------------------------------
Core::Core()
{
}

//---------------------------------------------------------------------------
void Core::Dummy_Handler(const QString &FileName)
{

    FileInfo Current;
    MetaDataList MetaData;

    Current.Modified = false;

    Current.CurrentRegistry = "ad-id.org";
    MetaData.insert("ad-id.org", "");

    QString BaseName = QFileInfo(FileName).baseName();
    int Pos = 0;
    if(AdIdValidator().validate(BaseName, Pos) == QValidator::Acceptable)
    {
        MetaData["ad-id.org"] = BaseName;
        Current.Modified = true;
    }

    Current.MetaData = MetaData;

    Current.H = new mp4_Handler();
    Current.H->Open(FileName.toLocal8Bit().constData());
    if (Current.H->PerFile_Error.str().empty())
        Current.Valid = true;

    Files.insert(FileName, Current);
}

//---------------------------------------------------------------------------
size_t Core::Open_Files(const QString &FileName)
{
    if(!QFileInfo(FileName).exists())
        return 0;

    QStringList List;
    if(QFileInfo(FileName).isFile())
        List.push_back(FileName);
    else
    {
        QDirIterator It(FileName, QDir::Files, QDirIterator::Subdirectories);
        while(It.hasNext())
            List.push_back(It.next());
    }

    for(int Pos = 0; Pos < List.size(); Pos++)
        Dummy_Handler(List[Pos]);

    return List.size();
}

//---------------------------------------------------------------------------
MetaDataList* Core::Get_MetaData(const QString& FileName)
{
    if(Files.contains(FileName))
        return &Files[FileName].MetaData;

    return NULL;
}

//---------------------------------------------------------------------------
bool Core::Save_File(const QString& FileName)
{
    if (Files.contains(FileName))
    {
        FileInfo &F=Files[FileName];
        
        Ztring Registry, Value;
        Registry.From_UTF8(F.CurrentRegistry.toUtf8().constData());
        Value.From_UTF8(F.MetaData.value(F.CurrentRegistry).toUtf8().constData());
        F.H->Set("com.universaladid.idregistry", Registry.To_Local(), mp4_Handler::rules());
        F.H->Set("com.universaladid.idvalue", Value.To_Local(), mp4_Handler::rules());

        F.H->Save();
    }

    return true;
}
