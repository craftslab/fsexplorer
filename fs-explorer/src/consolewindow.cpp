/**
 * consolewindow.cpp - The entry of consolewindow
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

#include "consolewindow.h"

ConsoleWindow::ConsoleWindow(QWidget *parent)
    : QWidget(parent)
{
  textEdit = new QTextEdit;
  textEdit->setReadOnly(false);
  textEdit->setLineWrapMode(QTextEdit::NoWrap);
  textEdit->setTextColor(QColor(0, 255, 0));
  textEdit->setStyleSheet("background-color: black");

  layout = new QVBoxLayout;
  layout->addWidget(textEdit);
  setLayout(layout);

  Qt::WindowFlags flags = Qt::Window;
  setWindowFlags(flags);

  setWindowTitle(tr("FS Console"));
}

void ConsoleWindow::closeEvent(QCloseEvent *event)
{
  event->accept();
}
