#include "FormulaireWindow.h"
#include "SimulationWindow.h"
#include "persistence/DataManager.h"

#include <QVBoxLayout> // dispose les widgets verticalement
#include <QHBoxLayout> // dispose les widgets horizontalement
#include <QFormLayout> // disposition étiquette | champ (idéal pour formulaire)
#include <QMessageBox> // boîte de dialogue pour les erreurs

FormulaireWindow::FormulaireWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Simulation Atterrissage — Paramètres");
    setMinimumWidth(400);
    construireUI();
}

void FormulaireWindow::construireUI()
{
    // QFormLayout : chaque ligne = un label à gauche + un champ à droite
    QFormLayout *form = new QFormLayout();

    // Crée chaque champ avec sa valeur par défaut
    m_champNom = new QLineEdit("Air Test");
    m_champAltitude = new QLineEdit("1000");
    m_champVitesseX = new QLineEdit("400");
    m_champVitesseY = new QLineEdit("-50");
    m_champDecrochage = new QLineEdit("300");
    m_champAcceleration = new QLineEdit("10");
    m_champGammaX = new QLineEdit("40");
    m_champGammaY = new QLineEdit("60");

    // Ajoute chaque ligne : label → champ
    form->addRow("Nom avion :", m_champNom);
    form->addRow("Altitude initiale (m) :", m_champAltitude);
    form->addRow("Vitesse X (km/h) :", m_champVitesseX);
    form->addRow("Vitesse Y (km/h) :", m_champVitesseY);
    form->addRow("Vitesse décrochage (km/h):", m_champDecrochage);
    form->addRow("Accélération (m/s²) :", m_champAcceleration);
    form->addRow("Gamma X max (m/s) :", m_champGammaX);
    form->addRow("Gamma Y max (m/s) :", m_champGammaY);

    // Label d'erreur — caché par défaut, rouge quand visible
    m_labelErreur = new QLabel("");
    m_labelErreur->setStyleSheet("color: red;");
    m_labelErreur->hide();

    // Bouton VALIDER
    m_boutonValider = new QPushButton("VALIDER ET DÉMARRER");
    m_boutonValider->setStyleSheet("background-color: #2ecc71; color: white;"
                                   "font-weight: bold; padding: 8px;");

    // Connexion : clic sur bouton → slot onValider()
    connect(m_boutonValider, &QPushButton::clicked, this, &FormulaireWindow::onValider);

    // Layout principal : formulaire + erreur + bouton empilés verticalement
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(m_labelErreur);
    layout->addWidget(m_boutonValider);
    setLayout(layout);
}

bool FormulaireWindow::validerChamps()
{
    // Vérifie que le nom n'est pas vide
    if (m_champNom->text().trimmed().isEmpty())
    {
        m_labelErreur->setText("Le nom de l'avion est obligatoire.");
        m_labelErreur->show();
        return false;
    }

    // Vérifie que les champs numériques sont bien des nombres
    bool ok;
    QList<QLineEdit *> champsNumeriques = {
        m_champAltitude, m_champVitesseX, m_champVitesseY,
        m_champDecrochage, m_champAcceleration,
        m_champGammaX, m_champGammaY};

    for (QLineEdit *champ : champsNumeriques)
    {
        champ->text().toDouble(&ok);
        if (!ok)
        {
            m_labelErreur->setText("Tous les champs numériques doivent être des nombres valides.");
            m_labelErreur->show();
            return false;
        }
    }

    // Vérifie que la vitesse de décrochage est positive
    if (m_champDecrochage->text().toDouble() <= 0)
    {
        m_labelErreur->setText("La vitesse de décrochage doit être positive.");
        m_labelErreur->show();
        return false;
    }

    m_labelErreur->hide();
    return true;
}

void FormulaireWindow::onValider()
{
    // Étape 1 : validation des champs
    if (!validerChamps())
        return;

    // Étape 2 : récupère les valeurs saisies
    QString nom = m_champNom->text().trimmed();
    double vx = m_champVitesseX->text().toDouble();
    double vy = m_champVitesseY->text().toDouble();
    double acc = m_champAcceleration->text().toDouble();
    double vd = m_champDecrochage->text().toDouble();
    double alt = m_champAltitude->text().toDouble();
    double dist = -5000.0; // valeur fixe du cahier des charges
    double gx = m_champGammaX->text().toDouble();
    double gy = m_champGammaY->text().toDouble();

    // Étape 3 : crée les entités et sauvegarde dans data.txt
    Avion *avion = new Avion(nom, vx, vy, acc, vd, alt, dist, gx, gy);
    ParametresFreinage *freinage = new ParametresFreinage(gx, gy);

    if (!DataManager::sauvegarder(*avion, *freinage))
    {
        m_labelErreur->setText("Erreur : impossible d'écrire data.txt");
        m_labelErreur->show();
        delete avion;
        delete freinage;
        return;
    }

    // Étape 4 : ouvre la fenêtre de simulation et ferme le formulaire
    SimulationWindow *simWin = new SimulationWindow(avion, freinage);
    simWin->show();
    this->close(); // ferme le formulaire
}