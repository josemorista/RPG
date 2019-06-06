#include "PIG.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"

#define MAX_INIMIGOS 15
#define Tam 32

PIG_Evento evento;
PIG_Teclado meuTeclado;

/**************************
Informações iniciais para compreensão do código:

Comandos: Z para ação, X para voltar, setas direcionais para movimentação

Tipos de movimentação para entradas da matriz de cenário:
0-Não movimentável
1-Movimentável
2-Movimentável pós-evento

Tipos de evento:
1-Evento de batalha
2-Evento de diálogo
3-Evento de mudança de cenário

Estados do jogo:
0-Menu inicial
1-Primeira fase do jogo
2-Segunda fase do jogo


**************************/

/***************************
Estruturas de dados
***************************/

typedef struct Inimigo
{
    char nome[20];
    int xp,id,ataque,defesa,hp;

}Inimigo;

typedef struct Habilidade
{
    int id_animacao,dano,gasto_mana,id_btn;
    struct Habilidade *prox;
}Habilidade;

typedef struct Heroi
{
    int id,posi,posj,timer_movimentacao,posiant,posjant;
    int xp,lvl,ataque,defesa,manamax,mana,hpmax,hp;
    int potion,elixir;
    Habilidade *skills;

}Heroi;


typedef struct Eventojogo
{
    int tipoevento,idinimigo,nfalas,novoestadojogo;
    char falas[10][50];

}Eventojogo;

typedef struct campo
{
    int movimentavel,neventos,evrepetivel,imgpropria,aotocar;
    /**
    A variavel aotocar define se o evento sera iniciado assim que o personagem pisar no evento ou se
    a tecla de ação deve ser pressionada para acessá-lo
    **/
    Eventojogo eventos[5];

}Cenario;

typedef struct ListaImgsEventos //Lista de eventos que possuem imagens a serem desenhadas e que não podem ser incorporadas ao arquivo estático de fundo
{
    int idobj,posi,posj,desloc_x,desloc_y;
    struct ListaImgsEventos *prox;

}ListaImgsEventos;

/***************************
Variaveis globais
***************************/

//Variaveis jogo
Heroi heroi; //Heroi do jogo
Inimigo inimigos[MAX_INIMIGOS]; //Inimigos disponíveis para confronto
Cenario **cenario; //Cenario de cada fase
ListaImgsEventos *imgs_ev=NULL; //Lista dos eventos a serem desenhados em cada fase
int fundo,linhas_cenario=20,colunas_cenario=26,fundobatalha,provacoes=3;

//Fontes e timers
int fonte_himalaya;

//Camera
int cx=0,cy=0,cxant=0,cyant=0;


/***************************
Protótipos Funções
***************************/
void interpretar_eventos(int posi,int posj);
void Game_Over();
int CriaAnimacaoSkill(char *nomearq,int n_frames);

/***************************
Funções Auxiliares
***************************/

void animacao_batalha()
{
    float fator=0.2;
    int vortex,timer_animacao=CriaTimer();
    vortex=CriaObjeto("..//imagens//fundos//vortex.png",0,255);
    MoveObjeto(vortex,-297,-194);
    SetPivoObjeto(vortex,713,548);
    while(TempoDecorrido(timer_animacao)<1.5)
    {
        IniciaDesenho();
        DesenhaObjeto(vortex);
        SetOpacidadeObjeto(vortex,(1-TempoDecorrido(timer_animacao)/1.5)*255);
        SetAnguloObjeto(vortex,fator+GetAnguloObjeto(vortex));
        EncerraDesenho();
    }
    DestroiObjeto(vortex);
}

void GETXY_Posij(int i,int j,int *x,int *y)
{
    float t=TempoDecorrido(heroi.timer_movimentacao);
    int xcam,ycam;
    if(t<=0.3)
    {
        xcam=(cxant*(1-(t/0.3)))+(cx*(t/0.3));
        ycam=(cyant*(1-(t/0.3)))+(cy*(t/0.3));
    }else
    {
        xcam=cx;
        ycam=cy;
    }
    *x=(j*Tam)-xcam;
    *y=(linhas_cenario-1-i)*Tam-ycam;
}

void ImprimeFPS()
{
    float fps=GetFPS();
    char aux2[10];
    sprintf(aux2,"Fps:%.0f",fps);
    EscreverEsquerda(aux2,0,0,fonte_himalaya);
}

Habilidade* add_skill(Habilidade *nova)
{
    nova->prox=heroi.skills;
    return nova;
}

void upar_Heroi(int xpinimigo)
{
    heroi.xp+=xpinimigo;
    Habilidade *novo;
        while(heroi.xp >= ((2*heroi.lvl-1)*100))
        {
            heroi.lvl++;
            heroi.ataque+=10;
            heroi.defesa+=10;
            heroi.hpmax+=10;
            heroi.hp=heroi.hpmax;
            heroi.manamax+=5;
            heroi.mana=heroi.manamax;
            if(heroi.lvl==2)
            {
                printf("Nova skill ganha!\n");
                novo=(Habilidade*)malloc(sizeof(Habilidade));
                novo->dano=50;
                novo->gasto_mana=10;
                novo->id_btn=CriaObjeto("..//imagens//Botoes//btnThunder.png",0);
                MoveObjeto(novo->id_btn,11,86);
                novo->id_animacao=CriaAnimacaoSkill("..//imagens//Animacoes//thunder.png",7);
                heroi.skills=add_skill(novo);

            }

        }


}

