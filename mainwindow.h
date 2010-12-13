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
#include <QPointer>
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
    QString getEditor(QStringList& list);
    QString getTerminal(QStringList& list);
public slots:
    void updateStatus( );
    void alt1Pressed();
    void alt2Pressed();
    void alt3Pressed();
    void alt4Pressed();
    void alt5Pressed();
    void alt6Pressed();
    void alt7Pressed();
    void alt8Pressed();
    void altEPressed();
    void altTPressed();
    void altQPressed();
    void copyFiles( const QStringList& files, const QString& dest, bool left );
    void ctrlCPressed();
    void ctrlVPressed();
    void helpPressed();
protected:
    void resizeEvent( QResizeEvent * event );
private:
    QSharedPointer<QVBoxLayout> wholeLayout;
    QPointer<QHBoxLayout> listLayout;
    QPointer<FMPanel> rightPanel;
    QPointer<FMPanel> leftPanel;
    QPointer<QWidget> contentWidget;
    QPointer<ControlPanel> controlPanel;
    QSharedPointer<QListWidget> statusList;
    //QTextBrowser* statusBrowser;
    QTimer statusTimer;
    QSharedPointer<QShortcut> alt1;
    QSharedPointer<QShortcut> alt2;
    QSharedPointer<QShortcut> alt3;
    QSharedPointer<QShortcut> alt4;
    QSharedPointer<QShortcut> alt5;
    QSharedPointer<QShortcut> alt6;
    QSharedPointer<QShortcut> alt7;
    QSharedPointer<QShortcut> alt8;
    QSharedPointer<QShortcut> altE;
    QSharedPointer<QShortcut> altT;
    QSharedPointer<QShortcut> altQ;
    QSharedPointer<QShortcut> ctrlC;
    QSharedPointer<QShortcut> ctrlV;
    QSharedPointer<QShortcut> help;
};

#endif // MAINWINDOW_H
