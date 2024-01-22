# Ndiaye_lab3

# I. Introduction :
L'objectif de ce rapport est de présenter le travail accompli lors des travaux pratiques de l'UE Conception Conjointe, visant à approfondir nos connaissances et à les mettre en œuvre à travers la création d'une architecture NIOS pour récupérer les valeurs d'un accélérometre. 
Le cahier des charges était clair : 
- Calibrez l'accéléromètre de votre carte
- Demander des données d'accélération chaque seconde
- Lire l'accélération selon X, Y et Z 
- Affichez leur valeur sur l'écran 7seg
- Basculez entre les valeurs en utilisant un bouton poussoir

# II. Architecture :
À partir du cahier des charges, nous avons déterminé l'architecture à mettre en place sur la plate-forme designer. Le schéma ci-dessous présente notre choix :

![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/f9e4183d-5430-420b-a41b-9d0cb8e651cb)


Création d'un système NIOS II basique Platform Designer

Les composants de notre systeme sont:

onchip_memory : Une RAM offrant une capacité de stockage de 40 Mo.

pio_0 : Représente l'entrée Qsys relié à notre bouton poussoir.

pio_1,pio_2,pio_3,pio_4 : Représente les sorties Qsys affichant la valeur de notre accélérometre sur les afficheurs 7 segments.

pio_5:  Représente les sorties Qsys affichant le signe de la valeur de notre accélérometre sur un afficheur 7 segments.

Timer_0 : Configurer la période de ce dernier à 1 seconde.

Opencores_i2c_0: Représente l'I2C qui sera interfacé avec notre accélérometre.

![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/1fdde69c-bef1-4699-9c52-ff28f299a7cc)

![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/37a2c52e-605d-46f6-852b-a0cac9b755f3)


# III Processus et Résultats
# - Processus
Après la configuration de notre fichier qsys, nous devons créer les codes VHDl de notre système Quartus

![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/b43b0a70-bd66-48d5-936b-23d84e6c37ec)

Les différents fichiers du projet sont

lab3.qsys: le fichier qsys généré préalablement dans platform designer

bin_to_7_seg.vhd : Ce fichier VHDL prend en entrée un signal de 4 bits  et génère en sortie une correspondance de 7 bits pour l'activation des segments. 

bin_to_7_seg_virgule.vhd : Ce fichier VHDL est le meme que celui précédent à l'exception que le point de notre afficheur est tout le temps activé pour voir nos données en g.

image

image

lab2_vhd.vhd : Ce fichier définit les entrées et les sorties, avec trois sorties de 7 bits chacune correspondant aux segments des 3 compteurs.

image

image

Flux logiciel du NIOS II Ouvrons un terminal NIOS II et générons le BSP en lien avec les spécifications de la carte DE-10, en ce mettant de préférence dans le répertoire où se trouve nos fichiers vhd et qsys :

image

Pour générer lz fichier BSP, il faut utiliser la commande nio2-bsp hal <system.sopinfo>

Générons le Makefile du projet avec la commande nios2-app-generate-makefile --app-dir --bsp-dir --elf-name <system.elf> --src-files <fichier.c>

Tout est prêt, nous allons maintenant écrire notre code C qui fonctionne sur notre propre processeur.

Notre programme en C est structuré comme ci:

Une fonction main initialisant mon timer et l'activant:

image

Et ma fonction simple_irq qui incrémente mon compteur toutes les secondes et me l'affiche sur mes afficheurs 7 segments.

image

# - Résultat
Après le make download-elf permettant à notre carte de recevoir le code c créé, nous obtenons:


https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/bb230438-3633-4ad8-849c-1c2d88dea330


IV Conclusion
Ce TP a été synthèse de tout ce que l'on a appris sur les 2 précédents permettant de solidifier ces connaissances tout apportant de nouvelles connaissances et compétences
dans la tache qui fut de récupérer et calibrer les données de l'accélérometre de notre carte.
