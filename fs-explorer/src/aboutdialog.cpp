/**
 * aboutdialog.cpp - The entry of aboutdialog
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

#include <QDialog>
#include <QWidget>
#include <QString>

#include "aboutdialog.h"

AboutDialog::AboutDialog(const QString &version, QWidget *parent)
  : QDialog(parent)
{
  uiAbout.setupUi(this);

  uiAbout.mVersion->setText(uiAbout.mVersion->text().arg(version));
  QString url = "<a href=\"https://github.com/lemonjia/Fricando/\">https://github.com/lemonjia/Fricando/</a>";
  uiAbout.mHomepage->setText(uiAbout.mHomepage->text().arg(url));
  connect(uiAbout.mButtons, SIGNAL(accepted()), this, SLOT(accept()));
}
