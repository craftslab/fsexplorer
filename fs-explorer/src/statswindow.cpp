/**
 * statswindow.cpp - The entry of statswindow
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

#include "statswindow.h"

const int StatsWindow::width = 480;
const int StatsWindow::height = 640;

StatsWindow::StatsWindow(const QString &title, const QString &stat, QWidget *parent)
    : QWidget(parent)
{
  textEdit = new QTextEdit(this);
  textEdit->setReadOnly(true);
  textEdit->setLineWrapMode(QTextEdit::NoWrap);
  textEdit->clear();
  textEdit->setPlainText(stat);

  frameHLine = new QFrame(this);
  frameHLine->setFrameShape(QFrame::HLine);
  frameHLine->setFrameShadow(QFrame::Sunken);
  frameHLine->setLineWidth(1);
  frameHLine->setMidLineWidth(0);

  copyToClipboardButton = new QPushButton(tr("Copy to Clipboard"), this);
  connect(copyToClipboardButton, SIGNAL(clicked()), this, SLOT(copyToClipboard()));

  closeButton = new QPushButton(tr("Close"), this);
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
  connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

  hLayout = new QHBoxLayout(this);
  hLayout->insertSpacing(0, 300);
  hLayout->insertStretch(0, 1);
  hLayout->addWidget(copyToClipboardButton);
  hLayout->addWidget(closeButton);

  hLayoutWidget = new QWidget(this);
  hLayoutWidget->setLayout(hLayout);

  vLayout = new QVBoxLayout(this);
  vLayout->addWidget(textEdit);
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
}

void StatsWindow::closeEvent(QCloseEvent *event)
{
  event->accept();
}

void StatsWindow::copyToClipboard()
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
  textEdit->selectAll();
  textEdit->copy();
#endif
}
