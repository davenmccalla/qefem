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

#ifndef COPYTASK_H
#define COPYTASK_H

#include <QThread>
#include <QString>
#include <QStringList>

class CopyTask : public QThread
{
public:
    enum CopyMode{ Dir, List, File };
public:
    CopyTask(const QString& dirName, const QString& dest, CopyMode mode );
    CopyTask(const QStringList& files, const QString& dest);
    ~CopyTask();
    void run();
private:
    void copyDirs( const QString& dirName, const QString& dest );
    void copyFiles( const QString homeDir, const QStringList& files, const QString& dest );
    void copyFile( const QString& file, const QString& dest );
    void copyFiles( const QStringList& files, const QString& dest );
private:
    CopyMode copyMode;
    QString destination;
    QStringList srcList;
    QString srcDir;
    QString srcFile;
};

#endif // COPYTASK_H
