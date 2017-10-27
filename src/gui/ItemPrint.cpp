/*
The file ItemPrint.cpp is part of the Harmony software.

Harmony is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2016 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2016/11/08

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
*/

#include <QtWidgets>
#include "ItemPrint.hpp"
#include "Item.hpp"
#include "Mission.hpp"
#include "easylogging++.h"


ItemPrint::ItemPrint(QWidget *_parent) {
	// Definition of the logger
	CLOG(TRACE, "gui_ip") << "Initialising item print class";
  this->setParent(_parent);
  this->drawCircle();
}

ItemPrint::ItemPrint(QWidget *_parent, QColor _color, bool _auto) {
  this->setParent(_parent);
  m_color = _color;
  m_automatic = _auto;
  m_size = this->width();
  if (!_auto) {
    this->drawRectangle();
  } else {
    this->drawCircle();
  }
}

void ItemPrint::drawCircle() {
	CLOG(TRACE, "gui_ip") << "Process : draw circle";
	m_pix = new QPixmap(this->width(), this->height());
  m_pix->fill(Qt::transparent);
  QPainter painter(m_pix);
  QPen pencil(m_color);
  pencil.setWidth(2);
  painter.setPen(pencil);
  painter.drawEllipse(1, 1, this->width() - 2, this->height() - 2);

  this->setPixmap(*m_pix);
  this->setStyleSheet("background-color: transparent;");
}

void ItemPrint::drawRectangle() {
	CLOG(TRACE, "gui_ip") << "Process : draw rectangle";
  m_pix = new QPixmap(this->width(), this->height());
  m_pix->fill(Qt::transparent);
  QPainter painter(m_pix);
  QPen pencil(m_color);
  pencil.setWidth(2);
  painter.setPen(pencil);
  painter.drawRect(1, 1, this->width()-2, this->height()-2);

  this->setPixmap(*m_pix);
  this->setStyleSheet("background-color: transparent;");
}

void ItemPrint::resizeEvent(QResizeEvent*) {
	CLOG(TRACE, "gui_ip") << "Process : resize";
  m_size = this->width();
  if (!m_automatic) {
    this->drawRectangle();
  } else {
    this->drawCircle();
  }
  //update();
}

