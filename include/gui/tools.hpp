/*
The file tools.hpp is part of the WildlifeCounter software.

WildlifeCounter is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2015 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
- OpenCV : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause

*/

#include <QWidget>
// #include <QtWidgets>
#include <string>
#include "opencv2/core/core.hpp"

/*!
 * \brief Convert
 *
 * Convert a QString to a std::string
 *
 * \param input : QString to conve
 *
 * \return std::string convertion result
 */
std::string cs(QString input);

 /*!
  * \brief conversion
  *
  * cv::mat to qimage
  *
  * \param _inMat : cv::Mat to convert
  *
  * \return QImage
  */
QImage MatToQImage(const cv::Mat& _inMat);

/*!
 * \brief conversion
 *
 * cv::MatToQPixmap
 *
 * \param _inMat : cv::Mat to convert
 *
 * \return QPixmap
 */
QPixmap MatToQPixmap(const cv::Mat& _inMat);


/*!
 * \brief test existance
 *
 * check if a file exist
 *
 * \param absolute fileName
 *
 * \return true if it does exist
 */
bool file_exist(const std::string& name);
