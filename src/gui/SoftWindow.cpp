/*
The file SoftWindow.cpp is part of the WildlifeCounter software.

WildlifeCounter is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2017 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
- OpenCV 3.0.0 : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
- Caffe : BSD 2-Clause license => https://opensource.org/licenses/bsd-2-clause
- Boost : custom license => http://www.boost.org/users/license.html
n*/

#include <QApplication>
#include <QMenuBar>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "SoftWindow.hpp"
#include "Mission.hpp"
#include "Observation.hpp"
#include "tools.hpp"
#include "misc.hpp"
#include "ImgZoom.hpp"
#include "DockInfos.hpp"
#include "DockImages.hpp"
#include "DockFiles.hpp"
#include "ModifySpecies.hpp"
#include "QtCore"

#include "moc_SoftWindow.cpp"
#include "easylogging++.h"
#include "wipro_config.h"

//INITIALIZE_EASYLOGGINGPP

SoftWindow::SoftWindow() {
  this->setWindowTitle("Harmony");
  m_iconPath = qApp->applicationDirPath() + "/../../images/";
  setWindowIcon(QIcon(m_iconPath+"iconeHarmony.png"));

  // initialize logger
  initLogger();
  // Initialize actions
  initActions();

  // Menu bar defition
  initMenus();

  // Toolbar definition
  initToolBar();
  // create a fake mission for the welcome images
  m_mission = std::make_shared<Mission>(cs("welcome"));
  QString welcomePath = qApp->applicationDirPath() + "/../../images/welcome/";
  m_mission->addFolder(cs(welcomePath));
  m_currentFolder = m_mission;

  //  Central area definition
  m_centralArea = new CentralArea(this);
  setCentralWidget(m_centralArea);

  m_speciesGroup = new QActionGroup(this);
  connect(m_speciesGroup, SIGNAL(triggered(QAction*)),
          this, SLOT(selectSpecies()));

  // connexions
  connect(this, SIGNAL(iterchanged()),
          this, SLOT(actualizeMainImage()));

  this->showMaximized();
  CLOG(INFO, "gui_sw") << "All initialization done, Harmony is ready";
}
SoftWindow::~SoftWindow() {}

void SoftWindow::initLogger() {
  // Load configuration from file
  el::Configurations conf(cs(qApp->applicationDirPath() + "/my_conf.conf"));
  // el::Loggers::addFlag(el::LoggingFlag::AutoSpacing);
  el::Loggers::addFlag(el::LoggingFlag::LogDetailedCrashReason);
  el::Loggers::setVerboseLevel(1);
	
  // setting default configuration for all loggers
  el::Loggers::setDefaultConfigurations(conf, true);

  // Definition of the loggers
  el::Logger* gui_sw = el::Loggers::getLogger("gui_sw");
  el::Logger* cor_mi = el::Loggers::getLogger("cor_mi");
  el::Logger* cor_ob = el::Loggers::getLogger("cor_ob");
  el::Logger* cor_it = el::Loggers::getLogger("cor_it");
  el::Logger* cor_al = el::Loggers::getLogger("cor_al");
  el::Logger* cor_cl = el::Loggers::getLogger("cor_cl");
  el::Logger* gui_ca = el::Loggers::getLogger("gui_ca");
  el::Logger* gui_iz = el::Loggers::getLogger("gui_iz");
  el::Logger* gui_ip = el::Loggers::getLogger("gui_ip");
  el::Logger* gui_di = el::Loggers::getLogger("gui_di");
  el::Logger* gui_df = el::Loggers::getLogger("gui_df");
  el::Logger* gui_ms = el::Loggers::getLogger("gui_ms");
  el::Logger* ser_wp = el::Loggers::getLogger("ser_wp");

  // to be removed : just because of the -Werror unused variables
  if (gui_sw)
    std::cout << "sbra";
  if (gui_ca)
    std::cout << "sbra";
  if (gui_iz)
    std::cout << "sbra";
  if (gui_ip)
    std::cout << "sbra";
  if (gui_di)
    std::cout << "sbra";
  if (gui_df)
    std::cout << "sbra";
  if (gui_ms)
    std::cout << "sbra";
  if (ser_wp)
    std::cout << "sbra";
  if (cor_mi)
    std::cout << "sbra";
  if (cor_ob)
    std::cout << "sbra";
  if (cor_it)
    std::cout << "sbra";
  if (cor_al)
    std::cout << "sbra";
  if (cor_cl)
    std::cout << "sbra";
  // Configuring the performance logger and initialize
  el::Configurations perf;
  perf.setToDefault();
  perf.parseFromText("*GLOBAL:\n  FILENAME             =  ""./logs/myPerf.log""");
  el::Loggers::reconfigureLogger("performance", perf);
  CLOG(INFO, "performance") << "----------------------------------------------------------------";
  CLOG(INFO, "performance") << "-                     NEW SESSION                              -";
  CLOG(INFO, "performance") << "----------------------------------------------------------------";

	// initial log
	CLOG(INFO,"gui_sw") << "----------------------------------------------------------------";
	CLOG(INFO,"gui_sw") << "-                     NEW SESSION                              -";
	CLOG(INFO,"gui_sw") << "----------------------------------------------------------------";
	CLOG(INFO,"gui_sw") << "Logger configured, starting Harmony";
	CLOG(INFO,"gui_sw") << "----------------------System Infos------------------------------";
	CLOG(INFO,"gui_sw") << "Operating system : " << cs(QSysInfo::productType()) << " " << cs(QSysInfo::productVersion());
	CLOG(INFO,"gui_sw") << "Architecture : " << QSysInfo::WordSize << " bits";
	CLOG(INFO,"gui_sw") << "Build CPU Architecture : " << cs(QSysInfo::buildCpuArchitecture());
	CLOG(INFO,"gui_sw") << "Current CPU Architecture : " << cs(QSysInfo::currentCpuArchitecture());
	CLOG(INFO,"gui_sw") << "Kernel type : " << cs(QSysInfo::kernelType());
	CLOG(INFO,"gui_sw") << "Kernel version : " << cs(QSysInfo::kernelVersion());
	CLOG(INFO,"gui_sw") << "---------------------Harmony config-----------------------------";
	CLOG(INFO,"gui_sw") << "Harmony major version : " << Harmony_VERSION_MAJOR;
	CLOG(INFO,"gui_sw") << "Harmony minor version : " << Harmony_VERSION_MINOR;
	CLOG(INFO,"gui_sw") << "Harmony patch version : " << Harmony_VERSION_PATCH;
	CLOG(INFO,"gui_sw") << "Build date : " << __DATE__ << "-" << __TIME__;
#ifdef USE_SERVER 
	CLOG(INFO,"gui_sw") << "+++++++++++++++++++++++++++";
	CLOG(INFO,"gui_sw") << "USE_SERVER = " << USE_SERVER; 
	CLOG(INFO,"gui_sw") << "WPS_version = 2.1.0";
#endif
#ifdef USE_ALGO 
	CLOG(INFO,"gui_sw") << "+++++++++++++++++++++++++++";
	CLOG(INFO,"gui_sw") << "USE_ALGO = " << USE_ALGO;
	CLOG(INFO,"gui_sw") << "Caffe commit on windows branch  =  88ddc95e5f1c4578ad888190627544b2a91a953d";
#endif
#ifdef USE_ALGO
	CLOG(INFO,"gui_sw") << "+++++++++++++++++++++++++++";
	CLOG(INFO,"gui_sw") << "USE_ALGO = " << USE_ALGO;
#endif
#ifdef USE_RESEARCH
	CLOG(INFO,"gui_sw") << "+++++++++++++++++++++++++++";
	CLOG(INFO,"gui_sw") << "USE_RESEARCH = " << USE_RESEARCH;
#endif
#ifdef USE_TREES
	CLOG(INFO,"gui_sw") << "+++++++++++++++++++++++++++";
	CLOG(INFO,"gui_sw") << "USE_TREES = " << USE_TREES;
#endif
	CLOG(INFO,"gui_sw") << "********************Starting session****************************";
}

