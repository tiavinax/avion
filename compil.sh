#!/bin/bash
# Créer le dossier build s'il n'existe pas et y entrer
mkdir -p build && cd build

# Nettoyer les anciens fichiers de build pour éviter les conflits
rm -rf *

# Générer le Makefile et compiler
qmake ../simulation_atterrissage.pro
make
