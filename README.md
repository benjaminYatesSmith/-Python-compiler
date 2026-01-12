This computer science project took place from 06/01 to 17/01 and focused on the implementation in C of a Python 2.7 assembler.

Our SICOM 4 group consisted of Benjamin Yates-Smith as team leader, Eliott Canville as architect, Thomas Touzet as tester, and Clément Melkior as tooling engineer. The group quickly divided tasks while working on site. For communication outside working hours, we used a WhatsApp group to report individual progress.

Increment 1:

We implemented a regular expression engine.

We created the function regexp-read. To call this function from the Bash terminal, we use "./prog/regexp-read.exe 'regular_expression_to_test'".

If an invalid regular expression is entered, the program returns a corresponding error. For example, "+abc" results in the error: "Error: special character '+' as first character, index 0".

If a valid regular expression is entered, the program returns a message such as "one (not) in 'regular expression' zero or more times, one or more times, zero or one times", depending on the operator used (^, +, *, ?).

We then created a second function called regexp-match. This function compares a regular expression with the beginning of a character string. It is called in the terminal using "./prog/regexp-match.exe 'regexp' 'string'".

If the regular expression matches the beginning of the input string, the program returns for example: "the start of 'aaabc' (string) is in 'aaab' (regexp), next: c". Otherwise, the program returns: "The start of 'a+' (string) is not in 'aaa' (regexp)".

Increment 2:

We performed lexical analysis.

We created the function list_t lex(char *regexp_file, char *source_file). The file regexp_file.txt contains the list of regular expression definitions for lexemes. The function list_t lex returns a list of lexemes from one of the .pys files located in the test-data folder.

The list returned by lex corresponds to the .orc files created manually by copying and pasting the terminal output produced by running lexer.exe. These files contain the list of all lexemes and their associated categories, as defined in regexp_file.txt, for each .pys file.

If the source file is empty, the program returns EXIT_FAILURE.

The program lexer.exe takes as arguments regexp_file.txt and the .pys file whose lexemes are to be analyzed. The program then displays in the terminal the list of all lexemes in the given .pys file.

The test function in lexem.c (originally called test-lexem.c) tests only the list_t lex function. Our test verifies that the list returned by lex and the .orc files effectively contain the same data.

We performed our tests on five .pys files of varying complexity, ranging from a simple Hello World to the test of a second-degree polynomial function.

We had also planned to test a Python program containing one million print statements, but its complexity was too high to be executed without crashing the machine. When using list_add_last, the complexity of this type of code was too high for our lexer. However, by replacing list usage with queues and using the unqueue function, the lexer is now able to handle files of such size.

Increment 3:

We performed syntax analysis.

We created the function pyobj_t parse(list_t *lexems). This function returns a Python code object and takes as input a list of lexemes constructed using the functions introduced in the previous increments.

The structure pyobj_t allows us to store and handle information about the type of object being manipulated, such as integers, strings, lists, or even a full block of code. It is defined in the file parse.c.

The file test-parser.c verifies that the parse function correctly returns a syntax tree containing a py_codeblock, which includes all the information required to represent a compiled Python code block. The bytecode is extracted via parse_code, which processes all characters written after a .text directive.

The program parser.exe is called from the terminal using "./prog/parser.exe 'file.pys'". The program returns the Python code object. In case of an error, it returns the line and column where a lexical or syntax error is detected, followed by EXIT_FAILURE.



## Description du projet

Ce projet d'informatique s'est déroulé du 06/01 au 17/01 2025, il concernait l'implémentation en langage C d'un assembleur python 2.7.
Notre groupe SICOM 4 était composé de Benjamin Yates-Smith chef d'équipe, Eliott Canville architecte, Thomas Touzet testeur et Clément Melkior.
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
