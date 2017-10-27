/*
The file DockInfos.cpp is part of the WildlifeCounter software.

WildlifeCounter is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2016 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
- OpenCV : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
*/

#include <vector>
#include <string>
#include <iostream>
#include "DockInfos.hpp"
#include "SoftWindow.hpp"
#include "easylogging++.h"

DockInfos::DockInfos(SoftWindow *parent) {
 

	CLOG(TRACE, "gui_di") << "Initialising dock infos";
  // global parameters
  this->setWindowTitle("Project Information:");
  this->setObjectName("projectInfos");
  this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  this->setFeatures(QDockWidget::DockWidgetMovable);
  this->setMinimumSize(QSize(200, 200));
  this->setStyleSheet("background-color : white;");
  this->setFeatures(QDockWidget::DockWidgetClosable);
  m_parent = parent;
  
  // Main widget and layout
  m_masterWidget = new QFrame;
  QVBoxLayout *masterLayout = new QVBoxLayout;

  // included widgets
  //  QLabel *timeTitle = new QLabel("Time (max 120s) : ");
  //  masterLayout->addWidget(timeTitle); // removed for operational version


  m_showTime = new QProgressBar;
  m_showTime->setValue(0);
  m_showTime->setMinimum(0);
  m_showTime->setMaximum(120);
  // masterLayout->addWidget(m_showTime);  //  removed for operational version

  m_imageNumber = new QLabel("");
  masterLayout->addWidget(m_imageNumber);
  m_imageNumber->setStyleSheet("color : black;");
  //  labelVector.push_back(m_imageNumber);

  // Species count for the project
  m_speciesCountsProject = new QFrame;
  QVBoxLayout *speciesCountProjectLayout = new QVBoxLayout;
  speciesCountProjectLayout->setMargin(0);
  std::vector<QLabel*> projectLabelVector;
  
  QLabel *tempProject = new QLabel();
  QFont font;
  font.setPointSize(32);
  font.setBold(true);
  tempProject->setFont(font);
  tempProject->setText("Counting project results");
  

  projectLabelVector.push_back(tempProject);
  for (size_t i = 0; i < projectLabelVector.size(); i++) {
    speciesCountProjectLayout->addWidget(projectLabelVector[i]);
  }
  m_speciesCountsProject->setLayout(speciesCountProjectLayout);
  masterLayout->addWidget(m_speciesCountsProject);

    // Species count for the folder
  m_speciesCountsFolder = new QFrame;
  QVBoxLayout *speciesCountFolderLayout = new QVBoxLayout;
  speciesCountFolderLayout->setMargin(0);
  std::vector<QLabel*> folderLabelVector;
  
  QLabel *tempFolder = new QLabel("Counting folder results");
  tempFolder->setStyleSheet("color : black;");
  folderLabelVector.push_back(tempFolder);
  for (size_t i = 0; i < folderLabelVector.size(); i++) {
    speciesCountFolderLayout->addWidget(folderLabelVector[i]);
  }
  m_speciesCountsFolder->setLayout(speciesCountFolderLayout);
  masterLayout->addWidget(m_speciesCountsFolder);

  // Species count for the img
  m_speciesCountsImg = new QFrame;
  QVBoxLayout *speciesCountImgLayout = new QVBoxLayout;
  speciesCountImgLayout->setMargin(0);
  std::vector<QLabel*> imgLabelVector;
  
  QLabel *tempImg = new QLabel("Counting image results");
  tempImg->setStyleSheet("color : black;");
  imgLabelVector.push_back(tempImg);
  for (size_t i = 0; i < imgLabelVector.size(); i++) {
    speciesCountImgLayout->addWidget(imgLabelVector[i]);
  }
  m_speciesCountsImg->setLayout(speciesCountImgLayout);
  masterLayout->addWidget(m_speciesCountsImg);

  // m_turtleGame = new QLabel;
  //  m_turtleGame->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
  //  masterLayout->addWidget(m_turtleGame, Qt::AlignTop | Qt::AlignHCenter);

  masterLayout->setAlignment(Qt::AlignTop|Qt::AlignLeft);
  
  // Inclusion des widget dans le masterWidget via le layout
  m_masterWidget->setLayout(masterLayout);

  this->setWidget(m_masterWidget);
  CLOG(TRACE, "gui_di") << "Status : Dock infos initialized";
}
DockInfos::~DockInfos() {}

