#include "Automate.h"

using namespace std;

/**
 * CONSTRUCTEURS/DESTRUCTEURS
 **/
Automate::Automate(string nom): m_nom(nom), m_tailleA(0), m_estAsynchrone(true), m_estDeterministe(false), m_estComplet(false), m_estMinimal(false), m_estStandard(false) {
    // Recuperation de l'adresse du fichier
    string automateFileName = (string)DIR_BASE + "L2-" + (string)TEAM_NAME + "-" + nom + ".txt"; /// Par default: L2-TEAM_NAME-Default.txt

    // Ouverture du fichier
    ifstream automateFile(automateFileName.c_str(), ios::in);

    // Verification de la bonne ouverture du fichier
    if(automateFile) {
		string automateLine = ""; // Conteneur des lignes

		// Recuperation de la taille de l'alphabet
        getline(automateFile, automateLine); /// automateLine = NOMBRE DE SYMBOLES DANS L'ALPHABET DE L'AUTOMATE
        m_tailleA = MPI_Tools::stoi(automateLine);

        // Verification d'une taille correcte
        m_tailleA = (m_tailleA > 26)? 26 : m_tailleA;

        // Recuperation des etats
        getline(automateFile, automateLine); /// automateLine = NOMBRE D'ETATS
        for(int i = 0; i < MPI_Tools::stoi(automateLine); i++) {
			// Ajout de l'etat
            m_Q.push_back(new Etat(this, i));
        }

        vector<int> automateList; // Liste de nombre extrait depuis la ligne concerne

        // Recuperation des etats initiaux
        getline(automateFile, automateLine); /// automateLine = NOMBRE D'ETATS INITIAUX ET ETATS INITIAUX
        automateList = MPI_Tools::extractList(automateLine);
        // Parcours des etats initiaux
        for(int i = 0; i < automateList[0]; i++) {
            int numEtat = automateList[i + 1]; // Numero de l'etat concerne

			// Ajout de l'etat concerne
			m_I.push_back(m_Q[numEtat]);

			// Modification de l'etat concerne
            m_Q[numEtat]->setInitial(true);
        }

        // Recuperation des etats terminaux
        getline(automateFile, automateLine); /// automateLine = NOMBRE D'ETATS TERMINAUX ET ETATS TERMINAUX
        automateList = MPI_Tools::extractList(automateLine);
        // Parcours des etats finaux
        for(int i = 0; i < automateList[0]; i++) {
            int numEtat = automateList[i + 1]; // Numero de l'etat concerne

			// Ajout de l'etat concerne
			m_T.push_back(m_Q[numEtat]);

			// Modification de l'etat concerne
            m_Q[numEtat]->setFinal(true);
        }

        string automateLineBis = "";  // Conteneur des lignes secondaires
        int depart             = 0;   // Etat de depart
        char symb              = '*'; // Symbole de transition
        int arrive             = 0;   // Etat d'arrive

        // Recuperation des transitions
        getline(automateFile, automateLine); /// automateLine = NOMBRE DE TRANSITIONS
        for(int i = 0; i < MPI_Tools::stoi(automateLine); i++) {
			// Recuperation de la transition
            getline(automateFile, automateLineBis); /// automateLineBis = TRANSITION COURANTE

            // Recuperation des donnees
            MPI_Tools::extractTransition(automateLineBis, depart, symb, arrive);

			// Ajout de la transition
			m_E.push_back(new Transition(this, m_Q[depart], symb, m_Q[arrive]));
        }

        // Fermeture du fichier
        automateFile.close();

        // Suppression des transitions doubles
        supprimerTransitionsDoubles();
    } else {
        // Affichage du message d'erreur
        cout << "Le fichier n'a pas pu etre ouvert" << endl;

        // Affectation d'un nom d'erreur
        m_nom = "ERREUR";
    }
}
Automate::Automate(const Automate &aut): m_nom(aut.m_nom), m_tailleA(aut.m_tailleA), m_estAsynchrone(aut.m_estAsynchrone), m_estDeterministe(aut.m_estDeterministe), m_estComplet(aut.m_estComplet), m_estMinimal(aut.m_estMinimal), m_estStandard(aut.m_estStandard) {
    // Chargement des etats
    for(vector<Etat*>::const_iterator it = aut.m_Q.begin(); it != aut.m_Q.end(); ++it) {
        // Creation du nouvel etat
        Etat* etat = new Etat(**it);

        // Ajout de l'etat courant
        m_Q.push_back(etat);

        // Modification de l'automate
        etat->setAutomate(this);

        // Modification des transitions
        etat->freeTransition();

        // Verification d'etat initial
        if(etat->estInitial()) m_I.push_back(etat);

        // Verification d'etat final
        if(etat->estFinal()) m_T.push_back(etat);
    }

    // Chargement des transitions
    for(vector<Transition*>::const_iterator it = aut.m_E.begin(); it != aut.m_E.end(); ++it) {
        // Creation de la nouvelle transition
        Transition* transition = new Transition(**it);

        // Ajout de la transition courante
        m_E.push_back(transition);

        // Modification de l'automate
        transition->setAutomate(this);

        // Recuperation des index de l'etat de depart et l'etat d'arrive
        vector<Etat*>::const_iterator itb = find(aut.m_Q.begin(), aut.m_Q.end(), transition->recupEtatDepart()); /// aut.m_Q = Etats de l'automate de depart
        if(itb != aut.m_Q.end()) {
            transition->setEtatDepart(m_Q[itb - aut.m_Q.begin()]); /// m_Q = Etats de l'automate de copie
        }
        itb = find(aut.m_Q.begin(), aut.m_Q.end(), transition->recupEtatArrive()); /// aut.m_Q = Etats de l'automate de depart
        if(itb != aut.m_Q.end()) {
            transition->setEtatArrive(m_Q[itb - aut.m_Q.begin()]); /// m_Q = Etats de l'automate de copie
        }
    }
}
Automate::~Automate() {
    // Parcours des etats
    for(vector<Etat*>::iterator it = m_Q.begin(); it != m_Q.end(); ++it) {
        // Liberation de l'etat courant
        delete *it;
    }

    // Parcours des transitions
    for(vector<Transition*>::iterator it = m_E.begin(); it != m_E.end(); ++it) {
        // Liberation de la transition courante
        delete *it;
    }
}

/**
 * Retourne le nom de l'automate
 **/
string Automate::recupNom() const {
    return m_nom;
}

/**
 * Affiche l'automate
 **/
