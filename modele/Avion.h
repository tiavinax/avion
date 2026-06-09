#ifndef AVION_H
#define AVION_H

#include <QString>

// Modes de gestion du budget freinage
enum class ModeFreinage
{
    ABSOLU,
    FLEXIBLE
};

// Modes de calcul du décrochage
enum class ModeDecrochage
{
    VX_SEULE,
    NORME
};

class Avion
{
public:
    Avion(const QString &nom,
          double vitesseX_kmh,
          double vitesseY_kmh,
          double acceleration_ms2,
          double vitesseDecrochage_kmh,
          double altitude,
          double distancePiste,
          double gammaX_ms = 40.0,  // m/s directement
          double gammaY_ms = 60.0); // m/s directement

    // ── Mise à jour physique ──────────────────────────────────────
    void mettreAJour(double dt);

    // ── Contrôle des vitesses (Mode Direct) ───────────────────────
    // Vx += acceleration_ms2 instantanément
    void augmenterVx(ModeFreinage mode = ModeFreinage::FLEXIBLE);

    // Vx -= acceleration_ms2, vérifie budget selon mode freinage
    // Retourne true si accepté
    bool diminuerVx(ModeFreinage mode = ModeFreinage::FLEXIBLE);

    void augmenterVy(ModeFreinage mode = ModeFreinage::FLEXIBLE);
    bool diminuerVy(ModeFreinage mode = ModeFreinage::FLEXIBLE);

    // ── Conversions ───────────────────────────────────────────────
    static double kmhToMs(double kmh);
    static double msToKmh(double ms);

    // ── Calculs physiques ─────────────────────────────────────────
    double getVitesseTotale_ms() const;
    bool estEnVol() const;

    // Vérifie décrochage selon le mode choisi
    bool estEnDecrochage(ModeDecrochage mode) const;

    // ── Getters ───────────────────────────────────────────────────
    QString getNom() const { return m_nom; }
    double getVitesseX_kmh() const { return msToKmh(m_vx_ms); }
    double getVitesseY_kmh() const { return msToKmh(m_vy_ms); }
    double getVitesseX_ms() const { return m_vx_ms; }
    double getVitesseY_ms() const { return m_vy_ms; }
    double getAltitude() const { return m_altitude; }
    double getDistancePiste() const { return m_distancePiste; }
    double getAcceleration_ms2() const { return m_acceleration_ms2; }
    double getVitesseDecrochage_kmh() const { return msToKmh(m_vDecrochage_ms); }
    double getVitesseDecrochage_ms() const { return m_vDecrochage_ms; }
    double getFreinageConsommeX_ms() const { return m_freinageConsommeX_ms; }
    double getFreinageConsommeY_ms() const { return m_freinageConsommeY_ms; }
    double getGammaX_ms() const { return m_gammaX_ms; }
    double getGammaY_ms() const { return m_gammaY_ms; }

    // ── Setters ───────────────────────────────────────────────────
    void setVitesseX_ms(double ms);
    void setVitesseY_ms(double ms);
    void setAltitude(double alt);
    void setDistancePiste(double dist);

    // Réinitialise les budgets de freinage (utilisé par RESTART)
    void resetFreinage();

private:
    QString m_nom;

    // ── Vitesses en m/s ───────────────────────────────────────────
    double m_vx_ms;
    double m_vy_ms;

    // ── Paramètres ────────────────────────────────────────────────
    double m_acceleration_ms2; // palier direct : 10 m/s par clic
    double m_vDecrochage_ms;   // seuil décrochage en m/s

    // ── Position ─────────────────────────────────────────────────
    double m_altitude;
    double m_distancePiste;

    // ── Budget freinage en m/s ────────────────────────────────────
    double m_gammaX_ms; // budget max X (ex: 40 km/h → 11.11 m/s)
    double m_gammaY_ms; // budget max Y (ex: 60 km/h → 16.67 m/s)

    // Mode ABSOLU : cumul total des diminutions
    double m_freinageConsommeX_ms;
    double m_freinageConsommeY_ms;

    // Mode FLEXIBLE : vitesse de référence au premier freinage
    // Si on accélère puis freine → la référence se remet à jour
    double m_vxRef_ms; // Vx au moment où on commence à freiner
    double m_vyRef_ms;
};

#endif // AVION_H