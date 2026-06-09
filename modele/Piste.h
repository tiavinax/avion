#ifndef PISTE_H
#define PISTE_H

// Représente la piste d'atterrissage
// La piste commence à distance = 0 et se termine à distance = longueur
class Piste
{
public:
    // Constructeur — longueur par défaut 7000 m
    Piste(double longueur = 7000.0);

    double getLongueur() const { return m_longueur; }

    // Retourne true si la position donnée est sur la piste
    // Condition : 0 <= distance <= longueur
    bool contientPosition(double distance) const;

private:
    double m_longueur;  // longueur de la piste en mètres
};

#endif // PISTE_H