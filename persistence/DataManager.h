#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include "modele/Avion.h"
#include "modele/ParametresFreinage.h"

// Gère la lecture et l'écriture du fichier data.txt
class DataManager
{
public:
    static const QString FICHIER;

    // Sauvegarde les paramètres dans data.txt
    static bool sauvegarder(const Avion& avion, const ParametresFreinage& freinage);

    // Charge les paramètres depuis data.txt
    static bool charger(QString& nom,
                        double& vitesseX, double& vitesseY,
                        double& acceleration, double& vitesseDecrochage,
                        double& altitude, double& distancePiste,
                        double& gammaX, double& gammaY,
                        double& capaciteCarburant_m3, double& consommation_m3_s,
                        int& uniteVolumePreferee, int& uniteDebitPreferee);
};

#endif // DATAMANAGER_H