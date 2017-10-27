/*
The file Mission.cpp is part of the Harmony software.

Harmony is a software that provides tools for the detection of wildlife in series of images.

Copyright 2014-2016 WIPSEA SARL, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- OpenCV 3.0.0 : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
- Boost : custom license => http://www.boost.org/users/license.html
*/

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <set>
#include <map>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include "Mission.hpp"
#include "Observation.hpp"
#include "Item.hpp"
#include "misc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "wipro_config.h"
#include "easylogging++.h"

namespace fs = boost::filesystem;

Mission::Mission() : m_name("") {
  init();
}

Mission::~Mission() {
}

void Mission::init() {
  CLOG(TRACE, "cor_mi") << "Process : initialise mission=" << m_name;
  species NO_CLASS;
  NO_CLASS.name = "noClass";
  NO_CLASS.color = "#000000";
  NO_CLASS.hide = true;
  NO_CLASS.automatic = true;

  species FALSE_POS;
  FALSE_POS.name = "falsePos";
  FALSE_POS.color = "#000000";
  FALSE_POS.hide = true;
  FALSE_POS.automatic = true;


  m_speciesList["noClass"] = NO_CLASS;
  m_speciesList["falsePos"] = FALSE_POS;
}

Mission::Mission(Mission const& _missionToCopy) :
    m_name(_missionToCopy.m_name), m_obsList(_missionToCopy.m_obsList),
    m_speciesList(_missionToCopy.m_speciesList),
    m_folderPath(_missionToCopy.m_folderPath),
    m_subFolders(_missionToCopy.m_subFolders) {
}

Mission& Mission::operator=(Mission const& _missionToCopy) {
  if (this != &_missionToCopy) {
    m_name = _missionToCopy.m_name;
    m_obsList.clear();
    m_obsList = _missionToCopy.m_obsList;
    m_speciesList = _missionToCopy.m_speciesList;
    m_folderPath = _missionToCopy.m_folderPath;
    m_subFolders.clear();
    m_subFolders = _missionToCopy.m_subFolders;
  }
  return *this;
}

Mission::Mission(std::string _name) : m_name(_name) {
  init();
}

bool Mission::addObservation(std::string const& _filePath) {
  std::shared_ptr<Observation> temp(new Observation(_filePath));
  m_obsList.push_back(std::move(temp));
  return true;
}

bool Mission::addFolder(std::string const& _folderPath) {
  CLOG(TRACE, "cor_mi") << "Process : add foler="
                        << _folderPath << " to mission=" << m_name;
  m_folderPath = _folderPath;

  fs::path p = fs::system_complete(_folderPath);

  typedef std::vector<fs::path> PathVector;
  std::set<std::string> targetExtensions;
  targetExtensions.insert(".JPG");
  targetExtensions.insert(".BMP");
  targetExtensions.insert(".GIF");
  targetExtensions.insert(".PNG");
  targetExtensions.insert(".TIF");
  targetExtensions.insert(".JPEG");
  // raw format is about to be added.

  std::auto_ptr<PathVector> paths(new PathVector());
  std::unique_ptr<PathVector> foldPaths(new PathVector());

  fs::directory_iterator end;
  for (fs::directory_iterator iter(p); iter != end; ++iter) {
    if (fs::is_directory(iter->status())) {
      foldPaths->push_back(iter->path());
      continue;
    }
    if (!fs::is_regular_file(iter->status())) { continue; }
    std::string extension = iter->path().extension().string();
    std::transform(extension.begin(),
                   extension.end(),
                   extension.begin(), ::toupper);
    if (targetExtensions.find(extension) == targetExtensions.end()) {
      continue;
    }

    paths->push_back(iter->path());
  }
  sort(paths->begin(), paths->end());
  PathVector::iterator it;
  for (it = paths->begin(); it != paths->end(); it++) {
    std::string convertPath =
        boost::filesystem::canonical(*it).generic_string();
    addObservation(convertPath);
  }
  sort(foldPaths->begin(), foldPaths->end());
  PathVector::iterator itf;
  for (itf = foldPaths->begin(); itf != foldPaths->end(); itf++) {
    std::string convertPath =
        boost::filesystem::canonical(*itf).generic_string();
    addSubFolder(convertPath);
  }
  return true;
}

