/*
The file algo.cpp is part of the Harmony software.

Harmony is a software that provides tools for the detection of wildlife in series of images.

Copyright 2014-2016 WIPSEA SARL, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/11/22

Tous droits réservés - All rights reserved

Library used :
- OpenCV 3.0.0 : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause
- Boost : custom license => http://www.boost.org/users/license.html
*/

#include "misc.hpp"
#include "easylogging++.h"
#include <set>
#include <boost/filesystem.hpp>
#include "exiv2/exiv2.hpp"

namespace fs = boost::filesystem;

bool isStringIn(std::string _test, const std::vector<std::string>& _container) {
	CLOG(TRACE, "cor_al") << "Process : is string in for string =" << _test;
  std::vector<std::string>::const_iterator it;
  for (it = _container.begin(); it != _container.end(); it++) {
    if (_test == *it) {
      return true;
    }
  }
  return false;
}

double my_stod(const std::string &valueAsString) {
  std::istringstream totalSString(valueAsString);
  double valueAsDouble;
  totalSString >> valueAsDouble;
  if (!totalSString)
    throw std::runtime_error("Error converting to double");
  return valueAsDouble;
}

bool isImage(std::string _path) {
	CLOG(TRACE, "cor_al") << "Process : checking valid extension for image =" << _path;
  std::set<std::string> targetExtensions;
  targetExtensions.insert(".JPG");
  targetExtensions.insert(".BMP");
  targetExtensions.insert(".GIF");
  targetExtensions.insert(".PNG");
  targetExtensions.insert(".TIF");

  fs:: path p(_path);
  if (!fs::is_regular_file(p)) { return false;}
  std::string extension = p.extension().string();
  std::transform(extension.begin(),
                 extension.end(), extension.begin(), ::toupper);
  if (targetExtensions.find(extension) == targetExtensions.end()) {
	  CLOG(WARNING, "cor_al") << "Wrong image format detected for image=" << _path;
    return false;
  }
  CLOG(TRACE, "cor_al") << "Status : image extension is OK image=" << _path;
  return true;
}