void Automate::affichage(ostream &os) const {
    // Initialisation des variables
    int tailleEtat       = recupTailleEtatMax(); // Taille maximale des etats

    // Affichage du nom de l'automate
    os << "AUTOMATE : " << m_nom << endl;

    /// AFFICHAGE LIGNE TITRE
    os << "    ";

    // Eccart pour le nom des etats
    MPI_Tools::printSpace(tailleEtat, os);

    // Parcours de l'alphabet
    for(int i = 0; i < m_tailleA; i++) {
        char symb = (char)('a' + i); // Symbole courant

        // Affichage du symbole
        os << " | " << symb;
        MPI_Tools::printSpace(recupTailleTransitionMax(symb) - 1, os);
    }

    // CAS EPSILON
    os << " | " << EPSILON;
    MPI_Tools::printSpace(recupTailleTransitionMax(EPSILON) - 1, os);

    os << endl;

    /// AFFICHAGE LIGNE D'ETAT
    // Parcours des etats
    for(vector<Etat*>::const_iterator it = m_Q.begin(); it != m_Q.end(); ++it) {
        os << (((*it)->estFinal())? "< " : "  ");
        os << (((*it)->estInitial())? "> " : "  ");

        // Affichage de l'etat
        os << **it;
        MPI_Tools::printSpace(tailleEtat - (*it)->recupTailleNom(), os);

        // Parcours de l'alphabet
        for(int i = 0; i < m_tailleA; i++) {
            char symb = (char)('a' + i); // Symbole courant

            os << " | ";

            // Recuperation des transitions sortantes
            vector<Etat*> etatsCibles = (*it)->recupEtatsCibles(symb);

            // Affichage de la transition du symbole
            for(vector<Etat*>::iterator itb = etatsCibles.begin(); itb != etatsCibles.end(); ++itb) {
                // Affichage de l'etat courant
                os << **itb;

                // Verification du non traitement du dernier element
                if(itb != etatsCibles.end() - 1) os << SEPARATEUR_ETAT;
            }

            // Affichage de l'espacement restant
            MPI_Tools::printSpace(recupTailleTransitionMax(symb) - (*it)->recupTailleTransition(symb), os);
        }

        // CAS EPSILON
        os << " | ";

        // Recuperation des transitions sortantes
        vector<Etat*> etatsCibles = (*it)->recupEtatsCibles(EPSILON);

        // Affichage de la transition du symbole
        for(vector<Etat*>::iterator itb = etatsCibles.begin(); itb != etatsCibles.end(); ++itb) {
            // Affichage de l'etat courant
            os << **itb;

            // Verification du non traitement du dernier element
            if(itb != etatsCibles.end() - 1) os << SEPARATEUR_ETAT;
        }

        // Affichage de l'espacement restant
        MPI_Tools::printSpace(recupTailleTransitionMax(EPSILON) - (*it)->recupTailleTransition(EPSILON), os);

        os << endl;
    }

    // Affichage de la legende
    os << "***************" << endl;
    os << "** LEGENDE : **" << endl;
    os << "***************" << endl;
    os << "> : Etat(s) Initial/aux" << endl;
    os << "< : Etats Terminaux" << endl;
    os << GROUP_ETAT_0 << " : Separateur niveau 0" << endl;
    os << GROUP_ETAT_1 << " : Separateur niveau 1" << endl;
    os << GROUP_ETAT_2 << " : Separateur niveau 2" << endl;
    os << SEPARATEUR_ETAT << " : Separateur de liste d'etats" << endl;
}

/**
 * Test la reconnaissance d'un mot
 **/
bool Automate::testMot(string s) const {
    // Creation d'un automate secondaire de travail
    Automate aut = *this;

    // Verification d'automate synchrone
    if(m_estAsynchrone) {
        aut.synchronisation();
    }

    // Parcours des etats initiaux
    for(vector<Etat*>::const_iterator it = aut.m_I.begin(); it != aut.m_I.end(); ++it) {
        // Lancement de la reconnaissance depuis l'etat courant
         if((*it)->testMot(s)) return true;
    }

    return false;
}

/**
 * Determinise et complete l'automate
 **/
void Automate::determiniserEtCompleter() {
    MPI_Tools::printTitle("DETERMINISATION ET COMPLETION D'AUTOMATE QUELCONQUE");

    // Verification d'automate synchrone
    if(testAutomateAsynchrone()) {
        // Determinisation et completion de l'automate
        determinisationEtCompletionAutomateAsynchrone();
    } else {
        // Verification d'automate deterministe
        if(testAutomateDeterministe()) {
            // Verification d'automate non complet
            if(!testAutomateComplet()) {
                // Completion de l'automate
                completion();
            }
        } else {
            // Determinisation et completion de l'automate
            determinisationEtCompletionAutomateSynchrone();
        }
    }

    MPI_Tools::printTitle("=> L'AUTOMATE DETERMINISTE COMPLET EST :");
    cout << *this;
}

/**
 * Minimise l'automate
 **/
void Automate::minimiser() {
    MPI_Tools::printTitle("MINIMISATION D'AUTOMATE SYNCHRONE DETERMINISTE COMPLET");

    // Minimisation de l'automate
    minimisation();

    MPI_Tools::printTitle("=> L'AUTOMATE DETERMINISTE COMPLET MINIMAL EST :");
    cout << *this;
}

/**
 * Retourne le complementaire de l'automate
 **/
Automate* Automate::automateComplementaire() {
    // Creation de l'automate secondaire
    Automate* aut = new Automate(*this);

    // Modification du nom
    aut->m_nom = "COMPLEMENTAIRE - " + aut->m_nom;

    // Verification d'un automate asynchrone
    if(m_estAsynchrone && testAutomateAsynchrone()) {
        MPI_Tools::printTitle("=> AUTOMATE NON SYNCHRONE");

        aut->m_nom = "ERREUR";

        return aut;
    }

    // Verification d'un automate deterministe
    if(!m_estDeterministe && !testAutomateDeterministe()) {
        MPI_Tools::printTitle("=> AUTOMATE NON DETERMINISTE");

        aut->m_nom = "ERREUR";

        return aut;
    }

    // Verification d'un automate complet
    if(!m_estComplet && !testAutomateComplet()) {
        MPI_Tools::printTitle("=> AUTOMATE NON COMPLET");

        aut->m_nom = "ERREUR";

        return aut;
    }

    /*****************
     * ALGO DU COURS *
     * vvvvvvvvvvvvv *
     *****************/
    /// Changement : T' = Q\T
    // Sauvegarde des precedents etats terminaux
    vector<Etat*> old_T = aut->m_T;

    // Vidage des etats terminaux
    aut->m_T.clear();

    // Parcours des etats
    for(vector<Etat*>::iterator it = aut->m_Q.begin(); it != aut->m_Q.end(); ++it) {
        // Verification de la correspondance de l'etat courant dans les anciens terminaux
        if(find(old_T.begin(), old_T.end(), *it) == old_T.end()) {
            // Ajout du nouvel etat terminal
            aut->m_T.push_back(*it);

            // MAJ de son etat
            (*it)->setFinal(true);
        } else {
            // MAJ de son etat
            (*it)->setFinal(false);
        }
    }
    /*****************
     * ^^^^^^^^^^^^^ *
     * ALGO DU COURS *
     *****************/

    return aut;
}

