#ifndef TABLEAUDEBORD_H
#define TABLEAUDEBORD_H

#include <QWidget>
#include <QLabel>
#include "modele/Simulation.h"

// Affiche en temps réel les paramètres de vol
// Mis à jour à chaque tick via le slot rafraichir()
class TableauDeBord : public QWidget
{
    Q_OBJECT

public:
    explicit TableauDeBord(Simulation* sim, QWidget* parent = nullptr);

public slots:
    // Appelé à chaque tick — relit les valeurs de l'avion et met à jour les labels
    void rafraichir();

private:
    Simulation* m_sim;

    // ── Labels de valeurs ─────────────────────────────────────────
    QLabel* m_lblAltitude;
    QLabel* m_lblDistancePiste;
    QLabel* m_lblVitesseX;
    QLabel* m_lblVitesseY;
    QLabel* m_lblVitesseTotale;
    QLabel* m_lblChronometre;
    QLabel* m_lblFreinageX;   // budget GammaX consommé
    QLabel* m_lblFreinageY;   // budget GammaY consommé
    QLabel* m_lblEtat;        // état courant de la simulation

    void construireUI();

    // Formate un temps en ms → "HH:MM:SS"
    QString formaterTemps(qint64 ms) const;
};

#endif // TABLEAUDEBORD_H