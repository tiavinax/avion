#include "ZoneSimulation.h"
#include <QPen>
#include <QBrush>
#include <QFont>
#include <cmath>

static const double SCENE_Y_MAX  = 1200.0;

ZoneSimulation::ZoneSimulation(Simulation* sim, QWidget* parent)
    : QWidget(parent)
    , m_sim(sim)
    , m_vue(Vue::GAUCHE)
{
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(15, 20, 40));
    setPalette(pal);
    setMinimumSize(900, 400);
}

void ZoneSimulation::setVue(Vue vue)
{
    m_vue = vue;
    update();
}

void ZoneSimulation::rafraichir()
{
    update();
}

// ── Conversion altitude → pixel Y (commune à toutes les vues de profil) ───────
int ZoneSimulation::altitudeVersPixelY(double altitude) const
{
    double ratio = altitude / SCENE_Y_MAX;
    return static_cast<int>((1.0 - ratio) * height());
}

// ── paintEvent : aiguille le dessin selon la vue active ───────────────────────
void ZoneSimulation::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if (m_vue == Vue::ARRIERE)
        dessinerVueArriere(p);
    else
        dessinerVueProfil(p);
}

// ════════════════════════════════════════════════════════════════════════════
// VUE GAUCHE / VUE DROITE — repère centré sur l'avion
// L'avion est fixe au centre, c'est la scène qui défile autour de lui
// ════════════════════════════════════════════════════════════════════════════
void ZoneSimulation::dessinerVueProfil(QPainter& p)
{
    if (!m_sim || !m_sim->getAvion()) return;

    Avion* avion = m_sim->getAvion();
    double distAvion = avion->getDistancePiste(); // position réelle de l'avion

    // L'avion est toujours dessiné au centre horizontal du widget
    int centreX = width() / 2;
    int avionY  = altitudeVersPixelY(avion->getAltitude());

    // Facteur d'échelle : combien de pixels pour 1 mètre horizontal
    // On affiche 6000m de large autour de l'avion
    double echelleX = width() / 6000.0;

    // ── Sol ───────────────────────────────────────────────────────
    int solY = altitudeVersPixelY(0.0);
    p.setPen(QPen(QColor(80, 80, 80), 2));
    p.drawLine(0, solY, width(), solY);
    p.fillRect(0, solY, width(), height() - solY, QColor(30, 30, 30));

    // ── Piste : position relative à l'avion ──────────────────────
    // début piste = 0m dans le monde, fin = 7000m
    // pixel_piste = centreX + (position_monde - distAvion) * echelleX
    // Vue Droite : tout est miroir → on inverse le signe
    int signe = (m_vue == Vue::GAUCHE) ? 1 : -1;

    int xDebutPiste = centreX + static_cast<int>((0.0    - distAvion) * echelleX * signe);
    int xFinPiste   = centreX + static_cast<int>((7000.0 - distAvion) * echelleX * signe);

    // S'assure que x1 < x2 pour le dessin du rectangle
    int xP1 = qMin(xDebutPiste, xFinPiste);
    int xP2 = qMax(xDebutPiste, xFinPiste);

    int ep = 8; // épaisseur visuelle piste
    p.fillRect(xP1, solY - ep/2, xP2 - xP1, ep, QColor(100, 100, 110));

    // Marquages début/fin piste
    p.setPen(QPen(Qt::white, 2));
    p.drawLine(xDebutPiste, solY - 12, xDebutPiste, solY + 12);
    p.drawLine(xFinPiste,   solY - 12, xFinPiste,   solY + 12);

    // Étiquettes début/fin
    p.setFont(QFont("Arial", 8));
    p.setPen(Qt::white);
    p.drawText(xDebutPiste + 3, solY + 22, "0m");
    p.drawText(xFinPiste   + 3, solY + 22, "7000m");

    // ── Graduations altitude ──────────────────────────────────────
    p.setFont(QFont("Arial", 7));
    for (int alt = 200; alt <= 1000; alt += 200) {
        int y = altitudeVersPixelY(alt);
        p.setPen(QPen(QColor(50, 50, 70), 1, Qt::DotLine));
        p.drawLine(0, y, width(), y);
        p.setPen(QColor(120, 120, 140));
        p.drawText(4, y - 2, QString("%1m").arg(alt));
    }

    // ── Soleil (vue gauche uniquement) ────────────────────────────
    if (m_vue == Vue::GAUCHE) {
        p.setBrush(QBrush(QColor(255, 220, 50, 200)));
        p.setPen(QPen(QColor(255, 200, 0), 1));
        p.drawEllipse(20, 20, 45, 45);
        p.setPen(QPen(QColor(255, 220, 50, 130), 1));
        for (int angle = 0; angle < 360; angle += 45) {
            double rad = angle * M_PI / 180.0;
            int x1 = 42 + static_cast<int>(28 * cos(rad));
            int y1 = 42 + static_cast<int>(28 * sin(rad));
            int x2 = 42 + static_cast<int>(38 * cos(rad));
            int y2 = 42 + static_cast<int>(38 * sin(rad));
            p.drawLine(x1, y1, x2, y2);
        }
    }

    // ── Avion (centré, silhouette selon vue) ──────────────────────
    QColor couleur = Qt::white;
    if (m_sim->getEtat() == EtatSimulation::DETRUIT) couleur = Qt::red;
    if (m_sim->getEtat() == EtatSimulation::REUSSI)  couleur = Qt::green;

    p.setPen(QPen(couleur, 2));
    p.setBrush(QBrush(couleur));

    // Vue Gauche : nez vers droite (signe=+1)
    // Vue Droite : nez vers gauche (signe=-1) → miroir
    int d = signe;

    // Corps
    p.drawEllipse(centreX - 20*d, avionY - 6, 40, 12);

    // Nez
    QPolygon nez;
    nez << QPoint(centreX + 20*d, avionY)
        << QPoint(centreX + 30*d, avionY - 3)
        << QPoint(centreX + 30*d, avionY + 3);
    p.drawPolygon(nez);

    // Aile principale
    QPolygon aile;
    aile << QPoint(centreX,          avionY + 2)
         << QPoint(centreX - 15*d,   avionY + 2)
         << QPoint(centreX - 5*d,    avionY + 14);
    p.drawPolygon(aile);

    // Stabilisateur arrière
    QPolygon stab;
    stab << QPoint(centreX - 18*d, avionY - 6)
         << QPoint(centreX - 25*d, avionY - 6)
         << QPoint(centreX - 20*d, avionY - 12);
    p.drawPolygon(stab);

    // Point de position
    p.setPen(QPen(Qt::red, 3));
    p.drawPoint(centreX, avionY);
}

