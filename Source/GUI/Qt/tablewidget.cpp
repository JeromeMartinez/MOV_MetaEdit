/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include <QResizeEvent>
#include <QScrollBar>
#include <QFont>
#include <QDebug>

#include "tablewidget.h"

//***************************************************************************
// Info
//***************************************************************************

static const int ColumnSize[MAX_COLUMN] =
{
    60,
    20,
    20,
};

//***************************************************************************
// Helpers
//***************************************************************************


//---------------------------------------------------------------------------
AdIdValidator::AdIdValidator(QObject* Parent) : QValidator(Parent)
{
}

//---------------------------------------------------------------------------
QValidator::State AdIdValidator::validate(QString& Input, int& Pos) const
{
    Q_UNUSED(Pos)

    if(Input.size() > 12)
        return QValidator::Invalid;

    QString Acceptable = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    for(int Pos = 0; Pos < Input.size(); Pos++)
    {
        if (Input[Pos] >= 'a' && Input[Pos] <= 'z')
            Input[Pos] = Input[Pos].toUpper();

        if(!Acceptable.contains(Input.at(Pos)))
            return QValidator::Invalid;
    }

    if(Input.size() < 11)
        return QValidator::Intermediate;

    return QValidator::Acceptable;
}

//---------------------------------------------------------------------------
OtherValidator::OtherValidator(QObject* Parent) : QValidator(Parent)
{
}

//---------------------------------------------------------------------------
QValidator::State OtherValidator::validate(QString& Input, int& Pos) const
{
    Q_UNUSED(Pos)

    if(Input.size() > 255)
        return QValidator::Invalid;

    return QValidator::Acceptable;
}

//---------------------------------------------------------------------------
RegistryDelegate::RegistryDelegate(QObject* Parent, Core* C) : QItemDelegate(Parent), C(C)
{
}

//---------------------------------------------------------------------------
QWidget* RegistryDelegate::createEditor(QWidget* Parent,
                                        const QStyleOptionViewItem& Option,
                                        const QModelIndex& Index) const
{
    Q_UNUSED(Option)
    Q_UNUSED(Index)

    QLineEdit *Editor = new QLineEdit(Parent);

    return Editor;
}

//---------------------------------------------------------------------------
void RegistryDelegate::updateEditorGeometry(QWidget* Editor,
                                            const QStyleOptionViewItem& Option,
                                            const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    Editor->setGeometry(Option.rect);
}

//---------------------------------------------------------------------------
void RegistryDelegate::setEditorData(QWidget* Editor, const QModelIndex& Index) const
{
    qobject_cast<QLineEdit*>(Editor)->setText(Index.data(Qt::EditRole).toString());
}

//---------------------------------------------------------------------------
void RegistryDelegate::setModelData(QWidget* Editor,
                                    QAbstractItemModel* Model,
                                    const QModelIndex& Index) const
{
    QLineEdit* LineEditor = qobject_cast<QLineEdit*>(Editor);

    QString OldValue = Model->data(Index, Qt::EditRole).toString();
    QString Value = LineEditor->text();

    if(Value != OldValue)
    {
        Model->setData(Index, Value);
        emit Value_Changed(Index.row());
    }
}

//---------------------------------------------------------------------------
ValueDelegate::ValueDelegate(QObject* Parent, Core* C) : QItemDelegate(Parent), C(C)
{
}

//---------------------------------------------------------------------------
QWidget* ValueDelegate::createEditor(QWidget* Parent,
                                    const QStyleOptionViewItem& Option,
                                    const QModelIndex& Index) const
{
    Q_UNUSED(Option)

    QLineEdit *Editor = new QLineEdit(Parent);

    if(Index.sibling(Index.row(), REGISTRY_COLUMN).data(Qt::EditRole).toString() == "ad-id.org")
        Editor->setValidator(new AdIdValidator);
    else
        Editor->setValidator(new OtherValidator);

    return Editor;
}

//---------------------------------------------------------------------------
void ValueDelegate::updateEditorGeometry(QWidget* Editor,
                                        const QStyleOptionViewItem& Option,
                                        const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    Editor->setGeometry(Option.rect);
}