int TestaVencedor(Inimigo *inimigo,int hpinimigo)
{
        if(hpinimigo<=0)
        {

            upar_Heroi(inimigo->xp);
            EncerraDesenho();
            return 1;
        }
        if(heroi.hp<=0)
        {
            EncerraDesenho();
            return 0;
        }
        return -1;
}

void Destruir_Lista_Imgs_Evs(ListaImgsEventos *l)
{
    //Libera espaço da memória destruindo a lista de eventos a serem desenhados
    if(l!=NULL)
    {
        DestroiObjeto(l->idobj);
        Destruir_Lista_Imgs_Evs(l->prox);
        free(l);
    }
}
ListaImgsEventos *Add_Img_Evento(ListaImgsEventos *l,char *nomearq,int posi,int posj,int x,int y)
{
    //Recebe um novo evento que deve ser desenhado e o adiciona na lista.
    if(l==NULL)
    {
        ListaImgsEventos *novo;
        novo=(ListaImgsEventos*)malloc(sizeof(ListaImgsEventos));
        novo->idobj=CriaObjeto(nomearq,0,255);
        novo->posi=posi;
        novo->posj=posj;
        novo->desloc_x=x;
        novo->desloc_y=y;
        return novo;

    }else
    {
        l->prox=Add_Img_Evento(l->prox,nomearq,posi,posj,x,y);
        return l;
    }
}

 ListaImgsEventos *remove_Img_Ev(ListaImgsEventos *l,int i,int j)
{
    if(l==NULL)
    {
        return NULL;

    }else if(l->posi==i && l->posj==j)
    {
        ListaImgsEventos *aux=l->prox;
        DestroiObjeto(l->idobj);
        free(l);
        return l->prox;
    }else
    {
        l->prox=remove_Img_Ev(l->prox,i,j);
        return l;
    }
}

void Desenha_Imgs_Evs(ListaImgsEventos *l)
{
    if(l!=NULL)
    {
        int x,y;
        GETXY_Posij(l->posi,l->posj,&x,&y);
        MoveObjeto(l->idobj,x+l->desloc_x,y+l->desloc_y);
        DesenhaObjeto(l->idobj);
        Desenha_Imgs_Evs(l->prox);
    }
}

int Mouse_sobre_obj(int idobjeto)
{
    int x,y,xmouse,ymouse,altura,largura;
    xmouse=evento.mouse.posX;
    ymouse=evento.mouse.posY;
    GetXYObjeto(idobjeto,&x,&y);
    GetDimensoesObjeto(idobjeto,&altura,&largura);
    if(xmouse>=x && xmouse<=x+largura && ymouse>=y && ymouse<=y+altura)
    {

        return 1;
    }else
    {

        return 0;
    }
}

void Criar_Heroi()
{
    int modo,i;
    FILE *arq;
    heroi.id=CriaAnimacao("..//imagens//Animacoes//heroisprite.png",0,255);
    /**
    Processo de criação da animação do herói, a id da animação será armazenada em heroi.id
    **/
    for(modo=0;modo<4;modo++)
    {
        CriaModoAnimacao(heroi.id,modo,0);
        for(i=0;i<3;i++)
        {
            CriaFrameAnimacao(heroi.id,modo*4+i,i*32,modo*32,32,32);
            InsereFrameAnimacao(heroi.id,modo,modo*4+i,0.1);
        }
    }
    SetDimensoesAnimacao(heroi.id,Tam,Tam);
    MudaModoAnimacao(heroi.id,0,1);
    /**
    Processo de leitura dos atributos iniciais do herói
    **/
    arq=fopen("..//databases//personagens//heroi.txt","rt");
    fscanf(arq,"%d %d %d %d %d\n",&heroi.lvl,&heroi.ataque,&heroi.defesa,&heroi.hpmax,&heroi.manamax);
    heroi.hp=heroi.hpmax;
    heroi.mana=heroi.manamax;
    heroi.skills=NULL;
    heroi.potion=0;
    heroi.elixir=0;
    heroi.xp=0;

    heroi.posi=17;
    heroi.posj=0;
    heroi.posiant=17;
    heroi.posjant=0;

    heroi.timer_movimentacao=CriaTimer();

    fclose(arq);
}

int CriaAnimacaoSkill(char *nomearq,int n_frames)
{
    int i=0,j=0,ani;
    ani=CriaAnimacao(nomearq,0,255);
    CriaModoAnimacao(ani,0,0);
    while(i<n_frames)
    {
        CriaFrameAnimacao(ani,i,i*192,j*174,174,192);
        InsereFrameAnimacao(ani,0,i,0.1);
        i++;
        if(i%5==0)
        {
            j++;
        }
    }
    MudaModoAnimacao(ani,0,1);
    MoveAnimacao(ani,416-80,320-70);
    SetDimensoesAnimacao(ani,149,160);
    return ani;
}

