import random

def gerar_linhas():
    l = random.randint(1, 50)
    n = random.randint(1, 50)

    linhas = []
    for _ in range(n):
        Gi = random.randint(1, l)
        numeros = [random.randint(1, l) for _ in range(Gi)]
        linhas.append(f"{Gi} " + " ".join(map(str, numeros)))
    
    return l, n, linhas

def imprimir_linhas(l, n, linhas):
    print(f"{l} {n}")
    for linha in linhas:
        print(linha)

if __name__ == "__main__":
    l, n, linhas = gerar_linhas()
    imprimir_linhas(l, n, linhas)