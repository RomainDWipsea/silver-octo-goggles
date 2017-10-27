/*
The file itemTest.cpp is part of the Harmony software.

Harmony is a software that provides tools for the detection of wildlife in series of images.

Copyright 2014-2016 WIPSEA SARL, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2016/05/04

Tous droits réservés - All rights reserved

Library used :
- OpenCV 3.0.0 : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
- Boost : custom license => http://www.boost.org/users/license.html
*/

#include <gmock/gmock.h>
  using ::testing::Eq;
#include <gtest/gtest.h>
  using ::testing::Test;

#include <string>
#include <fstream>
#include <vector>
#include "Item.hpp"

namespace testing {
class ItemTest : public Test {
 protected:
ItemTest() {}
~ItemTest() {}

virtual void SetUp() {}
virtual void TearDown() {}
Item myItem;
};


TEST_F(ItemTest, copy_constructor) {
  Item it("monImage", cv::Point(3, 4));
  Item testItem(it);
  EXPECT_THAT(testItem.getImageName(), Eq("monImage"));
  EXPECT_THAT(testItem.getCenter(), Eq(cv::Point(3, 4)));
  EXPECT_THAT(testItem.getClass(), Eq("noClass"));
  EXPECT_THAT(testItem.maxScore(), Eq(0));
  
}

TEST_F(ItemTest, copy_operator) {
  Item test("monImage", cv::Point(3, 4));
  Item testCopy = test;
  testCopy.setCenter(cv::Point(1, 2));
  EXPECT_THAT(testCopy.getImageName(), Eq("monImage"));
  EXPECT_THAT(testCopy.getCenter(), Eq(cv::Point(1, 2)));
  EXPECT_THAT(testCopy.getClass(), Eq("noClass"));
  EXPECT_THAT(testCopy.maxScore(), Eq(0));
  
}

TEST_F(ItemTest, writeJson) {
  Item test("monImage", cv::Point(3, 4), 100, "testSpecies", 0.53);
  Json::Value save;
  test.writeJson(&save);
  std::ofstream out("itemTest.txt", std::ofstream::out);
  out << save;
  out.close();
}

TEST_F(ItemTest, readJson) {
  std::ifstream in("itemTest.txt");
  Json::Value reading;
  in >> reading;
  Item test;
  test.readJsonv110(reading);
  EXPECT_THAT(test.getImageName(), Eq("monImage"));
  EXPECT_THAT(test.getCenter(), Eq(cv::Point(3, 4)));
  EXPECT_THAT(test.getClass(), Eq("testSpecies"));
  EXPECT_THAT(test.maxScore(), Eq(static_cast<float>(0.53)));
  //  EXPECT_THAT(test.getSize(), Eq(100));
  in.close();
  }

/*
TEST_F(ItemTest, addId) {
  Item test("img1", cv::Point(100, 100));
  test.addId("Turtle", 0.65);
  EXPECT_THAT(test.getClass(), Eq("Turtle"));
  EXPECT_THAT(test.maxScore(), Eq(static_cast<float>(0.65)));

  test.addId("Ray", 0.75);
  test.setScore("Ray", 0.77);
  EXPECT_THAT(test.getClass(), Eq("Ray"));
  EXPECT_THAT(test.maxScore(), Eq(static_cast<float>(0.77)));
  }*/

#ifdef USE_RESEARCH
TEST_F(ItemTest, compareItem) {
  Item ref("img1", cv::Point(100, 100), 20, "spec1", 0.60);
  Item same("img1", cv::Point(148, 100), 20, "spec1", 0.55);
  Item wIm("img2", cv::Point(100, 100), 20, "spec1", 0.55);
  Item wSpec("img1", cv::Point(100, 140), 20, "spec2", 0.60);
  Item wDist("img1", cv::Point(151, 140), 20, "spec1", 0.60);
  EXPECT_THAT(ref.compare(same, 0.50), Eq(static_cast<int>(1)));
  EXPECT_THAT(ref.compare(same, 0.56), Eq(static_cast<int>(0)));
  EXPECT_THAT(ref.compare(wIm, 0.50), Eq(static_cast<int>(0)));
  EXPECT_THAT(ref.compare(wSpec, 0.50), Eq(static_cast<int>(2)));
  EXPECT_THAT(ref.compare(wDist, 0.50), Eq(static_cast<int>(0)));
}
#endif  // USE_RESEARCH

}  // namespace testing
