#include "SimulationWindow.h"
#include "persistence/DataManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QFont>

// ── Style des boutons ─────────────────────────────────────────────────────────
static const QString STYLE_BTN_VITESSE =
    "background-color: #1e5f8e; color: white; font-weight: bold;"
    "padding: 10px 18px; border-radius: 4px; font-size: 13px;";

static const QString STYLE_BTN_BLOQUE =
    "background-color: #c0392b; color: white; font-weight: bold;"
    "padding: 10px 18px; border-radius: 4px; font-size: 13px;";

static const QString STYLE_BTN_BASCULE =
    "background-color: #2c3e50; color: #aaaaaa; font-weight: bold;"
    "padding: 6px 14px; border-radius: 4px; border: 1px solid #444;";

static const QString STYLE_BTN_BASCULE_ACTIF =
    "background-color: #1a6b8a; color: white; font-weight: bold;"
    "padding: 6px 14px; border-radius: 4px; border: 1px solid #2980b9;";

SimulationWindow::SimulationWindow(Avion* avion, ParametresFreinage* freinage,
                                   QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle("Simulation Atterrissage");
    resize(1200, 650);

    // Sauvegarde des paramètres initiaux pour le RESTART
    m_nomInitial  = avion->getNom();
    m_vxInitial   = avion->getVitesseX_kmh();
    m_vyInitial   = avion->getVitesseY_kmh();
    m_accInitial  = avion->getAcceleration_ms2();
    m_vdInitial   = avion->getVitesseDecrochage_kmh();
    m_altInitial  = avion->getAltitude();
    m_distInitial = avion->getDistancePiste();
    m_gxInitial   = avion->getGammaX_ms();
    m_gyInitial   = avion->getGammaY_ms();
    
    // NOUVEAUX : sauvegarde paramètres carburant
    m_capaciteCarburantInitial_m3 = avion->getCapaciteEnM3();
    m_consommationInitial_m3_s    = avion->getConsommation_m3_s();
    m_uniteVolumeInitial          = avion->getUniteVolumePreferee();
    m_uniteDebitInitial           = avion->getUniteDebitPreferee();

    Piste* piste = new Piste();
    m_sim    = new Simulation(avion, piste, freinage, this);
    m_zone   = new ZoneSimulation(m_sim, this);
    m_tableau = new TableauDeBord(m_sim, this);

    construireUI();

    connect(m_sim, &Simulation::simulationMiseAJour,
            m_zone,    &ZoneSimulation::rafraichir);
    connect(m_sim, &Simulation::simulationMiseAJour,
            m_tableau, &TableauDeBord::rafraichir);
    connect(m_sim, &Simulation::simulationTerminee,
            this,  &SimulationWindow::onSimulationTerminee);
            
            // Dans le constructeur, après création de m_sim
    connect(m_sim, &Simulation::simulationMiseAJour, this, [this]() {
    if (m_sim->getEtat() == EtatSimulation::EN_CHUTE) {
        activerControles(false);
        m_btnStart->setEnabled(false);
        m_btnPause->setEnabled(false);
        // Afficher message "CHUTE EN COURS..." si voulu
       }
    });
}

