/*
The file MissionTest.cpp is part of the Harmony software.

Harmony is a software that provides tools for the detection of wildlife in series of images.

Copyright 2014-2016 WIPSEA SARL, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- OpenCV 3.0.0 : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
- Boost : custom license => http://www.boost.org/users/license.html
*/

#include <gmock/gmock.h>
  using ::testing::Eq;
#include <gtest/gtest.h>
  using ::testing::Test;

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "Mission.hpp"

#include "Observation.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

namespace fs = boost::filesystem;

#ifdef USE_RESEARCH

#include "algo.hpp"

#endif  // USE_RESEARCH

namespace testing {
class MissionTest : public Test {
 protected:
MissionTest() {}
~MissionTest() {}

virtual void SetUp() {}
virtual void TearDown() {}
Mission myMission;
};

TEST_F(MissionTest, constructor_withName) {
  Mission testMission("test");
  EXPECT_THAT(testMission.getName(), Eq("test"));
  EXPECT_THAT(testMission.getObsNumber(), Eq(0));
  EXPECT_THAT(testMission.getSubFolderCount(), Eq(0));
}

TEST_F(MissionTest, copy_constructor) {
  myMission.setName("test");
  Mission testMission(myMission);
  EXPECT_THAT(testMission.getName(), Eq("test"));
}

TEST_F(MissionTest, copy_operator) {
  myMission.setName("test");
  Mission testMission = myMission;
  EXPECT_THAT(testMission.getName(), Eq("test"));
}

TEST_F(MissionTest, add_observation) {
  myMission.addObservation("../images/testFolder/test.JPG");
  EXPECT_THAT(myMission.getObsNumber(), Eq(1));
}

TEST_F(MissionTest, addFolder) {
  // This also test all acceptable format for the software, and subfolders
  myMission.addFolder("../../../images/testFolder/");
  EXPECT_THAT(myMission.getObsNumber(), Eq(7));
}

TEST_F(MissionTest, addSubFolder) {
	
  myMission.addFolder("../../../images/testFolder/");
  bool found(false);
  std::string subPath = "../../../images/testFolder/sub/";
  fs::path p = fs::system_complete(subPath);
  std::string convertPath = boost::filesystem::canonical(p).generic_string();
  std::shared_ptr<Mission> sub = myMission.getSubFolder(convertPath, found).lock();
  EXPECT_THAT(sub->getObsNumber(), Eq(2));
}

TEST_F(MissionTest, addSpecies) {
  myMission.addFolder("../../../images/testFolder/");
  myMission.addSpecies("Dragon", "#000000", true, 0.5);
  Mission::species test = myMission.getSpecies("Dragon");
  EXPECT_THAT(test.name, Eq("Dragon"));
  EXPECT_THAT(test.color, Eq("#000000"));
  EXPECT_THAT(test.hide, Eq(true));
  EXPECT_THAT(test.threshold, Eq(static_cast<float>(0.5)));

  bool found(false);
  std::string subPath = "../../../images/testFolder/sub/";
  std::shared_ptr<Mission> sub = myMission.getSubFolder(subPath, found).lock();
  Mission::species testSub = myMission.getSpecies("Dragon");
  EXPECT_THAT(testSub.name, Eq("Dragon"));
  EXPECT_THAT(testSub.color, Eq("#000000"));
  EXPECT_THAT(testSub.hide, Eq(true));
  EXPECT_THAT(testSub.threshold, Eq(static_cast<float>(0.5)));
}

TEST_F(MissionTest, removeSubFolder) {
  myMission.addFolder("../../../images/testFolder/");
  std::string subPath = "../../../images/testFolder/sub/";
  fs::path p = fs::system_complete(subPath);
  std::string convertPath = boost::filesystem::canonical(p).generic_string();
  myMission.removeSubFolder(convertPath);
  EXPECT_THAT(myMission.getSubFoldersCount(), Eq(0));
}


#ifdef USE_RESEARCH
/*
TEST_F(MissionTest, compareItems) {
  // create ground truth Mission
  std::string folderName("../../../examples/testLoading/");

  myMission.addFolder(folderName);
  std::string scorePath = "../../../examples/resFdECaf7t5_allScore.txt";
  fs::path p = fs::system_complete(scorePath);
  std::string convertPath = boost::filesystem::canonical(p).generic_string();
  myMission.loadResultsItem(convertPath);
  std::string jsonPath = "../../../examples/testLoading/testCompare.json";
  fs::path p2 = fs::system_complete(jsonPath);
  std::string c = boost::filesystem::canonical(p2).generic_string();
  std::ifstream in(c);
  Json::Value reading;
  in >> reading;
  Mission test;
  test.readJsonItem(reading);

  std::vector<std::vector<double>> results;
  results = myMission.compareItems(test, 0.98);
  std::vector<double> recall = results[0];
  std::vector<double> precision = results[1];
  EXPECT_THAT(recall[0], Eq(0.6));
  EXPECT_THAT(precision[0], Eq(0.5));
}
*/
#ifdef USE_ALGO

/*
TEST_F(MissionTest, detect_turtles) {
  myMission.addObservation("../../../images/testFolder/test2.JPG");
myMission.detectTurtles(Mission::FILTER);
std::shared_ptr<Observation> obsTest = myMission.getObs(0).lock();
  int res = obsTest->getItemCount();
  int expected(16);  // Depending on the version of detectTurtle in algo.cpp
  EXPECT_THAT(res, Eq(expected));
}*/

#endif  // USE_ALGO


#endif  // USE_RESEARCH


}  // namespace testing
