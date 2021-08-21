#include <iostream>
#include <cmath>
#include <stdlib.h>
#include "sudoku.h"

using namespace std;

/* ===================================================================
AS FUNCOES DE ENTRADA/SAIDA FORMATADA
=================================================================== */

// Imprime msg e espera que o usuario digite uma tecla
void espera_tecla()
{
	system("Pause");
}

// Desenha os textos iniciais e molduras dos tabuleiros
void exibir_cabecalho()
{
  cout << " +-----------------------+\n";
  cout << " |         SUDOKU        |\n";
  cout << " +-----------------------+\n";
}

// Informa que o tabuleiro foi resolvido
void exibir_congratulacoes()
{
  cout << "PARABENS! JOGO CONCLUIDO!\n";
  espera_tecla();
}

// Exibe o numero de casas faceis preenchidas imediatamente
void exibir_preenchidas(int N)
{
  if (N != 0)
  {
    unsigned NN = abs(N);
    if (NN <= 81)
    {
      cout << NN << ' ';
      if (NN>1)
      {
        cout << "casas preenchidas.\n";
      }
      else
      {
        cout << "casa preenchida.\n";
      }
    }
    if (N < 0)
    {
      cout << "O TABULEIRO NAO TEM SOLUCAO!\n";
    }
    espera_tecla();
  }
}

int main(void)
{
  Sudoku Origem("sudoku.txt"),S(Origem);
  Jogada J;

  exibir_cabecalho();

  Origem.exibir_origem();
  S.exibir();

  do
  {
    do
    {
      J.ler();
    } while(!J.jogada() && !J.apagamento() &&
            !J.resolver_jogo() && !J.preencher_jogo() &&
            !J.novo() && !J.voltar() &&
            !J.fim_de_jogo());
    if (J.jogada())
    {
      S.fazer_jogada(J);
    }
    if (J.apagamento())
    {
      S.apagar_jogada(J,Origem);
    }
    if (J.preencher_jogo())
    {
      S.exibir_origem();
      int N = S.resolver_casas_faceis();
      S.exibir();
      exibir_preenchidas(N);
      Origem.exibir_origem();
    }
    if (J.resolver_jogo())
    {
      S.exibir_origem();
      S.resolver();
      espera_tecla();
      Origem.exibir_origem();
    }
    if (J.novo())
    {
      Origem.gerar();
      S=Origem;
      Origem.exibir_origem();
    }
    if (J.voltar())
    {
      S = Origem;
    }
    S.exibir();
    if (!J.fim_de_jogo() && S.fim_de_jogo())
    {
      exibir_congratulacoes();
    }
  } while(!J.fim_de_jogo());
}

