#ifndef PARAMETRESFREINAGE_H
#define PARAMETRESFREINAGE_H

// Contient les budgets de freinage maximum autorisés pour le pilote
// GammaX = 40 km/h : total de diminutions autorisées sur l'axe X
// GammaY = 60 km/h : total de diminutions autorisées sur l'axe Y
class ParametresFreinage
{
public:
    // Constructeur avec valeurs par défaut du cahier des charges
    ParametresFreinage(double gammaX = 40.0, double gammaY = 60.0);

    double getGammaX() const { return m_gammaX; }
    double getGammaY() const { return m_gammaY; }

    void setGammaX(double val) { m_gammaX = val; }
    void setGammaY(double val) { m_gammaY = val; }

private:
    double m_gammaX;  // budget freinage axe X (km/h)
    double m_gammaY;  // budget freinage axe Y (km/h)
};

#endif // PARAMETRESFREINAGE_H