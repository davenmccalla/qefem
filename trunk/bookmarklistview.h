#ifndef BOOKMARKLISTVIEW_H
#define BOOKMARKLISTVIEW_H

#include <QListView>

class bookmarkListView : public QListView
{
Q_OBJECT
public:
    explicit bookmarkListView(QWidget *parent = 0);    
    void addBookmark( const QString& path );
signals:

public slots:

};

#endif // BOOKMARKLISTVIEW_H
