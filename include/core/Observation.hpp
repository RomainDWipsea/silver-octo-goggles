/*
The file Observation.hpp is part of the Harmony software.

Harmony is a software that provides tools for the detection of wildlife in series of images.

Copyright 2014-2016 WIPSEA SARL, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- OpenCV : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
- Boost : custom license => http://www.boost.org/users/license.html
*/

#ifndef OBSERVATION_HPP
#define OBSERVATION_HPP

#include <string>
#include <vector>

#include "Item.hpp"
#include "opencv2/core/core.hpp"

class Observation {
  friend class Mission;
 public:
  /*!
   * \brief Constructor
   *
   * Class constructor
   *
   * \param 
   *
   * \return 
   */
  Observation();

  /*!
   * \brief Destructor
   *
   * Class destructor
   *
   * \param 
   *
   * \return 
   */
  ~Observation();

  /*!
   * \brief Constructor
   *
   * Copy constructor
   *
   * \param -observationToCopy : observation to copy
   *
   * \return 
   */
  Observation(Observation const& _Observation);

  /*!
   * \brief Operator overloading
   *
   * overloading copy operator
   *
   * \param -observationToCopy : observation to copy
   *
   * \return 
   */
  Observation& operator=(Observation const& _observationToCopy);

  /*!
   * \brief Constructor
   *
   * Constructor initialising the file absolute path
   *
   * \param -fileAbsolutePathp : file absolute path
   *
   * \return 
   */
  explicit Observation(std::string const _fileAbsolutePathp);

    /*!
   * \brief Add species
   *
   * add a species to the observation
   *
   * \param -speciesName : species name
   *
   * \return bool : true if successful
   */
  bool addSpecies(std::string const _speciesName);

  /*!
   * \brief remove species
   *
   * Remove a species from the observation by its name
   *
   * \param -speciesName : species to remove
   *
   * \return bool : true if successful
   */
  bool removeSpecies(std::string const _speciesName);

  /*!
   * \brief add an item 
   *
   * add itme to the obsevation given all parameters
   *
   * \param _center : position of the item in the image
   * \param _size : size of the bounding box (supposed sqaure)
   * \param _species : name of the item (species for animals)
   * \param _score : probability for the item to belong to _species (default 1)
   * \return 
   */
  bool addItem(cv::Point _center,
               int _size,
               std::string _species,
               float _score = 1,
               bool _automatic = false);
  bool addItem(Item _item);

  /*!
   * \brief add an item
   *
   * same as addItem but including multiple species and scores, adapted to read files from caffe
   *
   * \param _center : center point of the item
   * \param _size : size of the item (square side)
   * \param _species : Possible species of the item
   * \param _score : scores corresponding to the species (in the same order)
   *
   * \return true if successful
   */bool addItemMultSpecies(cv::Point _center,
                             int _size,
                             std::vector<std::string> _species,
                             std::vector<float> _score,
                             bool _automatic = false);

  /*!
   * \brief remove an animal
   *
   * remove a species member in the observation
   *
   * \param - speciesName : species to decrement
   *        - iter : Number of the member to remove in the species vector    
   *
   * \return bool : true if successful
   */
  bool removeMember(std::string const& _speciesName, int _iter);

  /*!
   * \brief remove a detection
   *
   * Remove an item from the observation, first you have to find the iteration
   *
   * \param _iter : iteration of the item in the Item vector : m_items.
   *
   * \return true if successful
   */
  bool removeItem(int _iter);

  /*!
   * \brief clean observation
   *
   * Remove all detections from the observation
   *
   * \param 
   *
   * \return 
   */
  void cleanSpecies();
  
  /*!
   * \brief read from json
   *
   * read a json object as an Observation
   *
   * \param - jsonObject : object to read
   *
   * \return bool : true if successful
   */
  bool readJson(Json::Value const& _jsonObject);

  /*!
   * \brief read from json 
   *
   * read a json item as an observation, it will increment the mission's species list if a new species is encountered (_speciesList should be the m_speciesList from the Mission class)
   *
   * \param _jsonObject : object to read
   * \param _speciesList : speciesList from the mission
   *
   * \return true if successful
   */
  bool readJsonItem(Json::Value const& _jsonObject,
                    std::vector<std::string>* _speciesList);

    /*!
   * \brief read from json 
   *
   * read a json item as an observation (used for version >= 1.0.0)
   *
   * \param _jsonObject : object to read
   * \param _speciesList : speciesList from the mission
   *
   * \return true if successful
   */
  bool readJsonItem(Json::Value const& _jsonObject);
  bool readJsonItemv110(Json::Value const& _jsonObject);

  /*!
   * \brief write json object
   *
   * Write an observation as a json object
   *
   * \param - jsonObject : object to write in
   *
   * \return bool : true if successful
   */
  bool writeJson(Json::Value *_jsonObject) const;

  /*!
   * \brief write json object
   *
   * write an observation as a json object according to the item version
   *
   * \param _jsonObject : object to write into
   *
   * \return true if successful
   */
  bool writeJsonItem(Json::Value *_jsonObject) const;

  /*!
   * \brief set species threshold
   *
   * Set the threshold for a given species to be considered as detected. If their score
   * is lower than this threshold, they might turn to noClass if they don't have any other
   * species score > than the species' threshold.
   *
   * \param -_species : speciic threshold we must change
   *        -_threshold : new value of the threshold
   *
   * \return true if successfull
   */
  bool setThreshold(std::string _species, float _threshold);
  
