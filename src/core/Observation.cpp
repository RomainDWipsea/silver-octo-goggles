/*
The file Observation.cpp is part of the Harmony software.

Harmony is a software that provides tools for the detection of wildlife in series of images.

Copyright 2014-2015 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- Boost : custom license => http://www.boost.org/users/license.html
- OpenCV 3.0.0 : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
*/

#include "Observation.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include "Item.hpp"
#include "misc.hpp"
#include "easylogging++.h"

Observation::Observation() {
}

Observation::~Observation() {
}

Observation::Observation(std::string const _fileAbsolutePath) :
    m_fileAbsolutePath(_fileAbsolutePath) {
  CLOG(TRACE, "cor_ob") << "Process : create obs=" << _fileAbsolutePath;
}

Observation::Observation(Observation const& _observationToCopy) :
    m_fileAbsolutePath(_observationToCopy.m_fileAbsolutePath),
    m_items(_observationToCopy.m_items) {
}

Observation& Observation::operator=(Observation const& _observationToCopy) {
  if (this != &_observationToCopy) {
    m_fileAbsolutePath = _observationToCopy.m_fileAbsolutePath;
    m_items = _observationToCopy.m_items;
  }
  return *this;
}

bool Observation::addItem(cv::Point _center,
                          int _size,
                          std::string _species,
                          float _score, bool _automatic) {
  Item plop(m_fileAbsolutePath, _center, _size, _species, _score, _automatic);
  m_items.push_back(plop);
  return true;
}

bool Observation::addItem(Item _item) {
  m_items.push_back(_item);
  return true;
}

bool Observation::addItemMultSpecies(cv::Point _center,
                                     int _size,
                                     std::vector<std::string> _species,
                                     std::vector<float> _score,
                                     bool _automatic) {
  Item plop(m_fileAbsolutePath, _center, _size, _species, _score, _automatic);
  m_items.push_back(plop);
  return true;
}

#ifdef USE_ALGO
bool Observation::clearAutomaticDetections() {
  for (size_t i = 0; i < m_items.size(); i++) {
    if (m_items[i].isSpeciesIn("Turtle")) {
      if (m_items[i].getScores("Turtle").automatic) {
        m_items.erase(m_items.begin() + i);
        i--;
      }
    }
  }
  return true;
}
#endif  // USE_ALGO

bool Observation::removeItem(int _iter) {
  if (m_items[_iter].getScores(m_items[_iter].getClass()).automatic)
    m_items[_iter].addClass("falsePos", 1);
  else
    m_items.erase(m_items.begin()+_iter);
  return true;
}

bool Observation::readJsonItem(Json::Value const& _jsonObject,
                               std::vector<std::string>* _speciesList) {
  m_fileAbsolutePath = _jsonObject["filePath"].asString();
  m_items.clear();
  Json::Value itemArray = _jsonObject["items"];
  Json::Value::iterator it;
  for (it = itemArray.begin(); it != itemArray.end(); it++) {
    Json::Value itemObject = *it;
    Item item;
    item.readJson(itemObject, _speciesList);
    m_items.push_back(item);
  }
  return true;
}

bool Observation::readJsonItem(Json::Value const& _jsonObject) {
  m_fileAbsolutePath = _jsonObject["filePath"].asString();
  m_items.clear();
  Json::Value itemArray = _jsonObject["items"];
  Json::Value::iterator it;
  for (it = itemArray.begin(); it != itemArray.end(); it++) {
    Json::Value itemObject = *it;
    Item item;
    item.readJson(itemObject);
    m_items.push_back(item);
  }
  return true;
}

bool Observation::readJsonItemv110(Json::Value const& _jsonObject) {
  m_fileAbsolutePath = _jsonObject["filePath"].asString();
  m_items.clear();
  Json::Value itemArray = _jsonObject["items"];
  Json::Value::iterator it;
  for (it = itemArray.begin(); it != itemArray.end(); it++) {
    Json::Value itemObject = *it;
    Item item;
    item.readJsonv110(itemObject);
    m_items.push_back(item);
  }
  return true;
}

