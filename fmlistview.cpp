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
#include "fmfilesystemmodel.h"
#include <QDebug>
#include <QShortcut>
#include <QUrl>
#include <QHeaderView>
#include <QStandardItemModel>

FMListView::FMListView( QWidget *parent) :
    QListView(parent)
{    
    setDragEnabled ( true );
    setDragDropMode( QAbstractItemView::DragDrop );
    setAcceptDrops( true );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setEditTriggers( QAbstractItemView::NoEditTriggers );
    QString home( QDir::homePath() );
    setRootPath( home );
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
    QModelIndexList list = selectedIndexes();
    QModelIndex index;
    QStringList slist;
    foreach( index, list)
    {
        slist.append( index.data().toString() );
    }
    return slist;
}

void FMListView::setRootPath( QString& path )
{
    qDebug()<<"root path set "<<path;
    QDir dir( path );
    rootDir.clear();
    rootDir.append( path );
    QStringList dirs = dir.entryList( QDir::AllEntries | QDir::NoDotAndDotDot,QDir::DirsFirst | QDir::Type);
    QStandardItemModel* oldmod = qobject_cast<QStandardItemModel*>( model() );
    QStandardItemModel* mod = new QStandardItemModel();
    QStandardItem *itemUp = new QStandardItem( style()->standardIcon( QStyle::SP_ArrowUp ), ".." );
    mod->appendRow( itemUp );
    for( int i = 0; i < dirs.count(); i++ )
    {
        QString full( path );
        full.append("/");
        full.append( dirs[i] );
        QFileInfo inf( full );
        QStandardItem *item = NULL;
        if( inf.isDir() || inf.isBundle() )
        {
            item = new QStandardItem( style()->standardIcon( QStyle::SP_DirIcon ), dirs[i] );
        }
        else //if( inf.isFile() )
        {
            item = new QStandardItem( style()->standardIcon( QStyle::SP_FileIcon ), dirs[i] );
        }
        if( item != NULL )
            mod->appendRow( item );
    }
    this->setModel( mod );
    if( oldmod != NULL )
    {
        delete oldmod;
    }
}

QString FMListView::getRootDir()
{
    return rootDir;
}
