#include "findtask.h"
#include <QDebug>

findTask::findTask(const QString& dirName, const QString& pattern)
{
    args<<"/c"<<"dir"<<"/b"<<"/s"<<pattern;
    //qDebug()<<args;
    process = new QProcess();
    process->setReadChannel( QProcess::StandardOutput );
    process->setReadChannelMode( QProcess::SeparateChannels );
    process->setWorkingDirectory( dirName );
    //connect( process, SIGNAL(finished( int , QProcess::ExitStatus )), this, SLOT( findTaskFinished( int , QProcess::ExitStatus ) ));
    connect( process, SIGNAL(readyRead ()), this, SLOT( readFindResult() ));
}

findTask::~findTask()
{
    delete process;
}

void findTask::run()
{
    process->start("cmd.exe", args);
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
    emit getSearchResult( list );
    return;
}
