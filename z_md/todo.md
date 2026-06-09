# Simulation avion

Position initial : Vol a une altitude avec une vitesse suivant x et suivant y deja au dessus sans decolage 
 

avion(
    vitesse_x,
    vitesse_y,
    acceleration(10),
    vitesse_decrossage(300km/h),
    altitude(1000m),
    distance_par_rapport_piste(-5000m) :
    );
piste(
    longueur : 7000m
);
parametre(
    vitesse_decrossage : 300km/h
    capacite de freignage_max 
    gamma_x = -40m/s
    gamma_y = -60m/s
)
Maquette de dessin : 

element_information : 
Repere orthonormer + avion + piste
tableu kilometrique, 
altitude, 
Distance parcourie(-7000) et diminue lorsque l'avion aproche de la piste,
vitesse_x,
vitesse_y
vue : droite , gauche, deriere;
chronometre

element_action : 

acceleration(-m/s) : 10m/s
freignage_x : - +  ex : -acceleration m/s
freignage_y : - +  ex : -acceleration /s 

bouton start : Demare la simulation : avion avance avec sa vitesse : x,y
bouton pause/reprendre : Lorsqu'on clique pause on fige tous , comme si le pc plante et le bouton pause deviens reprendre et si on clique reprendre on revient dans l'etat avant pause
bouton : 
freignage x
+ : augmente la vitesse de l'avion suivant x de acceleration
- : diminue la vitesse de l'avion suivant x de acceleration

freignage y
+ : augmente la vitesse de l'avion suivant y de acceleration
- : diminue la vitesse de l'avion suivant y de acceleration

Regle de destruction de l'avion : Lorsquer la vitesse de l'avion n'atteint pas le vitesse de decrossage c a d : 
V.avion < V.decrrosage => avion => detruit 
Lorsquer l'avion atterie en dehors de la piste ou atteint le boug de la piste => detruit
Lorsquer l'avion est sur la piste et demarer avec une vitesse >= V.decrossage l'avion augment 

Techno : c++ fenetrer 