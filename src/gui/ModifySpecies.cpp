/*
The file ModifySpecies.cpp is part of the Harmony software.

Harmony is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2016 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2016/11/07

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
- Boost : custom license => http://www.boost.org/users/license.html
*/

#include "ModifySpecies.hpp"
#include "SoftWindow.hpp"
#include "Mission.hpp"
#include "easylogging++.h"

#include "moc_ModifySpecies.cpp"

ModifySpecies::ModifySpecies(SoftWindow *_parent, Mission::species spec) {
	CLOG(TRACE, "gui_ms") << "Process : initialise modify species window for species=" << spec.name;
  m_parent = _parent;
  m_spec = spec;
  m_name = QString::fromStdString(m_spec.name);
  m_color = QColor(QString::fromStdString(m_spec.color));
  
  this->setTitle("Edit " + QString::fromStdString(spec.name));

  QVBoxLayout *vLayout = new QVBoxLayout;

  // Name edition
  QFrame *nameEditW = new QFrame;
  QHBoxLayout *nameEditL = new QHBoxLayout;
  QLabel *nameTitle = new QLabel("Species Name :");
  m_nameEdit = new QLineEdit(QString::fromStdString(m_spec.name), this);
  connect(m_nameEdit, SIGNAL(editingFinished()),
          this, SLOT(changeName()));
  nameEditL->addWidget(nameTitle);
  nameEditL->addWidget(m_nameEdit);
  nameEditW->setLayout(nameEditL);
  vLayout->addWidget(nameEditW);

  // Color Edition
  QFrame *colorEditW = new QFrame;
  QHBoxLayout *colorEditL = new QHBoxLayout;
  QLabel *colorTitle = new QLabel("Color :");
  m_colorButton = new QPushButton(colorEditW);
  QPixmap pix(10, 10);
  pix.fill(QColor(QString::fromStdString(m_spec.color)));
  m_colorButton->setIcon(QIcon(pix));
  connect(m_colorButton, SIGNAL(clicked()),
          this, SLOT(changeColor()));
  colorEditL->addWidget(colorTitle);
  colorEditL->addWidget(m_colorButton);
  colorEditW->setLayout(colorEditL);
  vLayout->addWidget(colorEditW);

  // OK and Cancel buttons
  QFrame *validation = new QFrame;
  QHBoxLayout *validationL = new QHBoxLayout;

  m_okButton = new QPushButton(this);
  m_okButton->setText("OK");
  connect(m_okButton, SIGNAL(clicked()),
          this, SLOT(transferData()));

  m_cancelButton = new QPushButton(this);
  m_cancelButton->setText("Cancel");
  connect(m_cancelButton, SIGNAL(clicked()),
          this, SLOT(abort()));

  validationL->addWidget(m_cancelButton);
  validationL->addWidget(m_okButton);
  validation->setLayout(validationL);
  vLayout->addWidget(validation);
  
  this->setLayout(vLayout);
  this->show();
}

ModifySpecies::~ModifySpecies() {
}

void ModifySpecies::changeColor() {
  m_color = QColorDialog::getColor(QColor(QString::fromStdString(m_spec.color)),
                                        this,
                                        "Chose " + m_name +
                                        " representative color :");
  QPixmap pix(10, 10);
  pix.fill(m_color);
  m_colorButton->setIcon(QIcon(pix));
}

void ModifySpecies::changeName() {
	m_name = m_nameEdit->text();
}

void ModifySpecies::transferData() {
	if (QString::fromStdString(m_spec.name) != "Turtle" && m_name == "Turtle"){
		QString warnText = "Turtle is not an allowed name, it is restricted for the automatic algorithm";
		QMessageBox::warning(this, "Wrong name", warnText);
		CLOG(WARNING, "gui_ms") << "User tried to use the Turtle name";
		this->close();
		return;
	}
  m_parent->changeSpeciesName(QString::fromStdString(m_spec.name),
                           m_name);
  m_parent->setSpeciesColor(m_name,
                            m_color.name());

  this->close();
}

void ModifySpecies::abort() {
  this->close();
}
