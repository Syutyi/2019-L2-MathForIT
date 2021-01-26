#ifndef TRANSITION_H_INCLUDED
#define TRANSITION_H_INCLUDED

class Transition;

#include <iostream>
#include "Automate.h"
#include "Etat.h"

/****
 * CLASSE : Transition
 * Classe qui gere les transitions des automates
 ****/
class Transition {
private:
    Etat*     m_etatDepart; // Etat de depart de la transition
    char      m_symbole;    // Symbbole de la transition
    Etat*     m_etatArrive; // Etat d'arrive de la transition
    Automate* m_automate;   // Automate lie

public:
    /**
     * CONSTRUCTEURS/DESTRUCTEURS
     **/
    Transition(Automate*, Etat*, char, Etat*);
    Transition(const Transition&);

    /**
     * METHODES SANS MODIFICATION
     **/
    void affichage(std::ostream& =std::cout) const;
    char recupSymbole() const;
    Etat* recupEtatDepart() const;
    Etat* recupEtatArrive() const;

    /**
     * METHODES AVEC MODIFICATION
     **/
    void setEtatDepart(Etat*);
    void setEtatArrive(Etat*);
    void setAutomate(Automate*);

    /**
     * OPERATEURS
     **/
    friend bool operator==(const Transition&, const Transition&);
    friend std::ostream& operator<<(std::ostream&, const Transition&);
};

#endif // TRANSITION_H_INCLUDED
