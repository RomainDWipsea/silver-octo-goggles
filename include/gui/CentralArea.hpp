/*
The file CentralArea.hpp is part of the WildlifeCounter software.

WildlifeCounter is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2015 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
- OpenCV : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
*/

#ifndef HEADER_CENTRALAREA
#define HEADER_CENTRALAREA

#include <QtWidgets>
class SoftWindow;
class ImgZoom;
class ImgZoomBox;

/*! \class CentralArea
* \brief Classe d'IHM.
*
* La classe gère le comportement de la zone centrale de l'IHM.
*/
class CentralArea : public QFrame {
  // Q_OBJECT
  friend class ImgZoom;
  friend class SoftWindow;
 public:
  /*!
   * \brief Constructeur
   * 
   * Constructeur de la classe CentralArea
   * 
   * \param parent : Widget parent (main window)
   */
  explicit CentralArea(SoftWindow *parent);

 protected:
  /*!
   * \brief Overwriting resizeEvent	
   *
   * This function adapts the resize event to match expected output
   *
   * \param event : event that will trigger this function
   */
  void resizeEvent(QResizeEvent* event);

 private:
  //! pointer to the main image
  ImgZoom *m_imageCentral = 0;

  //! pointer to the right button
  QPushButton *m_flecheDroiteButton = 0;

  //! pointer to the left button
  QPushButton *m_flecheGaucheButton = 0;

  //! central image frame
  QFrame *m_imageFrame = 0;

  //! parent window
  SoftWindow *m_parent;
};

#endif
