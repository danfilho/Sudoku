#include <cstdlib>     /* srand, rand */
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <time.h>
#include <iostream>
#include <algorithm>
#include "sudoku.h"

using namespace std;

/**********************************************************
 * CLASSE JOGADA                                          *
 **********************************************************/

/// Construtor (por default, cria Jogada que termina o jogo)
Jogada::Jogada(int I, int J, int V)
{
    setCell(I,J);
    setValue(V);
}

/// Fixa as coordenadas de uma jogada
/// Por default, cria Jogada que termina o jogo
void Jogada::setCell(int I, int J)
{
    if (I<0 || I>8) I=-1;
    if (J<0 || J>8) J=-1;
    i = I;
    j = J;
}

/// Fixa o valor de uma jogada
/// Por default, cria Jogada que termina o jogo
void Jogada::setValue(int V)
{
    if (V<0 || V>9) V=-1;
    v = V;
}

/**********************************************************
 * CLASSE SUDOKU                                          *
 **********************************************************/

/// Limpa o tabuleiro
void Sudoku::limpar()
{
  for (unsigned i=0; i<9; i++) for (unsigned j=0; j<9; j++)
  {
    set(i,j) = 0;
  }
}

/// Gera um novo tabuleiro preenchido de maneira aleatoria em algumas casas
/// Esse algoritmo nao eh otimizado, no sentido que nao hah garantia de que
/// o tabuleiro gerado tenha solucao ou que tenha solucao unica.
void Sudoku::gerar()
{
  Jogada J;
  int i,k,n;

  srand(time(NULL));

  do
  {
    // Inicia com o tabuleiro vazio
    limpar();

    // Sorteia o numero "n" de casas a serem preenchidas: de 17 a 27
    // Sabe-se que nao existe nenhum tabuleiro de Sudoku com menos de 17 pedras
    // iniciais que tenha solucao unica
    // O valor maximo 27 foi escolhido de forma arbitraria
    n = 17+rand()%11;

    // Preenche aleatoriamente "n" casas do tabuleiro
    for (i=0; i<n; i++)
    {
      do
      {
        J.i = rand()%9;
        J.j = rand()%9;
        J.v = 1+rand()%9;
      } while (!jogada_valida(J));
      fazer_jogada(J);
    }
    // Resolve as casas triviais
    k = resolver_casas_faceis();
    // Recalcula se o tabuleiro for insoluvel ou se jah tiver muitas casas preenchidas
  } while (k<0 || num_casas_vazias()<40);
}

/// Cria um tabuleiro com o conteudo do arquivo nome_arq
/// Caso nao consiga ler do arquivo, retorna tabuleiro vazio
/// ou lido parcialmente, caso contrario
Sudoku::Sudoku(const char *nome_arq): tab(81)
{
  // Comeca com tabuleiro vazio
  limpar();

  // Le o tabuleiro inicial de arquivo
  ifstream arq(nome_arq);
  if (!arq.is_open())
  {
    return;
  }

  string prov;
  int valor;

  arq >> prov;
  if (prov != "SUDOKU")
  {
    return;
  }

  // Leh as pedras do arquivo
  for (unsigned i=0; i<9; i++) for (unsigned j=0; j<9; j++)
  {
    arq >> valor;
    if (valor != 0)
    {
      Jogada J(i,j,valor);
      fazer_jogada(J);
    }
  }
  arq.close();
}

/// Compara se dois tabuleiros sao iguais
bool Sudoku::operator==(const Sudoku &S) const
{
  for (unsigned i=0; i<9; i++) for (unsigned j=0; j<9; j++)
  {
    if (at(i,j) != S(i,j)) return false;
  }
  return true;
}

/// Testa se a jogada J eh possivel para o tabuleiro
bool Sudoku::jogada_valida(Jogada J) const
{
  unsigned i,j;
  // Testar a jogada
  if (!J.jogada()) return false;

  // Testar se a casa nao estah vazia
  if (at(J.i,J.j) != 0) return false;

  // Testar a linha
  for (i=0; i<9; i++)
  {
    if (at(i,J.j) == J.v) return false;
  }
  // Testar a coluna
  for (j=0; j<9; j++)
  {
    if (at(J.i,j) == J.v) return false;
  }
  // Testar o bloco
  unsigned iIni = 3*(J.i/3);
  unsigned jIni = 3*(J.j/3);
  for (i=0; i<3; i++) for (j=0; j<3; j++)
  {
    if (at(iIni+i,jIni+j) == J.v) return false;
  }
  return true;
}