bool Observation::writeJsonItem(Json::Value *_jsonObject) const {
  (*_jsonObject)["filePath"] = m_fileAbsolutePath;
  Json::Value itemArray;
  std::vector<Item>::const_iterator it;
  for (it = m_items.begin(); it != m_items.end(); it++) {
    Json::Value itemObject;
    Item item = *it;
    item.writeJson(&itemObject);
    itemArray.append(itemObject);
  }
  (*_jsonObject)["items"] = itemArray;
  return true;
}

bool Observation::removeSpecies(std::string const _speciesName) {
  std::vector<int> index2remove;
  for (size_t i = 0 ; i < m_items.size(); i++)
    m_items[i].removeSpecies(_speciesName);
  return true;
}

void Observation::cleanSpecies() {
  m_items.clear();
}

void Observation::changeSpeciesName(std::string _oldName,
                                    std::string _newName) {
  for (size_t i = 0; i < m_items.size(); i++) {
    m_items[i].changeSpeciesName(_oldName, _newName);
  }
}

bool Observation::setThreshold(std::string _species, float _threshold) {
  for (size_t i = 0; i < m_items.size(); i++)
    m_items[i].setThreshold(_species, _threshold);
  return true;
}

// gets
float Observation::getThreshold(std::string _species) {
  for (size_t i = 0; i < m_items.size(); i++) {
    if (m_items[i].isSpeciesIn(_species))
      return m_items[i].getScores(_species).threshold;
  }
  return static_cast<float>(0.5);
}

int Observation::getSpeciesCount(std::string _species) const {
  int res = 0;
  for (size_t i = 0; i < m_items.size(); i++)
    res = (m_items[i].getClass() == _species) ? res+1 : res;
  return res;
}

std::vector<std::string> Observation::getSpeciesName() {
  std::vector<std::string> res;
  for (size_t i = 0; i < m_items.size(); i++) {
    if (!isStringIn(m_items[i].getClass(), res))
      res.push_back(m_items[i].getClass());
  }
  return res;
}

std::vector<Item> Observation::getItems(std::string _species) {
  std::vector<Item> res;
  for (size_t i = 0; i < m_items.size(); i++) {
    if (_species == m_items[i].getClass())
      res.push_back(*(getItem(i)));
  }
  return res;
}

std::string Observation::getFilePath() const {
  return m_fileAbsolutePath;
}

int Observation::getItemCount() const {
  int res = static_cast<int>(m_items.size());
  return res;
}

std::vector<std::string> Observation::getItemsName() const {
  std::vector<std::string> speciesName;
  std::vector<Item>::const_iterator it;
  for (it = m_items.begin(); it != m_items.end(); it++) {
    std::vector<std::string> itemNames = it->getNames();
    std::vector<std::string>::iterator itString;
    for (itString = itemNames.begin();
         itString != itemNames.end(); itString++) {
      if (!isStringIn(*itString, speciesName)) {
        speciesName.push_back(*itString);
      }
    }
  }
  return speciesName;
}

Item* Observation::getItem(int _iter) {
  return &m_items[_iter];
}

Item* Observation::getItem(cv::Point _center) {
  for (size_t i = 0; i < m_items.size(); i++) {
    if (m_items[i].getCenter() == _center)
      return &m_items[i];
  }
  Item *falseItem = 0;
  return falseItem;
}

// sets
bool Observation::setFilePath(std::string const _fileAbsolutePath) {
  m_fileAbsolutePath = _fileAbsolutePath;
  if (m_fileAbsolutePath == _fileAbsolutePath)
    return true;
  return false;
}



// Full Version
#ifdef USE_ALGO

