#ifndef FORMULAIREWINDOW_H
#define FORMULAIREWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include "modele/Avion.h"

class FormulaireWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FormulaireWindow(QWidget* parent = nullptr);

private slots:
    void onValider();

private:
    // ── Champs de saisie existants ────────────────────────────────
    QLineEdit* m_champNom;
    QLineEdit* m_champAltitude;
    QLineEdit* m_champVitesseX;
    QLineEdit* m_champVitesseY;
    QLineEdit* m_champDecrochage;
    QLineEdit* m_champAcceleration;
    QLineEdit* m_champGammaX;
    QLineEdit* m_champGammaY;
    QLineEdit* m_champDistancePiste;      // NOUVEAU : distance initiale

    // ── NOUVEAUX champs carburant ─────────────────────────────────
    QLineEdit*   m_champCapaciteCarburant;
    QComboBox*   m_comboUniteVolume;       // L ou m³
    QLineEdit*   m_champConsommation;
    QComboBox*   m_comboUniteDebit;        // L/s, L/min, L/h, m³/s, m³/min, m³/h

    QPushButton* m_boutonValider;
    QLabel*      m_labelErreur;

    // ── Méthodes internes ─────────────────────────────────────────
    void construireUI();
    bool validerChamps();
    
    // ── NOUVELLES conversions ─────────────────────────────────────
    double convertirVersM3(double valeur, int indexUnite);
    double convertirVersM3ParSeconde(double valeur, int indexUnite);
};

#endif // FORMULAIREWINDOW_H