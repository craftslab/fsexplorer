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

SearchWindow::SearchWindow(const QString &title, const QStringList &list, QWidget *parent)
    : QWidget(parent)
{
  setWindowTitle(title);
  setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
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

void SearchWindow::closeEvent(QCloseEvent *event)
{
  event->accept();
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