void Observation::extractPatchesItem(std::string _path,
                                 int _obsNumber,
                                 std::ofstream* _file,
                                 ExtractionMode _mode) const {
  cv::Mat in = cv::imread(m_fileAbsolutePath);
  if (in.empty()) {
    std::cerr << "Can't open the file : " << m_fileAbsolutePath << "\n";
  }
  // if (m_fileAbsolutePath.substr(static_cast<int>(
  //         m_fileAbsolutePath.size())-3, 3) == "tif") {
  //   in = geoTransform(in);
  // }
  switch (_mode) {
    case ExtractionMode::RAW:
      extraction(_path, in, _obsNumber, _file, false);
      break;
      /*    case ExtractionMode::BALANCED:
      if (it->getName() != "false" && it->getName() != "Object") {
        it->extractPatches(_path, in, _obsNumber, _file, false);
      } else {
        int nbFalse = getTrueMembers();
        it->extractPatches(_path, in, _obsNumber, _file, false, nbFalse);
      }
      break;
    case ExtractionMode::MULTPOS:
      if (it->getName() != "false" && it->getName() != "Object") {
        it->extractPatches(_path, in, _obsNumber, _file, true);
      } else {
        int nbFalse = getTrueMembers();
        // multiply by 8 because of the rotations for other classes...
        it->extractPatches(_path, in, _obsNumber, _file, false, nbFalse*8);
      }
      break;*/
    default:
      std::cout << "An error as occured when trying to extract the patches."
                << std::endl;
  }
}

void Observation::extraction(std::string _path,
                             const cv::Mat& _input,
                             int _obsNumber,
                             std::ofstream *_file,
                             bool _rotation,
                             int _nbPatch) const {
  if (_nbPatch == -1) {
    _nbPatch = static_cast<int>(m_items.size());
  } else {
    // nbPatch should never exceed m_member.size()
    _nbPatch = std::min(_nbPatch, static_cast<int>(m_items.size()));
  }
  for (int i = 0; i < _nbPatch; i++) {
    // set rotation to true to get multiple rotations of the patch for training
    cv::Rect currentPatch(m_items[i].getCenter().x -50,
                          m_items[i].getCenter().y -50,
                          100, 100);
     if (_rotation)
       std::cout << "Just to use it for compiling";

    //   for (int r = 0; r < 8; r++) {  // 8 to get angles every 45°
    //     int angle = 45*r;

    //     cv::Mat rotImage = rotatePatch(_input, currentPatch, angle);

    //     std::string name = _path + "/obs" +
    //         std::to_string(_obsNumber) + "patch" +
    //         std::to_string(i) + "_rot" +
    //         std::to_string(angle) + "_" + m_items[i].getClass() + ".jpg";

    //     cv::imwrite(name, rotImage);

    //     // train.txt writing : file for caffe
    //     if (*_file) {

    //       *_file << _path + "/obs"<< _obsNumber << "patch"
    //              << std::to_string(i) +
    //           "_rot" + std::to_string(angle) + "_" + m_items[i].maxClass()
    //              << ".jpg," << _obsNumber  << ","  << m_items[i].getCenter().x
    //              << "," << m_items[i].getCenter().y << ","
    //              << m_items[i].maxClass() << ",0," << "\n";
    //       // 0 stands for the status, there is no equivalent with item class
    //     } else {
    //       std::cerr << "Can't open file !" << std::endl;
    //     }
    //   }
    // } else {
    cv::Mat patch = _input(currentPatch);
    std::string name = _path + "/obs" + std::to_string(_obsNumber) + "patch" +
        std::to_string(i) + "_" +  m_items[i].getClass() + ".jpg";
    cv::imwrite(name, patch);
      
    // train.txt writing : file for caffe
    if (*_file) {
      *_file << _path + "/obs"<< _obsNumber << "patch" << std::to_string(i) +
          "_" + m_items[i].getClass() << ".jpg," << _obsNumber  << ","
             << m_items[i].getCenter().x << ","
             << m_items[i].getCenter().y << "," <<
          m_items[i].getClass() << ",0,"<< "\n";
      // 0 stands for the old m_status, no equivalent with item class
    } else {
      std::cerr << "Can't open file !" << std::endl;
    }
    //}
  }
}
#endif  // USE_ALGO
int Observation::findTruePos(Observation _obsToCompare) {
  int tp = 0;
  std::vector<Item> turtles = getItems("Turtle");
  std::vector<Item> gt = _obsToCompare.getItems("Turtle");
  for (size_t i = 0; i < turtles.size(); i++) {
    for (size_t j = 0; j < gt.size(); j++) {
      if (turtles[i].compare(gt[j]) == 1) {
        tp += 1;
        break;
      }
    }
  }
  return tp;
}

