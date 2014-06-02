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

ExportEngine::ExportEngine(const QString &title, const QList<unsigned long long> &list, const QString &path, FsEngine *engine, QWidget *parent)
{
  struct fs_dirent dent;
  QStringList address;
  int num = 0;

  progress = new QProgressDialog(parent);

  progress->setWindowTitle(title);
  progress->setWindowModality(Qt::WindowModal);
  progress->setMinimumDuration(0);

  QSize sizeProgress = progress->size();
  sizeProgress.setWidth(parent->width() >> 1);
  progress->setFixedSize(sizeProgress);

  QPoint pointProgress = parent->pos();
  pointProgress.setX(pointProgress.x() + ((parent->width() - sizeProgress.width()) >> 1));
  pointProgress.setY(pointProgress.y() + ((parent->height() - sizeProgress.height()) >> 1));
  progress->move(pointProgress);

  fsEngine = engine;
  filePath = new QDir(path);
  fileCounter = 0;

  for (int i = 0; i < list.size(); ++i) {
    num += (int)count(list[i]);
  }

  progress->setRange(0, num);

  for (int i = 0; i < list.size(); ++i) {
    dent = fsEngine->getFileChildsDent(list[i]);

    address.clear();
    address << dent.d_name;

    int ret = traverse(list[i], address);
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
  struct fs_dirent root = fsEngine->getFileRoot();
  struct fs_dirent dent = fsEngine->getFileChildsDent(ino);
  bool ret = true;

  if (address.size() == 1 && !QString::compare(address[0], QString(root.d_name))) {
    return true;
  }

  progress->setLabelText(QString(tr(dent.d_name)));
  progress->setValue(++fileCounter);

  QString absolutePath = filePath->path();

  if (!QString::compare(address[0], QString(root.d_name))) {
    for (int i = 1; i < address.size(); ++i) {
      absolutePath.append(QDir::separator()).append(address[i]);
    }
  } else {
    for (int i = 0; i < address.size(); ++i) {
      absolutePath.append(QDir::separator()).append(address[i]);
    }
  }

  if (filePath->exists(absolutePath)) {
    if (!exportWithConfirm(ino, absolutePath)) {
      ret = false;
    }
  } else {
    if (!exportNoConfirm(ino, absolutePath)) {
      ret = false;
    }
  }

  return ret;
}

bool ExportEngine::exportNoConfirm(unsigned long long ino, const QString &name)
{
  struct fs_dirent dent = fsEngine->getFileChildsDent(ino);

  if (dent.d_type == FT_DIR) {
    if (!exportDir(name)) {
      return false;
    }
  } else {
    if (!exportFile(ino, name)) {
      return false;
    }
  }

  return true;
 }

bool ExportEngine::exportWithConfirm(unsigned long long ino, const QString &name)
{
  struct fs_dirent dent = fsEngine->getFileChildsDent(ino);
  bool ret;

  QMessageBox msgBox(progress);
  msgBox.setIcon(QMessageBox::Information);
  msgBox.setText(name + QString(tr(" already exists.")));
  msgBox.setInformativeText(QString(tr("Do you want to overwrite it?")));
  msgBox.setStandardButtons(QMessageBox::Apply | QMessageBox::Ignore | QMessageBox::Abort);

  int select = msgBox.exec();
  switch (select) {
  case QMessageBox::Apply:
    if (dent.d_type == FT_DIR) {
      filePath->setPath(name);

      if (!filePath->removeRecursively()) {
        QMessageBox::critical(progress, QString(tr("Error")), QString(tr("Failed to remove ")) + name);
        return false;
      }

      if (!exportDir(name)) {
        return false;
      }
    } else {
      if (!QFile::remove(name)) {
        QMessageBox::critical(progress, QString(tr("Error")), QString(tr("Failed to remove ")) + name);
        return false;
      }

      if (!exportFile(ino, name)) {
        return false;
      }
    }

    ret = true;
    break;

  case QMessageBox::Ignore:
    ret = true;
    break;

  case QMessageBox::Abort:
  default:
    ret = false;
    break;
  }

  return ret;
}

bool ExportEngine::exportDir(const QString &name)
{
  if (!filePath->mkpath(name)) {
    QMessageBox::critical(progress, QString(tr("Error")), QString(tr("Failed to create ")) + name);
    return false;
  }

  return true;
}

bool ExportEngine::exportFile(unsigned long long ino, const QString &name)
{
  struct fs_kstat stat = fsEngine->getFileChildsStat(ino);
  long count = stat.size, num = 0;
  char *buf = new char[count];
  QFile file;
  QFileDevice::Permissions perm;
  bool ret;

  memset((void *)buf, 0, count);

  if (!fsEngine->readFile(ino, 0, buf, count, &num)) {
    QMessageBox::critical(progress, QString(tr("Error")), QString(tr("Failed to read ")) + name);
    ret = false;
    goto exportFileExit;
  }

  file.setFileName(name);

  perm = getFilePermissions(ino);
  if (!file.setPermissions(perm)) {
    QMessageBox::critical(progress, QString(tr("Error")), QString(tr("Failed to set permission of ")) + name);
    ret = false;
    goto exportFileExit;
  }

  if (file.write(buf, num) == -1) {
    QMessageBox::critical(progress, QString(tr("Error")), QString(tr("Failed to write ")) + name);
    ret = false;
    goto exportFileExit;
  }

  ret = true;

exportFileExit:

  if (buf) {
    delete[] buf;
    buf = NULL;
  }

  return ret;
}

QFileDevice::Permissions ExportEngine::getFilePermissions(unsigned long long ino)
{
  struct fs_kstat stat = fsEngine->getFileChildsStat(ino);
  enum libfs_imode mode = stat.mode;
  QFileDevice::Permissions perm = QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner;

  if (mode & IRUSR) {
    perm |= QFileDevice::ReadUser;
  }

  if (mode & IWUSR) {
    perm |= QFileDevice::WriteUser;
  }

  if (mode & IXUSR) {
    perm |= QFileDevice::ExeUser;
  }

  if (mode & IRGRP) {
    perm |= QFileDevice::ReadGroup;
  }

  if (mode & IWGRP) {
    perm |= QFileDevice::WriteGroup;
  }

  if (mode & IXGRP) {
    perm |= QFileDevice::ExeGroup;
  }

  if (mode & IROTH) {
    perm |= QFileDevice::ReadOther;
  }

  if (mode & IWOTH) {
    perm |= QFileDevice::WriteOther;
  }

  if (mode & IXOTH) {
    perm |= QFileDevice::ExeOther;
  }

  return perm;
}
