/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#ifndef CORE_H
#define CORE_H

#include "Common/mp4_Handler.h"
#include <QMap>
#include <QPair>
#include <QString>

typedef QPair<QString, QString> MetaDataType;

struct FileInfo
{
    bool         Valid;
    bool         Modified;
    MetaDataType MetaData;
    mp4_Handler* H;

    FileInfo()
        : Valid(false)
        , Modified(false)
        , H(NULL)
    {}
};

typedef QMap<QString, FileInfo> FileList;

class Core
{
public:
    Core();
    void Dummy_Handler(const QString &FileName);

    size_t Open_Files(const QString& FileName);

    MetaDataType* Get_MetaData(const QString& FileName);

    size_t Files_Count() { return Files.size(); }

    FileList* Get_Files() { return &Files; }

    bool Save_File(const QString& FileName);

private:
    FileList Files;
};

#endif // CORE_H
