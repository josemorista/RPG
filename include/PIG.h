/********************************
Versão 0.5.1 da Biblioteca Pig.h
********************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "Tipos_PIG.h"
#include "CPoolNumeros.h"
#include "CAssetLoader.h"
#include "CMapaCaracteres.h"
#include "CGerenciadorObjetos.h"
#include "CGerenciadorTimers.h"
#include "CGerenciadorParticulas.h"
#include "CGerenciadorAnimacoes.h"
#include "CMouse.h"
#include "CJogo.h"

/********************************
/ Variáveis Globais
/********************************/
Jogo jogo=NULL;

/********************************
A função CriaJogo() é responsável pela criação da janela do jogo e pelos componentes audio-visuais.
Ela deve ser chamada antes de qualquer outra função da biblioetac PIG.h. Não é possível criar dois
jogos no mesmo progama. Logo, somente a primeira chamada terá efeito.
Parâmetros:
nome_Janela (entrada, passagem por referência*): indica o nome que a janela do jogo mostrará no seu título.
É utilizado o operador * apenas por se tratar de um parâmetro string. O valor do parâmetro não é alterado dentro da função.
cursor_Proprio (entrada, passagem por valor): indica se o jogo utilizará cursores (ponteiros) próprios para o mouse ou utilizará o cursor padrão.
********************************/
void CriaJogo(char *nome_Janela,int cursor_Proprio=0)
{
    if (jogo==NULL){
        jogo = new CJogo(nome_Janela,cursor_Proprio);
        CAssetLoader::Inicia();
        if (cursor_Proprio!=0)
            CMouse::Inicia(jogo->renderer);
        CGerenciadorTimers::Inicia();
        CGerenciadorObjetos::Inicia(jogo->renderer);
        CGerenciadorParticulas::Inicia(jogo->renderer);
        CGerenciadorAnimacoes::Inicia(jogo->renderer);
    }
}

/********************************
A função GetEstadoJogo() indica o estado do jogo. Este estado é apenas um valor inteiro
que pode ser usado para indicar uma fase do jogo, ou a vez de algum jogador por exemplo.
Retorno:
inteiro indicando o valor do estado do jogo.
********************************/
int GetEstadoJogo()
{
    return jogo->GetEstado();
}

/********************************
A função SetEstadoJogo() atribui um novo valor ao estado do jogo. Este estado é apenas um valor inteiro
que pode ser usado para indicar uma fase do jogo, ou a vez de algum jogador por exemplo.
Parâmetro:
estado (entrada, passagem por valor): indica o novo estado do jogo;
********************************/
void SetEstadoJogo(int estado)
{
    jogo->SetEstado(estado);
}

/********************************
A função DefineFundo() permite escolher um arquivo de imagem como fundo (background) do jogo que será automaticamente desenhado a cada nova etapa de desenho na tela.
Parâmetros:
nome_ArquivoImagem (entrada, passagem por referência*): indica o caminho relativo ou absoluto do arquivo de imagem que será utilizado como fundo do jogo.
É utilizado o operador * apenas por se tratar de um parâmetro string. O valor do parâmetro não é alterado dentro da função.
********************************/
void DefineFundo(char *nome_ArquivoImagem)
{
    jogo->DefineFundo(nome_ArquivoImagem);
}

/********************************
A função GetTeclado() faz um mapeamento entre o teclado físico a uma variável do tipo PIG_Teclado que funciona como um vetor de variáveis lógicas,
sendo cada posição do vetor relativo a uma tecla física.
Uma lista das possíveis teclas e seus código pode ser vista no arquivo Tipos_PIG.h.
Retorno:
O valor de retorno da função é um vetor de números inteiros que indicam se uma determinada tecla (índice do vetor) foi
pressionada (valor diferente de 0,zero) ou não (valor igual a zero).
********************************/
PIG_Teclado GetTeclado()
{
    return jogo->PegaTeclado();
}

/********************************
A função GetEvento() indica se houve algum tipo de evento desde a última vez que a função foi chamada.
Um evento pode ser uma tecla pressionada, um clique do mouse ou outra coisa relacionada à tela do jogo.
Maiores informaçõe podem ser vistas no arquivo Tipo_PIG.h
Retorno:
O valor de retorno é um struct contendo o tipo de evento ocorrido e vários outros structs com informações específicas.
********************************/
PIG_Evento GetEvento()
{
    return jogo->PegaEvento();
}

/********************************
A função IniciaDesenho() é responsável por preparar a janela do jogo para que os objetos gráficos
possam ser mostrados, além de desenhar a imagem de fundo do jogo, caso tenha sido definida.
Deve ser chamada repetidamente a cada novo quadro (frame) a ser desenhado.
********************************/
void IniciaDesenho()
{
    jogo->IniciaDesenho();
}

/********************************
A função EncerraDesenho() é responsável por encerrar o desenho que foi criado, mostrando na tela do jogo.
Deve ser chamado sempre ao final do loop principal do jogo.
********************************/
void EncerraDesenho()
{
    if (jogo->cursorPadrao!=0)
        CMouse::Desenha();
    jogo->EncerraDesenho();
}

/********************************
A função SalvaTela() é responsável por criar um arquivo Bitmap, contendo uma cópia da tela do jogo naquele momento.
Parâmetros:
nomeArquivoBMP (entrada, passagem por referência*): indica o nome do arquivo Bitmap que será salvo. O valor do parâmetro deve conter a extensão ".bmp".
É utilizado o operador * apenas por se tratar de um parâmetro string. O valor do parâmetro não é alterado dentro da função.
********************************/
void SalvaTela(char *nomeArquivoBMP)
{
    jogo->SaveScreenshotBMP(nomeArquivoBMP);
}

/********************************
A função JogoRodando() é reponsável por retornar se o jogo ainda está ativo ou não.
O jogo será encerrado se a janela do jogo for fechada, por exemplo.
Retorno:
inteiro que indica se o jogo ainda está sendo executado (valor diferente de zero) ou não (valor igual a 0, zero).
********************************/
int JogoRodando()
{
    return jogo->GetRodando();
}

/********************************
A função Espera() "congela" a tela do jogo por alguns milisegundos, impedindo que qualquer outro comando seja executado enquanto isso.
Parâmetros:
tempo (entrada, passagem por valor): indica a quantidade de milisegundos durante a qual o jogo será pausado.
********************************/
void Espera(int tempo)
{
    SDL_Delay(tempo);
}

/********************************
A função FinalizaJogo() é responsável por encerrar com a PIG. Após tudo o jogo ser executado
a função deve ser chamada e ela irá realizar a liberação de memória dos elementos criados pela PIG.
********************************/
void FinalizaJogo()
{
    CGerenciadorAnimacoes::Encerra();
    CGerenciadorParticulas::Encerra();
    CGerenciadorObjetos::Encerra();
    CGerenciadorTimers::Encerra();
    if (jogo->cursorPadrao==0)
        CMouse::Encerra();
    CAssetLoader::Encerra();
    delete jogo;
}

/********************************
A função GetFPS() é responsável por calcular o valor do Frames Per Seconds (FPS), que representa a quantidade de
frames (quadros) desenhados por segundo pela PIG.
Retorno:
float que indica a média de quadros desenhados por segundo.
********************************/
float GetFPS()
{
    return jogo->GetFPS();
}

/********************************
A função CarregaCursor() é responsável por armazenar um novo cursor para o mouse, dado pelo arquivo de imagem indicado.
O arquivo deve ser uma imagem com 32 pixels de altura orbigatoriamente.
Parâmetro:
indice (entrada, passagem por valor): informa um índice para este novo cursor. Se o índice já estiver sendo utilizado,
o cursor antigo é removido. O valor deve estar entre 0..MAX_CURSORES-1.
nomeArquivoCursor (entrada, passagem por referência*): indica o nome do arquivo de imagem que será utilziado como cursor do mouse.
É utilizado o operador * apenas por se tratar de um parâmetro string. O valor do parâmetro não é alterado dentro da função.
********************************/
void CarregaCursor(int indice,char *nomeArquivoCursor)
{
    CMouse::CarregaCursor(indice,nomeArquivoCursor);
}

