/*
 * Copyright (c) 2002-2006 Samit Basu
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
#include "HandleFigure.hpp"
#include "HandleList.hpp"
#include "HandleCommands.hpp"
#include "HandleWindow.hpp"
#include <math.h>
#include <qgl.h>
#include <math.h>
#include <QApplication>

HandleFigure::HandleFigure(HandleWindow *win) {
  m_width = 640;
  m_height = 480;
  m_win = win;
  ConstructProperties();
  SetupDefaults();
}

void HandleFigure::Repaint() {
  m_win->update();
}
  
void HandleFigure::ConstructProperties() {
  //!
  //@Module FIGUREPROPERTIES Figure Object Properties
  //@@Section HANDLE
  //@@Usage
  //Below is a summary of the properties for the axis.
  //\begin{itemize}
  //  \item @|alphamap| - @|vector| - Contains the alpha (transparency) map for the
  // figure.  If this is set to a scalar, then all values are mapped to the same 
  // transparency.  It defaults to @|1|, which is all values being fully opaque.  
  // If you set this to a vector, the values of graphics objects will be mapped to 
  // different transparency values, based on the setting of their @|alphadatamapping|
  // property.
  //  \item @|color| - @|colorspec| - The background color of the figure (defaults to a
  // gray @|[0.6,0.6,0.6]|).  During printing, this color is set to white, and then
  // is restored.
  //  \item @|colormap| - @|color vector| - an @|N x 3| matrix of RGB values that
  // specifies the colormap for the figure.  Defaults to an @|HSV| map.  
  //  \item @|children| - @|handle vector| - the handles for objects that are children
  // of this figure.  These should be axis objects.
  //  \item @|currentaxes| - @|handle| - the handle for the current axes.  Also returned
  // by @|gca|.
  //  \item @|doublebuffer| - Not used.
  //  \item @|parent| -  Not used.
  //  \item @|position| - Not used.
  //  \item @|type| - @|string| - returns the string @|'figure'|.
  //  \item @|userdata| - @|array| - arbitrary array you can use to store data associated
  // with the figure.
  //  \item @|nextplot| - @|{'add','replace','replacechildren'}| - If set to @|'add'| then
  // additional axes are added to the list of children for the current figure.  If set to 
  // @|'replace'|, then a new axis replaces all of the existing children.
  //  \item @|figsize| - @|two vector| - the size of the figure window in pixels (width x height).
  //  \item @|renderer| - @|{'painters','opengl'}| - When set to @|'painters'| drawing is based
  // on the Qt drawing methods (which can handle flat shading of surfaces with transparency).
  // If you set the renderer to @|'opengl'| then OpenGL is used for rendering.  Support for 
  // OpenGL is currently in the alpha stage, and FreeMat does not enable it automatically.
  // You can set the renderer mode to @|'opengl'| manually to experiment.  Also, OpenGL 
  // figures cannot be printed yet.
  //\end{itemize}
  //!
  AddProperty(new HPVector,"alphamap");
  AddProperty(new HPColor,"color");
  AddProperty(new HPColorVector,"colormap");
  AddProperty(new HPHandles,"children");
  AddProperty(new HPHandles,"currentaxes");
  AddProperty(new HPOnOff,"doublebuffer");
  AddProperty(new HPHandles,"parent");
  AddProperty(new HPFourVector,"position");
  AddProperty(new HPString,"type");
  AddProperty(new HPArray,"userdata");
  AddProperty(new HPNextPlotMode,"nextplot");
  AddProperty(new HPTwoVector,"figsize");
  AddProperty(new HPRenderMode,"renderer");
}

void HSVRAMP(double h, double &r, double &g, double &b) {
  int i;
  double f, p, q, t;
  h *= 6;                        // sector 0 to 5
  i = (int) floor( h );
  f = h - i;                    // fractional part of h
  p = 0;
  q = 1 - f ;
  t = f ;
  switch( i ) {
  case 0:
    r = 1;      g = t;      b = p;
    break;
  case 1:
    r = q;      g = 1;      b = p;
    break;
  case 2:
    r = p;      g = 1;      b = t;
    break;
  case 3:
    r = p;      g = q;      b = 1;
    break;
  case 4:
    r = t;      g = p;      b = 1;
    break;
  default:                // case 5:
    r = 1;      g = p;      b = q;
    break;
  }
}

void HandleFigure::UpdateState() {
  m_win->UpdateState();
}

void HandleFigure::LoadDefaultColorMap() {
  QVector<double> cmap;
  for (int i=0;i<64;i++) {
    double h = i/(64.0);
    double r, g, b;
    HSVRAMP(h,r,g,b);
    cmap.push_back(r);
    cmap.push_back(g);
    cmap.push_back(b);
  }
  HPColorVector *hcv = (HPColorVector*) LookupProperty("colormap");
  hcv->Data(cmap);
  cmap.clear();
  cmap.push_back(1.0);
  HPVector *hv = (HPVector*) LookupProperty("alphamap");
  hv->Data(cmap);
}

void HandleFigure::SetupDefaults() {
  SetStringDefault("renderer","painters");
  SetStringDefault("type","figure");
  SetThreeVectorDefault("color",0.7,0.7,0.7);
  //  QColor bg(qApp->palette().color(QPalette::Window));
  //  SetThreeVectorDefault("color",bg.redF(),bg.greenF(),bg.blueF());
  SetStringDefault("nextplot","replace");
  SetTwoVectorDefault("figsize",500,300);
  // Set a default colormap to hsv(64) - this matches
  LoadDefaultColorMap();
  resized = false;
}

void HandleFigure::PaintMe(RenderEngine &gc) {
  try {
    qDebug("size = %d %d",m_width,m_height);
    // draw the children...
    //    qDebug() << "paint!\r";
    HPColor *color = (HPColor*) LookupProperty("color");
    if (color->Data()[0] >= 0) {
      //      qDebug() << "clear!\r";
      gc.clear(color->Data());
    }
    HPHandles *children = (HPHandles*) LookupProperty("children");
    QVector<unsigned> handles(children->Data());
    for (int i=0;i<handles.size();i++) {
      HandleObject *fp = LookupHandleObject(handles[i]);
      fp->PaintMe(gc);
    }
    resized = false;
  } catch (Exception& e) {
    //    cout << "Warning: Graphics subsystem reports: " << e.getMessageCopy() << "\n";
  }
}

void HandleFigure::resizeGL(int width, int height) {
  m_width = width;
  m_height = height;
  SetTwoVectorDefault("figsize",width,height);
  resized = true;
  UpdateState();
  // Change to be recursive...
  HPHandles *children = (HPHandles*) LookupProperty("children");
  QVector<unsigned> handles(children->Data());
  for (int i=0;i<handles.size();i++) {
    HandleObject *fp = LookupHandleObject(handles[i]);
    fp->UpdateState();
  }
}

bool HandleFigure::Resized() {
  return resized;
}

void HandleFigure::SetSize() {
  HPTwoVector *htv = (HPTwoVector*) LookupProperty("figsize");
  // First get the size of the main window
  QSize main_window_size = m_win->size();
  // Now get the size of the central widget
  QSize central_widget_size = m_win->centralWidget()->size();
  m_win->resize((int)(htv->Data()[0]) + main_window_size.width() - central_widget_size.width(),
		(int)(htv->Data()[1]) + main_window_size.height() - central_widget_size.height());
  //   m_win->centralWidget()->resize((int)(htv->Data()[0]),(int)(htv->Data()[1]));
  //   m_win->centralWidget()->updateGeometry();
  //   m_win->GetQtWidget()->resize((int)(htv->Data()[0]),(int)(htv->Data()[1]));
  //   m_win->GetQtWidget()->updateGeometry();
  //   m_win->updateGeometry();
  //   resizeGL((int)(htv->Data()[0]),(int)(htv->Data()[1]));
}

