#ifndef FORMULAIREWINDOW_H
#define FORMULAIREWINDOW_H

#include <QWidget>      // classe de base pour toute fenêtre Qt
#include <QLineEdit>    // champ de saisie texte (équivalent JTextField)
#include <QLabel>       // texte non éditable
#include <QPushButton>  // bouton cliquable

// Fenêtre du formulaire de saisie des paramètres de vol
// Hérite de QWidget → c'est une vraie fenêtre affichable
class FormulaireWindow : public QWidget
{
    Q_OBJECT  // obligatoire pour les signaux/slots

public:
    explicit FormulaireWindow(QWidget* parent = nullptr);

private slots:
    // Appelé quand l'utilisateur clique sur VALIDER
    void onValider();

private:
    // ── Champs de saisie ──────────────────────────────────────────
    QLineEdit* m_champNom;
    QLineEdit* m_champAltitude;
    QLineEdit* m_champVitesseX;
    QLineEdit* m_champVitesseY;
    QLineEdit* m_champDecrochage;
    QLineEdit* m_champAcceleration;
    QLineEdit* m_champGammaX;
    QLineEdit* m_champGammaY;

    QPushButton* m_boutonValider;
    QLabel*      m_labelErreur;   // affiche les messages d'erreur en rouge

    // ── Méthodes internes ─────────────────────────────────────────
    void construireUI();    // construit et place tous les widgets
    bool validerChamps();   // vérifie que les saisies sont correctes
};

#endif // FORMULAIREWINDOW_H