/********************************
A função MudaCursor() é responsável por alterar o cursor atual do mouse por outro já carregado.
Parâmetro:
indice (entrada, passagem por valor): informa um índice para este novo cursor que já deve ter sido
carregado anteriormente e deve estar no intervalo 0..MAX_CURSORES-1.
********************************/
void MudaCursor(int indice)
{
    CMouse::MudaCursor(indice);
}


/********************************
Seção de elementos geométricos
********************************/

/********************************
A função Desenha_LinhasSimples() é responsável por desenhar uma única linha, cujo início é o ponto (x1,y1) e o fim é o ponto (x2,y2).
Parâmetros:
pontoX1 (entrada, passagem por valor): inteiro que indica o início da linha no eixo X.
pontoY1 (entrada, passagem por valor): inteiro que indica o início da linha no eixo Y.
pontoX2 (entrada, passagem por valor): inteiro que indica o fim da linha no eixo X.
pontoY2 (entrada, passagem por valor): inteiro que indica o fim da linha no eixo Y.
cor (entrada, passagem por valor): indica a cor no sistema RGB para o desenho da linha;
********************************/
void DesenhaLinhaSimples(int pontoX1,int pontoY1,int pontoX2,int pontoY2, PIG_Cor cor)
{
    jogo->DesenhaLinhaSimples(pontoX1,pontoY1,pontoX2,pontoY2,cor);
}

/********************************
A função DesenhaLinhasDisjuntas() é responsável por desenhar linhas separadamente, ou seja, os ponto iniciais de cada linha estão nas posições pares e
os pontos finais estão nas posições ímpares. Assim, os vetores precisam ter tamanho qtd*2.
Parâmetros:
pontosX (entrada, passagem por referência): vetor de inteiros, de tamanho qtd*2, contendo os valores X (inicias e finais) de cada linha.
pontosY (entrada, passagem por referência): vetor de inteiros, de tamanho qtd*2, contendo os valores Y (inicias e finais) de cada linha.
qtd (entrada, passagem por valor): quantidade de linhas a serem desenhadas.
cor (entrada, passagem por valor): indica a cor no sistema RGB para o desenho das linhas;
********************************/
void DesenhaLinhasDisjuntas(int *pontosX,int *pontosY,int qtd, PIG_Cor cor)
{
    jogo->DesenhaLinhasDisjuntas(pontosX,pontosY,qtd,cor);
}

/********************************
A função DesenhaLinhasSequencia() é responsável por desenhar linhas em sequência, ou seja, o ponto final da primeira linha
corresponde ao ponto inicial da segunda linha.
Parâmetros:
pontosX (entrada, passagem por referência): vetor de inteiros, de tamanho qtd+1, contendo os valores X de cada linha da sequência.
pontosY (entrada, passagem por referência): vetor de inteiros, de tamanho qtd+1, contendo os valores Y de cada linha da sequência.
qtd (entrada, passagem por valor): quantidade de linhas a serem desenhadas.
cor (entrada, passagem por valor): indica a cor no sistema RGB para o desenho das linhas;
********************************/
void DesenhaLinhasSequencia(int *pontosX,int *pontosY,int qtd, PIG_Cor cor)
{
    jogo->DesenhaLinhasSequencia(pontosX,pontosY,qtd,cor);
}

/********************************
A função DesenhaRetangulo() é responsável por desenhar um retângulo com preenchimento sólido na tela em uma posição desejada pelo usuário.
O mesmo pode ser colocado em qualquer lugar do campo de visão da tela.
Parâmetros:
posicaoX (entrada, passagem por valor): Valor da coordenada X da tela onde o usuário deseja colocar o retângulo.
posicaoY (entrada, passagem por valor): Valor da coordenada Y da tela onde o usuário deseja colocar o retângulo.
altura (entrada, passagem por valor): Valor onde o usuário irá fornecer o número correspondente em pixels à altura desse retângulo.
largura (entrada, passagem por valor): Valor onde o usuário irá fornecer o número correspondente em pixels à largura desse retângulo.
cor (entrada, passagem por valor): indica a cor no sistema RGB para o desenho do retângulo;
********************************/
void DesenhaRetangulo(int posicaoX, int posicaoY, int altura, int largura, PIG_Cor cor)
{
    jogo->DesenhaRetangulo(posicaoX,posicaoY,altura,largura,cor);
}

/********************************
A função DesenhaRetanguloVazado() é responsável por desenhar um retângulo sem preenchimento na tela em uma posição desejada pelo usuário.
O mesmo pode ser colocado em qualquer lugar do campo de visão da tela.
Parâmetros:
posicaoX (entrada, passagem por valor): Valor da coordenada X da tela onde o usuário deseja colocar o retângulo.
posicaoY (entrada, passagem por valor): Valor da coordenada Y da tela onde o usuário deseja colocar o retângulo.
altura (entrada, passagem por valor): Valor onde o usuário irá fornecer o número correspondente em pixels à altura desse retângulo.
largura (entrada, passagem por valor): Valor onde o usuário irá fornecer o número correspondente em pixels à largura desse retângulo.
cor (entrada, passagem por valor): indica a cor no sistema RGB para a borda do retângulo;
********************************/
void DesenhaRetanguloVazado(int posicaoX, int posicaoY, int altura, int largura, PIG_Cor cor)
{
    jogo->DesenhaRetanguloVazado(posicaoX,posicaoY,altura,largura,cor);
}

/********************************
Seção de desenho Offscreen
********************************/

/********************************
A função PreparaOffScreenRenderer() serve para criar um novo bitmap offscreen que servirá de área
de desenho para outras funções que utilizem este bitmap.
Parâmetros:
altura (entrada, passagem por valor): indica a altura em pixels do novo bitmap.
largura (entrada, passagem por valor): indica a largura em pixels do novo bitmap.
********************************/
void PreparaOffScreenRenderer(int altura,int largura)
{
    jogo->PreparaOffScreenRenderer(altura,largura);
}

/********************************
A função SalvaOffScreenBMP() serve para efetivamente salvar o bitmap offscreen como um arquivo do sistema operacional.
O bitmap offscreen continuará existindo em memória e ações subsequentes ainda poderão ser feitas. Para "limpar" o bitmap
deve-se utilizar a função PintaFundoOffScreen().
Parâmetros:
nomeArquivoBMP (entrada, passagem por referência*): indica com que nome o bitmap offscreen será salvo. O valor do parâmetro deve conter a extensão ".bmp".
É utilizado o operador * apenas por se tratar de um parâmetro string. O valor do parâmetro não é alterado dentro da função.
********************************/
void SalvaOffScreenBMP(char *nomeArquivoBMP)
{
    jogo->SalvaOffScreenBMP(nomeArquivoBMP);
}

/********************************
A função PintaAreaOffScreen() serve para mudar a cor de uma área contígua de pixels da mesma cor.
A funcionalidade é semelhante à ferramenta "balde" dos editores de imagem. A área será pintada a partir do
ponto (posicaoX,posicaoY) de acordo com o sistema de coordenadas da PIG.
Parâmetros:
posicaoX (entrada, passagem por valor): indica o ponto no eixo X para começar a pintar a área contígua de pixels.
posicaoY (entrada, passagem por valor): indica o ponto no eixo Y para começar a pintar a área contígua de pixels.
cor (entrada, passagem por valor): indica a cor que deve ser usada durante a pintura da área contígua de pixels.
********************************/
void PintaAreaOffScreen(int posicaoX,int posicaoY,PIG_Cor cor)
{
    jogo->PintaAreaOffScreen(posicaoX,posicaoY,cor);
}

/********************************
A função PintaFundoOffScreen() serve para "limpar" o bitmap offscreen, mudando completamente para cor indicada.
Parâmetros:
cor (entrada, passagem por valor): indica a cor que deve ser usada durante a pintura completa do bitmap offscreen.
********************************/
void PintaFundoOffScreen(PIG_Cor cor)
{
    jogo->PintaFundoOffScreen(cor);
}

