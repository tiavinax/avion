#include <QApplication>
#include "ui/FormulaireWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Point d'entrée : on affiche le formulaire
    FormulaireWindow fenetre;
    fenetre.show();

    return app.exec();  // boucle d'événements Qt
}