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

#ifndef FMPANEL_H
#define FMPANEL_H

#include "fmlistview.h"
#include "bookmarklistview.h"
#include "historylistview.h"
#include "drivelistview.h"
#include "fmfilesystemmodel.h"
#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListView>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTime>
#include <QFileInfo>
#include <QProcess>
#include <QShortcut>
#include <QFileSystemModel>

class MainWindow;

class FMPanel : public QWidget
{
Q_OBJECT
Q_ENUMS(EditMode)
public:
    FMPanel( MainWindow* aMainW, bool aLeft, QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~FMPanel();
    QStringList getStatus();
    void setDirListFocus();
public:
    enum EditMode{ None, Rename, MkDir, Zip, Create };
public:
    //TODO: later multiply selection
    QString selectedFile();
    QStringList selectedFiles();
    QString getCurrentDir();
    QTime lastFocus();
    QLineEdit* getPathEdit();
    void setEditMode( EditMode emode );
    void setZipOutputDir( QString dir );
    QString curDir();
    QString curFile();
    void addBookmark( const QString& path );
public slots:
    void reload();
    void driveReload();
    void zipTaskFinished( int exitCode, QProcess::ExitStatus exitStatus );
    void unzipTaskFinished( int exitCode, QProcess::ExitStatus exitStatus );
    void selectionChanged();
    void rowsChanged();
    void copy( const QStringList& files );
    void rootChanged( const QString & newPath );
signals:
    void copyFiles( const QStringList& files, const QString& dest, bool left );
private slots:
    void listClicked( const QModelIndex &index );
    void driveClicked( const QModelIndex &index);
    void dirClicked( const QModelIndex &  index );
    void dirDoubleClicked( const QModelIndex &  index );
    void editFinished();
    void highlightMoved();
private:
    void setPathEditText(QString text);
    void focusInEvent ( QFocusEvent * event );
private:
    QListWidget *driveList;
    FMListView *dirList;
    bookmarkListView *blist;
    historyListView *hlist;
    driveListView *dlist;
    QHBoxLayout *listLayout;
    QVBoxLayout *wholeLayout;
    QVBoxLayout *leftLayout;
    QLineEdit* pathEdit;
    QString currentDir;
    QString currentFile;
    QTime lastClick;
    EditMode mode;
    bool noDrive;
    QFileInfoList drives;
    QString zipOutputDir;
    QVector<QPair<QProcess*,QPair<QString, bool > > > zipVector;//delete thread,path,left or right panel
    QVector<QPair<QProcess*,QPair<QString, bool > > > unzipVector;//delete thread,path,left or right panel
    MainWindow* mainW;
    bool left;
    bool driveJustLoaded;
    QTabWidget* tab;
};

#endif // FMPANEL_H
