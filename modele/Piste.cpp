#include "Piste.h"

Piste::Piste(double longueur)
    : m_longueur(longueur)
{}

bool Piste::contientPosition(double distance) const
{
    // L'avion est sur la piste si sa distance est entre 0 et la fin
    return distance >= 0.0 && distance <= m_longueur;
}