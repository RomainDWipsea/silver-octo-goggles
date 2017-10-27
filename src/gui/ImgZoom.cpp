/*
The file ImgZoom.cpp is part of the WildlifeCounter software.

WildlifeCounter is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2015 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
- OpenCV : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
*/

#include <qwidget.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "ImgZoom.hpp"
#include "CentralArea.hpp"
#include "SoftWindow.hpp"
#include "tools.hpp"
#include "Mission.hpp"
#include "Item.hpp"
#include "ItemPrint.hpp"
#include "opencv2/core/core.hpp"
#include "easylogging++.h"
#include "misc.hpp"

//  http://stackoverflow.com/questions/16472543/errors-in-generated-moc-files-for-qt5-from-cmake
#include "moc_ImgZoom.cpp"

ImgZoom::ImgZoom(SoftWindow *parent) : QWidget(0), pressed(false), snapped(false), zoomed(false) {
	CLOG(TRACE, "gui_iz") << "Initialising imgzoom class";
  m_rubberBand = 0;
  m_zoomCoeff = 1.5;
  const QPoint offset(0, 0);
  this->setStyleSheet("background-color : transparent;");
  m_parent = parent;
}

void ImgZoom::setImage(std::shared_ptr<Observation> _currentObs,
                       CentralArea *centralFrame) {
	if (!(_currentObs == m_currentObs))
	{
		m_currentObs = _currentObs;

		// checking for tiff images, to display via opencv
		std::string imagePath = _currentObs->getFilePath();

		if (imagePath.substr(static_cast<int>(imagePath.size()) - 3, 3) == "tif") {
			CLOG(TRACE, "gui_iz") << "Status : image is tiff";
			//cv::Mat dsm = cv::imread(imagePath, CV_LOAD_IMAGE_UNCHANGED);
			cv::Mat dsm = geoTransformAlpha(imagePath);
			image = MatToQImage(dsm);
			smallPixmap = MatToQPixmap(dsm);
			largePixmap = MatToQPixmap(dsm);
		}
		else {
			CLOG(TRACE, "gui_iz") << "Status : image is not tiff";
			image.load(QString::fromStdString(_currentObs->getFilePath()));
			smallPixmap.load(QString::fromStdString(_currentObs->getFilePath()));
		}
		originalImage = smallPixmap;
	}
	CLOG(TRACE, "gui_iz") << "Process : set image=" << _currentObs->getFilePath();
  pressed = false;
  snapped = false;
  zoomed = false;
   //m_currentObs = m_parent->m_currentFolder->getObs(m_parent->m_iter).lock(); to be removed
  
  smallPixmap = originalImage.scaled(centralFrame->m_imageFrame->size(),
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation);

  offset = QPoint(0, 0);
  this->setFixedHeight(centralFrame->m_imageFrame->height());
  this->setFixedWidth(centralFrame->m_imageFrame->width());

  // Clean the new image
  for (size_t n = 0; n < m_item.size(); n++) {
    delete m_item[n];
  }
  m_item.clear();
  m_detected.clear();

  // Load information for the new one
  for (int i = 0; i < _currentObs->getItemCount(); i++) {
    Item* currentItem = _currentObs->getItem(i);
    addItem(*currentItem);
    //      addItem(position, spec, convertWidth2Screen(bbox.width));
  }
}

int ImgZoom::convertHeight2Screen(int trueSize) {
  int res = static_cast<int>(static_cast<float>(trueSize*this->height())/
                             static_cast<float>(image.height()));
  return res;
}

int ImgZoom::convertWidth2Screen(int trueSize) {
  int res = static_cast<int>(static_cast<float>(trueSize*smallPixmap.width())/
                             static_cast<float>(image.width()));
  return res;
}

int ImgZoom::convertWidth2Real(int screenSize) {
  int res = static_cast<int>(static_cast<float>(screenSize*image.width())/
                             static_cast<float>(smallPixmap.width()));
  return res;
}
int ImgZoom::convertHeight2Real(int screenSize) {
  int res = static_cast<int>(static_cast<float>(screenSize*image.height())/
                             static_cast<float>(smallPixmap.height()));
  return res;
}

