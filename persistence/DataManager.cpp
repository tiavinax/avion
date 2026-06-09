#include "DataManager.h"
#include <QFile>        // gère l'accès aux fichiers
#include <QTextStream>  // lit/écrit du texte ligne par ligne
#include <QMap>         // stocke les paires clé=valeur lues

// Nom du fichier — défini une seule fois ici
const QString DataManager::FICHIER = "data.txt";

bool DataManager::sauvegarder(const Avion& avion, const ParametresFreinage& freinage) {
    QFile fichier(FICHIER);

    // Ouvre en écriture — crée le fichier s'il n'existe pas
    if (!fichier.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&fichier);

    // Écrit chaque paramètre sur une ligne au format clé=valeur
    out << "nom="                << avion.getNom()                   << "\n";
    out << "vitesse_x="          << avion.getVitesseX_kmh()          << "\n";
    out << "vitesse_y="          << avion.getVitesseY_kmh()          << "\n";
    out << "acceleration="       << avion.getAcceleration_ms2()      << "\n";
    out << "vitesse_decrochage=" << avion.getVitesseDecrochage_kmh() << "\n";
    out << "altitude="           << avion.getAltitude()              << "\n";
    out << "distance_piste="     << avion.getDistancePiste()         << "\n";
    out << "gamma_x="            << freinage.getGammaX()             << "\n";
    out << "gamma_y="            << freinage.getGammaY()             << "\n";

    fichier.close();
    return true;
}

bool DataManager::charger(QString& nom,
                           double& vitesseX, double& vitesseY,
                           double& acceleration, double& vitesseDecrochage,
                           double& altitude, double& distancePiste,
                           double& gammaX, double& gammaY)
{
    QFile fichier(FICHIER);

    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&fichier);
    QMap<QString, QString> donnees;  // stocke toutes les paires clé=valeur

    // Lit chaque ligne et découpe sur le '='
    while (!in.atEnd()) {
        QString ligne = in.readLine().trimmed();
        if (ligne.isEmpty() || !ligne.contains('=')) continue;

        QString cle    = ligne.section('=', 0, 0).trimmed(); // avant le '='
        QString valeur = ligne.section('=', 1).trimmed();    // après le '='
        donnees[cle] = valeur;
    }

    fichier.close();

    // Vérifie que toutes les clés attendues sont présentes
    QStringList clesRequises = {"nom","vitesse_x","vitesse_y","acceleration",
                                "vitesse_decrochage","altitude",
                                "distance_piste","gamma_x","gamma_y"};
    for (const QString& cle : clesRequises) {
        if (!donnees.contains(cle)) return false;
    }

    // Affecte les valeurs lues aux variables de sortie
    nom               = donnees["nom"];
    vitesseX          = donnees["vitesse_x"].toDouble();
    vitesseY          = donnees["vitesse_y"].toDouble();
    acceleration      = donnees["acceleration"].toDouble();
    vitesseDecrochage = donnees["vitesse_decrochage"].toDouble();
    altitude          = donnees["altitude"].toDouble();
    distancePiste     = donnees["distance_piste"].toDouble();
    gammaX            = donnees["gamma_x"].toDouble();
    gammaY            = donnees["gamma_y"].toDouble();

    return true;
}