void SimulationWindow::construireUI()
{
    setStyleSheet("background-color: #0d1117; color: #e0e0e0;");

    // ── Sélecteur de vue ──────────────────────────────────────────
    m_comboVue = new QComboBox();
    m_comboVue->addItem("Vue Gauche");
    m_comboVue->addItem("Vue Droite");
    m_comboVue->addItem("Vue Arrière");
    m_comboVue->setStyleSheet("background-color: #1e2530; color: white;"
                              "padding: 4px; border: 1px solid #333;");
    connect(m_comboVue, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SimulationWindow::onChangerVue);

    // ── Boutons principaux ────────────────────────────────────────
    m_btnStart = new QPushButton("▶  START");
    m_btnStart->setStyleSheet("background-color: #2ecc71; color: white;"
                              "font-weight: bold; padding: 8px 20px; border-radius: 4px;");
    connect(m_btnStart, &QPushButton::clicked, this, &SimulationWindow::onStart);

    m_btnPause = new QPushButton("⏸  PAUSE");
    m_btnPause->setEnabled(false);
    m_btnPause->setStyleSheet("background-color: #e67e22; color: white;"
                              "font-weight: bold; padding: 8px 20px; border-radius: 4px;");
    connect(m_btnPause, &QPushButton::clicked, this, &SimulationWindow::onPauseReprendre);

    m_btnRestart = new QPushButton("↺  RESTART");
    m_btnRestart->setStyleSheet("background-color: #8e44ad; color: white;"
                                "font-weight: bold; padding: 8px 20px; border-radius: 4px;");
    connect(m_btnRestart, &QPushButton::clicked, this, &SimulationWindow::onRestart);

    // ── Barre haute ───────────────────────────────────────────────
    QHBoxLayout* barreHaut = new QHBoxLayout();
    barreHaut->addWidget(m_comboVue);
    barreHaut->addSpacing(10);
    barreHaut->addWidget(m_btnStart);
    barreHaut->addWidget(m_btnPause);
    barreHaut->addWidget(m_btnRestart);
    barreHaut->addStretch();

    // ── Boutons vitesse ───────────────────────────────────────────
    m_btnVxPlus  = new QPushButton("Vx  +");
    m_btnVxMoins = new QPushButton("Vx  −");
    m_btnVyPlus  = new QPushButton("Vy  +");
    m_btnVyMoins = new QPushButton("Vy  −");

    m_btnVxPlus->setStyleSheet(STYLE_BTN_VITESSE);
    m_btnVxMoins->setStyleSheet(STYLE_BTN_VITESSE);
    m_btnVyPlus->setStyleSheet(STYLE_BTN_VITESSE);
    m_btnVyMoins->setStyleSheet(STYLE_BTN_VITESSE);

    activerControles(false);

    connect(m_btnVxPlus,  &QPushButton::clicked, this, &SimulationWindow::onVxPlus);
    connect(m_btnVxMoins, &QPushButton::clicked, this, &SimulationWindow::onVxMoins);
    connect(m_btnVyPlus,  &QPushButton::clicked, this, &SimulationWindow::onVyPlus);
    connect(m_btnVyMoins, &QPushButton::clicked, this, &SimulationWindow::onVyMoins);

    // Groupes freinage
    QGroupBox* grpX = new QGroupBox("Freinage X");
    grpX->setStyleSheet("QGroupBox { color: #aaa; border: 1px solid #333;"
                        "margin-top: 6px; padding: 4px; }"
                        "QGroupBox::title { subcontrol-origin: margin; left: 8px; }");
    QHBoxLayout* lx = new QHBoxLayout(grpX);
    lx->addWidget(m_btnVxPlus);
    lx->addWidget(m_btnVxMoins);

    QGroupBox* grpY = new QGroupBox("Freinage Y");
    grpY->setStyleSheet(grpX->styleSheet());
    QHBoxLayout* ly = new QHBoxLayout(grpY);
    ly->addWidget(m_btnVyPlus);
    ly->addWidget(m_btnVyMoins);

    // ── Boutons bascule modes ─────────────────────────────────────
    m_btnModeFreinage = new QPushButton("Freinage: FLEXIBLE");
    m_btnModeFreinage->setStyleSheet(STYLE_BTN_BASCULE_ACTIF);
    connect(m_btnModeFreinage, &QPushButton::clicked,
            this, &SimulationWindow::onToggleModeFreinage);

    m_btnModeDecrochage = new QPushButton("Décrochage: Vx");
    m_btnModeDecrochage->setStyleSheet(STYLE_BTN_BASCULE_ACTIF);
    connect(m_btnModeDecrochage, &QPushButton::clicked,
            this, &SimulationWindow::onToggleModeDecrochage);

    // ── Barre des contrôles ───────────────────────────────────────
    QHBoxLayout* barreControles = new QHBoxLayout();
    barreControles->addWidget(grpX);
    barreControles->addWidget(grpY);
    barreControles->addSpacing(20);
    barreControles->addWidget(m_btnModeFreinage);
    barreControles->addWidget(m_btnModeDecrochage);
    barreControles->addStretch();

    // ── Message final ─────────────────────────────────────────────
    m_lblMessage = new QLabel("");
    m_lblMessage->setAlignment(Qt::AlignCenter);
    m_lblMessage->setFont(QFont("Arial", 16, QFont::Bold));
    m_lblMessage->hide();

    // ── Zone + tableau ────────────────────────────────────────────
    QHBoxLayout* contenu = new QHBoxLayout();
    contenu->addWidget(m_zone,    4);
    contenu->addWidget(m_tableau, 1);

    // ── Layout principal ──────────────────────────────────────────
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(barreHaut);
    layout->addLayout(barreControles);
    layout->addWidget(m_lblMessage);
    layout->addLayout(contenu);
    setLayout(layout);
}

