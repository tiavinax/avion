#include "ZoneSimulation.h"
#include <QPen>
#include <QBrush>
#include <QFont>
#include <cmath>

static const double SCENE_Y_MAX = 1200.0;

ZoneSimulation::ZoneSimulation(Simulation *sim, QWidget *parent)
    : QWidget(parent), m_sim(sim), m_vue(Vue::GAUCHE)
{
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(220, 230, 245));
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
void ZoneSimulation::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if (m_vue == Vue::ARRIERE)
        dessinerVueArriere(p);
    else
        dessinerVueProfil(p);
}

// VUES DE PROFIL (GAUCHE/DROITE)
void ZoneSimulation::dessinerVueProfil(QPainter &p)
{
    if (!m_sim || !m_sim->getAvion()) return;

    Avion  *avion    = m_sim->getAvion();
    double  distAvion = avion->getDistancePiste();

    int    centreX = width()  / 2;
    int    avionY  = altitudeVersPixelY(avion->getAltitude());
    double echelleX = width() / 6000.0;

    // ── Sol ───────────────────────────────────────────────────────
    int solY = altitudeVersPixelY(0.0);
    p.setPen(QPen(QColor(80, 80, 80), 2));
    p.drawLine(0, solY, width(), solY);
    p.fillRect(0, solY, width(), height() - solY, QColor(80, 70, 60));

    // ── Signe selon vue ───────────────────────────────────────────
    // Vue Gauche : caméra au NORD, avion vole vers l'OUEST (gauche écran)
    //   → nez vers gauche → signe = -1
    //   → piste devant = à gauche = valeurs décroissantes en X
    // Vue Droite : caméra au SUD, avion vu de l'autre côté
    //   → nez vers droite → signe = +1
    //   → piste devant = à droite = valeurs croissantes en X
    int signe = (m_vue == Vue::GAUCHE) ? -1 : 1;

    // ── Piste : position relative à l'avion ──────────────────────
    int xDebutPiste = centreX + static_cast<int>(
                          (0.0    - distAvion) * echelleX * signe);
    int xFinPiste   = centreX + static_cast<int>(
                          (7000.0 - distAvion) * echelleX * signe);

    int xP1 = qMin(xDebutPiste, xFinPiste);
    int xP2 = qMax(xDebutPiste, xFinPiste);

    int ep = 8;
    p.fillRect(xP1, solY - ep / 2, xP2 - xP1, ep, QColor(100, 100, 110));

    // Marquages début/fin piste
    p.setPen(QPen(Qt::white, 2));
    p.drawLine(xDebutPiste, solY - 12, xDebutPiste, solY + 12);
    p.drawLine(xFinPiste,   solY - 12, xFinPiste,   solY + 12);

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
        p.setPen(QColor(60, 60, 80));
        p.drawText(4, y - 2, QString("%1m").arg(alt));
    }

    // ── Soleil (vue gauche uniquement, côté droit = EST = derrière) ──
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

    // ── Avion — rectangle rouge + tête pointue ────────────────────
    // Couleur : rouge uni, sauf réussi → vert
    QColor couleurAvion = Qt::red;
    if (m_sim->getEtat() == EtatSimulation::REUSSI)
        couleurAvion = QColor(0, 180, 0);

    p.setPen(QPen(couleurAvion, 1));
    p.setBrush(QBrush(couleurAvion));

    // Corps : rectangle horizontal centré sur (centreX, avionY)
    // Largeur = 40px, Hauteur = 14px
    int w = 40;   // largeur du corps
    int h = 14;   // hauteur du corps

    // signe = -1 (Vue Gauche) : nez vers gauche → nez à centreX - w/2
    // signe = +1 (Vue Droite) : nez vers droite → nez à centreX + w/2
    int xGauche = centreX - w / 2;
    int xDroite = centreX + w / 2;
    int yHaut   = avionY  - h / 2;
    int yBas    = avionY  + h / 2;

    // Corps rectangulaire
    p.drawRect(xGauche, yHaut, w, h);

    // Tête pointue : triangle collé au bout avant du corps
    // Vue Gauche (signe=-1) : pointe vers la gauche
    // Vue Droite (signe=+1) : pointe vers la droite
    int xBasePointe = (signe == -1) ? xGauche : xDroite;
    int xPointe     = xBasePointe + signe * 15;  // 15px de longueur de pointe

    QPolygon tete;
    tete << QPoint(xBasePointe, yHaut)      // coin haut de la base
         << QPoint(xBasePointe, yBas)       // coin bas de la base
         << QPoint(xPointe,     avionY);    // sommet de la pointe
    p.drawPolygon(tete);

    // Petite aile : ligne perpendiculaire au corps côté arrière
    int xAile = (signe == -1) ? xDroite - 5 : xGauche + 5;
    p.setPen(QPen(couleurAvion, 3));
    p.drawLine(xAile, avionY - h / 2 - 10,
               xAile, avionY + h / 2 + 10);
}