void SoftWindow::initActions() {
  // // TIMED_FUNC(initActionsTimer);
  m_actions["exit"] = new QAction("&Exit", this);
  m_actions["exit"]->setShortcut(QKeySequence("Ctrl+Q"));
  m_actions["exit"]->setIcon(QIcon(m_iconPath + "exit.png"));
  connect(m_actions["exit"], SIGNAL(triggered(bool)),
          this, SLOT(Exit()));

  m_actions["newProject"] = new QAction("&New project", this);
  m_actions["newProject"]->setShortcut(QKeySequence("Ctrl+N"));
  m_actions["newProject"]->setIcon(QIcon(m_iconPath + "edit.png"));
  connect(m_actions["newProject"], SIGNAL(triggered(bool)),
          this, SLOT(createNewMission()));

  m_actions["saveProject"] = new QAction("&Save project", this);
  m_actions["saveProject"]->setShortcut(QKeySequence("Ctrl+S"));
  m_actions["saveProject"]->setIcon(QIcon(m_iconPath + "save.png"));
  m_actions["saveProject"]->setEnabled(false);
  connect(m_actions["saveProject"], SIGNAL(triggered(bool)),
          this, SLOT(saveMission()));

  m_actions["saveAs"] = new QAction("&Save project as", this);
  m_actions["saveAs"]->setEnabled(false);
  connect(m_actions["saveAs"], SIGNAL(triggered(bool)),
          this, SLOT(saveMissionAs()));

  m_actions["load"] = new QAction("Load", this);
  m_actions["load"]->setIcon(QIcon(m_iconPath + "open.png"));
  connect(m_actions["load"], SIGNAL(triggered(bool)),
          this, SLOT(loadMission()));

 // m_actions["showFiles"] = m_rightDock->toggleViewAction();
  //connect(m_actions["showFiles"], SIGNAL(triggered(bool)),
	 // this, SLOT(showFiles()));

  m_actions["addSpecies"] = new QAction("Add object", this);
  m_actions["addSpecies"]->setIcon(QIcon(m_iconPath + "plus.png"));
  m_actions["addSpecies"]->setEnabled(false);
  connect(m_actions["addSpecies"], SIGNAL(triggered(bool)),
          this, SLOT(addSpecies()));

  m_actions["editSpecies"] = new QAction("Edit object", this);
  m_actions["editSpecies"]->setIcon(QIcon(m_iconPath + "editSpecies.png"));
  m_actions["editSpecies"]->setEnabled(false);
  connect(m_actions["editSpecies"], SIGNAL(triggered(bool)),
          this, SLOT(editSpecies()));

  m_actions["removeSpecies"] = new QAction("Remove object", this);
  m_actions["removeSpecies"]->setIcon(QIcon(m_iconPath + "moins.png"));
  m_actions["removeSpecies"]->setEnabled(false);
  connect(m_actions["removeSpecies"], SIGNAL(triggered(bool)),
          this, SLOT(removeSpecies()));

  m_actions["clearObs"] = new QAction("Clear image detections", this);
  m_actions["clearObs"]->setIcon(QIcon(m_iconPath + "Clear_icon.png"));
  m_actions["clearObs"]->setEnabled(false);
  connect(m_actions["clearObs"], SIGNAL(triggered(bool)),
          this, SLOT(clearCurrentObs()));

  m_actions["documentation"] = new QAction("Documentation", this);
  m_actions["documentation"]->setIcon(QIcon(m_iconPath + "manual.png"));
  connect(m_actions["documentation"], SIGNAL(triggered(bool)),
          this, SLOT(openUserManual()));

  m_actions["aboutWipsea"] = new QAction("About WIPSEA", this);
  m_actions["aboutWipsea"]->setIcon(QIcon(m_iconPath + "logo.png"));
  connect(m_actions["aboutWipsea"], SIGNAL(triggered(bool)),
          this, SLOT(aboutWipsea()));

  m_actions["aboutHarmony"] = new QAction("About Harmony", this);
  m_actions["aboutHarmony"]->setIcon(QIcon(m_iconPath + "iconeHarmony.png"));
  connect(m_actions["aboutHarmony"], SIGNAL(triggered(bool)),
          this, SLOT(aboutHarmony()));
}