/********************************
A função DesenhaRetanguloOffScreen() é responsável por desenhar um retângulo cem preenchimento sólido no bitmap offscreen em uma posição desejada pelo usuário.
O sistema de coordenadas é o mesmo da tela, com o eixo Y aumentando para cima.
Parâmetros:
posicaoX (entrada, passagem por valor): Valor da coordenada X onde o usuário deseja colocar o retângulo.
posicaoY (entrada, passagem por valor): Valor da coordenada Y onde o usuário deseja colocar o retângulo.
altura (entrada, passagem por valor): Valor onde o usuário irá fornecer o número correspondente em pixels à altura desse retângulo.
largura (entrada, passagem por valor): Valor onde o usuário irá fornecer o número correspondente em pixels à largura desse retângulo.
cor (entrada, passagem por valor): indica a cor no sistema RGB para o preenchimento do retângulo;
********************************/
void DesenhaRetanguloOffScreen(int x1,int y1,int altura,int largura,PIG_Cor cor)
{
    jogo->DesenhaRetanguloOffScreen(x1,y1,altura,largura,cor);
}

/********************************
A função DesenhaRetanguloVazadoOffScreen() é responsável por desenhar um retângulo sem preenchimento no bitmap offscreen em uma posição desejada pelo usuário.
O sistema de coordenadas é o mesmo da tela, com o eixo Y aumentando para cima.
Parâmetros:
posicaoX (entrada, passagem por valor): Valor da coordenada X onde o usuário deseja colocar o retângulo.
posicaoY (entrada, passagem por valor): Valor da coordenada Y onde o usuário deseja colocar o retângulo.
altura (entrada, passagem por valor): Valor onde o usuário irá fornecer o número correspondente em pixels à altura desse retângulo.
largura (entrada, passagem por valor): Valor onde o usuário irá fornecer o número correspondente em pixels à largura desse retângulo.
cor (entrada, passagem por valor): indica a cor no sistema RGB para a borda do retângulo;
********************************/
void DesenhaRetanguloVazadoOffScreen(int x1,int y1,int altura,int largura,PIG_Cor cor)
{
    jogo->DesenhaRetanguloVazadoOffScreen(x1,y1,altura,largura,cor);
}

/********************************
A função DesenhaLinhasSimplesOffScreen() é responsável por desenhar no bitmap offscreen uma única linha, cuja início é o ponto (x1,y1) e o fim é o ponto (x2,y2).
Parâmetros:
pontoX1 (entrada, passagem por valor): inteiro que indica o início da linha no eixo X.
pontoY1 (entrada, passagem por valor): inteiro que indica o início da linha no eixo Y.
pontoX2 (entrada, passagem por valor): inteiro que indica o fim da linha no eixo X.
pontoY2 (entrada, passagem por valor): inteiro que indica o fim da linha no eixo Y.
cor (entrada, passagem por valor): indica a cor no sistema RGB para o desenho das linhas;
********************************/
void DesenhaLinhaSimplesOffScreen(int x1,int y1,int x2,int y2,PIG_Cor cor)
{
    jogo->DesenhaLinhaOffScreen(x1,y1,x2,y2,cor);
}

/********************************
Seção de fontes
********************************/

/********************************
A função CriaFonteNormal() é responsável por disponibilizar uma nova fonte com preenchimento sólido.
Parâmetros:
nome (entrada, passagem por referência): nome do arquivo que contém a fonte (normalmente com extensão ttf).
tamanho (entrada, passagem por valor): tamanho da fonte, que normalmente representa a altura média (em pixels) dos caracteres da fonte.
corLetra (entrada, passagem por valor): representa a cor de preenchimento da fonte.
contorno (entrada, passagem por valor): representa a espessura do contorno que fonte terá. Caso não seja desejado um contorno, basta utilizar um valor 0.
corContorno (entrada, passagem por valor): representa a cor do contorno da fonte.
estilo (entrada, passagem por valor): representa o estilo da fonte. Pode ser uma combinação binária de: ESTILO_NEGRITO, para negrito; ESTILO_SUBLINHADO, para sublinhado; ESTILO_ITALICO, para itálico;
ESTILO_CORTADO, para uma linha horizontal à meia-altura. Caso nenhum estilo seja desejado, pode ser omitido ou usado ESTILO_NORMAL.
retono:
inteiro que representa a ideintificação única da fonte. Futuras referência a esta fonte devem idenitificá-las pelo número.
********************************/
int CriaFonteNormal(char *nome,int tamanho,PIG_Cor corLetra,int contorno,PIG_Cor corContorno,PIG_Estilo estilo=ESTILO_NORMAL)
{
    return jogo->CriaFonteNormal(nome,tamanho,estilo,corLetra,contorno,corContorno);
}

/********************************
A função CriaFonteFundo() é responsável por disponibilizar uma nova fonte com preenchimento de um bitmap específico.
Parâmetros:
nome (entrada, passagem por referência): nome do arquivo que contém a fonte (normalmente com extensão ttf).
tamanho (entrada, passagem por valor): tamanho da fonte, que normalmente representa a altura média (em pixels) dos caracteres da fonte.
arquivoFundo (entrada, passagem por referência): indica o nome do arquivo de imagem que servirá de fundo para a fonte.
contorno (entrada, passagem por valor): representa a espessura do contorno que fonte terá. Caso não seja desejado um contorno, basta utilizar um valor 0.
corContorno (entrada, passagem por valor): representa a cor do contorno da fonte.
estilo (entrada, passagem por valor): representa o estilo da fonte. Pode ser uma combinação binária de: ESTILO_NEGRITO, para negrito; ESTILO_SUBLINHADO, para sublinhado; ESTILO_ITALICO, para itálico;
ESTILO_CORTADO, para uma linha horizontal à meia-altura. Caso nenhum estilo seja desejado, pode ser omitido ou usado ESTILO_NORMAL.
retono:
inteiro que representa a ideintificação única da fonte. Futuras referência a esta fonte devem idenitificá-las pelo número.
********************************/
int CriaFonteFundo(char *nome,int tamanho,char *arquivoFundo,int contorno,PIG_Cor corContorno,PIG_Estilo estilo=ESTILO_NORMAL)
{
    return jogo->CriaFonteFundo(nome,tamanho,estilo,arquivoFundo,contorno,corContorno);
}

/********************************
A função CalculaLarguraPixels() por realizar a soma das larguras de cada letra da string informada.
Parâmetros:
str (entrada, passagem por referência): string a ser escrita na tela.
numFonte (entrada, passagem por valor): número da fonte a ser utilizada. Caso o usuário não deseje uma fonte especial, será utilizada a fonte padrão (numeroFonte=0, tipo=Arial, tamanho=36, cor = Branco).
retorno:
inteiro que representa o total de pixels (no eixo x) necessários para escrever a string.
********************************/
int CalculaLarguraPixels(char *str,int numFonte=0)
{
    return jogo->GetLarguraPixels(str);
}

/********************************
A função EscreverDireita() é responsável por exibir uma string na tela de jogo, com alinhamento à direita do valor de X.
Parâmetros:
str (entrada, passagem por referência): string a ser escrita na tela.
posicaoX (entrada, passagem por valor): Valor da coordenada X da tela onde o usuário deseja começar a escrever a string.
posicaoY (entrada, passagem por valor): Valor da coordenada Y da tela onde o usuário deseja começar a escrever a string.
numFonte (entrada, passagem por valor): número da fonte a ser utilizada. Caso o usuário não deseje uma fonte especial, será utilizada a fonte padrão (numeroFonte=0, tipo=Arial, tamanho=36, cor = Branco).
********************************/
void EscreverDireita(char *str,int posicaoX,int posicaoY,int numFonte=0)
{
    jogo->EscreverDireita(str,posicaoX,posicaoY,numFonte);
}

/********************************
A função EscreverEsquerda() é responsável por exibir uma string na tela de jogo, com alinhamento à esquerda do valor de X.
Parâmetros:
str (entrada, passagem por referência): string a ser escrita na tela.
posicaoX (entrada, passagem por valor): Valor da coordenada X da tela onde o usuário deseja começar a escrever a string.
posicaoY (entrada, passagem por valor): Valor da coordenada Y da tela onde o usuário deseja começar a escrever a string.
numFonte (entrada, passagem por valor): número da fonte a ser utilizada. Caso o usuário não deseje uma fonte especial, será utilizada a fonte padrão (numeroFonte=0, tipo=Arial, tamanho=36, cor = Branco).
********************************/
void EscreverEsquerda(char *str,int posicaoX,int posicaoY,int numFonte=0)
{
    jogo->EscreverEsquerda(str,posicaoX,posicaoY,numFonte);
}

