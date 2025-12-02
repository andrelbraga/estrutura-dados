#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

typedef struct Grafo Grafo;
typedef struct Lista Lista;
typedef struct Distancia Distancia;

struct Distancia
{
  double distancia;
  Grafo *vertice;
};

struct Lista
{
  Distancia *distancia;
  Lista *prox;
};

struct Grafo
{
  char nome[50];
  Lista *listaGrafo;
  Grafo *prox;
};

Grafo *buscarGrafo(Grafo *g, char *nome)
{
  while (g != NULL)
  {
    if (strcmp(g->nome, nome) == 0)
      return g;
    g = g->prox;
  }
  return NULL;
}

Grafo *criarVertice(char *nome)
{
  Grafo *g = malloc(sizeof(Grafo));
  strcpy(g->nome, nome);
  g->listaGrafo = NULL;
  g->prox = NULL;
  return g;
}

Distancia *novaDistancia(double valor, Grafo *dest)
{
  Distancia *d = malloc(sizeof(Distancia));
  d->distancia = valor;
  d->vertice = dest;
  return d;
}

Lista *inserirLista(Lista *lista, Distancia *distancia)
{
  Lista *novo = malloc(sizeof(Lista));
  novo->distancia = distancia;
  novo->prox = lista;
  return novo;
}

Grafo *inserirGrafo(Grafo *inicio, char *nomeOrigem, double distancia, char *nomeDestino)
{

  Grafo *origem = buscarGrafo(inicio, nomeOrigem);
  if (!origem)
  {
    origem = criarVertice(nomeOrigem);
    origem->prox = inicio;
    inicio = origem;
  }

  Grafo *dest = buscarGrafo(inicio, nomeDestino);
  if (!dest)
  {
    dest = criarVertice(nomeDestino);

    dest->prox = inicio;
    inicio = dest;

    Distancia *conexaoVolta = novaDistancia(distancia, origem);
    dest->listaGrafo = inserirLista(dest->listaGrafo, conexaoVolta);
  }

  Distancia *d = novaDistancia(distancia, dest);
  origem->listaGrafo = inserirLista(origem->listaGrafo, d);

  return inicio;
}

void imprimirGrafo(Grafo *g)
{
  printf("\n==== LISTA COMPLETA DO GRAFO ====\n\n");
  Grafo *v = g;
  while (v != NULL)
  {
    printf("Vértice: %s\n", v->nome);
    printf(" Conexões:\n");

    Lista *a = v->listaGrafo;
    while (a != NULL)
    {
      printf("   -> %s (distância: %.2f)\n",
             a->distancia->vertice->nome,
             a->distancia->distancia);
      a = a->prox;
    }

    double soma = 0.0;
    Lista *aux = v->listaGrafo;
    while (aux != NULL)
    {
      soma += aux->distancia->distancia;
      aux = aux->prox;
    }
    printf("   Soma das distâncias: %.2f\n", soma);
    printf("------------------------\n");
    v = v->prox;
  }
}

int indiceVertice(Grafo **nodes, int n, Grafo *v)
{
  for (int i = 0; i < n; ++i)
  {
    if (nodes[i] == v)
      return i;
  }
  return -1;
}

void imprimirCaminhoPrev(int *prev, Grafo **nodes, int src, int dest)
{
  if (dest == -1)
  {
    printf("Nenhum caminho\n");
    return;
  }

  int *stack = malloc(sizeof(int) * 1000);
  int top = 0;
  int cur = dest;
  while (cur != -1)
  {
    stack[top++] = cur;
    if (cur == src)
      break;
    cur = prev[cur];
  }

  if (top == 0 || stack[top - 1] != src)
  {
    printf("Nenhum caminho encontrado entre os vértices informados.\n");
    free(stack);
    return;
  }

  printf("Caminho: ");
  for (int i = top - 1; i >= 0; --i)
  {
    printf("%s", nodes[stack[i]]->nome);
    if (i > 0)
      printf(" -> ");
  }
  printf("\n");
  free(stack);
}

void calcularMelhorCaminho(Grafo *g, char *nomeOrigem, char *nomeDestino)
{
  // Monta vetor de vértices
  int n = 0;
  for (Grafo *p = g; p != NULL; p = p->prox)
    n++;

  if (n == 0)
  {
    printf("Grafo vazio.\n");
    return;
  }

  Grafo **nodes = malloc(sizeof(Grafo *) * n);
  int idx = 0;
  for (Grafo *p = g; p != NULL; p = p->prox)
    nodes[idx++] = p;

  int src = -1, dest = -1;
  for (int i = 0; i < n; ++i)
  {
    if (strcmp(nodes[i]->nome, nomeOrigem) == 0)
      src = i;
    if (strcmp(nodes[i]->nome, nomeDestino) == 0)
      dest = i;
  }

  if (src == -1 || dest == -1)
  {
    printf("Origem ou destino não encontrado no grafo.\n");
    free(nodes);
    return;
  }

  double *dist = malloc(sizeof(double) * n);
  int *prev = malloc(sizeof(int) * n);
  int *visitado = malloc(sizeof(int) * n);

  for (int i = 0; i < n; ++i)
  {
    dist[i] = DBL_MAX;
    prev[i] = -1;
    visitado[i] = 0;
  }

  dist[src] = 0.0;

  for (int it = 0; it < n; ++it)
  {
    // escolhe o não visitado com menor distância
    double best = DBL_MAX;
    int u = -1;
    for (int i = 0; i < n; ++i)
    {
      if (!visitado[i] && dist[i] < best)
      {
        best = dist[i];
        u = i;
      }
    }

    if (u == -1)
      break;

    visitado[u] = 1;

    // relaxa arestas de u
    Lista *a = nodes[u]->listaGrafo;
    while (a != NULL)
    {
      int v = indiceVertice(nodes, n, a->distancia->vertice);
      if (v != -1 && !visitado[v])
      {
        double alt = dist[u] + a->distancia->distancia;
        if (alt < dist[v])
        {
          dist[v] = alt;
          prev[v] = u;
        }
      }
      a = a->prox;
    }
  }

  printf("\n=== MELHOR CAMINHO: %s -> %s ===\n", nomeOrigem, nomeDestino);
  if (dist[dest] == DBL_MAX)
  {
    printf("Não existe caminho entre %s e %s.\n", nomeOrigem, nomeDestino);
  }
  else
  {
    printf("Distância total: %.2f\n", dist[dest]);
    imprimirCaminhoPrev(prev, nodes, src, dest);
  }

  free(nodes);
  free(dist);
  free(prev);
  free(visitado);
}

int main()
{
  Grafo *g = NULL;

  g = inserirGrafo(g, "ESCOLA 1", 1.0, "ESCOLA 2");
  g = inserirGrafo(g, "ESCOLA 1", 5.0, "ESCOLA 3");
  g = inserirGrafo(g, "ESCOLA 2", 2.0, "ESCOLA 4");
  g = inserirGrafo(g, "ESCOLA 3", 8.0, "ESCOLA 1");

  imprimirGrafo(g);

  calcularMelhorCaminho(g, "ESCOLA 1", "ESCOLA 4");
  
  return 0;
}