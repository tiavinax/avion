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
    setStyleSheet("background-color: #0d1117; color: #e0e0e0;");
    setMinimumWidth(280);

    auto creerLigne = [](QGridLayout *grid, int ligne,
                         const QString &titre, QLabel *&labelValeur)
    {
        QLabel *lblTitre = new QLabel(titre);
        lblTitre->setStyleSheet("color: #888888; font-size: 10px;");

        labelValeur = new QLabel("---");
        labelValeur->setStyleSheet("color: #00ff99; font-size: 13px;"
                                   "font-weight: bold;");
        grid->addWidget(lblTitre, ligne, 0);
        grid->addWidget(labelValeur, ligne, 1);
    };

    // ── Groupe : Position ─────────────────────────────────────────
    QGroupBox *grpPosition = new QGroupBox("Position");
    QGridLayout *gridPos = new QGridLayout(grpPosition);
    grpPosition->setStyleSheet("QGroupBox { color: #aaaaaa; "
                               "border: 1px solid #333; margin-top: 6px; }"
                               "QGroupBox::title { subcontrol-origin: margin; "
                               "left: 8px; }");

    creerLigne(gridPos, 0, "Altitude (m)", m_lblAltitude);
    creerLigne(gridPos, 1, "Distance piste (m)", m_lblDistancePiste);

    // ── Groupe : Vitesses ─────────────────────────────────────────
    QGroupBox *grpVitesse = new QGroupBox("Vitesses");
    QGridLayout *gridVit = new QGridLayout(grpVitesse);
    grpVitesse->setStyleSheet(grpPosition->styleSheet());

    creerLigne(gridVit, 0, "Vx (km/h | m/s)", m_lblVitesseX);
    creerLigne(gridVit, 1, "Vy (km/h | m/s)", m_lblVitesseY);
    creerLigne(gridVit, 2, "Vtotale (m/s)", m_lblVitesseTotale);

    // ── Groupe : Freinage ─────────────────────────────────────────
    QGroupBox *grpFreinage = new QGroupBox("Budget freinage");
    QGridLayout *gridFrn = new QGridLayout(grpFreinage);
    grpFreinage->setStyleSheet(grpPosition->styleSheet());

    creerLigne(gridFrn, 0, "Consommé X / 40 m/s", m_lblFreinageX);
    creerLigne(gridFrn, 1, "Consommé Y / 60 m/s", m_lblFreinageY);

    // ── NOUVEAU : Groupe Carburant ─────────────────────────────────
    QGroupBox *grpCarburant = new QGroupBox("Carburant");
    grpCarburant->setStyleSheet(grpPosition->styleSheet());
    QVBoxLayout *layoutCarb = new QVBoxLayout(grpCarburant);

    // Valeur texte
    m_lblCarburantValeur = new QLabel("---");
    m_lblCarburantValeur->setStyleSheet("color: #ffdd00; font-size: 13px; font-weight: bold;");
    m_lblCarburantValeur->setAlignment(Qt::AlignCenter);

    // Barre de progression
    m_barreCarburant = new QProgressBar();
    m_barreCarburant->setRange(0, 100);
    m_barreCarburant->setValue(100);
    m_barreCarburant->setStyleSheet(
        "QProgressBar {"
        "    background-color: #2a2a2a;"
        "    border: 1px solid #444;"
        "    border-radius: 4px;"
        "    text-align: center;"
        "    color: white;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #f39c12;"
        "    border-radius: 3px;"
        "}");

    // Consommation
    m_lblConsommation = new QLabel("Consommation: ---");
    m_lblConsommation->setStyleSheet("color: #aaaaaa; font-size: 10px;");
    m_lblConsommation->setAlignment(Qt::AlignCenter);

    layoutCarb->addWidget(m_lblCarburantValeur);
    layoutCarb->addWidget(m_barreCarburant);
    layoutCarb->addWidget(m_lblConsommation);

    // ── Groupe : Temps ────────────────────────────────────────────
    QGroupBox *grpTemps = new QGroupBox("Chronomètre");
    QGridLayout *gridTps = new QGridLayout(grpTemps);
    grpTemps->setStyleSheet(grpPosition->styleSheet());

    creerLigne(gridTps, 0, "Temps", m_lblChronometre);
    m_lblChronometre->setStyleSheet("color: #ffdd00; font-size: 15px;"
                                    "font-weight: bold;");

    // ── Groupe : État ─────────────────────────────────────────────
    QGroupBox *grpEtat = new QGroupBox("État");
    QGridLayout *gridEtat = new QGridLayout(grpEtat);
    grpEtat->setStyleSheet(grpPosition->styleSheet());

    creerLigne(gridEtat, 0, "", m_lblEtat);
    m_lblEtat->setStyleSheet("color: #aaaaaa; font-size: 12px;");
    m_lblEtat->setText("En attente...");

    // ── Layout principal ──────────────────────────────────────────
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(grpPosition);
    layout->addWidget(grpVitesse);
    layout->addWidget(grpFreinage);
    layout->addWidget(grpCarburant); // NOUVEAU
    layout->addWidget(grpTemps);
    layout->addWidget(grpEtat);
    layout->addStretch();
    setLayout(layout);
}

