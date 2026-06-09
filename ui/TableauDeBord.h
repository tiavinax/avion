#ifndef TABLEAUDEBORD_H
#define TABLEAUDEBORD_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include "modele/Simulation.h"

// Affiche en temps réel les paramètres de vol
class TableauDeBord : public QWidget
{
    Q_OBJECT

public:
    explicit TableauDeBord(Simulation* sim, QWidget* parent = nullptr);

public slots:
    void rafraichir();

private:
    Simulation* m_sim;

    // ── Labels existants ──────────────────────────────────────────
    QLabel* m_lblAltitude;
    QLabel* m_lblDistancePiste;
    QLabel* m_lblVitesseX;
    QLabel* m_lblVitesseY;
    QLabel* m_lblVitesseTotale;
    QLabel* m_lblChronometre;
    QLabel* m_lblFreinageX;
    QLabel* m_lblFreinageY;
    QLabel* m_lblEtat;
    
    // ── NOUVEAUX : Carburant ──────────────────────────────────────
    QLabel*       m_lblCarburantValeur;   // texte : "xxx L / yyy L"
    QProgressBar* m_barreCarburant;       // barre de progression
    QLabel*       m_lblConsommation;      // consommation formatée

    void construireUI();
    QString formaterTemps(qint64 ms) const;
};

#endif // TABLEAUDEBORD_H