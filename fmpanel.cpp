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
    dlist = new driveListView();
    dirList = new FMListView();
    tab->addTab(dlist,"Drives");
    tab->addTab(dirList,"Files");
    tab->addTab(hlist,"History");
    tab->addTab(blist,"Bookmarks");
    tab->setCurrentIndex(1);
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
    connect(dlist, SIGNAL(clicked( const QModelIndex& )), this, SLOT( driveClicked( const QModelIndex & ) ));
    connect(blist, SIGNAL(clicked( const QModelIndex& )), this, SLOT( listClicked( const QModelIndex & ) ));
    connect(hlist, SIGNAL(clicked( const QModelIndex& )), this, SLOT( listClicked( const QModelIndex & ) ));
    connect(dirList, SIGNAL(clicked( const QModelIndex& )), this, SLOT( dirClicked( const QModelIndex & ) ));
    connect(dirList, SIGNAL(activated( const QModelIndex& )), this, SLOT( dirDoubleClicked( const QModelIndex & ) ));
    connect(dirList, SIGNAL(entered( const QModelIndex& )), this, SLOT( dirClicked( const QModelIndex & ) ));
    connect(pathEdit, SIGNAL(editingFinished()),this,SLOT( editFinished() ));
    connect(dirList, SIGNAL(keyUpOrDownPressed()),this,SLOT( highlightMoved() ));
    connect(dirList, SIGNAL(copyFiles(const QStringList&)),this,SLOT( copy(const QStringList& ) ));
    connect(dirList->model(), SIGNAL( rowChange()),this,SLOT(rowsChanged()));
    connect(dirList->model(), SIGNAL( rootPathChanged ( const QString& )),this, SLOT( rootChanged ( const QString& ) ));
}

/*
FMPanel::FMPanel( MainWindow* aMainW, bool aLeft, QWidget * parent, Qt::WindowFlags f) : QWidget( parent, f ), left(aLeft)
{
    mainW = aMainW;
    wholeLayout = new QVBoxLayout();
    listLayout = new QHBoxLayout();
    leftLayout = new QVBoxLayout();
    pathEdit = new QLineEdit();
    wholeLayout->setContentsMargins( 0, 0, 0, 0 );
    wholeLayout->setSpacing(0);
    wholeLayout->addLayout( listLayout );
    wholeLayout->addWidget( pathEdit );
    listLayout->setSpacing(1);
    driveList = new QListWidget();
    dirList = new FMListView();
    dirList->setSelectionMode( QAbstractItemView::ExtendedSelection );
#ifdef Q_WS_MAC
    drives = QDir("/Volumes/").entryInfoList( QDir::Dirs | QDir::NoDotAndDotDot );
    driveList->setMaximumWidth ( 45 );
    driveList->setMinimumWidth ( 45 );
#endif
#ifdef Q_WS_WIN
    QFileInfoList list = QDir::drives();
    driveList->setMaximumWidth ( 30 );
    driveList->setMinimumWidth ( 30 );
#endif
    for( int i=0; i<drives.count(); i++ )
    {
#ifdef Q_WS_MAC
        QListWidgetItem* item = new QListWidgetItem( drives[i].fileName(), driveList );
        item->setWhatsThis( drives[i].filePath () );
        item->setToolTip( drives[i].fileName() );
        driveList->addItem( item );
#endif
#ifdef Q_WS_WIN
        driveList->addItem( drives[i].absolutePath() );
#endif
    }
    //QDirModel *model = new QDirModel();
    //model->setFilter( QDir::AllEntries );
    dirModel = new FMFileSystemModel();
    dirModel->setRootPath(QDir::homePath());
    dirModel->setFilter( QDir::AllEntries );
    dirList->setModel(dirModel);
    dirList->setRootIndex(dirModel->index(QDir::homePath()));
    currentFile.clear();    
    currentDir.append( QDir::homePath());
    //dirList->setRootIndex(model->index( QDir::homePath() ));
    setPathEditText( QDir::homePath());
    leftLayout->addWidget( driveList );
    listLayout->addLayout( leftLayout );
    listLayout->addWidget( dirList );
    listLayout->setContentsMargins( 0, 0, 0, 0 );
    listLayout->setSpacing(0);
    setLayout( wholeLayout );
    setVisible( true );
    //setting events
    connect(driveList, SIGNAL(itemClicked( QListWidgetItem*)), this, SLOT( driveClicked( QListWidgetItem *) ));
    connect(driveList, SIGNAL(itemActivated( QListWidgetItem*)), this, SLOT( driveClicked( QListWidgetItem *) ));
    connect(dirList, SIGNAL(clicked( const QModelIndex& )), this, SLOT( dirClicked( const QModelIndex & ) ));
    connect(dirList, SIGNAL(activated( const QModelIndex& )), this, SLOT( dirDoubleClicked( const QModelIndex & ) ));
    connect(dirList, SIGNAL(entered( const QModelIndex& )), this, SLOT( dirClicked( const QModelIndex & ) ));
    connect(pathEdit, SIGNAL(editingFinished()),this,SLOT( editFinished() ));
    connect(dirList, SIGNAL(keyUpOrDownPressed()),this,SLOT( highlightMoved() ));
    connect(dirList, SIGNAL(copyFiles(const QStringList&)),this,SLOT( copy(const QStringList& ) ));
    connect(dirModel, SIGNAL( rowChange()),this,SLOT(rowsChanged()));
    connect(dirModel, SIGNAL( rootPathChanged ( const QString& )),this, SLOT( rootChanged ( const QString& ) ));

    lastClick = QTime::currentTime ();
    noDrive = false;
    driveJustLoaded = false;
    QTimer *driveTimer = new QTimer(this);
    connect(driveTimer, SIGNAL(timeout()), this, SLOT(driveReload()));
    driveReload();    
    driveTimer->start( 5000 );
}*/