void DockInfos::actualizeInfos(std::vector<std::string> speciesNames,
                               std::vector<int>  speciesCountProject,
                               std::vector<int>  speciesCountFolder,
                               std::vector<int>  speciesCountImg,
                               int iter, int total) {
	CLOG(TRACE, "gui_di") << "Process : actualize infos";
  // actualize Project :
  delete m_speciesCountsProject->layout();  // reset function
  while (QWidget* w = m_speciesCountsProject->findChild<QWidget*>())
    delete w;

  QVBoxLayout *speciesCountProjectLayout = new QVBoxLayout;
  speciesCountProjectLayout->setMargin(0);
  std::vector<QLabel*> projectLabelVector;

  QLabel *tempProject = new QLabel("Project counts :");
  tempProject->setStyleSheet("color : black;");
  QFont font;
  font.setPointSize(14);
  font.setBold(true);
  tempProject->setFont(font);
  //  tempProject->setText("Counting project results");
  

  projectLabelVector.push_back(tempProject);
  for (size_t k = 0; k < speciesNames.size(); k++) {
    QString name = QString::fromStdString(speciesNames[k]);
    if (!m_parent->getSpecies(name).hide) {
      QLabel *temp = new QLabel(name + " : " +
                                QString::number(speciesCountProject[k]));
      temp->setStyleSheet("color : black;");
      projectLabelVector.push_back(temp);
    }
  }
  for (size_t i = 0; i < projectLabelVector.size(); i++) {
    speciesCountProjectLayout->addWidget(projectLabelVector[i]);
  }
  m_speciesCountsProject->setLayout(speciesCountProjectLayout);

  // actualize folder :
  QString imNumberText = "Image : " + QString::number(iter+1) + "/" +
      QString::number(total);
  m_imageNumber->setText(imNumberText);
  update();

  delete m_speciesCountsFolder->layout();  // reset function
  while (QWidget* w = m_speciesCountsFolder->findChild<QWidget*>())
    delete w;

  QVBoxLayout *speciesCountFolderLayout = new QVBoxLayout;
  speciesCountFolderLayout->setMargin(0);
  std::vector<QLabel*> folderLabelVector;

  QLabel *tempFolder = new QLabel("Folder counts :");
  tempFolder->setStyleSheet("color : black;");
  tempFolder->setFont(font);

  folderLabelVector.push_back(tempFolder);
  for (size_t k = 0; k < speciesNames.size(); k++) {
    QString name = QString::fromStdString(speciesNames[k]);
    if (!m_parent->getSpecies(name).hide) {
      QLabel *temp = new QLabel(name + " : " +
                                QString::number(speciesCountFolder[k]));
      temp->setStyleSheet("color : black;");
      folderLabelVector.push_back(temp);
    }
  }
  for (size_t i = 0; i < folderLabelVector.size(); i++) {
    speciesCountFolderLayout->addWidget(folderLabelVector[i]);
  }
  m_speciesCountsFolder->setLayout(speciesCountFolderLayout);

  // actualize img :
  delete m_speciesCountsImg->layout();  // reset function
  while (QWidget* w = m_speciesCountsImg->findChild<QWidget*>())
    delete w;

  QVBoxLayout *speciesCountImgLayout = new QVBoxLayout;
  speciesCountImgLayout->setMargin(0);
  std::vector<QLabel*> imgLabelVector;

  QLabel *tempImg = new QLabel("Image counts :");
  tempImg->setStyleSheet("color : black;");
  tempImg->setFont(font);
  imgLabelVector.push_back(tempImg);
  for (size_t k = 0; k < speciesNames.size(); k++) {
    QString name = QString::fromStdString(speciesNames[k]);
    if (!m_parent->getSpecies(name).hide) {
      QLabel *temp = new QLabel(name + " : " +
                                QString::number(speciesCountImg[k]));
      temp->setStyleSheet("color : black;");
      imgLabelVector.push_back(temp);
    }
  }
  for (size_t i = 0; i < imgLabelVector.size(); i++) {
    speciesCountImgLayout->addWidget(imgLabelVector[i]);
  }
  m_speciesCountsImg->setLayout(speciesCountImgLayout);
}

void DockInfos::setTime(int time) {
  m_showTime->setValue(time/1000);
}

