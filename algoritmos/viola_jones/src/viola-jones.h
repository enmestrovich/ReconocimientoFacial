/*
 * sobel.h
 *
 *  Created on: 21 ene. 2018
 *      Author: ray
 */

#ifndef ALGORITMOS_SRC_VJ_H_
#define ALGORITMOS_SRC_VJ_H_
#include <string>
#include <iostream>
#include "../../../vision/src/algoritmoPlugin.h"
#include "../../../herramientas/Deteccion-OPENCV/src/v&j/viola_jones.h"
#include <opencv2/core/core.hpp>

#include <boost/filesystem.hpp>

#include "../../src/algoritmoPlugin.h"
namespace vision {
	
	class ViolaJones: public Algoritmo_plugin {
		private:

			unsigned long long int * sumaIntegral(const cv::Mat & imgOriginal);
			unsigned long long int * sumaIntegral(const cv::Mat & imgOriginal, cv::Mat & imgGris);

			unsigned long long int valorIntegral(const unsigned long long int * integral, const int x, const int y,const int desplazarX,const int desplazarY, const int width, const int height)const;
			std::vector<cv::Rect_<double>> zonas;
		public:
			ViolaJones();
			~ViolaJones(){};
			const char * getNombre()const{return "ViolaJones";};
			bool procesar(const cv::Mat & imgOriginal);

	};


} /* namespace vision */

#endif
