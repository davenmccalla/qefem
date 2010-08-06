#include "findtask.h"
#include <QDebug>

findTask::findTask(const QString& dirName, const QString& pattern)
{
#ifdef Q_WS_WIN
    args<<"/c"<<"dir"<<"/b"<<"/s"<<pattern;
#elif defined(Q_WS_MAC)
    //find /Users/kp/ -name "*.txt" -print
    QString pat;
    pat.append("\"");
    pat.append(pattern);
    pat.append("\"");
    args<<dirName<<"-name"<<pat<<"-print";
#elif defined(Q_OS_LINUX)
    QString pat;
    pat.append("find ");
    pat.append(dirName);
    pat.append(" -name ");
    pat.append(pattern);
    pat.append(" -print");
    args<<pat;
#endif
    qDebug()<<args;
    process = QSharedPointer<QProcess>(new QProcess());
    process->setReadChannel( QProcess::StandardOutput );
    process->setReadChannelMode( QProcess::SeparateChannels );
    process->setWorkingDirectory( dirName );
    //process->setWorkingDirectory( "/usr/bin" );
    connect( process.data(), SIGNAL(finished(int,QProcess::ExitStatus )), this, SLOT(findTaskFinished(int,QProcess::ExitStatus) ));
    connect( process.data(), SIGNAL(readyRead()),this,SLOT(readFindResult()));
    ended=false;
}

findTask::~findTask()
{
    //delete process;
}

void findTask::run()
{
#ifdef Q_WS_WIN
    process->start("cmd.exe", args);
    if (!process->waitForStarted())
    {
        emit finished();
        return;
    }
#elif defined(Q_WS_MAC)
    process->start("find", args);
    if (!process->waitForStarted())
    {
        emit finished();
        return;
    }

    process->waitForFinished(600000);
    emit finished();
#elif defined(Q_OS_LINUX)
    qDebug()<<args;
    process->start(args[0]);
    if (!process->waitForStarted())
    {
        emit finished();
        return;
    }
#endif
}

void findTask::readFindResult()
{
    QStringList list;
    qDebug()<<"1";
    QString result = process->readAll();
    qDebug()<<result;
    list = result.split("\n");
    list.removeLast();
#ifndef Q_OS_LINUX
    for( int j=0; j<list.count(); j++ )
    {
        list[j].chop(1);
    }
#endif
    qDebug()<<list;
    emit getSearchResult( list );
    return;
}

bool findTask::isEnded()
{
    return ended;
}

void findTask::findTaskFinished( int exit, QProcess::ExitStatus status)
{
    emit finished();
    return;
}
