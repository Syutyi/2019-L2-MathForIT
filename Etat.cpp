#include "Etat.h"

using namespace std;

/**
 * CONSTRUCTEURS/DESTRUCTEURS
 **/
Etat::Etat(Automate* aut, int num, bool init, bool fin): m_num(num), m_automate(aut), m_estInitial(init), m_estFinal(fin) {
}
Etat::Etat(Automate* aut, vector<Etat*> etats, bool init, bool fin): m_groupEtat(etats), m_automate(aut), m_estInitial(init), m_estFinal(fin) {
    // Parcours des etats
    int numMin = 0;
    for(vector<Etat*>::iterator it = etats.begin(); it != etats.end(); ++it) {
        // Verification du numero
        if((*it)->m_num < numMin) numMin = (*it)->m_num;
    }

    // Choix du numero de l'etat
    m_num = numMin - 1;
}
Etat::Etat(const Etat &etat): m_num(etat.m_num), m_transitionDepart(etat.m_transitionDepart), m_transitionArrive(etat.m_transitionArrive), m_automate(etat.m_automate), m_estInitial(etat.m_estInitial), m_estFinal(etat.m_estFinal) {
    // Chargement des etats
    for(vector<Etat*>::const_iterator it = etat.m_groupEtat.begin(); it != etat.m_groupEtat.end(); ++it) {
        // Ajout de l'etat courant
        m_groupEtat.push_back(new Etat(**it));
    }
}
Etat::~Etat() {
    // Parcours des etats
    for(vector<Etat*>::iterator it = m_groupEtat.begin(); it != m_groupEtat.end(); ++it) {
        // Liberation de l'etat courant
        delete *it;
    }
}

/**
 * Affiche l'etat
 **/
bool Etat::estInitial() const {
    return m_estInitial;
}

/**
 * Affiche l'etat
 **/
bool Etat::estFinal() const {
    return m_estFinal;
}

/**
 * Affiche l'etat
 **/
void Etat::affichage(ostream &os) const {
    // Parcours des groupes d'etats
    for(vector<Etat*>::const_iterator it = m_groupEtat.begin(); it != m_groupEtat.end(); ++it) {
        // Affichage du groupe d'etat
        os << **it;

        // Verification du non traitement du dernier element
        if(it != m_groupEtat.end() - 1) {
            switch(m_num) {
            case -1:
                os << GROUP_ETAT_0;
                break;
            case -2:
                os << GROUP_ETAT_1;
                break;
            default:
                os << GROUP_ETAT_2;
                break;
            }
        }
    }

    // Verification du nom d'etat
    if(m_num >= 0) os << m_num;
}

/**
 * Recupere la taille du nom
 **/
int Etat::recupTailleNom() const {
    int taille = 0; // Taille du nom

    // Parcours des groupes d'etats
    for(vector<Etat*>::const_iterator it = m_groupEtat.begin(); it != m_groupEtat.end(); ++it) {
        // Ajout de la taille du nom du groupe d'etat
        taille += (*it)->recupTailleNom();
    }

    // Verification de l'existance d'au moins 1 element
    int tailleGroupEtat = m_groupEtat.size(); // Nombre de groupes d'etats
    if(tailleGroupEtat > 0) taille += tailleGroupEtat - 1;

    // Verification du nom d'etat
    if(m_num >= 0) taille += recupTailleReelle();

    return taille;
}

/**
 * Recupere la taille reelle du nom
 **/
int Etat::recupTailleReelle() const {
    // Calcul de la puissance de 10
    int i   = 0;     // Iterateur
    int nbr = m_num; // Nombre
    while(nbr) {
        // Division du nombre
        nbr /= 10;

        i++;
    }

    return ((i!=0)? i : 1);
}

/**
 * Recupere la taille des noms cibles par transition
 **/
int Etat::recupTailleTransition(char c) const {
    vector<Etat*> etatsCibles = recupEtatsCibles(c); // Conteneur des etats cibles
    int taille                = 0;                  // Taille courante

    // Parcours des etats cibbles
    for(vector<Etat*>::const_iterator it = etatsCibles.begin(); it != etatsCibles.end(); ++it) {
        // Ajout de la taille
        taille += (*it)->recupTailleNom();
    }

    // Verification de l'existance d'au moins 1 element
    int tailleEtatsCibles = etatsCibles.size(); // Nombre d'etats cibles
    if(tailleEtatsCibles > 0) taille += tailleEtatsCibles - 1;

    return taille;
}

/**
 * Recupere la liste des etats cibbles par le symbole concerne
 **/
vector<Etat*> Etat::recupEtatsCibles(char c) const {
    vector<Etat*> etats; // Conteneur des etats cibles

    // Parcours des transitions au depart de l'etat
    for(vector<Transition*>::const_iterator it = m_transitionDepart.begin(); it != m_transitionDepart.end(); ++it) {
        // Recuperation de l'etat d'arrive
        Etat *etatA = (*it)->recupEtatArrive();

        // Verification de la transition
        if((*it)->recupSymbole() == c && (find(etats.begin(), etats.end(), etatA) == etats.end())) {
            // Ajout de l'etat d'arrive
            etats.push_back(etatA);
        }
    }

    return etats;
}

/**
 * Recupere le nombre de transitions entrantes
 **/
