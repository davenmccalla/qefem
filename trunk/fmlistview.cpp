/*
The QeFeM project.
Copyright (C) 2010  Karim Pinter

This code is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "fmlistview.h"
#include <QDebug>
#include <QShortcut>
#include <QUrl>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QDir>
#ifdef Q_WS_WIN
#include <windows.h>
#else
#include <QProcess>
#endif
#ifdef Q_WS_MAC
#include <QTimer>
#endif

FMListView::FMListView( QWidget *parent) :
    QListView(parent)
{    
    freeSpace.clear();
#if !defined(Q_WS_MAEMO_5) && !defined(HB_Q_WS_MAEMO)&& !defined(Q_WS_HILDON)
    setDragEnabled ( true );
    setDragDropMode( QAbstractItemView::DragDrop );
    setAcceptDrops( true );
#endif    
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    
    setEditTriggers( QAbstractItemView::NoEditTriggers );
    QString home( QDir::homePath() );
    
    setRootPath( home );
    
    connect( &watcher, SIGNAL(directoryChanged( const QString& )), this, SLOT(setRootPath(const QString&)) );
    selectionModel()->clear();
}

FMListView::~FMListView()
{
}

void FMListView::keyPressEvent( QKeyEvent * event )
{
    QListView::keyPressEvent( event );
    if(( event->key() == Qt::Key_Up )||( event->key() == Qt::Key_Down )
        ||( event->key() == Qt::Key_Home ) || ( event->key() == Qt::Key_End )
        ||( event->key() == Qt::Key_PageDown )||( event->key() == Qt::Key_PageUp ))
    {
        //qDebug()<<"key up or down";
        emit keyUpOrDownPressed();
    }
}

void FMListView::dropEvent( QDropEvent* e )
{
    qDebug()<<"dropEvent";
    qDebug()<< e->mimeData()->urls()[0].toString();
    QStringList files;
    int i;
    for( i=0; i < e->mimeData()->urls().count(); i++ )
    {
        QString file( e->mimeData()->urls()[i].toString() );
        if( file.contains("file:///", Qt::CaseInsensitive ))
        {
            #ifdef Q_WS_WIN
            file.remove( 0, 8 );
            #else
            file.remove( 0, 7 );
            #endif
            files.append( file );
            qDebug()<<file;
        }
    }
    if( i > 0 )
    {
        emit copyFiles( files );
    }
    e->acceptProposedAction();
}


void FMListView::dragEnterEvent( QDragEnterEvent * event )
{
    if( event->mimeData()->hasUrls() &&
      (( event->proposedAction() == Qt::CopyAction) || ( event->proposedAction() == Qt::MoveAction)))
    {
        event->acceptProposedAction();
    }
}

void FMListView::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}


QStringList FMListView::selectedFiles()
{
    QModelIndex index;
    QStringList slist;
    if( this->hasFocus() )
    {
        QModelIndexList list = selectedIndexes();
        foreach( index, list)
        {
            slist.append( index.data().toString() );
        }
    }
    else
    {
        foreach( index, selectionList)
        {
            slist.append( index.data().toString() );
        }
    }
    return slist;
}

void FMListView::setRootPath( const QString& path )
{    
    qDebug()<<"root path set "<<path;
    bool changed = true;
    selectionList.clear();

    if( path == rootDir )
    {
        changed = false;
    }
    else
    {
        watcher.removePath( rootDir );
        rootDir.clear();
        rootDir.append( path );
        watcher.addPath( rootDir );
    }
    QDir dir( path );    
    QStringList dirs = dir.entryList( QDir::AllEntries | QDir::NoDotAndDotDot,QDir::DirsFirst | QDir::Type);
    QStandardItemModel* mod = qobject_cast<QStandardItemModel*>( model() );
    if( mod == NULL )
    {
        mod = new QStandardItemModel();
        setModel( mod );
    }
    mod->clear();
    getFreeSpace( path );
    QString firstLine("..");
    firstLine.append("\t");
    firstLine.append(freeSpace);
    QStandardItem *itemUp = new QStandardItem( style()->standardIcon( QStyle::SP_ArrowUp ), firstLine );
    mod->appendRow( itemUp );
    for( int i = 0; i < dirs.count(); i++ )
    {
        QString full( path );
        full.append("/");
        full.append( dirs.at(i) );
        QFileInfo inf( full );
        QStandardItem *item = NULL;
        if( inf.isDir() || inf.isBundle() )
        {
            item = new QStandardItem( style()->standardIcon( QStyle::SP_DirIcon ), dirs.at(i) );

        }
        else //if( inf.isFile() )
        {
            item = new QStandardItem( style()->standardIcon( QStyle::SP_FileIcon ), dirs.at(i) );

        }
        if( item != NULL )
            mod->appendRow( item );
    }
    if( changed )
    {        
        curIndex = model()->index(0,0);
        selectionModel()->setCurrentIndex( curIndex, QItemSelectionModel::Select );
        emit rootPathChanged( rootDir );
    }
    #ifdef Q_WS_MAC
    QTimer::singleShot(100, this, SLOT(lateUpdate));
    #endif
}

QString FMListView::getRootDir()
{
    return rootDir;
}

#ifdef Q_WS_WIN
void FMListView::getFreeSpace(const QString& path)
{
    quint64 availUser, total, avail;
    QFileInfo fi(path);
    QString dir = QDir::toNativeSeparators(fi.absoluteDir().canonicalPath());
    freeSpace.clear();
    if(GetDiskFreeSpaceExW((LPCWSTR)dir.utf16(),
        (PULARGE_INTEGER)&availUser,
        (PULARGE_INTEGER)&total,
        (PULARGE_INTEGER)&avail) != 0)
    {
        availUser = availUser / 1048576;
        total = total / 1048576;
        avail = avail / 1048576;
        qDebug()<<"availUser "<<availUser<<"total "<<total<<"avail "<<avail;
        freeSpace.append(QString::number(availUser) );
        freeSpace.append(" mb/ ");
        freeSpace.append(QString::number(total) );
        freeSpace.append(" mb free");
    }
}

#elif defined(Q_WS_MAC)
void FMListView::getFreeSpace(const QString& path)
{

    QProcess df;
         df.start("df", QStringList() << "-g"<<"-t"<<path);
         if (!df.waitForStarted())
             return;

         if (!df.waitForFinished())
             return;

         df.readLine();
         QString res( df.readLine() );
         QStringList data = res.split(' ', QString::SkipEmptyParts);
         qDebug()<<"df output :"<<data;
         freeSpace.clear();
         freeSpace.append( data[3] );
         freeSpace.append(" gb/ ");
         freeSpace.append( data[2] );
         freeSpace.append(" gb free ");
         freeSpace.append( data[4] );
}
#elif defined(Q_OS_LINUX)
void FMListView::getFreeSpace(const QString& path)
{

    QProcess df;
         df.start("df", QStringList() <<"-h"<<path);
         if (!df.waitForStarted())
             return;

         if (!df.waitForFinished())
             return;

         df.readLine();
         QString res( df.readLine() );
         QStringList data = res.split(' ', QString::SkipEmptyParts);
         qDebug()<<"df output :"<<data;
         freeSpace.clear();
         freeSpace.append( data[3] );
         freeSpace.append(" gb/ ");
         freeSpace.append( data[2] );
         freeSpace.append(" gb free ");
         freeSpace.append( data[4] );
}
#endif

void FMListView::focusInEvent( QFocusEvent * event )
{    
    if( selectionModel() == NULL )
        return;
    if( !selectionList.isEmpty() )
    {
        foreach (QModelIndex index, selectionList){
            selectionModel()->select( index, QItemSelectionModel::Select );
        }
    }
    else
    {
        curIndex = model()->index(0,0);
    }
    selectionModel()->setCurrentIndex( curIndex, QItemSelectionModel::Select );
}

void FMListView::focusOutEvent( QFocusEvent * event )
{
    selectionList = selectedIndexes();
    curIndex = selectionModel()->currentIndex();
    selectionModel()->clear();
}

#ifdef Q_WS_MAC
void FMListView::lateUpdate()
{
    setRootPath( rootDir );
}
#endif
