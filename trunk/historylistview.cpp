#include "historylistview.h"
#include <QStandardItemModel>
#include <QDir>
#include <QTimer>

historyListView::historyListView( bool left, QWidget *parent) :
    QListView(parent), mleft(left)
{
    QString fileName(QDir::homePath());
    if( mleft )
    {
        fileName.append("/.Qefem/.lhistory");
    }
    else
    {
        fileName.append("/.Qefem/.rhistory");
    }
    QFile file(fileName);
    QStandardItemModel* model = new QStandardItemModel();
    model->setColumnCount(1);
    if( !file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        this->setToolTip("No history yet.");
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

historyListView::~historyListView()
{
    QStandardItemModel* mod = qobject_cast<QStandardItemModel*>( model() );
    if( mod == NULL )
    {
        return;
    }
    this->setToolTip("");
    QString fileName(QDir::homePath());
    if( mleft )
    {
        fileName.append("/.Qefem/.lhistory");
    }
    else
    {
        fileName.append("/.Qefem/.rhistory");
    }
    QFile file(fileName);
    if( file.open(QIODevice::WriteOnly | QIODevice::Text) )
    {
        int count = mod->rowCount();
        for( int i=0; i < count; i++ )
        {
            QStandardItem *item = mod->item( i, 0);
            file.write(qPrintable(item->data(Qt::DisplayRole).toString()));
            file.write("\n");
        }
        file.flush();
        file.close();
    }
}

void historyListView::addHistoryItem(const QString &path)
{
#ifdef Q_WS_WIN
    QString temp( path );
    temp.replace( QString("/"), QString("\\"));
#endif
    QStandardItemModel* mod = qobject_cast<QStandardItemModel*>( model() );
    if( mod != NULL )
    {
#ifdef Q_WS_WIN
        QStandardItem *item = new QStandardItem( style()->standardIcon( QStyle::SP_DirIcon ), temp );
#else
        QStandardItem *item = new QStandardItem( style()->standardIcon( QStyle::SP_DirIcon ), path );
#endif
        mod->insertRow( 0, item);
        if( mod->rowCount() > 128 )
        {
            mod->removeRow( 128 );
        }
    }
}