void TableauDeBord::rafraichir()
{
    if (!m_sim || !m_sim->getAvion())
        return;

    Avion *avion = m_sim->getAvion();

    // ── Position ──────────────────────────────────────────────────
    m_lblAltitude->setText(
        QString("%1 m").arg(avion->getAltitude(), 0, 'f', 1));

    m_lblDistancePiste->setText(
        QString("%1 m").arg(avion->getDistancePiste(), 0, 'f', 1));

    // ── Vitesses ──────────────────────────────────────────────────
    m_lblVitesseX->setText(
        QString("%1 | %2")
            .arg(avion->getVitesseX_kmh(), 0, 'f', 1)
            .arg(avion->getVitesseX_ms(), 0, 'f', 2));

    m_lblVitesseY->setText(
        QString("%1 | %2")
            .arg(avion->getVitesseY_kmh(), 0, 'f', 1)
            .arg(avion->getVitesseY_ms(), 0, 'f', 2));

    m_lblVitesseTotale->setText(
        QString("%1 m/s").arg(avion->getVitesseTotale_ms(), 0, 'f', 2));

    // ── Budget freinage ───────────────────────────────────────────
    double consX = avion->getFreinageConsommeX_ms();
    double consY = avion->getFreinageConsommeY_ms();

    m_lblFreinageX->setStyleSheet(
        consX >= avion->getGammaX_ms() ? "color: #ff4444; font-weight: bold;"
                                       : "color: #00ff99; font-size: 13px; font-weight: bold;");
    m_lblFreinageY->setStyleSheet(
        consY >= avion->getGammaY_ms() ? "color: #ff4444; font-weight: bold;"
                                       : "color: #00ff99; font-size: 13px; font-weight: bold;");

    m_lblFreinageX->setText(QString("%1 / %2 m/s")
                                .arg(consX, 0, 'f', 1)
                                .arg(avion->getGammaX_ms(), 0, 'f', 0));
    m_lblFreinageY->setText(QString("%1 / %2 m/s")
                                .arg(consY, 0, 'f', 1)
                                .arg(avion->getGammaY_ms(), 0, 'f', 0));

    // ── NOUVEAU : Carburant ───────────────────────────────────────
    double pourcentage = avion->getPourcentageCarburant();
    QString valeurFormatee = avion->getCarburantRestantFormate();
    QString capaciteFormatee = avion->getCapaciteFormatee();

    m_lblCarburantValeur->setText(QString("%1 / %2")
                                      .arg(valeurFormatee)
                                      .arg(capaciteFormatee));

    m_barreCarburant->setValue(static_cast<int>(pourcentage));

    // Changement de couleur selon niveau
    if (pourcentage < 10)
    {
        m_barreCarburant->setStyleSheet(
            "QProgressBar { background-color: #2a2a2a; border: 1px solid #444; border-radius: 4px; text-align: center; color: white; }"
            "QProgressBar::chunk { background-color: #e74c3c; border-radius: 3px; }");
    }
    else if (pourcentage < 25)
    {
        m_barreCarburant->setStyleSheet(
            "QProgressBar { background-color: #2a2a2a; border: 1px solid #444; border-radius: 4px; text-align: center; color: white; }"
            "QProgressBar::chunk { background-color: #f39c12; border-radius: 3px; }");
    }
    else
    {
        m_barreCarburant->setStyleSheet(
            "QProgressBar { background-color: #2a2a2a; border: 1px solid #444; border-radius: 4px; text-align: center; color: white; }"
            "QProgressBar::chunk { background-color: #2ecc71; border-radius: 3px; }");
    }

    m_lblConsommation->setText(QString("Consommation: %1")
                                   .arg(avion->getConsommationFormatee()));

    // ── Chronomètre ───────────────────────────────────────────────
    m_lblChronometre->setText(formaterTemps(m_sim->getTempsEcoule_ms()));

    // ── État ──────────────────────────────────────────────────────
    switch (m_sim->getEtat())
    {
    case EtatSimulation::EN_ATTENTE:
        m_lblEtat->setStyleSheet("color: #aaaaaa; font-size: 12px;");
        m_lblEtat->setText("En attente...");
        break;
    case EtatSimulation::EN_COURS:
        m_lblEtat->setStyleSheet("color: #00ff99; font-size: 12px;");
        m_lblEtat->setText("En cours");
        break;
    case EtatSimulation::EN_PAUSE:
        m_lblEtat->setStyleSheet("color: #ffdd00; font-size: 12px;");
        m_lblEtat->setText("PAUSE");
        break;
    case EtatSimulation::REUSSI:
        m_lblEtat->setStyleSheet("color: #00ff99; font-size: 14px;"
                                 "font-weight: bold;");
        m_lblEtat->setText("ATTERRISSAGE RÉUSSI ✓");
        break;
    case EtatSimulation::EN_CHUTE:
        m_lblEtat->setStyleSheet("color: #ff6600; font-size: 12px; font-weight: bold;");
        m_lblEtat->setText("⚠️ CHUTE EN COURS...");
        break;
    case EtatSimulation::DETRUIT:
    {
        m_lblEtat->setStyleSheet("color: #ff4444; font-size: 14px;"
                                 "font-weight: bold;");
        QString cause;
        switch (m_sim->getCauseDestruction())
        {
        case CauseDestruction::DECROCHAGE:
            cause = "DÉCROCHAGE";
            break;
        case CauseDestruction::AVANT_PISTE:
            cause = "AVANT PISTE";
            break;
        case CauseDestruction::APRES_PISTE:
            cause = "APRÈS PISTE";
            break;
        case CauseDestruction::DEPASSEMENT_PISTE:
            cause = "DÉPASSEMENT";
            break;
        case CauseDestruction::PANNE_SECHE:
            cause = "PANNE SÈCHE";
            break;
        default:
            cause = "INCONNU";
        }
        m_lblEtat->setText(QString("DÉTRUIT — %1").arg(cause));
        break;
    }
    }
}

QString TableauDeBord::formaterTemps(qint64 ms) const
{
    qint64 secondes = ms / 1000;
    qint64 minutes = secondes / 60;
    qint64 heures = minutes / 60;

    secondes %= 60;
    minutes %= 60;

    return QString("%1:%2:%3")
        .arg(heures, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(secondes, 2, 10, QChar('0'));
}