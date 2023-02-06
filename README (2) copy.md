# TP1: Dessiner sur un canevas ASCII"


## Format Markdown (supprimer cette section avant la remise)

N'oubliez pas de bien exploiter le format Markdown.

Sauter une ligne pour changer de paragraphe.

Mettre les noms de fichier et bout de code courts entre apostrophes inversés.
Par exemple, si vous parlez du fichier `Makefile`.

Mettre les longs bouts de code dans des blocs de code (triples apostrophes
inversés). Par exemple, vous pouvez donner un exemple de commande comme suit:

```sh
$ make
$ ls -a
```

Utiliser des listes à puces ou des énumérations le plus possible (plus agréable
à lire). Par exemple, pour décrire le contenu du projet:

* `README.md`: ce fichier
* `Makefile`: permet d'automatiser la compilation
* etc.

Bien aérer le contenu du fichier source (`README.md`). Éviter les longues
lignes dans le fichier Markdown (par exemple, limiter à 80) pour une meilleure
lisibilité avec un éditeur de texte.

## Description

Décrivez ici le projet. Commencez d'abord par une description générale, puis
donnez ensuite des détails. Indiquez le contexte dans lequel ce travail est
accompli (cours, sigle, enseignant, université).

Dans le cadre du cours inf3135, Construction et maintenance de logiciels, notre enseignant de l'Université du Québec à Montréal, Gnagnely Serge Dogny, nous a présnté le travail pratique numéro 1 sous le nom de "Dessiner sur un canevas ASCII". 
L'objectif de ce travail est de faire un programme C nommé `canvascii.c` qui permet de dessiner sur un canevas ASCII à l'aide d'options implémentés dans le programme. Ce programme est intéractif, c'est-à-dire que l'utilisateur peut préciser les options qu'il souhaite à la ligne de commande. Évidemment, les demandes éronnées se retrouverons sur la sortie standard d'érreur (`stderr`) tandis que le reste sera sur la sortie standard (`stdout`).
Si vous souhaitez voir l'intégralité du projet, voici un lien vers le [sujet du travail](https://gitlab.info.uqam.ca/tamas.isabelle/inf3135-tp1-h23/-/blob/main/sujet.md).

## Auteur

Indiquez ici votre prénom et nom, puis votre code permanent entre parenthèses,
sans mettre en gras ou en italique.

Par exemple,

Isabelle Tamas (TAMI76580208)

## Fonctionnement

Expliquez d'abord en mots comment faire fonctionner le projet (imaginez que la
personne qui l'utilisera ne connaît pas C, ne connaît pas le projet et souhaite
seulement saisir une série de commandes pour l'exécuter). En particulier,
indiquez les commandes qui doivent être entrées pour la compilation et
l'exécution.

Le fonctionnement est simple. Tout d'abord, il faut commencer par compiler le programme grâce à la commande suivante: 
```sh
$ make
```

Ensuite, il faudra l'exécuter avec la commande :
```sh
$ ./canvascii
```
Cette dernière affichera un manuel d'utilisation sur la sortie standard (`stdout`). À partir de la, vous allez faire connaissance de toutes les options possibles et vous saurez exactement comment utiliser le programme. 

## Tests

Expliquez ici comment lancer la suite de tests automatiques avec la commande
`make test`, ainsi que le résultat que vous obtenez.

En premier, il faut compiler le programme à l'aide de la commande:
```sh
$ make
```
Ensuite, nous pouvons utiliser la commande:
```sh
$ make test
```
pour lancer automatiquement les tests. 

Il est nécessaire d'installer [Bats](https://github.com/bats-core/bats-core) dans le but de pouvoir lancer la suite de tests. 
## Dépendances

Indiquez les dépendances du projet, avec lien officiel. Il faudrait au moins
mentionner GCC et [Bats](https://github.com/bats-core/bats-core). Utiliser une
liste à puces pour donner la liste des dépendances.

Certaines dépendances sont nécessaires pour le bon fonctionnement de ce programme. Nous pensons explicitement aux outils suivants:
* [Bats](https://github.com/bats-core/bats-core) : cadre de test
* [GCC](https://gcc.gnu.org/install/download.html) : compilateur
* [Pandoc](https://pandoc.org/) : logiciel de transformateur de fichiers



## Références

Indiquez ici les références que vous avez utilisées pour compléter le projet,
avec l'hyperlien vers la référence. Pas besoin de mentionner les diapositives
du cours, mais si vous avez eu recours à un site d'aide, un fragment de code ou
une discussion sur un forum, mentionnez-le.

Références utilisées pour compléter le projet :
* [Algorithme de Bresenham](https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm) de la section [All cases](https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm?section=7#All_cases)
* [Algorithme de tracé d'arc de cercle par point milieu](https://en.wikipedia.org/wiki/Midpoint_circle_algorithm) sur le site [Rosetta Code](https://rosettacode.org/wiki/Bitmap/Midpoint_circle_algorithm#C)
* [code de couleur ANSI](https://en.wikipedia.org/wiki/ANSI_escape_code#3/4_bit)


## État du projet

Indiquez toutes les tâches qui ont été complétés en insérant un `X` entre les
crochets. Si une tâche n'a pas été complétée, expliquez pourquoi (lors de la
remise, vous pouvez supprimer ce paragraphe).

* [x] Le nom du dépôt GitLab est exactement `inf3135-tp1-h23` (Pénalité de
  **50%**).
* [x] L'URL du dépôt GitLab est exactement (remplacer `utilisateur` par votre
  nom identifiant GitLab) `https://gitlab.info.uqam.ca/utilisateur/inf3135-tp1-h23`
  (Pénalité de **50%**).
* [ ] Les usagers `dogny_g` et `correcteur` ont accès au projet en mode *Developer*
  (Pénalité de **50%**).
* [x] Le dépôt GitLab est un *fork* de [ce
  dépôt](https://gitlab.info.uqam.ca/inf3135-sdo/canvascii)
  (Pénalité de **50%**).
* [x] Le dépôt GitLab est privé (Pénalité de **50%**).
* [ ] Le dépôt contient au moins un fichier `.gitignore`.
* [ ] Le fichier `Makefile` permet de compiler le projet lorsqu'on entre
  `make`. Il supporte les cibles `html`, `test` et `clean`.
* [ ] Le nombre de tests qui réussissent/échouent avec la `make test` est
  indiqué quelque part dans le fichier `README.md`.
* [ ] Les sections incomplètes de ce fichier (`README.md`) ont été complétées.
* [ ] L'en-tête du fichier est documentée.
* [ ] L'en-tête des déclarations des fonctions est documentée (*docstring*).
* [ ] Le programme ne contient pas de valeurs magiques.
