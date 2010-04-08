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

#include "fmpanel.h"
#include "mainwindow.h"
#include <QDirModel>
#include <QDir>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QDebug>
#include <QToolTip>
#include <QTimer>
#include <QApplication>
#include <QDesktopServices>

FMPanel::FMPanel( MainWindow* aMainW, bool aLeft, QWidget * parent, Qt::WindowFlags f) : QWidget( parent, f ), left(aLeft)
{
    //setup variables
    mainW = aMainW;
    lastClick = QTime::currentTime ();
    noDrive = false;
    driveJustLoaded = false;
    //setup layouts and lists
    wholeLayout = new QVBoxLayout();
    tab = new QTabWidget();
    blist = new bookmarkListView();
    hlist = new historyListView( left );
#if !defined(Q_WS_MAEMO_5) && !defined(HB_Q_WS_MAEMO) && !defined(Q_WS_HILDON) && !defined(Q_OS_LINUX)
    dlist = new driveListView();
#endif    
    dirList = new FMListView();
#if !defined(Q_WS_MAEMO_5) && !defined(HB_Q_WS_MAEMO) && !defined(Q_WS_HILDON) && !defined(Q_OS_LINUX)
    tab->addTab(dlist,"Drives");
#endif    
    tab->addTab(dirList,"Files");
    tab->addTab(hlist,"History");
    tab->addTab(blist,"Bookmarks");
#if defined(Q_WS_MAEMO_5) || defined(HB_Q_WS_MAEMO) || defined(Q_WS_HILDON) || defined(Q_OS_LINUX) 
    tab->setCurrentIndex(0);
#else
    tab->setCurrentIndex(1);
#endif    
    pathEdit = new QLineEdit();
    wholeLayout->setContentsMargins( 0, 0, 0, 0 );
    wholeLayout->setSpacing(0);
    wholeLayout->addWidget( tab );
    wholeLayout->addWidget( pathEdit );
    setLayout( wholeLayout );
    setVisible( true );
    setPathEditText( QDir::homePath());
    currentDir.clear();
    currentDir.append(QDir::homePath());
    currentFile.clear();
    currentFile.append(QDir::homePath());
    //setup signals and slots
#if !defined(Q_WS_MAEMO_5) && !defined(HB_Q_WS_MAEMO) && !defined(Q_WS_HILDON) && !defined(Q_OS_LINUX)
    connect(dlist, SIGNAL(clicked( const QModelIndex& )), this, SLOT( driveClicked( const QModelIndex & ) ));
    connect(dlist, SIGNAL(activated( const QModelIndex& )), this, SLOT( driveClicked( const QModelIndex & ) ));
#endif
    connect(blist, SIGNAL(clicked( const QModelIndex& )), this, SLOT( listClicked( const QModelIndex & ) ));
    connect(blist, SIGNAL(activated( const QModelIndex& )), this, SLOT( listClicked( const QModelIndex & ) ));
    connect(hlist, SIGNAL(clicked( const QModelIndex& )), this, SLOT( listClicked( const QModelIndex & ) ));
    connect(hlist, SIGNAL(activated( const QModelIndex& )), this, SLOT( listClicked( const QModelIndex & ) ));
    connect(dirList, SIGNAL(clicked( const QModelIndex& )), this, SLOT( dirClicked( const QModelIndex & ) ));
    connect(dirList, SIGNAL(activated( const QModelIndex& )), this, SLOT( dirDoubleClicked( const QModelIndex & ) ));
    connect(dirList, SIGNAL(entered( const QModelIndex& )), this, SLOT( dirClicked( const QModelIndex & ) ));
    connect(pathEdit, SIGNAL(editingFinished()),this,SLOT( editFinished() ));
    connect(dirList, SIGNAL(keyUpOrDownPressed()),this,SLOT( highlightMoved() ));
    connect(dirList, SIGNAL(copyFiles(const QStringList&)),this,SLOT( copy(const QStringList& ) ));
    //connect(dirList->model(), SIGNAL( rowChange()),this,SLOT(rowsChanged()));
    connect(dirList, SIGNAL( rootPathChanged ( const QString& )),this, SLOT( rootChanged ( const QString& ) ));
}

