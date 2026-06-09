#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include "modele/Avion.h"
#include "modele/ParametresFreinage.h"

// Gère la lecture et l'écriture du fichier data.txt
// Equivalent d'un DAO (Data Access Object) en Java
class DataManager
{
public:
    // Chemin du fichier de persistance
    static const QString FICHIER;

    // Sauvegarde les paramètres dans data.txt
    // Retourne true si réussi, false si erreur d'écriture
    static bool sauvegarder(const Avion& avion, const ParametresFreinage& freinage);

    // Charge les paramètres depuis data.txt
    // Retourne true si réussi, false si fichier introuvable ou corrompu
    static bool charger(QString& nom,
                        double& vitesseX, double& vitesseY,
                        double& acceleration, double& vitesseDecrochage,
                        double& altitude, double& distancePiste,
                        double& gammaX, double& gammaY);
};

#endif // DATAMANAGER_H