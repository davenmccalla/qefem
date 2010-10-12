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

#include <QtGlobal>
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <QLibraryInfo>

#include "mainwindow.h"
#include "defines.h"

#ifdef Q_WS_MAC
extern void qt_set_sequence_auto_mnemonic(bool b);
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{    
//qDebug()<<__FILE__<<__LINE__;
#ifdef Q_WS_MAC
    qt_set_sequence_auto_mnemonic( true );
#endif
    qDebug()<<QLibraryInfo::location( QLibraryInfo::PluginsPath );
    contentWidget = QPointer<QWidget>(new QWidget(this));
    contentWidget->setWindowFlags( Qt::SubWindow );
    wholeLayout = QSharedPointer<QVBoxLayout>(new QVBoxLayout());
    listLayout = QPointer<QHBoxLayout>(new QHBoxLayout());
    wholeLayout->setContentsMargins( 2, 2, 2, 2 );
    listLayout->setContentsMargins( 2, 2, 2, 2 );
    wholeLayout->setSpacing(2);
    listLayout->setSpacing(2);
    rightPanel = QPointer<FMPanel>(new FMPanel( this, false, contentWidget.data() ));
    leftPanel = QPointer<FMPanel>(new FMPanel( this, true, contentWidget.data() ));
    alt1 = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_1),
                              this));
    connect( alt1.data(), SIGNAL(activated()), this, SLOT(alt1Pressed()));
    alt2 = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_2),
                              this));
    connect( alt2.data(), SIGNAL(activated()), this, SLOT(alt2Pressed()));
    alt3 = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_3),
                              this));
    connect( alt3.data(), SIGNAL(activated()), this, SLOT(alt3Pressed()));
    alt4 = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_4),
                              this));
    connect( alt4.data(), SIGNAL(activated()), this, SLOT(alt4Pressed()));
    alt5 = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_5),
                              this));
    connect( alt5.data(), SIGNAL(activated()), this, SLOT(alt5Pressed()));
    alt6 = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_6),
                              this));
    connect( alt6.data(), SIGNAL(activated()), this, SLOT(alt6Pressed()));
    alt7 = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_7),
                              this));
    connect( alt7.data(), SIGNAL(activated()), this, SLOT(alt7Pressed()));
    alt8 = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_8),
                              this));
    connect( alt8.data(), SIGNAL(activated()), this, SLOT(alt8Pressed()));

    altE = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_E),
                              this));
    connect( altE.data(), SIGNAL(activated()), this, SLOT(altEPressed()));

    altT = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_T),
                              this));
    connect( altT.data(), SIGNAL(activated()), this, SLOT(altTPressed()));

    ctrlC = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C),
                              this));
    connect( ctrlC.data(), SIGNAL(activated()), this, SLOT(ctrlCPressed()));
    ctrlV = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_V),
                              this));
    connect( ctrlV.data(), SIGNAL(activated()), this, SLOT(ctrlVPressed()));
    listLayout->addWidget( leftPanel.data() );
    listLayout->addWidget( rightPanel.data() );
    controlPanel =QPointer<ControlPanel>(new ControlPanel(this,leftPanel.data(),rightPanel.data()));
    wholeLayout->addWidget( controlPanel.data() );
    wholeLayout->addLayout( listLayout.data() );
    contentWidget->setLayout( wholeLayout.data() );
    statusList = QSharedPointer<QListWidget>(new QListWidget(contentWidget.data()));
    statusList->setVisible( false );    
#if defined(Q_WS_WIN)||defined(Q_WS_MAC)||defined(Q_WS_X11)
    contentWidget->setMinimumSize(600,320);
    setMinimumSize(600,320);
    setGeometry(100,100,900,480);    
#endif
#if defined(QEFEM_MAEMO_DEV)
    setWindowState(Qt::WindowFullScreen);
    contentWidget->setMinimumSize(800,480);
    setMinimumSize(800,480);
    setGeometry(0,0,800,480);    
#endif               
    rightPanel->setDirListFocus();
//signals slots        
    connect(rightPanel.data(), SIGNAL(copyFiles(const QStringList&,const QString&,bool)),this,SLOT( copyFiles(const QStringList&,const QString& ,bool) ));
    connect(leftPanel.data(), SIGNAL(copyFiles(const QStringList&,const QString&,bool)),this,SLOT(copyFiles(const QStringList&,const QString& ,bool) ));
}

MainWindow::~MainWindow()
{
    //delete wholeLayout;
    //delete alt1;
    //delete alt2;
}

void MainWindow::resizeEvent ( QResizeEvent * /*event*/ )
{
    contentWidget->resize( size() );
}

void MainWindow::showStatus( )
{
    //statusBrowser->setVisible( true );
    //wholeLayout->removeItem( listLayout );
    //wholeLayout->addWidget( statusBrowser );
    statusList->setVisible( true );
    wholeLayout->removeItem( listLayout.data() );
    wholeLayout->addWidget( statusList.data() );
    //statusTimer.start(1000);
    controlPanel->disableButtons();
}

void MainWindow::closeStatus( )
{
    statusList->setVisible( false );
    wholeLayout->removeWidget( statusList.data() );
    wholeLayout->addLayout( listLayout.data() );
    //statusTimer.stop();
    controlPanel->enableButtons();
}

