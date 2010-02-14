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

#include "controlpanel.h"
#include "mainwindow.h"
#include "copytask.h"
#include "deletetask.h"
#include <QDir>
#include <QProcess>
#include <QDebug>
#include <QThreadPool>
#include <QToolTip>
#include <QApplication>
#include <QMessageBox>

ControlPanel::ControlPanel( MainWindow* aMainW, FMPanel* aLeftPanel, FMPanel* aRightPanel )
{
    mainW = aMainW;
    leftPanel = aLeftPanel;
    rightPanel = aRightPanel;
    commandLayout = new QHBoxLayout();
    commandLayout->setContentsMargins( 2, 2, 2, 2 );
    commandLayout->setSpacing(1);
    cpButton = new QPushButton( "&Copy" );
    rnButton = new QPushButton( "&Rename" );
    delButton = new QPushButton( "&Del" );
    zipButton = new QPushButton( "&Zip" );
    mkdirButton = new QPushButton( "&Mkdir" );
    statusButton = new QPushButton( "&Status" );
    animation = new QLabel();
    movie = new QMovie(":/images/spiral.gif");
    animation->setMinimumSize( 22, 22);
    animation->setMaximumSize( 22, 22);
    animation->setMovie(movie);
    movie->setScaledSize( QSize(22,22) );
    movie->start();
    commandLayout->addWidget( animation );
    commandLayout->addWidget( cpButton );
    commandLayout->addWidget( rnButton );
    commandLayout->addWidget( delButton );
    commandLayout->addWidget( mkdirButton );
    commandLayout->addWidget( zipButton );
    commandLayout->addWidget( statusButton );
    //add events
    connect(cpButton, SIGNAL(clicked( bool )), this, SLOT( cpButtonClicked( bool ) ));
    connect(rnButton, SIGNAL(clicked( bool )), this, SLOT( rnButtonClicked( bool ) ));
    connect(delButton, SIGNAL(clicked( bool )), this, SLOT( delButtonClicked( bool ) ));
    connect(zipButton, SIGNAL(clicked( bool )), this, SLOT( zipButtonClicked( bool ) ));
    connect(mkdirButton, SIGNAL(clicked( bool )), this, SLOT( mkdirButtonClicked( bool ) ));
    connect(statusButton, SIGNAL(clicked( bool )), this, SLOT( statusButtonClicked( bool ) ));
    setLayout( commandLayout );
    setVisible( true );
    statusOn = false;
    movie->stop();
}

ControlPanel::~ControlPanel()
{
}

void ControlPanel::cpButtonClicked( bool /*checked*/ )
{
    movie->start();
    if( leftPanel->lastFocus() > rightPanel->lastFocus() )
    {
        QStringList list = leftPanel->selectedFiles();
        qDebug()<<list;
        if( list.count() > 0 )
            copyFiles( list, rightPanel->curDir(), true );
    }
    else
    {
        QStringList list = rightPanel->selectedFiles();
        qDebug()<<list;
        if( list.count() > 0 )
            copyFiles( list, leftPanel->curDir(), false );
    }

}


void ControlPanel::rnButtonClicked( bool /*checked*/ )
{
    movie->start();
    if( leftPanel->lastFocus() > rightPanel->lastFocus() )
    {
        leftPanel->setEditMode( FMPanel::Rename );
    }
    else
    {
        rightPanel->setEditMode( FMPanel::Rename );
    }
}

void ControlPanel::delButtonClicked( bool /*checked*/ )
{
    movie->start();
    if( leftPanel->lastFocus() > rightPanel->lastFocus() )
    {
        QStringList list = leftPanel->selectedFiles();
        qDebug()<<"files to delete : "<<list;
        if( list.count() > 0 )
            deleteFiles( list, true );
    }
    else
    {
        QStringList list = rightPanel->selectedFiles();
        qDebug()<<"files to delete : "<<list;
        if( list.count() > 0 )
            deleteFiles( list, false );
    }
    movie->stop();
}

void ControlPanel::zipButtonClicked( bool /*checked*/ )
{
    movie->start();
    if( leftPanel->lastFocus() > rightPanel->lastFocus() )
    {
        leftPanel->setZipOutputDir( rightPanel->getCurrentDir() );
        leftPanel->setEditMode( FMPanel::Zip );
    }
    else
    {
        rightPanel->setZipOutputDir( leftPanel->getCurrentDir() );
        rightPanel->setEditMode( FMPanel::Zip );
    }

}

void ControlPanel::mkdirButtonClicked( bool /*checked*/ )
{
    movie->start();
    if( leftPanel->lastFocus() > rightPanel->lastFocus() )
    {
        leftPanel->setEditMode( FMPanel::MkDir );
    }
    else
    {
        rightPanel->setEditMode( FMPanel::MkDir );
    }
}

