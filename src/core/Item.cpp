/*
The file Item.cpp is part of the Harmony software.

Harmony is a software that provides tools for the detection of wildlife in series of images.

Copyright 2014-2016 WIPSEA SARL, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2016/05/26

Tous droits réservés - All rights reserved

Library used :
- OpenCV 3.0.0 : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
- Boost : custom license => http://www.boost.org/users/license.html
*/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Item.hpp"
#include "misc.hpp"
#include "./json.h"
//#include "./easylogging++.h"

const int DEFAULT_SIZE = 100;
const float DEFAULT_THRESHOLD = 0.5;
const bool DEFAULT_AUTOMATIC = false;
Item::Item() {
}

Item::~Item() {
}

Item::Item(std::string _imageName, cv::Point _center) :
    m_imageName(_imageName) {
  //CLOG(TRACE, "cor_it") << "Process : creating Item at x="
  //                        << _center.x << " y=" << _center.y
  //                    << " for image =" << _imageName;
  m_bbox = cv::Rect(_center.x-DEFAULT_SIZE/2, _center.y -DEFAULT_SIZE/2,
                    DEFAULT_SIZE, DEFAULT_SIZE);
  updateClass();  // will simply set the class to "noClass"
}


Item::Item(std::string _imageName, cv::Point _center, std::string _species) :
  m_imageName(_imageName) {
	//CLOG(TRACE, "cor_it") << "Process : creating Item="<< _species <<" at x=" << _center.x << " y=" << _center.y << " for image =" << _imageName;
  m_bbox = cv::Rect(_center.x-DEFAULT_SIZE/2, _center.y -DEFAULT_SIZE/2,
                    DEFAULT_SIZE, DEFAULT_SIZE);
  m_classes[_species].score = 1;
  m_classes[_species].threshold = DEFAULT_THRESHOLD;
  m_classes[_species].automatic = DEFAULT_AUTOMATIC;
  updateClass();
}

Item::Item(std::string _imageName, cv::Point _center,
           std::string _species, float _score, bool _automatic) :
    m_imageName(_imageName) {
	//CLOG(TRACE, "cor_it") << "Process : creating Item=" << _species << " at x=" << _center.x << " y=" << _center.y << " for image =" << _imageName;
  m_bbox = cv::Rect(_center.x-DEFAULT_SIZE/2, _center.y -DEFAULT_SIZE/2,
                    DEFAULT_SIZE, DEFAULT_SIZE);
  m_classes[_species].score = _score;
  m_classes[_species].threshold = DEFAULT_THRESHOLD;
  m_classes[_species].automatic = _automatic;
  updateClass();
}

Item::Item(std::string _imageName, cv::Point _center,
           int _size, std::string _species, float _score,
           bool _automatic) :
    m_imageName(_imageName) {
	//CLOG(TRACE, "cor_it") << "Process : creating Item=" << _species << " at x=" << _center.x << " y=" << _center.y << " for image =" << _imageName;
  m_bbox = cv::Rect(_center.x-_size/2, _center.y -_size/2,
                    _size, _size);
  m_classes[_species].score = _score;
  m_classes[_species].threshold = DEFAULT_THRESHOLD;
  m_classes[_species].automatic = _automatic;
  updateClass();
}

Item::Item(std::string _imageName, cv::Point _center,
           int _size, std::vector<std::string> _species,
           std::vector<float> _score, bool _automatic) :
    m_imageName(_imageName) {
	//CLOG(TRACE, "cor_it") << "Process : creating Item at x=" << _center.x << " y=" << _center.y << " for image =" << _imageName;
  m_bbox = cv::Rect(_center.x-_size/2, _center.y -_size/2,
                    _size, _size);

  for (size_t j = 0; j < _species.size(); j++) {
    m_classes[_species[j]].score = _score[j];
    m_classes[_species[j]].threshold = DEFAULT_THRESHOLD;
    m_classes[_species[j]].automatic = _automatic;
  }
  updateClass();
}

Item::Item(std::string _imageName, cv::Rect _boundingBox,
           std::string _species, float _score, bool _automatic) :
  m_imageName(_imageName) {
	//CLOG(TRACE, "cor_it") << "Process : creating Item=" << _species << " for image =" << _imageName;
  m_bbox = _boundingBox;
  m_classes[_species].score = _score;
  m_classes[_species].threshold = DEFAULT_THRESHOLD;
  m_classes[_species].automatic = _automatic;
  updateClass();
}