void Mission::addSubFolder(std::string _folderPath) {
  std::shared_ptr<Mission> sub(new Mission(_folderPath));
  sub->addFolder(_folderPath);
  m_subFolders.push_back(std::move(sub));
}


bool Mission::addSpecies(std::string const& _speciesName,
                         std::string _color,
                         bool _hide,
                         bool _auto,
                         float _thresh) {
  CLOG(TRACE, "cor_mi") << "Process : add species="
                        << _speciesName << " to mission=" << m_name;
  species temp;
  temp.name = _speciesName;
  temp.color = _color;
  temp.hide = _hide;
  temp.automatic = _auto;
  temp.threshold = _thresh;
  m_speciesList[_speciesName] = temp;

  // Dealing with subfolders
  for (size_t i = 0; i < m_subFolders.size(); i++) {
    m_subFolders[i]->addSpecies(_speciesName, _color, _hide, _auto, _thresh);
  }
  return true;
}

bool Mission::removeSubFolder(std::string _folderPath) {
  for (size_t i = 0; i < m_subFolders.size(); i++) {
    if (m_subFolders[i]->getFolderPath() == _folderPath) {
      m_subFolders[i].reset();
      m_subFolders.erase(m_subFolders.begin() + i);
      return true;
    }
  }
  return false;
}

bool Mission::removeSpecies(std::string const& _speciesName) {
  CLOG(TRACE, "cor_mi") << "Remove species="
                        << _speciesName << " to mission=" << m_name;
  // remove in m_speciesList
  m_speciesList.erase(_speciesName);
  // remove the species in all items (pass them to noClass
  for (size_t j = 0; j < m_obsList.size(); j++) {
    m_obsList[j]->removeSpecies(_speciesName);
  }

  // Dealing with subfolders
  for (size_t k = 0; k < m_subFolders.size(); k++) {
    m_subFolders[k]->removeSpecies(_speciesName);
  }
  return true;
}

bool Mission::writeJsonItem(Json::Value *_jsonObject) const {
  std::string version = std::to_string(Harmony_VERSION_MAJOR) +
      std::to_string(Harmony_VERSION_MINOR) +
      std::to_string(Harmony_VERSION_PATCH);
  (*_jsonObject)["Version"] = atoi(version.c_str());
  (*_jsonObject)["Name"] = m_name;
  Json::Value obsArray;
  std::vector<std::shared_ptr<Observation>>::const_iterator itObs;
  for (itObs = m_obsList.begin(); itObs !=  m_obsList.end(); itObs++) {
    Json::Value obsObject;
    Observation obs = *(*itObs);
    obs.writeJsonItem(&obsObject);
    obsArray.append(obsObject);
  }
  (*_jsonObject)["Observations"] = obsArray;

  (*_jsonObject)["FolderPath"] = m_folderPath;

  // Dealing with subfolders :
  Json::Value foldArray;
  std::vector<std::shared_ptr<Mission>>::const_iterator itFold;
  for (itFold = m_subFolders.begin(); itFold !=  m_subFolders.end(); itFold++) {
    Json::Value foldObject;
    // Mission mis = *(*itFold);
    (*(*itFold)).writeJsonItem(&foldObject);
    foldArray.append(foldObject);
  }
  (*_jsonObject)["subFolders"] = foldArray;

  Json::Value listSpecies;
  for (auto& x : m_speciesList) {
    Json::Value tempSpec;
    tempSpec["name"] = (x.second).name;
    tempSpec["color"] = (x.second).color;
    tempSpec["hide"] = (x.second).hide;
    tempSpec["automatic"] = (x.second).automatic;
    tempSpec["threshold"] = (x.second).threshold;
    listSpecies.append(tempSpec);
  }

  (*_jsonObject)["speciesList"] = listSpecies;
  return true;
}

