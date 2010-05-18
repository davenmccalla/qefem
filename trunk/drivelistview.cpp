#include "drivelistview.h"
#include <QStandardItemModel>
#include <QDir>
#include <QTimer>

driveListView::driveListView(QWidget *parent) :
    QListView(parent)
{
    driveTimer = new QTimer(this);
    connect(driveTimer, SIGNAL(timeout()), this, SLOT(driveReload()));
    driveReload();
    driveTimer->start( 5000 );
    setSelectionMode( QAbstractItemView::NoSelection );
    setEditTriggers( QAbstractItemView::NoEditTriggers );
}

driveListView::~driveListView()
{
    driveTimer->stop();
    delete driveTimer;
}

void driveListView::driveReload()
{
#ifdef Q_WS_MAC
    QFileInfoList list = QDir("/Volumes/").entryInfoList( QDir::Dirs | QDir::NoDotAndDotDot );
#endif
#ifdef Q_WS_WIN
    QFileInfoList list = QDir::drives();
#endif
#if defined(Q_WS_MAEMO_5) || defined(HB_Q_WS_MAEMO)||defined(Q_WS_HILDON)
    QFileInfoList list = QDir::drives();
#endif
        
    if( list != drives )
    {
        drives.clear();
        QStandardItemModel* model = new QStandardItemModel();
        model->setColumnCount(1);
        model->setRowCount(list.count());
        for( int i=0; i<list.count(); i++ )
        {
            drives.append( list[i] );
    #ifdef Q_WS_MAC
            QStandardItem *item = new QStandardItem(style()->standardIcon( QStyle::SP_DriveHDIcon ), list[i].fileName());
            item->setWhatsThis( list[i].filePath () );
            item->setToolTip( list[i].fileName() );
            model->setItem(i, 0, item);
    #endif
    #ifdef Q_WS_WIN
            QStandardItem *item = new QStandardItem(style()->standardIcon( QStyle::SP_DriveHDIcon ),list[i].absolutePath());
            model->setItem(i, 0, item);
    #endif
        }
        QAbstractItemModel *m = this->model();
        setModel( model );
        delete m;
    }
}

