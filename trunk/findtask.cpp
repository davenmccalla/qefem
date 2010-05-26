#include "findtask.h"
#include <QDebug>

findTask::findTask(const QString& dirName, const QString& pattern)
{
#ifdef Q_WS_WIN
    args<<"/c"<<"dir"<<"/b"<<"/s"<<pattern;
#elif defined(Q_WS_MAC)
    //find /Users/karimpinter/ -name "*.txt" -print
    QString pat;
    pat.append("\"");
    pat.append(pattern);
    pat.append("\"");
    args<<dirName<<"-name"<<pat<<"-print";
#elif defined(Q_OS_LINUX)
#endif
    qDebug()<<args;
    process = new QProcess();
    process->setReadChannel( QProcess::StandardOutput );
    process->setReadChannelMode( QProcess::SeparateChannels );
    process->setWorkingDirectory( dirName );
    //process->setWorkingDirectory( "/usr/bin" );
    //connect( process, SIGNAL(finished( int , QProcess::ExitStatus )), this, SLOT( findTaskFinished( int , QProcess::ExitStatus ) ));
    connect( process, SIGNAL(readyRead ()), this, SLOT( readFindResult() ));
}

findTask::~findTask()
{
    delete process;
}

void findTask::run()
{
#ifdef Q_WS_WIN
    process->start("cmd.exe", args);
#elif defined(Q_WS_MAC)
    process->start("find", args);
    if (!process->waitForStarted())
        return;

    if (!process->waitForFinished())
        return;
    qDebug()<<process->readLine();
#elif defined(Q_OS_LINUX)
#endif
}

void findTask::readFindResult()
{
    QStringList list;
    QString result = process->readAll();
    list = result.split("\n");
    list.removeLast();
    for( int j=0; j<list.count(); j++ )
    {
        list[j].chop(1);
    }
    qDebug()<<list;
    emit getSearchResult( list );
    return;
}
