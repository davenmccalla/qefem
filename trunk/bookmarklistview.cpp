#include "bookmarklistview.h"
#include <QStandardItemModel>
#include <QDir>
#include <QTimer>

bookmarkListView::bookmarkListView(QWidget *parent) :
    QListView(parent)
{
//for testing purposes
#ifdef Q_WS_MAC
    QFileInfoList list = QDir("/Volumes/").entryInfoList( QDir::Dirs | QDir::NoDotAndDotDot );
#endif
#ifdef Q_WS_WIN
    QFileInfoList list = QDir::drives();
#endif
    QStandardItemModel* model = new QStandardItemModel();
    model->setColumnCount(1);
    model->setRowCount(list.count());
    for( int i=0; i<list.count(); i++ )
    {
#ifdef Q_WS_MAC
        QStandardItem *item = new QStandardItem(list[i].filePath());
        item->setWhatsThis( list[i].filePath () );
        item->setToolTip( list[i].fileName() );
        model->setItem(i, 0, item);
#endif
#ifdef Q_WS_WIN
        QStandardItem *item = new QStandardItem(list[i].absolutePath());
        model->setItem(i, 0, item);
#endif
    }
    setModel( model );
}
