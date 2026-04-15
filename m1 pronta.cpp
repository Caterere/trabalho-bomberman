#include <iostream>
#include <locale.h>
#include <conio.h> // biblioteca para para ler uma tecla sem precisar apertar enter.
#include <ctime>  //  biblioteca para gerar números aleatórios
#include <windows.h>
using namespace std;


    void movejogador(int matrizprincipal[11][11], int tecla, int &px, int &py, bool &tembomba, int &bx, int &by){
        
        if (tecla==224){
            tecla=getch();
        }

            if (tecla == 75 || tecla == 'a' || tecla == 'A') { // Esquerda
                if (matrizprincipal[px][py-1] == 2 || matrizprincipal[px][py-1] == 5) { //Verifica se tem passagem
                    if (px == bx && py == by) { // Verifica se a posição do personagem é a mesma da bomba
                        matrizprincipal[px][py] = 5; 
                        } else {                    // Se for, bomba fica no espaço. Se não, espaço fica vazio
                        matrizprincipal[px][py] = 2;
                        } py--; // Move o jogador
                        matrizprincipal[px][py] = 3; // Desenha o jogador na nova posição
        
                        } else if (matrizprincipal[px][py-1] == 4) { // Colisão com inimigo
                        matrizprincipal[px][py] = 6; // Define morte do jogador
                        }
                            }
                else if (tecla == 77 || tecla=='d' || tecla == 'D'){      // Direita
                    if (matrizprincipal[px][py+1] == 2 || matrizprincipal[px][py+1] == 5) { 
                        if (px == bx && py == by) { 
                            matrizprincipal[px][py] = 5; 
                            } else {                    
                            matrizprincipal[px][py] = 2;
                            } py++; 
                            matrizprincipal[px][py] = 3; 
        
                    } else if (matrizprincipal[px][py+1] == 4) { 
                    matrizprincipal[px][py] = 6; 
                    }   
                        }
                    else if (tecla==72 || tecla=='w' || tecla == 'W'){     // Cima
                        if (matrizprincipal[px-1][py] == 2 || matrizprincipal[px-1][py] == 5) { 
                        if (px == bx && py == by) { 
                            matrizprincipal[px][py] = 5; 
                            } else {                    
                            matrizprincipal[px][py] = 2;
                            } px--; 
                            matrizprincipal[px][py] = 3; 
        
                    } else if (matrizprincipal[px-1][py] == 4) { 
                    matrizprincipal[px][py] = 6; 
                    }   
                        }
                      else if (tecla==80 || tecla=='s'|| tecla=='S'){     // Baixo
                            if (matrizprincipal[px+1][py] == 2 || matrizprincipal[px+1][py] == 5) { 
                        if (px == bx && py == by) { 
                            matrizprincipal[px][py] = 5; 
                            } else {                    
                            matrizprincipal[px][py] = 2;
                            } px++; 
                            matrizprincipal[px][py] = 3; 
        
                    } else if (matrizprincipal[px+1][py] == 4) { 
                    matrizprincipal[px][py] = 6; 
                    }   
                        }
        
                    else if (tecla == 'Z' || tecla == 'z'){
                            if (tembomba==false){   // Verifica se não tem outra bomba no mapa
                                bx=px;
                                by=py;
                                matrizprincipal[bx][by]=5; // Coloca bomba na posição
                                    tembomba=true;
                            }
                                }
    }
        

    void explodebomba(int matrizprincipal[11][11], bool &tembomba, int &contbomba, int &contexplosao, int &bx, int &by) {
    
    if (tembomba == true) { // contagem para explodir
        contbomba++;
        if (contbomba == 15) {
            contbomba = 0;
            contexplosao = 1; // Da passagem para explosão
            tembomba = false;
            
            matrizprincipal[bx][by] = 6; // Define o centro da explosão
            
            if (bx + 1 < 11) matrizprincipal[bx+1][by] = (matrizprincipal[bx+1][by] != 0) ? 6 : matrizprincipal[bx+1][by];
            if (bx - 1 >= 0) matrizprincipal[bx-1][by] = (matrizprincipal[bx-1][by] != 0) ? 6 : matrizprincipal[bx-1][by]; // Define as direções e garante que não saia da matriz
            if (by + 1 < 11) matrizprincipal[bx][by+1] = (matrizprincipal[bx][by+1] != 0) ? 6 : matrizprincipal[bx][by+1];
            if (by - 1 >= 0) matrizprincipal[bx][by-1] = (matrizprincipal[bx][by-1] != 0) ? 6 : matrizprincipal[bx][by-1];
        }
    }
    if (contexplosao > 0) {
        contexplosao++;
        if (contexplosao == 6) { // Fim do tempo de explosão
            contexplosao = 0;
            matrizprincipal[bx][by] = 2; // Limpa o centro
            
            if (bx + 1 < 11) matrizprincipal[bx+1][by] = (matrizprincipal[bx+1][by] == 6) ? 2 : matrizprincipal[bx+1][by];
            if (bx - 1 >= 0) matrizprincipal[bx-1][by] = (matrizprincipal[bx-1][by] == 6) ? 2 : matrizprincipal[bx-1][by]; // Limpa as 4 direções apenas se ainda forem explosão
            if (by + 1 < 11) matrizprincipal[bx][by+1] = (matrizprincipal[bx][by+1] == 6) ? 2 : matrizprincipal[bx][by+1];
            if (by - 1 >= 0) matrizprincipal[bx][by-1] = (matrizprincipal[bx][by-1] == 6) ? 2 : matrizprincipal[bx][by-1];
        }
    }
}
                    
    void checamorte (int matrizprincipal[11][11], int &px, int &py, bool &jogoativo, bool &jogadorvivo,
                 int &in1x, int &in1y, bool &in1vivo,
                 int &in2x, int &in2y, bool &in2vivo,
                 int &in3x, int &in3y, bool &in3vivo) {

    if (matrizprincipal[px][py] == 4 || matrizprincipal[px][py] == 6) { // Verifica se o jogador se chocar com alguma condição de morte
        matrizprincipal[px][py] = 6; // Define ícone de morte do jogador
        jogoativo = false;  // Finaliza o loop do jogo    
        jogadorvivo = false; // Define morte do jogador
    }
    
    if (in1vivo == true) {
        if (matrizprincipal[in1x][in1y] == 6) { // Checa morte do inimigo 1
            in1vivo = false;
        }
    }

    if (in2vivo == true) {
        if (matrizprincipal[in2x][in2y] == 6) { // Checa morte do inimigo 2
            in2vivo = false;
        }
    }

    if (in3vivo == true) {
        if (matrizprincipal[in3x][in3y] == 6) { // // Checa morte do inimigo 3
            in3vivo = false;
        }
    }

    if (in1vivo == false && in2vivo == false && in3vivo == false) { // Verifica condição de vitória
        jogoativo = false; 
    }
}

    void moveinimigo(int matrizprincipal[11][11], int &inx, int &iny, int &cont) {
    int direcao, passos;
    cont++;

    if (cont == 3) {
        direcao = rand() % 4; // Atribuimos valores para cada direção que serão definidos aleatoriamente 
        passos = rand() % 3 + 1; // Definimos quantos passos o inimigo dará e impede que não de nenhum passo
        cont = 0;

        for (int i = 0; i < passos; i++) { // Cima
            if (direcao == 0) {
                if (inx - 1 >= 0) { // Impede que não atravesse a matriz
                    if (matrizprincipal[inx - 1][iny] == 2 || matrizprincipal[inx - 1][iny] == 3) {
                        if (matrizprincipal[inx - 1][iny] == 3) { // verifica se é o jogador e define morte
                            matrizprincipal[inx - 1][iny] = 6;
                            break;
                        }
                        matrizprincipal[inx][iny] = 2;
                        inx--;
                        matrizprincipal[inx][iny] = 4;
                    }
                }
            }
            else if (direcao == 1) { // Baixo
                if (inx + 1 < 11) {
                    if (matrizprincipal[inx + 1][iny] == 2 || matrizprincipal[inx + 1][iny] == 3) {
                        if (matrizprincipal[inx + 1][iny] == 3) {
                            matrizprincipal[inx + 1][iny] = 6;
                            break;
                        }
                        matrizprincipal[inx][iny] = 2;
                        inx++;
                        matrizprincipal[inx][iny] = 4;
                    }
                }
            }
            else if (direcao == 2) { // Esquerda
                if (iny - 1 >= 0) {
                    if (matrizprincipal[inx][iny - 1] == 2 || matrizprincipal[inx][iny - 1] == 3) {
                        if (matrizprincipal[inx][iny - 1] == 3) {
                            matrizprincipal[inx][iny - 1] = 6;
                            break;
                        }
                        matrizprincipal[inx][iny] = 2;
                        iny--;
                        matrizprincipal[inx][iny] = 4;
                    }
                }
            }
            else if (direcao == 3) { // Direita
                if (iny + 1 < 11) {
                    if (matrizprincipal[inx][iny + 1] == 2 || matrizprincipal[inx][iny + 1] == 3) {
                        if (matrizprincipal[inx][iny + 1] == 3) {
                            matrizprincipal[inx][iny + 1] = 6;
                            break;
                        }
                        matrizprincipal[inx][iny] = 2;
                        iny++;
                        matrizprincipal[inx][iny] = 4;
                    }
                }
            }
        }
    }
}

    void gotoxy(int x, int y) {
    COORD coord; // Cria uma variável do tipo COORD (uma estrutura que guarda X e Y)
    
    coord.X = x; // Define a coluna horizontal onde o cursor vai ficar
    coord.Y = y; // Define a linha verticalonde o cursor vai ficar
    
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// ============================================================
//                  FUNÇÕES DO MENU
// ============================================================

void setColor(HANDLE hConsole, int cor) {
    SetConsoleTextAttribute(hConsole, cor);
}

void exibirBanner(HANDLE hConsole) {
    setColor(hConsole, 14); // amarelo
    cout << "  ██████╗  ██████╗ ███╗   ███╗██████╗ ███████╗██████╗ ███╗   ███╗ █████╗ ███╗  ██╗" << endl;
    cout << "  ██╔══██╗██╔═══██╗████╗ ████║██╔══██╗██╔════╝██╔══██╗████╗ ████║██╔══██╗████╗ ██║" << endl;
    cout << "  ██████╔╝██║   ██║██╔████╔██║██████╔╝█████╗  ██████╔╝██╔████╔██║███████║██╔██╗██║" << endl;
    cout << "  ██╔══██╗██║   ██║██║╚██╔╝██║██╔══██╗██╔══╝  ██╔══██╗██║╚██╔╝██║██╔══██║██║╚████║" << endl;
    cout << "  ██████╔╝╚██████╔╝██║ ╚═╝ ██║██████╔╝███████╗██║  ██║██║ ╚═╝ ██║██║  ██║██║ ╚███║" << endl;
    cout << "  ╚═════╝  ╚═════╝ ╚═╝     ╚═╝╚═════╝ ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚══╝" << endl;
    setColor(hConsole, 7);
}

void exibirMenuPrincipal(HANDLE hConsole) {
    system("cls");
    exibirBanner(hConsole);

    setColor(hConsole, 8); // cinza
    cout << "  ══════════════════════════════════════════════════════════════════════════════════" << endl;
    setColor(hConsole, 7);

    cout << endl;
    setColor(hConsole, 10); cout << "        [ 1 ]"; setColor(hConsole, 15); cout << "  JOGAR" << endl;
    cout << endl;
    setColor(hConsole, 11); cout << "        [ 2 ]"; setColor(hConsole, 15); cout << "  INSTRUCOES" << endl;
    cout << endl;
    setColor(hConsole, 13); cout << "        [ 3 ]"; setColor(hConsole, 15); cout << "  REGRAS" << endl;
    cout << endl;
    setColor(hConsole, 14); cout << "        [ 4 ]"; setColor(hConsole, 15); cout << "  SOBRE" << endl;
    cout << endl;
    setColor(hConsole, 12); cout << "        [ 5 ]"; setColor(hConsole, 15); cout << "  SAIR" << endl;
    cout << endl;

    setColor(hConsole, 8);
    cout << "  ══════════════════════════════════════════════════════════════════════════════════" << endl;
    setColor(hConsole, 7);

    cout << endl;
    setColor(hConsole, 7); cout << "  Escolha uma opcao: ";
}

void exibirInstrucoes(HANDLE hConsole) {
    system("cls");

    setColor(hConsole, 11);
    cout << endl;
    cout << "  ╔══════════════════════════════════════╗" << endl;
    cout << "  ║           INSTRUCOES                 ║" << endl;
    cout << "  ╚══════════════════════════════════════╝" << endl;
    cout << endl;

    setColor(hConsole, 15);
    cout << "  CONTROLES:" << endl;
    setColor(hConsole, 7);
    cout << "  ┌─────────────────────────────────────┐" << endl;
    cout << "  │  W / Seta Cima    →  Mover para cima │" << endl;
    cout << "  │  S / Seta Baixo   →  Mover para baixo│" << endl;
    cout << "  │  A / Seta Esq     →  Mover esquerda  │" << endl;
    cout << "  │  D / Seta Dir     →  Mover direita   │" << endl;
    cout << "  │  Z                →  Colocar bomba   │" << endl;
    cout << "  └─────────────────────────────────────┘" << endl;
    cout << endl;

    setColor(hConsole, 15);
    cout << "  LEGENDA DO MAPA:" << endl;
    setColor(hConsole, 7);
    cout << "  ┌─────────────────────────────────────┐" << endl;

    setColor(hConsole, 7);  cout << "  │  "; setColor(hConsole, 13); cout << "  "; setColor(hConsole, 7); cout << " (rosa)  →  Parede solida       │" << endl;
    setColor(hConsole, 7);  cout << "  │  "; setColor(hConsole, 9);  cout << "  "; setColor(hConsole, 7); cout << " (azul)  →  Parede fragil       │" << endl;
    setColor(hConsole, 7);  cout << "  │  "; setColor(hConsole, 10); cout << "@"; setColor(hConsole, 7); cout << "        →  Voce (jogador)        │" << endl;
    setColor(hConsole, 7);  cout << "  │  "; setColor(hConsole, 12); cout << "E"; setColor(hConsole, 7); cout << "        →  Inimigo              │" << endl;
    setColor(hConsole, 7);  cout << "  │  "; setColor(hConsole, 14); cout << "B"; setColor(hConsole, 7); cout << "        →  Bomba                │" << endl;
    setColor(hConsole, 7);  cout << "  │  "; setColor(hConsole, 4);  cout << "*"; setColor(hConsole, 7); cout << "        →  Explosao             │" << endl;

    cout << "  └─────────────────────────────────────┘" << endl;
    cout << endl;

    setColor(hConsole, 8);
    cout << "  Pressione qualquer tecla para voltar...";
    setColor(hConsole, 7);
    getch();
}

void exibirRegras(HANDLE hConsole) {
    system("cls");

    setColor(hConsole, 13);
    cout << endl;
    cout << "  ╔══════════════════════════════════════╗" << endl;
    cout << "  ║              REGRAS                  ║" << endl;
    cout << "  ╚══════════════════════════════════════╝" << endl;
    cout << endl;

    setColor(hConsole, 15);
    cout << "  OBJETIVO:" << endl;
    setColor(hConsole, 7);
    cout << "  → Elimine todos os 3 inimigos para vencer!" << endl;
    cout << endl;

    setColor(hConsole, 15);
    cout << "  COMO JOGAR:" << endl;
    setColor(hConsole, 7);
    cout << "  1. Use WASD ou as setas para se mover pelo mapa." << endl;
    cout << "  2. Pressione Z para colocar uma bomba no chao." << endl;
    cout << "  3. A bomba explode apos alguns instantes." << endl;
    cout << "  4. A explosao alcanca as 4 direcoes (cruz)." << endl;
    cout << "  5. Paredes frageis (azuis) sao destruidas pela explosao." << endl;
    cout << "  6. Paredes solidas (rosas) sao indestrutíveis." << endl;
    cout << endl;

    setColor(hConsole, 15);
    cout << "  CUIDADO:" << endl;
    setColor(hConsole, 7);
    cout << "  ! Nao fique perto da sua propria bomba!" << endl;
    cout << "  ! Se um inimigo te tocar, voce perde." << endl;
    cout << "  ! Se a explosao te atingir, voce perde." << endl;
    cout << "  ! Voce so pode ter uma bomba ativa por vez." << endl;
    cout << endl;

    setColor(hConsole, 15);
    cout << "  CONDICOES DE VITORIA / DERROTA:" << endl;
    setColor(hConsole, 7);
    cout << "  ✓ VITORIA  → Todos os inimigos eliminados." << endl;
    cout << "  ✗ DERROTA  → Jogador morto por explosao ou inimigo." << endl;
    cout << endl;

    setColor(hConsole, 8);
    cout << "  Pressione qualquer tecla para voltar...";
    setColor(hConsole, 7);
    getch();
}

void exibirSobre(HANDLE hConsole) {
    system("cls");

    setColor(hConsole, 14);
    cout << endl;
    cout << "  ╔══════════════════════════════════════╗" << endl;
    cout << "  ║               SOBRE                  ║" << endl;
    cout << "  ╚══════════════════════════════════════╝" << endl;
    cout << endl;

    setColor(hConsole, 7);
    cout << "  Bomberman - Versao Console" << endl;
    cout << endl;
    cout << "  Este jogo e uma versao simplificada do classico" << endl;
    cout << "  Bomberman, desenvolvido em C++ para o console." << endl;
    cout << "  → Código feito por Kamilli Correia, Melissa Melendez Lopes, Luiz Eduardo Oro" << endl;
    cout << endl;

    setColor(hConsole, 15);
    cout << "  TECNOLOGIAS UTILIZADAS:" << endl;
    setColor(hConsole, 7);
    cout << "  → Linguagem: C++" << endl;
    cout << "  → Bibliotecas: iostream, conio.h, windows.h, ctime" << endl;
    cout << "  → Renderizacao via matriz 11x11" << endl;
    cout << "  → Cores via SetConsoleTextAttribute (WinAPI)" << endl;
    cout << endl;

    setColor(hConsole, 15);
    cout << "  MECANICAS IMPLEMENTADAS:" << endl;
    setColor(hConsole, 7);
    cout << "  → Movimento do jogador (WASD + setas)" << endl;
    cout << "  → 3 inimigos com movimento aleatorio" << endl;
    cout << "  → Sistema de bomba com temporizador" << endl;
    cout << "  → Geracao aleatoria de paredes frageis" << endl;
    cout << "  → Deteccao de colisao e morte" << endl;
    cout << endl;

    setColor(hConsole, 8);
    cout << "  Pressione qualquer tecla para voltar...";
    setColor(hConsole, 7);
    getch();
}

// ============================================================


int main()
{
    system("chcp 65001 > nul");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    int opcao;
    bool sairDoMenu = false; // Controle para saber quando parar o loop

    while (sairDoMenu == false) {
        exibirMenuPrincipal(hConsole);

        do {
            cin >> opcao;
        } while (opcao < 1 || opcao > 5);

        switch (opcao) {
            case 1: 
                sairDoMenu = true; // Sai do while e começa o jogo
                break;

            case 2: 
                exibirInstrucoes(hConsole); 
                break;

            case 3: 
                exibirRegras(hConsole); 
                break;

            case 4: 
                exibirSobre(hConsole); 
                break;

            case 5: 
                return 0; // Fecha o programa
        }
    }

    // --- A PARTIR DAQUI COMEÇA O JOGO ---

   int matrizprincipal[11][11] =    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 0 parede sólida
                                     {0, 3, 2, 2, 2, 2, 2, 2, 2, 2, 0}, // 2 livre
                                     {0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0}, // 3 jogador
                                     {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0}, // 4 inimigo
                                     {0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0}, // 5 bomba
                                     {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0}, // 6 explosão/morte
                                     {0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0}, // 1 parede frágil
                                     {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0},
                                     {0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0},
                                     {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0},
                                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
                                     
        srand(time(0));
        for (int i = 1; i < 10; i++) { // gerador de parede frágil
        for (int j = 1; j < 10; j++) {
             if (matrizprincipal[i][j] == 2 
                        && !(i <= 2 && j <= 2)        
                        && !(i <= 2 && j >= 8)   // verifica se não é o canto de saída dos inimigos ou do jogador     
                        && !(i >= 8 && j <= 2)        
                        && !(i >= 8 && j >= 8)) {    
             if (rand() % 3 == 0) {             // 1 chance em 3 de virar parede frágil
                matrizprincipal[i][j] = 1;
            }
        }
    }
}
        bool jogadorvivo = true;
        bool jogoativo = true; 
        bool in1vivo = true;
        bool in2vivo = true;
        bool in3vivo = true;

        int px = 1, py = 1;     // posição personagem
        
        int bx, by;     // posição bomba
        
        bool tembomba = false;   //bomba
        int contbomba = 0;
        int contexplosao = 0;

        int in1x=9, in1y=9;     // inimigo1
        matrizprincipal[9][9]=4;
        int cont1=0;

        int in2x=9, in2y=1;     // inimigo2
        matrizprincipal[9][1]=4;
        int cont2=0;

        int in3x=1, in3y=9;     // inimigo3
        matrizprincipal[1][9]=4;
        int cont3=0;


        CONSOLE_CURSOR_INFO cursorInfo;
        cursorInfo.dwSize = 1;
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);

        system("cls");  // limpa uma vez só antes do jogo começar

    while (jogoativo) {
        gotoxy(0, 0); // Limpa a tela para atualizar a posição
    for (int x = 0; x < 11; x++) {
        for (int y = 0; y < 11;  y++) {

           

            switch (matrizprincipal[x][y]) {
                case 0:
                SetConsoleTextAttribute(hConsole, 13*16); 
                cout << "  ";                           // parede
                SetConsoleTextAttribute(hConsole, 7); 
                break;
                case 1:
                SetConsoleTextAttribute(hConsole, 9*16); 
                cout << "  ";                           // parede fragil
                SetConsoleTextAttribute(hConsole, 7); 
                break;
                case 2:
                SetConsoleTextAttribute(hConsole, 0*16); 
                cout << "  ";                           // espaço
                SetConsoleTextAttribute(hConsole, 7);
                break;
                case 3: 
                SetConsoleTextAttribute(hConsole, 10);  
                cout << "@ ";                           // jogador
                SetConsoleTextAttribute(hConsole, 7);  
                break;
                case 4: 
                SetConsoleTextAttribute(hConsole, 12);  
                cout << "E ";                           // inimigo
                SetConsoleTextAttribute(hConsole, 7);
                break;
                case 5:
                SetConsoleTextAttribute(hConsole, 14);
                cout << "B ";                           // bomba 
                SetConsoleTextAttribute(hConsole, 7);
                break;
                case 6:
                SetConsoleTextAttribute(hConsole, 4);
                cout << "* ";                           // explosao
                SetConsoleTextAttribute(hConsole, 7);
                break;
                }

            }

            cout << endl;
        }

        if (kbhit()){
        int tecla = getch(); 
        movejogador(matrizprincipal, tecla, px, py, tembomba, bx, by);
        }
        if (in1vivo == true){
        moveinimigo(matrizprincipal, in1x, in1y, cont1);
        } 
        if (in2vivo == true){
        moveinimigo(matrizprincipal, in2x, in2y, cont2);
        }
        if ( in3vivo == true){  
        moveinimigo(matrizprincipal, in3x, in3y, cont3);
        }
        explodebomba(matrizprincipal, tembomba, contbomba, contexplosao, bx, by);
        checamorte(matrizprincipal, px, py, jogoativo, jogadorvivo,
           in1x, in1y, in1vivo,
           in2x, in2y, in2vivo,
           in3x, in3y, in3vivo);
           Sleep(300);
        }
        
        if (jogadorvivo == true) {
        cout << "=========== VITORIA! ===========" << endl;
        } else {
        cout << "========== GAME OVER ===========" << endl;
        }
    

    return 0;
}