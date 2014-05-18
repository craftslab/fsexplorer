/**
 * exportengine.cpp - The entry of exportengine
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

#include "exportengine.h"

ExportEngine::ExportEngine(const QString &title, const QList<unsigned long long> &list, const QString &name, FsEngine *engine, QWidget *parent)
{
  QStringList address;
  int num = 0;
  int ret;

  progress = new QProgressDialog(parent);

  progress->setWindowTitle(title);
  progress->setWindowModality(Qt::WindowModal);
  progress->setMinimumDuration(0);

  QSize sizeProgress = progress->size();
  sizeProgress.setWidth(parent->width() >> 1);
  progress->resize(sizeProgress);

  QPoint pointProgress = parent->pos();
  pointProgress.setX(pointProgress.x() + ((parent->width() - sizeProgress.width()) >> 1));
  pointProgress.setY(pointProgress.y() + ((parent->height() - sizeProgress.height()) >> 1));
  progress->move(pointProgress);

  fsEngine = engine;
  filePath = new QDir(name);
  fileCounter = 0;

  for (int i = 0; i < list.size(); ++i) {
    num += (int)count(list[i]);
  }
  progress->setRange(0, num);

  for (int i = 0; i < list.size(); ++i) {
    address.clear();
    address << QDir::separator();
    ret = traverse(list[i], address);
    if (!ret) {
      break;
    }
  }
  progress->setValue(num);
}

ExportEngine::~ExportEngine()
{
  if (filePath) {
    delete filePath;
    filePath = NULL;
  }

  if (progress) {
    delete progress;
    progress = NULL;
  }
}

unsigned int ExportEngine::count(unsigned long long ino)
{
  struct fs_dirent dent = fsEngine->getFileChildsDent(ino);
  unsigned int num = fsEngine->getFileChildsNum(ino);
  unsigned int len = num;

  if ((dent.d_type == FT_DIR && num == 2)
      || (dent.d_type != FT_DIR && num == 0)) {
    return 1;
  }

  struct fs_dirent *childs = new fs_dirent[num];
  if (!childs) {
    return num;
  }
  memset((void *)childs, 0, sizeof(struct fs_dirent) * num);

  bool ret = fsEngine->getFileChildsList(ino, childs, num);
  if (!ret) {
    goto countExit;
  }

  for (unsigned int i = 0; i < len; ++i) {
    if (!strcmp((const char *)childs[i].d_name, (const char *)FS_DNAME_DOT)
        || !strcmp((const char *)childs[i].d_name, (const char *)FS_DNAME_DOTDOT)) {
      continue;
    }

    num += count(childs[i].d_ino);
  }

countExit:

  if (childs) {
    delete[] childs;
    childs = NULL;
  }

  return num;
}

bool ExportEngine::traverse(unsigned long long ino, const QStringList &address)
{
  bool ret;

  if (progress->wasCanceled()) {
    return false;
  }

  ret = handleExport(ino, address);
  if (!ret) {
    return false;
  }

  struct fs_dirent dent = fsEngine->getFileChildsDent(ino);
  unsigned int num = fsEngine->getFileChildsNum(ino);

  if ((dent.d_type == FT_DIR && num == 2)
      || (dent.d_type != FT_DIR && num == 0)) {
    return true;
  }

  struct fs_dirent *childs = new fs_dirent[num];
  if (!childs) {
    return false;
  }
  memset((void *)childs, 0, sizeof(struct fs_dirent) * num);

  ret = fsEngine->getFileChildsList(ino, childs, num);
  if (!ret) {
    goto traverseExit;
  }

  for (unsigned int i = 0; i < num; ++i) {
    if (!strcmp((const char *)childs[i].d_name, (const char *)FS_DNAME_DOT)
        || !strcmp((const char *)childs[i].d_name, (const char *)FS_DNAME_DOTDOT)) {
      continue;
    }

    QStringList list = address;
    list << QString(tr(childs[i].d_name));

    ret = traverse(childs[i].d_ino, list);
    if (!ret) {
      break;
    }
  }

traverseExit:

  if (childs) {
    delete[] childs;
    childs = NULL;
  }

  return ret;
}

bool ExportEngine::handleExport(unsigned long long ino, const QStringList &address)
{
  struct fs_dirent dent = fsEngine->getFileChildsDent(ino);
  struct fs_kstat stat = fsEngine->getFileChildsStat(ino);
  int i;

  progress->setLabelText(QString(tr(dent.d_name)));
  progress->setValue(++fileCounter);

  QString relativePath;
  relativePath.clear();
  relativePath.append(address[0]);

  if (address.size() > 1) {
    for (i = 1; i < address.size() - 1; ++i) {
      relativePath.append(address[i]).append(QDir::separator());
    }
    relativePath.append(address[i]);
  }

  QString absolutePath = filePath->path();
  absolutePath.append(relativePath);

  if (filePath->exists(absolutePath)) {
    bool status = confirm(absolutePath, dent.d_type);
    if (!status) {
      return false;
    }
  } else {
    if (dent.d_type == FT_DIR) {
      filePath->mkpath(absolutePath);
    } else {
      // TODO
    }
  }

  return true;
}

bool ExportEngine::confirm(const QString &name, enum libfs_ftype type)
{
  bool status;
  QMessageBox msgBox;

  msgBox.setIcon(QMessageBox::Information);
  msgBox.setText(name + QString(tr(" already exists.")));
  msgBox.setInformativeText(tr("Do you want to overwrite it?"));
  msgBox.setStandardButtons(QMessageBox::Apply | QMessageBox::Ignore | QMessageBox::Abort);

  int ret = msgBox.exec();
  switch (ret) {
  case QMessageBox::Apply:
    if (type == FT_DIR) {
      filePath->setPath(name);
      filePath->removeRecursively();
      filePath->mkpath(name);
    } else {
      filePath->remove(name);
      // TODO
    }
    status = true;
    break;

  case QMessageBox::Ignore:
    status = true;
    break;

  case QMessageBox::Abort:
  default:
    status = false;
    break;
  }

  return status;
}
