#include "TableauDeBord.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFont>

TableauDeBord::TableauDeBord(Simulation *sim, QWidget *parent)
    : QWidget(parent), m_sim(sim)
{
    construireUI();
}

void TableauDeBord::construireUI()
{
    // Fond blanc, texte noir — mode jour
    setStyleSheet("background-color: #f5f5f5; color: #1a1a1a;");
    setMinimumWidth(280);

    // Style commun des groupes
    QString styleGroupe =
        "QGroupBox {"
        "    color: #333333;"
        "    border: 1px solid #cccccc;"
        "    border-radius: 4px;"
        "    margin-top: 8px;"
        "    font-weight: bold;"
        "    font-size: 11px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 8px;"
        "    padding: 0 4px;"
        "}";

    // Créateur de ligne : label titre gris foncé + valeur noire bold
    auto creerLigne = [](QGridLayout *grid, int ligne,
                         const QString &titre, QLabel *&labelValeur)
    {
        QLabel *lblTitre = new QLabel(titre);
        // Label titre : gris foncé, lisible
        lblTitre->setStyleSheet(
            "color: #555555; font-size: 14px; font-weight: semi-bold;");

        labelValeur = new QLabel("---");
        // Valeur : noir, gras, bien visible
        labelValeur->setStyleSheet(
            "color: #111111; font-size: 13px; font-weight: bold;");

        grid->addWidget(lblTitre,    ligne, 0);
        grid->addWidget(labelValeur, ligne, 1);
    };

    // ── Groupe : Position ─────────────────────────────────────────
    QGroupBox   *grpPosition = new QGroupBox("Position");
    QGridLayout *gridPos     = new QGridLayout(grpPosition);
    grpPosition->setStyleSheet(styleGroupe);

    creerLigne(gridPos, 0, "Altitude (m)",       m_lblAltitude);
    creerLigne(gridPos, 1, "Distance piste (m)", m_lblDistancePiste);

    // ── Groupe : Vitesses ─────────────────────────────────────────
    QGroupBox   *grpVitesse = new QGroupBox("Vitesses");
    QGridLayout *gridVit    = new QGridLayout(grpVitesse);
    grpVitesse->setStyleSheet(styleGroupe);

    creerLigne(gridVit, 0, "Vx (km/h | m/s)", m_lblVitesseX);
    creerLigne(gridVit, 1, "Vy (km/h | m/s)", m_lblVitesseY);
    creerLigne(gridVit, 2, "Vtotale (m/s)",   m_lblVitesseTotale);

    // ── Groupe : Freinage ─────────────────────────────────────────
    QGroupBox   *grpFreinage = new QGroupBox("Budget freinage");
    QGridLayout *gridFrn     = new QGridLayout(grpFreinage);
    grpFreinage->setStyleSheet(styleGroupe);

    creerLigne(gridFrn, 0, "Consommé X", m_lblFreinageX);
    creerLigne(gridFrn, 1, "Consommé Y", m_lblFreinageY);

    // ── Groupe : Carburant ────────────────────────────────────────
    QGroupBox  *grpCarburant = new QGroupBox("Carburant");
    grpCarburant->setStyleSheet(styleGroupe);
    QVBoxLayout *layoutCarb = new QVBoxLayout(grpCarburant);

    m_lblCarburantValeur = new QLabel("---");
    // Valeur carburant : reste colorée (orange → visible)
    m_lblCarburantValeur->setStyleSheet(
        "color: #e67e22; font-size: 13px; font-weight: bold;");
    m_lblCarburantValeur->setAlignment(Qt::AlignCenter);

    m_barreCarburant = new QProgressBar();
    m_barreCarburant->setRange(0, 100);
    m_barreCarburant->setValue(100);
    // Barre sobre sur fond clair
    m_barreCarburant->setStyleSheet(
        "QProgressBar {"
        "    background-color: #e0e0e0;"
        "    border: 1px solid #bbbbbb;"
        "    border-radius: 4px;"
        "    text-align: center;"
        "    color: #333333;"
        "    font-weight: bold;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #27ae60;"
        "    border-radius: 3px;"
        "}");

    m_lblConsommation = new QLabel("Consommation: ---");
    m_lblConsommation->setStyleSheet(
        "color: #777777; font-size: 10px;");
    m_lblConsommation->setAlignment(Qt::AlignCenter);

    layoutCarb->addWidget(m_lblCarburantValeur);
    layoutCarb->addWidget(m_barreCarburant);
    layoutCarb->addWidget(m_lblConsommation);

    // ── Groupe : Chronomètre ──────────────────────────────────────
    QGroupBox   *grpTemps = new QGroupBox("Chronomètre");
    QGridLayout *gridTps  = new QGridLayout(grpTemps);
    grpTemps->setStyleSheet(styleGroupe);

    creerLigne(gridTps, 0, "Temps", m_lblChronometre);
    // Chronomètre : bleu foncé bien lisible sur fond clair
    m_lblChronometre->setStyleSheet(
        "color: #2980b9; font-size: 16px; font-weight: bold;");

    // ── Groupe : État ─────────────────────────────────────────────
    QGroupBox   *grpEtat  = new QGroupBox("État");
    QGridLayout *gridEtat = new QGridLayout(grpEtat);
    grpEtat->setStyleSheet(styleGroupe);

    creerLigne(gridEtat, 0, "", m_lblEtat);
    m_lblEtat->setStyleSheet("color: #555555; font-size: 12px;");
    m_lblEtat->setText("En attente...");

    // ── Layout principal ──────────────────────────────────────────
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(6);
    layout->addWidget(grpPosition);
    layout->addWidget(grpVitesse);
    layout->addWidget(grpFreinage);
    layout->addWidget(grpCarburant);
    layout->addWidget(grpTemps);
    layout->addWidget(grpEtat);
    layout->addStretch();
    setLayout(layout);
}

