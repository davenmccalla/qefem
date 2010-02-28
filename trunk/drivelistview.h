#ifndef DRIVELISTVIEW_H
#define DRIVELISTVIEW_H

#include <QListView>
#include <QFileInfoList>

class driveListView : public QListView
{
Q_OBJECT
public:
    explicit driveListView(QWidget *parent = 0);
    ~driveListView();
signals:
public slots:
    void driveReload();

protected:
    QFileInfoList drives;
    QTimer *driveTimer;
};

#endif // DRIVELISTVIEW_H
