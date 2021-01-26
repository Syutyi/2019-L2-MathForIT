#ifndef MPI_TOOLS_H_INCLUDED
#define MPI_TOOLS_H_INCLUDED

#include <iostream>
#include <vector>
#include <string>
#include "Transition.h"
#include "Etat.h"

/****
 * ESPACE DE NOM : MPI_Tools
 * Fonctions utilitaires
 ****/
namespace MPI_Tools {
    // Extrait les entiers d'une liste
    std::vector<int> extractList(std::string);

    // Extrait les composants d'une transition
    void extractTransition(std::string, int&, char&, int&);

    // Convertit une chaine en entier
    int stoi(std::string);

    // Affiche des espaces
    void printSpace(int=0, std::ostream& =std::cout);

    // Affiche un titre
    void printTitle(std::string, std::ostream& =std::cout);

    // Supprime les duplicatats
    void supprimDuplicata(std::vector<Etat*>&);
    void supprimDuplicata(std::vector<Transition*>&);

    // Verifie la presence d'un etat
    Etat* testPresenceEtatDansVecteur(const std::vector<Etat*>&, const Etat&);

    // Recupere la partie d'un etat
    int recupPartie(const std::vector<std::vector<Etat*> >&, const Etat*);

    // Recupere la taille prise par un nombre
    int recupNombreTaille(int);

    // Lis un mot
    std::string lisMot();
}

#endif // MPI_TOOLS_H_INCLUDED
