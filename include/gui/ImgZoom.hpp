/*
The file ImgZoom.hpp is part of the WildlifeCounter software.

WildlifeCounter is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2015 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
- OpenCV : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
*/

#ifndef _IMGZOOM_H_
#define _IMGZOOM_H_


#include <QtCore>
#include <QtGui>
#include <qpoint.h>
#include <qwidget.h>
#include <vector>
#include "CentralArea.hpp"
#include "SoftWindow.hpp"
#include "Mission.hpp"

#if QT_VERSION < 0x0040500
#error You need Qt 4.5 or newer
#endif

class ImgZoom : public QWidget {
  Q_OBJECT

 public:
  explicit ImgZoom(SoftWindow *parent);
  void pan(const QPoint &delta);
  void activateZoom();

 public slots:
  void setImage(std::shared_ptr<Observation> currentObs,
                CentralArea *centralFrame);

 protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *);
  void wheelEvent(QWheelEvent* event);
  void resizeEvent(QResizeEvent*);
  void paintEvent(QPaintEvent*);

  public slots :
  //  void addItem(QString speciesName, QPoint position);
  // void addItem(QString speciesName, QRect box);
  //  void removeItem(int id);

  int convertHeight2Screen(int trueSize);
  int convertWidth2Screen(int trueSize);
  int convertHeight2Real(int screenSize);
  int convertWidth2Real(int screenSize);
  //  void addItem(QPoint _position, Mission::species _species,
  //                       int _itemSize);
  void addItem(Item _item);
  
 protected:
  /*!
   * \brief Coordinate conversion
   *
   * Convert widget related position to pixel position in the image plan
   *
   * \param _screenPoint : relative position in the widget (return by QMouseEvent.pos()
   *
   * \return QPoint : Coordinate in the original image reference plan
   */
  QPoint toRawCoordinate(QPoint _screenPoint);
  
  //! QImage format for the image
  QImage image;

  //! QPixmap being displayed in normal mode
  QPixmap smallPixmap;

  //! Image being displayed in the magnifyig glass
  QPixmap largePixmap;

  QPixmap originalImage;

  //! offset position of the image
  QPoint offset;

  bool pressed;
  bool snapped;
  QPoint pressPos;
  QPoint dragPos;

  //! true if the image is zoomed
  bool zoomed;

  QPixmap zoomPixmap;
  QPixmap maskPixmap;
  float m_zoomCoeff = 0.5;
  SoftWindow *m_parent;
  std::vector<QLabel*> m_item;
  std::vector<QPoint> m_detected;
  //! Used to create bounding boxes for the ground truth
  QRubberBand *m_rubberBand;

  //! Size of the detection
  int m_itemSize = 100;

  //! No idea what's that doing
  QPoint origin;

  //! current obs
  std::shared_ptr<Observation> m_currentObs = 0;
};

#endif  // _IMGZOOM_H_
