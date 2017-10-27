/*
The file DockImages.hpp is part of the WildlifeCounter software.

WildlifeCounter is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2015 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
- OpenCV : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
*/

#ifndef HEADER_DOCKIMAGES
#define HEADER_DOCKIMAGES

#include <QtWidgets>

/*! \class DockImages
* \brief Classe d'IHM
*
* La classe gère le comportement du dock affichant les mini imagettes 
*/
class DockImages : public QDockWidget {
  //  Q_OBJECT

 public:
  /*!
   * \brief Constructeur
   *
   * Constructeur de la classe Dock images
   *
   * \param parent : Widget parent (main window)
   */
  explicit DockImages(SoftWindow *parent);
};

#endif
