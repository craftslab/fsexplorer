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

const QString SearchWindow::separator = QObject::tr("/");
const QString SearchWindow::stopstr = QObject::tr("Stop");
const QString SearchWindow::startstr = QObject::tr("Search again");
const int SearchWindow::width = 640;
const int SearchWindow::height = 240;

SearchWindow::SearchWindow(const QString &title, FsEngine *engine, const QString &text, QWidget *parent)
    : QWidget(parent)
{
  listWidget = new QListWidget(this);
  connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(handleItemDoubleClicked(QListWidgetItem *)));

  frameHLine = new QFrame(this);
  frameHLine->setFrameShape(QFrame::HLine);
  frameHLine->setFrameShadow(QFrame::Sunken);
  frameHLine->setLineWidth(1);
  frameHLine->setMidLineWidth(0);

  goButton = new QPushButton(tr("Go"), this);
  goButton->setEnabled(false);
  connect(goButton, SIGNAL(clicked()), this, SLOT(go()));

  copyToClipboardButton = new QPushButton(tr("Copy to Clipboard"), this);
  copyToClipboardButton->setEnabled(false);
  connect(copyToClipboardButton, SIGNAL(clicked()), this, SLOT(copyToClipboard()));

  switchButton = new QPushButton(stopstr, this);
  switchButton->setEnabled(true);
  connect(switchButton, SIGNAL(clicked()), this, SLOT(stopStart()));

  closeButton = new QPushButton(tr("Close"), this);
  closeButton->setEnabled(true);
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
  connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

  hLayout = new QHBoxLayout(this);
  hLayout->insertSpacing(0, 300);
  hLayout->insertStretch(0, 1);
  hLayout->addWidget(goButton);
  hLayout->addWidget(copyToClipboardButton);
  hLayout->addWidget(switchButton);
  hLayout->addWidget(closeButton);

  hLayoutWidget = new QWidget(this);
  hLayoutWidget->setLayout(hLayout);

  vLayout = new QVBoxLayout(this);
  vLayout->addWidget(listWidget);
  vLayout->addWidget(frameHLine);
  vLayout->addWidget(hLayoutWidget);
  setLayout(vLayout);

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

  searchName = text;
  searchEngine = new SearchEngine(engine, this);

  connect(searchEngine, SIGNAL(started()), this, SLOT(handleStarted()));
  connect(searchEngine, SIGNAL(finished()), this, SLOT(handleFinished()));
  connect(searchEngine, SIGNAL(found(const QStringList &)), this, SLOT(handleFound(const QStringList &)));

#if 0 // DISUSED here
  connect(searchEngine, SIGNAL(finished()), searchEngine, SLOT(deleteLater()));
#endif

  connect(this, SIGNAL(search(const QString &)), searchEngine, SLOT(search(const QString &)));
  connect(this, SIGNAL(stop()), searchEngine, SLOT(stop()));

  emit search(searchName);
}

void SearchWindow::closeEvent(QCloseEvent *event)
{
  event->accept();

  if (searchEngine) {
    delete searchEngine;
    searchEngine = NULL;
  }
}

void SearchWindow::go()
{
  handleItemDoubleClicked(listWidget->currentItem());
}

void SearchWindow::copyToClipboard()
{
  QClipboard *clipboard = QApplication::clipboard();
  QMimeData *mimeData = new QMimeData();
  QString text;
  
  if (listWidget->count() == 0 || !clipboard) {
    return;
  }

  text.clear();

  for (int i = 0; i < listWidget->count() - 1; ++i) {
    text.append(listWidget->item(i)->text()).append('\n');
  }
  text.append(listWidget->item(listWidget->count() - 1)->text());

  mimeData->setText(text);
  clipboard->setMimeData(mimeData);
}

void SearchWindow::stopStart()
{
  static bool isStopped = true;

  if (isStopped) {
    goButton->setEnabled(true);
    copyToClipboardButton->setEnabled(true);
    switchButton->setText(startstr);

    emit stop();
  } else {
    listWidget->clear();

    goButton->setEnabled(false);
    copyToClipboardButton->setEnabled(false);
    switchButton->setText(stopstr);

    emit search(searchName);
  }

  isStopped = !isStopped;
}

void SearchWindow::handleStarted()
{
  // TODO
}

void SearchWindow::handleFinished()
{
  stopStart();
}

void SearchWindow::handleFound(const QStringList &address)
{
  QString item;
  int i;

  if (address.size() <= 0) {
    return;
  }

  item.clear();
  item.append(address[0]);

  if (address.size() > 1) {
    for (i = 1; i < address.size() - 1; ++i) {
      item.append(address[i]).append(separator);
    }
    item.append(address[i]);
  }

  listWidget->addItem(item);
}

void SearchWindow::handleItemDoubleClicked(QListWidgetItem *item)
{
  if (!item) {
    return;
  }

  emit selected(item->text());
}
