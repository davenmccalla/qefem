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
#if !defined(Q_WS_MAEMO_5) && !defined(HB_Q_WS_MAEMO) && !defined(Q_WS_HILDON)
    zipButton = new QPushButton( "&Zip" );
#endif
    mkdirButton = new QPushButton( "&Mkdir" );
#if defined(Q_WS_MAEMO_5) || defined(HB_Q_WS_MAEMO)||defined(Q_WS_HILDON)
    bookmarkButton = new QPushButton( "&Bmark" );
#else    
    bookmarkButton = new QPushButton( "&Bookmark" );
#endif    
    statusButton = new QPushButton( "&Status" );
#if !defined(Q_WS_MAEMO_5) && !defined(HB_Q_WS_MAEMO) && !defined(Q_WS_HILDON)
    //animation->setToolTip("When this animation is rotating its indicating that some file operation is going on.");
    cpButton->setToolTip("This copies the selected file or directory to the other panels current directory.");
    rnButton->setToolTip("This renames the selected file, directory or multiple file renaming doesn't work.");
    delButton->setToolTip("This deletes the selected file or directory, if it is not locked.");
    mkdirButton->setToolTip("This creates a directory in the active panel.");
    zipButton->setToolTip("This zips the selected files or directories to the other panel, if zip is installed on OSX or Linux and 7z on Windows.");
    bookmarkButton->setToolTip("This bookmarks the selected directory or the current directory. Those can be activeted through the bookmarks tab. The bookmarks are stored in home dir/.Qefem/.bookmarks text file.");
    statusButton->setToolTip("This opens the status view where all the ongoing operations are listed.");
    //animation->setWhatsThis("When this animation is rotating its indicating that some file operation is going on.");
    cpButton->setWhatsThis("This copies the selected file or directory to the other panels current directory.");
    rnButton->setWhatsThis("This renames the selected file, directory or multiple file renaming doesn't work.");
    delButton->setWhatsThis("This deletes the selected file or directory, if it is not locked.");
    mkdirButton->setWhatsThis("This creates a directory in the active panel.");
    zipButton->setWhatsThis("This zips the selected files or directories to the other panel, if zip is installed on OSX or Linux and 7z on Windows.");
    bookmarkButton->setWhatsThis("This bookmarks the selected directory or the current directory. Those can be activeted through the bookmarks tab. The bookmarks are stored in home dir/.Qefem/.bookmarks text file.");
    statusButton->setWhatsThis("This opens the status view where all the ongoing operations are listed.");
#endif
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
#if !defined(Q_WS_MAEMO_5) && !defined(HB_Q_WS_MAEMO) && !defined(Q_WS_HILDON)
    commandLayout->addWidget( zipButton );
#endif    
    commandLayout->addWidget( bookmarkButton );
    commandLayout->addWidget( statusButton );
#if defined(Q_WS_MAEMO_5) || defined(HB_Q_WS_MAEMO)||defined(Q_WS_HILDON)
    exitButton = new QPushButton( "E&xit" );
    commandLayout->addWidget( exitButton );
#endif    
    //add events
    connect(cpButton, SIGNAL(clicked( bool )), this, SLOT( cpButtonClicked( bool ) ));
    connect(rnButton, SIGNAL(clicked( bool )), this, SLOT( rnButtonClicked( bool ) ));
    connect(delButton, SIGNAL(clicked( bool )), this, SLOT( delButtonClicked( bool ) ));
#if !defined(Q_WS_MAEMO_5) && !defined(HB_Q_WS_MAEMO) && !defined(Q_WS_HILDON)
    connect(zipButton, SIGNAL(clicked( bool )), this, SLOT( zipButtonClicked( bool ) ));
#endif
    connect(mkdirButton, SIGNAL(clicked( bool )), this, SLOT( mkdirButtonClicked( bool ) ));
    connect(statusButton, SIGNAL(clicked( bool )), this, SLOT( statusButtonClicked( bool ) ));
    connect(bookmarkButton, SIGNAL(clicked( bool )), this, SLOT( bookmarkButtonClicked( bool ) ));
