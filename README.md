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

![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/185f94bd-cbec-47f2-b0c4-a6503b626c7b)
![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/d5795fe1-b0c1-4bbb-a44c-1ab443eda932)

bin_to_7_seg_virgule.vhd : Ce fichier VHDL est le meme que celui précédent à l'exception que le point de notre afficheur est tout le temps activé pour voir nos données en g.

![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/fa9065e4-9364-4198-b9cc-559e7b4a76d7)
![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/f69866d4-db43-4f8f-8a81-b184d8ec1d5b)


lab3_vhd.vhd : Ce fichier définit les entrées et les sorties avec tous leurs branchements.

![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/26c2aeb2-bf7e-416c-85cd-a9f8c604e6a2)
![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/6be30fdb-8ff6-4529-a107-c7e99dea63ba)
![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/6ef734f2-3216-47ab-8909-caea98212a0f)


Flux logiciel du NIOS II

Ouvrons un terminal NIOS II et générons le BSP en lien avec les spécifications de la carte DE-10, en ce mettant de préférence dans le répertoire où se trouve nos fichiers vhd et qsys :

![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/f1aa3041-8481-43da-8983-4d273195ab90)

Pour générer lz fichier BSP, il faut utiliser la commande nio2-bsp hal <system.sopcinfo>

Générons le Makefile du projet avec la commande nios2-app-generate-makefile --app-dir --bsp-dir --elf-name <system.elf> --src-files <fichier.c>

Tout est prêt, nous allons maintenant écrire notre code C qui fonctionne sur notre propre processeur.

Notre programme en C est structuré comme ci:

Des fonctions permettant de lire ou d'écrire sur n'importe quel registe 1 octet de données
![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/6dffb227-211e-4752-adb6-250603a3b06a)

Une fonction main initialisant mon timer, activant nos intéruptions, reinitialisant et configurant nos offsets pour chaque axe:

![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/33aea1f2-c0f6-4580-9f0a-7f17d62e72a1)

Pour le calcul de l'offset, nous nous sommes basés sur le tableau des facteurs d'échelles des offsets:
![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/ea15be87-4288-49d9-ac9b-5ffb5b7e8b44)

Nous voyons que dans notre cas, nous devons prendre comme facteur d'échelle 3,9 mg/LSB.
Ensuite placons la carte dans une position initiale bien définie.
Enregistrons les valeurs de l'accélération dans cette position. Ces valeurs représentent l'offset initial.
A ces valeurs, on y soustrait la valeur théorique voulu sur chaque axe et on fait une division par rapport au facteur d'échelle.

Pour la récupération et l'affichage des données de l'accélérometre, nous utilisons la fonction irq_timer qui s'active à chaque secondes grace à l'interruption du timer
![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/05545908-2989-4464-ab5e-6b9b75c0cd34)

Et enfin pour permutter entre les valeurs des différents axes, on utilise la fonction de l'interruption du bouton poussoir irq_button qui incremente un compteur lié à un tableau où
sont mis les valeurs de X, Y et Z.
![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/9c9039d4-b34c-4f9e-8d9d-dac177ff9934)
![image](https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/9473a8ae-521c-4100-af69-684813913b95)


# - Résultat
Après le make download-elf permettant à notre carte de recevoir le code c créé, nous obtenons:

https://github.com/ESN2024/Ndiaye_lab3/assets/153745637/bb230438-3633-4ad8-849c-1c2d88dea330


# IV Conclusion

Ce TP a été synthèse de tout ce que l'on a appris sur les 2 précédents permettant de solidifier ces connaissances et compétences tout apportant de nouvelles
dans la tache qui fut de récupérer et calibrer les données de l'accélérometre de notre carte.
