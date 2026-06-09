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

// Unités pour l'affichage du carburant
enum class UniteVolume
{
    LITRES,
    METRES_CUBES
};

enum class UniteDebit
{
    L_S,
    L_MIN,
    L_H,
    M3_S,
    M3_MIN,
    M3_H
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
          double gammaX_ms = 40.0,
          double gammaY_ms = 60.0,
          double capaciteCarburant_m3 = 5.0,     // 5000 L = 5 m³
          double consommation_m3_par_s = 0.0001666667, // 10 L/min
          UniteVolume uniteVolumePreferee = UniteVolume::LITRES,
          UniteDebit uniteDebitPreferee = UniteDebit::L_MIN);

    // ── Mise à jour physique ──────────────────────────────────────
    void mettreAJour(double dt);

    // ── Consommation carburant ────────────────────────────────────
    void consommerCarburant(double dt);
    bool estEnPanneSeche() const;

    // ── Contrôle des vitesses (Mode Direct) ───────────────────────
    void augmenterVx(ModeFreinage mode = ModeFreinage::FLEXIBLE);
    bool diminuerVx(ModeFreinage mode = ModeFreinage::FLEXIBLE);
    void augmenterVy(ModeFreinage mode = ModeFreinage::FLEXIBLE);
    bool diminuerVy(ModeFreinage mode = ModeFreinage::FLEXIBLE);

    // ── Conversions ───────────────────────────────────────────────
    static double kmhToMs(double kmh);
    static double msToKmh(double ms);
    
    // ── Conversions carburant ─────────────────────────────────────
    double getCapaciteEnL() const;      // m³ → L
    double getCapaciteEnM3() const;     // retour direct
    double getCarburantRestantEnL() const;
    double getCarburantRestantEnM3() const;
    double getPourcentageCarburant() const;
    
    // Formate selon l'unité préférée de l'utilisateur
    QString getCapaciteFormatee() const;
    QString getConsommationFormatee() const;
    QString getCarburantRestantFormate() const;
    
    // ── Gestion unités préférées ──────────────────────────────────
    void setUniteVolumePreferee(UniteVolume unite);
    void setUniteDebitPreferee(UniteDebit unite);
    UniteVolume getUniteVolumePreferee() const { return m_uniteVolumePreferee; }
    UniteDebit getUniteDebitPreferee() const { return m_uniteDebitPreferee; }

    // ── Calculs physiques ─────────────────────────────────────────
    double getVitesseTotale_ms() const;
    bool estEnVol() const;
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
    double getCarburant_m3() const { return m_carburant_m3; }
    double getCapacite_m3() const { return m_capacite_m3; }
    double getConsommation_m3_s() const { return m_consommation_m3_s; }

    // ── Setters ───────────────────────────────────────────────────
    void setVitesseX_ms(double ms);
    void setVitesseY_ms(double ms);
    void setAltitude(double alt);
    void setDistancePiste(double dist);
    void resetFreinage();
    void declencherChute();  // NOUVELLE MÉTHODE

private:
    QString m_nom;

    // ── Vitesses en m/s ───────────────────────────────────────────
    double m_vx_ms;
    double m_vy_ms;

    // ── Paramètres ────────────────────────────────────────────────
    double m_acceleration_ms2;
    double m_vDecrochage_ms;

    // ── Position ─────────────────────────────────────────────────
    double m_altitude;
    double m_distancePiste;

    // ── Budget freinage en m/s ────────────────────────────────────
    double m_gammaX_ms;
    double m_gammaY_ms;
    double m_freinageConsommeX_ms;
    double m_freinageConsommeY_ms;
    double m_vxRef_ms;
    double m_vyRef_ms;

    // ── Carburant (stocké en unités SI) ───────────────────────────
    double m_carburant_m3;           // carburant actuel en m³
    double m_capacite_m3;            // capacité totale en m³
    double m_consommation_m3_s;      // consommation en m³/seconde
    
    // ── Préférences d'affichage utilisateur ───────────────────────
    UniteVolume m_uniteVolumePreferee;  // L ou m³
    UniteDebit  m_uniteDebitPreferee;   // L/s, L/min, L/h, m³/s, m³/min, m³/h
};

#endif // AVION_H