void SoftWindow::initMenus() {
 // TIMED_FUNC(initMenusTimer);
  m_fileMenu = menuBar()->addMenu("&File");
  m_fileMenu->addAction(m_actions["newProject"]);
  m_fileMenu->addAction(m_actions["load"]);
  m_fileMenu->addAction(m_actions["saveProject"]);
  m_fileMenu->addAction(m_actions["saveAs"]);
  m_fileMenu->addAction(m_actions["exit"]);

  m_editionMenu = menuBar()->addMenu("&Edition");
  m_editionMenu->addAction(m_actions["addSpecies"]);
  m_editionMenu->addAction(m_actions["editSpecies"]);
  m_editionMenu->addAction(m_actions["removeSpecies"]);
  m_editionMenu->addAction(m_actions["clearObs"]);

  m_speciesGroup = new QActionGroup(this);

  m_aboutMenu = menuBar()->addMenu("&About");
  m_aboutMenu->addAction(m_actions["documentation"]);
  m_aboutMenu->addAction(m_actions["aboutHarmony"]);
  m_aboutMenu->addAction(m_actions["aboutWipsea"]);
}

void SoftWindow::initToolBar() {
 // TIMED_FUNC(initToolBarTimer);
  m_toolBarMain = addToolBar("MainActions");
  m_toolBarMain->addAction(m_actions["newProject"]);
  m_toolBarMain->addAction(m_actions["load"]);
  m_toolBarMain->addAction(m_actions["saveProject"]);
  m_toolBarMain->addAction(m_actions["exit"]);

  m_toolBarMain->addSeparator();
  
  m_toolBarMain->addAction(m_actions["addSpecies"]);
  m_toolBarMain->addAction(m_actions["editSpecies"]);
  m_toolBarMain->addAction(m_actions["removeSpecies"]);

  m_toolBarMain->addSeparator();
  m_toolBarMain->addAction(m_actions["clearObs"]);

  m_subToolbar = new QToolBar("Species");
  m_subToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  m_toolBarMain->addWidget(m_subToolbar);

  // Just to move m_currentImage to the right end of the tool bar
  QWidget* spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_spacer = m_toolBarMain->addWidget(spacer);

  // path input window
  m_imageTreatedName = "";
  m_currentImage = new QLineEdit(m_imageTreatedName, m_toolBarMain);
  m_currentImage->setMaximumWidth(500);
  m_currentImage->setMinimumWidth(300);
  QFont f("Verdana", 12, QFont::DemiBold);
  m_currentImage->setFont(f);
  m_toolBarMain->addWidget(m_currentImage);
  connect(m_currentImage, SIGNAL(returnPressed()),
          this, SLOT(changeMainImage()));
}

// Core functions
void SoftWindow::resetWorkspace() {
  CLOG(INFO, "gui_sw") << "Process : reset workspace";
  for (size_t i = 0; i < m_species.size(); i++) {
    m_speciesGroup->removeAction(m_species[i]);
    delete m_species[i];
  }
  m_species.clear();
  m_subToolbar->clear();
  m_speciesTreated = "";

  if (m_rightDock != 0) {
    removeDockWidget(m_rightDock);
    delete m_rightDock;
    m_rightDock = 0;
  }
  if (m_leftDock != 0) {
    removeDockWidget(m_leftDock);
    delete m_leftDock;
    m_leftDock = 0;
  }
}

void SoftWindow::createNewMission(std::string _missionName, std::string _folder) {
  CLOG(TRACE, "gui_sw") << "Process = create new Mission";
  CLOG(INFO, "gui_sw") << "Data : missionName="
                       << _missionName << "; folder=" << _folder;
  QString currentFolder = "/";
  if (m_mission->getName() != "welcome") {
    saveMission(true);
    resetWorkspace();
  }
  m_mission = std::make_shared<Mission>(_missionName);
  m_mission->addFolder(_folder);

  CLOG(INFO, "gui_sw")
      << "Data : Number of subfolders=" << m_mission->getSubFolderCount();
  CLOG(INFO, "gui_sw")
      << "Data : Number of image =" << m_mission->getObsNumber();
  if (m_mission->getObsNumber() <= 0) {
    for (int k = 0; k < m_mission->getSubFolderCount(); k++) {
      if (m_mission->getSubFolder(k).lock()->getObsNumber() > 0) {
        m_currentFolder = m_mission->getSubFolder(k).lock();
        break;
      }
    }
  } else {
    m_currentFolder = m_mission;
  }
  CLOG(INFO, "gui_sw")
      << "Data : current folder path=" << m_currentFolder->getFolderPath();

  if (m_rightDock != 0)
    delete m_rightDock;
  m_rightDock = new DockInfos(this);
  addDockWidget(Qt::LeftDockWidgetArea, m_rightDock);
  m_actions["showInfo"] = m_rightDock->toggleViewAction();
  m_editionMenu->addAction(m_actions["showInfo"]);

  if (m_leftDock != 0)
    delete m_leftDock;
  m_leftDock = new DockFiles(this);
  addDockWidget(Qt::RightDockWidgetArea, m_leftDock);
  m_actions["showFiles"] = m_leftDock->toggleViewAction();
  m_editionMenu->addAction(m_actions["showFiles"]);

  m_iter = 0;
  emit iterchanged();
  m_actions["addSpecies"]->setEnabled(true);
  m_actions["detectTurtle"]->setEnabled(true);
  m_actions["saveProject"]->setEnabled(true);
  m_actions["saveAs"]->setEnabled(true);

  CLOG(TRACE, "gui_sw") << "Process : save mission";
  Json::Value save;
  m_mission->writeJsonItem(&save);
  QString fileName = QString::fromStdString(m_mission->getFolderPath()) +
      "/" + QString::fromStdString(m_mission->getName()) + ".json";
  std::ofstream out(cs(fileName), std::ofstream::out);
  out << save;
  out.close();
}

void SoftWindow::createNewMission() {
  CLOG(INFO,"gui_sw") << "Action : Create new misison";
  QString currentFolder = "/";
  if (m_mission->getName() != "welcome") {
    currentFolder = QString::fromStdString(m_mission->getFolderPath());
  }

#ifdef WINDOWS
  QString folder = QFileDialog::getExistingDirectory(this,
                                                     "Project root folder",
                                                     currentFolder);
#else
  QString folder = QFileDialog::getExistingDirectory(this,
                                                     "Project root folder",
                                                     currentFolder,
                                                     QFileDialog::DontUseNativeDialog);
#endif
  QString missionName;
  if (!folder.isEmpty()) {
    bool ok;
    do {
      QString message = "Please enter the project name, \n"
          "it will aslo be used for automatic saving of your progress : ";
      missionName = QInputDialog::getText(this,
                                          "Start new project",
                                          message, QLineEdit::Normal,
                                          QString(), &ok);
      if (!ok)
        return;
      if (missionName == "" && ok) {
        QMessageBox::warning(this, "Empty project name",
                             "The project name cannot be empty, please chose one");
      }
    } while (!ok);
    createNewMission(cs(missionName), cs(folder));
  }
}

