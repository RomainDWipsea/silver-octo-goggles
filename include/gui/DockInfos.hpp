/*
The file DockInfos.hpp is part of the WildlifeCounter software.

WildlifeCounter is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2015 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits r�serv�s - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
- OpenCV : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
*/

#ifndef HEADER_DOCKINFOS
#define HEADER_DOCKINFOS

#include <QtWidgets>
#include <vector>
#include <string>
class SoftWindow;

/*! \class DockInfos
* \brief Classe d'IHM.
*
* La classe g�re le comportement du dock affichant les infos compl�mentaires de l'�tude en cours.
*/
class DockInfos : public QDockWidget {
  friend class SoftWindow;
  //  Q_OBJECT

 public:
  /*!
   * \brief Constructeur
   *
   * Constructeur de la classe DockInfos
   *
   * \param 
   */
  explicit DockInfos(SoftWindow *parent);
  ~DockInfos();

 public slots :
  
  /*!
   * \brief Raffraichissement des infos affich�es
   * 
   * Fonction permettant de recalculer les infos affich�es dans le dock en fonction des nouveaux r�sultats
   * 
   * \param	- speciesNames : liste des esp�ces trait�es dans cette mission
   *			- species count : nombre d'individus identifi�s pour chaque esp�ces
   */
  void actualizeInfos(std::vector<std::string> speciesNames,
                               std::vector<int>  speciesCountProject,
                               std::vector<int>  speciesCountFolder,
                               std::vector<int>  speciesCountImg,
                                 int iter, int total);

  /*!
   * \brief Gestion du temps en mode jeu
   *
   * Fonction de calcul et d'affichage du temps en mode jeu.
   *
   * \param	- time : temps � afficher
   */
  void setTime(int time);

 protected slots :

 private:
  //! Un dock ne peut afficher qu'une seul widget, celui-ci contient les autres
  QFrame *m_masterWidget;

  //! Frame designed to plot the count for each species
  QFrame *m_speciesCountsProject;

  //! Frame designed to plot the count for each species
  QFrame *m_speciesCountsFolder;

  //! Frame designed to plot the count for each species
  QFrame *m_speciesCountsImg;

  //! Frame designed to plot time in game mode
  QProgressBar *m_showTime;

  //! Frame designed to plot the logo in game mode
  //  QLabel *m_turtleGame;

  QLabel *m_imageNumber;

  SoftWindow *m_parent;
};

#endif
