#include "Carte.h"

using namespace std;

// Constructeur
Carte::Carte(){
	Clear();
}

void Carte::Remplir_buffer(cv::Mat image, vector <cv::Point2i> quad){
	buffer_img.push_back(image);
	buffer_quad.push_back(quad);
}

void Carte::Traiter_buffer(){
	if(is_empty){
		root = New_vignette(buffer_img.back(), buffer_quad.back());
		liste.push_back(root);
		buffer_img.pop_back();
		buffer_quad.pop_back();
	}
	while(!buffer_img.empty()){
		bool casser = false;
		for(int i = 0; i < buffer_img.size(); i++){
			cv::Point2i Ai = buffer_quad[i][0];
			cv::Point2i Bi = buffer_quad[i][1];
			cv::Point2i Ci = buffer_quad[i][2];
			cv::Point2i Di = buffer_quad[i][3];
			for(int j = 0; j < liste.size(); j++){
				cv::Point2i Aj = liste[j].A;
				cv::Point2i Bj = liste[j].B;
				cv::Point2i Cj = liste[j].C;
				cv::Point2i Dj = liste[j].D;
				if(Aj == Ai){
					if(Dj == Bi){
						Lier_vignettes(j, buffer_img[i], 3, +0, -1, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
					if(Bj == Di){
						Lier_vignettes(j, buffer_img[i], 1, -1, +0, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
				}
				if(Aj == Bi){
					if(Dj == Ci){
						Lier_vignettes(j, buffer_img[i], 0, +0, -1, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
					if(Bj == Ai){
						Lier_vignettes(j, buffer_img[i], 2, -1, +0, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
				}
				if(Aj == Ci){
					if(Dj == Di){
						Lier_vignettes(j, buffer_img[i], 1, +0, -1, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
					if(Bj == Bi){
						Lier_vignettes(j, buffer_img[i], 3, -1, +0, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
				}
				if(Aj == Di){
					if(Dj == Ai){
						Lier_vignettes(j, buffer_img[i], 2, +0, -1, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
					if(Bj == Ci){
						Lier_vignettes(j, buffer_img[i], 0, -1, +0, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
				}
				if(Bj == Ai){
					if(Cj == Di){
						Lier_vignettes(j, buffer_img[i], 0, +0, +1, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
				}
				if(Bj == Bi){
					if(Cj == Ai){
						Lier_vignettes(j, buffer_img[i], 1, +0, +1, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
				}
				if(Bj == Ci){
					if(Cj == Bi){
						Lier_vignettes(j, buffer_img[i], 2, +0, +1, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
				}
				if(Bj == Di){
					if(Cj == Ci){
						Lier_vignettes(j, buffer_img[i], 3, +0, +1, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
				}
				if(Cj == Ai){
					if(Dj == Di){
						Lier_vignettes(j, buffer_img[i], 3, +1, +0, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
				}
				if(Cj == Bi){
					if(Dj == Ai){
						Lier_vignettes(j, buffer_img[i], 0, +1, +0, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
				}
				if(Cj == Ci){
					if(Dj == Bi){
						Lier_vignettes(j, buffer_img[i], 1, +1, +0, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
				}
				if(Cj == Di){
					if(Dj == Ci){
						Lier_vignettes(j, buffer_img[i], 2, +1, +0, buffer_quad[i]);
						casser = Enlever_element_buffer(i);
						break;
					}
				}
			}
			if(casser){break;}
		}
		if(!casser){break;}
	}
}

void Carte::Lier_vignettes(int id_fixe, cv::Mat image, int rotation, int dx, int dy, vector <cv::Point2i> quad){
	STRUCT_VIGNETTE nouveau = New_vignette(image, quad);
	nouveau.orientation =	(liste[id_fixe].orientation + rotation) % 4;
	switch(liste[id_fixe].orientation){
		case 0:
			nouveau.x = liste[id_fixe].x + dx;
			nouveau.y = liste[id_fixe].y + dy;
			break;
		case 1:
			nouveau.x = liste[id_fixe].x - dy;
			nouveau.y = liste[id_fixe].y + dx;
			break;
		case 2:
			nouveau.x = liste[id_fixe].x - dx;
			nouveau.y = liste[id_fixe].y - dy;
			break;
		case 3:
			nouveau.x = liste[id_fixe].x + dy;
			nouveau.y = liste[id_fixe].y - dx;
			break;
	}
	nouveau.A =		quad[0];
	nouveau.B =		quad[1];
	nouveau.C =		quad[2];
	nouveau.D =		quad[3];
	if(dx == -1 && dy == +0){
		liste[id_fixe].nord = liste.size();
		Nouveaux_cardinaux(&nouveau, rotation + 0, id_fixe);
	}
	if(dx == +1 && dy == +0){
		liste[id_fixe].sud = liste.size();
		Nouveaux_cardinaux(&nouveau, rotation + 2, id_fixe);
	}
	if(dx == +0 && dy == -1){
		liste[id_fixe].ouest = liste.size();
		Nouveaux_cardinaux(&nouveau, rotation + 1, id_fixe);
	}
	if(dx == +0 && dy == +1){
		liste[id_fixe].est = liste.size();
		Nouveaux_cardinaux(&nouveau, rotation + 3, id_fixe);
	}
	liste.push_back(nouveau);
}

void Carte::Clear(){
	liste.clear();
	buffer_img.clear();
	buffer_quad.clear();
	is_empty = true;
}

STRUCT_VIGNETTE Carte::New_vignette(cv::Mat image, vector <cv::Point2i> quad){
	STRUCT_VIGNETTE vignette;
	vignette.image =	image;
	vignette.index =	liste.size();
	vignette.x =		0;
	vignette.y =		0;
	vignette.orientation =	0;
	vignette.nord =		-1;
	vignette.sud =		-1;
	vignette.ouest =	-1;
	vignette.est =		-1;
	vignette.A =		quad[0];
	vignette.B =		quad[1];
	vignette.C =		quad[2];
	vignette.D =		quad[3];
	return vignette;
}

bool Carte::Enlever_element_buffer(int i){
	buffer_img.erase(buffer_img.begin() + i);
	buffer_quad.erase(buffer_quad.begin() + i);
	return true;
}

void Carte::Nouveaux_cardinaux(STRUCT_VIGNETTE *nouveau, int somme, int id_fixe){
	switch(somme % 4){
		case 0:	nouveau->sud = id_fixe;		break;
		case 1:	nouveau->ouest = id_fixe;	break;
		case 2:	nouveau->nord = id_fixe;	break;
		case 3:	nouveau->est = id_fixe;		break;
	}
}

std::vector <STRUCT_VIGNETTE> Carte::Get_cases(){return liste;}