Item::Item(Item const& _itemToCopy) :
  m_imageName(_itemToCopy.m_imageName),
  m_classes(_itemToCopy.m_classes),
  m_class(_itemToCopy.m_class),
  m_bbox(_itemToCopy.m_bbox) {
}

Item& Item::operator=(Item const& _itemToCopy) {
  if (this != &_itemToCopy) {
    m_imageName = _itemToCopy.m_imageName;
    m_bbox = _itemToCopy.m_bbox;
    m_classes = _itemToCopy.m_classes;
    m_class = _itemToCopy.m_class;
  }
  return *this;
}

bool Item::writeJson(Json::Value *_json) const {
  (*_json)["imageName"] = m_imageName;

  Json::Value memberObject;
  memberObject["tlx"] = m_bbox.x;
  memberObject["tly"] = m_bbox.y;
  memberObject["width"] = m_bbox.width;
  memberObject["height"] = m_bbox.height;
  (*_json)["bbox"] = memberObject;

  Json::Value membersArray;
  for (auto& x : m_classes) {
    Json::Value objSpec;
    objSpec["species"] = x.first;
    objSpec["score"] = std::to_string(x.second.score);
    objSpec["threshold"] = std::to_string(x.second.threshold);
    objSpec["automatic"] = x.second.automatic;
    membersArray.append(objSpec);
  }
  (*_json)["classes"] = membersArray;
  (*_json)["class"] = m_class;
  return true;
}

bool Item::removeSpecies(std::string _species) {
  if (isStringIn(_species, getNames())) {
    m_classes.erase(m_classes.find(_species));
    updateClass();
    return true;
  }
  return false;
}

bool Item::readJsonv110(Json::Value const& _json) {
  m_imageName = _json["imageName"].asString();

  Json::Value memberObject = _json["bbox"];
  cv::Point center;
  m_bbox.x = memberObject["tlx"].asInt();
  m_bbox.y = memberObject["tly"].asInt();
  m_bbox.width = memberObject["width"].asInt();
  m_bbox.height = memberObject["height"].asInt();

  Json::Value membersArray = _json["classes"];
  for (int Index = 0; Index < static_cast<int>(membersArray.size()); ++Index) {
    Json::Value memberObj = membersArray[Index];
    std::string speciesName = memberObj["species"].asString();
    m_classes[speciesName].score =
        ::atof(memberObj["score"].asString().c_str());
    m_classes[speciesName].threshold =
        ::atof(memberObj["threshold"].asString().c_str());
    m_classes[speciesName].automatic = memberObj["automatic"].asBool();
  }

  m_class = _json["class"].asString();
  return true;
}

bool Item::readJson(Json::Value const& _json) {
  m_imageName = _json["imageName"].asString();

  // to be able to read the previous groundTruth (from windows version)
  std::string forCompatibility = _json["initial"].asString();

  cv::Point center;
  Json::Value memberObject = _json["center"];
  center.x = memberObject["x"].asDouble();
  center.y = memberObject["y"].asDouble();

  double size = _json["size"].asDouble();

  std::vector<std::string> itemName;
  Json::Value membersArray = _json["itemName"];
  for (int Index = 0; Index < static_cast<int>(membersArray.size()); ++Index) {
    Json::Value memberObj = membersArray[Index];
    std::string speciesName = memberObj["species"].asString();
    itemName.push_back(speciesName);
  }
  std::vector<float> score;
  Json::Value membersArray2 = _json["Scores"];
  for (int Index2 = 0;
       Index2 < static_cast<int>(membersArray2.size()); ++Index2) {
    Json::Value memberObj2 = membersArray2[Index2];
    score.push_back(memberObj2["score"].asFloat());
  }
  Item temp(m_imageName, center, size, itemName, score);
  *this = temp;
  return true;
}

bool Item::readJson(Json::Value const& _json,
                    std::vector<std::string>* _speciesList) {
  m_imageName = _json["imageName"].asString();

  // to be able to read the previous groundTruth (from windows version)
  std::string forCompatibility = _json["initial"].asString();

  Json::Value memberObject = _json["center"];
  cv::Point center;
  center.x = memberObject["x"].asDouble();
  center.y = memberObject["y"].asDouble();

  int size;
  size = _json["size"].asInt();

  std::vector<std::string> itemName;
  Json::Value membersArray = _json["itemName"];
  for (int Index = 0; Index < static_cast<int>(membersArray.size()); ++Index) {
    Json::Value memberObj = membersArray[Index];
    std::string speciesName = memberObj["species"].asString();
    itemName.push_back(speciesName);
    if (!isStringIn(speciesName, *_speciesList)) {
      _speciesList->push_back(speciesName);
    }
  }

  std::vector<float> score;
  Json::Value membersArray2 = _json["Scores"];
  for (int Index2 = 0;
       Index2 < static_cast<int>(membersArray2.size()); ++Index2) {
    Json::Value memberObj2 = membersArray2[Index2];
    score.push_back(memberObj2["score"].asFloat());
  }

  Item temp(m_imageName, center, size, itemName, score);
  *this = temp;
  return true;
}