#if defined(Q_WS_MAEMO_5) || defined(HB_Q_WS_MAEMO)||defined(Q_WS_HILDON)
    connect(exitButton, SIGNAL(clicked( bool )), this, SLOT( exitButtonClicked( bool ) ));
#endif
    setLayout( commandLayout );
    setVisible( true );
    statusOn = false;
    movie->stop();
}

ControlPanel::~ControlPanel()
{
    delete commandLayout;
}

void ControlPanel::bookmarkButtonClicked( bool /*checked*/ )
{
    QString path;
    if( leftPanel->lastFocus() > rightPanel->lastFocus() )
    {
        if( leftPanel->curFile().isEmpty() || QFileInfo( leftPanel->curFile() ).isFile() )
        {
            path.append( leftPanel->curDir() );
            leftPanel->addBookmark( path);
            rightPanel->addBookmark( path );
        }
        else
        {
            path.append( leftPanel->curFile() );
            leftPanel->addBookmark( path );
            rightPanel->addBookmark( path );
        }
    }
    else
    {
        if( rightPanel->curFile().isEmpty() || QFileInfo( rightPanel->curFile() ).isFile() )
        {
            path.append( rightPanel->curDir() );
            leftPanel->addBookmark( rightPanel->curDir() );
            rightPanel->addBookmark( rightPanel->curDir() );
        }
        else
        {
            path.append( rightPanel->curFile() );
            leftPanel->addBookmark( path );
            rightPanel->addBookmark( path );
        }
    }    
    QString fileName(QDir::homePath());
    fileName.append("/.Qefem/.bookmarks");
    QFile file(fileName);
    if( file.open(QIODevice::Append | QIODevice::Text) )
    {
        file.write(qPrintable(path));
        file.write("\n");
        file.flush();
        file.close();
    }
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

void ControlPanel::exitButtonClicked( bool /*checked*/ )
{
    QCoreApplication::exit(0);
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
    qDebug()<<__FILE__<<__LINE__;
    for(int i=0; i < copyVector.size(); i++ )
    {
        if( copyVector[i].first->isFinished() )
        {
            QString text("Copy of ");
            text.append( copyVector[i].second.first );
            text.append(" finished.");
            //QToolTip::showText( QApplication::activeWindow()->pos(), text);
            delete copyVector[i].first;
            /*if( copyVector[i].second.second )
            {
                leftPanel->reload();
            }
            else
            {
                rightPanel->reload();
            }*/
            copyVector.remove(i);
            i--;
        }
    }
    mainW->updateStatus();
}

void ControlPanel::delTaskFinished()
{
    qDebug()<<__FILE__<<__LINE__;
    for(int i=0; i < delVector.size(); i++ )
    {
        if( delVector[i].first->isFinished() )
        {
            QString text("Deletion of ");
            text.append( delVector[i].second.first );
            text.append(" finished.");
            qDebug()<<text;
            //QToolTip::showText( QApplication::activeWindow()->pos(), text);
            delete delVector[i].first;
            /*if( delVector[i].second.second )
            {
                leftPanel->reload();
            }
            else
            {
                rightPanel->reload();
            }*/
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
#if !defined(Q_WS_MAEMO_5) && !defined(HB_Q_WS_MAEMO) && !defined(Q_WS_HILDON)
    zipButton->setDisabled( true );
#endif    
    mkdirButton->setDisabled( true );
    bookmarkButton->setDisabled( true );
}

void ControlPanel::enableButtons()
{
    cpButton->setEnabled( true );
    rnButton->setEnabled( true );
    delButton->setEnabled( true );
#if !defined(Q_WS_MAEMO_5) && !defined(HB_Q_WS_MAEMO) && !defined(Q_WS_HILDON)
    zipButton->setEnabled( true );
#endif
    mkdirButton->setEnabled( true );
    bookmarkButton->setEnabled( true );
}

void ControlPanel::stopAnimation()
{
    movie->stop();
}

void ControlPanel::startAnimation()
{
    movie->start();
}
