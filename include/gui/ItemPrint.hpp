/*
The file ItemPrint.hpp is part of the Harmony software.

Harmony is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2016 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2016/11/08

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
*/

#ifndef _ITEM_PRINT_H_
#define _ITEM_PRINT_H_

#include <QWidget>
#include <QtCore>
#include <QLabel>
#include "Item.hpp"
#include "SoftWindow.hpp"

class ItemPrint : public QLabel {
 public:
  /*!
   * \brief Class constructor
   *
   * Construct a default item
   *
   * \param 
   *
   * \return 
   */
  ItemPrint(QWidget *_parent);

  /*!
   * \brief Class constructor
   *
   * Item with given color and size 
   *
   * \param 
   *
   * \return 
   */
  explicit ItemPrint(QWidget *_parent, QColor _color, bool _auto);

 public slots:
  /*!
   * \brief draw a circlepixmap
   *
   * Draw a circle with transparent background on the widget, attributes are used to
   * configure the circle parameter
   */
  void drawCircle();

  /*!
   * \brief draw a rectangle pixmap
   *
   * Draw a rectangle with transparent background on the widget, attributes are used to
   * configure the rectangle paramters
   */
  void drawRectangle();

  /*!
   * \brief handle resizeEvent
   *
   * When the widget is resized, the circle size should be adjusted and repainted
   */
  void resizeEvent(QResizeEvent*);
  
 protected:
  //! Representative image
  QPixmap *m_pix = 0;
  
  //! color of the item
  QColor m_color = QColor(Qt::black);

  //! size of the item
  int m_size = 100;

  //! Relative position in the parent widget
  QPoint m_position = QPoint(0, 0);

  //! remember if it's automatic or manual detection
  bool m_automatic;
};

#endif  // _ITEM_PRINT_H_
