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
#include <QDesktopServices>

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
    //qDebug()<<QLibraryInfo::location( QLibraryInfo::PluginsPath );
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

    altQ = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_Q),
                              this));
    connect( altQ.data(), SIGNAL(activated()), this, SLOT(altQPressed()));

    ctrlC = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C),
                              this));
    connect( ctrlC.data(), SIGNAL(activated()), this, SLOT(ctrlCPressed()));
    ctrlV = QSharedPointer<QShortcut>(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_V),
                              this));
    connect( ctrlV.data(), SIGNAL(activated()), this, SLOT(ctrlVPressed()));
    help = QSharedPointer<QShortcut>(new QShortcut(QKeySequence::HelpContents,
                              this));
    connect( help.data(), SIGNAL(activated()), this, SLOT(helpPressed()));
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
//init settings
    QString fileName(QDir::homePath());
    fileName.append("/.Qefem/qefem.ini");
    QFile file(fileName);
    if( !file.exists() )
    {
        QSettings settings(fileName, QSettings::IniFormat);
        #ifdef Q_WS_WIN
        settings.setValue("editor","notepad.exe");
        settings.setValue("editorargs","");
        settings.setValue("terminal","cmd.exe");
        settings.setValue("terminalargs","");
        #elif defined(Q_WS_MAC)
        settings.setValue("editor","open");
        settings.setValue("editorargs","-e");
        settings.setValue("terminal","/Applications/Utilities/Terminal.app");
        settings.setValue("terminalargs","");
        #elif defined(Q_OS_LINUX)
        //NOTE: this will not work on Maemo or Meego
        settings.setValue("editor","/usr/bin/gedit");
        settings.setValue("editorargs","");
        settings.setValue("terminal","/usr/bin/gnome-terminal");
        settings.setValue("terminalargs","");
        #endif
    }
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
    QClipboard *clipboard = QApplication::clipboard();
    QString str("file://");
#ifdef Q_WS_WIN
    str.append("/");
#endif
    if( leftPanel->lastFocus() > rightPanel->lastFocus() )
    {
        if( leftPanel->curFile().isEmpty() )
            return;
        str.append( leftPanel->curFile() );
    }
    else
    {
        if( rightPanel->curFile().isEmpty() )
            return;
        str.append( rightPanel->curFile() );
    }
    QUrl url;
    url.setUrl( str );
    QList<QUrl> urls;
    urls << str;
    QMimeData* mime = new QMimeData();
    mime->clear();
    mime->setUrls( urls );
    clipboard->setMimeData( mime );
}

void MainWindow::ctrlVPressed()
{
    //TODO check for overwriting
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if( !mimeData->hasUrls() )
        return;
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

void  MainWindow::helpPressed()
{
    QString url("file:///");
    QString fileName(QDir::homePath());
    fileName.append("/.Qefem/qefem_help.html");
    if(!QFile::exists(fileName))
    {
        QFile::copy(":/help/qefem_help.html",fileName);
    }
    url.append(fileName);
    QDesktopServices::openUrl( url );
}

void  MainWindow::altEPressed()
{
    QStringList list;
    QString editor(getEditor( list ));
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
        QProcess::startDetached( editor, list );
    }
}

void  MainWindow::altTPressed()
{
    QStringList list;
    QString dir;
    QString terminal(getTerminal( list ));
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
        QProcess::startDetached( terminal, list, dir );
    }
}

void  MainWindow::altQPressed()
{
    QStringList list;
    QString fileName(QDir::homePath());
    fileName.append("/.Qefem/qefem.ini");
    QString editor(getEditor(list));
    list.append(fileName);
    #if defined(Q_WS_WIN)
        QProcess::startDetached( editor, list );
    #endif
    #if defined(Q_WS_MAC)
        QProcess::startDetached( editor, list );
    #endif
}

QString MainWindow::getEditor(QStringList& list)
{
    QString fileName(QDir::homePath());
    fileName.append("/.Qefem/qefem.ini");
    QSettings settings(fileName, QSettings::IniFormat);
    QString args(settings.value("editorargs").toString());
    list.append( args.split(" ", QString::SkipEmptyParts) );
    return settings.value("editor").toString();
}

QString MainWindow::getTerminal(QStringList& list)
{
    QString fileName(QDir::homePath());
    fileName.append("/.Qefem/qefem.ini");
    QSettings settings(fileName, QSettings::IniFormat);
    QString args(settings.value("terminalargs").toString());
    list.append( args.split(" ", QString::SkipEmptyParts) );
    return settings.value("terminal").toString();
}