void CalculaXYInimigo(int idinimigo,int *x,int *y)
{
    int altura,largura;
    GetDimensoesObjeto(idinimigo,&altura,&largura);
    *x=416-largura/2;
    *y=320-altura/2;
}

int CalculaDano(int ataque,int defesa)
{
    float dano=ataque*(1-(defesa/100.0));
    if(dano<0)
    {
        return 0;
    }else
    {
        int aux=dano;
        return aux;
    }
}

void Is_Evento()
{
    if(cenario[heroi.posi][heroi.posj].neventos>0)
    {
        if(cenario[heroi.posi][heroi.posj].aotocar)
        {
            interpretar_eventos(heroi.posi,heroi.posj);
            heroi.posi=heroi.posiant;
            heroi.posj=heroi.posjant;
            int x,y;
            GETXY_Posij(heroi.posi,heroi.posj,&x,&y);
            MoveAnimacao(heroi.id,x,y);
            MoveObjeto(fundo,-cx,-cy);
        }else
        {
            if(evento.tipoEvento==EVENTO_TECLADO && evento.teclado.acao==TECLA_PRESSIONADA && evento.teclado.tecla==TECLA_z)
            {
                interpretar_eventos(heroi.posi,heroi.posj);
            }
        }
    }
}

/**************************
Funções de interpretação
**************************/

void Carrega_inimigos(char *nomearq)
{
    /***
    Essa função lê de um arquivo recebido os atributos e os nomes dos arquivos de
    imagem a serem utilizadas nas batalhas com os inimigos.
    ***/
    int i=0;
    char aux[20],aux2[40];
    FILE *arq=fopen(nomearq,"rt");
    if(arq==NULL){
        printf("Arquivo de inimigos inexistente");
        exit(0);
    }
    while(!feof(arq))
    {
        fscanf(arq,"%s %d %d %d %d\n%s\n",inimigos[i].nome,&inimigos[i].hp,&inimigos[i].ataque,&inimigos[i].defesa,&inimigos[i].xp,aux);
        sprintf(aux2,"..//imagens//Inimigos//%s",aux);
        inimigos[i].id=CriaObjeto(aux2,0,255);
        i++;
    }
    inimigos[i].id=-1;//Para saber quando acabam os inimigos carregados
    fclose(arq);
    system("cls");
}

void Carrega_cenario(char *nomearq,int altura,int largura)
{
    FILE *arq;
    arq=fopen(nomearq,"rt"); //Carrega arquivo com os dados do cenario
    if(arq==NULL)
    {
        printf("Arquivo de cenario %s inexistente!\n",nomearq);
        exit(0);
    }

    linhas_cenario=altura/Tam;
    colunas_cenario=largura/Tam;
    //Alocacao dinamica da matriz do cenario
    int i,j;
    cenario=(Cenario**)malloc(sizeof(Cenario*)*linhas_cenario);
    for(i=0;i<linhas_cenario;i++)
    {
        cenario[i]=(Cenario*)malloc(sizeof(Cenario)*colunas_cenario);
    }

    //Inicialização da matriz de cenario com valores default

    for(i=0;i<linhas_cenario;i++)
    {
        for(j=0;j<colunas_cenario;j++)
        {
            cenario[i][j].movimentavel=1;
            cenario[i][j].neventos=0;
        }
    }

    //Leitura efetiva dos eventos  do arquivo de cenario
    fscanf(arq,"%d %d\n",&heroi.posi,&heroi.posj);

    heroi.posiant=heroi.posi;
    heroi.posjant=heroi.posj;
    int x,y;
    GETXY_Posij(heroi.posi,heroi.posj,&x,&y);
    MoveAnimacao(heroi.id,x,y);
    while(!feof(arq))
    {
        int k;
        fscanf(arq,"%d %d ",&i,&j);
        fscanf(arq,"%d ",&cenario[i][j].movimentavel);
        fscanf(arq,"%d\n",&cenario[i][j].neventos);
        if(cenario[i][j].neventos!=0)
        {
            fscanf(arq,"%d ",&cenario[i][j].aotocar);
            fscanf(arq,"%d ",&cenario[i][j].evrepetivel);
            fscanf(arq,"%d\n",&cenario[i][j].imgpropria);
            if(cenario[i][j].imgpropria)
            {
                char nomearq[40];
                int desloc_x,desloc_y;
                fscanf(arq,"%s %d %d\n",nomearq,&desloc_x,&desloc_y);
                imgs_ev=Add_Img_Evento(imgs_ev,nomearq,i,j,desloc_x,desloc_y);
            }
        }
        for(k=0;k<cenario[i][j].neventos;k++)
        {
            fscanf(arq,"%d ",&cenario[i][j].eventos[k].tipoevento);
            if(cenario[i][j].eventos[k].tipoevento==1)
            {
                fscanf(arq,"%d\n",&cenario[i][j].eventos[k].idinimigo);
            }else if(cenario[i][j].eventos[k].tipoevento==2)
            {
                int l;
                fscanf(arq,"%d\n",&cenario[i][j].eventos[k].nfalas);
                for(l=0;l<cenario[i][j].eventos[k].nfalas;l++)
                {

                    int m=0;
                    do
                    {
                        fscanf(arq,"%c",&cenario[i][j].eventos[k].falas[l][m]);
                        m++;
                    }while(cenario[i][j].eventos[k].falas[l][m-1]!=';');
                    cenario[i][j].eventos[k].falas[l][m-1]='\0';
                    fscanf(arq,"\n");
                }
            }else if(cenario[i][j].eventos[k].tipoevento==3)
            {
                    fscanf(arq,"%d\n",&cenario[i][j].eventos[k].novoestadojogo);
            }
        }
    }
    fclose(arq);

}

