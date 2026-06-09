Je souhaite développer une application de simulation d'atterrissage d'avion.

Avant de proposer une architecture, des classes ou du code, je veux que tu analyses entièrement le besoin métier et que tu me confirmes ta compréhension du projet. N'écris aucun code tant que la compréhension fonctionnelle n'est pas validée.

Contexte général

L'application simule un avion déjà en vol qui doit réussir son approche et son atterrissage sur une piste.
L'avion ne décolle pas depuis la piste. Il est déjà dans les airs au démarrage de la simulation.
L'objectif de l'utilisateur est de contrôler les vitesses de l'avion afin de réussir l'atterrissage sans provoquer de destruction.

Paramètres de l'avion

L'avion possède les informations suivantes :

Nom avion
Vitesse X (horizontale)
Vitesse Y (verticale)
Accélération : 10 m/s
Vitesse de décrochage : 300 km/h
Altitude initiale : 1000 m
Distance initiale par rapport à la piste : -5000 m

Exemple :

Avion(
    vitesse_x,
    vitesse_y,
    acceleration = 10 m/s,
    vitesse_decrochage = 300 km/h,
    altitude = 1000 m,
    distance_piste = -5000 m
)
Paramètres de la piste
Piste(
    longueur = 7000 m
)
Paramètres de freinage
GammaX = -40 m/s
GammaY = -60 m/s

Ces valeurs représentent les limites maximales de freinage autorisées.
Persistance des données
Avant le lancement de la simulation, l'utilisateur remplit un formulaire contenant :

Nom avion
Altitude
Vitesse X
Vitesse Y
Vitesse de décrochage
Accélération
Paramètres de freinage

Après validation du formulaire :

les données doivent être enregistrées dans un fichier texte nommé data.txt
la simulation doit récupérer ses paramètres depuis ce fichier
les données saisies doivent donc être persistées avant utilisation
Interface attendue

L'application doit contenir :

Zone de simulation
Repère orthonormé
Avion
Piste
Déplacement visuel de l'avion
Tableau de bord

Affichage en temps réel de :

Altitude
Distance à la piste
Vitesse X
Vitesse Y
Chronomètre
Vues disponibles
Vue gauche
Vue droite
Vue arrière

Ces vues changent uniquement l'affichage.
Evolution de la distance
Au démarrage :

Distance = -5000 m
L'avion est situé avant la piste.
Lorsque l'avion avance :

-5000
-4500
-4000
...
-100
0

La valeur se rapproche de zéro jusqu'à atteindre le début de la piste.

Bouton START

Lorsque l'utilisateur clique sur START :

la simulation démarre
le chronomètre démarre
l'avion commence à se déplacer selon ses vitesses X et Y
Bouton PAUSE / REPRENDRE
Pause

Lors d'un clic sur PAUSE :

tout est figé
avion
chronomètre
calculs
animations

Le système doit conserver exactement son état courant.

Reprendre

Lors d'un clic sur REPRENDRE :

la simulation reprend à partir de l'état précédemment sauvegardé
Contrôle des vitesses
Contrôle vitesse X

Bouton +

Vx = Vx + acceleration

Bouton -

Vx = Vx - acceleration
Contrôle vitesse Y

Bouton +

Vy = Vy + acceleration

Bouton -

Vy = Vy - acceleration
Règles de décrochage

La vitesse de décrochage est :

300 km/h

La règle est :

Si Vitesse Avion < Vitesse Décrochage
=> Décrochage
=> Avion détruit

IMPORTANT :

Cette règle est vérifiée uniquement lorsque l'avion est encore en vol.

Une fois que l'avion a touché la piste, cette règle n'est plus appliquée.

Conditions de réussite

L'atterrissage est considéré comme réussi lorsque :

l'avion touche le sol ;
le point de contact est situé sur la piste ;
les conditions d'atterrissage sont respectées.
Conditions de destruction
Cas 1 : Décrochage

Pendant le vol :

Vitesse Avion < Vitesse Décrochage

=> avion détruit.

Cas 2 : Atterrissage avant la piste

L'avion touche le sol avant le début de la piste.

=> avion détruit.

Cas 3 : Atterrissage après la piste

L'avion touche le sol après la fin de la piste.

=> avion détruit.

Cas 4 : Dépassement de piste

L'avion atteint ou dépasse l'extrémité de la piste sans réussir son atterrissage.

=> avion détruit.

Ce que j'attends de toi
Reformule complètement le besoin pour vérifier ta compréhension.
Identifie les éventuelles ambiguïtés ou incohérences.
Propose les règles métier détaillées.
Propose les entités métier et leurs responsabilités.
Attends ensuite que je te communique les technologies à utiliser avant de proposer une architecture ou du code.

Commence uniquement par confirmer ta compréhension du projet. Ne génère pas encore de code.