void ControlPanel::statusButtonClicked( bool /*checked*/ )
{
    if( statusOn )
    {
        mainW->closeStatus();
        statusOn = false;
    }
    else
    {
        mainW->showStatus();
        statusOn = true;
    }
}

void ControlPanel::copyDirs(const QString& dirName, const QString& dest, bool left )
{
    CopyTask *ct = new CopyTask( dirName, dest, CopyTask::Dir );
    connect( ct, SIGNAL(finished()), this, SLOT( copyTaskFinished() ));
    copyVector.append( QPair<CopyTask *,QPair<QString, bool> >( ct, QPair<QString, bool>( dirName, left)) );
    ct->start();
    mainW->updateStatus();
}

void ControlPanel::copyFiles( const QStringList& files, const QString& dest, bool left )
{
    if( files.count() == 0 )
        return;
    CopyTask *ct = new CopyTask( files, dest );
    connect( ct, SIGNAL(finished()), this, SLOT( copyTaskFinished() ));
    copyVector.append( QPair<CopyTask *,QPair<QString, bool> >( ct, QPair<QString, bool>( files[0], left)) );
    ct->start();
    mainW->updateStatus();
}

void ControlPanel::deleteFiles( const QStringList& files, bool left )
{
    if( files.count() == 0 )
        return;
    QMessageBox msgBox;
    QString names = files.join(",");
    msgBox.setText( names );
    msgBox.setInformativeText("Do you want to delete?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    if( ret == QMessageBox::No )
    {
        movie->stop();
        return;
    }

    DeleteTask *dt;
    dt = new DeleteTask( files );
    connect( dt, SIGNAL(finished()), this, SLOT( delTaskFinished() ));
    delVector.append( QPair<DeleteTask *,QPair<QString, bool> >( dt, QPair<QString, bool>( files[0], left)) );
    dt->start();
    mainW->updateStatus();
}

void ControlPanel::copyFile( const QString& file, const QString& dest, bool left )
{
    CopyTask *ct = new CopyTask( file, dest, CopyTask::File );
    connect( ct, SIGNAL(finished()), this, SLOT( copyTaskFinished() ));
    copyVector.append( QPair<CopyTask *,QPair<QString, bool> >( ct, QPair<QString, bool>( file, left)) );
    ct->start();
    mainW->updateStatus();
}

void ControlPanel::copyTaskFinished()
{
    for(int i=0; i < copyVector.size(); i++ )
    {
        if( copyVector[i].first->isFinished() )
        {
            QString text("Copy of ");
            text.append( copyVector[i].second.first );
            text.append(" finished.");
            //QToolTip::showText( QApplication::activeWindow()->pos(), text);
            delete copyVector[i].first;
            if( copyVector[i].second.second )
            {
                leftPanel->reload();
            }
            else
            {
                rightPanel->reload();
            }
            copyVector.remove(i);
            i--;
        }
    }
    mainW->updateStatus();
}

void ControlPanel::delTaskFinished()
{
    for(int i=0; i < delVector.size(); i++ )
    {
        if( delVector[i].first->isFinished() )
        {
            QString text("Deletion of ");
            text.append( delVector[i].second.first );
            text.append(" finished.");
            //QToolTip::showText( QApplication::activeWindow()->pos(), text);
            delete delVector[i].first;
            if( delVector[i].second.second )
            {
                leftPanel->reload();
            }
            else
            {
                rightPanel->reload();
            }
            delVector.remove(i);
            i--;
        }
    }
    mainW->updateStatus();
}

QStringList ControlPanel::getStatus()
{
    QStringList list;
    for(int i=0; i < copyVector.size(); i++ )
    {
        if( copyVector[i].first->isRunning() )
        {
            QString text("Copy of ");
            text.append( copyVector[i].second.first );
            text.append(" is running.");
            list << text;
        }
    }
    for(int i=0; i < delVector.size(); i++ )
    {
        if( delVector[i].first->isRunning() )
        {
            QString text("Deletion of ");
            text.append( delVector[i].second.first );
            text.append(" is running.");
            list << text;
        }
    }
    return list;
}

void ControlPanel::disableButtons()
{
    cpButton->setDisabled( true );
    rnButton->setDisabled( true );
    delButton->setDisabled( true );
    zipButton->setDisabled( true );
    mkdirButton->setDisabled( true );
}

void ControlPanel::enableButtons()
{
    cpButton->setEnabled( true );
    rnButton->setEnabled( true );
    delButton->setEnabled( true );
    zipButton->setEnabled( true );
    mkdirButton->setEnabled( true );
}

void ControlPanel::stopAnimation()
{
    movie->stop();
}

void ControlPanel::startAnimation()
{
    movie->start();
}
