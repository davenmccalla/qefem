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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QTextBrowser>
#include <QTimer>
#include <QShortcut>
#include "fmpanel.h"
#include "controlpanel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void showStatus();
    void closeStatus();
    void stopAnimation();
    void startAnimation();
    QString leftCurrentDir();
    QString rightCurrentDir();
    QString leftCurrentFile();
    QString rightCurrentFile();
public slots:
    void updateStatus( );
    void alt1Pressed();
    void alt2Pressed();
    void copyFiles( const QStringList& files, const QString& dest, bool left );
    void ctrlCPressed();
    void ctrlVPressed();
protected:
    void resizeEvent( QResizeEvent * event );
private:
    QVBoxLayout *wholeLayout;
    QHBoxLayout *listLayout;
    FMPanel* rightPanel;
    FMPanel* leftPanel;
    QWidget* contentWidget;
    ControlPanel* controlPanel;
    QListWidget* statusList;
    //QTextBrowser* statusBrowser;
    QTimer statusTimer;
    QShortcut* alt1;
    QShortcut* alt2;
    QShortcut* ctrlC;
    QShortcut* ctrlV;
};

#endif // MAINWINDOW_H