float Item::maxScore() const {
  float maxScore = 0;
  for (auto& x : m_classes)
    maxScore = (x.second.score > maxScore) ? x.second.score : maxScore;
  return maxScore;
}

void Item::updateClass() {
	//CLOG(TRACE, "cor_it") << "Process : updateClass";
  float maxScore = 0;
  bool found(false);
  for (auto& x : m_classes) {
    if (x.second.score >= x.second.threshold && x.second.score > maxScore) {
      m_class = x.first;
      maxScore = x.second.score;
      found = true;
    }
  }
  if (!found)
    m_class = "noClass";
}

std::string Item::getClass() const {
  return m_class;
}

bool Item::isSpeciesIn(std::string _species) {
  for (auto& x : m_classes) {
    if (x.first == _species)
      return true;
  }
  return false;
}

Item::scores Item::getScores(std::string _species) {
  Item::scores res = m_classes[_species];
  return res;
}

std::string Item::getImageName() const {
  return m_imageName;
}

cv::Point Item::getCenter() const {
  cv::Point center(m_bbox.x + m_bbox.width / 2, m_bbox.y + m_bbox.height / 2);
  return center;
}

std::vector<std::string> Item::getNames() const {
  std::vector<std::string> res;
  for (auto& x : m_classes)
    res.push_back(x.first);
  return res;
}

cv::Rect Item::getBbox() const {
  return m_bbox;
}

int Item::setCenter(cv::Point _center) {
  cv::Point center = getCenter();
  m_bbox.x += _center.x - center.x;
  m_bbox.y += _center.y - center.y;
  return 0;
}

bool Item::setThreshold(std::string _speciesName, float _threshold) {
  if (isStringIn(_speciesName, getNames()) && check0to1(_threshold)) {
    m_classes[_speciesName].threshold = _threshold;
    updateClass();
    return true;
  }
  return false;
}

bool Item::setSize(int _width, int _height) {
  cv::Point center = getCenter();
  m_bbox = cv::Rect(center.x-_width/2, center.y -_height/2,
                    _width, _height);
  return true;
}

cv::Size Item::getSize() {
  return m_bbox.size();
}

bool Item::addClass(std::string _name, float _score) {
  Item::scores temp;
  if (check0to1(_score))
    temp.score = _score;
  else
    return false;
  temp.threshold = DEFAULT_THRESHOLD;
  m_classes.emplace(_name, temp);
  updateClass();
  return true;
}

void Item::changeSpeciesName(std::string _oldName, std::string _newName) {
  if (!isStringIn(_oldName, getNames()))
    return;
  std::map<std::string, scores>::iterator it;
  it = m_classes.find(_oldName);

  if (it != m_classes.end()) {
    // Swap value from oldKey to newKey, note that a default constructed value 
    // is created by operator[] if 'm' does not contain newKey.
    std::swap(m_classes[_newName], it->second);
    // Erase old key-value from map
    if (_oldName != _newName)
      m_classes.erase(it);
  }
  updateClass();
}

bool check0to1(float _test) {
  if (_test >= 0 && _test <= 1)
    return true;
  return false;
}

bool Item::setScore(std::string _name, float _score) {
  if (isStringIn(_name, getNames()) && check0to1(_score)) {
    m_classes[_name].score = _score;
    return true;
  }
  return false;
}


int Item::compare(Item const& _itemToCompare, double _threshold) const {
  double distance = cv::norm(this->getCenter() - _itemToCompare.getCenter());

  if (m_imageName != _itemToCompare.m_imageName)
    return 0;
  if (distance < 50) {
    if (maxScore() >= static_cast<float>(_threshold) &&
        _itemToCompare.maxScore() >= static_cast<float>(_threshold)) {
      if (getClass() == _itemToCompare.getClass()) {
        return 1;
      } else {
        return 2;
      }
    }
  }
  return 0;
}
