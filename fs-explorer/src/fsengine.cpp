/**
 * fsengine.cpp - The entry of fsengine
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

#include "fsengine.h"

#define LIB_NAME "libfs"
#define LIB_SYMBOL "fs_opt_init"

static const char* fileTypeList[] = {
  FS_TYPE_EXT4,
  FS_TYPE_FAT,
};

FsEngine::FsEngine(QWidget *parent)
{
  parent = parent;

  fileLib = NULL;
  fileOpt = NULL;
  fileName = NULL;
  fileMount = NULL;
  fileType = NULL;
  fileRoot = NULL;
  fileParent = NULL;
  fileChilds = NULL;
  fileChildsNum = 0;
}

FsEngine::~FsEngine()
{
  closeFile();
}

bool FsEngine::openFile(QString &name)
{
  const char *dev = NULL, *dir = NULL, *type = NULL;
  QByteArray ba;
  int32_t i, len;
  int32_t ret;

  if (fileName && !fileName->compare(name)) {
    return false;
  }

  if (!loadLibrary()) {
    goto openFileFail;
  }

  if (!fileOpt || !fileOpt->mount || !fileOpt->umount) {
    goto openFileFail;
  }

  /*
   * Invalid character will be appended to string
   * if 'name.toLatin1.data()' is used instead.
   */
  ba = name.toLatin1();
  dev = (const char*)ba.data();

  dir = (const char*)"/";
  len = sizeof(fileTypeList) / sizeof(const char*);

  fileRoot = (struct fs_dirent *)malloc(sizeof(struct fs_dirent));
  if (!fileRoot) {
    goto openFileFail;
  }
  memset((void *)fileRoot, 0, sizeof(struct fs_dirent));

  for (i = 0; i < len; ++i) {
    type = fileTypeList[i];
    ret = fileOpt->mount(dev, dir, type, 0, fileRoot);
    if (ret == 0) {
      break;
    }
  }

  if (ret != 0) {
    goto openFileFail;
  }

  fileName = new QString(name);
  fileMount = new QString(dir);
  fileType = new QString(type);
  if (!fileName || !fileMount || !fileType) {
    goto openFileFail;
  }

  fileParent = (struct fs_dirent *)malloc(sizeof(struct fs_dirent));
  if (!fileParent) {
    goto openFileFail;
  }
  memset((void *)fileParent, 0, sizeof(struct fs_dirent));

  return true;

openFileFail:

  closeFile();

  return false;
}

bool FsEngine::closeFile()
{
  if (fileType) {
    delete fileType;
    fileType = NULL;
  }

  if (fileName) {
    delete fileName;
    fileName = NULL;
  }

  if (fileOpt && fileOpt->umount && fileMount) {
    (void)fileOpt->umount((const char *)fileMount->toLatin1().data(), 0);
  }

  if (fileMount) {
    delete fileMount;
    fileMount = NULL;
  }

  if (fileRoot) {
    free(fileRoot);
    fileRoot = NULL;
  }

  if (fileParent) {
    free(fileParent);
    fileParent = NULL;
  }

  if (fileChilds) {
    free(fileChilds);
    fileChilds = NULL;
  }
  fileChildsNum = 0;

  unloadLibrary();

  return true;
}

QString FsEngine::getFileType() const
{
  if (!fileType) {
    return QString("N/A");
  }

  return *fileType;
}

struct fs_dirent FsEngine::getFileRoot() const
{
  struct fs_dirent dent;

  memset((void *)&dent, 0, sizeof(struct fs_dirent));
  if (fileRoot) {
    dent = *fileRoot;
  }

  return dent;
}

void FsEngine::initFileChilds(unsigned long long ino)
{
  int32_t ret;

  if (!fileOpt || !fileOpt->querydent || !fileOpt->getdents || !fileParent || fileChilds) {
    return;
  }

  memset((void *)fileParent, 0, sizeof(struct fs_dirent));

  ret = fileOpt->querydent(ino, fileParent);
  if (ret != 0) {
    goto initFileChildsFail;
  }

  fileChildsNum = fileParent->d_childnum;

  fileChilds = (struct fs_dirent *)malloc(sizeof(struct fs_dirent) * fileChildsNum);
  if (!fileChilds) {
    goto initFileChildsFail;
  }
  memset((void *)fileChilds, 0, sizeof(struct fs_dirent) * fileChildsNum);

  ret = fileOpt->getdents(ino, fileChilds, fileChildsNum);
  if (ret != 0) {
    goto initFileChildsFail;
  }

  return;

initFileChildsFail:

  if (fileChilds) {
    free(fileChilds);
    fileChilds = NULL;
  }

  fileChildsNum = 0;

  return;
}

void FsEngine::deinitFileChilds()
{
  if (fileChilds) {
    free(fileChilds);
    fileChilds = NULL;
  }

  fileChildsNum = 0;
}

unsigned int FsEngine::getFileChildsNum() const
{
  return fileChildsNum;
}

struct fs_dirent FsEngine::getFileChilds(unsigned int index) const
{
  struct fs_dirent ret;

  memset((void *)&ret, 0, sizeof(struct fs_dirent));

  if (index >= fileChildsNum || !fileChilds) {
    return ret;
  }

  return fileChilds[index];
}

bool FsEngine::loadLibrary()
{
  fs_opt_init_t optHandle;

  fileLib = new QLibrary(LIB_NAME);
  if (!fileLib) {
    goto loadLibraryExit;
  }

  optHandle = (fs_opt_init_t)fileLib->resolve(LIB_SYMBOL);
  if (!optHandle) {
    goto loadLibraryExit;
  }

  fileOpt = new fs_opt_t;
  if (!fileOpt) {
    goto loadLibraryExit;
  }

  if (optHandle(fileOpt) != 0) {
    goto loadLibraryExit;
  }

  return true;

 loadLibraryExit:

  unloadLibrary();

  return false;
}

void FsEngine::unloadLibrary()
{
  if (fileOpt) {
    delete fileOpt;
    fileOpt  = NULL;
  }

  if (fileLib) {
    delete fileLib;
    fileLib = NULL;
  }
}