int Observation::findFalsePos(Observation _obsToCompare) {
  int fp = 0;
  std::vector<Item> turtles = getItems("Turtle");
  std::vector<Item> gt = _obsToCompare.getItems("Turtle");
  for (size_t i = 0; i < turtles.size(); i++) {
    bool found = false;
    for (size_t j = 0; j < gt.size(); j++) {
      if (turtles[i].compare(gt[j]) == 1) {
        found = true;
        break;
      }
    }
    if (!found) {
      fp +=1;
    }
  }
  return fp;
}
int Observation::findMissDet(Observation _obsToCompare) {
  int md = 0;
  std::vector<Item> turtles = getItems("Turtle");
  std::vector<Item> gt = _obsToCompare.getItems("Turtle");
  for (size_t i = 0; i < gt.size(); i++) {
    bool found = false;
    for (size_t j = 0; j < turtles.size(); j++) {
      if (gt[i].compare(turtles[j]) == 1) {
        found = true;
        break;
      }
    }
    if (!found) {
      md +=1;
    }
  }
  return md;
}
void Observation::compareItems(std::vector<std::vector<int>>* _confusionMat,
                  std::vector<std::string> _speciesList,
                               Observation _obsToCompare,
                               double _threshold) {
  // vector initialisation
  std::vector<Item> detections = m_items;
  std::vector<Item> gt = _obsToCompare.m_items;
  std::vector<Item> falseDetection;
  std::vector<Item> detected;

  for (size_t i = 0; i < detections.size(); i++) {
    bool matched = false;
    int detSpecIt = 0;  // detections[i] species iteration
    for (size_t itSpec = 0; itSpec < _speciesList.size(); itSpec++) {
      if (detections[i].getClass() == _speciesList[itSpec]) {
        detSpecIt = itSpec;
        break;
      }
    }
    for (size_t j = 0; j < gt.size(); j++) {
      // get species iteration of the items treated
      int gtSpecIt = 0;  // gt[j] species iteration
      for (size_t itSpec = 0; itSpec < _speciesList.size(); itSpec++) {
        if (gt[j].getClass() == _speciesList[itSpec]) {
          gtSpecIt = itSpec;
          break;
        }
      }
      // compare items
      if (detections[i].compare(gt[j], _threshold) != 0) {  // fill m or t
        (*_confusionMat)[gtSpecIt][detSpecIt]+=1;
        // transfer gt item from gt to detected
        detected.push_back(gt[j]);
        gt.erase(gt.begin() + j);
        matched = true;
        break;
      }
    }
    if (!matched) {
      (*_confusionMat)[_speciesList.size()][detSpecIt]+=1;
      falseDetection.push_back(detections[i]);
    }
  }
  // Now we need to count the not detected object
  for (size_t ndIt = 0; ndIt < gt.size(); ndIt++) {
    int ndSpecIt = 0;  // gt[j] species iteration
    for (size_t itSpec = 0; itSpec < _speciesList.size(); itSpec++) {
      if (gt[ndIt].getClass() == _speciesList[itSpec]) {
        ndSpecIt = itSpec;
        break;
      }
    }
    (*_confusionMat)[ndSpecIt][_speciesList.size()]+=1;
  }
}

//#ifdef USE_RESEARCH

int Observation::label(Observation const& _gtObs, int _pre) {
  std::vector<std::string> speciesNamesg = _gtObs.getItemsName();

  // for all the detection in the current obs

  std::vector<Item>::iterator it;
  for (it = m_items.begin(); it != m_items.end(); it++) {
    // info about the member we are studying in the loop
      bool positive = false;
      std::vector<Item>::const_iterator itGt;
      for (itGt = _gtObs.m_items.begin();
           itGt != _gtObs.m_items.end(); itGt++) {
        cv::Point vtTemp = itGt->getCenter();
        cv::Point resTemp = it->getCenter();
        double distance = cv::norm(vtTemp - resTemp);
        if (distance < _pre && !positive) {
          positive = true;
          it->addClass(itGt->getClass(), itGt->maxScore());
          break;
        }
      }
      // if no groundTruth matched, then the detection is unvalidated

      if (!positive) {
        it->addClass("false", 1);
      }
    }
  return 0;
}


//#endif  // USE_RESEARCH
