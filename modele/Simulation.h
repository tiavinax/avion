#ifndef SIMULATION_H
#define SIMULATION_H

#include "Avion.h"
#include "Piste.h"
#include "ParametresFreinage.h"
#include <QObject>
#include <QTimer>
#include <QElapsedTimer>

enum class EtatSimulation {
    EN_ATTENTE,
    EN_COURS,
    EN_PAUSE,
    EN_CHUTE,      // ← NOUVEAU
    REUSSI,
    DETRUIT
};

enum class CauseDestruction {
    AUCUNE,
    DECROCHAGE,
    AVANT_PISTE,
    APRES_PISTE,
    DEPASSEMENT_PISTE,
    PANNE_SECHE      // ← NOUVEAU
};


class Simulation : public QObject
{
    Q_OBJECT

public:
    Simulation(Avion* avion, Piste* piste, ParametresFreinage* freinage,
               QObject* parent = nullptr);

    // ── Contrôle ──────────────────────────────────────────────────
    void demarrer();
    void pause();
    void reprendre();

    // Remet tout à l'état initial (version avec carburant)
    void restart(const QString& nom,
                 double vx, double vy, double acc, double vd,
                 double alt, double dist, double gx, double gy,
                 double capaciteCarburant_m3, double consommation_m3_s,
                 UniteVolume uniteVolume, UniteDebit uniteDebit);

    // ── Modes ─────────────────────────────────────────────────────
    void setModeFreinage(ModeFreinage mode)       { m_modeFreinage   = mode; }
    void setModeDecrochage(ModeDecrochage mode)   { m_modeDecrochage = mode; }

    ModeFreinage   getModeFreinage()   const { return m_modeFreinage; }
    ModeDecrochage getModeDecrochage() const { return m_modeDecrochage; }

    // ── Getters ───────────────────────────────────────────────────
    EtatSimulation   getEtat()             const { return m_etat; }
    CauseDestruction getCauseDestruction() const { return m_cause; }
    qint64           getTempsEcoule_ms()   const;
    Avion*           getAvion()            const { return m_avion; }

signals:
    void simulationMiseAJour();
    void simulationTerminee(EtatSimulation etat, CauseDestruction cause);

private slots:
    void tick();

private:
    Avion*              m_avion;
    Piste*              m_piste;
    ParametresFreinage* m_freinage;

    EtatSimulation   m_etat;
    CauseDestruction m_cause;

    // ── Modes actifs ──────────────────────────────────────────────
    ModeFreinage   m_modeFreinage;
    ModeDecrochage m_modeDecrochage;

    QTimer*       m_timer;
    QElapsedTimer m_chrono;
    qint64        m_tempsPause_ms;

    void evaluerConditions();
    void terminer(EtatSimulation etat, CauseDestruction cause);
    CauseDestruction m_causeStockee;
};

#endif // SIMULATION_H