/********************************
A função EscreverCentralizada() é responsável por exibir uma string na tela de jogo, com alinhamento em relação ao valor de X.
Parâmetros:
str (entrada, passagem por referência): string a ser escrita na tela.
posicaoX (entrada, passagem por valor): Valor da coordenada X da tela onde o usuário deseja começar a escrever a string.
posicaoY (entrada, passagem por valor): Valor da coordenada Y da tela onde o usuário deseja começar a escrever a string.
numFonte (entrada, passagem por valor): número da fonte a ser utilizada. Caso o usuário não deseje uma fonte especial, será utilizada a fonte padrão (numeroFonte=0, tipo=Arial, tamanho=36, cor = Branco).
********************************/
void EscreverCentralizada(char *str,int posicaoX,int posicaoY,int numFonte=0)
{
    jogo->EscreverCentralizada(str,posicaoX,posicaoY,numFonte);
}

/********************************
Seção de objetos
********************************/

/********************************
A função CriaObjeto() é responsável por criar um objeto. Qualquer objeto que for necessário,
pode ser criado através dessa função. O objeto ainda não será desenhado, apenas criado dentro do jogo.
Parâmetros:
nomeArquivo (entrada, passagem por referência): string que informa o nome do arquivo da imagem do objeto a ser criado.
retiraFundo (entrada, passagem por valor): inteiro que indica se o fundo da imagem deve ser retirado ou não ao ler o arquivo em questão. O valor padrão é 1, indicando que o fundo deve ser retirado.
opacidade (entrada,passagem por valor): nível de opacidade do objeto na faixa 0-255. O valor padrão é 255, indicando nível máximo de opacidade.
Retorno:
inteiro que representa o identificador único do objeto. Todas as operações subsequentes com este objeto deverão receber este identificador como parâmetro.
********************************/
int CriaObjeto(char* nomeArquivo,int retiraFundo=1,int opacidade=255)
{
    return CGerenciadorObjetos::CriaObjeto(0,0,nomeArquivo,retiraFundo,opacidade);
}

/********************************
A função DestroiObjeto() é responsável por eliminar o objeto em questão do jogo.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto a ser excluído.
********************************/
void DestroiObjeto(int id_objeto)
{
    CGerenciadorObjetos::DestroiObjeto(id_objeto);
}

/********************************
A função SetValorIntObjeto() é responsável incorporar a um objeto um atributo inteiro.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
indice (entrada, passagem por valor): relaciona um número inteiro ao atributo.
valor (entrada, passagem por valor): valor do atributo inteiro a ser associado ao objeto.
********************************/
void SetValorIntObjeto(int id_objeto,int indice,int valor)
{
    CGerenciadorObjetos::SetValorIntObjeto(id_objeto,indice,valor);
}

/********************************
A função SetValorFloatObjeto() é responsável incorporar a um objeto um atributo float.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
indice (entrada, passagem por valor): relaciona um número inteiro ao atributo.
valor (entrada, passagem por valor): valor do atributo float a ser associado ao objeto.
********************************/
void SetValorFloatObjeto(int id_objeto,int indice,int valor)
{
    CGerenciadorObjetos::SetValorFloatObjeto(id_objeto,indice,valor);
}

/********************************
A função GetValorIntObjeto() é responsável recuperar o valor de um atributo int relacionado a um objeto.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
indice (entrada, passagem por valor): indica o número do atributo que se deseja recuperar.
Retorno:
Retorna o valor do atributo int solicitado pelo usuário.
********************************/
int GetValorIntObjeto(int id_objeto,int indice)
{
    return CGerenciadorObjetos::GetValorIntObjeto(id_objeto,indice);
}

/********************************
A função GetValorFloatObjeto() é responsável recuperar o valor de um atributo float relacionado a um objeto.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
indice (entrada, passagem por valor): indica o número do atributo que se deseja recuperar.
Retorno:
Retorna o valor do atributo float solicitado pelo usuário.
********************************/
float GetValorFloatObjeto(int id_objeto,int indice)
{
    return CGerenciadorObjetos::GetValorFloatObjeto(id_objeto,indice);
}

/********************************
A função GetXYObjeto() é responsável recuperar o valor da posição (X,Y) do objeto de acordo com o sistema de coordenadas do jogo.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
posicaoX (saída, passagem por referencia): indica a posicao no eixo X do objeto.
posicaoY (saída, passagem por referencia): indica a posicao no eixo Y do objeto.
********************************/
void GetXYObjeto(int id_objeto,int *posicaoX,int *posicaoY)
{
    CGerenciadorObjetos::GetPosicaoXY(id_objeto,posicaoX,posicaoY);
}

/********************************
A função MoveObjeto() é responsável por movimentar um determinado obeto para uma nova posição informada.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto a ser movido.
posicaoX (entrada, passagem por valor): Valor da coordenada X da tela onde o usuário deseja reposicionar o objeto.
posicaoY (entrada, passagem por valor): Valor da coordenada Y da tela onde o usuário deseja reposicionar o objeto.
********************************/
void MoveObjeto(int id_objeto,int posicaoX,int posicaoY)
{
    CGerenciadorObjetos::Move(id_objeto,posicaoX,posicaoY);
}

/********************************
A função SetAnguloObjetoo() é responsável pela angulação de determinado objeto. A angulação é calculada em sentido
horário a partir do eixo X (0 graus). O objeto será desenhado com a angulação informada no próximo comando
DesenhaObjeto(). A detecção de colisão não funciona com objetos fora da angulação padrão (0 graus).
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
angulo (entrada, passagem por valor): valor para indicar a angulação do objeto em graus.
********************************/
void SetAnguloObjeto(int id_objeto, float angulo)
{
    CGerenciadorObjetos::SetAngulo(id_objeto,angulo);
}

/********************************
A função GetAnguloObjeto() é responsável por recuperar o ângulo de rotação de determinado objeto. A angulação é calculada em sentido
horário a partir do eixo X (0 graus). O objeto será desenhado com a angulação informada no próximo comando
DesenhaObjeto(). A detecção de colisão não funciona com objetos fora da angulação padrão (0 graus).
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
Retorno:
Retorna o valor do ângulo em graus.
********************************/
float GetAnguloObjeto(int id_objeto)
{
    return CGerenciadorObjetos::GetAngulo(id_objeto);
}

/********************************
A função SetPivoObjeto() define um ponto (X,Y) em relação ao ponto (0,0) do objeto, sobre o qual o objeto será
rotacionado quando a função SetAnguloObjeto() for executada.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
posicaoX (entrada, passagem por valor): Valor da coordenada X do pivô em relação ao ponto (0,0) do objeto.
posicaoY (entrada, passagem por valor): Valor da coordenada Y do pivô em relação ao ponto (0,0) do objeto.
********************************/
void SetPivoObjeto(int id_objeto,int posicaoX,int posicaoY)
{
    CGerenciadorObjetos::SetPivo(id_objeto,posicaoX,posicaoY);
}

/********************************
A função GetPivoObjeto() define um ponto (X,Y) em relação ao ponto (0,0) do objeto, sobre o qual o objeto será
rotacionado quando a função SetAnguloObjeto() for executada.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
posicaoX (saída, passagem por referência): Valor da coordenada X do pivô em relação ao ponto (0,0) do objeto.
posicaoY (saída, passagem por referência): Valor da coordenada Y do pivô em relação ao ponto (0,0) do objeto.
********************************/
void GetPivoObjeto(int id_objeto,int *posicaoX,int *posicaoY)
{
    CGerenciadorObjetos::GetPivo(id_objeto,posicaoX,posicaoY);
}

/********************************
A função SetFlipObjeto() é responsável por virar o objeto, invertendo-o em alguma direção. O objeto somente será
desenhado na nova orientação no próximo comando DesenhaObjeto().
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto a ser virado.
valor (entrada, passagem por valor): valor do tipo de Flip. Pode ser FLIP_NENHUM (nenhum tipo de inversão),
FLIP_HORIZONTAL (inverte da esquerda para a direita), FLIP_VERTICAL (inverte de cima para baixo),
ou FLIP_HORIZ_VERT (inverte da esquerda para direita e de cima para baixo).
********************************/
void SetFlipObjeto(int id_objeto,PIG_Flip valor)
{
    CGerenciadorObjetos::SetFlip(id_objeto,valor);
}