// ════════════════════════════════════════════════════════════════════════════
// VUE ARRIÈRE — caméra derrière l'avion, on regarde vers la piste
// La piste apparaît devant nous, en bas au centre
// Plus on est proche, plus elle est large
// ════════════════════════════════════════════════════════════════════════════
void ZoneSimulation::dessinerVueArriere(QPainter& p)
{
    if (!m_sim || !m_sim->getAvion()) return;

    Avion* avion = m_sim->getAvion();
    double distAvion  = avion->getDistancePiste(); // négatif avant la piste
    double altitude   = avion->getAltitude();

    int cx = width()  / 2;  // centre horizontal
    int cy = height() / 2;  // centre vertical

    // ── Ciel dégradé ─────────────────────────────────────────────
    // déjà géré par la couleur de fond du widget (bleu nuit)

    // ── Sol : ligne d'horizon ────────────────────────────────────
    // Plus l'altitude est haute, plus l'horizon est bas dans l'image
    double ratioHorizon = 0.5 + (altitude / SCENE_Y_MAX) * 0.3;
    int horizonY = static_cast<int>(ratioHorizon * height());

    p.fillRect(0, horizonY, width(), height() - horizonY, QColor(30, 30, 30));
    p.setPen(QPen(QColor(80, 80, 80), 2));
    p.drawLine(0, horizonY, width(), horizonY);

    // ── Piste vue en perspective ──────────────────────────────────
    // distanceRestante = distance qu'il reste à parcourir avant le début de piste
    // Si distAvion = -5000 → il reste 5000m
    // Si distAvion = 0     → on est au début de la piste
    // Si distAvion = 3500  → on est au milieu de la piste
    double distanceAuDebutPiste = -distAvion;       // positive quand avant la piste
    double distanceAlaFinPiste  = 7000.0 - distAvion; // toujours positive devant nous

    // Facteur de perspective : plus on est loin, plus la piste est petite
    // On clamp pour éviter les divisions par zéro ou valeurs négatives
    double distDebut = qMax(distanceAuDebutPiste, 1.0);
    double distFin   = qMax(distanceAlaFinPiste,  1.0);

    // Largeur apparente de la piste selon la distance (perspective simple)
    // Plus on est proche, plus la largeur en pixels est grande
    double largeurRef = width() * 0.4;  // largeur max à distance 0
    double refDist    = 500.0;           // distance de référence

    int largeurDebut = static_cast<int>(largeurRef * refDist / distDebut);
    int largeurFin   = static_cast<int>(largeurRef * refDist / distFin);

    // Clamp pour ne pas dépasser la largeur de l'écran
    largeurDebut = qMin(largeurDebut, width());
    largeurFin   = qMin(largeurFin,   width());

    // Position Y de chaque extrémité de la piste
    // Plus loin = plus haut (vers l'horizon)
    int yDebut = horizonY + static_cast<int>(
        (height() - horizonY) * qMin(refDist / distDebut, 1.0));
    int yFin   = horizonY + static_cast<int>(
        (height() - horizonY) * qMin(refDist / distFin,   1.0));

    yDebut = qMin(yDebut, height() - 5);
    yFin   = qMin(yFin,   height() - 5);

    // Dessine la piste comme un trapèze (perspective)
    QPolygon piste;
    piste << QPoint(cx - largeurFin/2,   yFin)
          << QPoint(cx + largeurFin/2,   yFin)
          << QPoint(cx + largeurDebut/2, yDebut)
          << QPoint(cx - largeurDebut/2, yDebut);
    p.setBrush(QBrush(QColor(100, 100, 110)));
    p.setPen(QPen(Qt::white, 1));
    p.drawPolygon(piste);

    // Ligne centrale de la piste (axe de guidage)
    p.setPen(QPen(QColor(255, 255, 100, 180), 2, Qt::DashLine));
    p.drawLine(cx, yFin, cx, yDebut);

    // ── Avion vu de dos (au centre) ───────────────────────────────
    QColor couleur = Qt::white;
    if (m_sim->getEtat() == EtatSimulation::DETRUIT) couleur = Qt::red;
    if (m_sim->getEtat() == EtatSimulation::REUSSI)  couleur = Qt::green;

    p.setPen(QPen(couleur, 2));
    p.setBrush(QBrush(couleur));

    // Fuselage (cercle)
    p.drawEllipse(cx - 10, cy - 10, 20, 20);

    // Ailes (ligne horizontale)
    p.setPen(QPen(couleur, 4));
    p.drawLine(cx - 60, cy, cx + 60, cy);

    // Réacteurs sous les ailes
    p.setPen(QPen(couleur, 2));
    p.setBrush(QBrush(QColor(50, 50, 50)));
    p.drawEllipse(cx - 55, cy - 5,  18, 10);
    p.drawEllipse(cx + 37, cy - 5,  18, 10);

    // Stabilisateur vertical (dérive)
    QPolygon derive;
    derive << QPoint(cx,      cy - 10)
           << QPoint(cx,      cy - 30)
           << QPoint(cx + 12, cy - 10);
    p.setBrush(QBrush(couleur));
    p.drawPolygon(derive);

    // Stabilisateurs horizontaux arrière
    p.setPen(QPen(couleur, 3));
    p.drawLine(cx - 25, cy + 8, cx - 8, cy + 2);
    p.drawLine(cx + 8,  cy + 2, cx + 25, cy + 8);

    // ── Indicateur de distance à la piste ────────────────────────
    p.setFont(QFont("Arial", 10, QFont::Bold));
    p.setPen(Qt::white);
    QString txtDist = QString("Distance piste : %1 m")
                          .arg(static_cast<int>(distAvion));
    p.drawText(10, 20, txtDist);

    // Altitude
    QString txtAlt = QString("Altitude : %1 m")
                         .arg(static_cast<int>(altitude));
    p.drawText(10, 38, txtAlt);
}