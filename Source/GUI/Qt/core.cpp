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
#include "Common/mp4_Handler.h"
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

    MetaData.insert("ad-id.org", "");

    QString BaseName = QFileInfo(FileName).baseName();
    int Pos = 0;
    if(AdIdValidator().validate(BaseName, Pos) == QValidator::Acceptable)
    {
        MetaData["ad-id.org"] = BaseName;
        Current.Modified = true;
    }

    Current.MetaData = MetaData;
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
        mp4_Handler H;
        if (!H.Open(Ztring().From_UTF8(FileName.toUtf8().constData()).To_Local()))
        {
            int A = 0;
        }
        if (H.Canceled_Get())
        {
            int A = 0;
        }
        if (H.IsModified_Get())
        {
            int A = 0;
        }

        if (!H.Errors.str().empty())
        {
            //cout << H.Errors.str();
            return 1;
        }

        Ztring Registry, Value;
        Registry.From_UTF8(Files[FileName].MetaData.firstKey().toUtf8().constData());
        Value.From_UTF8(Files[FileName].MetaData.first().toUtf8().constData());
        H.Set("com.universaladid.idregistry", Registry.To_Local(), mp4_Handler::rules());
        H.Set("com.universaladid.idvalue", Value.To_Local(), mp4_Handler::rules());

        H.Save();
    }

    return NULL;
}