bool Mission::readJsonItemv100(Json::Value const& _jsonObject) {
  m_name = _jsonObject["Name"].asString();
  m_obsList.clear();
  Json::Value obsArray = _jsonObject["Observations"];
  Json::Value::iterator it;
  for (it = obsArray.begin(); it != obsArray.end(); ++it) {
    Json::Value obsObject = *it;
    std::shared_ptr<Observation> obs(new Observation);
    obs->readJsonItem(obsObject);
    m_obsList.push_back(obs);
  }

  std::string readFolder = _jsonObject["FolderPath"].asString();
  if (readFolder.empty()) {
    // this means the json doesn't contain this info (older version)
    std::string firstObsPath = m_obsList[0]->getFilePath();
    boost::filesystem::path p(firstObsPath);
    boost::filesystem::path dir = p.parent_path();
    m_folderPath = dir.string();
  } else {
    m_folderPath = _jsonObject["FolderPath"].asString();
  }

  m_subFolders.clear();
  Json::Value foldArray = _jsonObject["subFolders"];
  Json::Value::iterator itf;
  for (itf = foldArray.begin(); itf != foldArray.end(); ++itf) {
    Json::Value foldObject = *itf;
    std::shared_ptr<Mission> mis(new Mission);
    mis->readJsonItem(foldObject);
    m_subFolders.push_back(mis);
  }

  Json::Value listSpecies = _jsonObject["speciesList"];
  Json::Value::iterator itSpec;
  for (itSpec = listSpecies.begin(); itSpec != listSpecies.end(); ++itSpec) {
    Json::Value  tempSpec = *itSpec;
    species temp;
    temp.name = tempSpec["name"].asString();
    temp.color = tempSpec["color"].asString();
    temp.hide = tempSpec["hide"].asBool();
    temp.threshold = tempSpec["threshold"].asFloat();
    m_speciesList[temp.name] = temp;
  }
  return true;
}

bool Mission::readJsonItemv110(Json::Value const& _jsonObject) {
  m_name = _jsonObject["Name"].asString();
  m_obsList.clear();
  Json::Value obsArray = _jsonObject["Observations"];
  Json::Value::iterator it;
  for (it = obsArray.begin(); it != obsArray.end(); ++it) {
    Json::Value obsObject = *it;
    std::shared_ptr<Observation> obs(new Observation);
    obs->readJsonItemv110(obsObject);
    m_obsList.push_back(obs);
  }

  std::string readFolder = _jsonObject["FolderPath"].asString();
  if (readFolder.empty()) {
    // this means the json doesn't contain this info (older version)
    std::string firstObsPath = m_obsList[0]->getFilePath();
    boost::filesystem::path p(firstObsPath);
    boost::filesystem::path dir = p.parent_path();
    m_folderPath = dir.string();
  } else {
    m_folderPath = _jsonObject["FolderPath"].asString();
  }

  m_subFolders.clear();
  Json::Value foldArray = _jsonObject["subFolders"];
  Json::Value::iterator itf;
  for (itf = foldArray.begin(); itf != foldArray.end(); ++itf) {
    Json::Value foldObject = *itf;
    std::shared_ptr<Mission> mis(new Mission);
    mis->readJsonItemv110(foldObject);
    m_subFolders.push_back(mis);
  }

  Json::Value listSpecies = _jsonObject["speciesList"];
  Json::Value::iterator itSpec;
  for (itSpec = listSpecies.begin(); itSpec != listSpecies.end(); ++itSpec) {
    Json::Value  tempSpec = *itSpec;
    species temp;
    temp.name = tempSpec["name"].asString();
    temp.color = tempSpec["color"].asString();
    temp.hide = tempSpec["hide"].asBool();
    temp.automatic = tempSpec["automatic"].asBool();
    temp.threshold = tempSpec["threshold"].asFloat();
    m_speciesList[temp.name] = temp;
  }
  return true;
}