void Executar_Dialogo(char falas[10][50],int nfalas,char* fundo_dialogo)
{
    int tela_atual,caixadialogo,i=0;
    tela_atual=CriaObjeto(fundo_dialogo,0,255);
    caixadialogo=CriaObjeto("..//imagens//Others//caixadialogo.png",0);
    MoveObjeto(caixadialogo,12,8);


    EncerraDesenho();
    while(i<nfalas)
    {
        evento=GetEvento();

        IniciaDesenho();

        DesenhaObjeto(tela_atual);
        DesenhaObjeto(caixadialogo);
        EscreverEsquerda(falas[i],37,91,fonte_himalaya);


        if(evento.tipoEvento==EVENTO_TECLADO && evento.teclado.acao==TECLA_PRESSIONADA && evento.teclado.tecla==TECLA_z)
        {
            i++;
        }

        EncerraDesenho();
    }

    DestroiObjeto(tela_atual);
    DestroiObjeto(caixadialogo);
}

Habilidade* Escolher_Skill()
{
    int i;
    Habilidade *aux=heroi.skills;
    while(aux!=NULL)
    {
        if(Mouse_sobre_obj(aux->id_btn))
        {

            if(evento.tipoEvento==EVENTO_MOUSE && evento.mouse.acao == MOUSE_PRESSIONADO && evento.mouse.botao==MOUSE_ESQUERDO){
                return aux;
            }
        }
        aux=aux->prox;
    }
    return NULL;
}

