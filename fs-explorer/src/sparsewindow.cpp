/**
 * sparsewindow.cpp - The entry of sparsewindow
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This file is part of Fs Explorer.
 *
 * Fs Explorer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fs Explorer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fs Explorer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sparsewindow.h"

const int SparseWindow::width = 640;
const int SparseWindow::height = 240;

SparseWindow::SparseWindow(const QString &text, const QString &name, QWidget *parent)
  : QWidget(parent)
{
  label = new QLabel(text, this);

  cancelButton = new QPushButton(tr("Cancel"), this);
  cancelButton->setEnabled(true);
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

  shortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
  connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

  hLayout = new QHBoxLayout(this);
  hLayout->insertSpacing(0, 300);
  hLayout->insertStretch(0, 1);
  hLayout->addWidget(cancelButton);

  hLayoutWidget = new QWidget(this);
  hLayoutWidget->setLayout(hLayout);

  vLayout = new QVBoxLayout(this);
  vLayout->addWidget(label);
  vLayout->addWidget(hLayoutWidget);
  setLayout(vLayout);

  setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_DeleteOnClose, true);
  setWindowModality(Qt::NonModal);

  QDesktopWidget *desktopWidget = QApplication::desktop();
  QRect screenRect = desktopWidget->screenGeometry();
  if ((screenRect.width() - width) >= 0 && ((screenRect.height() - height) >= 0)) {
    move((screenRect.width() - width) / 2, (screenRect.height() - height) / 2);
  } else {
    move(0, 0);
  }
  setFixedSize(width, height);

  sparseName = sparseName;
  sparseEngine = new SparseEngine(name, this);

  connect(sparseEngine, SIGNAL(resultReady(const QString &)), this, SLOT(handleResultReady(const QString &)));
  connect(sparseEngine, SIGNAL(finished()), sparseEngine, SLOT(deleteLater()));
  connect(this, SIGNAL(stop()), sparseEngine, SLOT(stop()));

  sparseEngine->start();
}

SparseWindow::~SparseWindow()
{
  if (sparseEngine) {
    delete sparseEngine;
    sparseEngine = NULL;
  }
}

QString SparseWindow::getSparseName()
{
  return sparseName;
}

void SparseWindow::closeEvent(QCloseEvent *event)
{
  event->accept();

  if (sparseEngine) {
    delete sparseEngine;
    sparseEngine = NULL;
  }
}

void SparseWindow::handleResultReady(const QString &name)
{
  sparseName = name;
}
