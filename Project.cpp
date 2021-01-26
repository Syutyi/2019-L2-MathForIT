#include "Project.h"

using namespace std;

/**
 * DESTRUCTEUR
 **/
Project::~Project() {
    // Parcours des automates
    for(vector<Automate*>::iterator it = m_automates.begin(); it != m_automates.end(); ++it) {
        // Liberation de l'automate
        delete *it;
    }
}

/**
 * Methode principale
 **/
void Project::execute() {
    // Affichage du titre general
    cout << "=============================" << endl;
    cout << "| MATHS POUR L'INFORMATIQUE |" << endl;
    cout << "|          PROJECT          |" << endl;
    cout << "=============================" << endl;

    // Bouclage principale
    while(int choix = affichageMenu()) {
        // Verification du choix
        switch(choix) {
            case 1: { /// Traitement standard [PROJECT STEP]
                /// Lecture de l'automate + Mise en memoire
                // Creation de l'automate
                Automate *aut = creationAutomate();

                // Bouclage jusqu'a obtenir un automate convenable
                while(aut->recupNom() == "ERREUR") aut = creationAutomate();

                /// Affichage de l'automate
                cout << *aut;

                /// Obtention de l'automate deterministe complet
                aut->determiniserEtCompleter();

                /// Calcul de l'automate minimal equivalent
                aut->minimiser();

                /// Test de reconnaissance de mot
                testReconnaissanceMot(aut);

                /// Creation d'un automate complementaire
                Automate *autComp = aut->automateComplementaire();

                /// Affichage du langage complementaire
                MPI_Tools::printTitle("AUTOMATE COMPLEMENTAIRE :");
                cout << *autComp;

                /// Test de reconnaissance de mot
                testReconnaissanceMot(autComp);

                /// Standardisation du complementaire
                MPI_Tools::printTitle("TRAITEMENT AUTOMATE COMPLEMENTAIRE :");
                autComp->standardiser();

                /// Test de reconnaissance de mot
                testReconnaissanceMot(autComp);
                break;
            }
            case 2: { /// Ajouter un automate
                // Recuperation de l'automate
                Automate *aut = creationAutomate();

                // Verification de l'existance de l'automate
                if(aut->recupNom() != "ERREUR") m_automates.push_back(aut);
                else MPI_Tools::printTitle("AUTOMATE NON CREE");
                break;
            }
            case 3: { /// Traiter un automate
                // Bouclage de choix de l'automate
                while(Automate *aut = affichageMenuAutomate()) {
                    // Bouclage de choix de l'action
                    bool fin   = false;
                    int action = 0;
                    while(!fin && (action = affichageMenuAction())) {
                        // Verification de l'action
                        switch(action) {
                            case 1: { /// Afficher
                                cout << *aut;
                                break;
                            }
                            case 2: { /// Rendre synchrone
                                aut->synchronisation();
                                break;
                            }
                            case 3: { /// Determiniser et Completer
                                aut->determiniserEtCompleter();
                                break;
                            }
                            case 4: { /// Determiniser
                                aut->determinisation();
                                break;
                            }
                            case 5: { /// Completer
                                aut->completion();
                                break;
                            }
                            case 6: { /// Minimiser
                                aut->minimisation();
                                break;
                            }
                            case 7: { /// Reconnaissance de mots
                                testReconnaissanceMot(aut);
                                break;
                            }
                            case 8: { /// Obtenir le complementaire
                                // Recuperation de l'automate complementaire
                                Automate *autComp = aut->automateComplementaire();

                                // Verification du bon deroulement de la procedure
                                if(autComp->recupNom() == "ERREUR") {
                                    // Affichage du message informatif
                                    MPI_Tools::printTitle("=> L'automate complementaire n'a pas pu etre cree");
                                } else {
                                    // Stockage de l'automate
                                    m_automates.push_back(autComp);

                                    // Affichage du message informatif
                                    MPI_Tools::printTitle("=> L'automate vient d'etre ajoute dans votre liste d'automate");
                                }
                                break;
                            }
                            case 9: { /// Standardiser
                                aut->standardisation();
                                break;
                            }
                            case 10: { /// Supprimer
                                // Liberation de l'automate
                                delete aut;

                                // Recuperation de l'automate
                                vector<Automate*>::iterator it = find(m_automates.begin(), m_automates.end(), aut);

                                // Suppression de l'automate
                                m_automates.erase(it);

                                // Affichage du message informatif
                                MPI_Tools::printTitle("=> AUTOMATE SUPPRIME");

                                // Forcage de sortie
                                fin = true;
                                break;
                            }
                            default:
                                cout << "Faites un choix correct !" << endl;
                            break;
                        }
                    }
                }
                break;
            }
            default:
                cout << "Faites un choix correct !" << endl;
            break;
        }
    }

    // Affichage du message final
    cout << endl;
    cout << "=============================" << endl;
    cout << "| MATHS POUR L'INFORMATIQUE |" << endl;
    cout << "|            FIN            |" << endl;
    cout << "=============================" << endl;
}

