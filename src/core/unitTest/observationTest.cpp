/*
The file observationTest.cpp is part of the Harmony software.

Harmony is a software that provides tools for the detection of wildlife in series of images.

Copyright 2014-2016 WIPSEA SARL, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- Boost : custom license => http://www.boost.org/users/license.html
- OpenCV 3.0.0 : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
*/

#include <gmock/gmock.h>
  using ::testing::Eq;
#include <gtest/gtest.h>
  using ::testing::Test;

#include <string>
#include <vector>
#include <fstream>
#include "Observation.hpp"

namespace testing {
class ObservationTest : public Test {
 protected:
ObservationTest() {}
~ObservationTest() {}

virtual void SetUp() {}
virtual void TearDown() {}
Observation myObservation;
};

TEST_F(ObservationTest, constructor_withName) {
  std::string imgPath("/home/romain/code/wildlifeCounter/images/logo.png");
  Observation testObservation(imgPath);
  EXPECT_THAT(testObservation.getFilePath(), Eq(imgPath));
  EXPECT_THAT(testObservation.getItemCount(), Eq(static_cast<int>(0)));
}

TEST_F(ObservationTest, copy_constructor) {
  std::string imgPath("/home/romain/code/wildlifeCounter/images/logo.png");
  myObservation.setFilePath(imgPath);
  Observation testObservation(myObservation);
  EXPECT_THAT(testObservation.getFilePath(), Eq(imgPath));
  EXPECT_THAT(testObservation.getItemCount(), Eq(static_cast<int>(0)));
}

TEST_F(ObservationTest, copy_operator) {
  std::string imgPath("/home/romain/code/wildlifeCounter/images/logo.png");
  myObservation.setFilePath(imgPath);
  Observation testObservation = myObservation;
  EXPECT_THAT(testObservation.getFilePath(), Eq(imgPath));
  EXPECT_THAT(testObservation.getItemCount(), Eq(static_cast<int>(0)));
}

TEST_F(ObservationTest, addremoveItem) {
  myObservation.addItem(cv::Point(50, 50), 4, "Turtle", 1);
  myObservation.addItem(cv::Point(50, 25), 5, "Turtle", 1);
  EXPECT_THAT(myObservation.getItemCount(), Eq(2));  // test addMember
  myObservation.removeItem(0);
  EXPECT_THAT(myObservation.getItemCount(), Eq(1));  // test removeMember
  EXPECT_THAT(myObservation.getItemCount(), Eq(1));
  Item it = *(myObservation.getItem(0));
  EXPECT_THAT(it.getClass(), Eq("Turtle"));
}

TEST_F(ObservationTest, writeReadJsonItem) {
  std::string imgPath("/home/romain/code/wildlifeCounter/images/logo.png");
  myObservation.setFilePath(imgPath);
  myObservation.addItem(cv::Point(50, 50), 4, "Turtle", 1);
  myObservation.addItem(cv::Point(50, 25), 5, "Turtle", 1);
  Json::Value save;
  myObservation.writeJsonItem(&save);
  std::ofstream out("testObservationItem.txt", std::ofstream::out);
  out << save;
  out.close();

  std::ifstream in("testObservationItem.txt");
  Json::Value reading;
  in >> reading;
  Observation test;
  std::vector<std::string>* speciesList = new std::vector<std::string>;
  test.readJsonItem(reading, speciesList);
  delete speciesList;
  in.close();

  EXPECT_THAT(myObservation.getItemCount(), Eq(2));
  EXPECT_THAT(myObservation.getFilePath(), Eq(imgPath));
  Item it = *(myObservation.getItem(0));
  EXPECT_THAT(it.getClass(), Eq("Turtle"));
}






}  // namespace testing