bool Mission::readJsonItemOld(Json::Value const& _jsonObject) {
  m_name = _jsonObject["Name"].asString();
  m_obsList.clear();
  std::vector<std::string>* speciesList = new std::vector<std::string>;
  Json::Value obsArray = _jsonObject["Observations"];
  Json::Value::iterator it;
  for (it = obsArray.begin(); it != obsArray.end(); ++it) {
    Json::Value obsObject = *it;
    std::shared_ptr<Observation> obs(new Observation);
    obs->readJsonItem(obsObject, speciesList);
    m_obsList.push_back(obs);
    for (size_t j =0; j < speciesList->size(); j++) {
      if (!(isStringIn((*speciesList)[j], getSpeciesNames()))) {
        species temp;
        temp.name = (*speciesList)[j];
        temp.color = "#000000";
        temp.hide = false;
        temp.threshold = 0.5;
        m_speciesList[temp.name] = temp;
      }
    }
  }
  delete speciesList;

  std::string readFolder = _jsonObject["FolderPath"].asString();
  if (readFolder.empty()) {
    // this means the json doesn't contain this info (older version)
    std::string firstObsPath = m_obsList[0]->getFilePath();
    boost::filesystem::path p(firstObsPath);
    boost::filesystem::path dir = p.parent_path();
    m_folderPath = dir.string();
  } else {
    m_folderPath = _jsonObject["FolderPath"].asString();
  }

  m_subFolders.clear();
  Json::Value foldArray = _jsonObject["subFolders"];
  Json::Value::iterator itf;
  for (itf = foldArray.begin(); itf != foldArray.end(); ++itf) {
    Json::Value foldObject = *itf;
    std::shared_ptr<Mission> mis(new Mission);
    mis->readJsonItem(foldObject);
    m_subFolders.push_back(mis);
  }

  // the global project must have a species list
  // containing all subfolders species
  for (size_t i = 0; i < m_subFolders.size(); i++) {
    for (size_t j = 0; j  < m_subFolders[i]->getSpeciesNames().size(); j++) {
      if (!isStringIn(m_subFolders[i]->getSpeciesNames()[j],
                      getSpeciesNames())) {
        species temp;
        temp.name = m_subFolders[i]->getSpeciesNames()[j];
        temp.color = "#000000";
        temp.hide = false;
        temp.threshold = 0.5;
        m_speciesList[temp.name] = temp;
      }
    }
  }

  return true;
}

bool Mission::readJsonItem(Json::Value const& _jsonObject) {
  int version = _jsonObject["Version"].asInt();
  CLOG(INFO, "cor_mi") << "Reading a mission from version="
                       << version << " to mission=" << m_name;

  if (version < 100)
    return readJsonItemOld(_jsonObject);
  else if (version < 100)
    return readJsonItemv100(_jsonObject);
  else
    return readJsonItemv110(_jsonObject);
}

// getters
int Mission::getSpeciesCount(std::string _species) const {
  int res =  0;
  for (size_t obsIt = 0; obsIt < m_obsList.size(); obsIt++) {
    res += m_obsList[obsIt]->getSpeciesCount(_species);
  }
  for (size_t k = 0; k < m_subFolders.size(); k++) {
    res += m_subFolders[k]->getSpeciesCount(_species);
  }
  return res;
}

std::string Mission::getName() const {
  return m_name;
}

int Mission::getObsNumber() {
  return static_cast<int>(m_obsList.size());
}

std::weak_ptr<Observation> Mission::getObs(int _iter) const {
  if (static_cast<int>(m_obsList.size()) >= _iter) {
    return std::weak_ptr<Observation>(m_obsList[_iter]);
  } else {
    std::cout << "fatal error in Mission::getObs";
    return std::weak_ptr<Observation>(std::weak_ptr<Observation>());
  }
}

std::shared_ptr<Observation> Mission::getObs(std::string _path) {
  for (size_t i = 0; i < m_obsList.size(); i++) {
    if (m_obsList[i]->getFilePath() == _path)
      return m_obsList[i];
  }
  // TODO(Romain) : change default output
  return std::shared_ptr<Observation>(new Observation());
}

int Mission::getItemCount() const {
  int res = 0;
  std::vector<std::shared_ptr<Observation>>::const_iterator itObs;
  for (itObs = m_obsList.begin(); itObs != m_obsList.end(); itObs++) {
    res += (*itObs)->getItemCount();
  }
  for (size_t k = 0; k < m_subFolders.size(); k++) {
    res += m_subFolders[k]->getItemCount();
  }
  return res;
}

