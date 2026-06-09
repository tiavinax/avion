#include "Simulation.h"
#include "persistence/DataManager.h"

static const int TICK_MS = 100;
static const double DT = TICK_MS / 1000.0;

Simulation::Simulation(Avion *avion, Piste *piste, ParametresFreinage *freinage, QObject *parent)
    : QObject(parent), m_avion(avion), m_piste(piste), 
    m_freinage(freinage), m_etat(EtatSimulation::EN_ATTENTE), 
    m_cause(CauseDestruction::AUCUNE), m_modeFreinage(ModeFreinage::FLEXIBLE), 
    m_modeDecrochage(ModeDecrochage::VX_SEULE), 
    m_tempsPause_ms(0), 
    m_causeStockee(CauseDestruction::AUCUNE)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Simulation::tick);
}

void Simulation::demarrer()
{
    if (m_etat != EtatSimulation::EN_ATTENTE)
        return;
    m_etat = EtatSimulation::EN_COURS;
    m_chrono.start();
    m_timer->start(TICK_MS);
}

void Simulation::pause()
{
    if (m_etat != EtatSimulation::EN_COURS)
        return;
    m_etat = EtatSimulation::EN_PAUSE;
    m_timer->stop();
    m_tempsPause_ms += m_chrono.elapsed();
}

void Simulation::reprendre()
{
    if (m_etat != EtatSimulation::EN_PAUSE)
        return;
    m_etat = EtatSimulation::EN_COURS;
    m_chrono.restart();
    m_timer->start(TICK_MS);
}

void Simulation::restart(const QString &nom,
                         double vx, double vy, double acc, double vd,
                         double alt, double dist, double gx, double gy,
                         double capaciteCarburant_m3, double consommation_m3_s,
                         UniteVolume uniteVolume, UniteDebit uniteDebit)
{
    m_timer->stop();

    delete m_avion;
    m_avion = new Avion(nom, vx, vy, acc, vd, alt, dist, gx, gy,
                        capaciteCarburant_m3, consommation_m3_s,
                        uniteVolume, uniteDebit);

    m_etat = EtatSimulation::EN_ATTENTE;
    m_cause = CauseDestruction::AUCUNE;
    m_tempsPause_ms = 0;

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
    if (m_etat == EtatSimulation::EN_CHUTE)
    {
        // Uniquement mise à jour position (chute libre)
        m_avion->mettreAJour(DT);

        // Pas de consommation carburant ni autre logique
        // Les boutons sont déjà désactivés par l'UI
    }
    else
    {
        m_avion->mettreAJour(DT);
        m_avion->consommerCarburant(DT);
    }

    evaluerConditions();
    emit simulationMiseAJour();
}

void Simulation::evaluerConditions()
{
    double altitude = m_avion->getAltitude();
    double distance = m_avion->getDistancePiste();

    // ── Cas spécial : en chute ────────────────────────────────────
    if (m_etat == EtatSimulation::EN_CHUTE)
    {
        // On attend juste que l'avion touche le sol
        if (altitude <= 0.0)
        {
            terminer(EtatSimulation::DETRUIT, m_causeStockee); // cause stockée
        }
        return;
    }

    // ── Détection déclenchement chute (panne ou décrochage) ──────
    if (m_avion->estEnVol())
    {
        // Panne sèche
        if (m_avion->estEnPanneSeche())
        {
            m_causeStockee = CauseDestruction::PANNE_SECHE;
            m_avion->declencherChute();
            m_etat = EtatSimulation::EN_CHUTE;
            emit simulationMiseAJour(); // Rafraîchir UI immédiatement
            return;
        }

        // Décrochage
        if (m_avion->estEnDecrochage(m_modeDecrochage))
        {
            m_causeStockee = CauseDestruction::DECROCHAGE;
            m_avion->declencherChute();
            m_etat = EtatSimulation::EN_CHUTE;
            emit simulationMiseAJour();
            return;
        }
    }

    // ── Reste des conditions (atterrissage, avant/après piste, dépassement) ──
    if (altitude <= 0.0)
    {
        if (m_piste->contientPosition(distance))
            terminer(EtatSimulation::REUSSI, CauseDestruction::AUCUNE);
        else if (distance < 0.0)
            terminer(EtatSimulation::DETRUIT, CauseDestruction::AVANT_PISTE);
        else
            terminer(EtatSimulation::DETRUIT, CauseDestruction::APRES_PISTE);
        return;
    }

    if (distance >= m_piste->getLongueur())
    {
        terminer(EtatSimulation::DETRUIT, CauseDestruction::DEPASSEMENT_PISTE);
    }
}

void Simulation::terminer(EtatSimulation etat, CauseDestruction cause)
{
    m_etat = etat;
    m_cause = cause;
    m_timer->stop();
    emit simulationTerminee(etat, cause);
}