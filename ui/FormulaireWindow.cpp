#include "FormulaireWindow.h"
#include "SimulationWindow.h"
#include "persistence/DataManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QGroupBox>

FormulaireWindow::FormulaireWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Simulation Atterrissage — Paramètres");
    setMinimumWidth(500);
    construireUI();
}

void FormulaireWindow::construireUI()
{
    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);
    
    // ──────────────────────────────────────────────────────────────
    // SECTION 1 : PARAMÈTRES DE VOL (existants)
    // ──────────────────────────────────────────────────────────────
    QGroupBox* grpVol = new QGroupBox("Paramètres de vol");
    QFormLayout* form = new QFormLayout(grpVol);
    
    m_champNom = new QLineEdit("Air Test");
    m_champAltitude = new QLineEdit("1000");
    m_champDistancePiste = new QLineEdit("-5000");   // NOUVEAU
    m_champVitesseX = new QLineEdit("400");
    m_champVitesseY = new QLineEdit("-50");
    m_champDecrochage = new QLineEdit("300");
    m_champAcceleration = new QLineEdit("10");
    m_champGammaX = new QLineEdit("40");
    m_champGammaY = new QLineEdit("60");
    
    form->addRow("Nom avion :", m_champNom);
    form->addRow("Altitude initiale (m) :", m_champAltitude);
    form->addRow("Distance initiale (m) :", m_champDistancePiste);  // NOUVEAU
    form->addRow("Vitesse X (km/h) :", m_champVitesseX);
    form->addRow("Vitesse Y (km/h) :", m_champVitesseY);
    form->addRow("Vitesse décrochage (km/h):", m_champDecrochage);
    form->addRow("Accélération (m/s²) :", m_champAcceleration);
    form->addRow("Gamma X max (m/s) :", m_champGammaX);
    form->addRow("Gamma Y max (m/s) :", m_champGammaY);
    
    // ──────────────────────────────────────────────────────────────
    // SECTION 2 : CARBURANT (NOUVEAU)
    // ──────────────────────────────────────────────────────────────
    QGroupBox* grpCarburant = new QGroupBox("Carburant");
    QFormLayout* formCarb = new QFormLayout(grpCarburant);
    
    // Capacité avec choix unité
    QHBoxLayout* layoutCapacite = new QHBoxLayout();
    m_champCapaciteCarburant = new QLineEdit("5000");
    m_comboUniteVolume = new QComboBox();
    m_comboUniteVolume->addItem("Litres (L)", static_cast<int>(UniteVolume::LITRES));
    m_comboUniteVolume->addItem("Mètres cubes (m³)", static_cast<int>(UniteVolume::METRES_CUBES));
    m_comboUniteVolume->setCurrentIndex(0);  // Litres par défaut
    layoutCapacite->addWidget(m_champCapaciteCarburant);
    layoutCapacite->addWidget(m_comboUniteVolume);
    
    // Consommation avec choix unité
    QHBoxLayout* layoutConsommation = new QHBoxLayout();
    m_champConsommation = new QLineEdit("10");
    m_comboUniteDebit = new QComboBox();
    m_comboUniteDebit->addItem("L/s", static_cast<int>(UniteDebit::L_S));
    m_comboUniteDebit->addItem("L/min", static_cast<int>(UniteDebit::L_MIN));
    m_comboUniteDebit->addItem("L/h", static_cast<int>(UniteDebit::L_H));
    m_comboUniteDebit->addItem("m³/s", static_cast<int>(UniteDebit::M3_S));
    m_comboUniteDebit->addItem("m³/min", static_cast<int>(UniteDebit::M3_MIN));
    m_comboUniteDebit->addItem("m³/h", static_cast<int>(UniteDebit::M3_H));
    m_comboUniteDebit->setCurrentIndex(1);  // L/min par défaut
    layoutConsommation->addWidget(m_champConsommation);
    layoutConsommation->addWidget(m_comboUniteDebit);
    
    formCarb->addRow("Capacité :", layoutCapacite);
    formCarb->addRow("Consommation :", layoutConsommation);
    
    // ──────────────────────────────────────────────────────────────
    // BOUTON VALIDER ET ERREUR
    // ──────────────────────────────────────────────────────────────
    m_labelErreur = new QLabel("");
    m_labelErreur->setStyleSheet("color: red;");
    m_labelErreur->hide();
    
    m_boutonValider = new QPushButton("VALIDER ET DÉMARRER");
    m_boutonValider->setStyleSheet("background-color: #2ecc71; color: white;"
                                   "font-weight: bold; padding: 8px;");
    connect(m_boutonValider, &QPushButton::clicked, this, &FormulaireWindow::onValider);
    
    // ──────────────────────────────────────────────────────────────
    // ASSEMBLAGE FINAL
    // ──────────────────────────────────────────────────────────────
    layoutPrincipal->addWidget(grpVol);
    layoutPrincipal->addWidget(grpCarburant);
    layoutPrincipal->addWidget(m_labelErreur);
    layoutPrincipal->addWidget(m_boutonValider);
    setLayout(layoutPrincipal);
}

