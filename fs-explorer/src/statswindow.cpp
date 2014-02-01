/**
 * statswindow.cpp - The entry of statswindow
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the distribution
 * in the file COPYING); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QClipboard>
#include <QMimeData>

#include "statswindow.h"

StatsWindow::StatsWindow(QWidget *parent)
    : QWidget(parent)
{
  textEdit = new QTextEdit;
  textEdit->setReadOnly(true);
  textEdit->setLineWrapMode(QTextEdit::NoWrap);

  frameHLine = new QFrame;
  frameHLine->setFrameShape(QFrame::HLine);
  frameHLine->setFrameShadow(QFrame::Sunken);
  frameHLine->setLineWidth(1);
  frameHLine->setMidLineWidth(0);

  copyToClipboardButton = new QPushButton(tr("Copy to Clipboard"));
  connect(copyToClipboardButton, SIGNAL(clicked()), this, SLOT(copyToClipboard()));

  closeButton = new QPushButton(tr("Close"));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  hLayout = new QHBoxLayout;
  hLayout->insertSpacing(0, 300);
  hLayout->insertStretch(0, 1);
  hLayout->addWidget(copyToClipboardButton);
  hLayout->addWidget(closeButton);

  hLayoutWidget = new QWidget();
  hLayoutWidget->setLayout(hLayout);

  vLayout = new QVBoxLayout;
  vLayout->addWidget(textEdit);
  vLayout->addWidget(frameHLine);
  vLayout->addWidget(hLayoutWidget);
  setLayout(vLayout);

  setWindowTitle(tr("FS Stats"));
  Qt::WindowFlags flags = Qt::Window | Qt::WindowStaysOnTopHint;
  setWindowFlags(flags);
  setAttribute(Qt::WA_DeleteOnClose, true);

  int width = 480;
  int height = 640;
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
  QClipboard *clipboard = QApplication::clipboard();
  if (!clipboard) {
    return;
  }

  const QString textDemo = QString(tr("Demo"));
  const QString textSummary = textDemo;

  const QString htmlDemo = QString(tr("<h3>%1<h3>\n")).arg(tr("Demo"));
  const QString htmlSummary = htmlDemo;

  QMimeData *mimeData = new QMimeData();
  mimeData->setText(textSummary);
  mimeData->setHtml(htmlSummary);

  clipboard->setMimeData(mimeData);
}