int Executar_Batalha(int idinimigo)
{
    /**
    Essa funcao executa uma batalha e retorna seu vencedor,são permitidos ataques normais,uso de poções
    e skills.
    A batalha se desenrola por turnos controlados pela variável vez e termina quando o hp de um dos combatentes
    for menor ou igual a 0.
    **/

    Inimigo *inimigo=&inimigos[idinimigo];
    int timer_animacao,xinimigo,first_time=1,yinimigo,hpinimigo,vez=1,vencedor=-1,menuBatalha,btnAtacar,btnSkills,btnItens;
    int altura,largura,Animacao_atqnormal=CriaAnimacaoSkill("..//imagens//Animacoes//sword1.png",6),animacao=-1;
    int barra_hp,fundo_hp_barra,barra_mana,fundo_mana_barra;
    int timer_atq_inimigo,btnpotion,btnElixir,btnSkillsAcionado=0;




    timer_atq_inimigo=CriaTimer();
    timer_animacao=CriaTimer();

    hpinimigo=inimigo->hp;
    CalculaXYInimigo(inimigo->id,&xinimigo,&yinimigo);
    GetDimensoesObjeto(inimigo->id,&altura,&largura);
    MoveObjeto(inimigo->id,xinimigo,yinimigo);


    //btnpotion=CriaObjeto("..//imagens//btnPotion.png",0);
    //btnElixir=CriaObjeto("..//imagens//btnElixir.png",0);

    fundo_hp_barra=CriaObjeto("..//imagens//Others//fundo_hp_bar.png",0);
    fundo_mana_barra=CriaObjeto("..//imagens//Others//fundo_mana_bar.png",0);
    MoveObjeto(fundo_hp_barra,109,611);
    MoveObjeto(fundo_mana_barra,109,592);

    SetDimensoesObjeto(fundo_hp_barra,14,heroi.hpmax+2);
    SetDimensoesObjeto(fundo_mana_barra,14,heroi.manamax+2);

    barra_hp=CriaObjeto("..//imagens//Others//hp_bar.png",0);
    barra_mana=CriaObjeto("..//imagens//Others//mana_bar.png",0);
    MoveObjeto(barra_hp,110,612);
    MoveObjeto(barra_mana,110,593);


    menuBatalha=CriaObjeto("..//imagens//fundos//menubatalha.png",0,255);
    btnAtacar= CriaObjeto("..//imagens//Botoes//btnAtacar.png",0,255);
    MoveObjeto(btnAtacar,258,21);
    btnSkills= CriaObjeto("..//imagens//Botoes//btnHabilidades.png",0,255);
    MoveObjeto(btnSkills,11,21);
    btnItens= CriaObjeto("..//imagens//Botoes//btnItens.png",0,255);
    MoveObjeto(btnItens,587,21);

    EncerraDesenho();

    animacao_batalha();

    while(vencedor==-1)
    {

        evento=GetEvento();

        if(animacao==-1)
        vencedor=TestaVencedor(inimigo,hpinimigo);


        IniciaDesenho();

        DesenhaObjeto(fundobatalha);
        DesenhaObjeto(menuBatalha);
        DesenhaObjeto(inimigo->id);
        DesenhaObjeto(btnAtacar);
        DesenhaObjeto(btnSkills);
        DesenhaObjeto(btnItens);

        //Desenho das Barras de hp e mana do heroi
        SetDimensoesObjeto(barra_hp,12,heroi.hp);
        DesenhaObjeto(barra_hp);
        DesenhaObjeto(fundo_hp_barra);
        SetDimensoesObjeto(barra_mana,12,heroi.mana);
        DesenhaObjeto(barra_mana);
        DesenhaObjeto(fundo_mana_barra);


        //Desenho da barra de hp do inimigo
        DesenhaRetangulo(xinimigo+largura/2-(inimigo->hp/2),yinimigo-15,12,(hpinimigo),VERMELHO);
        DesenhaRetanguloVazado(xinimigo-1+largura/2-(inimigo->hp/2),yinimigo-15,13,(inimigo->hp+1),BRANCO);

        //execução da animação da skill solicitada
        if(TempoDecorrido(timer_animacao)<0.25 && !first_time)
        {
            if(animacao!=-1)
                DesenhaAnimacao(animacao);
                SetOpacidadeObjeto(inimigo->id,(1-TempoDecorrido(timer_animacao))*255);
        }else
        {
            SetOpacidadeObjeto(inimigo->id,255);
            animacao=-1;
        }

        //define de quem é o turno
        if(vez && TempoDecorrido(timer_atq_inimigo)>0.8)
        {

                if(btnSkillsAcionado==1)
                {
                    Habilidade *aux;
                    for(aux=heroi.skills;aux!=NULL;aux=aux->prox)
                    {
                        DesenhaObjeto(aux->id_btn);
                    }
                    Habilidade *skill=NULL;
                    skill=Escolher_Skill();
                    if(animacao==-1 && skill!=NULL){

                            if(heroi.mana-skill->gasto_mana>=0){
                            heroi.mana-=skill->gasto_mana;
                            hpinimigo=hpinimigo-CalculaDano(skill->dano,inimigo->defesa);
                            if(hpinimigo<0)hpinimigo=0;
                            vez=0;
                            btnSkillsAcionado=0;
                            animacao=skill->id_animacao;
                            MudaModoAnimacao(animacao,0,1);
                            ReiniciaTimer(timer_animacao);
                            }
                        }
                }
                if(evento.tipoEvento==EVENTO_MOUSE && evento.mouse.acao == MOUSE_PRESSIONADO && evento.mouse.botao==MOUSE_ESQUERDO)
                {

                    if(Mouse_sobre_obj(btnAtacar)){

                        if(animacao==-1){
                            animacao=Animacao_atqnormal;
                            MudaModoAnimacao(animacao,0,1);
                            ReiniciaTimer(timer_animacao);
                        }
                        hpinimigo=hpinimigo-CalculaDano(heroi.ataque,inimigo->defesa);
                        if(hpinimigo<0)hpinimigo=0;
                        vez=0;
                        first_time=0;
                    }
                    if(Mouse_sobre_obj(btnSkills))
                    {

                        if(btnSkillsAcionado==1)
                        {
                            btnSkillsAcionado=0;
                        }else
                        {
                            btnSkillsAcionado=1;
                        }
                        first_time=0;
                    }

                }


        }else if(vez==0 && TempoDecorrido(timer_animacao)>0.75)
        {
            heroi.hp=heroi.hp-CalculaDano(inimigo->ataque,heroi.defesa);
             if(heroi.hp<0)heroi.hp=0;
            ReiniciaTimer(timer_atq_inimigo);
            vez=1;
            first_time=0;

        }else if(animacao==-1 && TempoDecorrido(timer_atq_inimigo)<=0.8 && !first_time)
        {
            if(TempoDecorrido(timer_atq_inimigo)<0.2 && vencedor==-1)
            {
                DesenhaRetangulo(0,0,640,832,VERMELHO);
            }

        }

        EncerraDesenho();

    }

    if(vencedor==1)
    {
            char fala[10][50];
            sprintf(fala[0],"Voce derrotou %s!",inimigo->nome);
            sprintf(fala[1],"XP recebida: %d",inimigo->xp);
            SalvaTela("tela_atual.bmp");
            Executar_Dialogo(fala,2,"tela_atual.bmp");
            remove("tela_atual.bmp");
            animacao_batalha();
    }else
    {
            char fala[10][50];
            sprintf(fala[0],"Voce foi derrotado!");
            SalvaTela("tela_atual.bmp");
            Executar_Dialogo(fala,1,"tela_atual.bmp");
            remove("tela_atual.bmp");
    }

    return vencedor;
}

