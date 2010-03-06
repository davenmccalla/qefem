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


#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVector>
#include <QLabel>
#include <QMovie>
#include "fmpanel.h"
#include "CopyTask.h"
#include "DeleteTask.h"
//#include "mainwindow.h"

class MainWindow;

class ControlPanel : public QWidget
{
    Q_OBJECT
public:
    ControlPanel( MainWindow* aMainW, FMPanel* aLeftPanel, FMPanel* aRightPanel );
    ~ControlPanel();
    QStringList getStatus();
    void disableButtons();
    void enableButtons();
    void stopAnimation();
    void startAnimation();
public slots:
    void copyTaskFinished();
    void delTaskFinished();
private slots:
    void cpButtonClicked( bool checked );
    void rnButtonClicked( bool checked );
    void delButtonClicked( bool checked );
    void zipButtonClicked( bool checked );
    void mkdirButtonClicked( bool checked );
    void statusButtonClicked( bool checked );
    void bookmarkButtonClicked( bool checked );
public:
    void copyDirs( const QString& dirName, const QString& dest, bool left );
    void copyFiles( const QStringList& files, const QString& dest, bool left );
    void copyFile( const QString& file, const QString& dest, bool left );
    void deleteFiles( const QStringList& files, bool left );
private:
    QLabel *animation;
    QMovie *movie;
    QHBoxLayout* commandLayout;
    QPushButton *cpButton;
    QPushButton *rnButton;
    QPushButton *delButton;
    QPushButton *zipButton;
    QPushButton *mkdirButton;
    QPushButton *statusButton;
    QPushButton *bookmarkButton;
    FMPanel* leftPanel;
    FMPanel* rightPanel;
    MainWindow* mainW;
    bool statusOn;
    QVector<QPair<CopyTask*,QPair<QString, bool > > > copyVector;//copy thread,path,left or right panel
    QVector<QPair<DeleteTask*,QPair<QString, bool > > > delVector;//delete thread,path,left or right panel
};

#endif // CONTROLPANEL_H