FMPanel::~FMPanel()
{
    delete wholeLayout;
}

void FMPanel::listClicked( const QModelIndex &index )
{
    if( noDrive )
        return;
    const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>( index.model() );
    if( model != 0 )
    {
        lastClick = QTime::currentTime ();
        QStandardItem* item = model->itemFromIndex(index);
        qDebug()<<item->text();
        setPathEditText( item->text() );
        currentDir.clear();
        currentDir.append( item->text() );
        dirList->setRootPath( currentDir );
        setPathEditText( currentDir );
    }
    tab->setCurrentIndex(1);
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
        path.append("/");
        QString dirs( item->data(Qt::DisplayRole ).toString() );
        if( dirs == ".." )
        {
            QDir dir( path);
            if( dir.cdUp() )
            {
                path.clear();
                path.append( dir.canonicalPath () );
            }
            else
            {
                return;
            }

        }
        else
        {
            path.append( item->data(Qt::DisplayRole ).toString());
        }
        dirList->setRootPath( path );
        setPathEditText( path );
        currentFile.clear();
        currentFile.append( path );
    }
}

void FMPanel::dirDoubleClicked( const QModelIndex &  index )
{
    if( noDrive )
        return;
    const FMFileSystemModel* model = qobject_cast<const FMFileSystemModel*>( index.model() );
    if( model != 0 )
    {
        lastClick = QTime::currentTime ();
        QFileInfo inf = model->fileInfo(index);
        //isAbsolute()
        //isBundle()
        if( inf.isDir() )
        {
            mainW->startAnimation();
            currentFile.clear();
            //TODO check the size
            if( model->filePath(index).at(model->filePath(index).size()-1) == '.' )
            {
                if( model->filePath(index).at(model->filePath(index).size()-2) == '/' )
                {
                    return;
                }
                if(( model->filePath(index).at(model->filePath(index).size()-2) == '.' )&&
                   ( model->filePath(index).at(model->filePath(index).size()-3) == '/' ))
                {
                    QDir dirUp( model->filePath(index) );
                    currentDir.clear();
                    currentDir.append( dirUp.absolutePath()  );
                    dirList->setRootPath( currentDir );
                    setPathEditText( currentDir );
                    return;
                }
            }
            currentDir.clear();
            currentDir.append( model->filePath(index) );
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
                //QDesktopServices::openUrl( QString("file://C:\\mudras-Yoga_in_your_hands.pdf" ) );
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
        setPathEditText( model->filePath(index) );
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
#ifdef Q_WS_MAC
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
    qDebug()<<"reload "<<left<<" "<<currentDir;
    dirList->setRootPath(currentDir );
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
    lastClick = QTime::currentTime ();
    //QModelIndex topLeft = dirList->model()->index(0, 0 );
    //QModelIndex bottomRight = dirList->model()->index(dirList->model()->rowCount()-1,10);
    //QItemSelection selection(topLeft, bottomRight);
    //dirList->selectionModel()->select(selection, QItemSelectionModel::Select);
}

void FMPanel::selectionChanged()
{
    qDebug()<<"selection changed.";
    dirClicked( dirList->currentIndex() );
}

void FMPanel::highlightMoved()
{
    //qDebug()<<"HL moved.";
    dirClicked( dirList->currentIndex() );
}

void FMPanel::rowsChanged()
{
    if( driveJustLoaded )
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
    }
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
}

void FMPanel::addBookmark( const QString& path )
{
    blist->addBookmark( path );
}