/// Testa se a jogada J eh um apagamento valido de uma casa para o tabuleiro,
/// levando em conta o tabuleiro original (nao eh permitido apagar casas que
/// estavam preenchidas no tabuleiro original)
bool Sudoku::apagamento_valido(Jogada J, const Sudoku &Origem) const
{
  // Testar a jogada
  if (!J.apagamento()) return false;

  // Testar se a casa estah vazia
  if (at(J.i,J.j) == 0) return false;

  // Testar se a casa do tabuleiro original nao estah vazia
  if (Origem(J.i,J.j) != 0) return false;

  return true;
}

/// Testa se o tabuleiro estah completo (fim de jogo)
bool Sudoku::fim_de_jogo() const
{
  for (unsigned i=0; i<9; i++) for (unsigned j=0; j<9; j++)
  {
    if (at(i,j) == 0) return false;
  }
  return true;
}

/// Retorna o numero de casas vazias no tabuleiro
unsigned Sudoku::num_casas_vazias() const
{
  unsigned N(0);
  for (unsigned i=0; i<9; i++) for (unsigned j=0; j<9; j++)
  {
    if (at(i,j) == 0) N++;
  }
  return N;
}

/// Calcula o numero de possibilidades para preencher a casa (i,j)
/// utilizando apenas as regras "faceis" (linha, coluna e bloco)
/// Se houver uma unica possibilidade de preenchimento, retorna o valor (1 a 9)
/// Se nao houver nenhuma possibilidade de preenchimento, retorna 0, o que
/// indica que o tabuleiro nao tem solucao
/// Se houver mais de uma possibilidade de preenchimento, retorna um numero
/// negativo (<= -2), cujo modulo eh o numero de possibilidades
int Sudoku::numero_possibilidades(unsigned i, unsigned j) const
{
  // A principio, todos os 9 valores de jogada sao possiveis
  bool valor_possivel[]={true,true,true,true,true,true,true,true,true};
  unsigned ii,I,jj,J,k;

  // Se a casa jah estiver preenchida, nao hah nenhuma possibilidade
  if (at(i,j)!=0) return -1;

  // Varre a linha
  for (k=0; k<9; k++)
  {
    if (at(i,k)>0) valor_possivel[at(i,k)-1] = false;
  }

  // Varre a coluna
  for (k=0; k<9; k++)
  {
    if (at(k,j)>0) valor_possivel[at(k,j)-1] = false;
  }

  // Varre a quadra
  I = 3*(i/3);
  J = 3*(j/3);
  for (ii=0; ii<3; ii++) for (jj=0; jj<3; jj++)
  {
    if (at(I+ii,J+jj)>0) valor_possivel[at(I+ii,J+jj)-1] = false;
  }

  // Conta o numero de valores possiveis
  unsigned num_possiveis=0;
  for (k=0; k<9; k++)
  {
    if (valor_possivel[k]) num_possiveis++;
  }

  // Nao hah nenhum valor possivel
  if (num_possiveis == 0) return 0;

  // Hah mais de um valor possivel
  if (num_possiveis > 1) return -num_possiveis;

  // Hah uma unica possibilidade de preenchimento da casa
  // Retorna o valor possivel
  k = 0;
  while (!valor_possivel[k]) k++;
  return k+1;
}

/// Preenche todas as casas "faceis" do tabuleiro, ou seja, aquelas que tem um
/// unico valor possivel pelas regras do Sudoku
/// Retorna o numero de casas adicionais preenhidas (0 ou >0) ou entao
/// retorna <0 se houver alguma casa sem possibilidade de jogada (tabuleiro insoluvel)
/// Quando retorna um valor negativo (ou seja, o tabuleiro eh insoluvel), o numero
/// retornado serah o negativo do numero de casas preenchidas. Caso nenhuma casa
/// tenha sido preeenchida e o tabuleiro seja insoluvel, serah retornado um numero
/// negativo menor que -81.
int Sudoku::resolver_casas_faceis()
{
  unsigned num_casas=0;
  bool insoluvel(false),resolveu_alguma;
  unsigned i,j;
  int valor;

  do
  {
    resolveu_alguma = false;
    for (i=0; i<9; i++) for (j=0; j<9; j++) if (at(i,j)==0)
    {
      valor = numero_possibilidades(i,j);
      if (valor == 0)
      {
        // Hah uma casa na qual nenhum valor eh possivel
        // O tabuleiro nao tem solucao
        insoluvel = true;
      }
      if (valor > 0)
      {
        set(i,j) = valor;
        resolveu_alguma = true;
        num_casas++;
      }
    }
  } while (resolveu_alguma);
  if (insoluvel)
  {
      if (num_casas==0) return -666;
      return (-num_casas);
  }
  return num_casas;
}

