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

#include "fmfilesystemmodel.h"

FMFileSystemModel::FMFileSystemModel(QObject *parent) :
    QFileSystemModel(parent)
{
    connect(this, SIGNAL( rowsRemoved( const QModelIndex &, int , int )),this,SLOT(rowsChanged( const QModelIndex &, int , int )));
    connect(this, SIGNAL( rowsInserted( const QModelIndex &, int , int )),this,SLOT(rowsChanged( const QModelIndex &, int , int )));
}

void FMFileSystemModel::rowsChanged( const QModelIndex & /*parent*/, int /*start*/, int /*end*/ )
{
    emit rowChange();
}