/**
 * Standardise l'automate
 **/
void Automate::standardiser() {
    MPI_Tools::printTitle("STANDARDISATION D'AUTOMATE QUELCONQUE");

    // Standardisation de l'automate
    standardisation();

    MPI_Tools::printTitle("=> L'AUTOMATE STANDARD EST :");
    cout << *this;
}

/**
 * Verifie si un automate est asynchrone
 **/
bool Automate::testAutomateAsynchrone() {
    MPI_Tools::printTitle("TEST : Automate Asynchrone ?");

    // Parcours des transitions de l'automate
    bool estAsynchrone = false;
    for(vector<Transition*>::const_iterator it = m_E.begin(); it != m_E.end(); ++it) {
        // Verification d'une transition EPSILON sur la transition courrante
        if((*it)->recupSymbole() == EPSILON) {
            // Affichage du message de conclusion
            cout << **it << " est une transition EPSILON" << endl;

            estAsynchrone = true;

            break;
        }
    }

    // MAJ des donnees de l'obbjet
    m_estAsynchrone = estAsynchrone;

    MPI_Tools::printTitle((m_estAsynchrone)? "=> AUTOMATE ASYNCHRONE" : "=> AUTOMATE SYNCHRONE");

    return m_estAsynchrone;
}

/**
 * Verifie si un automate est deterministe
 **/
bool Automate::testAutomateDeterministe() {
    MPI_Tools::printTitle("TEST : Automate Deterministe ?");

    // Verification d'automate asynchrone
    if(m_estAsynchrone && testAutomateAsynchrone()) {
        // Forcage de sortie
        m_estDeterministe = false;

        return false;
    }

    bool estDeterministe = true; // Verifie l'etat determinsite de l'automate

    // Verification du nombre d'etats initiaux
    int nbrInit = m_I.size();
    if(nbrInit > 1) {
        // Affichage du message de conclusion
        cout << "L'automate possede " << nbrInit << " etats initiaux" << endl;

        estDeterministe = false;
    } else {
        // Parcours des etats de l'automate
        for(vector<Etat*>::const_iterator it = m_Q.begin(); it != m_Q.end(); ++it) {
            // Parcours de l'alphabet
            for(int i = 0; i < m_tailleA; i++) {
                // Recuperation du nombre de transitions pour l'etat et la lettre courante
                int nbrTransitions = (*it)->recupEtatsCibles((char)('a' + i)).size();

                // Verification du nombre de transitions
                if(nbrTransitions > 1) {
                    // Affichage du message de conclusion
                    cout << "Depuis l'etat " << **it << " il est possible de passer a " << nbrTransitions << " etats cibles via la lettre " << (char)('a' + i) << endl;

                    estDeterministe = false;

                    break;
                }
            }
        }
    }

    //MAJ des donnees de l'objet
    m_estDeterministe = estDeterministe;

    MPI_Tools::printTitle((m_estDeterministe)? "=> AUTOMATE DETERMINISTE" : "=> AUTOMATE NON DETERMINISTE");

    return m_estDeterministe;
}

/**
 * Verifie si un automate est complet
 **/
bool Automate::testAutomateComplet() {
    MPI_Tools::printTitle("TEST : Automate Complet ?");

    // Parcours des etats de l'automate
    bool estComplet = true;
    for(vector<Etat*>::const_iterator it = m_Q.begin(); it != m_Q.end(); ++it) {
        // Parcours de l'alphabet
        for(int i = 0; i < m_tailleA; i++) {
            // Verification du nombre de transitions pour l'etat et la lettre courante
            if((*it)->recupEtatsCibles((char)('a' + i)).size() == 0) {
                // Affichage du message de conclusion
                cout << "Depuis l'etat " << **it << " il n'y a pas d'etat cible via la lettre " << (char)('a' + i) << endl;

                estComplet = false;

                break;
            }
        }
    }

    //MAJ des donnees de l'objet
    m_estComplet = estComplet;

    MPI_Tools::printTitle((m_estComplet)? "=> AUTOMATE COMPLET" : "=> AUTOMATE NON COMPLET");

    return m_estComplet;
}

/**
 * Verifie si un automate est standard
 **/
bool Automate::testAutomateStandard() {
    MPI_Tools::printTitle("TEST : Automate Standard ?");

    bool estStandard = true; // Verifie l'etat standard de l'automate

    // Verification du nombre d'etats initiaux
    int nbrInit = m_I.size();
    if(nbrInit > 1) {
        // Affichage du message de conclusion
        cout << "L'automate possede " << nbrInit << " etats initiaux" << endl;

        estStandard = false;
    } else if(nbrInit == 1) {
        // Verification du nombre de transitions entrantes
        int nbrTransEntrante = m_I[0]->recupNbrTransitionsEntrantes();
        if(nbrTransEntrante > 0) {
            // Affichage du message de conclusion
            cout << "L'etat initial " << *m_I[0] << " possede " << nbrTransEntrante << " transitions entrantes" << endl;

            estStandard = false;
        }
    }

    //MAJ des donnees de l'objet
    m_estStandard = estStandard;

    MPI_Tools::printTitle((m_estStandard)? "=> AUTOMATE STANDARD" : "=> AUTOMATE NON STANDARD");

    return m_estStandard;
}

/**
 * Determinise et complete l'automate asynchrone
 **/
void Automate::determinisationEtCompletionAutomateAsynchrone() {
    MPI_Tools::printTitle("DEBUT DE LA DETERMINISATION ET COMPLETION D'AUTOMATE ASYNCHRONE");

    // Verification d'automate synchrone
    if(!m_estAsynchrone || !testAutomateAsynchrone()) {
        MPI_Tools::printTitle("=> AUTOMATE NON ASYNCHRONE");

        // Determinisation et completion d'automate synchrone
        determinisationEtCompletionAutomateSynchrone();

        return;
    }

    // Rend l'automate syncrhone
    synchronisation();

    // Determine et complete l'automate synchrone
    determinisationEtCompletionAutomateSynchrone();

    MPI_Tools::printTitle("=> AUTOMATE ASYNCHRONE DETERMINISE ET COMPLETE");
}

/**
 * Complete l'automate
 **/