/**
 * Menu principal
 **/
int Project::affichageMenu() {
    // Affichage du menu
    cout << endl << "============ ---- MENU ---- ============" << endl;
    cout << "0/ QUITER" << endl;
    cout << "1/ Traitement standard [PROJECT STEP]" << endl;
    cout << "2/ Ajouter un automate" << endl;
    cout << "3/ Traiter un automate" << endl;
    cout << "Faite votre choix : ";

    // Recuperation du choix
    int choix = 0;
    cin >> choix;
    cin.ignore();

    // Affichage du menu
    cout << "======= ---- VOTRE CHOIX : " << choix << " ---- ======" << endl << endl;

    return choix;
}

/**
 * Menu de choix de l'automate
 **/
Automate* Project::affichageMenuAutomate() {
    /// Affichage du menu
    cout << endl << "============ ---- MENU ---- ============" << endl;
    cout << "========== ---- AUTOMATE ---- ==========" << endl;
    cout << "0/ RETOURNER EN ARRIERE" << endl;

    // Verification d'aucun automate
    if(m_automates.size() == 0) cout << "== Veuillez creer au moins un automate..." << endl;

    // Parcours des automates
    for(vector<Automate*>::const_iterator it = m_automates.begin(); it != m_automates.end(); ++it) {
        // Affichage de l'automate
        cout << it - m_automates.begin() + 1 << "/ " << (*it)->recupNom() << endl;
    }

    cout << "Faite votre choix : ";

    /// Recuperation de l'id de l'automate
    int autID = 0;
    cin >> autID;
    cin.ignore();

    // Verification d'un id correct
    autID = (autID < 0 || autID > (int)m_automates.size())? 0 : autID;

    // Affichage du menu
    cout << "==== ---- VOTRE AUTOMATE : " << autID << " ---- ====" << endl << endl;

    /// Recuperation de l'automate
    Automate *aut = (autID == 0)? NULL : m_automates[autID - 1];

    return aut;
}

/**
 * Menu de traitement
 **/
int Project::affichageMenuAction() {
    // Affichage du menu
    cout << endl << "============ ---- MENU ---- ============" << endl;
    cout << "========= ---- TRAITEMENT ---- =========" << endl;
    cout << "0/ RETOURNER EN ARRIERE" << endl;
    cout << "1/ Afficher" << endl;
    cout << "2/ Rendre synchrone" << endl;
    cout << "3/ Determiniser et Completer" << endl;
    cout << "4/ Determiniser" << endl;
    cout << "5/ Completer" << endl;
    cout << "6/ Minimiser" << endl;
    cout << "7/ Reconnaissance de mots" << endl;
    cout << "8/ Obtenir le complementaire" << endl;
    cout << "9/ Standardiser" << endl;
    cout << "10/ Supprimer" << endl;
    cout << "Faite votre choix : ";

    // Recuperation de l'action
    int action = 0;
    cin >> action;
    cin.ignore();

    // Affichage du menu
    cout << "====== ---- VOTRE ACTION : " << action << " ---- ======" << endl << endl;

    return action;
}

/**
 * Creation de l'automate
 **/
Automate* Project::creationAutomate() {
    // Recuperation du nom de l'automate
    cout << "Quel automate voulez-vous utiliser ? ";
    string choix = "Default";
    cin >> choix;
    cin.ignore();

    // Verification qu'il s'agit d'un chiffre unique
    if(choix.size() == 1 && choix[0] > '0'  && choix[0] < '9' ) choix = "0" + choix;

    // Creation de l'automate
    Automate *aut = new Automate(choix);

    cout << endl;

    return aut;
}

/**
 * Test de reconnaissance de mots
 **/
void Project::testReconnaissanceMot(Automate* aut) {
    MPI_Tools::printTitle("RECONNAISSANCE DE MOT SUR " + aut->recupNom());

    // Recuperation du mot
    cout << FIN << " -> FIN DE LA RECONNAISSANCE DE MOT" << endl;
    string s = MPI_Tools::lisMot();

    // Bouclage de la reconnaissance de mot
    while(s != (string)FIN) {
        // Verification de la reconnaissance du mot
        if(aut->testMot(s)) MPI_Tools::printTitle("=> [V] Le mot est reconnu par l'automate");
        else MPI_Tools::printTitle("=> [X] Le mot n'est pas reconnu par l'automate");

        // Recuperation du mot
        cout << FIN << " => FIN DE LA RECONNAISSANCE DE MOT" << endl;
        s = MPI_Tools::lisMot();
    }

    MPI_Tools::printTitle("=> FIN DE RECONNAISSANCE DE MOT");
}