FMPanel::~FMPanel()
{
    delete wholeLayout;
}

void FMPanel::listClicked( const QModelIndex &index )
{
#if defined(Q_WS_MAEMO_5) || defined(HB_Q_WS_MAEMO) || defined(Q_WS_HILDON) || defined(Q_OS_LINUX)
    tab->setCurrentIndex(0);
#else
    tab->setCurrentIndex(1);
#endif    
    if( noDrive )
        return;
    const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>( index.model() );
    if( model != 0 )
    {
        lastClick = QTime::currentTime ();
        QStandardItem* item = model->itemFromIndex(index);
        qDebug()<<item->text();
        //setPathEditText( item->text() );
        currentDir.clear();
        currentDir.append( item->text() );
        dirList->setRootPath( currentDir );
        //setPathEditText( currentDir );
    }
}


void FMPanel::driveClicked( const QModelIndex &index )
{
    const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>( index.model() );
    if( model == NULL )
        return;
    QStandardItem* item = model->itemFromIndex(index);
#ifdef Q_WS_MAC
    QString path( item->whatsThis() );
#else
    QString path( item->text() );
#endif
    QDir dir( path );
    currentFile.clear();
    if( dir.entryInfoList().count()!=0 )
    {
        lastClick = QTime::currentTime ();
        currentDir.clear();
        currentDir.append( path );
        dirList->setRootPath( currentDir );
        setPathEditText( currentDir );
        driveJustLoaded = true;
        noDrive = false;
    }
    else
    {
        //TODO: check this out on win
        QStandardItemModel* model = new QStandardItemModel( 1, 1 );
        QStandardItem* empty = new QStandardItem("No drive available.");
        currentDir.clear();
        model->insertRow( 0, empty );
        dirList->setModel( model );
        setPathEditText( "No drive available." );
        noDrive = true;
        mainW->stopAnimation();
    }
    tab->setCurrentIndex(1);
    return;
}

void FMPanel::dirClicked( const QModelIndex &  index )
{
    if( noDrive )
        return;
    const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>( index.model() );
    if( model != 0 )
    {
        lastClick = QTime::currentTime ();
        QStandardItem* item = model->itemFromIndex( index );
        QString path( dirList->getRootDir() );
        if( path.at(path.size()-1)!='/')
        {
            path.append("/");
        }
        if( index.row() == 0 )
        {
            path.append( ".." );
        }
        else
        {
            path.append( item->data(Qt::DisplayRole ).toString());
        }
        setPathEditText( path );
        currentFile.clear();
        currentFile.append( path );
    }
}