void Automate::completion() {
    MPI_Tools::printTitle("COMPLETION DE L'AUTOMATE");

    // Verification d'un automate complet
    if(m_estComplet || testAutomateComplet()) {
        MPI_Tools::printTitle("=> AUTOMATE DEJA COMPLET");

        return;
    }

    // Verification d'un automate asynchrone
    if(m_estAsynchrone && testAutomateAsynchrone()) {
        MPI_Tools::printTitle("=> AUTOMATE NON SYNCHRONE");

        return;
    }

    /*****************
     * ALGO DU COURS *
     * vvvvvvvvvvvvv *
     *****************/
    /// Ajout de l'etat poubelle
    Etat *etatPoubelle = new Etat(this, m_Q.size());
    m_Q.push_back(etatPoubelle); /// ETAT lie a l'automate /// NUMERO etant le nombre d'etat

    /// Ajout des transitions de l'automate non complet sont lies a l'automate

    /// Ajout des transitions (q,x,P) absentes de E, dont les (P,x,P)
    // Parcours des etats de l'automate
    for(vector<Etat*>::const_iterator it = m_Q.begin(); it != m_Q.end(); ++it) {
        // Parcours de l'alphabet
        for(int i = 0; i < m_tailleA; i++) {
            // Verification du nombre de transitions pour l'etat et la lettre courante
            char symb = (char)('a' + i);
            if((*it)->recupEtatsCibles(symb).size() == 0) {
                // Ajout de la transition
                m_E.push_back(new Transition(this, *it, symb, etatPoubelle));
            }
        }
    }
    /*****************
     * ^^^^^^^^^^^^^ *
     * ALGO DU COURS *
     *****************/

    //MAJ des donnees de l'objet
    m_estComplet = true;
    m_estMinimal = false;

    MPI_Tools::printTitle("=> AUTOMATE COMPLETE");
}

/**
 * Determinise et complete l'automate synchrone
 **/
void Automate::determinisationEtCompletionAutomateSynchrone() {
    MPI_Tools::printTitle("DEBUT DE LA DETERMINISATION ET COMPLETION D'AUTOMATE SYNCHRONE");

    // Verification d'automate asynchrone
    if(m_estAsynchrone && testAutomateAsynchrone()) {
        MPI_Tools::printTitle("=> AUTOMATE NON SYNCHRONE");

        // Determine et complete l'automate asynchrone
        determinisationEtCompletionAutomateAsynchrone();

        return;
    }

    // Determinisation de l'automate
    determinisation();

    // Completion de l'automate
    completion();

    MPI_Tools::printTitle("=> AUTOMATE SYNCHRONE DETERMINISE ET COMPLETE");
}

/**
 * Transforme l'automate en automate synchrone
 **/
void Automate::synchronisation() {
    MPI_Tools::printTitle("MISE EN SYNCHRONE DE L'AUTOMATE");

    // Verification d'un automate synchrone
    if(!m_estAsynchrone || !testAutomateAsynchrone()) {
        MPI_Tools::printTitle("=> AUTOMATE DEJA SYNCHRONE");

        return;
    }

    /*****************
     * ALGO DU COURS *
     *  MOMIRandum   *
     * vvvvvvvvvvvvv *
     *****************/
    // Initialisation des variables
    vector<Etat*>       new_I; // Nouveau conteneur des etats initiaux
    vector<Etat*>       new_T; // Nouveau conteneur des etats terminaux
    vector<Transition*> new_E; // Nouveau conteneur des transitions

    /// Initialisation avec les etats et les transitions de l'automate de base
    new_I = m_I;
    new_T = m_T;
    new_E = m_E;

    /// Parcours des etats
    for(vector<Etat*>::iterator it = m_Q.begin(); it != m_Q.end(); ++it) {
        /// Parcours des transitions entrantes
        // Recuperation des transitions
        vector<Transition*> transEntrantes = (*it)->recupTransitionArrive();

        // Parcours des transitions epsilon
        for(vector<Transition*>::iterator itB = transEntrantes.begin(); itB != transEntrantes.end(); ++itB) {
            // Verification d'une transition epsilon
            if((*itB)->recupSymbole() == EPSILON) {
                /// Creation des fermeture transitive
                // Recuperation des transitions sortantes
                vector<Transition*> transSortantes = (*it)->recupTransitionDepart();

                // Parcours des transitions sortantes
                for(vector<Transition*>::iterator itC = transSortantes.begin(); itC != transSortantes.end(); ++itC) {
                    // Creation de la fermeture
                    new_E.push_back(new Transition(this, (*itB)->recupEtatDepart(), (*itC)->recupSymbole(), (*itC)->recupEtatArrive()));
                }

                /// Transmission d'etat initial et final
                // Verification d'etat terminal
                if((*it)->estFinal()) {
                    // Recuperation de l'etat terminal
                    Etat* etatT = (*itB)->recupEtatDepart();

                    // Mise en etat terminal
                    etatT->setFinal(true);

                    // Ajout de l'etat terminal
                    new_T.push_back(etatT);
                }

                // Verification d'etat initial
                if((*itB)->recupEtatDepart()->estInitial()) {
                    // Mise en etat initial
                    (*it)->setInitial(true);

                    // Ajout de l'etat initial
                    new_T.push_back(*it);
                }

                /// Suppression de la transition entrante epsilon traite
                // Suppression de la transition sortante
                (*itB)->recupEtatDepart()->remTransitionDepart(*itB);

                // Suppression de la transition entrante
                (*it)->remTransitionArrive(*itB);

                // Recuperation du pointeur vers l'element
                vector<Transition*>::iterator elmt = find(new_E.begin(), new_E.end(), *itB);

                // Verification de la bonne recuperation
                if(elmt != new_E.end()) new_E.erase(elmt);

                // Liberation de la transition
                delete *itB;
            }
        }
    }

    /// MAJ des donnees
    m_I = new_I;
    m_T = new_T;
    m_E = new_E;
    /*****************
     * ^^^^^^^^^^^^^ *
     * ALGO DU COURS *
     *  MOMIRandum   *
     *****************/

    //MAJ des donnees de l'objet
    m_estAsynchrone = false;

    MPI_Tools::printTitle("=> AUTOMATE DEVENU SYNCHRONE");
}

/**
 * Determinise l'automate
 **/
