#ifndef PROJECT_H_INCLUDED
#define PROJECT_H_INCLUDED

#include <iostream>
#include <vector>
#include <algorithm>
#include "Automate.h"
#include "Transition.h"
#include "Etat.h"
#include "MPI_Tools.h"

/****
 * CLASSE : Project
 * Classe qui gere le projet dans son integralite
 ****/
class Project {
private:
    std::vector<Automate*> m_automates; // Stock les automates

    // Affichage du menu
    int affichageMenu();

    // Affichage du menu de choix d'automate
    Automate* affichageMenuAutomate();

    // Affichage du menu de traitement d'automate
    int affichageMenuAction();

    // Creation de l'automate
    Automate* creationAutomate();

    // Test de reconnaissance de mots
    void testReconnaissanceMot(Automate*);

public:
    ~Project();

    // Execute le projet
    void execute();
};

#endif // PROJECT_H_INCLUDED