void FMPanel::dirDoubleClicked( const QModelIndex &  index )
{
    if( noDrive )
        return;
    const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>( index.model() );
    if( model != 0 )
    {
        lastClick = QTime::currentTime ();
        QStandardItem* item = model->itemFromIndex( index );
        QString path( dirList->getRootDir() );
        if( path.at(path.size()-1)!='/')
        {
            path.append("/");
        }
        QString dirs( item->data(Qt::DisplayRole ).toString() );
        if(( dirs[0] == '.' )&&( dirs[1] == '.' )&&( dirs[2] == '\t' ))
        {
            QDir dir( path);
            if( dir.cdUp() )
            {
                path.clear();
                path.append( dir.canonicalPath () );
                currentDir.clear();
                currentDir.append( path  );
                dirList->setRootPath( currentDir );
                setPathEditText( currentDir );
                return;
            }
            else
            {
                return;
            }
        }
        path.append(dirs);
        QFileInfo inf( path ) ;
        //isAbsolute()
        //isBundle()
        if( inf.isDir() )
        {
            //mainW->startAnimation();
            currentFile.clear();
            //TODO check the size
            currentDir.clear();
            currentDir.append( path );
            dirList->setRootPath( currentDir );
            setPathEditText( currentDir );
            return;
        }
        if( inf.isExecutable() )
        {
            QProcess::startDetached( inf.absoluteFilePath() );
        }
        if( inf.isFile() )
        {
            if(( inf.suffix().compare( "7z", Qt::CaseInsensitive ) == 0)||
              ( inf.suffix().compare( "zip", Qt::CaseInsensitive ) == 0)||
              ( inf.suffix().compare( "gzip", Qt::CaseInsensitive ) == 0)||
              ( inf.suffix().compare( "bzip2", Qt::CaseInsensitive ) == 0)||
              ( inf.suffix().compare( "tar", Qt::CaseInsensitive ) == 0)||
              ( inf.suffix().compare( "arj", Qt::CaseInsensitive ) == 0)||
              ( inf.suffix().compare( "rar", Qt::CaseInsensitive ) == 0) )
            {
            mainW->startAnimation();
            //TODO make a lin version
#ifdef Q_WS_MAC
            QStringList args;
            //args<<"a"<<"-r"<<"-y"<<output<<fileInfo.fileName();
            //TODO change the output dir
            QString out;
            if( left )
            {
                //TODO check the current dir can be empty
                out.append(mainW->rightCurrentDir());
            }
            else
            {
                //TODO check the current dir can be empty
                out.append(mainW->leftCurrentDir());
            }
            args<<"-o"<<"-qq"<<inf.filePath()<<"-d"<<out;
            qDebug()<<args;
            QProcess* proc = new QProcess();
            proc->setWorkingDirectory( currentDir );
            connect( proc, SIGNAL(finished( int , QProcess::ExitStatus )), this, SLOT( unzipTaskFinished( int , QProcess::ExitStatus ) ));
            //TODO change the output dir
            unzipVector.append(QPair<QProcess *,QPair<QString, bool> >( proc, QPair<QString, bool>( currentDir, true)) );
            proc->start( "unzip", args );
            reload();
            setPathEditText( currentDir );
            dirList->setFocus();
#endif
#ifdef Q_WS_WIN
            QStringList args;
            //args<<"a"<<"-r"<<"-y"<<output<<fileInfo.fileName();
            //TODO change the output dir
            QString out("-o");
            if( left )
            {
                //TODO check the current dir can be empty
                out.append(mainW->rightCurrentDir());
            }
            else
            {
                //TODO check the current dir can be empty
                out.append(mainW->leftCurrentDir());
            }
            args<<"x"<<"-bd"<<"-y"<<out<<inf.filePath();
            qDebug()<<args;
            QProcess* proc = new QProcess();
            proc->setWorkingDirectory( currentDir );
            connect( proc, SIGNAL(finished( int , QProcess::ExitStatus )), this, SLOT( unzipTaskFinished( int , QProcess::ExitStatus ) ));
            //TODO change the output dir
            unzipVector.append(QPair<QProcess *,QPair<QString, bool> >( proc, QPair<QString, bool>( currentDir, true)) );
            proc->start( "7z.exe", args );
            reload();
            setPathEditText( currentDir );
            dirList->setFocus();
#endif
            }
            else
            {
                QString url("file:///");
                url.append( inf.filePath() );
                qDebug()<<url;
                QDesktopServices::openUrl( url );
            }
        }
        //if( isFile() )
        //isHidden()
        //isReadable()
        //isRelative()
        //isRoot()
        //isSymLink()
        //isWritable()
        //setPathEditText( model->filePath(index) );
    }
}

QString FMPanel::selectedFile()
{
    return pathEdit->text();
}

QStringList FMPanel::selectedFiles()
{
    QStringList list = dirList->selectedFiles();
    QStringList res;
    QString fileName;
    QString full;
    foreach( fileName, list)
    {
        full.clear();
        full.append( currentDir );
        full.append("/");
        full.append( fileName );
        res.append( full );
    }
    return res;
}


QTime FMPanel::lastFocus()
{
    return lastClick;
}

