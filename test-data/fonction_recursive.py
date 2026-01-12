def factorielle(n):
    # Cas de base : la factorielle de 0 ou 1 est 1
    if n == 0 or n == 1:
        return 1
    # Cas récursif : n! = n * (n-1)!
    else:
        return n * factorielle(n - 1)