void Automate::determinisation() {
    MPI_Tools::printTitle("DETERMINISATION DE L'AUTOMATE");

    // Verification d'un automate deterministe
    if(m_estDeterministe || testAutomateDeterministe()) {
        MPI_Tools::printTitle("=> AUTOMATE DEJA DETERMINISTE");

        return;
    }

    // Verification d'un automate asynchrone
    if(m_estAsynchrone && testAutomateAsynchrone()) {
        MPI_Tools::printTitle("=> AUTOMATE NON SYNCHRONE");

        return;
    }

    /*****************
     * ALGO DU COURS *
     * vvvvvvvvvvvvv *
     *****************/
    // Initialisation des variables
    vector<Etat*> etats;             // Conteneurs d'etat dupliques
    vector<Etat*> etatsOrigin;       // Conteneurs d'etat originaux
    vector<vector<Etat*> > aTraiter; // Conteneurs de groupes d'etats a traiter
    vector<Etat*> new_Q;             // Nouveau conteneur des etats
    vector<Etat*> new_I;             // Nouveau conteneur des etats initiaux
    vector<Etat*> new_T;             // Nouveau conteneur des etats terminaux
    vector<Transition*> new_E;       // Nouveau conteneur des transitions


    /// Initialisation de l'etat initial unique
    // Parcours des etats initiaux
    etatsOrigin.push_back(NULL); // Place reserve a l'etat global
    bool initialEstFinal = false; /// Test si l'etat initial devra egalement etre final
    for(vector<Etat*>::const_iterator it = m_I.begin(); it != m_I.end(); ++it) {
        // Copie de l'etat
        Etat *etat = new Etat(**it);
        etat->freeTransition();

        // Ajout de l'etat
        etats.push_back(etat);
        etatsOrigin.push_back(*it);

        // Verification d'etat final
        if((*it)->estFinal()) initialEstFinal = true;
    }
    // Creation du nouvel etat initial unique
    Etat *etatInitial = new Etat(this, etats, true, initialEstFinal);

    // Ajout de l'etat initial unique
    etatsOrigin[0] = etatInitial;
    new_Q.push_back(etatInitial);
    new_I.push_back(etatInitial);

    /// Marquage "a_traiter"
    aTraiter.push_back(etatsOrigin);

    // Reinitialisation du conteneur
    etats.clear();
    etatsOrigin.clear();

    /// Parcours des "a_traiter"
    while(aTraiter.size()) {
        /// Choix d'un etat a traiter
        vector<Etat*> etatP = aTraiter[0]; // Selection du premier element pour le traiter

        /// Parcours de l'alphabet
        for(int i = 0; i < m_tailleA; i++) {
            // Determination du symbole
            char symb = (char)('a' + i);

            /// Determination des q dont une transition les lie par un symbbole a p'
            // Parcours des etats de p'
            etatsOrigin.push_back(NULL); // Place reserve a l'etat global
            for(vector<Etat*>::const_iterator it = etatP.begin() + 1; it != etatP.end(); ++it) {
                // Recuperation des etats cibles
                vector<Etat*> etatsCible = (*it)->recupEtatsCibles(symb);

                // Ajout des etat cibles possibles
                etatsOrigin.insert(etatsOrigin.end(), etatsCible.begin(), etatsCible.end());
            }
            // Suppression des duplicatats
            MPI_Tools::supprimDuplicata(etatsOrigin);

            // Verification d'au moins un etat cible
            if(etatsOrigin.size() > 1) {
                // Parcours des etats q selectionnes
                bool estFinal = false; /// Determination d'etat final
                for(vector<Etat*>::const_iterator it = etatsOrigin.begin() + 1; it != etatsOrigin.end(); ++it) {
                    // Copie de l'etat
                    Etat *etat = new Etat(**it);
                    etat->freeTransition();

                    // Ajout de l'etat
                    etats.push_back(etat);

                    // Verification d'etat final
                    if((*it)->estFinal()) estFinal = true;
                }
                // Creation de l'etat global
                Etat *newEtat = new Etat(this, etats, false, estFinal);

                // Ajout de l'etat global au conteneur
                etatsOrigin[0] = newEtat;

                // Recuperation de l'etat existant s'il y a
                Etat* etatExistant = MPI_Tools::testPresenceEtatDansVecteur(new_Q, *etatsOrigin[0]);

                // Verification de la non pre-existance de l'etat
                if(!etatExistant) {
                    /// Ajout de la transition
                    // Creation de la transition
                    new_E.push_back(new Transition(this, etatP[0], symb, etatsOrigin[0])); /// etatsOrigin = etatQ

                    /// Marquage "a_traiter"
                    aTraiter.push_back(etatsOrigin);

                    // Ajout de l'etat global aux etats
                    new_Q.push_back(newEtat);
                    if(estFinal) new_T.push_back(newEtat);
                } else {
                    /// Ajout de la transition
                    // Creation de la transition
                    new_E.push_back(new Transition(this, etatP[0], symb, etatExistant)); /// etatExistant = etatQ equivalent dans new_Q
                }

                // Reinitialisation du conteneur
                etats.clear();
            }

            // Reinitialisation du conteneur
            etatsOrigin.clear();
        }

        /// Fin de traitement, => "traite"
        aTraiter.erase(aTraiter.begin());
    }

    /// Traitement des etats terminaux realise avec celui des etats

    /// EXTRA : MAJ des donnees
    // Parcours des etats
    for(vector<Etat*>::iterator it = m_Q.begin(); it != m_Q.end(); ++it) {
        // Liberation de l'etat courant
        delete *it;
    }

    // Parcours des transitions
    for(vector<Transition*>::iterator it = m_E.begin(); it != m_E.end(); ++it) {
        // Liberation de la transition courante
        delete *it;
    }
    m_Q = new_Q;
    m_I = new_I;
    m_T = new_T;
    m_E = new_E;
    /*****************
     * ^^^^^^^^^^^^^ *
     * ALGO DU COURS *
     *****************/

    // Elimination des etats et transitions doubles
    supprimerEtatDoubles();
    supprimerTransitionsDoubles();

    //MAJ des donnees de l'objet
    m_estDeterministe = true;

    MPI_Tools::printTitle("=> AUTOMATE DETERMINISE");
}

/**
 * Minimise l'automate
 **/
