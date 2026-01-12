Saisir dans ce fichier ce que vous avez pu travailler durant l'examen

** Ne pas oublier de add/commit/push les modifications apportées à votre code !**

** Ne pas oublier de add/commit/push ce fichier à votre dépot !**

A faire : Corriger bugs dans re_match + segfaults dans le lexer (voir dossier tests_exam/ )

# Liste des fichiers de test travaillés
test-lexem.c
test-regexp.c
test-regexp-read.ch

# Synthèse des améliorations apportées qui fonctionnent en quelques mots
modification de queue.c : traitement du cas où queue_to_list reçois une queue vide.
modification de test-lexem.c : rajout d'une exam section qui teste les fichiers pys de l'examen 03_int_KO.pys et 07_unterminated_string.pys .

modification de re_match.c : correction pour la négation 
modification de test-regexp.c : ajout des tests '^ab' 'cbefg' et '^ab' 'abefg'

modification de regexp.c : afin de traiter les cas comme [a-b-z] en produisant une erreur
modification de test-regexp-read.c : afin de tester le cas [a-b-z]

# Synthèse de ce que vous avez voulu faire sans succes
Toutes les modifications demandées ont été réalisées avec succès !