void SoftWindow::saveMission(bool _print) {
  //	TIMED_FUNC(saveMissionTimer);
	CLOG(TRACE,"gui_sw") << "Process : Save mission";
  Json::Value save;
  m_mission->writeJsonItem(&save);
  QString file = QString::fromStdString(m_mission->getName());

  QString fileName = QString::fromStdString(m_mission->getFolderPath()) +
      "/" + file + ".json";
  std::ofstream out(cs(fileName), std::ofstream::out);
  out << save;
  out.close();
  if (_print) {
    QMessageBox::information(this,
                             "Information",
                             "The current project has successfully been saved in "
                             + QString::fromStdString(m_mission->getName()) +
                             ".json");
    QString message = "Warning \n You won't be able to load"
        " the project if you move the images from their current folder.";
    QMessageBox::warning(this, "Warning", message);
  }
}

void SoftWindow::saveMissionAs() {
	CLOG(INFO,"gui_sw") << "Action : Save mission as";
  Json::Value save;
  m_mission->writeJsonItem(&save);
  QString file;
  bool ok;
  do {
    file = QInputDialog::getText(this,
                                 "File name",
                                 "Please enter file name");
    std::string test = cs(file) + ".json";
    if (file == QString::fromStdString(m_mission->getName()) ||
        file_exist(m_mission->getFolderPath() + "/" + test)) {
        QString message = "This name is already taken by another project.\n "
            "Please choose another one.";
        QMessageBox::warning(this, "Wrong project name", message);
        ok = false;
    } else {
      ok = true;
    }
  } while (!ok);
  if (!file.isNull()) {
    QString fileName = QString::fromStdString(m_mission->getFolderPath()) +
        "/" + file + ".json";
    std::ofstream out(cs(fileName), std::ofstream::out);
    out << save;
    out.close();

    // Change the target for automatic saving.
    m_mission->setName(cs(file));

    QMessageBox::information(this,
                             "Information",
                             "The current project has successfully been saved in "
                             + QString::fromStdString(m_mission->getName()) +
                             ".json");
    QString message = "Warning \n You won't be able to load"
        " the project if you move the images from their current folder.";

    QMessageBox::warning(this, "Back up condition", message);
  } else {
    QString message = "The file name can't be empty. "
        "The project was not saved, please retry.";

    QMessageBox::warning(this, "Error in saving", message);
  }
}

void SoftWindow::loadMission(std::shared_ptr<Mission> _mission) {
 // TIMED_FUNC(loadMissionTimer);
  CLOG(TRACE,"gui_sw") << "Process : Load Misison";
  CLOG(INFO,"gui_sw") << "Data : loaded mission =" << _mission->getName();
  // Clean previous loaded project
  if (m_mission->getName() != "welcome") {
    resetWorkspace();
  }
  m_currentFolder.reset();
  m_mission.reset();
  
  m_mission = _mission;
  if (m_mission->getObsNumber() <= 0) {
    for (int k = 0; k < m_mission->getSubFolderCount(); k++) {
      if (m_mission->getSubFolder(k).lock()->getObsNumber() > 0) {
        m_currentFolder = m_mission->getSubFolder(k).lock();
        break;
      }
    }
  }
  else {
    m_currentFolder = m_mission;
  }

  // Adapt the interface to the new project
  m_actions["addSpecies"]->setEnabled(true);
  m_actions["detectTurtle"]->setEnabled(true);
  m_actions["saveProject"]->setEnabled(true);
  m_actions["saveAs"]->setEnabled(true);
  std::vector<std::string> speciesNames =
      m_mission->getSpeciesNames();

  delete m_rightDock;
  m_rightDock = new DockInfos(this);
  addDockWidget(Qt::LeftDockWidgetArea, m_rightDock);
  m_actions["showInfo"] = m_rightDock->toggleViewAction();
  m_editionMenu->addAction(m_actions["showInfo"]);
  // create a dock to plot images
  //    m_leftDock = new DockImages(this);
  delete m_leftDock;
  m_leftDock = new DockFiles(this);
  addDockWidget(Qt::RightDockWidgetArea, m_leftDock);
  m_actions["showFiles"] = m_leftDock->toggleViewAction();
  m_editionMenu->addAction(m_actions["showFiles"]);

  for (size_t i = 0; i < speciesNames.size(); i++) {
    if (!m_mission->getSpecies(speciesNames[i]).hide) {
      addSpeciesButton(getSpecies(QString::fromStdString(speciesNames[i])));
      if (m_mission->getSpecies(speciesNames[i]).name == "Turtle") {
        #ifdef USE_ALGO
        m_turtleThreshold = createOptionsSlider(0, 100, 50, 1);
        connect(m_turtleThreshold, SIGNAL(sliderReleased()),
                this, SLOT(setThreshold()));
        m_turtleSpin = createOptionsSpinBox(0, 100, 50);
        connect(m_turtleSpin, SIGNAL(valueChanged(int)),
                m_turtleThreshold, SLOT(setValue(int)));
        connect(m_turtleThreshold, SIGNAL(valueChanged(int)),
                m_turtleSpin, SLOT(setValue(int)));
        
        connect(m_turtleSpin, SIGNAL(editingFinished()),
                this, SLOT(setThreshold()));

        m_threshAction = m_subToolbar->addWidget(m_turtleThreshold);
        m_spinAction = m_subToolbar->addWidget(m_turtleSpin);
#endif  // USE_ALGO
      }
    }
  }
  if (m_species.size() > 0) {
    m_speciesTreated = m_species[0]->text();
    m_species[0]->trigger();
  }
  // update main image and prepare to count
  m_iter = 0;
  emit iterchanged();
  
  // actualizeInfos
  std::vector<int> *allCount = new std::vector<int>();
  std::vector<int> *folderCount = new std::vector<int>();
  std::vector<int> *imgCount = new std::vector<int>();
  
  getAllSpeciesCount(allCount, folderCount, imgCount);
  m_rightDock->actualizeInfos(m_mission->getSpeciesNames(),
                              *allCount, *folderCount, *imgCount, m_iter,
                              m_currentFolder->getObsNumber());
  delete allCount;
  delete folderCount;
  delete imgCount;
}

