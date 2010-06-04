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
#ifndef FINDTASK_H
#define FINDTASK_H

#include <QThread>
#include <QProcess>
#include <QSharedPointer>

class findTask : public QThread
{
    Q_OBJECT
public:
    findTask(const QString& dirName, const QString& pattern);
    ~findTask();
    void run();
    bool isEnded();
private slots:
    void readFindResult();
    void findTaskFinished( int exit, QProcess::ExitStatus status);
signals:
    void getSearchResult( const QStringList& result );
private:
    bool ended;
    QSharedPointer<QProcess> process;
    QStringList args;
};

#endif // FINDTASK_H
