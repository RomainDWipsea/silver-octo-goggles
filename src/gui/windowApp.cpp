/*
The file windowApp.cpp is part of the Harmony software.

Harmony is a software that provides tools for the detection of wildlife in series of images.

Copyright 2014-2016 WIPSEA SARL, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html

*/

#include <QApplication>
#include <QtWidgets>
#include <iostream>
#include "SoftWindow.hpp"
#include "wipro_config.h"
#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

#ifdef WINDOWS
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif  // WINDOWS

int main(int argc, char *argv[]) {

  QApplication app(argc, argv);

  QString sDir = QCoreApplication::applicationDirPath();
  app.addLibraryPath(sDir+"/plugins");

  SoftWindow  window;
  return app.exec();
}
