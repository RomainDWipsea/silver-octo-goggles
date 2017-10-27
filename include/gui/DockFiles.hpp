/*
The file SoftWindow.hpp is part of the WildlifeCounter software.

WildlifeCounter is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2015 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
- OpenCV : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
*/

#ifndef HEADER_DOCKFILES
#define HEADER_DOCKFILES

#include <QtWidgets>
#include <vector>
#include <string>
class SoftWindow;

/*! \class DockFiles
 * \brief File tree dock
*
* This class handle the file system dock.
*/
class DockFiles : public QDockWidget {
  friend class SoftWindow;
    Q_OBJECT

 public:
  /*!
   * \brief Constructor
   *
   * Class constructor 
   *
   * \param 
   */
  explicit DockFiles(SoftWindow *parent);
  ~DockFiles();

 private slots:
  void itemClicked(QModelIndex const &index);

 private :
  //! Un dock ne peut afficher qu'une seul widget, celui-ci contient les autres
  QFrame *m_masterWidget;
  QFileSystemModel *m_model;
  SoftWindow *m_parent;
};
#endif  // HEADER_DOCKFILES