/********************************
A função GetFlipObjeto() é responsável por recuperar o valor da manipulação causada pela função SetFlipObjeto().
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto a ser virado.
Retorno:
inteiro que indica o tipo de Flip. Pode ser FLIP_NENHUM (nenhum tipo de inversão),
FLIP_HORIZONTAL (inverte da esquerda para a direita), FLIP_VERTICAL (inverte de cima para baixo),
ou FLIP_HORIZ_VERT (inverte da esquerda para direita e de cima para baixo).
********************************/
PIG_Flip GetFlipObjeto(int id_objeto)
{
    return CGerenciadorObjetos::GetFlip(id_objeto);
}

/********************************
A função SetDimensoesObjeto() é responsável por delimitar a altura e a largura do objeto que será desenhado na tela,
independentemente do tamanho original do arquivo de imagem.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
altura (entrada, passagem por valor): altura em pixels.
largura (entrada, passagem por valor): largura em pixels.
********************************/
void SetDimensoesObjeto(int id_objeto, int altura, int largura)
{
    CGerenciadorObjetos::SetDimensoes(id_objeto,altura,largura);
}

/********************************
A função GetDimensoesObjeto() é responsável por recuperar a altura e a largura da área a ser usada
para desenhar o objeto na tela. Em outras palavras, representa o tamanho atual do objeto.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
altura (saída, passagem por referência): altura atual em pixels do objeto.
largura (saída, passagem por referência): largura atual em pixels do objeto.
********************************/
void GetDimensoesObjeto(int id_objeto, int *altura, int *largura)
{
    CGerenciadorObjetos::GetDimensoes(id_objeto,altura,largura);
}

/********************************
A função GetDimensoesOriginaisObjeto() é responsável por recuperar a altura e a largura originais da imagem
que foi usada para criar o objeto. Qualquer utilização da função SetDimensoesObjeto é ignorada para
o cálculo desta função.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
altura (saída, passagem por referência): altura original em pixels do objeto.
largura (saída, passagem por referência): largura original em pixels do objeto.
********************************/
void GetDimensoesOriginaisObjeto(int id_objeto, int *altura, int *largura)
{
    CGerenciadorObjetos::GetDimensoesOriginais(id_objeto,altura,largura);
}

/********************************
A função DefineFrameObjeto() é responsável por delimitar o posicionamento dos pixels do arquivo de imagem que serão
utilizados para representar o objeto na tela. Desta forma, nem toda a imagem será automaticamente utilizada para
representar o objeto. O sistema de coordenadas deve ser o padrão dos arquivos de imagem, com o eixo Y aumentando para baixo.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto a ser desenhado.
xBitmap (entrada, passagem por valor): indica a posição de eixo X onde começa o frame.
yBitmap (entrada, passagem por valor): indica a posição de eixo Y onde começa o frame. Neste caso, o eixo Y aumenta para baixo.
altura (entrada, passagem por valor): altura em pixels do frame.
largura (entrada, passagem por valor): largura em pixels do frame.
********************************/
void DefineFrameObjeto(int id_objeto,int xBitmap, int yBitmap,int altura,int largura)
{
    CGerenciadorObjetos::CriaFrame(id_objeto,xBitmap,yBitmap,altura,largura);
}

/********************************
A função SetColoracaoObjeto() é responsável por mesclar uma determinada cor com os pixels do arquivo de imagem.
Após a modificação, todos os desenhos deste objeto serão mostrados já com a mesclagem definida.
Para voltar ao padrão original de pixels, deve-se chamar a função, passando a cor branca (255,255,255).
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto a ser desenhado.
cor (entrada,passagem por valor): cor do sistema RGB utilizada para mesclagem com o arquivo de imagem
********************************/
void SetColoracaoObjeto(int id_objeto,PIG_Cor cor)
{
    CGerenciadorObjetos::SetColoracao(id_objeto,cor);
}

/********************************
A função SetOpacidadeObjeto() é responsável por modificar o nível de opacidade do objeto.
O nível de opacidade varia de 0-255, sendo 0 totalmente transparente e 255 totalmente opaco.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
valor (entrada,passagem por valor): nível de opacidade do objeto na faixa 0-255.
********************************/
void SetOpacidadeObjeto(int id_objeto,int valor)
{
    CGerenciadorObjetos::SetOpacidade(id_objeto,valor);
}

/********************************
A função DesenhaObjeto() é responsável por desenhar um objeto na tela ou no Renderizador Offscreen. O objeto será desenhado de acordo com todas as definições
de posição e ângulo informado até o momento. Além disso, se o objeto estiver virado (flipping), isso também será levado em consideração.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto a ser desenhado.
offScreen (entrada, passagem por valor): indica se o objeto deve ser desenhado no Renderizador Offscreen.
********************************/
void DesenhaObjeto(int id_objeto,int offScreen=0)
{
    if (offScreen==0)
    CGerenciadorObjetos::Desenha(id_objeto,NULL);
    else CGerenciadorObjetos::Desenha(id_objeto,jogo->offRenderer);
}

/********************************
A função TestaColisaoObjetos() é responsável por testar se dois objetos se colidiram ou não, ao passar
os identificadores dos objetos por parâmetro.
Parâmetros:
id_objeto1 (entrada, passagem por valor): identificador do primeiro objeto.
id_objeto2 (entrada, passagem por valor): identificador do segundo objeto.
Retorno:
Inteiro indicando se houve colisão (valor diferente de zero) ou não (valor igual a 0, zero).
********************************/
int TestaColisaoObjetos(int id_objeto1,int id_objeto2)
{
    return CGerenciadorObjetos::TestaColisao(id_objeto1,id_objeto2);
}

/********************************
A função GetPixelsObjeto() é responsável por extrair os pixels da imagem que originou o objeto, devolvendo-os como
uma matriz de pixels (PIG_Cor). Mudanças nessa matriz só serão validadas após a chamada da função AtualizaPixelsObjeto().
O linha 0 corresponde à primeira linha da imagem, ou seja, à linha superior, pois o sistema de coordenadas
das imagens tem o eixo Y aumentando para baixo.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
Retorno:
matriz de pixels (PIG_Cor) da imagem que gerou o objeto. O total de linhas da matriz é dado pela altura original do objeto e o
total de colunas é dado pela largura original do objeto. Ambos os valores podem ser obtidos pela função GetDimensoesOriginaisObjeto().
********************************/
PIG_Cor **GetPixelsObjeto(int id_objeto)
{
    return CGerenciadorObjetos::GetPixels(id_objeto);
}

/********************************
A função AtualizaPixelsObjeto() é responsável por efetivar quaisquer mudanças nos pixels do objeto obtidos pela função GetPixelsObjeto().
Parâmetros:
id_objeto (entrada, passagem por valor): identificador do objeto.
retiraFundo (entrada, passagem por valor): inteiro que indica se o fundo da imagem deve ser retirado ou não ao ler o arquivo em questão. O valor padrão é 1, indicando que o fundo deve ser retirado.
opacidade (entrada,passagem por valor): nível de opacidade do objeto na faixa 0-255. O valor padrão é 255, indicando nível máximo de opacidade.
********************************/
void AtualizaPixelsObjeto(int id_objeto,int retiraFundo=1,int opacidade=255)
{
    CGerenciadorObjetos::AtualizaPixels(id_objeto,retiraFundo,opacidade);
}

/********************************
Seção de gerador de partículas
********************************/

/********************************
A função CriaGeradorParticulas() cria um novo gerador de partículas (GDP). As partículas em si precisam ser criadas
posteriormente através da função Cria_Particula(), passando o identificador do GDP como parâmetro.
Parâmetros:
maxParticulas (entrada, passagem por valor): informa o maior número de partículas que o gerador terá simultaneamente. O maior valor aceito é 1000 partículas.
nomeArquivo (entrada, passagem por referência): indica o caminho relativo ou absoluto do arquivo de imagem,
 que será utilizado para cada partícula do GDP.
********************************/
int CriaGeradorParticulas(int maxParticulas,char* nomeArquivo)
{
    return CGerenciadorParticulas::CriaGeradorParticulas(maxParticulas,nomeArquivo);
}