void MainWindow::updateStatus( )
{
    statusList->clear();
    statusList->addItems( controlPanel->getStatus() );
    statusList->addItems( leftPanel->getStatus() );
    statusList->addItems( rightPanel->getStatus() );
    if( statusList->count() == 0 )
    {
        controlPanel->stopAnimation();
    }
    else
    {
        controlPanel->startAnimation();
    }
}

void MainWindow::stopAnimation()
{
    controlPanel->stopAnimation();
}

void MainWindow::startAnimation()
{
    controlPanel->startAnimation();
}

QString MainWindow::leftCurrentDir()
{
    return leftPanel->curDir();
}

QString MainWindow::rightCurrentDir()
{
    return rightPanel->curDir();
}

QString MainWindow::leftCurrentFile()
{
    return leftPanel->curFile();
}

QString MainWindow::rightCurrentFile()
{
    return rightPanel->curFile();
}

void MainWindow::alt3Pressed()
{
    leftPanel->setDirListFocus();
}

void MainWindow::alt4Pressed()
{
    rightPanel->setDirListFocus();
}

void MainWindow::alt5Pressed()
{
    leftPanel->setHistoryFocus();
}

void MainWindow::alt6Pressed()
{
    rightPanel->setHistoryFocus();
}

void MainWindow::alt7Pressed()
{
    leftPanel->setBookmarkFocus();
}

void MainWindow::alt8Pressed()
{
    rightPanel->setBookmarkFocus();
}

void MainWindow::alt1Pressed()
{
    leftPanel->setDriveListFocus();
}

void MainWindow::alt2Pressed()
{
    rightPanel->setDriveListFocus();
}

void MainWindow::copyFiles( const QStringList& files, const QString& dest, bool left )
{
    controlPanel->copyFiles( files, dest, left );
}

void MainWindow::ctrlCPressed()
{
    qDebug()<<"ctrl c";
    QClipboard *clipboard = QApplication::clipboard();
    QString str("file://");
#ifdef Q_WS_WIN
    str.append("/");
#endif
    if( leftPanel->lastFocus() > rightPanel->lastFocus() )
    {
        qDebug()<<"Left panel :"<<leftPanel->curFile();
        if( leftPanel->curFile().isEmpty() )
            return;
        str.append( leftPanel->curFile() );
    }
    else
    {
        qDebug()<<"Right panel :"<<rightPanel->curFile();
        if( rightPanel->curFile().isEmpty() )
            return;
        str.append( rightPanel->curFile() );
    }
    QUrl url;
    url.setUrl( str );
    QList<QUrl> urls;
    qDebug()<< url;
    urls << str;
    QMimeData* mime = new QMimeData();
    mime->clear();
    mime->setUrls( urls );
    clipboard->setMimeData( mime );
}

void MainWindow::ctrlVPressed()
{
    qDebug()<<"ctrl v";
    //TODO check for overwriting
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if( !mimeData->hasUrls() )
        return;
    qDebug()<< mimeData->urls()[0].toString();
    QStringList files;
    int i;
    for( i=0; i < mimeData->urls().count(); i++ )
    {
        QString file( mimeData->urls()[i].toString() );
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
        if( leftPanel->lastFocus() > rightPanel->lastFocus() )
        {
            controlPanel->copyFiles( files, leftPanel->curDir(), true );
        }
        else
        {
            controlPanel->copyFiles( files, rightPanel->curDir(), false );
        }
    }
}

void MainWindow::keyPressEvent( QKeyEvent * event )
{
    switch (event->key()) {
    case Qt::Key_F1:
    case Qt::Key_Help:
        qDebug()<<"Help!!!";
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void  MainWindow::altEPressed()
{
    QStringList list;
    if( leftPanel->lastFocus() > rightPanel->lastFocus() )
    {
        QFileInfo fileToOpen( leftPanel->curFile() );
        if( fileToOpen.isFile() )
        {
            list<<leftPanel->curFile();
        }
    }
    else
    {
        QFileInfo fileToOpen( rightPanel->curFile() );
        if( fileToOpen.isFile() )
        {
            list<<rightPanel->curFile();
        }
    }
    if( list.count() > 0 )
    {
    #if defined(Q_WS_WIN)
        QProcess::startDetached( "notepad.exe", list );
    #endif
    #if defined(Q_WS_MAC)
        list.insert( 0, "-e");
        QProcess::startDetached( "open", list );
    #endif
    }
}

void  MainWindow::altTPressed()
{
    QStringList list;
    QString dir;
    dir.clear();
    if( leftPanel->lastFocus() > rightPanel->lastFocus() )
    {
            dir.append( leftPanel->curDir() );
    }
    else
    {
            dir.append( rightPanel->curDir() );
    }
    if( dir.length() > 0 )
    {
    #if defined(Q_WS_WIN)
        QProcess::startDetached( "cmd.exe", list, dir );
    #endif
    #if defined(Q_WS_MAC)
        list.append( "-n" );
        list.append("/Applications/Utilities/Terminal.app");
        //list.append("--args");
        //list.append("cd");
        //4list.append(dir);
        //list.append("&");
        QProcess::startDetached( "open", list, dir );
        //TODO: add terminal opening on OSX
    #endif
    }
}
