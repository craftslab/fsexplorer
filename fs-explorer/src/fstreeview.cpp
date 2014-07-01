/**
 * fstreeview.cpp - The entry of fstreeview
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This file is part of FS Explorer.
 *
 * FS explorer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FS Explorer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FS Explorer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fstreeview.h"

FsTreeView::FsTreeView(FsTreeModel *model, QWidget *parent)
  : QTreeView(parent)
{
  setModel(model);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setDragEnabled(false);
  setAcceptDrops(false);
  setDropIndicatorShown(false);

  setHeaderHidden(true);
  setColumnHidden(TREE_INO, true);

  for (int column = 0; column < model->columnCount(); ++column) {
    resizeColumnToContents(column);
  }

  QModelIndex index = model->index(0, 0);
  scrollTo(index);
  expand(index);
  setCurrentIndex(index);
}

FsTreeView::~FsTreeView()
{
  // Do nothing here
}
