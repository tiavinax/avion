#include "Simulation.h"
#include "persistence/DataManager.h"

static const int    TICK_MS = 100;
static const double DT      = TICK_MS / 1000.0;

Simulation::Simulation(Avion* avion, Piste* piste,
                       ParametresFreinage* freinage, QObject* parent)
    : QObject(parent)
    , m_avion(avion)
    , m_piste(piste)
    , m_freinage(freinage)
    , m_etat(EtatSimulation::EN_ATTENTE)
    , m_cause(CauseDestruction::AUCUNE)
    , m_modeFreinage(ModeFreinage::FLEXIBLE)      // défaut : flexible
    , m_modeDecrochage(ModeDecrochage::VX_SEULE)  // défaut : Vx seule
    , m_tempsPause_ms(0)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Simulation::tick);
}

void Simulation::demarrer()
{
    if (m_etat != EtatSimulation::EN_ATTENTE) return;
    m_etat = EtatSimulation::EN_COURS;
    m_chrono.start();
    m_timer->start(TICK_MS);
}

void Simulation::pause()
{
    if (m_etat != EtatSimulation::EN_COURS) return;
    m_etat = EtatSimulation::EN_PAUSE;
    m_timer->stop();
    m_tempsPause_ms += m_chrono.elapsed();
}

void Simulation::reprendre()
{
    if (m_etat != EtatSimulation::EN_PAUSE) return;
    m_etat = EtatSimulation::EN_COURS;
    m_chrono.restart();
    m_timer->start(TICK_MS);
}

void Simulation::restart(const QString& nom,
                          double vx, double vy, double acc, double vd,
                          double alt, double dist, double gx, double gy)
{
    // Stoppe le timer si en cours
    m_timer->stop();

    // Recrée l'avion avec les paramètres initiaux
    delete m_avion;
    m_avion = new Avion(nom, vx, vy, acc, vd, alt, dist, gx, gy);

    // Réinitialise l'état
    m_etat          = EtatSimulation::EN_ATTENTE;
    m_cause         = CauseDestruction::AUCUNE;
    m_tempsPause_ms = 0;

    // Émet un signal pour que l'UI se rafraîchisse
    emit simulationMiseAJour();
}

qint64 Simulation::getTempsEcoule_ms() const
{
    if (m_etat == EtatSimulation::EN_COURS)
        return m_tempsPause_ms + m_chrono.elapsed();
    return m_tempsPause_ms;
}

void Simulation::tick()
{
    m_avion->mettreAJour(DT);
    evaluerConditions();
    emit simulationMiseAJour();
}

void Simulation::evaluerConditions()
{
    double altitude = m_avion->getAltitude();
    double distance = m_avion->getDistancePiste();

    // ── Cas 1 : Décrochage (en vol uniquement) ────────────────────
    if (m_avion->estEnVol() && m_avion->estEnDecrochage(m_modeDecrochage)) {
        terminer(EtatSimulation::DETRUIT, CauseDestruction::DECROCHAGE);
        return;
    }

    // ── Cas 2 : Avion touche le sol ───────────────────────────────
    if (altitude <= 0.0) {
        if (m_piste->contientPosition(distance))
            terminer(EtatSimulation::REUSSI, CauseDestruction::AUCUNE);
        else if (distance < 0.0)
            terminer(EtatSimulation::DETRUIT, CauseDestruction::AVANT_PISTE);
        else
            terminer(EtatSimulation::DETRUIT, CauseDestruction::APRES_PISTE);
        return;
    }

    // ── Cas 3 : Dépassement de piste ──────────────────────────────
    if (distance >= m_piste->getLongueur()) {
        terminer(EtatSimulation::DETRUIT, CauseDestruction::DEPASSEMENT_PISTE);
    }
}

void Simulation::terminer(EtatSimulation etat, CauseDestruction cause)
{
    m_etat  = etat;
    m_cause = cause;
    m_timer->stop();
    emit simulationTerminee(etat, cause);
}