QLineEdit* FMPanel::getPathEdit()
{
    return pathEdit;
}

void FMPanel::setEditMode( EditMode emode )
{
    if( noDrive )
        return;
    mode = emode;
    switch( mode )
    {
        case None:
        {
            return;
        }
        case Rename:
        {
            QFileInfo fInfo( pathEdit->text() );
            if( !fInfo.isFile() )
            {
                //QToolTip::showText( window()->pos(), "Please select a file!");
            }
            else
            {
                currentFile.clear();
                currentFile.append( pathEdit->text() );
                setPathEditText( fInfo.fileName() );
                pathEdit->setFocus();
                pathEdit->selectAll();
            }
            break;
        }
        case MkDir:
        {
            //TODO generate dir names
            pathEdit->setText("New_Dir");
            pathEdit->setFocus();
            pathEdit->selectAll();
            break;
        }
        case Zip:
        {
            //TODO generate dir names
            currentFile.clear();
            currentFile.append( pathEdit->text() );
            pathEdit->setText("New.zip");
            pathEdit->setFocus();
            pathEdit->selectAll();
            break;
        }
        case Create:
            break;
        default:
            return;
    }
    return;
}

void FMPanel::editFinished()
{
    switch( mode )
    {
        case None:
        {
            QString dirName( pathEdit->text() );
            QFileInfo finfo( dirName );
            if( finfo.exists() && finfo.isDir() )
            {
                currentFile.clear();
                currentFile.append( dirName );
                currentDir.clear();
                currentDir.append( dirName );
                dirList->setRootPath( dirName );
            }
            else
            {
                setPathEditText( currentDir );
            }
            dirList->setFocus();
            return;
        }
        case Rename:
        {
            QFile file( currentFile );
            QString fileName( currentDir );
            if( fileName.at(fileName.size()-1) != '/')
                fileName.append("/");
            fileName.append( pathEdit->text() );
            dirList->setFocus();
            qDebug()<<"Renaming from"<<file.fileName()<<" to "<<fileName;
            if( !file.rename( fileName ) )
            {
                //QToolTip::showText( pathEdit->window()->pos(), "Renaming file failed!");
                setPathEditText( file.fileName() );
            }
            else
            {
                setPathEditText( file.fileName() );
            }
            reload();
            mainW->updateStatus();
            break;
        }
        case MkDir:
        {
            mode = None;
            QDir newdir( currentDir );
            QString newPath(currentDir);
            //TODO: check this on win with drives
            newPath.append("/");
            newPath.append( pathEdit->text() );
            qDebug()<<"Creating dir :"<<newPath;
            if( !newdir.mkdir( newPath ) )
            {
                qDebug()<<"Creating dir failed :"<<newPath;
                //QToolTip::showText( window()->pos(), "Creating dir failed!");
            }
            else
            {
                reload();
            }
            setPathEditText( currentDir );
            dirList->setFocus();
            mainW->updateStatus();
            break;
        }
        case Zip:
        {
            mode = None;
#if defined(Q_WS_MAC) || defined(Q_OS_LINUX)
            //TODO: there is a problem with updates cause the zip can take a lot of time
            QString output;
            output.append( zipOutputDir );
            output.append("/");
            output.append(pathEdit->text());
            QStringList selF = selectedFiles();
            QStringList shortF;
            QString name;
            foreach( name, selF)
            {
                QFileInfo fileInfo( name );
                shortF.append( fileInfo.fileName() );
            }
            QStringList args;            
            args<<"-r"<<"-u"<<output<<shortF;
            qDebug()<<args;
            QProcess* proc = new QProcess();
            proc->setWorkingDirectory( currentDir );
            connect( proc, SIGNAL(finished( int , QProcess::ExitStatus )), this, SLOT( zipTaskFinished( int , QProcess::ExitStatus ) ));
            zipVector.append(QPair<QProcess *,QPair<QString, bool> >( proc, QPair<QString, bool>( output, true)) );
            proc->start( "zip", args );
            reload();
            setPathEditText( currentDir );
            dirList->setFocus();
#endif
#ifdef Q_WS_WIN
            //TODO: there is a problem with updates cause the zip can take a lot of time
            QFileInfo fileInfo( currentFile );
            QString output;
            output.append( zipOutputDir );
            output.append("/");
            output.append(pathEdit->text());
            QStringList args;
            //args<<"a"<<"-r"<<"-y"<<output<<fileInfo.fileName();
            //args<<"a"<<"-r"<<"-y"<<output<<fileInfo.canonicalFilePath();
            args<<"a"<<"-r"<<"-y"<<output<<selectedFiles();
            qDebug()<<args;
            QProcess* proc = new QProcess();
            proc->setWorkingDirectory( currentDir );
            connect( proc, SIGNAL(finished( int , QProcess::ExitStatus )), this, SLOT( zipTaskFinished( int , QProcess::ExitStatus ) ));
            zipVector.append(QPair<QProcess *,QPair<QString, bool> >( proc, QPair<QString, bool>( output, true)) );
            proc->start( "7z.exe", args );
            reload();
            setPathEditText( currentDir );
            dirList->setFocus();
#endif
            mainW->updateStatus();
            break;
        }
        case Create:
            break;
    }
    mode = None;
    return;
}