// VUE ARRIÈRE
void ZoneSimulation::dessinerVueArriere(QPainter &p)
{
    if (!m_sim || !m_sim->getAvion())
        return;

    Avion *avion = m_sim->getAvion();
    double distAvion = avion->getDistancePiste(); // négatif avant la piste
    double altitude = avion->getAltitude();

    int cx = width() / 2;  // centre horizontal
    int cy = height() / 2; // centre vertical

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
    double distanceAuDebutPiste = -distAvion;        // positive quand avant la piste
    double distanceAlaFinPiste = 7000.0 - distAvion; // toujours positive devant nous

    // Facteur de perspective : plus on est loin, plus la piste est petite
    // On clamp pour éviter les divisions par zéro ou valeurs négatives
    double distDebut = qMax(distanceAuDebutPiste, 1.0);
    double distFin = qMax(distanceAlaFinPiste, 1.0);

    // Largeur apparente de la piste selon la distance (perspective simple)
    // Plus on est proche, plus la largeur en pixels est grande
    double largeurRef = width() * 0.4; // largeur max à distance 0
    double refDist = 500.0;            // distance de référence

    int largeurDebut = static_cast<int>(largeurRef * refDist / distDebut);
    int largeurFin = static_cast<int>(largeurRef * refDist / distFin);

    // Clamp pour ne pas dépasser la largeur de l'écran
    largeurDebut = qMin(largeurDebut, width());
    largeurFin = qMin(largeurFin, width());

    // Position Y de chaque extrémité de la piste
    // Plus loin = plus haut (vers l'horizon)
    int yDebut = horizonY + static_cast<int>(
                                (height() - horizonY) * qMin(refDist / distDebut, 1.0));
    int yFin = horizonY + static_cast<int>(
                              (height() - horizonY) * qMin(refDist / distFin, 1.0));

    yDebut = qMin(yDebut, height() - 5);
    yFin = qMin(yFin, height() - 5);

    // Dessine la piste comme un trapèze (perspective)
    QPolygon piste;
    piste << QPoint(cx - largeurFin / 2, yFin)
          << QPoint(cx + largeurFin / 2, yFin)
          << QPoint(cx + largeurDebut / 2, yDebut)
          << QPoint(cx - largeurDebut / 2, yDebut);
    p.setBrush(QBrush(QColor(100, 100, 110)));
    p.setPen(QPen(Qt::white, 1));
    p.drawPolygon(piste);

    // Ligne centrale de la piste (axe de guidage)
    p.setPen(QPen(QColor(255, 255, 100, 180), 2, Qt::DashLine));
    p.drawLine(cx, yFin, cx, yDebut);

    // ── Avion vu de dos (au centre) ───────────────────────────────
    QColor couleur = Qt::red;
    if (m_sim->getEtat() == EtatSimulation::DETRUIT)
        couleur = Qt::red;
    if (m_sim->getEtat() == EtatSimulation::REUSSI)
        couleur = Qt::green;

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
    p.drawEllipse(cx - 55, cy - 5, 18, 10);
    p.drawEllipse(cx + 37, cy - 5, 18, 10);

    // Stabilisateur vertical (dérive)
    QPolygon derive;
    derive << QPoint(cx, cy - 10)
           << QPoint(cx, cy - 30)
           << QPoint(cx + 12, cy - 10);
    p.setBrush(QBrush(couleur));
    p.drawPolygon(derive);

    // Stabilisateurs horizontaux arrière
    p.setPen(QPen(couleur, 3));
    p.drawLine(cx - 25, cy + 8, cx - 8, cy + 2);
    p.drawLine(cx + 8, cy + 2, cx + 25, cy + 8);

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