#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtCore>

extern QWaitCondition copyOverwriteAnswered;
extern QMutex copyOverwriteMutex;
extern int copyOverwriteAnswer;

#endif // GLOBAL_H
