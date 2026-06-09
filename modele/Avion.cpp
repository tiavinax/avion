#include "Avion.h"
#include <cmath>

// ─────────────────────────────────────────────────────────────────────────────
// CONSTRUCTEUR
// ─────────────────────────────────────────────────────────────────────────────
Avion::Avion(const QString& nom,
             double vitesseX_kmh,
             double vitesseY_kmh,
             double acceleration_ms2,
             double vitesseDecrochage_kmh,
             double altitude,
             double distancePiste,
             double gammaX_ms,
             double gammaY_ms,
             double capaciteCarburant_m3,
             double consommation_m3_par_s,
             UniteVolume uniteVolumePreferee,
             UniteDebit uniteDebitPreferee)
    : m_nom(nom)
    , m_vx_ms(kmhToMs(vitesseX_kmh))
    , m_vy_ms(kmhToMs(vitesseY_kmh))
    , m_acceleration_ms2(acceleration_ms2)
    , m_vDecrochage_ms(kmhToMs(vitesseDecrochage_kmh))
    , m_altitude(altitude)
    , m_distancePiste(distancePiste)
    , m_gammaX_ms(gammaX_ms)
    , m_gammaY_ms(gammaY_ms)
    , m_freinageConsommeX_ms(0.0)
    , m_freinageConsommeY_ms(0.0)
    , m_vxRef_ms(kmhToMs(vitesseX_kmh))
    , m_vyRef_ms(kmhToMs(vitesseY_kmh))
    , m_carburant_m3(capaciteCarburant_m3)
    , m_capacite_m3(capaciteCarburant_m3)
    , m_consommation_m3_s(consommation_m3_par_s)
    , m_uniteVolumePreferee(uniteVolumePreferee)
    , m_uniteDebitPreferee(uniteDebitPreferee)
{}

// ─────────────────────────────────────────────────────────────────────────────
// MISE À JOUR PHYSIQUE
// ─────────────────────────────────────────────────────────────────────────────
void Avion::mettreAJour(double dt)
{
    m_distancePiste += m_vx_ms * dt;
    m_altitude      += m_vy_ms * dt;
    if (m_altitude < 0.0) m_altitude = 0.0;
}

// ─────────────────────────────────────────────────────────────────────────────
// CARBURANT
// ─────────────────────────────────────────────────────────────────────────────
void Avion::consommerCarburant(double dt)
{
    if (m_carburant_m3 > 0.0) {
        m_carburant_m3 -= m_consommation_m3_s * dt;
        if (m_carburant_m3 < 0.0) m_carburant_m3 = 0.0;
    }
}

bool Avion::estEnPanneSeche() const
{
    return m_carburant_m3 <= 0.0;
}

// ─────────────────────────────────────────────────────────────────────────────
// CONVERSIONS CARBURANT (m³ ↔ Litres)
// ─────────────────────────────────────────────────────────────────────────────
double Avion::getCapaciteEnL() const
{
    return m_capacite_m3 * 1000.0;
}

double Avion::getCapaciteEnM3() const
{
    return m_capacite_m3;
}

double Avion::getCarburantRestantEnL() const
{
    return m_carburant_m3 * 1000.0;
}

double Avion::getCarburantRestantEnM3() const
{
    return m_carburant_m3;
}

double Avion::getPourcentageCarburant() const
{
    if (m_capacite_m3 <= 0.0) return 0.0;
    return (m_carburant_m3 / m_capacite_m3) * 100.0;
}

// ─────────────────────────────────────────────────────────────────────────────
// FORMATAGE SELON UNITÉ CHOISIE
// ─────────────────────────────────────────────────────────────────────────────
QString Avion::getCapaciteFormatee() const
{
    if (m_uniteVolumePreferee == UniteVolume::LITRES) {
        return QString("%1 L").arg(getCapaciteEnL(), 0, 'f', 0);
    } else {
        return QString("%1 m³").arg(m_capacite_m3, 0, 'f', 2);
    }
}