void TableauDeBord::rafraichir()
{
    if (!m_sim || !m_sim->getAvion()) return;

    Avion *avion = m_sim->getAvion();

    // ── Position ──────────────────────────────────────────────────
    m_lblAltitude->setText(
        QString("%1 m").arg(avion->getAltitude(), 0, 'f', 1));
    m_lblDistancePiste->setText(
        QString("%1 m").arg(avion->getDistancePiste(), 0, 'f', 1));

    // ── Vitesses ──────────────────────────────────────────────────
    m_lblVitesseX->setText(
        QString("%1 km/h | %2 m/s")
            .arg(avion->getVitesseX_kmh(), 0, 'f', 1)
            .arg(avion->getVitesseX_ms(),  0, 'f', 1));
    m_lblVitesseY->setText(
        QString("%1 km/h | %2 m/s")
            .arg(avion->getVitesseY_kmh(), 0, 'f', 1)
            .arg(avion->getVitesseY_ms(),  0, 'f', 1));
    m_lblVitesseTotale->setText(
        QString("%1 m/s").arg(avion->getVitesseTotale_ms(), 0, 'f', 1));

    // ── Budget freinage ───────────────────────────────────────────
    double consX = avion->getFreinageConsommeX_ms();
    double consY = avion->getFreinageConsommeY_ms();

    // Épuisé → rouge vif lisible sur fond clair
    m_lblFreinageX->setStyleSheet(
        consX >= avion->getGammaX_ms()
            ? "color: #c0392b; font-size: 13px; font-weight: bold;"
            : "color: #111111; font-size: 13px; font-weight: bold;");
    m_lblFreinageY->setStyleSheet(
        consY >= avion->getGammaY_ms()
            ? "color: #c0392b; font-size: 13px; font-weight: bold;"
            : "color: #111111; font-size: 13px; font-weight: bold;");

    m_lblFreinageX->setText(QString("%1 / %2 m/s")
        .arg(consX, 0, 'f', 1).arg(avion->getGammaX_ms(), 0, 'f', 0));
    m_lblFreinageY->setText(QString("%1 / %2 m/s")
        .arg(consY, 0, 'f', 1).arg(avion->getGammaY_ms(), 0, 'f', 0));

    // ── Carburant ─────────────────────────────────────────────────
    double pourcentage = avion->getPourcentageCarburant();
    m_lblCarburantValeur->setText(
        QString("%1 / %2")
            .arg(avion->getCarburantRestantFormate())
            .arg(avion->getCapaciteFormatee()));
    m_barreCarburant->setValue(static_cast<int>(pourcentage));

    // Couleur barre selon niveau
    QString couleurBarre;
    if (pourcentage < 10)
        couleurBarre = "#e74c3c";       // rouge critique
    else if (pourcentage < 25)
        couleurBarre = "#e67e22";       // orange avertissement
    else
        couleurBarre = "#27ae60";       // vert normal

    m_barreCarburant->setStyleSheet(
        "QProgressBar {"
        "    background-color: #e0e0e0; border: 1px solid #bbbbbb;"
        "    border-radius: 4px; text-align: center; color: #333333; font-weight: bold;"
        "}"
        "QProgressBar::chunk { background-color: " + couleurBarre + "; border-radius: 3px; }");

    m_lblConsommation->setText(
        QString("Conso: %1").arg(avion->getConsommationFormatee()));

    // ── Chronomètre ───────────────────────────────────────────────
    m_lblChronometre->setText(formaterTemps(m_sim->getTempsEcoule_ms()));

    // ── État ──────────────────────────────────────────────────────
    switch (m_sim->getEtat()) {
    case EtatSimulation::EN_ATTENTE:
        m_lblEtat->setStyleSheet("color: #555555; font-size: 12px;");
        m_lblEtat->setText("En attente...");
        break;
    case EtatSimulation::EN_COURS:
        m_lblEtat->setStyleSheet("color: #27ae60; font-size: 12px; font-weight: bold;");
        m_lblEtat->setText("En cours");
        break;
    case EtatSimulation::EN_PAUSE:
        m_lblEtat->setStyleSheet("color: #e67e22; font-size: 12px; font-weight: bold;");
        m_lblEtat->setText("PAUSE");
        break;
    case EtatSimulation::EN_CHUTE:
        m_lblEtat->setStyleSheet("color: #e74c3c; font-size: 12px; font-weight: bold;");
        m_lblEtat->setText("⚠ CHUTE EN COURS...");
        break;
    case EtatSimulation::REUSSI:
        m_lblEtat->setStyleSheet("color: #27ae60; font-size: 14px; font-weight: bold;");
        m_lblEtat->setText("ATTERRISSAGE RÉUSSI ✓");
        break;
    case EtatSimulation::DETRUIT: {
        m_lblEtat->setStyleSheet("color: #c0392b; font-size: 13px; font-weight: bold;");
        QString cause;
        switch (m_sim->getCauseDestruction()) {
            case CauseDestruction::DECROCHAGE:        cause = "DÉCROCHAGE";       break;
            case CauseDestruction::AVANT_PISTE:       cause = "AVANT PISTE";      break;
            case CauseDestruction::APRES_PISTE:       cause = "APRÈS PISTE";      break;
            case CauseDestruction::DEPASSEMENT_PISTE: cause = "DÉPASSEMENT";      break;
            case CauseDestruction::PANNE_SECHE:       cause = "PANNE SÈCHE";      break;
            default:                                   cause = "INCONNU";
        }
        m_lblEtat->setText(QString("DÉTRUIT — %1").arg(cause));
        break;
    }
    }
}

QString TableauDeBord::formaterTemps(qint64 ms) const
{
    qint64 secondes = ms / 1000;
    qint64 minutes  = secondes / 60;
    qint64 heures   = minutes  / 60;
    secondes %= 60;
    minutes  %= 60;
    return QString("%1:%2:%3")
        .arg(heures,   2, 10, QChar('0'))
        .arg(minutes,  2, 10, QChar('0'))
        .arg(secondes, 2, 10, QChar('0'));
}