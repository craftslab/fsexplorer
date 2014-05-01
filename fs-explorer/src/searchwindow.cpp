/**
 * searchwindow.cpp - The entry of searchwindow
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

#include "searchwindow.h"

const int SearchWindow::width = 640;
const int SearchWindow::height = 480;

SearchWindow::SearchWindow(const QString &title, const QString &text, QWidget *parent)
    : QWidget(parent)
{
  tableModel = new QStandardItemModel();
  tableModel->setColumnCount(2);

  tableView = new QTableView();
  tableView->setModel(tableModel);
  connect(tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClickItem(QModelIndex)));

  closeButton = new QPushButton(tr("Close"));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
  connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

  goButton = new QPushButton(tr("Go"));
  connect(goButton, SIGNAL(clicked()), this, SLOT(go()));

  copyToClipboardButton = new QPushButton(tr("Copy to Clipboard"));
  connect(copyToClipboardButton, SIGNAL(clicked()), this, SLOT(copyToClipboard()));

  vLayout = new QVBoxLayout;
  vLayout->insertSpacing(0, 300);
  vLayout->insertStretch(0, 1);
  vLayout->addWidget(closeButton);
  vLayout->addWidget(goButton);
  vLayout->addWidget(copyToClipboardButton);

  vLayoutWidget = new QWidget();
  vLayoutWidget->setLayout(vLayout);

  hLayout = new QHBoxLayout;
  hLayout->addWidget(tableView);
  hLayout->addWidget(vLayoutWidget);
  setLayout(hLayout);

  setWindowTitle(title);
  setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_DeleteOnClose, true);

  QDesktopWidget *desktopWidget = QApplication::desktop();
  QRect screenRect = desktopWidget->screenGeometry();
  if ((screenRect.width() - width) >= 0 && ((screenRect.height() - height) >= 0)) {
    move((screenRect.width() - width) / 2, (screenRect.height() - height) / 2);
  } else {
    move(0, 0);
  }
  resize(width, height);
}

void SearchWindow::closeEvent(QCloseEvent *event)
{
  event->accept();
}

void SearchWindow::go()
{
  // TODO
}

void SearchWindow::copyToClipboard()
{
#if 0 // DISUSED here
  QClipboard *clipboard = QApplication::clipboard();
  if (!clipboard) {
    return;
  }

  QMimeData *mimeData = new QMimeData();
  const QString text = QString(tr("Foo"));
  mimeData->setText(text);

  clipboard->setMimeData(mimeData);
#else
  // TODO
#endif
}

void SearchWindow::doubleClickItem(QModelIndex index)
{
  // TODO
}
