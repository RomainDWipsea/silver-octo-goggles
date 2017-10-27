/*
The file tools.cpp is part of the WildlifeCounter software.

WildlifeCounter is a software that provides tools for automatic detection of wildlife in series of images.

Copyright 2014-2015 WIPSEA, contact@wipsea.com

Author(s) : Romain Dambreville

Creation date : 2015/03/26

Tous droits réservés - All rights reserved

Library used :
- Qt : commercial license => http://doc.qt.io/qt-5/commerciallicense.html
- OpenCV : BSD 3-Clause license => http://opensource.org/licenses/BSD-3-Clause

*/

#include <string>
#include "tools.hpp"
#include "opencv2/core/core.hpp"
#include "easylogging++.h"

std::string cs(QString input) {
  std::string utf8_text = input.toUtf8().constData();
  return utf8_text;
}
 
QImage MatToQImage(const cv::Mat& _inMat) {
  switch (_inMat.type()) {
    // 8-bit, 4 channel
    case CV_8UC4:
      {
        QImage image(_inMat.data, _inMat.cols, _inMat.rows, static_cast<int>(_inMat.step), QImage::Format_RGB32);
        return image;
      }

      // 8-bit, 3 channel
    case CV_8UC3:
      {
        QImage image(_inMat.data, _inMat.cols, _inMat.rows, static_cast<int>(_inMat.step), QImage::Format_RGB888);
        return image.rgbSwapped();
      }

      // 8-bit, 1 channel
    case CV_8UC1:
      {
        static QVector<QRgb>  sColorTable;

        // only create our color table once
        if (sColorTable.isEmpty()) {
          for (int i = 0; i < 256; ++i)
            sColorTable.push_back(qRgb(i, i, i));
        }

        QImage image(_inMat.data, _inMat.cols, _inMat.rows, static_cast<int>(_inMat.step), QImage::Format_Indexed8);
        image.setColorTable(sColorTable);
        return image;
      }

    default:
      //      qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << _inMat.type();
      
      break;
  }

  return QImage();
}


QPixmap MatToQPixmap(const cv::Mat& _inMat) {
  return QPixmap::fromImage(MatToQImage(_inMat));
}

/*bool file_exist(const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
  }*/

bool file_exist(const std::string& name) {
  if (FILE *file = fopen(name.c_str(), "r")) {
    fclose(file);
    return true;
  } else {
    CLOG(WARNING, "gui_sw") << "non existent file=" << name;
    return false;
  }
}