//---------------------------------------------------------------------------
void ValueDelegate::setEditorData(QWidget *Editor, const QModelIndex& Index) const
{
    qobject_cast<QLineEdit*>(Editor)->setText(Index.data(Qt::EditRole).toString());
}

//---------------------------------------------------------------------------
void ValueDelegate::setModelData(QWidget* Editor,
                                QAbstractItemModel* Model,
                                const QModelIndex& Index) const
{
    QLineEdit* LineEditor = qobject_cast<QLineEdit*>(Editor);

    QString OldValue = Model->data(Index, Qt::EditRole).toString();
    QString Value = LineEditor->text();

    if(Value != OldValue)
    {
        Model->setData(Index, Value);
        emit Value_Changed(Index.row());
    }
}

//***************************************************************************
// TableWidget
//***************************************************************************

//---------------------------------------------------------------------------
TableWidget::TableWidget(QWidget* Parent) : QTableWidget(Parent)
{
}

//---------------------------------------------------------------------------
void TableWidget::Setup(Core *C)
{
    this->C = C;

    // Setup table widget
    QStringList Header_Labels;

    Header_Labels.append("File Name");
    Header_Labels.append("Registry");
    Header_Labels.append("Value");
    setColumnCount(MAX_COLUMN);
    setHorizontalHeaderLabels(Header_Labels);
#if QT_VERSION < 0x050000
    horizontalHeader()->setResizeMode(QHeaderView::Interactive);
#else
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
#endif

    ValueDelegate* ValueEditor = new ValueDelegate(NULL, C);
    connect(ValueEditor, SIGNAL(Value_Changed(int)), this, SLOT(On_Value_Changed(int)));

    RegistryDelegate* RegistryEditor = new RegistryDelegate(NULL, C);
    connect(RegistryEditor, SIGNAL(Value_Changed(int)), this, SLOT(On_Value_Changed(int)));

    setItemDelegateForColumn(2, qobject_cast<QAbstractItemDelegate*>(RegistryEditor));
    setItemDelegateForColumn(3, qobject_cast<QAbstractItemDelegate*>(ValueEditor));
}

//---------------------------------------------------------------------------
void TableWidget::Set_Modified(int Row, bool Modified)
{
    if(Row > this->rowCount())
        return;

    for(int Col = 0; Col < this->columnCount(); Col++)
    {
        QFont Font = this->item(Row, Col)->font();
        Font.setBold(Modified);
        this->item(Row, Col)->setFont(Font);
    }

    item(rowCount() - 1, REGISTRY_COLUMN)->setBackgroundColor(QColor(173, 216, 230, 127));
    item(rowCount() - 1, VALUE_COLUMN)->setBackgroundColor(QColor(173, 216, 230, 127));
}

//---------------------------------------------------------------------------
void TableWidget::Set_Valid(int Row, bool Valid)
{
    if (Valid || Row > this->rowCount())
        return;

    for (int Col = 1; Col < this->columnCount(); Col++)
    {
        Qt::ItemFlags Flags = this->item(Row, Col)->flags();
        if(Flags.testFlag(Qt::ItemIsEnabled))
            Flags &= ~Qt::ItemIsEnabled;
        if(Flags.testFlag(Qt::ItemIsSelectable))
            Flags &= ~Qt::ItemIsSelectable;
        this->item(Row, Col)->setFlags(Flags);
    }

    if (!Valid)
    {
        item(rowCount() - 1, REGISTRY_COLUMN)->setBackgroundColor(QColor(230, 173, 173, 127));
        item(rowCount() - 1, VALUE_COLUMN)->setBackgroundColor(QColor(230, 173, 173, 127));
    }
}

