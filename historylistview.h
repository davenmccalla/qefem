#ifndef HISTORYLISTVIEW_H
#define HISTORYLISTVIEW_H

#include <QListView>

class historyListView : public QListView
{
Q_OBJECT
public:
    explicit historyListView( bool left, QWidget *parent = 0 );
    ~historyListView();
    void addHistoryItem(const QString &path);
signals:

public slots:

private:
    bool mleft;
};

#endif // HISTORYLISTVIEW_H