// ── Slots ─────────────────────────────────────────────────────────────────────

void SimulationWindow::onStart()
{
    m_sim->demarrer();
    m_btnStart->setEnabled(false);
    m_btnPause->setEnabled(true);
    activerControles(true);
    m_tableau->rafraichir();
}

void SimulationWindow::onPauseReprendre()
{
    if (m_sim->getEtat() == EtatSimulation::EN_COURS) {
        m_sim->pause();
        m_btnPause->setText("▶  REPRENDRE");
        activerControles(false);
    } else if (m_sim->getEtat() == EtatSimulation::EN_PAUSE) {
        m_sim->reprendre();
        m_btnPause->setText("⏸  PAUSE");
        activerControles(true);
    }
    m_tableau->rafraichir();
}

void SimulationWindow::onRestart()
{
    // Recharge les paramètres initiaux (avec carburant)
    m_sim->restart(m_nomInitial, m_vxInitial, m_vyInitial, m_accInitial,
                   m_vdInitial, m_altInitial, m_distInitial,
                   m_gxInitial, m_gyInitial,
                   m_capaciteCarburantInitial_m3, m_consommationInitial_m3_s,
                   m_uniteVolumeInitial, m_uniteDebitInitial);

    // Remet l'UI à l'état initial
    m_btnStart->setEnabled(true);
    m_btnPause->setEnabled(false);
    m_btnPause->setText("⏸  PAUSE");
    activerControles(false);
    resetStyleBoutons();
    m_lblMessage->hide();
    m_tableau->rafraichir();
    m_zone->rafraichir();
}

void SimulationWindow::onVxPlus()
{
    m_sim->getAvion()->augmenterVx(m_sim->getModeFreinage());
    m_btnVxMoins->setStyleSheet(STYLE_BTN_VITESSE);
    m_tableau->rafraichir();
    m_zone->rafraichir();
}

void SimulationWindow::onVxMoins()
{
    bool ok = m_sim->getAvion()->diminuerVx(m_sim->getModeFreinage());
    if (!ok)
        m_btnVxMoins->setStyleSheet(STYLE_BTN_BLOQUE);
    else
        m_btnVxMoins->setStyleSheet(STYLE_BTN_VITESSE);
    m_tableau->rafraichir();
    m_zone->rafraichir();
}

void SimulationWindow::onVyPlus()
{
    m_sim->getAvion()->augmenterVy(m_sim->getModeFreinage());
    m_btnVyMoins->setStyleSheet(STYLE_BTN_VITESSE);
    m_tableau->rafraichir();
    m_zone->rafraichir();
}

void SimulationWindow::onVyMoins()
{
    bool ok = m_sim->getAvion()->diminuerVy(m_sim->getModeFreinage());
    if (!ok)
        m_btnVyMoins->setStyleSheet(STYLE_BTN_BLOQUE);
    else
        m_btnVyMoins->setStyleSheet(STYLE_BTN_VITESSE);
    m_tableau->rafraichir();
    m_zone->rafraichir();
}

void SimulationWindow::onToggleModeFreinage()
{
    if (m_sim->getModeFreinage() == ModeFreinage::FLEXIBLE) {
        m_sim->setModeFreinage(ModeFreinage::ABSOLU);
        m_btnModeFreinage->setText("Freinage: ABSOLU");
        m_btnModeFreinage->setStyleSheet(STYLE_BTN_BASCULE);
    } else {
        m_sim->setModeFreinage(ModeFreinage::FLEXIBLE);
        m_btnModeFreinage->setText("Freinage: FLEXIBLE");
        m_btnModeFreinage->setStyleSheet(STYLE_BTN_BASCULE_ACTIF);
    }
}

