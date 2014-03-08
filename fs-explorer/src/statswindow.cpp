/**
 * statswindow.cpp - The entry of statswindow
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
