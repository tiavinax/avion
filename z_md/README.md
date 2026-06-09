# Cahier des Charges — Simulation d'Atterrissage d'Avion
**Projet** : Simulation Atterrissage  
**Langage** : C++ / Qt5  
**Plateforme** : Ubuntu Linux  
**Version** : 1.0  

---

## Table des matières

1. [Présentation générale](#1-présentation-générale)
2. [Flux général de l'application](#2-flux-général-de-lapplication)
3. [Paramètres de la simulation](#3-paramètres-de-la-simulation)
4. [Règles métier](#4-règles-métier)
5. [Architecture technique](#5-architecture-technique)
6. [Comportements détaillés](#6-comportements-détaillés)
7. [Interface utilisateur](#7-interface-utilisateur)
8. [Checklist d'avancement](#8-checklist-davancement)
9. [Glossaire](#9-glossaire)

---

## 1. Présentation générale

### Contexte
Application de simulation d'atterrissage d'avion développée en C++ avec la bibliothèque Qt5.  
L'avion est déjà en vol au démarrage — il ne décolle pas depuis la piste.  
L'objectif est de contrôler les vitesses de l'avion pour réussir un atterrissage sans destruction.

### Objectif du pilote
Manœuvrer l'avion en ajustant ses vitesses horizontale (Vx) et verticale (Vy) pour :
- Atteindre la piste (distance = 0m à 7000m)
- Toucher le sol sur la piste
- Maintenir une vitesse supérieure au seuil de décrochage pendant tout le vol

### Technologies utilisées
| Composant | Technologie |
|---|---|
| Langage | C++17 |
| Interface graphique | Qt5 Widgets |
| Dessin 2D | QPainter |
| Timer de simulation | QTimer |
| Persistance | Fichier texte `data.txt` |
| Compilation | qmake + make |

---

## 2. Flux général de l'application

```
Démarrage
    ↓
FormulaireWindow — saisie des paramètres
    ↓
Validation + sauvegarde dans data.txt
    ↓
SimulationWindow — fenêtre principale
    ↓
Clic START → simulation active
    ↓
Boucle de simulation (tick toutes les 100ms)
    │
    ├── Mise à jour position (Avion::mettreAJour)
    ├── Évaluation des conditions (Simulation::evaluerConditions)
    ├── Rafraîchissement visuel (ZoneSimulation::rafraichir)
    └── Rafraîchissement tableau de bord (TableauDeBord::rafraichir)
    ↓
Fin : RÉUSSI ou DÉTRUIT
    ↓
QDialog résultat + option RESTART
```

---

## 3. Paramètres de la simulation

### Paramètres de l'avion

| Paramètre | Valeur par défaut | Unité | Description |
|---|---|---|---|
| Nom | Air Test | — | Identifiant de l'avion |
| Vitesse X | 400 | km/h | Vitesse horizontale initiale |
| Vitesse Y | -50 | km/h | Vitesse verticale initiale (négatif = descente) |
| Accélération | 10 | m/s² | Palier appliqué par chaque clic de bouton |
| Vitesse de décrochage | 300 | km/h | Seuil en dessous duquel l'avion décroche |
| Altitude initiale | 1000 | m | Hauteur de départ |
| Distance initiale | -5000 | m | Position avant la piste (fixe, non modifiable) |

### Paramètres de freinage

| Paramètre | Valeur par défaut | Unité | Description |
|---|---|---|---|
| GammaX | 40 | m/s | Budget maximum de freinage sur l'axe X |
| GammaY | 60 | m/s | Budget maximum de freinage sur l'axe Y |

### Paramètres de la piste

| Paramètre | Valeur | Unité | Description |
|---|---|---|---|
| Longueur | 7000 | m | La piste s'étend de 0m à 7000m |

### Paramètre du tick

| Paramètre | Valeur |
|---|---|
| Intervalle | 100 ms |
| dt (delta temps) | 0.1 s |

---

## 4. Règles métier

### 4.1 Physique du mouvement

À chaque tick de simulation (dt = 0.1s) :

```
distancePiste += Vx_ms * dt
altitude      += Vy_ms * dt

Si altitude < 0 → altitude = 0  (sol)
```

Les vitesses sont stockées en **m/s** en interne.  
L'affichage tableau de bord montre les deux unités : **km/h** et **m/s**.

### 4.2 Contrôle des vitesses — Mode Direct (défaut)

Les boutons modifient la vitesse **instantanément** par palier de `acceleration` (10 m/s) :

| Bouton | Effet |
|---|---|
| Vx + | Vx = Vx + 10 m/s |
| Vx − | Vx = Vx − 10 m/s (si budget disponible) |
| Vy + | Vy = Vy + 10 m/s |
| Vy − | Vy = Vy − 10 m/s (si budget disponible) |

### 4.3 Budget de freinage — deux modes

#### Mode Absolu
Le cumul total des diminutions ne peut pas dépasser GammaX (ou GammaY).  
Une fois le budget épuisé, le bouton **−** est refusé même si on a accéléré entre temps.

```
Exemple GammaX = 40 m/s :
Clic - x4 → consommé = 40/40 → BLOQUÉ
Clic +    → Vx remonte MAIS consommé reste 40/40 → toujours BLOQUÉ
```

#### Mode Flexible (défaut)
Le budget est relatif à la **vitesse de référence courante**.  
Si on accélère au-delà de la référence, la référence se met à jour et le budget se recharge.

```
Exemple GammaX = 40 m/s :
Vx = 111 m/s, ref = 111
Clic - x4 → Vx = 71, consommé = 40/40 → BLOQUÉ
Clic +    → Vx = 81, ref reste 111, consommé = 30/40 → DÉBLOQUÉ
Clic -    → Vx = 71 → accepté
```

### 4.4 Décrochage — deux modes

#### Mode Vx seule (défaut)
```
Si Vx < vitesse_decrochage ET avion en vol → DÉCROCHAGE → DÉTRUIT
```

#### Mode Norme
```
Si √(Vx² + Vy²) < vitesse_decrochage ET avion en vol → DÉCROCHAGE → DÉTRUIT
```

> La règle de décrochage est **désactivée** dès que l'avion touche le sol.

### 4.5 Conditions de destruction

| Cas | Condition | Cause |
|---|---|---|
| Décrochage | Vitesse < seuil pendant le vol | `DECROCHAGE` |
| Avant piste | altitude = 0 et distance < 0 | `AVANT_PISTE` |
| Après piste | altitude = 0 et distance > 7000 | `APRES_PISTE` |
| Dépassement | distance >= 7000 sans avoir touché le sol | `DEPASSEMENT_PISTE` |

### 4.6 Condition de réussite

```
altitude = 0
ET 0 ≤ distance ≤ 7000
```

### 4.7 Persistance

Avant la simulation, les paramètres sont sauvegardés dans `data.txt` (format clé=valeur) :

```
nom=Air Test
vitesse_x=400
vitesse_y=-50
acceleration=10
vitesse_decrochage=300
altitude=1000
distance_piste=-5000
gamma_x=40
gamma_y=60
```

La simulation charge ses paramètres depuis ce fichier.

---

## 5. Architecture technique

### 5.1 Structure des fichiers

```
simulation_atterrissage/
│
├── main.cpp                          ← Point d'entrée Qt
├── simulation_atterrissage.pro       ← Configuration qmake
│
├── modele/
│   ├── Avion.h / Avion.cpp           ← État physique + contrôle
│   ├── Piste.h / Piste.cpp           ← Zone d'atterrissage valide
│   ├── ParametresFreinage.h/.cpp     ← Budgets GammaX / GammaY
│   └── Simulation.h / Simulation.cpp ← Chef d'orchestre (QTimer, états)
│
├── persistence/
│   └── DataManager.h / DataManager.cpp ← Lecture/écriture data.txt
│
└── ui/
    ├── FormulaireWindow.h/.cpp       ← Saisie des paramètres
    ├── SimulationWindow.h/.cpp       ← Fenêtre principale
    ├── ZoneSimulation.h/.cpp         ← Rendu graphique QPainter
    └── TableauDeBord.h/.cpp          ← Affichage temps réel
```

### 5.2 Responsabilités des classes

#### `Avion`
- Stocke l'état physique : position, vitesses, altitude
- Calcule le déplacement à chaque tick (`mettreAJour`)
- Gère les budgets de freinage (Absolu / Flexible)
- Détecte le décrochage selon le mode choisi
- Stockage interne en **m/s**, conversion km/h pour l'affichage

#### `Piste`
- Définit la zone valide : 0m à 7000m
- Méthode `contientPosition(distance)` → true/false

#### `ParametresFreinage`
- Conteneur simple pour GammaX et GammaY
- Passé au constructeur de `Avion`

#### `Simulation`
- Possède un `QTimer` qui déclenche `tick()` toutes les 100ms
- Gère les états : `EN_ATTENTE` → `EN_COURS` → `EN_PAUSE` → `REUSSI` / `DETRUIT`
- Évalue les conditions à chaque tick
- Émet des **signaux Qt** vers l'UI :
  - `simulationMiseAJour()` → rafraîchit ZoneSimulation et TableauDeBord
  - `simulationTerminee(etat, cause)` → affiche le QDialog

#### `DataManager`
- Méthode statique `sauvegarder(avion, freinage)` → écrit data.txt
- Méthode statique `charger(...)` → lit data.txt et retourne les valeurs

#### `FormulaireWindow`
- Formulaire de saisie avec valeurs par défaut
- Validation des champs (types, valeurs obligatoires)
- Crée `Avion` et `ParametresFreinage`, sauvegarde, ouvre `SimulationWindow`

#### `SimulationWindow`
- Fenêtre principale de simulation
- Contient `ZoneSimulation` (4/5 largeur) + `TableauDeBord` (1/5 largeur)
- Gère tous les boutons : START, PAUSE, RESTART, Vx+/−, Vy+/−
- Gère les boutons bascule : mode freinage, mode décrochage
- Affiche le QDialog de fin

#### `ZoneSimulation`
- Hérite de `QWidget`, surcharge `paintEvent()`
- Trois modes de rendu selon la vue active :
  - **Vue Gauche** : profil, avion centré, nez vers droite, soleil
  - **Vue Droite** : profil miroir, nez vers gauche
  - **Vue Arrière** : perspective, piste en trapèze devant l'avion
- Avion centré horizontalement — c'est la scène qui défile

#### `TableauDeBord`
- Affiche en temps réel : altitude, distance, Vx, Vy, vitesse totale
- Affiche le budget de freinage consommé (rouge si épuisé)
- Affiche le chronomètre (HH:MM:SS)
- Affiche l'état courant avec couleur (vert=OK, jaune=pause, rouge=détruit)

### 5.3 Relations entre classes

```
FormulaireWindow
    └── crée → Avion, ParametresFreinage
    └── ouvre → SimulationWindow

SimulationWindow
    ├── possède → Simulation
    ├── possède → ZoneSimulation
    └── possède → TableauDeBord

Simulation
    ├── possède → Avion (pointeur)
    ├── possède → Piste
    ├── possède → ParametresFreinage
    └── émet signaux → ZoneSimulation, TableauDeBord

DataManager (statique)
    └── lit/écrit → data.txt
```

### 5.4 Énumérations

```cpp
enum class EtatSimulation {
    EN_ATTENTE, EN_COURS, EN_PAUSE, REUSSI, DETRUIT
};

enum class CauseDestruction {
    AUCUNE, DECROCHAGE, AVANT_PISTE, APRES_PISTE, DEPASSEMENT_PISTE
};

enum class ModeFreinage {
    ABSOLU, FLEXIBLE   // défaut : FLEXIBLE
};

enum class ModeDecrochage {
    VX_SEULE, NORME    // défaut : VX_SEULE
};

enum class Vue {
    GAUCHE, DROITE, ARRIERE
};
```

### 5.5 Système de signaux Qt

Qt utilise un mécanisme **signal/slot** — équivalent des listeners en Java :

```
QTimer::timeout  ──connect──→  Simulation::tick()
                                    │
                                    ├── emit simulationMiseAJour()
                                    │       ├──→ ZoneSimulation::rafraichir()
                                    │       └──→ TableauDeBord::rafraichir()
                                    │
                                    └── emit simulationTerminee(etat, cause)
                                            └──→ SimulationWindow::onSimulationTerminee()
```

---

## 6. Comportements détaillés

### 6.1 Démarrage

1. L'utilisateur remplit le formulaire (valeurs pré-remplies par défaut)
2. Clic VALIDER → validation → sauvegarde data.txt → ouverture SimulationWindow
3. L'avion est immobile à altitude=1000m, distance=-5000m
4. Le tableau de bord affiche `---` (simulation pas encore démarrée)

### 6.2 Simulation active (après START)

- Le QTimer déclenche `tick()` toutes les **100ms**
- À chaque tick : position recalculée, conditions vérifiées, UI rafraîchie
- L'avion se déplace visuellement — en vue profil, la scène défile autour de lui
- Le chronomètre s'incrémente en continu

### 6.3 PAUSE / REPRENDRE

- **PAUSE** : QTimer stoppé, chronomètre sauvegardé, boutons vitesse désactivés
- **REPRENDRE** : QTimer relancé, chronomètre reprend depuis où il s'était arrêté
- L'état exact de l'avion est conservé (position, vitesses, budgets)

### 6.4 RESTART

- Recharge les paramètres initiaux sauvegardés en mémoire (depuis la saisie formulaire)
- Recrée un nouvel objet `Avion` avec ces paramètres
- Remet tous les états à zéro : chronomètre, budgets, messages
- Tous les boutons reviennent à leur état initial
- Disponible à tout moment (avant, pendant, après la simulation)

### 6.5 Boutons bascule

| Bouton | État actif (bleu) | État inactif (gris) |
|---|---|---|
| Freinage | FLEXIBLE | ABSOLU |
| Décrochage | Vx | Norme |

Changeable à tout moment, même pendant la simulation.

### 6.6 Feedback visuel budget épuisé

- Bouton Vx− devient **rouge** quand le budget GammaX est épuisé
- Bouton Vy− devient **rouge** quand le budget GammaY est épuisé
- Ils redeviennent **bleus** si on accélère (mode Flexible uniquement)

### 6.7 Fin de simulation

Un `QDialog` s'affiche avec :
- Message coloré : vert (réussi) ou rouge (détruit + cause)
- Bouton **OK** → ferme le dialog
- Bouton **RESTART** → ferme et relance depuis l'état initial

---

## 7. Interface utilisateur

### 7.1 Disposition générale de SimulationWindow

```
┌─────────────────────────────────────────────────────────────────┐
│  [Vue ▼]  [▶ START]  [⏸ PAUSE]  [↺ RESTART]                   │
├─────────────────────────────────────────────────────────────────┤
│  [Freinage X: Vx+ | Vx−]  [Freinage Y: Vy+ | Vy−]             │
│  [Freinage: FLEXIBLE]  [Décrochage: Vx]                         │
├──────────────────────────────────────────┬──────────────────────┤
│                                          │  Position            │
│                                          │  Altitude : xxx m    │
│         Zone Simulation                  │  Distance : xxx m    │
│         (QPainter)                       │                      │
│                                          │  Vitesses            │
│                                          │  Vx : xxx | xxx      │
│                                          │  Vy : xxx | xxx      │
│                                          │                      │
│                                          │  Budget freinage     │
│                                          │  X : xx/40 m/s       │
│                                          │  Y : xx/60 m/s       │
│                                          │                      │
│                                          │  Chronomètre         │
│                                          │  00:00:00            │
│                                          │                      │
│                                          │  État                │
│                                          │  En cours...         │
└──────────────────────────────────────────┴──────────────────────┘
```

### 7.2 Les trois vues

| Vue | Position caméra | Ce qu'on voit |
|---|---|---|
| Vue Gauche | À gauche de l'avion | Profil nez vers droite, soleil en haut à gauche, piste à droite |
| Vue Droite | À droite de l'avion | Profil nez vers gauche (miroir), piste à gauche |
| Vue Arrière | Derrière l'avion | Dos de l'avion, piste en trapèze perspective au loin |

> Les vues sont **purement visuelles** — elles ne modifient pas la physique.

### 7.3 Palette de couleurs

| Élément | Couleur |
|---|---|
| Fond fenêtre | `#0d1117` (noir bleuté) |
| Zone simulation | `#0f1428` (bleu nuit) |
| Avion en vol | Blanc |
| Avion détruit | Rouge |
| Avion posé (réussi) | Vert |
| Valeurs tableau | `#00ff99` (vert néon) |
| Chronomètre | `#ffdd00` (jaune) |
| Budget épuisé | `#ff4444` (rouge) |
| Bouton START | `#2ecc71` (vert) |
| Bouton PAUSE | `#e67e22` (orange) |
| Bouton RESTART | `#8e44ad` (violet) |
| Boutons vitesse | `#1e5f8e` (bleu) |

---

## 8. Checklist d'avancement

### Phase 1 — Mise en place du projet
- [x] Créer le projet Qt (`simulation_atterrissage.pro`)
- [x] Créer l'arborescence `modele/`, `persistence/`, `ui/`
- [x] Compilation d'un `main.cpp` minimal avec fenêtre Qt

### Phase 2 — Modèle métier
- [x] `Avion` — attributs, mettreAJour, contrôle vitesses
- [x] `Avion` — budget freinage Absolu et Flexible
- [x] `Avion` — décrochage mode Vx et Norme
- [x] `Avion` — conversions km/h ↔ m/s
- [x] `Piste` — contientPosition()
- [x] `ParametresFreinage` — GammaX, GammaY
- [x] `Simulation` — états, QTimer, tick, evaluerConditions
- [x] `Simulation` — modes freinage et décrochage
- [x] `Simulation` — restart()

### Phase 3 — Persistance
- [x] `DataManager::sauvegarder()` → data.txt
- [x] `DataManager::charger()` → lecture clé=valeur

### Phase 4 — Formulaire
- [x] `FormulaireWindow` — 8 champs avec valeurs par défaut
- [x] Validation des champs
- [x] Sauvegarde + ouverture SimulationWindow

### Phase 5 — Zone de simulation
- [x] `ZoneSimulation` — Vue Gauche (profil, scène défile)
- [x] `ZoneSimulation` — Vue Droite (miroir)
- [x] `ZoneSimulation` — Vue Arrière (perspective trapèze)
- [x] Dessin piste, sol, graduations altitude
- [x] Dessin avion avec couleur selon état
- [ ] Affiner les vues visuellement (à faire)

### Phase 6 — Tableau de bord
- [x] Altitude, distance, Vx, Vy, vitesse totale
- [x] Budget freinage avec couleur rouge si épuisé
- [x] Chronomètre HH:MM:SS
- [x] État avec couleur

### Phase 7 — Contrôles et boutons
- [x] Bouton START
- [x] Bouton PAUSE / REPRENDRE
- [x] Bouton RESTART
- [x] Boutons Vx+/−, Vy+/−
- [x] Bouton bascule Freinage (Flexible/Absolu)
- [x] Bouton bascule Décrochage (Vx/Norme)
- [x] QDialog fin de simulation (réussi/détruit + cause)
- [x] Feedback rouge bouton bloqué

### Phase 8 — Tests et finitions
- [ ] Tester atterrissage réussi
- [ ] Tester chaque cas de destruction
- [ ] Tester PAUSE/REPRENDRE (état conservé)
- [ ] Tester RESTART complet
- [ ] Tester budget Absolu vs Flexible
- [ ] Tester mode décrochage Vx vs Norme
- [ ] Affiner rendu visuel des 3 vues
- [ ] Vérifier cohérence des unités partout

---

## 9. Glossaire

| Terme | Définition |
|---|---|
| **Tick** | Unité de temps de la simulation — déclenché toutes les 100ms par QTimer |
| **dt** | Delta-temps — durée d'un tick en secondes (0.1s) |
| **Vx** | Vitesse horizontale de l'avion (positive = avance vers la piste) |
| **Vy** | Vitesse verticale de l'avion (négative = descend) |
| **GammaX** | Budget maximum de freinage autorisé sur l'axe X (m/s) |
| **GammaY** | Budget maximum de freinage autorisé sur l'axe Y (m/s) |
| **Décrochage** | Perte de portance quand la vitesse passe sous le seuil critique |
| **Distance piste** | Position de l'avion relative au début de la piste (négatif = avant) |
| **Mode Direct** | Les boutons modifient la vitesse instantanément par palier |
| **Mode Absolu** | Budget freinage cumulatif — ne se recharge jamais |
| **Mode Flexible** | Budget freinage relatif à la vitesse courante — se recharge si on accélère |
| **QPainter** | Outil de dessin 2D de Qt utilisé dans paintEvent() |
| **QTimer** | Timer Qt qui déclenche un slot à intervalle régulier |
| **Signal/Slot** | Mécanisme Qt de communication entre objets (équivalent listener Java) |
| **paintEvent** | Méthode Qt appelée automatiquement pour redessiner un widget |
| **QDialog** | Fenêtre modale Qt pour afficher un message et attendre une réponse |