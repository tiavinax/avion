#ifndef SIMULATIONWINDOW_H
#define SIMULATIONWINDOW_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "modele/Avion.h"
#include "modele/Piste.h"
#include "modele/ParametresFreinage.h"
#include "modele/Simulation.h"
#include "ui/ZoneSimulation.h"
#include "ui/TableauDeBord.h"

class SimulationWindow : public QWidget
{
    Q_OBJECT
public:
    SimulationWindow(Avion* avion, ParametresFreinage* freinage,
                     QWidget* parent = nullptr);

private slots:
    void onChangerVue(int index);
    void onStart();
    void onPauseReprendre();
    void onRestart();
    void onVxPlus();
    void onVxMoins();
    void onVyPlus();
    void onVyMoins();
    void onToggleModeFreinage();
    void onToggleModeDecrochage();
    void onSimulationTerminee(EtatSimulation etat, CauseDestruction cause);

private:
    Simulation*     m_sim;
    ZoneSimulation* m_zone;
    TableauDeBord*  m_tableau;

    // ── Paramètres initiaux sauvegardés pour RESTART ──────────────
    QString m_nomInitial;
    double  m_vxInitial, m_vyInitial, m_accInitial;
    double  m_vdInitial, m_altInitial, m_distInitial;
    double  m_gxInitial, m_gyInitial;

    // ── Boutons principaux ────────────────────────────────────────
    QComboBox*   m_comboVue;
    QPushButton* m_btnStart;
    QPushButton* m_btnPause;
    QPushButton* m_btnRestart;

    // ── Boutons vitesse ───────────────────────────────────────────
    QPushButton* m_btnVxPlus;
    QPushButton* m_btnVxMoins;
    QPushButton* m_btnVyPlus;
    QPushButton* m_btnVyMoins;

    // ── Boutons bascule modes ─────────────────────────────────────
    QPushButton* m_btnModeFreinage;    // Flexible / Absolu
    QPushButton* m_btnModeDecrochage;  // Vx / Norme

    QLabel* m_lblMessage;  // message final réussi/détruit

    void construireUI();
    void resetStyleBoutons();  // remet les boutons vitesse au style normal
    void activerControles(bool actif);
};

#endif // SIMULATIONWINDOW_H