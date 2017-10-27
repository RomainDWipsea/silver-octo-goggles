/*
The file DockImages.hpp is part of the WildlifeCounter software.

WildlifeCounter is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2015 WIPSEA, contact@wipsea.com
n
Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
- OpenCV : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
*/

#include <string>
#include "SoftWindow.hpp"
#include "DockImages.hpp"
#include "tools.hpp"

DockImages::DockImages(SoftWindow *parent) {
  // Define global display parameters
  this->setWindowTitle("Images");
  this->setMinimumSize(QSize(200, 400));
  this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  this->setFeatures(QDockWidget::DockWidgetMovable);

  QPalette pal = this->palette();
  pal.setColor(QPalette::Background, QColor(255, 255, 255));
  this->setPalette(pal);
  this->setAutoFillBackground(true);

  QVBoxLayout *dockLayout = new QVBoxLayout;
  QButtonGroup *photoList = new QButtonGroup;

  // you can only check one picture at a time
  photoList->setExclusive(true);
  int nbPhoto = parent->m_mission->getObsNumber();
  for (int j = 0; j < nbPhoto; j++) {
    QPushButton *im = new QPushButton;
    im->setFixedSize(QSize(150, 100));
    std::shared_ptr<Observation> tmp = parent->m_mission->getObs(j).lock();
    
    std::string imagePath = tmp->getFilePath();
    QPixmap *imDock;
  if (imagePath.substr(static_cast<int>(imagePath.size())-3, 3) == "tif") {
      cv::Mat dsm = cv::imread(imagePath);
      imDock = new QPixmap(MatToQPixmap(dsm));

  } else {
    imDock = new QPixmap(QString::fromStdString(imagePath));
  }
  
  //    QPixmap imDock(filePath);
    im->setIcon(imDock->scaled(145, 95,
                              Qt::IgnoreAspectRatio,
                              Qt::FastTransformation));
    im->setIconSize(QSize(145, 95));

    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(5);
    im->setGraphicsEffect(effect);
    photoList->addButton(im, j);

    dockLayout->addWidget(im);
  }
  dockLayout->setSpacing(10);
  QWidget *includingWidget = new QWidget;
  includingWidget->setLayout(dockLayout);
  QScrollArea *menuGauche = new QScrollArea;
  menuGauche->setWidget(includingWidget);
  menuGauche->setAlignment(Qt::AlignHCenter);
  this->setWidget(menuGauche);

  // Connexion
  connect(photoList, SIGNAL(buttonClicked(int)), parent, SLOT(changeMainImage(int)));

}