void interpretar_eventos(int posi,int posj)
{
    int i;
    SalvaTela("telaatual.bmp");
    for(i=0;i<cenario[posi][posj].neventos;i++)
    {
        switch(cenario[posi][posj].eventos[i].tipoevento)
        {
        case 1 :
            {
                int winner=Executar_Batalha(cenario[posi][posj].eventos[i].idinimigo);
                if(winner==0)
                {
                    Game_Over();
                }
                break;
            }
        case 2 :
            {
                Executar_Dialogo(cenario[posi][posj].eventos[i].falas,cenario[posi][posj].eventos[i].nfalas,"telaatual.bmp");
                break;
            }
        case 3 :
            {
                SetEstadoJogo(cenario[posi][posj].eventos[i].novoestadojogo);
                Destruir_Lista_Imgs_Evs(imgs_ev);
                DestroiObjeto(fundo);
                free(cenario);
                imgs_ev=NULL;
                cx=0;
                cy=0;
                break;
            }
        }
    }
    if(cenario[posi][posj].evrepetivel==0)
    {
        cenario[posi][posj].neventos=0;
        if(cenario[posi][posj].imgpropria!=0){
            imgs_ev=remove_Img_Ev(imgs_ev,posi,posj);
        }
        if(cenario[posi][posj].movimentavel==2)
        {
            cenario[posi][posj].movimentavel==1;
        }else
        {
            cenario[posi][posj].movimentavel==0;
        }
    }
    remove("telaatual.bmp");

}

void movimentar_heroi()
{
    int i,j,x,y;

    if(TempoDecorrido(heroi.timer_movimentacao)>0.3)
    {
        heroi.posiant=heroi.posi;
        heroi.posjant=heroi.posj;
        cxant=cx;
        cyant=cy;
        GETXY_Posij(heroi.posi,heroi.posj,&x,&y);
        if(meuTeclado[TECLA_CIMA])
        {
            i=heroi.posi;
            j=heroi.posj;
            if((i-1>=0) && cenario[i-1][j].movimentavel==1){
                heroi.posi--;
                MudaModoAnimacao(heroi.id,3,1);
                ReiniciaTimer(heroi.timer_movimentacao);
                if(y>416)
                {

                    cy+=Tam;
                }
            }
        }
        else if(meuTeclado[TECLA_BAIXO])
        {
            i=heroi.posi;
            j=heroi.posj;
            if((i+1<linhas_cenario) && cenario[i+1][j].movimentavel==1){
                heroi.posi++;
                MudaModoAnimacao(heroi.id,0,1);
                ReiniciaTimer(heroi.timer_movimentacao);
                if(y<160)
                {

                    cy-=Tam;
                }
            }
        }
        else if(meuTeclado[TECLA_ESQUERDA])
        {
            i=heroi.posi;
            j=heroi.posj;
            if((j-1>=0) && cenario[i][j-1].movimentavel==1){
                heroi.posj--;
                MudaModoAnimacao(heroi.id,1,1);
                ReiniciaTimer(heroi.timer_movimentacao);
                if(x<160)
                {

                    cx-=Tam;
                }
            }
        }
        else if(meuTeclado[TECLA_DIREITA])
        {
            i=heroi.posi;
            j=heroi.posj;
            if((j+1<colunas_cenario) && cenario[i][j+1].movimentavel==1){
                heroi.posj++;
                MudaModoAnimacao(heroi.id,2,1);
                ReiniciaTimer(heroi.timer_movimentacao);
                if(x>576)
                {

                    cx+=Tam;
                }
            }
        }

    }
    if(TempoDecorrido(heroi.timer_movimentacao)<=0.3)
    {
            int x,y,xant,yant,xcam,ycam;
            float t=TempoDecorrido(heroi.timer_movimentacao);
            GETXY_Posij(heroi.posi,heroi.posj,&x,&y);
            GETXY_Posij(heroi.posiant,heroi.posjant,&xant,&yant);
            x=(xant*(1-(t/0.3)))+(x*(t/0.3));
            y=(yant*(1-(t/0.3)))+(y*(t/0.3));
            MoveAnimacao(heroi.id,x,y);
            xcam=(cxant*(1-(t/0.3)))+(cx*(t/0.3));
            ycam=(cyant*(1-(t/0.3)))+(cy*(t/0.3));
            MoveObjeto(fundo,-xcam,-ycam);
    }

}


