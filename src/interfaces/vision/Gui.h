/*
 * @(#)		Gui.h
 * @version	1.4
 * @autor	C. Rouvière
 */

/**
 * Classe affichant une image OpenCV2 dans une fenêtre
 *	- peut également y ajouter des vecteurs
 */

#ifndef GUI
#define GUI

#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <string.h>
#include "HSV_tools.h"
#include "struct_wrap_bound.h"

class Gui{

public:

	Gui(); ~Gui();

	void Creer_trackbar_HSV_sep(const char* titre_fenetre);			// initialisation de la fenêtre des trackbars de séparation HSV
	void Creer_trackbar_HSV_sep2(const char* titre_fenetre);		// initialisation de la fenêtre des trackbars de séparation HSV
	void Creer_trackbar_transfo(const char* titre_fenetre);			// initialisation de la fenêtre des trackbars de transformation
	HSV_tools *Get_HSV_bound();						// pointeur vers la structure contenant les paramètres de séparation
	HSV_tools *Get_HSV_bound2();						// pointeur vers la structure contenant les paramètres de séparation 2
	STRUCT_WRAP_BOUND *Get_wrap_bound() const;				// pointeur vers la structure contenant les paramètres de transformation		// cliquer si le vecteur n'est pas vide
	void Afficher_image(const std::string titre_fenetre, cv::Mat image);
	void Pad(const std::string titre_fenetre, const float dx, const float dy, const float dx_max, const float dy_max);
	void Ajouter_vecteurs(
		const std::string titre_fenetre,	// titre de la fenêtre
		cv::Mat image,				// image à traiter
		std::vector<cv::Point2i> pts_prev,	// tableau des points initiaux
		std::vector<cv::Point2i> pts_next);	// tableau des points finals

private:

	#define TAILLE_POD 200

	HSV_tools hsv, hsv2;
	STRUCT_WRAP_BOUND *wrap;

	cv::Scalar rouge;
	cv::Size dim_1;
	cv::Point pod_centre;

	static void Callback_HSV(int value, void *userdata);
	static void Callback_wrap(int value, void *object);

};

#endif