/********************************
A função DestroiGeradorParticulas() encerra um gerador de partículas (GDP). Todas as partículas relacionadas a ele também são finalizadas
neste momento.
Parâmetros:
id_gerador (entrada, passagem por valor): informa o identificador do GDP passado como retorno da função Cria_GeradorParticulas().
********************************/
void DestroiGeradorParticulas(int id_gerador)
{
    CGerenciadorParticulas::DestroiGeradorParticulas(id_gerador);
}

/********************************
A função MoveGeradorParticulas() reposiciona um gerador de partículas (GDP). Na prática, isto significa que as novas partículas que
forem criadas posteriormente terão esse ponto de origem.
Parâmetros:
id_gerador (entrada, passagem por valor): informa o identificador do GDP passado como retorno da função Cria_GeradorParticulas().
posicaoX (entrada, passagem por valor): informa a nova posição X do GDP, em relação ao sistema de coordenadas do jogo.
posicaoY (entrada, passagem por valor): informa a nova posição Y do GDP, em relação ao sistema de coordenadas do jogo.
********************************/
void MoveGeradorParticulas(int id_gerador,int posicaoX,int posicaoY)
{
    CGerenciadorParticulas::Move(id_gerador,posicaoX,posicaoY);
}

/********************************
A função MudaDirecao_Particulas() modifica a direção na qual as partículas se movimentarão após serem criadas.
Somente as partículas criadas posteriormente seguirão a direção especificada.
Parâmetros:
id_gerador (entrada, passagem por valor): informa o identificador do GDP passado como retorno da função Cria_GeradorParticulas().
valorX (entrada, passagem por valor): informa a componente X da direção a ser definida para as partículas.
valorY (entrada, passagem por valor): informa a componente Y da direção a ser definida para as partículas.
********************************/
void MudaDirecaoParticulas(int id_gerador,int valorX,int valorY)
{
    CGerenciadorParticulas::MudaDirecaoParticulas(id_gerador,valorX,valorY);
}

/********************************
A função CriaParticula() adiciona uma nova partícula ao jogo relacionada com o GDP passado como parâmetro.
Parâmetros:
id_gerador (entrada, passagem por valor): informa o identificador do GDP passado como retorno da função Cria_GeradorParticulas().
fadingOut (entrada, passagem por valor): valor lógico que indica se a partícula deve esmaecer ao longo do tempo.
minX (entrada, passagem por valor): valor mínimo de X para que a partícula seja considerada ativa
minY (entrada, passagem por valor): valor mínimo de Y para que a partícula seja considerada ativa
maxX (entrada, passagem por valor): valor máximo de X para que a partícula seja considerada ativa
maxY (entrada, passagem por valor): valor máximo de Y para que a partícula seja considerada ativa
maxTempo (entrada, passagem por valor): tempo de vida da partícula
Retorno:
inteiro indicando se a partícula foi criada (valor maior ou igual a zero) ou não (valor menor do que zero).
A partícula não será criada se já houver o número máximo de partículas ativas.
********************************/
int CriaParticula(int id_gerador,int fadingOut=0,int minX=-50,int minY=-50,int maxX=LARG_TELA+50,int maxY=ALT_TELA+50,float maxTempo=999999.9)
{
    return CGerenciadorParticulas::CriaParticula(id_gerador,fadingOut,minX,minY,maxX,maxY,maxTempo);
}

/********************************
A função MoveParticulas() faz com que todas as partícula ativas do GDP em questão se movimentem de acordo com as componentes
X e Y passadas na função MudaDirecao_Particulas(). As partículas que saírem da tela de jogo ou aqueles que já existerm a mais de 10000 segundos
serão consideradas inativas e deixarão de existir.
Parâmetros:
id_gerador (entrada, passagem por valor): informa o identificador do GDP passado como retorno da função Cria_GeradorParticulas().
********************************/
void MoveParticulas(int id_gerador)
{
    CGerenciadorParticulas::MoveParticulas(id_gerador);
}

/********************************
A função QuantidadeParticulasAtivas() indica quantas partículas do GDP em questão ainda estão ativas. As partículas deixam de
ser ativas quando saem da tela do jogo, quando colidem com algum objeto ou depois de 10000 segundos de vida.
Parâmetros:
id_gerador (entrada, passagem por valor): informa o identificador do GDP passado como retorno da função Cria_GeradorParticulas().
Retorno:
inteiro que indica o número de partículas ativas.
********************************/
int QuantidadeParticulasAtivas(int id_gerador)
{
    return CGerenciadorParticulas::GetQtdAtivas(id_gerador);
}

/********************************
A função DesenhaParticulas() desenha todas as partículas ativas do GDP na posição que estiverem.
Parâmetros:
id_gerador (entrada, passagem por valor): informa o identificador do GDP passado como retorno da função Cria_GeradorParticulas().
********************************/
void DesenhaParticulas(int id_gerador)
{
    CGerenciadorParticulas::Desenha(id_gerador);
}

/********************************
A função ColisaoParticulasObjeto() indica se houve colisão de alguma partícula ativa do GDP com um outro objeto específico, através do seu identificador.
Parâmetros:
id_gerador (entrada, passagem por valor): informa o identificador do GDP passado como retorno da função Cria_GeradorParticulas().
id_objeto (entrada, passagem por valor): identificador do objeto que pode ter colidido com as partículas do GDP.
Retorno:
inteiro que indica se houve colisão de alguma partícula ativa do GDP (valor diferente de zero) ou não (valor igual a 0, zero).
********************************/
int ColisaoParticulasObjeto(int id_gerador,int id_objeto)
{
    Objeto obj = CGerenciadorObjetos::GetObjeto(id_objeto);
    return CGerenciadorParticulas::Colisao(id_gerador,obj);
}

/********************************
A função ColisaoParticulasAnimacao() indica se houve colisão de alguma partícula ativa do GDP com uma outra animção específica, através do seu identificador.
Parâmetros:
id_gerador (entrada, passagem por valor): informa o identificador do GDP passado como retorno da função Cria_GeradorParticulas().
id_animacao (entrada, passagem por valor): identificador da animação que pode ter colidido com as partículas do GDP.
Retorno:
inteiro que indica se houve colisão de alguma partícula ativa do GDP (valor diferente de zero) ou não (valor igual a 0, zero).
********************************/
int ColisaoParticulasAnimacao(int id_gerador,int id_animacao)
{
    Animacao anima = CGerenciadorAnimacoes::GetAnimacao(id_animacao);
    return CGerenciadorParticulas::Colisao(id_gerador,anima);
}

/********************************
Seção dos temporizadores
********************************/

/********************************
A função CriaTimer() é responsável por criar um timer (temporizador). Após a criação o tempo decorrido
poderá ser obtido a partir da função Tempo_Decorrido(), informando o identificador que é passado na resposta.
Retorno:
inteiro que identifica o timer criado.
********************************/
int CriaTimer()
{
    return CGerenciadorTimers::CriaTimer();
}

/********************************
A função TempoDecorrido() é responsável por informar o tempo decorrido desde a criação do timer
até a hora em que a função for chamada.
Parâmetros:
id_timer (entrada, passagem por valor): identificador do timer.
Retorno:
número real indicando o tempo (em segundos) desde a criação do timer.
********************************/
float TempoDecorrido(int id_timer)
{
    return CGerenciadorTimers::GetTempoDecorrido(id_timer);
}

/********************************
A função PausaTimer() é responsável por realizar uma pausa na contagem do tempo de um timer. Após a despausa,
o timer continuará a contar o tempo normalmente.
Parâmetros:
id_timer (entrada, passagem por valor): identificador do timer.
********************************/
void PausaTimer(int id_timer)
{
    CGerenciadorTimers::PausaTimer(id_timer);
}

/********************************
A função PausaTudo() é responsável por realizar uma pausa na contagem do tempo de todos os timers
criados pela função CriaTimer();
********************************/
void PausaTudo()
{
    CGerenciadorTimers::PausaTodos();
}

/********************************
A função DespausaTimer() é responsável por retomar a contagem do tempo de um timer após uma pausa.
Parâmetros:
id_timer (entrada, passagem por valor): identificador do timer.
********************************/
void DespausaTimer(int id_timer)
{
    CGerenciadorTimers::DespausaTimer(id_timer);
}