void ImgZoom::addItem(Item _item) {
	CLOG(TRACE, "gui_iz") << "Process : adding item";
  
  if (!m_parent->getSpecies(QString::fromStdString(_item.getClass())).hide) {
    QColor color = QColor(QString::fromStdString(
        m_parent->getSpecies(QString::fromStdString(_item.getClass())).color));

    ItemPrint *item = new ItemPrint(this, color,
                                    _item.getScores(_item.getClass()).automatic);
  
    int posx((static_cast<float>(_item.getBbox().x) /
              static_cast<float>(image.width()))*smallPixmap.width());
    int posy((static_cast<float>(_item.getBbox().y) /
              static_cast<float>(image.height()))*smallPixmap.height());

    item->setGeometry(posx, posy,
                      convertWidth2Screen(_item.getBbox().width),
                      convertHeight2Screen(_item.getBbox().height));
    cv::Point center = _item.getCenter();
    m_item.push_back(item);
    m_detected.push_back(QPoint(center.x, center.y));
    m_item[m_item.size() - 1]->show();
	//update();
  }
  return;
}

void ImgZoom::pan(const QPoint &delta) {
  offset += delta;
  update();
}

void ImgZoom::activateZoom() {
  zoomed = true;
  update();
}

void ImgZoom::mousePressEvent(QMouseEvent *event) {
  if (m_parent->m_mission != 0) {
    std::shared_ptr<Observation> currentObs =
        m_parent->m_currentFolder->getObs(m_parent->m_iter).lock();
    if (event->button() == Qt::LeftButton &&
        event->type() == QEvent::MouseButtonPress) {
      if (this->childAt(event->x(), event->y()) == 0) {
        if (m_parent->m_speciesTreated != "") {
          QPoint pos = toRawCoordinate(event->pos());
          // Add the new item in the Observation
          Item temp(currentObs->getFilePath(),
                    cv::Point(static_cast<int>(pos.x()),
                                  static_cast<int>(pos.y())),
                    convertWidth2Real(m_itemSize),
                    cs(m_parent->m_speciesTreated), 1);
          m_parent->m_currentFolder->getObs(m_parent->m_iter).lock()
              ->addItem(temp);
		  CLOG(INFO, "gui_iz") << "Action : Add item =" << cs(m_parent->m_speciesTreated) << " at x=" << static_cast<int>(pos.x()) << "; y =" << static_cast<int>(pos.y());
          addItem(temp);
          Json::Value save;
          m_parent->m_mission->writeJsonItem(&save);
          QString fileName = QString::fromStdString(m_parent->m_mission->getFolderPath()) +
              "/" + QString::fromStdString(m_parent->m_mission->getName()) + ".json";
          std::ofstream out(cs(fileName), std::ofstream::out);
          out << save;
          out.close();
          // size, species, score

          // add item to the current image for visualisation
        }
      } else {
        /*There is already an object at the clicked position*/
        for (size_t k = 0; k < m_item.size(); k++) {
          /* We first need to find out which one
             of the previously detected object it is.
          */
          if (m_item[k] == this->childAt(event->x(), event->y())) {
            /* delete the graphical item
             */
            delete m_item[k];
            m_item.erase(m_item.begin() + k);
            std::vector<std::string> speciesNames = currentObs->getItemsName();

            for (int it = 0; it < currentObs->getItemCount(); it++) {
              QPoint center(currentObs->getItem(it)->getCenter().x,
                          currentObs->getItem(it)->getCenter().y);
              if (m_detected[k] == center) {
                m_parent->m_currentFolder->getObs(
                    m_parent->m_iter).lock()->removeItem(static_cast<int>(it));
				CLOG(INFO, "gui_iz") << "Action : Remove item at x=" << center.x() << "; y =" << center.y();
                Json::Value save;
                m_parent->m_mission->writeJsonItem(&save);
                QString fileName = QString::fromStdString(m_parent->m_mission->getFolderPath()) +
                    "/" + QString::fromStdString(m_parent->m_mission->getName()) + ".json";
                std::ofstream out(cs(fileName), std::ofstream::out);
                out << save;
                out.close();
              }
            }
            m_detected.erase(m_detected.begin() + k);
          }
        }
		if (zoomed)
			activateZoom();
      }
      // actualizeInfos
      if (m_parent->m_mission->getName() != "welcome") {
        std::vector<int> *allCount = new std::vector<int>();
        std::vector<int> *folderCount = new std::vector<int>();
        std::vector<int> *imgCount = new std::vector<int>();

        m_parent->getAllSpeciesCount(allCount, folderCount, imgCount);
        m_parent->m_rightDock->actualizeInfos(m_parent->m_mission->getSpeciesNames(),
                                              *allCount, *folderCount, *imgCount, m_parent->m_iter,
                                              m_parent->m_currentFolder->getObsNumber());
        delete allCount;
        delete folderCount;
        delete imgCount;
      }
     // return;
    } 
	if (event->button() == Qt::RightButton) {
    snapped = true;
    pressed =  true;
	dragPos = event->pos(); // pressPos = dragPos = event->pos();
    activateZoom();
    }
  }
  update();
}

