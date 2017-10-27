/*
The file SoftWindow.hpp is part of the Harmony software.

Harmony is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2015 WIPSEA, contact@wipsea.com

nAuthor(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html

*/

#ifndef _SOFT_WINDOW_H_
#define _SOFT_WINDOW_H_

#include <QWidget>
#include <QtWidgets>
#include <QtWidgets/QMainWindow>
#include <string>
#include <vector>
#include <map>
#include "CentralArea.hpp"
#include "DockImages.hpp"
#include "DockFiles.hpp"
#include "DockInfos.hpp"
#include "Mission.hpp"
class WpsServer;

class SoftWindow : public QMainWindow {
  friend class DockImages;
  friend class DockFiles;
  friend class DockInfos;
  friend class ImgZoom;
  friend class CentralArea;
  Q_OBJECT

 public:
  /*!
   * \brief Constructor
   *
   * SoftWindow class constructor
   */
  SoftWindow();

  /*!
   * \brief Destructor
   *
   * Class destructor
   */
  ~SoftWindow();

 public slots :

  /*!
   * \brief Create a new mission
   *
   * Select a folder to upload the mission's images and start processing
   */
  void createNewMission();

  /**
   * \brief create Project
   *
   * Create a mission object named _missionName, containing all the images in
   * _folder
   *
   * \param _missionName : Name of the project : used to save the project
   * \param _folder : Absolute path to the folder containing the project images
   *
   * \return : void 
   */
  void createNewMission(std::string _missionName, std::string _folder);

  /*!
   * \brief Save Mission
   *
   * Save the observations and the detected species in a Json file. This function can
   * provide a message window confirming the operation if _print is set to true.
   *
   * \param _print : if true, show confirmation
   *
   * \return : void
   */
  void saveMission(bool _print = true);

  /*!
   * \brief Save Mission as 
   *
   * Save the observations and the detected species in a Json file with a given name
   */
  void saveMissionAs();

  /*!
   * \brief Update the central image
   *
   * this function will display and update all the fields related to the main image (in the central area). It is meant to be call by all the functions that aim at affecting the main image (left arrow, right arrow, toolbar, ...)
   */
  void actualizeMainImage();

  /*!
   * \brief increment main image
   *
   * right arrow button action, increment the mission folder to the next image and display it
   */
  void incrementMainImage();

    /*!
   * \brief decrement main image
   *
   * left arrow button action, decrement the mission folder to the next image and display it
   */
  void decrementMainImage();

  /*!
   * \brief change main image
   *
   * select another image to display via its index
   *
   * \param _id : index of the wanted image in the folder
   */
  void changeMainImage(int _id);

  /**
   * \brief change main imgage
   *
   * Global slot to update the main image
   *
   * \param 
   *
   * \return 
   */
  void changeMainImage();

  /**
   * \brief change main image
   *
   * Select the image designated by _path as the new main image
   *
   * \param _path : 
   *
   * \return 
   */
  void changeMainImage(std::string _path);

  /*!
   * \brief Print info
   *
   * Print information concering the software (version number, goal, link, ...)
   */
  void aboutHarmony();

  /*!
   * \brief open doc
   *
   * Open a pdf document containing the software documentation
   * \return 
   */
  void openUserManual();

  /*!
   * \brief Print info
   *
   * Print information concerning the company
   */
  void aboutWipsea();

  /*!
   * \brief overwrite
   *
   * Take control of the closing event to ask for confirmation and save current project
   *
   * \param *_event : closing event (received from app->quit or from the "red cross" button)
   */
  void closeEvent(QCloseEvent *_event);

  /*!
   * \brief Exit
   *
   * Exit the programe
   */
  void Exit();
  
  /*!
   * \brief Add species
   *
   * Add a species to the mission, enabling the possibility to identify members in all the images of the mission
   * \return 
   */
  void addSpecies();
  
#ifdef USE_SERVER
  void startWpsServer();
  
  void stopWpsServer();
  
  void getWPSMission();
  void showWPSDirectory();
  
  void synchronizeWPS();
#endif  // USE_SERVER

  /*!
   * \brief addSpecies
   *
   * overload with species name as input
   *
   * \param _species = species name 

   */
  void addSpecies(QString _species, QString _color = "#000000");

  /*!
   * \brief select species color
   *
   * Open a graphical interface to select a color for the given species
   *
   * \param _speciesName : species to be modified
   */
  void choseSpeciesColor(QString _speciesName);

  /*!
   * \brief edit species
   *
   * Select and existing species and modify its parameters
   */
  void editSpecies();
      
  /*!
   * \brief Remove Species
   *
   * Select an existing species to be removed
   */
  void removeSpecies();

  /*!
   * \brief Remove species
   *
   * Remove _speciesName from the mission's know species. This will remove the button,
   * the counts as well as all the detections.
   *
   * \param _speciesName : name of the species to be removed as known in m_mission->m_speciesList
   */
  void removeSpecies(QString _speciesName);

  /*!
   * \brief select species 
   *
   * Select the species to be treated among the existing species
   */
  void selectSpecies();

  /*!
   * \brief clear current obs
   *
   * Remove all detection from the main image
   */
  void clearCurrentObs();
  