void Animacao_inicial()
{
    int i,timer=CriaTimer();
    int fnd1,fnd2,fnd3;
    int fala[8];
    float t;
    fnd1=CriaObjeto("..//imagens//Others//tela1_intro.png",0,255);
    fnd2=CriaObjeto("..//imagens//Others//tela2_intro.png",0,255);
    fnd3=CriaObjeto("..//imagens//Others//tela3_intro.png",0,255);
    for(i=0;i<8;i++)
    {
        char aux[20];
        sprintf(aux,"..//imagens//Others//fala%d.png",i);
        fala[i]=CriaObjeto(aux,0,255);
        SetOpacidadeObjeto(fala[i],0);
    }
    MoveObjeto(fala[0],59,403);
    MoveObjeto(fala[1],22,289);
    MoveObjeto(fala[3],25,560);
    MoveObjeto(fala[4],4,448);
    MoveObjeto(fala[5],10,360);
    MoveObjeto(fala[6],206,473);
    MoveObjeto(fala[7],318,402);


    while(TempoDecorrido(timer)<22.5)
    {
        t=TempoDecorrido(timer);
        evento=GetEvento();
        IniciaDesenho();

        if(t<7.5)
        {

           if(t<5.0){
                if(t<2.5)
                SetOpacidadeObjeto(fala[0],(t/2.5)*255);
                else
                SetOpacidadeObjeto(fala[1],((t-2.5)/2.5)*255);
           }else
           {
               SetOpacidadeObjeto(fala[0],((1-(t-5.0)/2.5))*255);
               SetOpacidadeObjeto(fala[1],((1-(t-5.0)/2.5))*255);
               SetOpacidadeObjeto(fnd1,((1-(t-5.0)/2.5))*255);
           }
            DesenhaObjeto(fnd1);
            DesenhaObjeto(fala[0]);
            DesenhaObjeto(fala[1]);
        }
        if(t>7.5 && t<15.0)
        {
            t-=7.5;
           if(t<5.0){
                if(t<2.5){
                    SetOpacidadeObjeto(fala[3],(t/2.5)*255);
                    SetOpacidadeObjeto(fala[4],(t/2.5)*255);
                }
                else
                SetOpacidadeObjeto(fala[5],((t-2.5)/2.5)*255);
           }else
           {
               SetOpacidadeObjeto(fala[3],((1-(t-5.0)/2.5))*255);
               SetOpacidadeObjeto(fala[4],((1-(t-5.0)/2.5))*255);
               SetOpacidadeObjeto(fala[5],((1-(t-5.0)/2.5))*255);
               SetOpacidadeObjeto(fnd2,((1-(t-5.0)/2.5))*255);
           }
            DesenhaObjeto(fnd2);
            DesenhaObjeto(fala[3]);
            DesenhaObjeto(fala[4]);
            DesenhaObjeto(fala[5]);
        }

        if(t>15.0 && t<22.5)
        {
            t-=15.0;
           if(t<5.0){
                if(t<2.5)
                SetOpacidadeObjeto(fala[6],(t/2.5)*255);
                else
                SetOpacidadeObjeto(fala[7],((t-2.5)/2.5)*255);
           }else
           {
               SetOpacidadeObjeto(fala[6],((1-(t-5.0)/2.5))*255);
               SetOpacidadeObjeto(fala[7],((1-(t-5.0)/2.5))*255);
               SetOpacidadeObjeto(fnd3,((1-(t-5.0)/2.5))*255);
           }
            DesenhaObjeto(fnd3);
            DesenhaObjeto(fala[6]);
            DesenhaObjeto(fala[7]);
        }


        EncerraDesenho();
    }
}

/***************************
Funções de fases
***************************/

void Game_Over()
{
    int fundogameover;

    fundogameover=CriaObjeto("..//imagens//fundos//fundogame_over.jpg",0,255);

    while(JogoRodando()){

        evento=GetEvento();

        IniciaDesenho();

        DesenhaObjeto(fundogameover);

        EncerraDesenho();
    }
    FinalizaJogo();
}

void menuinicial()
{
    int fundo,btnjogar,btnsair;

    fundo=CriaObjeto("..//imagens//fundos//fundomenu.png",0,255);
    btnjogar=CriaObjeto("..//imagens//Botoes//btnJogar.png");
    MoveObjeto(btnjogar,136,252);
    btnsair=CriaObjeto("..//imagens//Botoes//btnSair.png");
    MoveObjeto(btnsair,171,203);

    while(GetEstadoJogo()==0){

        evento=GetEvento();

        IniciaDesenho();

        DesenhaObjeto(fundo);
        DesenhaObjeto(btnjogar);
        DesenhaObjeto(btnsair);

        if(evento.tipoEvento==EVENTO_MOUSE && evento.mouse.acao==MOUSE_PRESSIONADO)
        {
            if(evento.mouse.botao==MOUSE_ESQUERDO)
            {
                if(Mouse_sobre_obj(btnjogar))
                {
                    Animacao_inicial();
                    SetEstadoJogo(1);//Inicia primeira fase do jogo
                }
                if(Mouse_sobre_obj(btnsair))
                {
                    FinalizaJogo(); //Encerra execucao do jogo
                }
            }
        }
        EncerraDesenho();
    }


}

void fase1()
{

    int paralax,x,y;
    Carrega_cenario("..//databases//cenarios//cenario.txt",896,1600);
    fundo=CriaObjeto("..//imagens//fundos//map01.png",0);

    cx=416;
    cy=0;
    paralax=CriaObjeto("..//imagens//Others//space.jpg",0);
    MoveObjeto(fundo,-cx,-cy);
    GETXY_Posij(heroi.posi,heroi.posj,&x,&y);
    MoveAnimacao(heroi.id,x,y);
    while(GetEstadoJogo()==1 && JogoRodando())
    {
        evento = GetEvento();
        IniciaDesenho();
        DesenhaObjeto(paralax);
        DesenhaObjeto(fundo);
        ImprimeFPS();
        DesenhaAnimacao(heroi.id);
        movimentar_heroi();
        Is_Evento();
        Desenha_Imgs_Evs(imgs_ev);
        EncerraDesenho();
    }

}

