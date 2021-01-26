#include "MPI_Tools.h"

using namespace std;

/****
 * ESPACE DE NOM : MPI_Tools
 ****/
namespace MPI_Tools {
    /**
     * Extrait les entiers d'une liste
     **/
    vector<int> extractList(string liste) {
        vector<int> nbrList; // Conteneur des nombres
        int nbr = 0;         // Conteneur du nombre courant

        // Parcours de la chaine
        int i = 0, j = 0; // Iterateur
        while(liste[i] != '\0') {
            // Verification d'un espace
            if(liste[i] == ' ') {
                // Ajout du nombre
                nbrList.push_back(nbr);

                // Reinitialisation du conteneur du nombre courant
                nbr = 0;

                j = i;
            } else {
                // Recuperation du chiffre courant
                int chiffre = (int)liste[i] - '0';
                chiffre     = (chiffre >= 0 && chiffre <= 9)? chiffre : 0;

                // Changement du nombre
                nbr = nbr * 10 + chiffre;
            }

            i++;
        }

        // Ajout du nombre si sortie de la boucle normalement (hors espace)
        if(i > j + 1) nbrList.push_back(nbr);

        return nbrList;
    }

    /**
     * Extrait les composants d'une transition
     **/
    void extractTransition(string s, int &depart, char &symb, int &arrive) {
        // Parcours de la chaine
        int i   = 0; // Iterateur
        int nbr = 0; // Nombre courant
        while(s[i] != '\0') {
            // Verification d'un chiffre
            if(s[i] >= '0' && s[i] <= '9') {
                // Chargement du nombre
                nbr = nbr * 10 + (int)s[i] - '0';
            } else {
                // Enregistrement de l'etat de depart
                depart = nbr;

                // Recuperation du symbole courant
                symb = s[i];

                // Reinitialisation du conteneur du nombre courant
                nbr = 0;
            }

            i++;
        }

        // Enregistrement de l'etat d'arrive
        arrive = nbr;
    }

    /**
     * Convertit une chaine en entier
     **/
    int stoi(string s) {
        // Parcours de la chaine
        int i   = 0; // Iterateur
        int nbr = 0; // Nombre courant
        while(s[i] >= '0' && s[i] <= '9') {
            // Chargement du nombre
            nbr = nbr * 10 + (int)s[i] - '0';

            i++;
        }

        return nbr;
    }

    /**
     * Affiche des espaces
     **/
    void printSpace(int n, ostream &os) {
        if(n <= 0) return;

        // Affichage de l'espace
        os << " ";

        // APPEL RECURSIF
        printSpace(n - 1, os);
    }

    /**
     * Affiche des espaces
     **/
    void printTitle(string s, ostream &os) {
        os << endl << "####" << endl;
        os << s << endl;
        os << "####" << endl << endl;
    }

    /**
     * Supprime les duplicatats
     **/
    void supprimDuplicata(vector<Etat*> &etats) {
        // Parcours des etats A
        for(vector<Etat*>::iterator itA = etats.begin(); itA != etats.end(); ++itA) {
            // Parcours des etats B
            for(vector<Etat*>::iterator itB = itA + 1; itB != etats.end(); ++itB) {
                // Verification de la correspondance des etats
                if(*itA == *itB) {
                    // Suppression de l'etat B
                    etats.erase(itB);

                    itB--; /// On a supprime => On reviens sur nos pas pour tester le nouveau courant
                }
            }
        }
    }

    /**
     * Supprime les duplicatats
     **/
    void supprimDuplicata(vector<Transition*> &transitions) {
        // Parcours des transitions A
        for(vector<Transition*>::iterator itA = transitions.begin(); itA != transitions.end(); ++itA) {
            // Parcours des transitions B
            for(vector<Transition*>::iterator itB = itA + 1; itB != transitions.end(); ++itB) {
                // Verification de la correspondance des transitions
                if(*itA == *itB) {
                    // Suppression de la transition B
                    transitions.erase(itB);

                    itB--; /// On a supprime => On reviens sur nos pas pour tester le nouveau courant
                }
            }
        }
    }

    /**
     * Verifie la presence d'un etat
     **/
    Etat* testPresenceEtatDansVecteur(const vector<Etat*> &v, const Etat &etat) {
        // Parcours du vecteur
        for(vector<Etat*>::const_iterator it = v.begin(); it != v.end(); ++it) {
            // Verification de l'egalite
            if(**it == etat) return *it;
        }

        return NULL;
    }

    /**
     * Recupere la partie d'un etat
     **/
    int recupPartie(const vector<vector<Etat*> > &partitions, const Etat *etat) {
        // Parcours de la partition
        for(vector<vector<Etat*> >::const_iterator it = partitions.begin(); it != partitions.end(); ++it) {
            // Parcours de la partie
            for(vector<Etat*>::const_iterator itB = (*it).begin(); itB != (*it).end(); ++itB) {
                // Verification de la correspondance de l'etat
                if(*itB == etat) {
                    return it - partitions.begin(); // Index de la partie dans la partition
                }
            }
        }

        return -1; // ERREUR => Etat non trouve
    }

    /**
     * Recupere la taille prise par un nombre
     **/
    int recupNombreTaille(int n) {
        // Verification du nombre 0
        if(!n) return 1; /// Rajoute 1 carractere pour gerer le 0 unique

        // Recuperation du nombre suivant
        int next = (int)(n / 10);

        // Verification du non traitement du dernier nombre
        if(next) return 1 + MPI_Tools::recupNombreTaille(next);

        return 1;
    }

    /**
     * Lis un mot
     **/
    string lisMot() {
        // Recupere le mot
        cout << "Entrez votre mot[a-z uniquement, sans accent et en minuscules] : ";
        string s;
        getline(cin, s);

        // Verification du mot de fin de lecture
        if(s == (string)FIN) return s;

        // Traitement de la chaine
        int i = 0;
        while(s[i] != '\0') {
            // Verification du caractere
            if(s[i] < 'a' || s[i] > 'z') {
                // On supprime le caractere interdit
                s.erase(s.begin() + i);

                // On reviens sur le caractere pour le traiter de nouveau
                i--;
            }

            i++;
        }

        // Affichage du mot reellement traite
        cout << "Apres traitement, le mot considere est : " << s << endl;

        return s;
    }
}
