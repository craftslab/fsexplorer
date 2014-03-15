/**
 * fsengine.h - Header of fsengine
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

#ifndef FSENGINE_H
#define FSENGINE_H

#include <QObject>
#include <QLibrary>  

#ifdef __cplusplus
extern "C" {
#include "libfs.h"
}
#endif /* __cplusplus */

class FsEngine : public QObject
{
  Q_OBJECT

public:
  FsEngine(QWidget *parent = 0);
  ~FsEngine();

  bool openFile(QString &name);
  bool closeFile();
  QString getFileType() const;
  struct fs_dirent getFileRoot() const;
  void initFileChilds(unsigned long long ino);
  void deinitFileChilds();
  unsigned int getFileChildsNum() const;
  struct fs_dirent getFileChilds(unsigned int index) const;

private:
  bool loadLibrary();
  void unloadLibrary();

  QLibrary *fileLib;
  fs_opt_t *fileOpt;
  QString *fileName;
  QString *fileMount;
  QString *fileType;
  struct fs_dirent *fileRoot;
  struct fs_dirent *fileParent;
  struct fs_dirent *fileChilds;
  unsigned int fileChildsNum;
};
#endif