void SoftWindow::loadMission(QString fileName) {
  // Read the new project from the json file
	CLOG(TRACE,"gui_sw") << "Process : load mission from filename";
	CLOG(INFO,"gui_sw") << "Data : filename =" << cs(fileName);
  std::shared_ptr<Mission> input = std::make_shared<Mission>();
  std::ifstream in(cs(fileName));
  Json::Value reading;
  in >> reading;
  input->readJsonItem(reading);
  in.close();
  loadMission(input);
}

void SoftWindow::loadMission() {
	CLOG(INFO,"gui_sw") << "Action : Load mission";
  QString currentFolder = "/";
  if (m_mission->getName() != "welcome") {
    saveMission(true);
    
    currentFolder = QString::fromStdString(m_mission->getFolderPath());
  }
  QString missionJsonFile =
      QFileDialog::getOpenFileName(this,
                                   "Open the project json file",
                                   currentFolder, "*.json",
                                   Q_NULLPTR,
                                   QFileDialog::DontUseNativeDialog);
  if (!missionJsonFile.isNull()) {
    QMessageBox msgBox;
    msgBox.setText("This program uses automatic saving.");
    msgBox.setInformativeText("Do you want to overwrite existing file?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    QString missionName;
    QMessageBox cancelBox;
    bool ok;
    switch (ret) {
      case QMessageBox::Yes :
        loadMission(missionJsonFile);
        break;
      case QMessageBox::No :
        do {
          QString message = "Please enter a new file name to save your project";
          missionName = QInputDialog::getText(this,
                                              "New project name",
                                              message, QLineEdit::Normal,
                                              QString(), &ok);
          if (missionName == "" && ok) {
            QMessageBox::warning(this, "Empty project name",
                                 "The project name cannot be empty, please choose one");
          }
        } while ((missionName == "" || missionName.isNull()) && ok );
        if (ok) {
          loadMission(missionJsonFile);
		  m_mission->setName(cs(missionName));
        } else {
          cancelBox.setText("Operation canceled");
          cancelBox.exec();
        }
        break;
      case QMessageBox::Cancel :
        cancelBox.setText("Operation canceled");
        cancelBox.exec();
        break;
      default :
        cancelBox.setText("Operation canceled");
        cancelBox.exec();
        break;
    }
  }
}

void SoftWindow::addSpeciesButton(Mission::species _spec) {
	CLOG(TRACE,"gui_sw") << "Process : add species button =" << _spec.name;
  int last = m_species.size();
  m_species.push_back(new QAction(QString::fromStdString(_spec.name), this));
  m_species[last]->setCheckable(true);
  QPixmap pix(10, 10);
  pix.fill(QColor(QString::fromStdString(_spec.color)));
  m_species[last]->setIcon(QIcon(pix));
  m_subToolbar->addAction(m_species[last]);
  m_speciesGroup->addAction(m_species[last]);

  m_actions["removeSpecies"]->setEnabled(true);
  m_actions["editSpecies"]->setEnabled(true);
  m_actions["clearObs"]->setEnabled(true);

  // pre-select the new species
  m_species[last]->setChecked(true);
  m_speciesTreated = m_species[last]->text();
}

void SoftWindow::removeSpeciesButton(Mission::species _spec) {
	CLOG(TRACE,"gui_sw") << "Process : remove species button=" << _spec.name;
  for (size_t i = 0; i < m_species.size(); i++) {
    if (cs(m_species[i]->text()) == _spec.name) {
      m_speciesGroup->removeAction(m_species[i]);
      delete m_species[i];
    }
  }
}

void SoftWindow::addSpecies(QString _species, QString _color) {
	CLOG(TRACE,"gui_sw") << "Process : add species =" << cs(_species) << "; color=" << cs(_color);
  m_mission->addSpecies(cs(_species), cs(_color));

  // manage the buttons
  addSpeciesButton(getSpecies(_species));
  //  m_speciesTreated = _species;

  // actualizeInfos
  std::vector<int> *allCount = new std::vector<int>();
  std::vector<int> *folderCount = new std::vector<int>();
  std::vector<int> *imgCount = new std::vector<int>();
  
  getAllSpeciesCount(allCount, folderCount, imgCount);
  m_rightDock->actualizeInfos(m_mission->getSpeciesNames(),
                              *allCount, *folderCount, *imgCount, m_iter,
                              m_currentFolder->getObsNumber());
  delete allCount;
  delete folderCount;
  delete imgCount;
}

void SoftWindow::addSpecies() {
	CLOG(INFO,"gui_sw") << "Action : Add species";
  QString question = "Please enter the object name :";
  QString windowTitle = "Add a new object class";
  QString speciesName = QInputDialog::getText(this, windowTitle, question);
  bool usedName = false;
  for (size_t i = 0; i < m_species.size(); i++) {
    if (m_species[i]->text() == speciesName) {
      usedName = true;
      break;
    }
  }
  if (!speciesName.isNull()) {
    if (speciesName.contains("Auto", Qt::CaseInsensitive) || usedName) {
      QString message =  "This object name is not valid. \n "
         "It may be an already existing object or a reserved name \n"
          "Please try again with another name for your object.";
      QMessageBox::warning(this, "Error", message);
	  LOG(WARNING) << "Wrong name : user entered a name containing ""Auto"" =" << cs(speciesName);
    } else if (isStringIn(cs(speciesName), m_mission->getSpeciesNames())) { // && !(speciesName == "Turtle")
		QString message = "Sorry, the project already contains an object with that name \n"
			"Please try again with another name for your object.";
		QMessageBox::warning(this, "Error", message);
		LOG(WARNING) << "Wrong name : user entered an existing name =" << cs(speciesName);
      
	} else if (speciesName == "Turtle") {
		QString message = "Sorry, this name is reserved for the algorithm, \n"
			"Please try again with another name for your object.";
		QMessageBox::warning(this, "Error", message);
		LOG(WARNING) << "Wrong name : user tried ""Turtle"" as a species name";
	} else	{
		addSpecies(speciesName);
		choseSpeciesColor(speciesName);
		CLOG(INFO,"gui_sw") << "Data : added species=" << cs(speciesName);
	}
	return;
  }
  LOG(WARNING) << "Abort : addSpecies";
}

void SoftWindow::choseSpeciesColor(QString _speciesName) {
	CLOG(TRACE,"gui_sw") << "Process : chose species color for species =" << cs(_speciesName);
  if (isStringIn(cs(_speciesName), m_mission->getSpeciesNames())) {
    QColor color = QColorDialog::getColor(Qt::white,
                                          this,
                                          "Choose " + _speciesName +
                                          " representative color :");
    m_mission->setSpeciesColor(cs(_speciesName), cs(color.name()));
    QPixmap pix(10, 10);
    pix.fill(QColor(color));
    for (size_t i = 0; i < m_species.size(); i++) {
      if (m_species[i]->text() == _speciesName) {
        m_species[i]->setIcon(pix);
        update();
      }
    }
  } else {
    QString warnText = "You are trying to change the color of an "
        "inexisting object. \n";
    QMessageBox::warning(this, "Object missing", warnText);
  }
}

void SoftWindow::editSpecies() {
	CLOG(INFO,"gui_sw") << "Action : Edit species";
  QStringList species;
  for (size_t i = 0; i < m_mission->getSpeciesNames().size(); i++) {
    if (m_mission->getSpeciesNames()[i] != "noClass" &&
        m_mission->getSpeciesNames()[i] != "falsePos") {
      species << QString::fromStdString((m_mission->getSpeciesNames()[i]));
    }
  }

  bool ok;
  QString speciesName = QInputDialog::getItem(this, tr("Edit object"),
                                         tr("Object to edit :"),
                                       species, 0, false, &ok);
  CLOG(INFO,"gui_sw") << "Data : edit species=" << cs(speciesName);
  if (ok && !speciesName.isEmpty()) {
    ModifySpecies *window = new ModifySpecies(this,
                                              m_mission->getSpecies(cs(speciesName)));
    window->adjustSize();
    window->move(QApplication::desktop()->screen()->rect().center() -
                 window->rect().center());
    window->setAlignment(Qt::AlignCenter);
    window->show();
  }
}

void SoftWindow::removeSpecies(QString _speciesName) {
	CLOG(TRACE,"gui_sw") << "Process : remove species=" << cs(_speciesName);
  bool speciesFound(false);
  for (size_t i = 0; i < m_species.size(); i++) {
    if (_speciesName == m_species[i]->text()) {
      if (m_species[i]->isChecked()) {
        m_species[i]->setChecked(false);
        m_speciesTreated = "";
      }
      // Actions to supress the species
      //  Erase the action
      m_speciesGroup->removeAction(m_species[i]);
      delete m_species[i];
      m_species.erase(m_species.begin() + i);

#ifdef USE_ALGO
	  m_subToolbar->removeAction(m_threshAction);
	  m_subToolbar->removeAction(m_spinAction);
	  delete m_turtleThreshold;
	  delete m_turtleSpin;
	  m_turtleSpin = 0;
	  m_turtleThreshold = 0;
#endif  // USE_ALGO
      //  Erase the species in all the observations of the mission
      m_mission->removeSpecies(cs(_speciesName));



      // actualizeInfos
      std::vector<int> *allCount = new std::vector<int>();
      std::vector<int> *folderCount = new std::vector<int>();
      std::vector<int> *imgCount = new std::vector<int>();
  
      getAllSpeciesCount(allCount, folderCount, imgCount);
      m_rightDock->actualizeInfos(m_mission->getSpeciesNames(),
                                  *allCount, *folderCount, *imgCount, m_iter,
                                  m_currentFolder->getObsNumber());
      delete allCount;
      delete folderCount;
      delete imgCount;

      // Protection
      if (m_species.size() < 1) {
        m_actions["removeSpecies"]->setEnabled(false);
        m_actions["editSpecies"]->setEnabled(false);
        m_actions["clearObs"]->setEnabled(false);
      }
      speciesFound = true;
    }
  }
  if (!speciesFound) {
    QMessageBox::information(this,
                             "Information",
                             "There is no such object in this project," \
                             "please enter the object name precisely");
  }
}

void SoftWindow::removeSpecies() {
	CLOG(INFO,"gui_sw") << "Action : remove species";
	QStringList species;
	for (size_t i = 0; i < m_mission->getSpeciesNames().size(); i++) {
		if (m_mission->getSpeciesNames()[i] != "noClass" && m_mission->getSpeciesNames()[i] != "falsePos")
			species << QString::fromStdString((m_mission->getSpeciesNames()[i]));
	}

  bool ok;
  QString speciesName = QInputDialog::getItem(this, tr("Remove object"),
                                         tr("Object to suppress :"),
                                       species, 0, false, &ok);
  if (ok && !speciesName.isEmpty()) {
    QMessageBox::StandardButton ans;
    QString confMessage =
        "Are you sure you want to remove \"" + speciesName +
        "\"? \n All \"" + speciesName + "\" detections will be lost.";
    ans = QMessageBox::question(this, "Confirmation", confMessage);
    if (ans == QMessageBox::Yes)
      removeSpecies(speciesName);
	CLOG(INFO,"gui_sw") << "Data : Removed species =" << cs(speciesName);
	emit iterchanged();
	return;
  }
  CLOG(INFO,"gui_sw") << "Abort : Remove species";
  
}

void SoftWindow::selectSpecies() {
	CLOG(TRACE,"gui_sw") << "Process : select species";
  for (size_t i = 0; i < m_species.size(); i++) {
    if (m_species[i]->isChecked()) {
      m_speciesTreated = m_species[i]->text();
	  CLOG(INFO,"gui_sw") << "Data : selected species =" << cs(m_speciesTreated);
    }
  }
}

void SoftWindow::clearCurrentObs() {
	CLOG(INFO,"gui_sw") << "Action : clear current Obs";
  QMessageBox::StandardButton ans;
  QString cMess =
      "Do you really want to clear all detections for this image?";
  ans = QMessageBox::question(this, "Confirmation", cMess);
  if (ans == QMessageBox::Yes) {
    m_currentFolder->getObs(m_iter).lock()->cleanSpecies();
    actualizeMainImage();
	CLOG(INFO,"gui_sw") << "Data : Obs=" << m_currentFolder->getObs(m_iter).lock()->getFilePath() << " cleared";
  } else {
	  CLOG(INFO,"gui_sw") << "Abort : clear Observation";
    return;
  }
  return;
}

void SoftWindow::openUserManual() {
	CLOG(INFO,"gui_sw") << "Action : open user manual";
#ifdef _WIN32
  QDesktopServices::openUrl(QUrl("file:///" +
                                 qApp->applicationDirPath() +
                                 "/../../doc/HarmonyUserManual_v1.1.4.pdf"));
  //QString path = qApp->applicationDirPath() +
//	  "/../../doc/HarmonyUserManual_v1.1.4.pdf";
 //QDesktopServices::openUrl(QUrl::fromLocalFile(path));

  return;
#else
  QString path = qApp->applicationDirPath() + "/../doc/HarmonyUserManual_v1.1.4.pdf";

  QUrl pathUrl(path);
  QDesktopServices::openUrl(pathUrl);
  QUrl baseUrl(qApp->applicationDirPath());
  return;
#endif
}

void SoftWindow::aboutHarmony() {
	CLOG(INFO,"gui_sw") << "Action : about Harmony";
  QMessageBox about;
  about.setIcon(QMessageBox::Question);
  about.setWindowIcon(QIcon(m_iconPath + "logo.png"));
  about.setWindowTitle(tr("About Harmony"));

  // Enable the HTML format.
  about.setTextFormat(Qt::RichText);

  // Getting the HTML from the file
#ifdef USE_ALGO
  std::ifstream file(cs(qApp->applicationDirPath() +
                        "/../../html/aboutHarmonyCaffe.html"));
#else
  std::ifstream file(cs(qApp->applicationDirPath() +
                        "/../html/aboutHarmony.html"));
#endif  // USE_ALGO
  std::string html, line;

  if (file.is_open()) {
    while (std::getline(file, line))
      html += line;
  }
  about.setText(html.c_str());
  about.exec();
}

void SoftWindow::aboutWipsea() {
	CLOG(INFO,"gui_sw") << "Action : about Wipsea";
  QMessageBox about;
  about.setIcon(QMessageBox::Question);
  about.setWindowIcon(QIcon(m_iconPath + "logo.png"));
  about.setWindowTitle(tr("About WIPSEA"));

  // Enable the HTML format.
  about.setTextFormat(Qt::RichText);

  // Getting the HTML from the file
  std::string htmlPath =
      cs(qApp->applicationDirPath() + "/../../html/aboutWipsea.html");
  std::ifstream file(htmlPath);
  std::string html, line;

  if (file.is_open()) {
    while (std::getline(file, line))
      html += line;
  }
  about.setText(tr(html.c_str()));
  about.exec();
}

void SoftWindow::Exit() {
  this->close();
}

// Graphical functions
void SoftWindow::resizeEvent(QResizeEvent* event) {
	CLOG(TRACE,"gui_sw") << "Process : resizeEvent";
  QMainWindow::resizeEvent(event);
  m_centralArea->resizeEvent(event);
  // update();
}

void SoftWindow::actualizeMainImage() {
	CLOG(TRACE,"gui_sw") << "Process : actualize main image";
  if (m_mission != 0) {
    std::shared_ptr<Observation> currentObs = m_currentFolder->getObs(m_iter).lock();
    QString obsName = QString::fromStdString(currentObs->getFilePath());
    if (isImage(cs(obsName))) {
        m_centralArea->m_imageCentral->setImage(currentObs, m_centralArea);
		CLOG(INFO,"gui_sw") << "Data : main image=" << cs(m_imageTreatedName);
      } else {
      QString warnText = "The path you entered is not a valid path"
          "to an image from this project";
        QMessageBox::warning(this, "Wrong file", warnText);
		LOG(WARNING) << cs(m_imageTreatedName) << "is not an image of the project";
    }
    m_imageTreatedName = obsName;
    m_currentImage->setText(m_imageTreatedName);

#ifdef USE_ALGO
    if (m_turtleThreshold != 0)
      m_turtleThreshold->setValue(currentObs->getThreshold("Turtle") * 100);
#endif  // USE_ALGO
    m_centralArea->m_flecheGaucheButton->setEnabled(true);
    m_centralArea->m_flecheDroiteButton->setEnabled(true);
    if (m_iter == 0)
      m_centralArea->m_flecheGaucheButton->setEnabled(false);
    if (m_iter == m_currentFolder->getObsNumber() - 1)
      m_centralArea->m_flecheDroiteButton->setEnabled(false);
    update();

    // actualizeInfos
    std::vector<int> *allCount = new std::vector<int>();
    std::vector<int> *folderCount = new std::vector<int>();
    std::vector<int> *imgCount = new std::vector<int>();
  
    getAllSpeciesCount(allCount, folderCount, imgCount);
    if (m_mission->getName() != "welcome")
    m_rightDock->actualizeInfos(m_mission->getSpeciesNames(),
                              *allCount, *folderCount, *imgCount, m_iter,
                                m_currentFolder->getObsNumber());
    delete allCount;
    delete folderCount;
    delete imgCount;
  }

  CLOG(TRACE,"gui_sw") << "Process : saving mission (actualize main image)";
  Json::Value save;
  m_mission->writeJsonItem(&save);
  QString fileName = QString::fromStdString(m_mission->getFolderPath()) +
      "/" + QString::fromStdString(m_mission->getName()) + ".json";
  std::ofstream out(cs(fileName), std::ofstream::out);
  out << save;
  out.close();
}

void SoftWindow::incrementMainImage() {
	CLOG(TRACE,"gui_sw") << "Action : increment main image";
  if (m_mission != 0) {
    if (m_iter < m_currentFolder->getObsNumber()-1) {
		m_iter+=1;
		CLOG(TRACE,"gui_sw") << "Data : iter =" << m_iter;
		emit iterchanged();
	} else {
		CLOG(TRACE,"gui_sw") << "Abort : increment main image, iterMax=" << m_iter;
	}
  }
}

void SoftWindow::decrementMainImage() {
	CLOG(TRACE,"gui_sw") << "Action : decrement main image";
  if (m_mission != 0) {
    if (m_iter >=1) {
		m_iter-= 1;
		CLOG(TRACE,"gui_sw") << "Data : iter =" << m_iter;
		emit iterchanged();
	} else {
		CLOG(TRACE,"gui_sw") << "Abort : increment main image, iterMax =" << m_iter;
	}
  }
}

void SoftWindow::changeMainImage(int _id) {
	CLOG(TRACE,"gui_sw") << "Process : changing main image by id =" << _id;
  if (m_mission != 0) {
    if (_id >=0 && _id < m_currentFolder->getObsNumber()) {
      m_iter = _id;
    emit iterchanged();
    }
  }
}

void SoftWindow::changeMainImage(std::string _path) {
	CLOG(TRACE,"gui_sw") << "Process : changing main image by path =" << _path;
  for (int i = 0; i < m_currentFolder->getObsNumber(); i++) {
    if (m_currentFolder->getObs(i).lock()->getFilePath() == _path) {
      m_iter = i;
      emit iterchanged();
    }
  }
}

void SoftWindow::changeMainImage() {
	CLOG(INFO,"gui_sw") << "Action : change main image writing path =" << cs(m_currentImage->text());
  bool success = false;
  if (m_mission != 0) {
    for (int i = 0; i < m_currentFolder->getObsNumber(); i++) {
      if (m_currentFolder->getObs(i).lock()->getFilePath() ==
          cs(m_currentImage->text())) {
        m_imageTreatedName = m_currentImage->text();
        m_iter = i;
        emit iterchanged();
        success = true;
      }
    }
    if (!success) {
      QString warnText = "The path you entered is not a valid"
          " path to an image from this project";
    QMessageBox::warning(this, "Wrong path", warnText);
	LOG(WARNING) << "Message : path =" << cs(m_currentImage->text()) << "; is not a valid path";
    }
  }
}

void SoftWindow::closeEvent(QCloseEvent *_event) {
	CLOG(INFO,"gui_sw") << "Action : Exit";
  _event->ignore();
  QMessageBox::StandardButton ans;
  ans = QMessageBox::question(this, "Quit?",
                              "Do you really want to quit Harmony?");
  if (ans == QMessageBox::Yes) {
    QString currentFolder = "/";
    if (m_mission->getName() != "welcome") {
      saveMission(false);
      QMessageBox::information(this,
                               "Information",
                               "The current project has successfully been saved in "
							   + QString::fromStdString(m_mission->getFolderPath()) + "/" + QString::fromStdString(m_mission->getName()) +
                               ".json");
    }
    QSettings settings("Wipsea", "Harmony");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
	CLOG(INFO,"gui_sw") << "Saved mission in" << m_mission->getFolderPath() << "; Exit without problems";
    QMainWindow::closeEvent(_event);
  } else {
	  CLOG(INFO,"gui_sw") << "Action : Abort exit process";
    return;
  }
}

QMenu* SoftWindow::createPopupMenu() {
  QMenu* filteredMenu = QMainWindow::createPopupMenu();
  filteredMenu->removeAction(m_toolBarMain->toggleViewAction());
  return filteredMenu;
}

// get
QString SoftWindow::getIconPath() const {
  return m_iconPath;
}

float SoftWindow::getThresh() {
  return m_threshold;
}

int SoftWindow::getIter() const {
  return m_iter;
}

void SoftWindow::getAllSpeciesCount(std::vector<int> *allCount,
                        std::vector<int> *folderCount,
                        std::vector<int> *imgCount) {
  for (size_t it = 0; it < m_mission->getSpeciesNames().size(); it++) {
    int totalDetected = m_mission->getSpeciesCount(m_mission->getSpeciesNames()[it]);
    int folderDetected = m_currentFolder->getSpeciesCount(m_currentFolder->getSpeciesNames()[it]);
    int imgDetected = m_currentFolder->getObs(m_iter).lock()->getSpeciesCount(m_currentFolder->getSpeciesNames()[it]);
      
    allCount->push_back(totalDetected);
    folderCount->push_back(folderDetected);
    imgCount->push_back(imgDetected);
  }
}

Mission::species SoftWindow::getSpecies(QString _name) {
  return m_mission->getSpecies(cs(_name));
}

// set
void SoftWindow::setIter(int iter) {
	CLOG(TRACE,"gui_sw") << "Process : set iter =" << iter;
  if (iter < m_currentFolder->getObsNumber() &&
      iter >= 0) {
    m_iter = iter;
	CLOG(TRACE,"gui_sw") << "Data : iter =" << iter;
  }
}

void SoftWindow::changeSpeciesName(QString _speciesOldName,
                                QString _speciesNewName) {
	if (m_speciesTreated == _speciesOldName)
		m_speciesTreated = _speciesNewName;
	CLOG(TRACE,"gui_sw") << "Process : change species name, oldname=" << cs(_speciesOldName) << "; newName=" << cs(_speciesNewName);
  m_mission->changeSpeciesName(cs(_speciesOldName), cs(_speciesNewName));

  for (size_t i = 0; i < m_species.size(); i++) {
    if (_speciesOldName == m_species[i]->text()) {
      m_species[i]->setText(_speciesNewName);
      break;
    }
  }
 
  CLOG(INFO,"gui_sw") << "Data : " << cs(_speciesOldName) << " is now known as " << cs(_speciesNewName);
  emit iterchanged();
}

void SoftWindow::setSpeciesColor(QString _speciesName,
                                QString _speciesColor) {
	CLOG(TRACE,"gui_sw") << "Process : change species color, speciesName=" << cs(_speciesName) << "; speciesColor=" << cs(_speciesColor);
  for (size_t i = 0; i < m_mission->getSpeciesNames().size(); i++) {
    if (cs(_speciesName) == m_mission->getSpeciesNames()[i]) {
      m_mission->setSpeciesColor(cs(_speciesName), cs(_speciesColor));
      break;
    }
  }
  for (size_t i = 0; i < m_species.size(); i++) {
    if (_speciesName == m_species[i]->text()) {
      QPixmap pix(10, 10);
      pix.fill(QColor(_speciesColor));
      m_species[i]->setIcon(QIcon(pix));
      break;
    }
  }
  emit iterchanged();
}