/********************************
A função DespausaTudo() é responsável por retomar a contagem do tempo de todos os timers
criados pela função CriaTimer();
********************************/
void DespausaTudo()
{
    CGerenciadorTimers::DespausaTodos();
}

/********************************
A função ReiniciaTimer() é responsável por zerar a contagem do tempo de um timer, idependente do tempo já decorrido.
Na prática, é equivalente a encerrer um temporizador e recriá-lo em seguida.
Parâmetros:
id_timer (entrada, passagem por valor): identificador do timer.
********************************/
void ReiniciaTimer(int id_timer)
{
    CGerenciadorTimers::ReiniciaTimer(id_timer);
}

/********************************
A função DestroiTimer() é responsável por encerrar a utilização de timer.
Seu identificador não poderá ser utilizado após a função ser executada.
Parâmetros:
id_timer (entrada, passagem por valor): identificador do timer.
********************************/
void DestroiTimer(int id_timer)
{
    CGerenciadorTimers::DestroiTimer(id_timer);
}

/********************************
Seção das animações
********************************/

/********************************
A função CriaAnimacao() é responsável por criar uma animacao. A animação pode ser considerada um tipo especial
de objeto, capaz de mudar de figura (sprite) dentro de um tempo determinado. Uma sequência completa de
sprites é chamada de modo. Uma mesma animção pode apresentar vários modos diferentes.
Para que a animação funcione corretamente, será necessário: definir cada frame (pedaço do arquivo bitmap que contém o sprite desejado),
criar um ou mais modos, inserir os frames criados no modo correspondente e dizer qual modo deverá ser exibido inicialmente.
Tudo isso pode ser feito por outras funções que manipulam as animações.
Parâmetros:
nomeArquivo (entrada, passagem por referência): string que informa o nome do arquivo da imagem (spritesheet) da animação.
retiraFundo (entrada, passagem por valor): inteiro que indica se o fundo da imagem deve ser retirado ou não ao ler o arquivo em questão. O valor padrão é 1, indicando que o fundo deve ser retirado.
opacidade (entrada,passagem por valor): nível de opacidade do objeto na faixa 0-255. O valor padrão é 255, indicando nível máximo de opacidade.
Retorno:
inteiro que representa o identificador único da animação. Todas as operações subsequentes com esta animação deverão receber este identificador como parâmetro.
********************************/
int CriaAnimacao(char* nomeArquivo,int retiraFundo=1,int opacidade=255)
{
    return CGerenciadorAnimacoes::CriaAnimacao(nomeArquivo,retiraFundo,opacidade);
}

/********************************
A função CriaAnimacao() é responsável por criar uma animacao. A animação pode ser considerada um tipo especial
de objeto, capaz de mudar de figura (sprite) dentro de um tempo determinado. Uma sequência completa de
sprites é chamada de modo. Uma mesma animção pode apresentar vários modos diferentes.
Esta forma de criar uma animação, copia todos os frames e modos de outra animação já existente, evitando assim
que toda esta preparação precise ser feita novamente. O usuário deve ter cuidado de não destruir a animação
original enquanto as cópias estiverem ativas ou haverá erros de execução. Da mesma forma, não se deve alterar os
frames ou os modos da cópia, pois pode provocar erros na animação original.
Parâmetros:
id_animacao (entrada, passagem por valor): indica o identificador da animação original que será copiada.
Retorno:
inteiro que representa o identificador único da animação. Todas as operações subsequentes com esta animação deverão receber este identificador como parâmetro.
********************************/
int CriaAnimacao(int id_animacao)
{
    return CGerenciadorAnimacoes::CriaAnimacao(id_animacao);
}

/********************************
A função DestroiAnimacao() é responsável por eliminar a animação em questão do jogo.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação a ser excluída.
********************************/
void DestroiAnimacao(int id_animacao)
{
    CGerenciadorAnimacoes::DestroiAnimacao(id_animacao);
}

/********************************
A função CriaFrameAnimacao() é responsável por estabelecar uma parte do sprisheet da animação que contém um sprite específico.
O frame criado deve ser posteriormente inserido em um ou mais modos da animação. O posicionamento do frame
respeita o sistema de coordenadas dos arquivos de imagem, com o eixo Y aumentando para baixo.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação a ter um novo frame.
codigo_frame (entrada, passagem por valor): numero do frame a ser criado. Pode ser usado qualquer número de 0..MAX_FRAMES-1.
xBitmap (entrada, passagem por valor): indica a posição de eixo X onde começa o frame.
yBitmap (entrada, passagem por valor): indica a posição de eixo Y onde começa o frame. Neste caso, o eixo Y aumenta para baixo.
altura (entrada, passagem por valor): altura em pixels do frame.
largura (entrada, passagem por valor): largura em pixels do frame.
********************************/
void CriaFrameAnimacao(int id_animacao,int codigo_frame,int xBitmap,int yBitmap,int altura,int largura)
{
    CGerenciadorAnimacoes::CriaFrame(id_animacao,codigo_frame,xBitmap,yBitmap,altura,largura);
}

/********************************
A função CriaModoAnimacao() é responsável por criar um modo, ou seja, uma sequência de sprites.
Após a criação do modo, os frames deve ser inseridos um a um. O modo pode ser criado antes dos frames
e vice-versa, mas os frames só podem ser associados ao modo, após as respectivas criações.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação a ter um novo modo.
codigo_modo (entrada, passagem por valor): numero do modo a ser criado. Pode ser usado qualquer número de 0..MAX_MODOS-1.
loop (entrada, passagem por valor): valor lógico que indica se este modo deve recomeçar do primeiro frame após a execução do último.
********************************/
void CriaModoAnimacao(int id_animacao,int codigo_modo,int loop)
{
    CGerenciadorAnimacoes::CriaModo(id_animacao,codigo_modo,loop);
}

/********************************
A função InsereFrameAnimacao() é responsável por associar os frames aos modos correspondentes. Um mesmo
frame pode fazer parte de diversos modos, mas cada modo deve ter no máximo MAX_FRAMES_MODO-1 frames.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação em questão.
codigo_modo (entrada, passagem por valor): numero do modo a ter um frame inserido.
codigo_frame (entrada, passagem por valor): numero do frame a ser inserido no modo.
tempo (entrada, passagem por valor): duração (em segundos) da exibição automática do frame neste modo específico.
********************************/
void InsereFrameAnimacao(int id_animacao,int codigo_modo, int codigo_frame, float tempo)
{
    CGerenciadorAnimacoes::InsereFrame(id_animacao,codigo_modo,codigo_frame,tempo);
}

/********************************
A função MudaModoAnimacao() é responsável por alterar a animação para um outro modo.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação em questão.
codigo_modo (entrada, passagem por valor): numero do novo modo.
inicia (entrada, passagem por valor): valor lógico que indica se a novo modo deve começar do primeiro sprite (valor 1) ou do último sprite (valor 0).
********************************/
void MudaModoAnimacao(int id_animacao,int codigo_modo,int inicia)
{
    CGerenciadorAnimacoes::MudaModo(id_animacao,codigo_modo,inicia);
}

/********************************
A função ColisaoAnimacoes() indica se houve colisão entre duas animações, de forma semelhante aos objetos.
Parâmetros:
id_animacao1 (entrada, passagem por valor): identificador da primeira animação.
id_animacao2 (entrada, passagem por valor): identificador da segunda animação.
Retorno:
inteiro que indica se houve colisão entre as animações.
********************************/
int ColisaoAnimacoes(int id_animacao1,int id_animacao2)
{
    return CGerenciadorAnimacoes::ColisaoAnimacoes(id_animacao1,id_animacao2);
}

/********************************
A função ColisaoAnimacaoObjeto() indica se houve colisão de alguma animacao com algum outro objeto específico, através do seu identificador.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação que pode ter colidido com o objeto.
id_objeto (entrada, passagem por valor): identificador do objeto que pode ter colidido com a animação.
Retorno:
inteiro que indica se houve colisão da animação com o objeto.
********************************/
int ColisaoAnimacaoObjeto(int id_animacao,int id_objeto)
{
    Objeto obj = CGerenciadorObjetos::GetObjeto(id_objeto);
    return CGerenciadorAnimacoes::ColisaoObjeto(id_animacao,obj);
}