void FMPanel::reload()
{
    //QDirModel* model = qobject_cast<QDirModel*>( dirList->model() );
    //if( model != NULL )
    //    model->refresh( model->index( currentDir ) );
    //qDebug()<<"reload "<<left<<" "<<currentDir;
    //dirList->setRootPath(currentDir );
    //dirList->update();
}

void FMPanel::driveReload()
{
    //qDebug()<<"driveReload";
#ifdef Q_WS_MAC
    QFileInfoList list = QDir("/Volumes/").entryInfoList( QDir::Dirs | QDir::NoDotAndDotDot );
#else
    QFileInfoList list = QDir::drives();
#endif
    if( list != drives )
    {
        qDebug()<<"drives changed.";
        drives.clear();
        driveList->clear();
        for( int i=0; i<list.count(); i++ )
        {
            drives.append( list[i] );
#ifdef Q_WS_MAC
            QListWidgetItem* item = new QListWidgetItem( list[i].fileName(), driveList );
            item->setWhatsThis( list[i].filePath () );
            item->setToolTip( list[i].fileName() );
            driveList->addItem( item );
#else
            driveList->addItem( list[i].absolutePath() );
#endif
        }
    }
}

void FMPanel::setZipOutputDir( QString dir )
{
    zipOutputDir.clear();
    zipOutputDir.append( dir );
}

QString FMPanel::getCurrentDir()
{
    return currentDir;
}

void FMPanel::zipTaskFinished( int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/ )
{
    qDebug()<<"Zip finished.";
    for(int i=0; i < zipVector.size(); i++ )
    {
        if( zipVector[i].first->state() == 0 )
        {
            //QString text("Zip of ");
            //text.append( zipVector[i].second.first );
            //text.append(" finished.");
            //QToolTip::showText( QApplication::activeWindow()->pos(), text);
            delete zipVector[i].first;
            zipVector.remove(i);
            i--;
        }
    }    
    mainW->updateStatus();
}

void FMPanel::unzipTaskFinished( int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/ )
{
    qDebug()<<"Unzip finished.";
    for(int i=0; i < unzipVector.size(); i++ )
    {
        if( unzipVector[i].first->state() == 0 )
        {
            //QString text("Unzip of ");
            //text.append( unzipVector[i].second.first );
            //text.append(" finished.");
            //QToolTip::showText( QApplication::activeWindow()->pos(), text);
            delete unzipVector[i].first;
            unzipVector.remove(i);
            i--;
        }
    }
    mainW->updateStatus();
}