void Automate::minimisation() {
    MPI_Tools::printTitle("MINIMISATION DE L'AUTOMATE");

    // Verification d'un automate minimal
    if(m_estMinimal) {
        MPI_Tools::printTitle("=> AUTOMATE DEJA MINIMAL");

        return;
    }

    // Verification d'un automate asynchrone
    if(m_estAsynchrone && testAutomateAsynchrone()) {
        MPI_Tools::printTitle("=> AUTOMATE NON SYNCHRONE");

        return;
    }

    // Verification d'un automate deterministe
    if(!m_estDeterministe && !testAutomateDeterministe()) {
        MPI_Tools::printTitle("=> AUTOMATE NON DETERMINISTE");

        return;
    }

    // Verification d'un automate complet
    if(!m_estComplet && !testAutomateComplet()) {
        MPI_Tools::printTitle("=> AUTOMATE NON COMPLET");

        return;
    }

    // Recuperation du nombre d'etats de l'automate a minimiser
    unsigned int nbrEtatAvant = m_Q.size();

    // Mise de l'automate en automate accessible
    accessibilisation();

    /*****************
     * ALGO DU COURS *
     * vvvvvvvvvvvvv *
     *****************/
    // Initialisation des variables
    vector<vector<Etat*> > partitions;     // Conteneur des parties
    vector<vector<Etat*> > partitionsTemp; // Conteneur temporaire des parties
    vector<Etat*> etats;                   // Conteneur d'etats

    /// Initialisation de la partition initiale
    // Creation des parties
    vector<Etat*> etatTerminaux;    // Partie des etats terminaux
    vector<Etat*> etatNonTerminaux; // Partie des etats non terminaux

    // Parcours des etats
    for(vector<Etat*>::iterator it = m_Q.begin(); it != m_Q.end(); ++it) {
        // Verification que l'etat est terminal
        if((*it)->estFinal()) etatTerminaux.push_back(*it);
        else etatNonTerminaux.push_back(*it);
    }

    // Ajout des parties a la partition
    partitions.push_back(etatTerminaux);
    partitions.push_back(etatNonTerminaux);

    /// Boucle de decoupage
    int diffTaille = 0; // Determine la difference de nombre d'etats avant et apres le traitement
    int decoupage  = 0; // Indique le no du decoupage en cours
    do {
        // Affichage du decoupage
        decoupage++;
        afficheTableTransitionPartition(partitions, decoupage, cout);

        /// Parcours de la partition
        for(vector<vector<Etat*> >::iterator it = partitions.begin(); it != partitions.end(); ++it) {
            /// Verification d'une partie compose d'un unique etat
            if((*it).size() == 1) {
                // Ajout de la partie sans autre traitement
                partitionsTemp.push_back(*it);
            } else {
                /// Separation de la partie en sous-parties
                // Recuperation de la partie, pour ne pas alterer la partition de base
                vector<Etat*> partie = *it;

                // Boucle de separation de la partie
                while(partie.size()) {
                    Etat *etat0 = partie[0];

                    // Ajout du premier etat
                    etats.push_back(etat0);

                    // Suppression de l'etat de la partie precedente
                    partie.erase(partie.begin());

                    // Parcours des etats restants de la partie
                    for(vector<Etat*>::iterator itB = partie.begin(); itB != partie.end(); ++itB) {
                        bool estIdentique = true; // Verifie que toutes les transitions pointent vers la meme partie que l'etat 0

                        // Parcours de l'alphabet
                        for(int i = 0; i < m_tailleA; i++) {
                            // Recuperation du symbole
                            char symb = (char)('a' + i);

                            // Recuperation des etats cibles de l'etat 0 et de l'etat courant par le symbole
                            Etat *etatCible0 = etat0->recupEtatsCibles(symb)[0];  /// DETERMINISTE => 1 Seulle cible
                            Etat *etatCibleC = (*itB)->recupEtatsCibles(symb)[0]; /// DETERMINISTE => 1 Seulle cible

                            // Recuperation de les parties associes aux etats
                            int partieCible0 = MPI_Tools::recupPartie(partitions, etatCible0);
                            int partieCibleC = MPI_Tools::recupPartie(partitions, etatCibleC);

                            // Verification de la non correspondance des parties cibles
                            if(partieCible0 != partieCibleC) {
                                // L'etat courant n'a pas le meme etat cible, on a la preuve qu'ils n'appartiennent pas a la meme partie
                                estIdentique = false;

                                break;
                            }
                        }

                        // Verification d'un etat identique
                        if(estIdentique) {
                            // Ajout de l'etat
                            etats.push_back(*itB);

                            // Suppression de l'etat de la partie precedente
                            partie.erase(itB);

                            // On revient sur nos pas pour tester le courant au prochain tour de boucle
                            itB--;
                        }
                    }

                    // Ajout de la sous-partie a la partition
                    partitionsTemp.push_back(etats);

                    // Vidage des etats
                    etats.clear();
                }
            }
        }

        /// La nouvelle partition est definie par l'ensemble des parties resultant des divisions eventuelles
        diffTaille = partitionsTemp.size() - partitions.size(); // Difference entre le nombre de parties a l'arrive et au depart
        partitions = partitionsTemp;
        partitionsTemp.clear();
    } while(diffTaille != 0); /// Si la difference est nulle, alors il n'y a pas eu de separation => Autant de partie au depart qu'a l'arrive

    /// Les etats sont les parties issues de la derniere etape de division
    /// L'etat initial est la partie contenant l'etat initial de l'automate a minimiser
    /// Les etats terminaux sont les parties qui contiennent les etats terminaux de l'automate a minimiser
    // Vidage du conteneur des etats
    m_Q.clear();

    // Parcours de la partition
    for(vector<vector<Etat*> >::iterator it = partitions.begin(); it != partitions.end(); ++it) {
        bool estInitial  = false; // Verifie s'il s'agit d'un etat initial
        bool estTerminal = false; // Verifie s'il s'agit d'un etat terminal

        // Parcours des etats
        for(vector<Etat*>::iterator itB = (*it).begin(); itB != (*it).end(); ++itB) {
            // Verification d'etat initial
            if((*itB)->estInitial()) estInitial = true;
            if((*itB)->estFinal()) estTerminal  = true;

            // On arrete la boucle si l'on trouve les deux statuts
            if(estInitial && estTerminal) break;
        }

        // Ajout de l'etat global
        m_Q.push_back(new Etat(this, *it, estInitial, estTerminal));
    }

    /// Les transitions sont reecrites en terme de parties
    // Creation du conteneur des nouvelles transitions
    vector<Transition*> new_E;

    // Parcours des nouveaux etats
    for(vector<Etat*>::const_iterator it = m_Q.begin(); it != m_Q.end(); ++it) {
        // Parcours de l'alphabet
        for(int i = 0; i < m_tailleA; i++) {
            // Recuperation du symbole
            char symb = (char)('a' + i);

            // Recuperation du premier des etats fusionne
            Etat *etat = (*it)->recupGroupEtat()[0];

            // Recuperation de l'etat cible
            Etat *etatCible = etat->recupEtatsCibles(symb)[0]; /// DETERMINISTE => 1 Seulle cible

            // Recuperation de la partie de l'etat cible
            int index = MPI_Tools::recupPartie(partitions, etatCible);

            // Ajout de l'etat
            new_E.push_back(new Transition(this, *it, symb, m_Q[index])); /// Les nouveaux etats sont cree a partir de la partition, donc les index correspondent
        }
    }

    // Parcours des transitions
    for(vector<Transition*>::iterator it = m_E.begin(); it != m_E.end(); ++it) {
        // Liberation de la transition courante
        delete *it;
    }
    m_E = new_E;
    /*****************
     * ^^^^^^^^^^^^^ *
     * ALGO DU COURS *
     *****************/

    //MAJ des donnees de l'objet
    m_estMinimal = true;

    // Verification de la correspondance du nombre d'etats avec l'automate de depart
    if(m_Q.size() == nbrEtatAvant) {
        /*
         * L'automate etait deja minimal
         * CAR Il existe un et un unique automate minimal
         * => si le nombre d'etat n'a pas change
         * => l'automate obtenu est le meme que l'automate de depart
         */
        MPI_Tools::printTitle("=> AUTOMATE DEJA MINIMAL");
    } else {
        MPI_Tools::printTitle("=> AUTOMATE MINIMISE");
    }
}

