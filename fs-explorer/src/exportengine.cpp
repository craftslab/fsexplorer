/**
 * exportengine.cpp - The entry of exportengine
 *
 * Copyright (c) 2014-2015 angersax@gmail.com
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

#include "exportengine.h"

const int ExportEngine::size = 1024;

ExportEngine::ExportEngine(const QList<unsigned long long> &list, const QString &path, FsEngine *engine, QProgressBar *bar)
{
  struct fs_dirent dent;
  QStringList address;
  int num = 0;

  fsEngine = engine;
  filePath = new QDir(path);
  fileCounter = 0;
  fileBuf = new char[size];

  progressBar = bar;
  progressBar->setRange(0, 0);
  progressBar->show();

  for (int i = 0; i < list.size(); ++i) {
    num += (int)count(list[i]);
  }

  progressBar->setRange(0, num);

  for (int i = 0; i < num; ++i) {
    progressBar->setValue(i);
  }

  for (int i = 0; i < list.size(); ++i) {
    dent = fsEngine->getFileChildsDent(list[i]);

    address.clear();
    address << dent.d_name;

    int ret = traverse(list[i], address);
    if (!ret) {
      break;
    }
  }
}

ExportEngine::~ExportEngine()
{
  if (fileBuf) {
    delete[] fileBuf;
    fileBuf = NULL;
  }

  if (filePath) {
    delete filePath;
    filePath = NULL;
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
  bool ret = true;

  if (address.size() == 1 && !QString::compare(address[0], QString(root.d_name))) {
    return true;
  }

  progressBar->setValue(++fileCounter);

  QString absolutePath = QDir::toNativeSeparators(filePath->path());

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
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
  } else if (dent.d_type == FT_SYMLINK) {
    if (!exportLink(ino, name)) {
      return false;
    }
#elif defined(Q_OS_WIN32)
  // Do nothing here
#else
  // Do nothing here
#endif
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

  QMessageBox msgBox(progressBar);
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
        QMessageBox::critical(progressBar, QString(tr("Error")), QString(tr("Failed to remove ")) + name);
        return false;
      }

      if (!exportDir(name)) {
        return false;
      }
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    } else if (dent.d_type == FT_SYMLINK) {
      if (!QFile::remove(name)) {
        QMessageBox::critical(progressBar, QString(tr("Error")), QString(tr("Failed to remove ")) + name);
        return false;
      }

      if (!exportLink(ino, name)) {
        return false;
      }
#elif defined(Q_OS_WIN32)
    // Do nothing here
#else
    // Do nothing here
#endif
    } else {
      if (!QFile::remove(name)) {
        QMessageBox::critical(progressBar, QString(tr("Error")), QString(tr("Failed to remove ")) + name);
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
    QMessageBox::critical(progressBar, QString(tr("Error")), QString(tr("Failed to create ")) + name);
    return false;
  }

  return true;
}

bool ExportEngine::exportFile(unsigned long long ino, const QString &name)
{
  QFile file;
  long offset;
  volatile long mod;
  bool ret;

  if (!fileBuf) {
    return showError(QString(tr("Insufficient memory")));
  }

  file.setFileName(name);

  if (!file.open(QIODevice::WriteOnly)) {
    return showError(QString(tr("Failed to open ")) + name);
  }

  QFileDevice::Permissions perm = getFilePermissions(ino);

  if (!file.setPermissions(perm)) {
    file.close();
    return showError(QString(tr("Failed to set permission of ")) + name);
  }

  struct fs_kstat stat = fsEngine->getFileChildsStat(ino);
  if (stat.size == 0) {
    file.close();
    return true;
  }

  long num = 0;
  offset = 0;
  for (int i = 0; i < (stat.size / size); ++i, offset += size) {
    memset((void *)fileBuf, 0, size);
    ret = fsEngine->readFile(ino, offset, fileBuf, size, &num);
    if (!ret || num == 0 || num != size) {
      ret = false;
      goto exportFileExit;
    }

    if (!file.seek(offset)) {
      ret = false;
      goto exportFileExit;
    }

    if (file.write(fileBuf, num) == -1) {
      ret = false;
      goto exportFileExit;
    }
    file.flush();
  }

  mod = stat.size % size;
  if (mod == 0) {
    ret = true;
    goto exportFileExit;
  }

  memset((void *)fileBuf, 0, size);
  ret = fsEngine->readFile(ino, offset, fileBuf, mod, &num);
  if (!ret || num == 0 || num != mod) {
    ret = false;
    goto exportFileExit;
  }

  if (!file.seek(offset)) {
    ret = false;
    goto exportFileExit;
  }

  if (file.write(fileBuf, num) == -1) {
    ret = false;
    goto exportFileExit;
  }
  file.flush();

  ret = true;

exportFileExit:

  file.close();

  if (!ret) {
    ret = showError(QString(tr("Failed to write ")) + name);
  }

  return ret;
}

bool ExportEngine::exportLink(unsigned long long ino, const QString &name)
{
  struct fs_kstat stat = fsEngine->getFileChildsStat(ino);
  long num = 0;
  bool ret;

  if (!fileBuf) {
    return showError(QString(tr("Insufficient memory")));
  }

  if (stat.size == 0) {
    return showError(QString(tr("Symbol link size of ")) + name + QString(tr(" is zero")));
  }

  memset((void *)fileBuf, 0, size);
  ret = fsEngine->readFile(ino, 0, fileBuf, size, &num);
  if (!ret || num == 0) {
    ret = false;
    goto exportLinkExit;
  }

  if (!QFile::link(fileBuf, name)) {
    ret = false;
    goto exportLinkExit;
  }

  ret = true;

exportLinkExit:

  if (!ret) {
    ret = showError(QString(tr("Failed to write ")) + name);
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

bool ExportEngine::showError(const QString &msg)
{
  bool ret;

  QMessageBox msgBox(progressBar);
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setText(msg);
  msgBox.setStandardButtons(QMessageBox::Ignore | QMessageBox::Abort);

  int select = msgBox.exec();
  switch (select) {
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
