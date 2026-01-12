## Description du projet

Ce projet d'informatique s'est déroulé du 06/01 au 17/01, il concernait l'implémentation en langage C d'un assembleur python 2.7.
Notre groupe SICOM 4 était composé de Clément Melkior chef d'équipe, Eliott Canville architecte, Thomas Touzet testeur et Benjamin Yates-Smith
Outilleur. Le groupe s'est rapidement réparti les tâches en travaillant sur site, le reste du temps pour communiquer nous avons utilisé un groupe
Whatsapp afin de rendre compte des avancées personnelles de chacun.



## Incrément 1 : 

	Implémentation d'un moteur d'expressions régulières

* on crée la fonction regexp-read,
Pour appeler cette fonction dans le terminal en bash on écrit "./prog/regexp-read.exe 'expression régulière que l'on souhaite tester'"
Si une expression régulière non valide est entrée le programme renvoie une erreur correspondant, par exemple '+abc' -> Erreur: caractère spécial '+' comme première caractère, index 0. 
Si une expression régulière valide est entrée le programme renvoie "one (not) in 'expression régulière' zero or more times, one or more times, zero or one times" selon l'opérateur utilisé (^, +, *, ?).

* On crée une deuxième fonction regexp-match,
Cette fonction a pour but de comparer une expression régulière et le début d'une chaîne de caractères,
On l'appelle dans le terminal avec la commande ./prog/regexp-match.exe 'regexp' 'string'
si l'expression régulière entrée correspond au début de la chaîne de caractère entrée alors le programme renvoie par exemple "the start of
'aaabc'(string) is in 'aaab'(regexp), next: c"  
Autrement, le programme renvoie "The start of 'a+'(string) is not in 'aaa'(regexp).


## Incrément 2 : 

	Analyse lexicale

* On crée la fonction list_t lex( char *regexp_file, char * source_file),
Le fichier regexp_file.txt comprend la liste des définitions des expressions régulières pour les lexèmes. 
list_t lex renvoie une liste de lexèmes à partir de l'un des fichiers .pys présents dans le dossier test-data.
La liste renvoyée par lex correspond aux fichiers .orc créés à la main en faisant un copier coller de la sortie du terminal en exécutant lexer.exe, ces derniers sont la liste de tous les lexèmes et leurs catégories correspondant à celles inscrites dans regexp_file.txt pour chacun des fichiers .pys.
En cas de fichier vide le programme renvoie un EXIT_FAILURE.

* Le programme lexer.exe prend en argument 'regexp_file.txt' et le fichier .pys dont l'on désire vérifier les lexèmes,
le programme affiche donc dans le terminal la liste de tous les lexèmes de notre fichier .pys.

* La fonction test du fichier lexem.c (originalement appelée test-lexem.c...) teste uniquement la fonction list_t lex,
notre test contrôle bien que la liste renvoyée par lex et les fichiers .orc retournent effectivement la même chose.
Nous avons effectué nos tests sur 5 fichiers .pys de complexité diverses et variées allant du simple Hello_world au test d'une fonction du second degré.
Nous avions également prévu un code python de 1 million de prints mais la complexité de celui-ci était trop importante pour qu'il puisse être testé sans faire crasher la machine. Lorsque nous utilisions list_add_last la complexité de ce type de code était trop importante pour notre lexer cependant en remplaçant l'utilisation de listes par l'utilisation de queues avec la fonction unqueue désormais la fonction lexer est capable de prendre en compte des fichiers aussi lourds.


## Incrément 3 : 

	Analyse syntaxique

* Création de la fonction pyobj_t parse(list_t *lexems), cette fonction renvoie un objet python de type code en prenant en argument une liste de lexems construite grâce  aux fonctions introduites dans les incréments précédents. La structure pyobj_t permet de stocker et de prendre en information le type d'objet que l'on manipule (integer, string, list voire un bout de code entier) elle est définie dans le fichier parse.c. 
La fonction test-parser.c  vérifie que la fonction parse retourne effectivement  un arbre syntaxique contenant le py_codeblock qui contient toutes les informations nécessaires pour représenter un bloc de code Python compilé, notons que que le bytecode est extrait via parse_code qui prend en compte tout caractère rédigé après une directive .text.

* Programme parser.exe appelée dans le terminal avec ./prog/parser.exe 'fichier.pys' le programme renvoie l'objet de code python, autrement, en cas d'erreur il renvoie la ligne et la colonne à laquelle il détecte une erreur lexicale ou de syntaxe puis EXIT_FAILURE.