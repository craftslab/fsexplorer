/**
 * searchengine.cpp - The entry of searchengine
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

#include "searchengine.h"

SearchEngine::SearchEngine(FsEngine *engine, QObject *parent)
{
  parent = parent;
  searchThread = new SearchThread(engine);
}

SearchEngine::~SearchEngine()
{
  if (searchThread) {
    delete searchThread;
    searchThread = NULL;
  }
}

void SearchEngine::search(const QString &name)
{
  // TODO
}

void SearchEngine::stop()
{
  // TODO
}

void SearchEngine::handleFound(const QString &name)
{
  // TODO
}

void SearchEngine::handleFinished()
{
  // TODO
}

SearchThread::SearchThread(FsEngine *engine, QObject *parent)
{
  fsEngine = engine;
  parent = parent;
}

SearchThread::~SearchThread()
{
  // TODO
}

void SearchThread::run()
{
}