void ImgZoom::mouseMoveEvent(QMouseEvent *event) {
  if (zoomed && event->type() == QEvent::MouseMove) {
    dragPos = event->pos();
	LOG(DEBUG) << "Moving dragPos";
    update();
  } else {
    return;
  }
}

void ImgZoom::mouseReleaseEvent(QMouseEvent* event) {
  if ((event->button() == Qt::RightButton) &&
      (event->type() == QEvent::MouseButtonRelease) &&
      zoomed) {
    zoomed = false;
    update();
  }
  return;
}

void ImgZoom::wheelEvent(QWheelEvent* event) {
  //  QPoint numPixels = event->pixelDelta();
  // angleDelta gives measures in (1/8) degrees
  QPoint numDegrees = event->angleDelta() / 8;
  float iw = image.height();
  float tw = this->height();
  float zoomMaxFactor = 2;
  float minZoomFactor = tw/iw;
  if (zoomed) {
      if (numDegrees.y() > 0) {
        m_zoomCoeff = (m_zoomCoeff*2 <= zoomMaxFactor) ? m_zoomCoeff*2 : zoomMaxFactor;
      } else {
        m_zoomCoeff = (m_zoomCoeff/2 >= minZoomFactor) ? m_zoomCoeff/2 : minZoomFactor;
      }
      update();
    return;
  } else {
    if (!(this->childAt(event->x(), event->y()) == 0)) {
      QSize currentSize = this->childAt(event->pos())->size();
      if (numDegrees.y() > 0) {  // we should make it bigger
        m_itemSize = currentSize.width() + 10;
        this->childAt(event->pos())->setGeometry(
            this->childAt(event->pos())->pos().x()-5,
            this->childAt(event->pos())->pos().y()-5,
            m_itemSize,
            m_itemSize);
      } else {  // make it smaller
        if (currentSize.width()-5 > 10) {
          m_itemSize = currentSize.width() - 10;
          this->childAt(event->pos())->setGeometry(
              this->childAt(event->pos())->pos().x()+5,
              this->childAt(event->pos())->pos().y()+5,
              m_itemSize,
              m_itemSize);
        }
      }
      update();
      for (size_t k = 0; k < m_item.size(); k++) {
        /* We first need to find out which one
           of the previously detected object it is.
        */
        if (m_item[k] == this->childAt(event->x(), event->y())) {
          cv::Point temp(m_detected[k].x(), m_detected[k].y());
          m_currentObs->getItem(temp)->setSize(
              convertHeight2Real(m_itemSize), convertHeight2Real(m_itemSize));
          Json::Value save;
          m_parent->saveMission(false);
          /*          m_parent->m_mission->writeJsonItem(&save);
          QString fileName = QString::fromStdString(m_parent->m_mission->getFolderPath()) +
              "/" + QString::fromStdString(m_parent->m_mission->getName()) + ".json";
          std::ofstream out(cs(fileName), std::ofstream::out);
          out << save;
          out.close();*/
        }
      }
    }
  }
}