std::string Mission::getFolderPath() {
  return m_folderPath;
}

std::weak_ptr<Mission> Mission::getSubFolder(std::string _path, bool &found) {
  for (size_t i = 0; i < m_subFolders.size(); i++) {
    if (m_subFolders[i]->getFolderPath() == _path) {
      found = true;
      return std::weak_ptr<Mission>(m_subFolders[i]);
    } else {
      std::weak_ptr<Mission> temp = m_subFolders[i]->getSubFolder(_path, found);
      if (found)
        return temp;
    }
  }
  return std::weak_ptr<Mission>(std::weak_ptr<Mission>());
}

int Mission::getSubFoldersCount() {
  return static_cast<int>(m_subFolders.size());
}

std::vector<std::string> Mission::getSpeciesNames() {
  std::vector<std::string> res;
  std::vector<species>::iterator it;
  for (auto& x : m_speciesList) {
    res.push_back(x.first);
  }
  return res;
}

std::vector<std::string> Mission::getObsPath() {
  std::vector<std::string> res;
  for (size_t i = 0; i < m_obsList.size(); i++) {
    res.push_back(m_obsList[i]->getFilePath());
  }
  return res;
}

std::map<std::string, Mission::species> Mission::getSpecies() {
  return m_speciesList;
}

Mission::species Mission::getSpecies(std::string _speciesName) {
  Mission::species res;
  if (isStringIn(_speciesName, getSpeciesNames())) {
    res = m_speciesList[_speciesName];
  }
  return res;
}

std::weak_ptr<Mission> Mission::getSubFolder(int _iter) {
  return std::weak_ptr<Mission>(m_subFolders[_iter]);
}

int Mission::getSubFolderCount() {
  return static_cast<int>(m_subFolders.size());
}

// setters
bool Mission::setName(std::string _name) {
  m_name = _name;
  return true;
}

bool Mission::setUser(std::string _userName) {
  m_user = _userName;
  return true;
}

std::string Mission::getUserName() {
  return m_user;
}

void Mission::isImgIn(std::string _fullPath, int &res, bool &found) {
  std::vector<std::shared_ptr<Observation>>::iterator it;
  for (it = m_obsList.begin(); it < m_obsList.end() ; it++) {
    if ((*it)->getFilePath() == _fullPath) {
      found = true;
      return;
    }
  }
  std::vector<std::shared_ptr<Mission>>::iterator jt;
  for (jt = m_subFolders.begin(); jt < m_subFolders.end(); jt++) {
    res +=1;
    (*jt)->isImgIn(_fullPath, res, found);
    if (found) {break;}
  }
  return;
}

void Mission::setSpeciesColor(std::string _speciesName, std::string _color) {
  CLOG(TRACE, "cor_mi") << "Process : set species color for species="
                        << _speciesName << "; color=" << _color;
  if (isStringIn(_speciesName, getSpeciesNames())) {
    m_speciesList[_speciesName].color = _color;
  }
  return;
}

void Mission::changeSpeciesName(std::string _speciesOldName,
                             std::string _speciesNewName) {
  CLOG(TRACE, "cor_mi") << "Process : change speciesName from species="
                        << _speciesOldName << " to species="
                        << _speciesNewName << " in mission=" << m_name;
  for (size_t i = 0; i < m_subFolders.size(); i++) {
    m_subFolders[i]->changeSpeciesName(_speciesOldName, _speciesNewName);
  }
  if (isStringIn(_speciesOldName, getSpeciesNames())) {
    m_speciesList[_speciesOldName].name = _speciesNewName;
    Mission::species temp = m_speciesList[_speciesOldName];
    m_speciesList.erase(_speciesOldName);
    m_speciesList.emplace(_speciesNewName, temp);
  }
  for (size_t i = 0; i < m_obsList.size(); i++) {
    m_obsList[i]->changeSpeciesName(_speciesOldName, _speciesNewName);
  }
  return;
}


void Mission::setFolderPath(std::string _path) {
  CLOG(TRACE, "cor_mi") << "Process : set folderPath="
                        << _path << " to mission=" << m_name;
  m_folderPath = _path;
}


