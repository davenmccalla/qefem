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

#include "deletetask.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QApplication>
#include <QToolTip>

DeleteTask::DeleteTask( const QString& delPath ): path( delPath )
{
    pathList.clear();
}

DeleteTask::DeleteTask( const QStringList& delPath ): pathList( delPath )
{
    path.clear();
}

DeleteTask::~DeleteTask( )
{
}

void DeleteTask::run()
{    
    if( !path.isEmpty() )
    {
        del( path );
    }
    if( !pathList.isEmpty() )
    {
        QString name;
        foreach( name, pathList)
        {
            del( name );
        }
    }
}

void DeleteTask::del(const QString& name )
{
    QFileInfo fileInfo( name );
    if( fileInfo.isFile() )
    {
        QFile file( name );
        file.remove();
        qDebug()<<"File to remove : "<<name;
    }
    else if( fileInfo.isDir() )
    {
        delDir( name );
    }
}

void DeleteTask::delDir(const QString& dirName )
{
    qDebug()<<"Del dir: "<<dirName;
    QDir dir( dirName );
    QStringList dirs = dir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot );
    QString newDir;
    for( int i = 0; i < dirs.count(); i++ )
    {
        newDir.clear();
        newDir.append( dirName );
        newDir.append("/");
        newDir.append( dirs[i] );
        delDir( newDir );
    }
    QStringList files = dir.entryList( QDir::Files );
    QString rmpath;
    for( int i = 0; i<files.count(); i++ )
    {
        rmpath.clear();
        rmpath.append(dirName);
        rmpath.append("/");
        rmpath.append( files[i] );
        QFile::remove( rmpath );
        qDebug()<<"File to remove : "<<rmpath;
    }
    dir.rmdir(dirName);
}