cv::Mat geoTransformAlpha(std::string _imagePath) {
  CLOG(TRACE, "cor_al") << "Process : projecting image=" << _imagePath;
  cv::Size realSize = getExifSize(_imagePath);
  cv::Mat im = cv::imread(_imagePath, CV_LOAD_IMAGE_UNCHANGED);
  cv::Mat channel[4];
  cv::split(im, channel);
  cv::Mat alphaChannel = channel[3].clone();

  cv::Mat first_column = alphaChannel.col(0);
  cv::Mat first_row = alphaChannel.row(0);
  cv::Mat last_column = alphaChannel.col(alphaChannel.cols - 1);
  cv::Mat last_row = alphaChannel.row(alphaChannel.rows - 1);

  std::vector<cv::Point> corners;

  cv::Mat nonZeroCoordinatesC0;
  findNonZero(first_column, nonZeroCoordinatesC0);
  cv::Point c0 = nonZeroCoordinatesC0.at<cv::Point>(0);  // first corner
  corners.push_back(c0);

  cv::Mat nonZeroCoordinatesC1;
  findNonZero(first_row, nonZeroCoordinatesC1);
  cv::Point c1 = nonZeroCoordinatesC1.at<cv::Point>(0);  // second corner
  corners.push_back(c1);

  cv::Mat nonZeroCoordinatesC2;
  findNonZero(last_column, nonZeroCoordinatesC2);
  cv::Point c2 = nonZeroCoordinatesC2.at<cv::Point>(0);  // 3rd corner
  corners.push_back(cv::Point(alphaChannel.cols - 1, c2.y));

  cv::Mat nonZeroCoordinatesC3;
  findNonZero(last_row, nonZeroCoordinatesC3);
  cv::Point c3 = nonZeroCoordinatesC3.at<cv::Point>(0);  // last corner
  corners.push_back(cv::Point(c3.x, alphaChannel.rows - 1));

  // create the destination image
  cv::Mat dst(realSize, CV_8UC3);

  /**
     To orientate the image properly, we use an a priori information that we put in SemmaDrone (QGIS module) : 
     The top left corner in the alpha channel is set to 250 instead of 255. We find this value to locate the tl corner and reproject the image accordingly.
  */
  cv::Mat mask = alphaChannel == 250;
  cv::Mat nonZeroCoordinatesFirst;
  int n = countNonZero(mask);
  // Checking wether we do have the tl corner info or not, if not, just project the image as it is.
  if (n > 0) {
    findNonZero(mask, nonZeroCoordinatesFirst);
    cv::Point first;
    // find the first element that is 250 and also on a border
    for (int j = 0; j < n; j++) {
      first = nonZeroCoordinatesFirst.at<cv::Point>(j);
      if (first.x == alphaChannel.cols - 1
          || first.x == 0
          || first.y == alphaChannel.rows - 1
          || first.y == 0) {
        break;
      }
    }
    for (size_t i = 0; i < corners.size(); i++) {
      // can't compare point because we don't know which one will still have the 250 tag after transformation.
      if ((corners[0].x > 0 && corners[0].x == first.x)
          || (corners[0].y > 0 && corners[0].y == first.y)) {
        break;
      }
      std::rotate(corners.begin(), corners.begin() + 1, corners.end());
    }
  } else {
    CLOG(WARNING, "cor_al") <<
        "Couldn't find corner information in the alpha channel, projection unsure for image =" << _imagePath;
  }

  cv::Point2f inputPoint[4];
  inputPoint[0] = cv::Point2f(corners[0].x, corners[0].y);
  inputPoint[1] = cv::Point2f(corners[1].x, corners[1].y);
  inputPoint[2] = cv::Point2f(corners[2].x, corners[2].y);
  inputPoint[3] = cv::Point2f(corners[3].x, corners[3].y);

  cv::Point2f outputPoint[4];
  outputPoint[0] = cv::Point2f(0, 0);
  outputPoint[1] = cv::Point2f(dst.cols - 1, 0);
  outputPoint[2] = cv::Point2f(dst.cols - 1, dst.rows - 1);
  outputPoint[3] = cv::Point2f(0, dst.rows - 1);

  cv::Mat transformMat = getPerspectiveTransform(inputPoint, outputPoint);

  cv::Mat imColor;  // = cv::imread(_imagePath);
  cv::cvtColor(im, imColor, CV_BGRA2BGR);
  warpPerspective(imColor, dst, transformMat, dst.size());
  return dst;
}

cv::Size getExifSize(std::string _fileName) {
  CLOG(TRACE, "cor_al") << "Process : find image size in exif for image=" << _fileName;
  Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(_fileName);
  assert(image.get() != 0);
  image->readMetadata();

  Exiv2::ExifData &exifData = image->exifData();
  if (exifData.empty()) {
    std::string error(_fileName);
    error += ": No Exif data found in the file";
    throw Exiv2::Error(1, error);
  }
  try {
    Exiv2::ExifData::const_iterator x =
        exifData.findKey(Exiv2::ExifKey("Exif.Photo.PixelXDimension"));
    long cols = x->toLong();
    Exiv2::ExifData::const_iterator y =
        exifData.findKey(Exiv2::ExifKey("Exif.Photo.PixelYDimension"));
    long rows = y->toLong();
    CLOG(TRACE, "cor_al") << "Data : cols="
                          << static_cast<int>(cols)
                          << "; rows=" << static_cast<int>(rows);
    return cv::Size(static_cast<int>(cols), static_cast<int>(rows));
  }
  catch (...) {
    CLOG(WARNING, "cor_al") << "Exif.Photo.PixelXDimension and Exif.Photo.PixelYDimension not found for image=" << _fileName << "\n default parameters set to 4000x3000 for projection";
    return cv::Size(4000, 3000);
  }
}
