#ifndef ETAT_H_INCLUDED
#define ETAT_H_INCLUDED

class Etat;

#include <iostream>
#include <vector>
#include <algorithm>
#include "Automate.h"
#include "Transition.h"

/****
 * CLASSE : Etat
 * Classe qui gere les etats des automates
 ****/
class Etat {
private:
    int                      m_num;              // Numero de l'etat
    std::vector<Etat*>       m_groupEtat;        // Groupe d'etats
    std::vector<Transition*> m_transitionDepart; // Transitions de depart
    std::vector<Transition*> m_transitionArrive; // Transitions d'arrive
    Automate*                m_automate;         // Automate lie

    bool m_estInitial; // TEST : Etat Initial ?
    bool m_estFinal;   // TEST : Etat Final ?

public:
    /**
     * CONSTRUCTEURS/DESTRUCTEURS
     **/
    Etat(Automate*, int, bool=false, bool=false);
    Etat(Automate*, std::vector<Etat*>, bool=false, bool=false);
    Etat(const Etat&);
    ~Etat();

    /**
     * METHODES SANS MODIFICATION
     **/
    void affichage(std::ostream& =std::cout) const;
    bool estInitial() const;
    bool estFinal() const;
    int recupTailleNom() const;
    int recupTailleReelle() const;
    int recupTailleTransition(char) const;
    std::vector<Etat*> recupEtatsCibles(char) const;
    int recupNbrTransitionsEntrantes() const;
    int recupNbrTransitionsEntrantes(char) const;
    std::vector<Transition*> recupTransitionDepart() const;
    std::vector<Transition*> recupTransitionArrive() const;
    std::vector<Etat*> recupGroupEtat() const;
    bool testMot(std::string) const;

    /**
     * METHODES AVEC MODIFICATION
     **/
    void addTransitionDepart(Transition*);
    void addTransitionArrive(Transition*);
    void remTransitionDepart(Transition*);
    void remTransitionArrive(Transition*);
    void freeTransition();
    void setAutomate(Automate*);
    void setInitial(bool);
    void setFinal(bool);

    /**
     * OPERATEURS
     **/
    friend bool operator==(const Etat&, const Etat&);
    friend std::ostream& operator<<(std::ostream&, const Etat&);
};

#endif // ETAT_H_INCLUDED
