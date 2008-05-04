/*
 * Copyright (c) 2007-2008 Eugene Ingerman
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "DynLib.hpp"
#include "Exception.hpp"
#include <string>
#include <iostream>

DynLib::DynLib(std::string filename) {
#ifdef WIN32
  lib = LoadLibrary(filename.c_str());
  if (!lib)
    throw Exception(std::string("Unable to open module: ") + filename);
#else
  lib = dlopen(filename.c_str(),RTLD_LAZY);
  if (!lib)
    throw Exception(std::string("Unable to open module: ") + filename + ", operating system reported error: " + dlerror());
#endif
}

DynLib::~DynLib() {
#ifdef WIN32
  FreeLibrary(lib);
#else
  dlclose(lib);
#endif
}

void* DynLib::GetSymbol(const char*symbolName) {
#ifdef WIN32
  void *func;
  func = (void*) GetProcAddress(lib,symbolName);
  if (func == NULL)
    throw Exception(std::string("Unable to find symbol ") + ((const char*) symbolName));
  return func;
#else
  void *func = dlsym(lib,symbolName);
  if (func == NULL)
    throw Exception(std::string("Unable to find symbol ") + ((const char*) symbolName) + " : " + dlerror());
  return func;
#endif
}