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

#include "copytask.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QToolTip>
#include <QMessageBox>

CopyTask::CopyTask(const QString& dirName, const QString& dest, CopyMode mode ) : copyMode( mode )
{
    if( copyMode == Dir )
        srcDir.append( dirName );
    else
        srcFile.append( dirName );
    destination.append( dest );
    yesToAll = false;
}

CopyTask::CopyTask(const QStringList& files, const QString& dest)
{
    copyMode = List;
    srcList = files;
    destination.append( dest );
    yesToAll = false;
}

CopyTask::~CopyTask( )
{
}

void CopyTask::run()
{
    switch( copyMode )
    {
    case File :
        {
            copyFile( srcFile, destination);
            break;
        }
    case List :
        {
            copyFiles( srcList, destination );
            break;
        }
    case Dir :
        {
            copyDirs( srcDir, destination );
            break;
        }
    }
}

void CopyTask::copyDirs( const QString& dirName, const QString& dest )
{
    qDebug()<<"Copy dir from: "<<dirName<<" dest: "<<dest;
    QDir dir( dirName );
    if( !QDir( dest ).mkdir( dir.dirName() ) )
        qDebug()<<"Creating dir"<<dest<<" "<<dir.dirName()<<" failed.";
    QString ndest;
    ndest.append( dest );
    ndest.append( dir.dirName() );
    QStringList dirs = dir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot );
    QString newDest(ndest);
    newDest.append("/");
    QString newFrom(dirName);
    for( int i = 0; i < dirs.count(); i++ )
    {
        //newDest.append( dirs[i] );
        //newDest.append("/");
        //if( !QDir( dest ).mkdir( dirs[i] ) )
        //    qDebug()<<"Creating dir"<<dirs[i]<<" failed.";
        newFrom.clear();
        newFrom.append(dirName);
        newFrom.append("/");
        newFrom.append( dirs[i] );
        copyDirs( newFrom, newDest );
    }
    QStringList files = dir.entryList( QDir::Files );
    copyFiles( dirName, files, ndest );
}

void CopyTask::copyFiles( const QStringList& files, const QString& dest )
{
    for( int i = 0; i < files.count(); i++ )
    {
        QFileInfo info( files[i] );
        QFileInfo file( files[i] );
        QString fullDest( dest );
        if((files[i][files[i].size()-1]!='\\')||
           (files[i][files[i].size()-1]!='/'))
        {
            fullDest.append("/");
        }
        if( info.isDir() )
        {
            copyDirs( files[i], fullDest );
        }
        else if( info.isFile() )
        {
            fullDest.append( file.fileName() );
            copyFile( files[i], fullDest );
        }
    }
}

void CopyTask::copyFiles( const QString homeDir, const QStringList& files, const QString& dest )
{
    for( int i = 0; i < files.count(); i++ )
    {
        qDebug()<<"homedir "<<homeDir<<" files "<<files[i]<<" dest "<<dest;
        QString fname;
        fname.append( homeDir );
        fname.append("/");
        fname.append( files[i] );
        QString dname;
        dname.append( dest );
        dname.append("/");
        dname.append( files[i] );
        copyFile( fname, dname );
    }
}

void CopyTask::copyFile( const QString& file, const QString& dest )
{
    if( file == dest )
        return;
    qDebug()<<"Copy file from: "<<file<<" dest: "<<dest;
//This causes many crashes should be moved to the main thread
    QFileInfo inf( dest );
    if( inf.exists() )
    {
        if( yesToAll )
        {
            QFile::remove( dest );
        }
        else{
            QMessageBox msgBox;
            msgBox.setText( dest );
            msgBox.setInformativeText("Do you want to overwrite it?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No );
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();
            wait();
            if( ret == QMessageBox::No )
            {
                return;
            }
            else if( ret == QMessageBox::Yes )
            {
                QFile::remove( dest );
            }
            else if( ret == QMessageBox::YesToAll )
            {
                QFile::remove( dest );
                yesToAll = true;
            }
        }
    }
//<- till here
    QFile fileToCopy( file );
    if( !fileToCopy.copy( dest ) )
    {
        qDebug()<<"File copy failed.";
    }
}

