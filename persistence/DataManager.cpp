#include "DataManager.h"
#include <QFile>
#include <QTextStream>
#include <QMap>

const QString DataManager::FICHIER = "data.txt";

bool DataManager::sauvegarder(const Avion& avion, const ParametresFreinage& freinage)
{
    QFile fichier(FICHIER);

    if (!fichier.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&fichier);

    // Paramètres existants
    out << "nom="                << avion.getNom()                   << "\n";
    out << "vitesse_x="          << avion.getVitesseX_kmh()          << "\n";
    out << "vitesse_y="          << avion.getVitesseY_kmh()          << "\n";
    out << "acceleration="       << avion.getAcceleration_ms2()      << "\n";
    out << "vitesse_decrochage=" << avion.getVitesseDecrochage_kmh() << "\n";
    out << "altitude="           << avion.getAltitude()              << "\n";
    out << "distance_piste="     << avion.getDistancePiste()         << "\n";
    out << "gamma_x="            << freinage.getGammaX()             << "\n";
    out << "gamma_y="            << freinage.getGammaY()             << "\n";
    
    // NOUVEAUX paramètres carburant (stockés en unités SI)
    out << "capacite_carburant_m3=" << avion.getCapaciteEnM3()       << "\n";
    out << "consommation_m3_s="     << avion.getConsommation_m3_s()  << "\n";
    out << "unite_volume_preferee=" << static_cast<int>(avion.getUniteVolumePreferee()) << "\n";
    out << "unite_debit_preferee="  << static_cast<int>(avion.getUniteDebitPreferee())  << "\n";

    fichier.close();
    return true;
}

bool DataManager::charger(QString& nom,
                           double& vitesseX, double& vitesseY,
                           double& acceleration, double& vitesseDecrochage,
                           double& altitude, double& distancePiste,
                           double& gammaX, double& gammaY,
                           double& capaciteCarburant_m3, double& consommation_m3_s,
                           int& uniteVolumePreferee, int& uniteDebitPreferee)
{
    QFile fichier(FICHIER);

    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&fichier);
    QMap<QString, QString> donnees;

    while (!in.atEnd()) {
        QString ligne = in.readLine().trimmed();
        if (ligne.isEmpty() || !ligne.contains('=')) continue;

        QString cle    = ligne.section('=', 0, 0).trimmed();
        QString valeur = ligne.section('=', 1).trimmed();
        donnees[cle] = valeur;
    }

    fichier.close();

    // Vérifie les clés obligatoires (anciennes + nouvelles)
    QStringList clesRequises = {"nom", "vitesse_x", "vitesse_y", "acceleration",
                                "vitesse_decrochage", "altitude", "distance_piste",
                                "gamma_x", "gamma_y",
                                "capacite_carburant_m3", "consommation_m3_s",
                                "unite_volume_preferee", "unite_debit_preferee"};
    
    for (const QString& cle : clesRequises) {
        if (!donnees.contains(cle)) return false;
    }

    // Affecte les valeurs lues
    nom               = donnees["nom"];
    vitesseX          = donnees["vitesse_x"].toDouble();
    vitesseY          = donnees["vitesse_y"].toDouble();
    acceleration      = donnees["acceleration"].toDouble();
    vitesseDecrochage = donnees["vitesse_decrochage"].toDouble();
    altitude          = donnees["altitude"].toDouble();
    distancePiste     = donnees["distance_piste"].toDouble();
    gammaX            = donnees["gamma_x"].toDouble();
    gammaY            = donnees["gamma_y"].toDouble();
    
    // NOUVEAUX : carburant
    capaciteCarburant_m3 = donnees["capacite_carburant_m3"].toDouble();
    consommation_m3_s    = donnees["consommation_m3_s"].toDouble();
    uniteVolumePreferee  = donnees["unite_volume_preferee"].toInt();
    uniteDebitPreferee   = donnees["unite_debit_preferee"].toInt();

    return true;
}