/**
 * Standardise l'automate
 **/
void Automate::standardisation() {
    MPI_Tools::printTitle("STANDARDISATION DE L'AUTOMATE");

    // Verification d'un automate standard
    if(m_estStandard || testAutomateStandard()) {
        MPI_Tools::printTitle("=> AUTOMATE DEJA STANDARD");

        return;
    }

    /*****************
     * ALGO DU COURS *
     * vvvvvvvvvvvvv *
     *****************/
    /// Ajout d'un nouvel etat
    Etat *etatInitial = new Etat(this, m_Q.size(), true);
    m_Q.push_back(etatInitial); /// ETAT lie a l'automate /// NUMERO etant le nombre d'etat

    /// Le nouvel etat devient le seul etat intial
    // Stockage des anciens etats initiaux
    vector<Etat*> old_I = m_I;

    // Vidage des etats initiaux
    m_I.clear();

    // Ajout du nouvel etat initial
    m_I.push_back(etatInitial);

    /// Le nouvel etat initial devient etat terminal si et seulement si l'un des anciens etats initiaux etait egalement final
    /// On ajoute les transitions qui partaient depuis les anciens etats initiaux
    // Parcours des anciens etats initiaux
    for(vector<Etat*>::iterator it = old_I.begin(); it != old_I.end(); ++it) {
        // Changement de l'etat d'etat initial
        (*it)->setInitial(false);

        // Verification d'etat final
        if((*it)->estFinal()) etatInitial->setFinal(true);

        // Recuperation des transitions sortantes
        vector<Transition*> transitionsDepart = (*it)->recupTransitionDepart();

        // Parcours des transitions sortantes
        for(vector<Transition*>::const_iterator it = transitionsDepart.begin(); it != transitionsDepart.end(); ++it) {
            // Ajout de la transition (I,x,ETAT) avec ETAT l'etat cible de la transition courante
            m_E.push_back(new Transition(this, etatInitial, (*it)->recupSymbole(), (*it)->recupEtatArrive()));
        }
    }
    /*****************
     * ^^^^^^^^^^^^^ *
     * ALGO DU COURS *
     *****************/

    //MAJ des donnees de l'objet
    m_estStandard = true;
    m_estComplet  = false;
    m_estMinimal  = false;

    MPI_Tools::printTitle("=> AUTOMATE STANDARDISE");
}

/**
 * OPPERATEUR <<
 **/
ostream& operator<<(std::ostream &os, const Automate &aut) {
    // Affichage de l'automate
    aut.affichage(os);

    return os;
}

/*******************
 * METHODES OUTILS *
 *******************/
/**
 * Recupere la taille maximale des etats
 **/
int Automate::recupTailleEtatMax() const {
    int tailleMax = 1; // Conteneur de la taille maximale

    // Parcours des etats
    for(vector<Etat*>::const_iterator it = m_Q.begin(); it != m_Q.end(); ++it) {
        // Verification du depassement de la taille du nom
        int tailleNom = (*it)->recupTailleNom(); // Taille du nom
        if(tailleNom > tailleMax) tailleMax = tailleNom;
    }

    return tailleMax;
}

/**
 * Recupere la taille maximale des etats de transition via le symbole concerne
 **/
int Automate::recupTailleTransitionMax(char symb) const {
    int tailleMax = 1; // Conteneur de la taille maximale

    // Parcours des etats
    for(vector<Etat*>::const_iterator it = m_Q.begin(); it != m_Q.end(); ++it) {
        // Verification du depassement de la taille du nom
        int tailleTransition = (*it)->recupTailleTransition(symb); // Taille du nom de la transition
        if(tailleTransition > tailleMax) tailleMax = tailleTransition;
    }

    return tailleMax;
}

/**
 * Affiche la trable de transition de la partition
 **/
void Automate::afficheTableTransitionPartition(const vector<vector<Etat*> > &partitions, int decoupage, ostream &os) const {
    // Initialisation des variables
    int tailleEtat  = recupTailleEtatMax();                                // Taille maximale des etats
    int tailleMarge = MPI_Tools::recupNombreTaille(partitions.size() - 1); /// On recupere la taille de la partition - 1 = Numero de la derniere partie

    /// AFFICHAGE LIGNE TITRE
    // Affichage du decoupage
    os << endl << "-- DECOUPAGE NO" << decoupage << " --" << endl << endl;

    // Affichage du titre des Parties
    os << "P";
    MPI_Tools::printSpace(tailleMarge - 1, os);

    // Marge des noms d'etats
    os << " | ";
    MPI_Tools::printSpace(tailleEtat, os);

    // Parcours de l'alphabet
    for(int i = 0; i < m_tailleA; i++) {
        char symb = (char)('a' + i); // Symbole courant

        // Affichage du symbole
        os << " | " << symb;
        MPI_Tools::printSpace(tailleMarge - 1, os);
    }

    os << endl;

    /// AFFICHAGE LIGNE D'ETAT
    // Parcours de la partition
    for(vector<vector<Etat*> >::const_iterator it = partitions.begin(); it != partitions.end(); ++it) {
        // Parcours de la partie
        for(vector<Etat*>::const_iterator itB = (*it).begin(); itB != (*it).end(); ++itB) {
            // Recuperation de la partie courante
            int partieID = it - partitions.begin();

            // Affichage de la partie
            os << partieID;
            MPI_Tools::printSpace(tailleMarge - MPI_Tools::recupNombreTaille(partieID), os);

            // Affichage de l'etat
            os << " | " << **itB;
            MPI_Tools::printSpace(tailleEtat - (*itB)->recupTailleNom(), os);

            // Parcours de l'alphabet
            for(int i = 0; i < m_tailleA; i++) {
                char symb = (char)('a' + i); // Symbole courant

                os << " | ";

                // Recuperation de la transition sortante, car l'automate est deterministe
                Etat* etatCible = (*itB)->recupEtatsCibles(symb)[0]; /// Premier etat cible, l'automate est egalement complet

                // Recuperation de la partie cible
                int partieCible = MPI_Tools::recupPartie(partitions, etatCible);

                // Affichage de la transition du symbole en terme de partie
                os << partieCible;
                MPI_Tools::printSpace(tailleMarge - MPI_Tools::recupNombreTaille(partieCible), os);
            }

            os << endl;
        }
    }
}

