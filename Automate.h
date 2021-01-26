#ifndef AUTOMATE_H_INCLUDED
#define AUTOMATE_H_INCLUDED

class Automate;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "const.h"
#include "MPI_Tools.h"
#include "Transition.h"
#include "Etat.h"

/****
 * CLASSE : Automate
 * Classe qui gere les automates
 ****/
class Automate {
private:
    std::string              m_nom;     // Nom de l'automate
    int                      m_tailleA; // Taille de l'alphabet
    std::vector<Etat*>       m_Q;       // Etats de l'automate
    std::vector<Etat*>       m_I;       // Etats initiaux de l'automate
    std::vector<Etat*>       m_T;       // Etats terminaux de l'automate
    std::vector<Transition*> m_E;       // Transitions de l'automate

    bool m_estAsynchrone;   // TEST : Automate Asynchrone ?
    bool m_estDeterministe; // TEST : Automate Deterministe ?
    bool m_estComplet;      // TEST : Automate Complet ?
    bool m_estMinimal;      // TEST : Automate Minimal ?
    bool m_estStandard;     // TEST : Automate Standard ?

    /**
     * METHODES SANS MODIFICATION
     **/
    int recupTailleEtatMax() const;
    int recupTailleTransitionMax(char) const;
    void afficheTableTransitionPartition(const std::vector<std::vector<Etat*> >&, int, std::ostream& =std::cout) const;

    /**
     * METHODES AVEC MODIFICATION
     **/
    void accessibilisation();
    std::vector<Etat*> fermetureEpsilon(Etat*);
    void supprimerEtatDoubles();
    void supprimerTransitionsDoubles();
    void evidageTransition();

public:
    /**
     * CONSTRUCTEURS/DESTRUCTEURS
     **/
    Automate(std::string="Default");
    Automate(const Automate&);
    ~Automate();

    /**
     * METHODES SANS MODIFICATION
     **/
    std::string recupNom() const;
    void affichage(std::ostream& =std::cout) const;
    bool testMot(std::string) const;

    /**
     * METHODES AVEC MODIFICATION
     **/
    void determiniserEtCompleter();
    void minimiser();
    Automate* automateComplementaire();
    void standardiser();
    bool testAutomateAsynchrone();
    bool testAutomateDeterministe();
    bool testAutomateComplet();
    bool testAutomateStandard();
    void determinisationEtCompletionAutomateAsynchrone();
    void completion();
    void determinisationEtCompletionAutomateSynchrone();
    void synchronisation();
    void determinisation();
    void minimisation();
    void standardisation();

    /**
     * OPERATEURS
     **/
    friend std::ostream& operator<<(std::ostream&, const Automate&);
};

#endif // AUTOMATE_H_INCLUDED