void ImgZoom::resizeEvent(QResizeEvent*) {
	CLOG(TRACE, "gui_iz") << "Process : resize";
  zoomed = false;
  //update();
}

void ImgZoom::paintEvent(QPaintEvent*) {
	CLOG(TRACE, "gui_iz") << "Process : Paint";
  QPainter p(this);
  p.drawPixmap(offset, smallPixmap);
  p.end();
  if (zoomed) {
    int dim = qMin(width(), height());
    int magnifierSize = dim*m_zoomCoeff/2;  // * 5 / 6;
    int radius = magnifierSize/2;
    //    int radius = 100;
    int ring = radius-15;
    QSize box = QSize(magnifierSize, magnifierSize);
    int newWidth = static_cast<int>(originalImage.width() *m_zoomCoeff);
    int newHeight = static_cast<int>(originalImage.height() *m_zoomCoeff);
    largePixmap = originalImage.scaled(newWidth, newHeight,
                                       Qt::KeepAspectRatioByExpanding,
                                       Qt::FastTransformation);

    
    
    // reupdate our mask
    if (maskPixmap.size() != box) {
      maskPixmap = QPixmap(box);
      maskPixmap.fill(Qt::transparent);

      QRadialGradient g;
      g.setCenter(radius, radius);
      g.setFocalPoint(radius, radius);
      g.setRadius(radius);
      g.setColorAt(1.0, QColor(64, 64, 64, 0));
      g.setColorAt(0.5, QColor(0, 0, 0, 255));

      QPainter mask(&maskPixmap);
      mask.setRenderHint(QPainter::Antialiasing);
      mask.setCompositionMode(QPainter::CompositionMode_Source);
      mask.setBrush(g);
      mask.setPen(Qt::NoPen);
      mask.drawRect(maskPixmap.rect());
      mask.setBrush(QColor(Qt::transparent));
      mask.drawEllipse(g.center(), ring, ring);
      mask.end();
    }

    QPoint center = dragPos;  //- QPoint(0, radius);
    QPoint corner = center - QPoint(radius, radius);

    // QPoint xy = center * m_zoomCoeff - QPoint(radius, radius);  // D'origine
    QPoint xyLarge = center;
    xyLarge.ry() = xyLarge.y()*largePixmap.height() / smallPixmap.height();
    xyLarge.rx() = xyLarge.x()*largePixmap.width() / smallPixmap.width();
    xyLarge -= QPoint(radius, radius);
    //  QPoint xy = center - QPoint(radius, radius);  // Dr Gromain

    // only set the dimension to the magnified portion
    if (zoomPixmap.size() != box)
      zoomPixmap = QPixmap(box);
    if (true) {
      zoomPixmap.fill(Qt::lightGray);
      QPainter p(&zoomPixmap);
      p.translate(-xyLarge);
      p.drawPixmap(offset *2, largePixmap);  // D'origine
      p.end();
    }

    QPainterPath clipPath;
    clipPath.addEllipse(center, ring, ring);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setClipPath(clipPath);
    p.drawPixmap(corner, zoomPixmap);
    p.setClipping(false);
    p.drawPixmap(corner, maskPixmap);
    p.setPen(Qt::gray);
    p.drawPath(clipPath);
  }
}

QPoint ImgZoom::toRawCoordinate(QPoint _screenPoint) {
  float posxRel = static_cast<float>(_screenPoint.x()) /
      static_cast<float>(smallPixmap.width());
  float posyRel = static_cast<float>(_screenPoint.y()) /
      static_cast<float>(smallPixmap.height());
  QPoint pos(posxRel * image.width(), posyRel * image.height());
  return pos;
}