/// Determina automaticamente a solucao do tabuleiro (preenche as casas vazias).
/// O parametro com_exibicao controla se o algoritmo deve (true) ou nao (false)
/// exibir os tabuleiros analisados e o numero de nohs durante o algoritmo.
/// O parametro "basta_primeira_solucao" fixa se o algoritmo deve (true) ou nao (false)
/// retornar assim que encontrar a primeira solucao, sem verificar se existe outra.
/// Retorna 0 se nao foi encontrada solucao, 1 se foi encontrada uma solucao unica
/// e >1 se existem mais de uma solucao (essa ultima possibilidade de retorno soh
/// existe se o parametro basta_primeira_solucao for false.
///
/// ATENCAO: ESSE ALGORITMO NAO EH O CORRETO!!!
/// Eh apenas um exemplo, que tenta resolver o problema aleatoriamente
/// O parametro basta_primeira_solucao eh ignorado
/// Voces devem substituir pelo seu proprio algoritmo
///
int Sudoku::resolver(bool com_exibicao, bool basta_primeira_solucao)
{   
#define NUMERO_ITERACOES_EXIBIDAS 100
	Sudoku Origem = *this;
	stack<Sudoku> Aberto;
	stack<Jogada> joga;
	Aberto.push(*this);
	int num_testados = 0;
	bool encontrou_solucao = false;
	int num_gerados = 1;
	int m; int n; // casa a analisar
	
	do{
		Sudoku tab = Aberto.top();
		Aberto.pop();
		tab.resolver_casas_faceis();
		num_testados++;
		if(tab.fim_de_jogo() == true){encontrou_solucao = true;}  // se o tabuleiro ja tiver solucionado
		
		else{ // caso contrario
			//determina as casas vazias, escolhe a que tem menos possibilidades de jogada
			int N = tab.num_casas_vazias();
			vector<int> num;
			vector<int>::iterator ptr;
			Jogada jog[N];
			int p = 0;
			
			for(int i=0;i<9;i++){//percorre linha
				for(int j=0;j<9;j++){//percorre coluna
					if(tab.at(i,j) == 0){//testa se a casa esta vazia
						Jogada y;
						y.setCell(i,j);
						num.push_back(tab.numero_possibilidades(i,j)); // numero de possibilidades para a casa
						jog[p] = y;                                   //  jogadas respectivas
						p++;
					}
				}
			}
			
			int aux = 0;
			ptr = find(num.begin(),num.end(),0);
			if(ptr == num.end()){ // se todas as casas tem jogadas possiveis			
				int maior = num[0];
				for(int i=0;i<p;i++){
					if(num[i] >= maior){
						maior = num[i];
						m = jog[i].linha();
						n = jog[i].coluna();	
					}
				}
			}else{ // se ao menos uma casa nao tem jogada possivel
				tab.apagar_jogada(joga.top(),*this);
				joga.pop();
				aux = 1;
			}
			
			if(aux==0){ // se a casa for possivel de ser jogada
				for(int val=1;val<=9;val++){
					Jogada x(m,n,val);
					if(tab.jogada_valida(x) == true){ // se a jogada for possivel //
						joga.push(x);
						Sudoku suc = tab; // gerar sucessor
						suc.fazer_jogada(joga.top());
						Aberto.push(suc); // adicionar ao container
						num_gerados++;
					}
				}
			}else{// após a jogada ser previamente apagada
				for(int val=9;val>=1;val--){
					Jogada x(m,n,val);
					if(tab.jogada_valida(x) == true){ // se a jogada for possivel //
						joga.push(x);
						Sudoku suc = tab; // gerar sucessor
						suc.fazer_jogada(joga.top());
						Aberto.push(suc); // adicionar ao container
						num_gerados++;
					}					
				}
			}
		}
	    if((num_gerados < NUMERO_ITERACOES_EXIBIDAS) || (num_gerados%NUMERO_ITERACOES_EXIBIDAS==0)){
			tab.exibir();
			tab.exibir_andamento(num_testados, num_gerados, Aberto.size());
		}
		*this = tab;
	
	}while(!encontrou_solucao && !Aberto.empty());
	
	if(!encontrou_solucao){
		cout << "Sudoku sem solucao\n";
		cout << "Testados: " << num_testados << endl;
		*this = Origem;
	}
	
}