void fase2()
{
    Carrega_inimigos("..//databases//personagens//inimigos_fase_fogo.txt");
    Carrega_cenario("..//databases//cenarios//cenario2.txt",1600,1280);
    fundobatalha=CriaObjeto("..//imagens//fundos//fundolava.png",0,255);
    fundo=CriaObjeto("..//imagens//fundos//map02.png",0);


    while(GetEstadoJogo()==2 && JogoRodando())
    {
        evento = GetEvento();
        IniciaDesenho();
        DesenhaObjeto(fundo);
        ImprimeFPS();
        DesenhaAnimacao(heroi.id);
        movimentar_heroi();
        Is_Evento();
        Desenha_Imgs_Evs(imgs_ev);
        EncerraDesenho();
    }
    provacoes--;

}

void fase3()
{

    Carrega_inimigos("..//databases//personagens//inimigos_fase_gelo.txt");
    Carrega_cenario("..//databases//cenarios//cenario3.txt",1920,1920);
    fundobatalha=CriaObjeto("..//imagens//fundos//fundogelo.png",0,255);
    fundo=CriaObjeto("..//imagens//fundos//map03.png",0);


    while(GetEstadoJogo()==3 && JogoRodando())
    {
        evento = GetEvento();
        IniciaDesenho();
        DesenhaObjeto(fundo);
        ImprimeFPS();
        DesenhaAnimacao(heroi.id);
        movimentar_heroi();
        Is_Evento();
        Desenha_Imgs_Evs(imgs_ev);
        EncerraDesenho();
    }
    provacoes--;
}

void fase4()
{
    Carrega_inimigos("..//databases//personagens//inimigos_fase_grama.txt");
    Carrega_cenario("..//databases//cenarios//cenario4.txt",1696,1632);
    fundobatalha=CriaObjeto("..//imagens//fundos//fundograma.png",0,255);
    fundo=CriaObjeto("..//imagens//fundos//map04.png",0);

    printf("linhas:%d colunas:%d ",linhas_cenario,colunas_cenario);
    while(GetEstadoJogo()==4 && JogoRodando())
    {
        evento = GetEvento();
        IniciaDesenho();
        DesenhaObjeto(fundo);
        ImprimeFPS();
        DesenhaAnimacao(heroi.id);
        movimentar_heroi();
        Is_Evento();
        Desenha_Imgs_Evs(imgs_ev);
        EncerraDesenho();
    }
    provacoes--;
}
void fasefinal()
{

    int paralax2,x,y;
    Carrega_cenario("..//databases//cenarios//cenario5.txt",896,1600);
    fundo=CriaObjeto("..//imagens//fundos//Map05.png",0);

    cx=416;
    cy=0;
    paralax2=CriaObjeto("..//imagens//Others//space.jpg",0);
    MoveObjeto(fundo,-cx,-cy);
    GETXY_Posij(heroi.posi,heroi.posj,&x,&y);
    MoveAnimacao(heroi.id,x,y);
    while(GetEstadoJogo()==5 && JogoRodando())
    {
        evento = GetEvento();
        IniciaDesenho();
        DesenhaObjeto(paralax2);
        DesenhaObjeto(fundo);
        ImprimeFPS();
        DesenhaAnimacao(heroi.id);
        movimentar_heroi();
        Is_Evento();
        Desenha_Imgs_Evs(imgs_ev);
        EncerraDesenho();
    }
}
void youwin()
{
    int fundowin;

    fundowin=CriaObjeto("..//imagens//fundos//youwin.jpg",0,255);

    while(JogoRodando()){

        evento=GetEvento();

        IniciaDesenho();

        DesenhaObjeto(fundowin);

        EncerraDesenho();
    }
    FinalizaJogo();
}
/***************************
Main
***************************/

int main( int argc, char* args[] )
{
    CriaJogo("Meu Jogo",0);
    meuTeclado = GetTeclado();

    Criar_Heroi(); //Cria e inicializa atributos do personagem principal

    SetEstadoJogo(0); //Define estado inicial do jogo
    fonte_himalaya=CriaFonteNormal("..//fontes//himalaya.ttf",35,BRANCO,0,PRETO,ESTILO_NORMAL);


    while(JogoRodando()){

        if(provacoes==0)
        {
            SetEstadoJogo(5);
        }
        evento = GetEvento();
            switch(GetEstadoJogo()) //Verifica qual fase do jogo deve ser executada
            {
            case 0 :
                {

                    menuinicial();
                    break;
                }
            case 1 :
                {
                    fase1();
                    break;
                }
            case 2 :
                {
                    fase2();
                    break;
                }
            case 3 :
                {
                    fase3();
                    break;
                }
            case 4 :
                {
                    fase4();
                    break;
                }
            case 5 :
                {
                    fasefinal();
                    break;
                }
            case 6 :
                {
                    youwin();
                    break;
                }
            }
    }
    FinalizaJogo();

    return 0;
}
