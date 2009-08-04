/*
 * Copyright (c) 2008, 2009 Eugene Ingerman, Samit Basu
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
#include <QtGui>
#include "DebugWin.hpp"
#include "DebugStream.hpp"

DebugStream dbout;

DebugWin::DebugWin(QWidget *parent) : QDockWidget("Debug",parent) {
  m_list = new QTextEdit(this);
  m_list->setReadOnly(true);
  setWidget(m_list);
  setObjectName("Debug");
  dbout.setWin(this);
}

void DebugWin::addString(const QString &t) {
  m_list->append(t);
  m_list->ensureCursorVisible();
}