  float getThreshold(std::string _species);
  std::vector<std::string> getSpeciesName();
  std::vector<Item> getItems(std::string _species);



  enum ExtractionMode {
    RAW,
    BALANCED,
    MULTPOS
  };
  
  /*!
   * \brief Extract patches
   *
   * Extract the object as subImages per species and members
   *
   * \param - path : folder where the subimages will be saved
   *
   * \return 
   */
  void extractPatches(std::string _path, int _obsNumber,
                      std::ofstream* _file, ExtractionMode _mode) const;
  void extractPatchesItem(std::string _path, int _obsNumber,
                          std::ofstream* _file, ExtractionMode _mode) const;
  void extraction(std::string _path,
                  const cv::Mat& _input,
                  int _obsNumber,
                  std::ofstream *_file,
                  bool _rotation,
                  int _nbPatch = -1) const;
  
  
  /*!
   * \brief Compare to another observation
   *
   * This function's first goal is to provide a tool for ground truth comparison. It compares the species and the location of the members identified with a precision of _pre in pixels. Will also update the status of the observation according to the comparison result.
   *
   * \param _pre : maximum distance between two member's center to be validated
   *        _gtObs : groundTruth Observation 
   *
   * \return 0 if OK
   */
  int classify(Observation const& _gtObs, int _pre = 40);

  /*!
   * \brief give a label to the detections
   *
   * Compare the current observation to a ground truth observation to set a label. The label is given as a new species with probability 1 in the item m_itemName field
   *
   * \param _gtObs : groundTruth observation
   * \param pre : distance in pixel to considered to judge if 2 locations are supposed to be linked to the same item
   *
   * \return 0 if succesfull
   */
  int label(Observation const& _gtObs, int _pre = 40);

  /*!
   * \brief change species name
   *
   * Replace a species name with a new name
   *
   * \param _oldName : Current name
   * \param _newName : Name to be set
   */
  void changeSpeciesName(std::string _oldName, std::string _newName);

  /*!
   * \brief set path
   *
   * set the absolute file path
   *
   * \param filePath
   *
   * \return bool : true if successful
   */
  bool setFilePath(std::string const _fileAbsolutePath);

  /*!
   * \brief compare 2 observations
   *
   * Compare 2 observation and return the updated confusion matrix
   * Suppose that we have 2 class c1 and c2, here is the matrix we are filling :
   * t : true detection
   * m : miss detection
   * ND: not detected
   * FD: false detection
   * x : blank case
   *  ----GroundTruth---->  
   *  |   |c1|c2|FD
   *  o c1|t |m |f
   *  b c2|m |t |f
   *  s ND|n |n |x
   *  |
   *
   * \param _confusionMat : results
   * \param _obsToCompare
   *
   * \return 
   */
  void compareItems(std::vector<std::vector<int>>* _confusionMat,
                    std::vector<std::string> speciesList,
                    Observation _obsToCompare,
                    double _threshold = 0.5);
  int findTruePos(Observation _obsToCompare);
  int findFalsePos(Observation _obsToCompare);
  int findMissDet(Observation _obsToCompare);

  /*!
   * \brief getn
   *
   * get a species count in the observation
   *
   * \param 
   *
   * \return 
   */
  int getSpeciesCount(std::string _species) const;

  /*!
   * \brief get info
   *
   * Get the number of true detection in the image so that we can extract as many false
   *
   * \param 
   *
   * \return sum of all the true detection (all species but false)
   */
  int getTrueMembers() const;

  /*!
   * \brief get number of item
   *
   * get the number of items in the image
   *
   * \param 
   *
   * \return 
   */
  int getItemCount() const;
  
  /*!
   * \brief get access
   *
   * get filePath
   *
   * \param 
   *
   * \return std::string : file absolute path
   */
  std::string getFilePath() const;
  bool clearAutomaticDetections();


  /*!
   * \brief get 
   *
   * Get species' name
   *
   * \param 
   *
   * \return std::vector<std::string> : species' names
   */
  std::vector<std::string> getSpeciesName() const;

  /*!
   * \brief get Item names
   *
   * get all possible name for the Observation items
   *
   * \param 
   *
   * \return list of names
   */
  std::vector<std::string> getItemsName() const;

  /*!
   * \brief get
   *
   * get the total number of object in the observation (whatever species)
   *
   * \param XB
   *
   * \return int : object number
   */
  int getMemberSize() const;

  /*!
   * \brief get
   *
   * get the vector of members of a given species in the observation
   *
   * \param 
   *
   * \return 
   */
  std::vector<cv::Rect> getSpeciesMembers(std::string _speciesName) const;

  /*!
   * \brief get item
   *
   * get a specific Item by passing it's center position
   *
   * \param _center : center position of the item (attribute)
   *
   * \return pointer to the given item
   */
  Item* getItem(cv::Point _center);

  /*!
   * \brief get item
   *
   * get a specific item from its iteration number, for internal purposes
   *
   * \param _iter : iteration in the m_items vector
   *
   * \return 
   */
  Item* getItem(int _iter);



 private:
  //! Image path
  std::string m_fileAbsolutePath;

  //! items in the observation
  std::vector<Item> m_items;
};

#endif /* OBSERVATION_HPP */
