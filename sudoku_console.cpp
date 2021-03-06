#include <iostream>
#include "sudoku.h"

using namespace std;

/**********************************************************
 * FUNCAO DE LEITURA DA JOGADA                            *
 **********************************************************/

// Leh uma jogada do teclado
/*
 \ i | i de 0-8 | i<0 | i>8 e j>=0 |
  \j |    e     | ou  |     ou     |
 v \ | j de 0-8 | j<0 | j>8 e i>=0 |
-----+----------+-----+------------+
1 a 9|   JOGAR  | FIM |  RESOLVER  |
  0  |  APAGAR  | FIM | PREENCHER  |
 > 9 |   NOVO   | FIM |   VOLTAR   |
 < 0 |    FIM   | FIM |     FIM    |
*/
void Jogada::ler()
{
  char tecla;

  cout << "Linha [A-I; V=Voltar; N=Novo; P=Preencher; R=Resolver; S=Sair]? ";
  do
  {
    cin >> tecla;
    tecla = toupper(tecla);
  } while ((tecla<'A' || tecla>'I') && tecla!='V' && tecla!='N' && tecla!='P' && tecla!='R' && tecla!='S');

  if (tecla=='V')
  {
    // Novo tabuleiro
    i = j = 10;
    v = 10;
    return;
  }
  if (tecla=='N')
  {
    // Novo tabuleiro
    i = j = 5;
    v = 10;
    return;
  }
  if (tecla=='P')
  {
    // Preencher as casas faceis (imediatas)
    i = j = 10;
    v = 0;
    return;
  }
  if (tecla=='R')
  {
    // Resolver o problema
    i = j = 10;
    v = 5;
    return;
  }
  if (tecla=='S')
  {
    // Sair do programa
    i = j = v = -1;
    return;
  }
  i = tecla-'A';  // De 0 a 8
  cout << "Coluna [1-9]? ";
  do
  {
    cin >> tecla;
    tecla = toupper(tecla);
  } while (tecla<'1' || tecla>'9');

  j = tecla-'1';  // De 0 a 8

  cout << "Valor [1-9; 0=apagar]? ";
  do
  {
    cin >> tecla;
    tecla = toupper(tecla);
  } while (tecla<'0' || tecla>'9');

  v = tecla-'0';  // De 0 a 9
}

/**********************************************************
 * FUNCOES DE EXIBICAO DO SUDOKU                          *
 **********************************************************/

/// Funcao auxiliar para exibir objetos da classe Sudoku
///
/// Imprime o conteudo do tabuleiro S na tela
/// O parametro booleano de entrada "inicial" indica o tipo da impressao:
/// tabuleiro inicial (true) ou atual (false)
void imprimir(const Sudoku &S, bool inicial)
{
  cout << (inicial ? "     TABULEIRO INICIAL" : "      TABULEIRO ATUAL") << endl;
  cout << "   1 2 3   4 5 6   7 8 9\n";
  cout << " +-------+-------+-------+\n";
  for (unsigned i=0; i<9; i++)
  {
    cout << char('A'+i) << "| ";
    for (unsigned j=0; j<9; j++)
    {
      if (S(i,j)>=1 && S(i,j)<=9) cout << int(S(i,j));
      else cout << '-';
      cout << ' ';
      if (j%3 == 2) cout << "| ";
    }
    cout << endl;
  }
  cout << " +-------+-------+-------+\n";
}

/// Exibir o conteudo do tabuleiro atual
void Sudoku::exibir() const
{
  imprimir(*this, false);
}

/// Exibir o conteudo do tabuleiro inicial
void Sudoku::exibir_origem() const
{
  imprimir(*this, true);
}

/// Exibe os numeros de tabuleiros gerados, testados e a analisar
void Sudoku::exibir_andamento(unsigned Ntab_testados, unsigned Ntab_gerados, unsigned Ntab_analisar) const
{
  cout << "Nos gerados, testados e a analisar: ";
  cout << Ntab_gerados << ' ';
  cout << Ntab_testados << ' ';
  cout << Ntab_analisar << endl;
}

