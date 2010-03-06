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

FMListView::FMListView( QWidget *parent) :
    QTableView(parent)
{    
    setDragEnabled ( true );
    setDragDropMode( QAbstractItemView::DragDrop );
    setAcceptDrops( true );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    dirModel = new FMFileSystemModel();
    dirModel->setRootPath(QDir::homePath());
    dirModel->setFilter( QDir::AllEntries | QDir::Hidden );
    dirModel->setResolveSymlinks(true);
    setModel(dirModel);
    setRootIndex(dirModel->index(QDir::homePath()));
    setShowGrid(false);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    verticalHeader()->setDefaultSectionSize( 18 );
    horizontalHeader()->setDefaultSectionSize( width() );
    hideRow(0);//hide the .
    hideColumn(1);
    hideColumn(2);
    hideColumn(3);
}

void FMListView::keyPressEvent( QKeyEvent * event )
{
    QTableView::keyPressEvent( event );
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
    qDebug()<<"root path set";
    dirModel->setRootPath( path );
    setModel( dirModel );
    setRootIndex(dirModel->index(path));
    this->hideRow(0);
    this->selectRow(1);
    //selectionModel()->setCurrentIndex(model()->index( 0, 1, QModelIndex() ), QItemSelectionModel::Select);
    //selectionModel()->select( model()->index( 0, 1, QModelIndex() ), QItemSelectionModel::Select );
    scrollToTop();
    repaint();
}