QString Avion::getConsommationFormatee() const
{
    double valeur;
    QString unite;
    
    switch (m_uniteDebitPreferee) {
        case UniteDebit::L_S:
            valeur = m_consommation_m3_s * 1000.0;
            unite = "L/s";
            break;
        case UniteDebit::L_MIN:
            valeur = m_consommation_m3_s * 1000.0 * 60.0;
            unite = "L/min";
            break;
        case UniteDebit::L_H:
            valeur = m_consommation_m3_s * 1000.0 * 3600.0;
            unite = "L/h";
            break;
        case UniteDebit::M3_S:
            valeur = m_consommation_m3_s;
            unite = "m³/s";
            break;
        case UniteDebit::M3_MIN:
            valeur = m_consommation_m3_s * 60.0;
            unite = "m³/min";
            break;
        case UniteDebit::M3_H:
            valeur = m_consommation_m3_s * 3600.0;
            unite = "m³/h";
            break;
        default:
            valeur = m_consommation_m3_s * 1000.0 * 60.0;
            unite = "L/min";
    }
    
    return QString("%1 %2").arg(valeur, 0, 'f', 4).arg(unite);
}

QString Avion::getCarburantRestantFormate() const
{
    if (m_uniteVolumePreferee == UniteVolume::LITRES) {
        return QString("%1 L").arg(getCarburantRestantEnL(), 0, 'f', 0);
    } else {
        return QString("%1 m³").arg(m_carburant_m3, 0, 'f', 2);
    }
}

void Avion::setUniteVolumePreferee(UniteVolume unite)
{
    m_uniteVolumePreferee = unite;
}

void Avion::setUniteDebitPreferee(UniteDebit debit)
{
    m_uniteDebitPreferee = debit;
}

// ─────────────────────────────────────────────────────────────────────────────
// CONTRÔLE VX
// ─────────────────────────────────────────────────────────────────────────────
void Avion::augmenterVx(ModeFreinage mode)
{
    m_vx_ms += m_acceleration_ms2;

    if (mode == ModeFreinage::FLEXIBLE) {
        if (m_vx_ms > m_vxRef_ms) {
            m_vxRef_ms = m_vx_ms;
            m_freinageConsommeX_ms = 0.0;
        } else {
            m_freinageConsommeX_ms = m_vxRef_ms - m_vx_ms;
        }
    }
}

bool Avion::diminuerVx(ModeFreinage mode)
{
    double palier = m_acceleration_ms2;

    if (mode == ModeFreinage::ABSOLU) {
        if (m_freinageConsommeX_ms + palier > m_gammaX_ms)
            return false;
        m_vx_ms -= palier;
        m_freinageConsommeX_ms += palier;
    }
    else {
        double vxMin = m_vxRef_ms - m_gammaX_ms;
        if (m_vx_ms - palier < vxMin)
            return false;
        m_vx_ms -= palier;
        m_freinageConsommeX_ms = m_vxRef_ms - m_vx_ms;
    }
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// CONTRÔLE VY
// ─────────────────────────────────────────────────────────────────────────────
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

// ─────────────────────────────────────────────────────────────────────────────
// CONVERSIONS GÉNÉRALES
// ─────────────────────────────────────────────────────────────────────────────
double Avion::kmhToMs(double kmh) { return kmh / 3.6; }
double Avion::msToKmh(double ms)  { return ms * 3.6;  }

// ─────────────────────────────────────────────────────────────────────────────
// CALCULS PHYSIQUES
// ─────────────────────────────────────────────────────────────────────────────
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
        return m_vx_ms < m_vDecrochage_ms;
    else
        return getVitesseTotale_ms() < m_vDecrochage_ms;
}


// ─────────────────────────────────────────────────────────────────────────────
// SETTERS
// ─────────────────────────────────────────────────────────────────────────────
void Avion::setVitesseX_ms(double ms)  { m_vx_ms = ms; m_vxRef_ms = ms; }
void Avion::setVitesseY_ms(double ms)  { m_vy_ms = ms; m_vyRef_ms = ms; }
void Avion::setAltitude(double alt)    { m_altitude = alt; }
void Avion::setDistancePiste(double d) { m_distancePiste = d; }

void Avion::resetFreinage()
{
    m_freinageConsommeX_ms = 0.0;
    m_freinageConsommeY_ms = 0.0;
    m_vxRef_ms = m_vx_ms;
    m_vyRef_ms = m_vy_ms;
}

// À ajouter après les autres méthodes
void Avion::declencherChute()
{
    // Perte totale de contrôle
    m_vx_ms = 0.0;           // Plus de mouvement horizontal
    m_vy_ms = -50.0;         // Chute verticale rapide (180 km/h vers le bas)
    // On laisse l'altitude inchangée, la chute commencera au prochain tick
}