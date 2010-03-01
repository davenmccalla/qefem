#include "bookmarklistview.h"
#include <QStandardItemModel>
#include <QDir>
#include <QFile>
#include <QTimer>

bookmarkListView::bookmarkListView(QWidget *parent) :
    QListView(parent)
{
    QString fileName(QDir::homePath());
    fileName.append("/.Qefem/.bookmarks");
    QFile file(fileName);
    QStandardItemModel* model = new QStandardItemModel();
    if( !file.open(QIODevice::ReadWrite | QIODevice::Text) )
    {
        model->setColumnCount(1);
        model->setRowCount(1);
        QStandardItem *item = new QStandardItem("No bookmarks set.");
        model->setItem(0, 0, item);
    }
    else
    {
        model->setColumnCount(1);
        while (!file.atEnd())
        {
            QByteArray line = file.readLine();
            QStandardItem *item = new QStandardItem( QString(line) );
            model->appendRow( item );
        }
    }
    file.close();
    setModel( model );
}
