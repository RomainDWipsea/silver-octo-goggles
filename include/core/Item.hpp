/*
The file Item.hpp is part of the Harmony software.

Harmony is a software that provides tools for the detection of wildlife in series of images.

Copyright 2014-2016 WIPSEA SARL, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2016/05/26

Tous droits réservés - All rights reserved

Library used :
- OpenCV 3.0.0 : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
- Boost : custom license => http://www.boost.org/users/license.html
*/

#ifndef ITEM_HPP
#define ITEM_HPP

#include <string>
#include <vector>
#include <map>
#include "json.h"
#include "opencv2/core/core.hpp"

class Item {
 public:
  //! classification (species scores)
  struct scores {
    float score;
    float threshold;
    bool automatic;
  };

  /*!
   * \brief Constructor
   *
   * Class constructor
   *
   * \param 
   *
   * \return 
   */
  Item();

  /*!
   * \brief Constructor
   *
   * Class constructor
   *
   * \param 
   *
   * \return 
   */
  Item(std::string _imageName, cv::Point _center);

  Item(std::string _imageName, cv::Point _center, std::string _species);
  
  Item(std::string _imageName, cv::Point _center,
       std::string _species, float _score, bool _automatic = false);

  Item(std::string _imageName, cv::Point _center,
       int _size, std::string _species, float _score,
       bool _automatic = false);

  Item(std::string _imageName, cv::Point _center,
       int _size, std::vector<std::string> _species,
       std::vector<float> _score, bool _automatic = false);

  Item(std::string _imageName, cv::Rect _boundingBox,
       std::string _species, float _score, bool _automatic = false);

  /*!
   * \brief Constructor
   *
   * Copy constructor
   *
   * \param -itemToCopy : item to copy
   *
   * \return 
   */
  Item(Item const& _itemToCopy);

  /*!
   * \brief destructor
   *
   * Class desctructor
   *
   * \param 
   *
   * \return 
   */
  ~Item();

  /*!
   * \brief operator overlaod
   *
   * Overloading operator =
   *
   * \param -itemToCopy : item to copy
   *
   * \return Item : new copy of Item
  p */
  Item& operator=(Item const& _itemToCopy);

  /*!
   * \brief Write json object
   *
   * Write Species as a json object
   *
   * \param -*json : pointer to the json value to edit (species is writen within observation).
   *
   * \return bool : true if successfull
   */
  bool writeJson(Json::Value *_json) const;

  /*!
   * \brief Read json object
   *
   * Read a species from a json object (used for version < 1.0.0)
   *
   * \param -_json : pointer to the json object to read
   *        -_speciesList : list of speciesNames to be updated by the read item
   *
   * \return bool : true if successfull
   */
  bool readJson(Json::Value const& _json,
                std::vector<std::string>* _speciesList);

    /*!
   * \brief Read json object
   *
   * Read an item from a json object (used for version >= 1.0.0)
   *
   * \param -*json : pointer to the json object to read
   *
   * \return bool : true if successfull
   */
  bool readJson(Json::Value const& _json);

  /*!
   * \brief read json object
   *
   * Read an item from a json object (version > 1.1.0)
   *
   * \param - *json : pointer to the json object to read
   *
   * \return bool : true if successfull
   */
  bool readJsonv110(Json::Value const& _json);

  /*!
   * \brief define winning class
   *
   * Compare the scores and return the winning class for the item
   *
   * \param 
   *
   * \return string : winning name from m_itemName 
   */
  std::string getClass() const;

  /*!
   * \brief get Scores
   *
   * get the scores structures for a given species name
   *
   * \param 
   *
   * \return 
   */
  scores getScores(std::string _species);
  
  bool isSpeciesIn(std::string _species);

  /*!
   * \brief Best probability score
   *
   * returns the value of the most probable species score (to be used with maxClass)
   *
   * \param 
   *
   * \return float : score
   */
  float maxScore() const;

  /*!
   * \brief get image name
   *
   * 
   *
   * \param 
   *
   * \return 
   */
  std::string getImageName() const;

  /*!
   * \brief get class names
   *
   * An item can have multiple possible classes, this functions returns their names
   *
   * \param 
   *
   * \return 
   */
  std::vector<std::string> getNames() const;

  /*!
   * \brief get center
   *
   * 
   *
   * \param 
   *
   * \return 
   */
  cv::Point getCenter() const;

  /*!
   * \brief get
   *
   * 
   *
   * \param 
   *
   * \return 
   */
  cv::Rect getBbox() const;

  /*!
   * \brief set
   *
   * 
   *
   * \param center 
   *
   * \return 
   */
  int setCenter(cv::Point _center);

  /*!
   * \brief Modify a threshold
   *
   * Modify the threshold for species _speciesName to be considered as detected
   *
   * \param _speciesName : to be compared with m_classes.first
   *
   * \return true if successfull
   */
  bool setThreshold(std::string _speciesName, float _threshold);

  /*!
   * \brief add a new identifiaction
   *
   * set a new probable species with the corresponding score
   *
   * \param _name : name of the species
   * \param _score : associated score, default is 0
   *
   * \return 0
   */
  bool addClass(std::string _name, float _score = 0);

  bool removeSpecies(std::string _species);

  /*!
   * \brief change species name
   *
   * replace a species/object name 
   *
   * \param _oldName : current name
   * \param _newName : new name we want to set
   */
  void changeSpeciesName(std::string _oldName, std::string _newName);

  /*!
   * \brief update class
   *
   * update the identification with respect to the scores and thresholds
   *
   * \param 
   *
   * \return 
   */
  void updateClass();

  /*!
   * \brief set a new score
   *
   * Change the score value of an existing species in the list
   *
   * \param _name : name of the species
   * \param _score : new score to enter
   *
   * \return 
   */
  bool setScore(std::string _name, float _score);

  /*!
   * \brief set size 
   *
   * Change the size of the object without changing it's center
   *
   * \param - _width = new width
   *        - _height = newHeight
   *
   * \return true if successfull
   */
  bool setSize(int _width, int _height);

  cv::Size getSize();

  /*!
   * \brief Compare 2 items
   *
   * Compare 2 items and return an int according to the result :
   * - 0 : different
   * - 1 : same
   * - 2 : same center, wrong species
   *
   * \param _itemToCompare,
   * \param _thresh, threshold to consider a detection
   *
   * \return int status (read description)
   */
  int compare(Item const& _itemToCompare, double _threshold = 0.5) const;
  
 private:
  //! original image path
  std::string m_imageName;

  //! scores for each possible identifications
  std::map<std::string, scores> m_classes;

  //! Current ID
  std::string m_class;

  //! Size of the object
  cv::Rect m_bbox;
};

bool check0to1(float _test);
#endif /* ITEM_HPP */