// ─────────────────────────────────────────────────────────────────────────────
// CONVERSIONS POUR LE CARBURANT
// ─────────────────────────────────────────────────────────────────────────────
double FormulaireWindow::convertirVersM3(double valeur, int indexUnite)
{
    UniteVolume unite = static_cast<UniteVolume>(indexUnite);
    if (unite == UniteVolume::LITRES) {
        return valeur / 1000.0;      // Litres → m³
    }
    return valeur;                    // déjà en m³
}

double FormulaireWindow::convertirVersM3ParSeconde(double valeur, int indexUnite)
{
    UniteDebit unite = static_cast<UniteDebit>(indexUnite);
    switch (unite) {
        case UniteDebit::L_S:
            return valeur / 1000.0;                           // L/s → m³/s
        case UniteDebit::L_MIN:
            return valeur / 1000.0 / 60.0;                    // L/min → m³/s
        case UniteDebit::L_H:
            return valeur / 1000.0 / 3600.0;                  // L/h → m³/s
        case UniteDebit::M3_S:
            return valeur;                                    // m³/s direct
        case UniteDebit::M3_MIN:
            return valeur / 60.0;                             // m³/min → m³/s
        case UniteDebit::M3_H:
            return valeur / 3600.0;                           // m³/h → m³/s
        default:
            return valeur / 1000.0 / 60.0;                    // défaut L/min
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// VALIDATION DES CHAMPS
// ─────────────────────────────────────────────────────────────────────────────
bool FormulaireWindow::validerChamps()
{
    // Nom non vide
    if (m_champNom->text().trimmed().isEmpty()) {
        m_labelErreur->setText("Le nom de l'avion est obligatoire.");
        m_labelErreur->show();
        return false;
    }
    
    // Vérification des champs numériques existants
    QList<QLineEdit*> champsNumeriques = {
        m_champAltitude, m_champDistancePiste, m_champVitesseX, m_champVitesseY,
        m_champDecrochage, m_champAcceleration, m_champGammaX, m_champGammaY,
        m_champCapaciteCarburant, m_champConsommation
    };
    
    bool ok;
    for (QLineEdit* champ : champsNumeriques) {
        champ->text().toDouble(&ok);
        if (!ok) {
            m_labelErreur->setText("Tous les champs numériques doivent être des nombres valides.");
            m_labelErreur->show();
            return false;
        }
    }
    
    // Vitesse décrochage positive
    if (m_champDecrochage->text().toDouble() <= 0) {
        m_labelErreur->setText("La vitesse de décrochage doit être positive.");
        m_labelErreur->show();
        return false;
    }
    
    // Capacité carburant positive
    if (m_champCapaciteCarburant->text().toDouble() <= 0) {
        m_labelErreur->setText("La capacité carburant doit être positive.");
        m_labelErreur->show();
        return false;
    }
    
    // Consommation positive
    if (m_champConsommation->text().toDouble() <= 0) {
        m_labelErreur->setText("La consommation doit être positive.");
        m_labelErreur->show();
        return false;
    }
    
    m_labelErreur->hide();
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// VALIDATION ET LANCEMENT
// ─────────────────────────────────────────────────────────────────────────────
void FormulaireWindow::onValider()
{
    if (!validerChamps()) return;
    
    // ── Récupération des valeurs existantes ──────────────────────
    QString nom = m_champNom->text().trimmed();
    double vx = m_champVitesseX->text().toDouble();
    double vy = m_champVitesseY->text().toDouble();
    double acc = m_champAcceleration->text().toDouble();
    double vd = m_champDecrochage->text().toDouble();
    double alt = m_champAltitude->text().toDouble();
    double dist = m_champDistancePiste->text().toDouble();  // maintenant modifiable
    double gx = m_champGammaX->text().toDouble();
    double gy = m_champGammaY->text().toDouble();
    
    // ── Récupération et conversion carburant ─────────────────────
    double capaciteValeur = m_champCapaciteCarburant->text().toDouble();
    int uniteVolumeIndex = m_comboUniteVolume->currentData().toInt();
    double capacite_m3 = convertirVersM3(capaciteValeur, uniteVolumeIndex);
    
    double consoValeur = m_champConsommation->text().toDouble();
    int uniteDebitIndex = m_comboUniteDebit->currentData().toInt();
    double consommation_m3_s = convertirVersM3ParSeconde(consoValeur, uniteDebitIndex);
    
    // Préférences d'affichage
    UniteVolume uniteVolumePref = static_cast<UniteVolume>(uniteVolumeIndex);
    UniteDebit uniteDebitPref = static_cast<UniteDebit>(uniteDebitIndex);
    
    // ── Création de l'avion et sauvegarde ────────────────────────
    Avion* avion = new Avion(nom, vx, vy, acc, vd, alt, dist, gx, gy,
                             capacite_m3, consommation_m3_s,
                             uniteVolumePref, uniteDebitPref);
    
    ParametresFreinage* freinage = new ParametresFreinage(gx, gy);
    
    if (!DataManager::sauvegarder(*avion, *freinage)) {
        m_labelErreur->setText("Erreur : impossible d'écrire data.txt");
        m_labelErreur->show();
        delete avion;
        delete freinage;
        return;
    }
    
    // ── Ouverture de la fenêtre de simulation ────────────────────
    SimulationWindow* simWin = new SimulationWindow(avion, freinage);
    simWin->show();
    this->close();
}