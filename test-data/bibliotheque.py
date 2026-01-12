

class Auteur:
    def __init__(self, nom, prenom):
        self.nom = nom
        self.prenom = prenom

    def __str__(self):
        return "{} {}".format(self.prenom, self.nom)

class Livre:
    def __init__(self, titre, auteur, annee_publication):
        self.titre = titre
        self.auteur = auteur
        self.annee_publication = annee_publication

    def __str__(self):
        return "{} par {} ({}).".format(self.titre, self.auteur, self.annee_publication)

class Bibliotheque:
    def __init__(self):
        self.livres = []

    def ajouter_livre(self, livre):
        self.livres.append(livre)

    def afficher_livres(self):
        for livre in self.livres:
            print(livre)

    def trouver_livres_par_auteur(self, auteur):
        return [livre for livre in self.livres if livre.auteur == auteur]

# Utilsation
if __name__ == "__main__":
    # Creer des auteurs
    auteur1 = Auteur("Hugo", "Victor")
    auteur2 = Auteur("Dumas", "Alexandre")

    # Creer des livres
    livre1 = Livre("Les Miserables", auteur1, 1862)
    livre2 = Livre("Notre-Dame de Paris", auteur1, 1831)
    livre3 = Livre("Le Comte de Monte-Cristo", auteur2, 1844)

    # Creer une bibliothèque
    bibliotheque = Bibliotheque()

    # Ajouter des livres à la bibliothèque
    bibliotheque.ajouter_livre(livre1)
    bibliotheque.ajouter_livre(livre2)
    bibliotheque.ajouter_livre(livre3)

    # Afficher tous les livres de la bibliothèque
    print("Tous les livres de la bibliotheque :")
    bibliotheque.afficher_livres()

    # Trouver les livres d'un auteur specifique, dans le cas que je choisis Victor Hugo
    print("\nLivres de Victor Hugo :")
    livres_hugo = bibliotheque.trouver_livres_par_auteur(auteur1)
    for livre in livres_hugo:
        print(livre)