int Etat::recupNbrTransitionsEntrantes() const {
    return m_transitionArrive.size();
}

/**
 * Recupere le nombre de transitions entrantes
 **/
int Etat::recupNbrTransitionsEntrantes(char c) const {
    // Parcours des etats d'arrive
    int counter = 0;
    for(vector<Transition*>::const_iterator it = m_transitionArrive.begin(); it != m_transitionArrive.end(); ++it) {
        // Verification que la transition utilise le symbole
        if((*it)->recupSymbole() == c) counter++;
    }

    return counter;
}

/**
 * Recupere les transitions de depart
 **/
vector<Transition*> Etat::recupTransitionDepart() const {
    return m_transitionDepart;
}

/**
 * Recupere les transitions d'arrive
 **/
vector<Transition*> Etat::recupTransitionArrive() const {
    return m_transitionArrive;
}

/**
 * Recupere le groupe d'etats
 **/
vector<Etat*> Etat::recupGroupEtat() const {
    return m_groupEtat;
}

/**
 * Test un mot
 **/
bool Etat::testMot(string s) const {
    // Verification de fin de mot
    if(s[0] == '\0') {
        // Verification de fin de l'automate
        if(m_estFinal) return true;

        return false;
    }

    // Paroucours des transitions au depart de l'etat
    for(vector<Transition*>::const_iterator it = m_transitionDepart.begin(); it != m_transitionDepart.end(); ++it) {
        // Verification d'une possibble transition par le symbole courant
        if((*it)->recupSymbole() == s[0]) {
            // Recuperation de l'etat cible
            Etat* etatCible = (*it)->recupEtatArrive();

            // Verification de la reconnaissance du mot
            if(etatCible->testMot(s.substr(1))) return true;
        }
    }

    return false;
}

/**
 * Ajoute une transition de depart a l'etat
 **/
void Etat::addTransitionDepart(Transition* trans) {
    // Ajoute la transition
    m_transitionDepart.push_back(trans);
}

/**
 * Ajoute une transition d'arrive a l'etat
 **/
void Etat::addTransitionArrive(Transition* trans) {
    // Ajoute la transition
    m_transitionArrive.push_back(trans);
}

/**
 * Supprime une transition de depart a l'etat
 **/
void Etat::remTransitionDepart(Transition* trans) {
    // Recuperation du pointeur vers l'element
    vector<Transition*>::iterator it = find(m_transitionDepart.begin(), m_transitionDepart.end(), trans);

    // Verification de la bonne recuperation
    if(it != m_transitionDepart.end()) m_transitionDepart.erase(it);
}

/**
 * Supprime une transition d'arrive a l'etat
 **/
void Etat::remTransitionArrive(Transition* trans) {
    // Recuperation du pointeur vers l'element
    vector<Transition*>::iterator it = find(m_transitionArrive.begin(), m_transitionArrive.end(), trans);

    // Verification de la bonne recuperation
    if(it != m_transitionArrive.end()) m_transitionArrive.erase(it);
}

/**
 * Vide les conteneurs de transition
 **/
void Etat::freeTransition() {
    // Vide les conteneurs
    m_transitionDepart.clear();
    m_transitionArrive.clear();
}

/**
 * Change l'automate associe
 **/
void Etat::setAutomate(Automate* aut) {
    m_automate = aut;

    // Parcours du groupe d'etats
    for(vector<Etat*>::iterator it = m_groupEtat.begin(); it != m_groupEtat.end(); ++it) {
        // Modification de l'automate associe
        (*it)->setAutomate(aut);
    }
}

/**
 * Change le statut d'etat initial
 **/
void Etat::setInitial(bool etat) {
    m_estInitial = etat;
}

/**
 * Change le statut d'etat final
 **/
void Etat::setFinal(bool etat) {
    m_estFinal = etat;
}

/**
 * OPPERATEUR ==
 **/
bool operator==(const Etat &etatA, const Etat &etatB) {
    // Verification de la correspondance du numero d'etat
    if(etatA.m_num == etatB.m_num) {
        // Verification d'un etat final
        if(etatA.m_num >= 0) return true;

        // Verification de la taille du sous groupe
        if(etatA.m_groupEtat.size() != etatB.m_groupEtat.size()) return false;

        /// Traitement d'un groupe d'etat
        // Parcours du groupe d'etat A
        for(vector<Etat*>::const_iterator itA = etatA.m_groupEtat.begin(); itA != etatA.m_groupEtat.end(); ++itA) {
            bool correspondance = false; // TEST : Verifie si l'etat du groupe existe dans l'autre etat
            // Parcours du groupe d'etat B
            for(vector<Etat*>::const_iterator itB = etatB.m_groupEtat.begin(); itB != etatB.m_groupEtat.end(); ++itB) {
                // Verification de la correspondance
                if(**itA == **itB) {
                    correspondance = true;

                    break;
                }
            }

            // Verification d'une non correspondance relative => Un des etats du groupe n'est pas contenu dans l'autre etat
            if(!correspondance) return false;
        }

        return true;
    }

    return false;
}

/**
 * OPPERATEUR <<
 **/
ostream& operator<<(std::ostream &os, const Etat &etat) {
    // Affichage de l'etat
    etat.affichage(os);

    return os;
}
