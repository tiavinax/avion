#include "Avion.h"
#include <cmath>

Avion::Avion(const QString& nom,
             double vitesseX_kmh,
             double vitesseY_kmh,
             double acceleration_ms2,
             double vitesseDecrochage_kmh,
             double altitude,
             double distancePiste,
             double gammaX_kmh,
             double gammaY_kmh)
    : m_nom(nom)
    , m_vx_ms(kmhToMs(vitesseX_kmh))
    , m_vy_ms(kmhToMs(vitesseY_kmh))
    , m_acceleration_ms2(acceleration_ms2)   // 10 m/s² stocké directement
    , m_vDecrochage_ms(kmhToMs(vitesseDecrochage_kmh))
    , m_altitude(altitude)
    , m_distancePiste(distancePiste)       
    , m_gammaX_ms(gammaX_kmh)             
    , m_gammaY_ms(gammaY_kmh)  
    , m_freinageConsommeX_ms(0.0)
    , m_freinageConsommeY_ms(0.0)
    , m_vxRef_ms(kmhToMs(vitesseX_kmh))  
    , m_vyRef_ms(kmhToMs(vitesseY_kmh))
{}

void Avion::mettreAJour(double dt)
{
    m_distancePiste += m_vx_ms * dt;
    m_altitude      += m_vy_ms * dt;
    if (m_altitude < 0.0) m_altitude = 0.0;
}

// ── Contrôle Vx ───────────────────────────────────────────────────────────────

void Avion::augmenterVx(ModeFreinage mode)
{
    m_vx_ms += m_acceleration_ms2;

    if (mode == ModeFreinage::FLEXIBLE) {
        // Flexible : si on dépasse la référence → recharge le budget
        if (m_vx_ms > m_vxRef_ms) {
            m_vxRef_ms = m_vx_ms;
            m_freinageConsommeX_ms = 0.0;
        } else {
            // On remonte sans dépasser → consommé se réduit
            m_freinageConsommeX_ms = m_vxRef_ms - m_vx_ms;
        }
    }
    // Mode ABSOLU : on ne touche pas au cumul — le budget ne se recharge jamais
}

bool Avion::diminuerVx(ModeFreinage mode)
{
    double palier = m_acceleration_ms2;  // 10 m/s

    if (mode == ModeFreinage::ABSOLU) {
        // Budget absolu : cumul total des diminutions ne dépasse pas GammaX
        if (m_freinageConsommeX_ms + palier > m_gammaX_ms)
            return false;
        m_vx_ms -= palier;
        m_freinageConsommeX_ms += palier;
    }
    else {
        // Budget flexible : Vx ne peut pas descendre sous (vxRef - GammaX)
        double vxMin = m_vxRef_ms - m_gammaX_ms;
        if (m_vx_ms - palier < vxMin)
            return false;
        m_vx_ms -= palier;
        m_freinageConsommeX_ms = m_vxRef_ms - m_vx_ms;  // recalcul du consommé
    }
    return true;
}

// ── Contrôle Vy ───────────────────────────────────────────────────────────────

void Avion::augmenterVy(ModeFreinage mode)
{
    m_vy_ms += m_acceleration_ms2;

    if (mode == ModeFreinage::FLEXIBLE) {
        if (m_vy_ms > m_vyRef_ms) {
            m_vyRef_ms = m_vy_ms;
            m_freinageConsommeY_ms = 0.0;
        } else {
            m_freinageConsommeY_ms = m_vyRef_ms - m_vy_ms;
        }
    }
}

bool Avion::diminuerVy(ModeFreinage mode)
{
    double palier = m_acceleration_ms2;

    if (mode == ModeFreinage::ABSOLU) {
        if (m_freinageConsommeY_ms + palier > m_gammaY_ms)
            return false;
        m_vy_ms -= palier;
        m_freinageConsommeY_ms += palier;
    }
    else {
        double vyMin = m_vyRef_ms - m_gammaY_ms;
        if (m_vy_ms - palier < vyMin)
            return false;
        m_vy_ms -= palier;
        m_freinageConsommeY_ms = m_vyRef_ms - m_vy_ms;
    }
    return true;
}

// ── Conversions ───────────────────────────────────────────────────────────────

double Avion::kmhToMs(double kmh) { return kmh / 3.6; }
double Avion::msToKmh(double ms)  { return ms * 3.6;  }

// ── Calculs physiques ─────────────────────────────────────────────────────────

double Avion::getVitesseTotale_ms() const
{
    return std::sqrt(m_vx_ms * m_vx_ms + m_vy_ms * m_vy_ms);
}

bool Avion::estEnVol() const
{
    return m_altitude > 0.0;
}

bool Avion::estEnDecrochage(ModeDecrochage mode) const
{
    if (mode == ModeDecrochage::VX_SEULE)
        // Vérifie uniquement la composante horizontale
        return m_vx_ms < m_vDecrochage_ms;
    else
        // Vérifie la norme du vecteur vitesse
        return getVitesseTotale_ms() < m_vDecrochage_ms;
}

// ── Setters ───────────────────────────────────────────────────────────────────

void Avion::setVitesseX_ms(double ms)  { m_vx_ms = ms; m_vxRef_ms = ms; }
void Avion::setVitesseY_ms(double ms)  { m_vy_ms = ms; m_vyRef_ms = ms; }
void Avion::setAltitude(double alt)    { m_altitude = alt; }
void Avion::setDistancePiste(double d) { m_distancePiste = d; }

void Avion::resetFreinage()
{
    // Remet les budgets à zéro — utilisé par RESTART
    m_freinageConsommeX_ms = 0.0;
    m_freinageConsommeY_ms = 0.0;
    m_vxRef_ms = m_vx_ms;
    m_vyRef_ms = m_vy_ms;
}