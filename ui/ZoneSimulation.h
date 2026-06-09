#ifndef ZONESIMULATION_H
#define ZONESIMULATION_H

#include <QWidget>
#include <QPainter>
#include "modele/Simulation.h"

// Vues disponibles pour l'affichage de l'avion
enum class Vue { GAUCHE, DROITE, ARRIERE };

// Widget de rendu graphique — dessine la scène à chaque tick
// Hérite de QWidget et surcharge paintEvent()
class ZoneSimulation : public QWidget
{
    Q_OBJECT

public:
    explicit ZoneSimulation(Simulation* sim, QWidget* parent = nullptr);

    // Change la vue courante (gauche/droite/arrière)
    void setVue(Vue vue);

public slots:
    // Appelé à chaque tick → force le redessin
    void rafraichir();

protected:
    // Qt appelle cette méthode automatiquement quand le widget doit se redessiner
    void paintEvent(QPaintEvent* event) override;

private:
    Simulation* m_sim;  // référence vers la simulation (pour lire position avion)
    Vue         m_vue;  // vue courante

    // ── Méthodes de dessin ────────────────────────────────────────

    // Vue gauche et droite : repère horizontal, avion centré, scène défile
    void dessinerVueProfil(QPainter& p);

    // Vue arrière : caméra derrière l'avion, piste en perspective devant
    void dessinerVueArriere(QPainter& p);

    // ── Conversion altitude → pixel Y ────────────────────────────
    // Commune aux vues de profil : altitude 0 = bas, altitude max = haut
    int altitudeVersPixelY(double altitude) const;
};

#endif // ZONESIMULATION_H