  /*!
   * \brief Load a mission
   *
   * Load a mission from a JSON file
   */
  void loadMission(QString fileName);
  void loadMission(std::shared_ptr<Mission> _mission);

  /*!
   * \brief Loading informations
   *
   * Get the file to load the mission
   */
  void loadMission();

#ifdef USE_ALGO
  /*!
   * \brief apply a marine turtle filter
   *
   * The filter is described in algo.cpp, it is designed to return most objects
   * looking like a turtle from an aircraft point of view (designed for drones)
   */
  void detectTurtles();

  void extractPatches();

  /*!
   * \brief Threshold config
   *
   * Set the threshold for automatic detection of turtles
   */
  void setThreshold();
  
#endif  // USE_ALGO

 public :
  /*!
   * \brief get
   *
   * Get the iconPath for the project
   *
   * \param 
   *
   * \return QString : relative path to the icon image folder
   */
  QString getIconPath() const;

  float getThresh();

  int getIter() const;

  void getAllSpeciesCount(std::vector<int> *allCount,
                        std::vector<int> *folderCount,
                          std::vector<int> *imgCount);
  
  void setIter(int iter);

  void changeSpeciesName(QString _speciesOldName,
                      QString _speciesNewName);

  void setSpeciesColor(QString _speciesName,
                                   QString _speciesColor);

    /*!
   * \brief remove a species button
   *
   * remove the button and it's connexions
   *
   * \param _spec : species to be removed
   */
  void removeSpeciesButton(Mission::species _spec);

    /*!
   * \brief Back to initial state
   *
   * Remove all project related buttons and infos
   */
  void resetWorkspace();

  Mission::species getSpecies(QString _name);

 signals :
  void iterchanged();

 protected:

  /*!
   * \brief Action initialisation
   *
   * Initialisation of all actions available from the menu
   */
  void initActions();

  /*!
   * \brief Menu initialisation
   *
   * Initialisation of the menus
   */
  void initMenus();

  /*!
   * \brief Toolbar initialisation
   *
   * Initialise all actions/buttons/widgets of the toolBar
   */
  void initToolBar();

  /*!
   * \brief Logger initialisation
   *
   * Initialisase logger configuration and log context information
   */
  void initLogger();

  /*!
   * \brief Add button to select species
   *
   * Add a button to m_subtoolbar to select the species _spec. This button will also
   * be in the QGroupButton
   *
   * \param -_spec : Name of the species to be added
   */
  void addSpeciesButton(Mission::species _spec);

  // remove access to hiding the main toolbar
  QMenu* createPopupMenu();

  /*!
   * \brief Resize window
   *
   * This function handles the resize event for all widgets inside
   */
  void resizeEvent(QResizeEvent* event);

  /*
    Attributes
  */
  
  //! Path to the icon image folder.
  QString m_iconPath;

  //! Pointer to the toolbar.
  QToolBar *m_toolBarMain = 0;

  //! Pointer to the toolbar.
  QToolBar *m_subToolbar = 0;

  //! Name of the image being displayed in the main window.
  QString m_imageTreatedName;

  //! Name of the species being treated
  QString m_speciesTreated;

  //! position of the current image in the mission's obsList
  int m_iter = -1;

  //! Object used to select an image by it's name in the toolbar.
  QLineEdit *m_currentImage = 0;

  //! Pointer the the CentralArea class, dealing with main image display.
  CentralArea *m_centralArea;

  //! Dock widget displaying the list of images
  DockFiles *m_leftDock = 0;

  //! Dock containing informations about the mission
  DockInfos *m_rightDock = 0;

  //! Mission
  std::shared_ptr<Mission> m_mission;

  //! current folder
  std::shared_ptr<Mission>  m_currentFolder = 0;


  //! List of species available in the current mission
  std::vector<QAction*> m_species;

  //! one species can be identified at a time
  QActionGroup *m_speciesGroup;

  //! Available actions
  std::map<std::string, QAction *> m_actions;

  //! File Menu
  QMenu* m_fileMenu;

  //! Edition Menu
  QMenu* m_editionMenu;
  
#ifdef USE_ALGO
  //! Tools Menu
  QMenu* m_toolsMenu;

  //! slider to set the threshold for turtle classification
  QSlider *m_turtleThreshold = 0;
  
  //! thresholdAction
  QAction *m_threshAction = 0;
  
  QSpinBox *m_turtleSpin = 0;
  QAction* m_spinAction = 0;
  
  QLabel *m_gpuShow= 0;
#endif  // USE_ALGO

  //! Help Menu
  QMenu* m_aboutMenu;

  //! threshold to show detection
  double m_threshold = 0.4;

  //! For graphical purposes in the toolbar
  QAction *m_spacer;

  //! Species edition window
  QGroupBox *m_editionWindow;
  
#ifdef USE_SERVER
  //! server
  WpsServer *m_server;
  
  //! wps Mission
  std::shared_ptr<Mission> m_wpsMission = 0;
#endif  // USE_SERVER
};
  void myCrashHandler(int sig);

#endif  // _SOFT_WINDOW_H_
