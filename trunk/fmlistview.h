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

#ifndef QFMLISTVIEW_H
#define QFMLISTVIEW_H

#include <QListView>
#include <QKeyEvent>
#include <QObject>
#include <QTimer>
#include <QFileSystemWatcher>

class FMListView : public QListView
{
Q_OBJECT
public:
    explicit FMListView(QWidget *parent = 0);
    ~FMListView();
    QStringList selectedFiles();
    QString getRootDir();
protected:
    void keyPressEvent( QKeyEvent * event );
    void dropEvent( QDropEvent * e );
    void dragEnterEvent( QDragEnterEvent * event );
    void dragMoveEvent(QDragMoveEvent *event);
    void getFreeSpace(const QString& path);
signals:
    void keyUpOrDownPressed();
    void copyFiles( const QStringList& files );
    void rootPathChanged ( const QString& );
public slots:
    void setRootPath( const QString& path );
protected:
    QString rootDir;
    QStringList* fileList;
    QString freeSpace;
    QFileSystemWatcher watcher;
};

#endif // QFMLISTVIEW_H
