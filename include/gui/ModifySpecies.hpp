/*
The file ModifySpecies.hpp is part of the Harmony software.

Harmony is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2015 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2016/11/07

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
*/

#ifndef _MODIFY_SPECIES_
#define _MODIFY_SPECIES_

#include <QWidget>
#include <QtWidgets>
#include <QtWidgets/QMainWindow>
#include "SoftWindow.hpp"
#include "Mission.hpp"
#include "QtCore"

class ModifySpecies : public QGroupBox {
  Q_OBJECT

 public:
  /*!
   * \brief Class Constructor
   *
   * ModifySpecies class constructor
   */
  ModifySpecies(SoftWindow *_parent, Mission::species spec);

  /*!
   * \brief Class Desctructor
   *
   * ModifySpecies class desctructor
   */
  ~ModifySpecies();

 public slots:
  /*!
   * \brief change color
   *
   * Select a new color and modify the button
   */
  void changeColor();

  /*!
   * \brief change name
   *
   * Modify the species name
   */
  void changeName();

  /*!
   * \brief transfer data to parent
   *
   * Transfer the modified parementers to parent and close window
   */
  void transferData();

  /*!
   * \brief abort 
   *
   * abort modification, close window without changing anything
   */
  void abort();

 private:
  //! parent window
  SoftWindow *m_parent;

  //! original species parameters
  Mission::species m_spec;
  //! color of the button color;
  QColor m_color;

  //! color button
  QPushButton *m_colorButton;

  //! Species name in the current state
  QString m_name;

  //! Name edition
  QLineEdit *m_nameEdit;

  //! Ok button
  QPushButton *m_okButton;

  //! Cancel button
  QPushButton *m_cancelButton;
};
#endif  // _MODIFY_SPECIES_