/**
 * Rend l'automate accessible
 **/
void Automate::accessibilisation() {
    MPI_Tools::printTitle("RENDU DE L'AUTOMATE ACCESSIBLE");

    /*****************
     * ALGO DU COURS *
     *  MOMIRandum   *
     * vvvvvvvvvvvvv *
     *****************/
    // Initialisation des variables
    vector<Etat*> new_Q;    // Nouveau conteneur des etats accessibles
    vector<Etat*> new_T;    // Nouveau conteneur des etats terminaux accessibles
    vector<Etat*> aTraiter; // Conteneur des etats "a_traiter"

    /// Ajout des etats initiaux
    new_Q.insert(new_Q.end(), m_I.begin(), m_I.end());

    /// Marquage "a_traiter"
    aTraiter.insert(aTraiter.end(), m_I.begin(), m_I.end());

    /// Parcours des etats "a_traiter"
    while(aTraiter.size()) {
        /// Selection d'un etat
        Etat *etatP = aTraiter[0];

        /// Recuperation des etats accessibles depuis l'etat courant
        // Recuperation des transitions sortantes
        vector<Transition*> transDepart = etatP->recupTransitionDepart();

        // Parcours des transitions
        for(vector<Transition*>::const_iterator it = transDepart.begin(); it != transDepart.end(); ++it) {
            // Recuperation de l'etat cible
            Etat *etatCible = (*it)->recupEtatArrive();

            // Verification de la non appartenance a l'automate de l'etat
            if(find(new_Q.begin(), new_Q.end(), etatCible) == new_Q.end()) {
                // Ajout de l'etat
                new_Q.push_back(etatCible);
                aTraiter.push_back(etatCible);
            }
        }

        /// Marquage "traite"
        aTraiter.erase(aTraiter.begin());
    }

    /// Attribution des nouveaux etats
    // Parcours des etats
    for(vector<Etat*>::iterator it = m_Q.begin(); it != m_Q.end(); ++it) {
        // Verification de l'inaccessibilite de l'etat
        if(find(new_Q.begin(), new_Q.end(), *it) == new_Q.end()) delete *it;
    }
    m_Q = new_Q;

    /// Attributions des nouveaux etats terminaux
    m_T = new_T;

    /// Evidage des transitions
    evidageTransition();
    /*****************
     * ^^^^^^^^^^^^^ *
     * ALGO DU COURS *
     *  MOMIRandum   *
     *****************/

    MPI_Tools::printTitle("=> AUTOMATE ACCESIBLE");
}

/**
 * Recupere la fermeture epsilon de l'etat
 **/
vector<Etat*> Automate::fermetureEpsilon(Etat* etat) {
    // Initialisation des variables
    vector<Etat*> fermetureEps; // Conteneur de la fermeture epsilon
    vector<Etat*> aTraiter;         // Conteneur des etats a traiter

    /*****************
     * ALGO DU COURS *
     * vvvvvvvvvvvvv *
     *****************/
    /// Initialisation de la fermeture epsilon
    fermetureEps.push_back(etat);

    /// Marquage "a_traiter"
    aTraiter.push_back(etat);

    /// Parcours des etats "a_traiter"
    while(aTraiter.size()) {
        /// Selection d'un etat
        Etat *etatP = aTraiter[0];

        /// Recuperation des etats cibles via une transition epsilon
        vector<Etat*> etatsCibles = etatP->recupEtatsCibles(EPSILON);

        /// Marquage "a_traiter"
        // Parcours des etats cibles
        for(vector<Etat*>::iterator it = etatsCibles.begin(); it != etatsCibles.end(); ++it) {
            // Verification de la non presence de l'etat dans la fermeture
            if(find(fermetureEps.begin(), fermetureEps.end(), *it) == fermetureEps.end()) {
                // Ajout de l'etat a la fermeture et marquage "a_traiter"
                fermetureEps.push_back(*it);
                aTraiter.push_back(*it);
            }
        }

        /// Marquage "traite"
        aTraiter.erase(aTraiter.begin());
    }
    /*****************
     * ^^^^^^^^^^^^^ *
     * ALGO DU COURS *
     *****************/

    return fermetureEps;
}

/**
 * Supprime les etats doubles
 **/
void Automate::supprimerEtatDoubles() {
    // Parcours des etats A
    for(vector<Etat*>::iterator itA = m_Q.begin(); itA != m_Q.end(); ++itA) {
        // Parcours des etats B
        for(vector<Etat*>::iterator itB = itA + 1; itB != m_Q.end(); ++itB) {
            // Verification de la correspondance des etats
            if(**itA == **itB) {
                // Suppression de l'etat B
                m_Q.erase(itB);

                itB--; /// On a supprime => On reviens sur nos pas pour tester le nouveau courant
            }
        }
    }
}

/**
 * Supprime les transitions doubles
 **/
void Automate::supprimerTransitionsDoubles() {
    // Parcours des transitions A
    for(vector<Transition*>::iterator itA = m_E.begin(); itA != m_E.end(); ++itA) {
        // Parcours des transitions B
        for(vector<Transition*>::iterator itB = itA + 1; itB != m_E.end(); ++itB) {
            // Verification de la correspondance des transitions
            if(**itA == **itB) {
                // Suppression de la transition B
                m_E.erase(itB);

                itB--; /// On a supprime => On reviens sur nos pas pour tester le nouveau courant
            }
        }
    }
}

/**
 * Supprime les transitions inutiles
 **/
void Automate::evidageTransition() {
    // Parcours des transitions
    for(vector<Transition*>::iterator it = m_E.begin(); it != m_E.end(); ++it) {
        // Recuperation des etats de depart et d'arrive
        Etat *etatD = (*it)->recupEtatDepart();
        Etat *etatA = (*it)->recupEtatArrive();

        // Verification de l'inexistance de l'etat
        if(find(m_Q.begin(), m_Q.end(), etatD) == m_Q.end() || find(m_Q.begin(), m_Q.end(), etatA) == m_Q.end()) {
            // Liberation de la transition
            delete *it;

            // Suppression de la transition B
            m_E.erase(it);

            it--; /// On a supprime => On reviens sur nos pas pour tester le nouveau courant
        }
    }
}