/********************************
A função DesenhaAnimação() é responsável por desenhar uma animação na tela. A animação será desenhada de acordo com o modo atual da animação,
fazendo com que os frames sejam automaticamente ajustados de acordo com o tempo estipulado. Caso o modo atual esteja em loop, o primeiro frame será
desenhado novamente logo após o término do último frame.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação a ser desenhada.
Retorno:
inteiro que indica se a animação encerrou ou não. O valor só será diferente de zero, se o modo atual também não estiver em loop.
********************************/
int DesenhaAnimacao(int id_animacao)
{
    return CGerenciadorAnimacoes::Desenha(id_animacao);
}

/********************************
A função MoveAnimacao() é responsável por movimentar uma determinada animação para uma nova posição informada.
Parâmetros:
id_animação (entrada, passagem por valor): identificador da animação a ser movida.
posicaoX (entrada, passagem por valor): Valor da coordenada X da tela onde o usuário deseja reposicionar a animação.
posicaoY (entrada, passagem por valor): Valor da coordenada Y da tela onde o usuário deseja reposicionar a animação.
********************************/
void MoveAnimacao(int id_animacao,int x,int y)
{
    CGerenciadorAnimacoes::Move(id_animacao,x,y);
}

/********************************
A função SetOpacidadeAnimacao() é responsável por modificar o nível de opacidade da animação.
O nível de opacidade varia de 0-255, sendo 0 totalmente transparente e 255 totalmente opaco.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação.
valor (entrada,passagem por valor): nível de opacidade da animação na faixa 0-255.
********************************/
void SetOpacidadeAnimacao(int id_animacao,int valor)
{
    CGerenciadorAnimacoes::SetOpacidade(id_animacao,valor);
}

/********************************
A função SetColoracaoAnimacao() é responsável por mesclar uma determinada cor com os pixels do arquivo de imagem.
Após a modificação, todos os desenhos desta animação serão mostrados já com a mesclagem definida.
Para voltar ao padrão original de pixels, deve-se chamar a função, passando a cor branca (255,255,255).
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação a ser desenhado.
cor (entrada,passagem por valor): cor do sistema RGB utilizada para mesclagem com o arquivo de imagem
********************************/
void SetColoracaoAnimacao(int id_animacao,PIG_Cor cor)
{
    CGerenciadorAnimacoes::SetColoracao(id_animacao,cor);
}

/********************************
A função SetAnguloAnimacao() é responsável pela angulação de determinada animacao. A angulação é calculada em sentido
horário a partir do eixo X (0 graus). A animação será desenhada com a angulação informada no próximo comando
DesenhaAnimcao(). A detecção de colisão não funciona com animações fora da angulação padrão (0 graus).
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação.
angulo (entrada, passagem por valor): valor para indicar a angulação da animação em graus.
********************************/
void SetAnguloAnimacao(int id_animacao, float angulo)
{
    CGerenciadorAnimacoes::SetAngulo(id_animacao,angulo);
}

/********************************
A função GetAnguloAnimacao() é responsável por recuperar o ângulo de rotação de determinada animação. A angulação é calculada em sentido
horário a partir do eixo X (0 graus). A animação será desenhada com a angulação informada no próximo comando
DesenhaAnimacao(). A detecção de colisão não funciona com animações fora da angulação padrão (0 graus).
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação.
Retorno:
Retorna o valor do ângulo em graus.
********************************/
float GetAnguloAnimacao(int id_animacao)
{
    return CGerenciadorAnimacoes::GetAngulo(id_animacao);
}

/********************************
A função SetPivoAnimacao() define um ponto (X,Y) em relação ao ponto (0,0) da animação, sobre o qual a animação será
rotacionado quando a função SetAnguloAnimacao() for executada.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação.
posicaoX (entrada, passagem por valor): Valor da coordenada X do pivô em relação ao ponto (0,0) da animação.
posicaoY (entrada, passagem por valor): Valor da coordenada Y do pivô em relação ao ponto (0,0) da animação.
********************************/
void SetPivoAnimacao(int id_animacao,int posicaoX,int posicaoY)
{
    CGerenciadorAnimacoes::SetPivo(id_animacao,posicaoX,posicaoY);
}

/********************************
A função GetPivoAnimacao() define um ponto (X,Y) em relação ao ponto (0,0) da animacao, sobre o qual a animação será
rotacionado quando a função SetAnguloAnimacao() for executada.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação.
posicaoX (saída, passagem por referência): Valor da coordenada X do pivô em relação ao ponto (0,0) da animação.
posicaoY (saída, passagem por referência): Valor da coordenada Y do pivô em relação ao ponto (0,0) da animação.
********************************/
void GetPivoAnimacao(int id_animacao,int *posicaoX,int *posicaoY)
{
    CGerenciadorAnimacoes::GetPivo(id_animacao,posicaoX,posicaoY);
}

/********************************
A função SetDimensoesAnimacao() é responsável por delimitar a altura e a largura da animação que será desenhada na tela,
independentemente do tamanho original do arquivo de imagem.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação.
altura (entrada, passagem por valor): altura em pixels.
largura (entrada, passagem por valor): largura em pixels.
********************************/
void SetDimensoesAnimacao(int id_animacao, int altura, int largura)
{
    CGerenciadorAnimacoes::SetDimensoes(id_animacao,altura,largura);
}

/********************************
A função GetDimensoesAnimacao() é responsável por recuperar a altura e a largura da área a ser usada
para desenhar a animação na tela. Em outras palavras, representa o tamanho atual da animação.
Parâmetros:
id_objeto (entrada, passagem por valor): identificador da animação.
altura (saída, passagem por referência): altura atual em pixels da animação.
largura (saída, passagem por referência): largura atual em pixels da animação.
********************************/
void GetDimensoesAnimacao(int id_animacao, int *altura, int *largura)
{
    CGerenciadorAnimacoes::GetDimensoes(id_animacao,altura,largura);
}

/********************************
A função SetValorIntAnimacao() é responsável incorporar a uma animação um atributo int.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação.
indice (entrada, passagem por valor): relaciona um número inteiro ao atributo.
valor (entrada, passagem por valor): valor do atributo int a ser associado à animação.
********************************/
void SetValorIntAnimacao(int id_animacao,int indice,int valor)
{
    CGerenciadorAnimacoes::SetValorInt(id_animacao,indice,valor);
}

/********************************
A função GetValorIntAnimacao() é responsável recuperar o valor de um atributo int relacionado a uma animação.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação.
indice (entrada, passagem por valor): indica o número do atributo que se deseja recuperar.
Retorno:
Retorna o valor do atributo int solicitado pelo usuário.
********************************/
int GetValorIntAnimacao(int id_animacao,int indice)
{
    return CGerenciadorAnimacoes::GetValorInt(id_animacao,indice);
}

/********************************
A função SetValorFloatAnimacao() é responsável incorporar a uma animação um atributo float.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação.
indice (entrada, passagem por valor): relaciona um número inteiro ao atributo.
valor (entrada, passagem por valor): valor do atributo float a ser associado à animação.
********************************/
void SetValorFloatAnimacao(int id_animacao,int indice,float valor)
{
    CGerenciadorAnimacoes::SetValorFloat(id_animacao,indice,valor);
}

/********************************
A função GetValorFloatAnimacao() é responsável recuperar o valor de um atributo float relacionado a uma animação.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação.
indice (entrada, passagem por valor): indica o número do atributo que se deseja recuperar.
Retorno:
Retorna o valor do atributo float solicitado pelo usuário.
********************************/
float GetValorFloatAnimacao(int id_animacao,int indice)
{
    return CGerenciadorAnimacoes::GetValorFloat(id_animacao,indice);
}

/********************************
A função GetXYAnimacao() é responsável recuperar o valor da posição (X,Y) da animacao de acordo com o sistema de coordenadas do jogo.
Parâmetros:
id_animacao (entrada, passagem por valor): identificador da animação.
posicaoX (saída, passagem por referencia): indica a posicao no eixo X da animação.
posicaoY (saída, passagem por referencia): indica a posicao no eixo Y da animação.
********************************/
void GetXYAnimacao(int id_animacao,int *posicaoX,int *posicaoY)
{
    return CGerenciadorAnimacoes::GetPosicaoXY(id_animacao,posicaoX,posicaoY);
}
