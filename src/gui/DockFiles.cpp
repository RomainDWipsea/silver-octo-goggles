/*
The file DockFiles.cpp is part of the WildlifeCounter software.

WildlifeCounter is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2016 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2016/08/31/

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
- OpenCV : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
*/

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <vector>
#include <string>
#include <iostream>

#include "DockFiles.hpp"
#include "SoftWindow.hpp"
#include "misc.hpp"
#include "tools.hpp"
#include "easylogging++.h"

#include "moc_DockFiles.cpp"

namespace fs = boost::filesystem;

DockFiles::DockFiles(SoftWindow *parent) {
 

  CLOG(TRACE, "gui_df") << "Initializing DockFile class";
  // global parameters
  m_parent = parent;
  this->setWindowTitle("File system");
  this->setObjectName("projectFiles");
  this->setAllowedAreas(Qt::LeftDockWidgetArea |
                        Qt::RightDockWidgetArea);
  this->setFeatures(QDockWidget::DockWidgetMovable);
  this->setMinimumSize(QSize(200, 200));
  this->setStyleSheet("background-color : white;");
  this->setFeatures(QDockWidget::DockWidgetClosable);

  // Main widget and layout
  m_masterWidget = new QFrame;
  QVBoxLayout *masterLayout = new QVBoxLayout;

  // File system model
  m_model = new QFileSystemModel;
  QString fPath =
      QString::fromStdString(parent->m_mission->getFolderPath()) + "/";
  
  QModelIndex index = m_model->index(fPath);
  m_model->setRootPath(fPath);
  m_model->setFilter(QDir::NoDotAndDotDot |
                     QDir::AllDirs |
                     QDir::Files);
  QStringList extFilter(tr("*.JPG"));
  extFilter << tr("*.PNG");
  extFilter << tr("*.BMP");
  extFilter << tr("*.JIF");
  extFilter << tr("*.TIF");
  m_model->setNameFilters(extFilter);
  m_model->setNameFilterDisables(false);

  /*  QListView *fileView = new QListView;
  QFileSystemModel *fileList = new QFileSystemModel;
  QString currentPath =
      QString::fromStdString(m_parent->m_currentFolder->getFolderPath()) + "/";
  fileList->setRootPath(currentPath);
  fileList->setFilter(QDir::Files);
  fileView->setModel(fileList);
  fileViewView->setViewMode(QListView::IconMode);*/

  // Graphical interface
  QTreeView *tree = new QTreeView(this);
  tree->setModel(m_model);
  // Hiding unwanted informations
  tree->hideColumn(1);
  tree->hideColumn(2);
  tree->hideColumn(3);
  
  //  tree->setViewMode(QListView::IconMode);
  tree->setRootIndex(index);

  connect(tree, SIGNAL(clicked(QModelIndex)),
          this, SLOT(itemClicked(QModelIndex)));

  // Adding to the layout
  masterLayout->addWidget(tree, Qt::AlignTop | Qt::AlignHCenter);
  masterLayout->setAlignment(Qt::AlignTop|Qt::AlignLeft);

  // Add layout to the main widget
  m_masterWidget->setLayout(masterLayout);

  this->setWidget(m_masterWidget);
  CLOG(TRACE, "gui_df") << "Dockfile initialized";
}
DockFiles::~DockFiles() {}

void DockFiles::itemClicked(QModelIndex const &index) {
  // Check wether it is a file or a folder
  std::string fileName = cs(m_model->filePath(index));
  CLOG(INFO, "gui_df") << "Action : clicked on item=" << fileName;
  if (m_model->isDir(index)) {  // Deal with directories
    bool found(false);
    std::weak_ptr<Mission> subFolder =
        m_parent->m_mission->getSubFolder(fileName, found).lock();

    if (found) {
      m_parent->m_currentFolder = subFolder.lock();
      m_parent->changeMainImage(static_cast<int>(0));
    }
    return;

  } else {    // Deal with files
    // If file is a correct image, plot in center
    int iterMission = 0;
    bool found(false);
    m_parent->m_mission->isImgIn(fileName, iterMission, found);
    //    m_parent->m_currentFolder.reset();
    if (iterMission == 0) {
      m_parent->m_currentFolder = m_parent->m_mission;
    } else {
      fs::path chemin(fileName);
      chemin.remove_filename();
      bool found(false);
      std::string folderPath = boost::filesystem::canonical(chemin).generic_string();
      std::weak_ptr<Mission> subFolder =
          m_parent->m_mission->getSubFolder(folderPath, found);
      if (found) {
        m_parent->m_currentFolder = subFolder.lock();
        m_parent->changeMainImage(static_cast<int>(0));
      }
    }
    m_parent->changeMainImage(fileName);
  }
}