//---------------------------------------------------------------------------
void TableWidget::Update_Table()
{
    size_t Valid = 0;
    bool Modified = false;
    
    //Get opened files
    FileList* Files = C->Get_Files();

    //Get displayed entries
    QStringList Entries;
    for(int Row = rowCount() - 1; Row >= 0; Row--)
    {
        QString Entry = item(Row, FILE_COLUMN)->data(Qt::DisplayRole).toString();

        if(Files->find(Entry) == Files->end())
        {
            //Remove deleted entry
            removeRow(Row);
            continue;
        }

        Entries.append(Entry);
        //Display modified entries in bold
        Set_Valid(Row, Files->value(Entry).Valid);
        Set_Modified(Row, Files->value(Entry).Modified);

        if(Files->value(Entry).Modified)
            Modified = true;

        if (Files->value(Entry).Valid)
            Valid++;
    }

    //Display new files
    for(FileList::iterator It = Files->begin(); It != Files->end(); It++)
    {
        if(!Entries.contains(It.key()))
        {
            QTableWidgetItem* Name = new QTableWidgetItem(It.key());
            Name->setFlags(Name->flags() ^ Qt::ItemIsEditable);
            QTableWidgetItem* Registry = new QTableWidgetItem(It->Valid ? It->MetaData.first  :QString::fromUtf8("(Parsing error)"));
            QTableWidgetItem* Value = new QTableWidgetItem(It->Valid ? It->MetaData.second : QString::fromStdString(It->H->PerFile_Error.str()));
            if (It->Valid)
            {
                Registry = new QTableWidgetItem(It->Valid ? It->MetaData.first : QString::fromUtf8("(Parsing error)"));
                Registry->setToolTip("Double-click for editing the Universal Ad-ID registry of this file.");
                Value = new QTableWidgetItem(It->Valid ? It->MetaData.second : QString::fromStdString(It->H->PerFile_Error.str()));
                Value->setToolTip("Double-click for editing the Universal Ad-ID value of this file.\nA-Z 0-9 only.");
            }
            else
            {
                Registry = new QTableWidgetItem("(Parsing error)");
                Value = new QTableWidgetItem(It->H->PerFile_Error.str().c_str());
            }

            insertRow(rowCount());
            setItem(rowCount() - 1, FILE_COLUMN, Name);
            setItem(rowCount() - 1, REGISTRY_COLUMN, Registry);
            setItem(rowCount() - 1, VALUE_COLUMN, Value);

            if(It->Modified)
                Modified = true;

            if (It->Valid)
            {
                Set_Modified(rowCount() - 1, It->Modified);
                Valid++;
            }
            else
            {
                Set_Valid(rowCount() - 1, false);
            }
        }
    }

    if (Valid)
        setStatusTip(QString::number(Valid)+ " editable files found, double-click on Registry or Value cells for editing then save");
    else
        setStatusTip("Drag and drop some MOV/MP4 files");

    emit Enable_Save(Modified);
}

//---------------------------------------------------------------------------
void TableWidget::resizeEvent(QResizeEvent* Event)
{
    //Do nothing if columns size exceed aviable space
    if(!horizontalScrollBar()->isVisible())
    {
        qreal Total_New = Event->size().width();
        setColumnWidth(0, Total_New * ColumnSize[0] / 100);
        setColumnWidth(1, Total_New * ColumnSize[1] / 100);
        setColumnWidth(2, Total_New * ColumnSize[2] / 100);
        setColumnWidth(3, Total_New * ColumnSize[3] / 100);
    }
    //Call base resizeEvent to handle the vertical resizing
    QTableView::resizeEvent(Event);
}

//---------------------------------------------------------------------------
void TableWidget::On_Value_Changed(int Row)
{
    if(!C)
        return;

    QString FileName = item(Row, FILE_COLUMN)->data(Qt::EditRole).toString();
    QString Registry = item(Row, REGISTRY_COLUMN)->data(Qt::EditRole).toString();
    QString Value = item(Row, VALUE_COLUMN)->data(Qt::EditRole).toString();

    MetaDataType* MetaData = C->Get_MetaData(FileName);

        if(!MetaData)
            return;

    MetaData->first = Registry;
    MetaData->second = Value;

    if(!Registry.isEmpty() && (Registry != "ad-id.org" || !Value.isEmpty()) )
    {
        (*C->Get_Files())[FileName].Modified = true;
        Update_Table();
    }
    else
    {
        (*C->Get_Files())[FileName].Modified = false;
        Update_Table();
    }
}
