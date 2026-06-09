QT += core gui widgets

CONFIG += c++17

TARGET = simulation_atterrissage
TEMPLATE = app

# Déclaration des dossiers d'inclusion pour éviter les problèmes d'include
INCLUDEPATH += . modele persistence ui

# Fichiers Sources (.cpp)
SOURCES += main.cpp \
           modele/Avion.cpp \
           modele/ParametresFreinage.cpp \
           modele/Piste.cpp \
           modele/Simulation.cpp \
           persistence/DataManager.cpp \
           ui/FormulaireWindow.cpp \
           ui/SimulationWindow.cpp \
           ui/TableauDeBord.cpp \
           ui/ZoneSimulation.cpp

# Fichiers En-têtes (.h)
HEADERS += modele/Avion.h \
           modele/ParametresFreinage.h \
           modele/Piste.h \
           modele/Simulation.h \
           persistence/DataManager.h \
           ui/FormulaireWindow.h \
           ui/SimulationWindow.h \
           ui/TableauDeBord.h \
           ui/ZoneSimulation.h