QStringList FMPanel::getStatus()
{
    QStringList list;
    for(int i=0; i < zipVector.size(); i++ )
    {
        if( zipVector[i].first->state() == 2 )
        {
            QString text("Zip of ");
            text.append( zipVector[i].second.first );
            text.append(" is running.");
            list << text;
        }
    }
    for(int i=0; i < unzipVector.size(); i++ )
    {
        if( unzipVector[i].first->state() == 2 )
        {
            QString text("Zip of ");
            text.append( unzipVector[i].second.first );
            text.append(" is running.");
            list << text;
        }
    }
    return list;
}

QString FMPanel::curDir()
{
    return currentDir;
}

QString FMPanel::curFile()
{
    return currentFile;
}

void FMPanel::setDirListFocus()
{
    dirList->setFocus(Qt::ShortcutFocusReason);
    lastClick = QTime::currentTime();
#if defined(Q_WS_MAEMO_5) || defined(HB_Q_WS_MAEMO) || defined(Q_WS_HILDON) || defined(Q_OS_LINUX)
    tab->setCurrentIndex(0);
#else
    tab->setCurrentIndex(1);
#endif
}

void FMPanel::setDriveListFocus()
{
#if !defined(Q_WS_MAEMO_5) && !defined(HB_Q_WS_MAEMO) && !defined(Q_WS_HILDON) && !defined(Q_OS_LINUX)
    dirList->setFocus(Qt::ShortcutFocusReason);
    lastClick = QTime::currentTime();
    tab->setCurrentIndex(0);
#endif
}

void FMPanel::setHistoryFocus()
{
    dirList->setFocus(Qt::ShortcutFocusReason);
    lastClick = QTime::currentTime();
#if defined(Q_WS_MAEMO_5) || defined(HB_Q_WS_MAEMO) || defined(Q_WS_HILDON)|| defined(Q_OS_LINUX)
    tab->setCurrentIndex(1);
#else
    tab->setCurrentIndex(2);
#endif
}

void FMPanel::setBookmarkFocus()
{
    dirList->setFocus(Qt::ShortcutFocusReason);
    lastClick = QTime::currentTime();
#if defined(Q_WS_MAEMO_5) || defined(HB_Q_WS_MAEMO) || defined(Q_WS_HILDON)|| defined(Q_OS_LINUX)
    tab->setCurrentIndex(2);
#else
    tab->setCurrentIndex(3);
#endif
}

void FMPanel::selectionChanged()
{
    //qDebug()<<"selection changed.";
    dirClicked( dirList->currentIndex() );
}

void FMPanel::highlightMoved()
{
    //qDebug()<<"HL moved.";
    dirClicked( dirList->currentIndex() );
}

void FMPanel::rowsChanged()
{
    /*if( driveJustLoaded )
    {
        driveJustLoaded = false;
        return;
    }
    if( dirList->hasFocus() )
    {
        dirList->selectionModel()->select(dirList->currentIndex(), QItemSelectionModel::Select);
    }
    else
    {
        dirClicked( dirList->currentIndex() );
    }*/
}

void FMPanel::setPathEditText(QString text)
{
#ifdef Q_WS_WIN
    text.replace( QString("/"), QString("\\"));
    pathEdit->setText( text );
#else
    pathEdit->setText( text );
#endif
}

void FMPanel::copy( const QStringList& files )
{
    emit copyFiles( files, currentDir, left );
}

void FMPanel::focusInEvent( QFocusEvent * /*event*/ )
{
    qDebug()<<"focusInEvent";
    lastClick = QTime::currentTime ();
}

void FMPanel::rootChanged ( const QString & newPath )
{
    qDebug()<<"root changed "<<newPath;

    mainW->stopAnimation();

    hlist->addHistoryItem( newPath );

    currentDir.clear();
    currentDir.append( newPath );
    currentFile.clear();
    currentFile.append( newPath );

    setPathEditText( newPath );
}

void FMPanel::addBookmark( const QString& path )
{
    #ifdef Q_WS_WIN
    QString temp( path );
    temp.replace( QString("/"), QString("\\"));
    blist->addBookmark( temp );
    #else
    blist->addBookmark( path );
    #endif
}
