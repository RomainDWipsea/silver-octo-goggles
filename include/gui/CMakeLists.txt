#/*on ajoute les .h dans une "variable" que l'on nomme ici ImageFileReader_HEADERS*/
SET(ImageFileReader_HEADERS ImgZoom.hpp) 
#/*La on appel la fonction QT4_WRAP_CPP sur cette variable et on envoi le tout dans une nouvelle variable*/
QT5_WRAP_CPP(ImageFileReader_HEADERS_MOC ${ImageFileReader_HEADERS})