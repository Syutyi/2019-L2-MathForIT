#include "Transition.h"

using namespace std;

/**
 * CONSTRUCTEURS/DESTRUCTEURS
 **/
Transition::Transition(Automate* aut, Etat* etatD, char symb, Etat* etatA): m_etatDepart(etatD), m_symbole(symb), m_etatArrive(etatA), m_automate(aut) {
    // Ajout des transitions aux etats
    etatD->addTransitionDepart(this);
    etatA->addTransitionArrive(this);
}
Transition::Transition(const Transition &trans): m_etatDepart(trans.m_etatDepart), m_symbole(trans.m_symbole), m_etatArrive(trans.m_etatArrive), m_automate(trans.m_automate) {
    // Ajout des transitions aux etats
    m_etatDepart->addTransitionDepart(this);
    m_etatArrive->addTransitionArrive(this);
}

/**
 * Affiche la transition
 **/
void Transition::affichage(std::ostream &os) const {
    os << "(" << *m_etatDepart << m_symbole << *m_etatArrive << ")";
}

/**
 * Recupere le symbole de transition
 **/
char Transition::recupSymbole() const {
    return m_symbole;
}

/**
 * Recupere l'etat de depart
 **/
Etat* Transition::recupEtatDepart() const {
    return m_etatDepart;
}

/**
 * Recupere l'etat d'arrive
 **/
Etat* Transition::recupEtatArrive() const {
    return m_etatArrive;
}

/**
 * Change l'etat de depart
 **/
void Transition::setEtatDepart(Etat* etat) {
    // Suppression de la transition pour l'ancien etat
    m_etatDepart->remTransitionDepart(this);

    // Changement de l'etat de depart
    m_etatDepart = etat;
    etat->addTransitionDepart(this);
}

/**
 * Change l'etat d'arrive
 **/
void Transition::setEtatArrive(Etat* etat) {
    // Suppression de la transition pour l'ancien etat
    m_etatArrive->remTransitionArrive(this);

    // Changement de l'etat d'arrive
    m_etatArrive = etat;
    etat->addTransitionArrive(this);
}

/**
 * Change l'automate associe
 **/
void Transition::setAutomate(Automate* aut) {
    m_automate = aut;
}

/**
 * OPPERATEUR ==
 **/
bool operator==(const Transition &transA, const Transition &transB) {
    return (transA.m_etatDepart == transB.m_etatDepart && transA.m_symbole == transB.m_symbole && transA.m_etatArrive == transB.m_etatArrive && transA.m_automate == transB.m_automate);
}

/**
 * OPPERATEUR <<
 **/
ostream& operator<<(std::ostream &os, const Transition &trans) {
    // Affichage de la transition
    trans.affichage(os);

    return os;
}