void SimulationWindow::onToggleModeDecrochage()
{
    if (m_sim->getModeDecrochage() == ModeDecrochage::VX_SEULE) {
        m_sim->setModeDecrochage(ModeDecrochage::NORME);
        m_btnModeDecrochage->setText("Décrochage: Norme");
        m_btnModeDecrochage->setStyleSheet(STYLE_BTN_BASCULE);
    } else {
        m_sim->setModeDecrochage(ModeDecrochage::VX_SEULE);
        m_btnModeDecrochage->setText("Décrochage: Vx");
        m_btnModeDecrochage->setStyleSheet(STYLE_BTN_BASCULE_ACTIF);
    }
}

void SimulationWindow::onSimulationTerminee(EtatSimulation etat, CauseDestruction cause)
{
    activerControles(false);
    m_btnStart->setEnabled(false);
    m_btnPause->setEnabled(false);
    m_btnRestart->setEnabled(true);

    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Fin de simulation");
    dialog->setMinimumWidth(350);

    QString message;
    QString couleur;

    if (etat == EtatSimulation::REUSSI) {
        message = "✓  ATTERRISSAGE RÉUSSI !";
        couleur = "#2ecc71";
    } else {
        QString causeStr;
        switch (cause) {
            case CauseDestruction::DECROCHAGE:
                causeStr = "DÉCROCHAGE — vitesse insuffisante"; break;
            case CauseDestruction::AVANT_PISTE:
                causeStr = "ATTERRISSAGE AVANT LA PISTE";       break;
            case CauseDestruction::APRES_PISTE:
                causeStr = "ATTERRISSAGE APRÈS LA PISTE";       break;
            case CauseDestruction::DEPASSEMENT_PISTE:
                causeStr = "DÉPASSEMENT DE PISTE";              break;
            case CauseDestruction::PANNE_SECHE:                 // NOUVEAU
                causeStr = "PANNE SÈCHE — plus de carburant";   break;
            default:
                causeStr = "CAUSE INCONNUE";
        }
        message = QString("✗  AVION DÉTRUIT\n%1").arg(causeStr);
        couleur = "#e74c3c";
    }

    QLabel* lblMsg = new QLabel(message);
    lblMsg->setAlignment(Qt::AlignCenter);
    lblMsg->setFont(QFont("Arial", 14, QFont::Bold));
    lblMsg->setStyleSheet(QString("color: %1;").arg(couleur));
    lblMsg->setWordWrap(true);

    QDialogButtonBox* btns = new QDialogButtonBox();
    QPushButton* btnOk      = btns->addButton("OK",      QDialogButtonBox::AcceptRole);
    QPushButton* btnRestart = btns->addButton("↺ RESTART", QDialogButtonBox::ResetRole);

    connect(btnOk, &QPushButton::clicked, dialog, &QDialog::accept);
    connect(btnRestart, &QPushButton::clicked, [this, dialog]() {
        dialog->accept();
        onRestart();
    });

    QVBoxLayout* dlayout = new QVBoxLayout(dialog);
    dlayout->addWidget(lblMsg);
    dlayout->addSpacing(10);
    dlayout->addWidget(btns);
    dialog->setLayout(dlayout);

    m_lblMessage->setStyleSheet(QString("color: %1;").arg(couleur));
    m_lblMessage->setText(message.replace("\n", " — "));
    m_lblMessage->show();

    m_tableau->rafraichir();
    m_zone->rafraichir();

    dialog->exec();
}

void SimulationWindow::onChangerVue(int index)
{
    if (index == 0) m_zone->setVue(Vue::GAUCHE);
    if (index == 1) m_zone->setVue(Vue::DROITE);
    if (index == 2) m_zone->setVue(Vue::ARRIERE);
}

void SimulationWindow::activerControles(bool actif)
{
    m_btnVxPlus->setEnabled(actif);
    m_btnVxMoins->setEnabled(actif);
    m_btnVyPlus->setEnabled(actif);
    m_btnVyMoins->setEnabled(actif);
}

void SimulationWindow::resetStyleBoutons()
{
    m_btnVxMoins->setStyleSheet(STYLE_BTN_VITESSE);
    m_btnVyMoins->setStyleSheet(STYLE_BTN_VITESSE);
}