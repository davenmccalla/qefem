#include "bookmarklistview.h"
#include <QStandardItemModel>
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QDebug>

bookmarkListView::bookmarkListView(QWidget *parent) :
    QListView(parent)
{
    QString fileName(QDir::homePath());
    fileName.append("/.Qefem/.bookmarks");
    QFile file(fileName);
    QStandardItemModel* model = new QStandardItemModel();
    model->setColumnCount(2);
    if( !file.exists() )
    {
        this->setToolTip("No bookmark set.");
    }
    else if( !file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        this->setToolTip("Failed to open bookmarks file.");
    }
    else
    {
        while (!file.atEnd())
        {
            QByteArray line = file.readLine();
            line.chop(1);
            QStandardItem *item = new QStandardItem( style()->standardIcon( QStyle::SP_DirIcon ), QString(line) );
            model->appendRow( item );
        }
    }
    file.close();
    setModel( model );
}

void bookmarkListView::addBookmark( const QString& path )
{
    QStandardItemModel* mod = qobject_cast<QStandardItemModel*>( model() );
    if( mod != NULL )
    {
        QStandardItem *item = new QStandardItem( style()->standardIcon( QStyle::SP_DirIcon ), path );
        mod->appendRow( item );
    }
}
