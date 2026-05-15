#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <conio.h>
#include <ctime>
#include <windows.h>
using namespace std;

// ============================================================
//  STRUCTS
// ============================================================
struct Item     { int x, y, tipo; bool ativo; };
struct Bomba    { int x, y, cont, contExp; bool ativa, relogio; };
struct Inimigo  { int x, y, cont, tipo, vida; bool vivo; };
// tipo: 0=normal  1=rapido  2=boss
// vida: boss precisa de 3 hits

struct Jogador {
    string nome;
    int x, y, vidas, raio, maxBombas, pontuacao, movimentos, bombasUsadas, inimigosAbatidos, caixasDestruidas;
    bool vivo, sobreviveBomba, passaBlocos;
};

struct RankEntry { string nome, data; int pontuacao, movimentos, bombasUsadas; };

// ============================================================
//  TEMPLATES
// ============================================================
template<typename T>
void bubbleSortRank(vector<T>& v) {
    for (int i = 0; i < (int)v.size()-1; i++)
        for (int j = 0; j < (int)v.size()-i-1; j++)
            if (v[j].pontuacao < v[j+1].pontuacao) swap(v[j], v[j+1]);
}

template<typename T>
void exibirLista(const vector<T>& v, const string& titulo) {
    cout << "  " << titulo << ":\n";
    for (int i = 0; i < (int)v.size(); i++) cout << "  " << i+1 << ". " << v[i] << "\n";
}

// ============================================================
//  GLOBALS
// ============================================================
HANDLE hCon;

// ============================================================
//  UTILITARIOS
// ============================================================
void cor(int c)      { SetConsoleTextAttribute(hCon, c); }
void corFundo(int c) { SetConsoleTextAttribute(hCon, c*16); }
void gotoxy(int x, int y) { COORD c={(short)x,(short)y}; SetConsoleCursorPosition(hCon, c); }

string dataHoje() {
    time_t t = time(0); struct tm* n = localtime(&t);
    char b[20]; sprintf(b,"%02d/%02d/%04d",n->tm_mday,n->tm_mon+1,n->tm_year+1900);
    return b;
}

// ============================================================
//  ARQUIVO
// ============================================================
void salvarRank(const RankEntry& e) {
    ofstream f("rank.txt", ios::app);
    if (f) f << e.nome<<";"<<e.pontuacao<<";"<<e.movimentos<<";"<<e.bombasUsadas<<";"<<e.data<<"\n";
}

vector<RankEntry> carregarRank() {
    vector<RankEntry> r; ifstream f("rank.txt"); string linha;
    while (getline(f, linha)) {
        if (linha.empty()) continue;
        RankEntry e; stringstream ss(linha); string t;
        getline(ss,e.nome,';'); getline(ss,t,';'); e.pontuacao=atoi(t.c_str());
        getline(ss,t,';'); e.movimentos=atoi(t.c_str());
        getline(ss,t,';'); e.bombasUsadas=atoi(t.c_str());
        getline(ss,e.data,';'); r.push_back(e);
    }
    return r;
}

void salvarJogo(const Jogador& j, int fase, int dif) {
    ofstream f("save.txt");
    if (!f) return;
    f<<j.nome<<"\n"<<j.vidas<<"\n"<<j.pontuacao<<"\n"<<j.movimentos<<"\n"
     <<j.bombasUsadas<<"\n"<<j.inimigosAbatidos<<"\n"<<j.caixasDestruidas<<"\n"
     <<j.raio<<"\n"<<j.maxBombas<<"\n"<<j.sobreviveBomba<<"\n"<<j.passaBlocos<<"\n"
     <<fase<<"\n"<<dif<<"\n";
    gotoxy(0,13); cor(10); cout<<"  [SALVO] Pressione qualquer tecla..."; cor(7); getch();
}

bool carregarJogo(Jogador& j, int& fase, int& dif) {
    ifstream f("save.txt"); if (!f) return false;
    f>>j.nome>>j.vidas>>j.pontuacao>>j.movimentos>>j.bombasUsadas
     >>j.inimigosAbatidos>>j.caixasDestruidas>>j.raio>>j.maxBombas
     >>j.sobreviveBomba>>j.passaBlocos>>fase>>dif;
    return true;
}

// ============================================================
//  PONTUACAO (recursiva: bonus acumula por fase)
// ============================================================
int bonusFase(int fase) { return fase <= 1 ? 100 : 100 + bonusFase(fase-1); }

int calcPontos(const Jogador& j, int fase, int tempo) {
    int p = j.inimigosAbatidos*200 + j.caixasDestruidas*50
          + tempo*5 + bonusFase(fase)
          - j.bombasUsadas*10 - j.movimentos*2;
    return max(0, p);
}

// ============================================================
//  MAPA
// ============================================================
void flood(int mp[11][11], bool vis[11][11], int x, int y) {
    if (x<0||x>=11||y<0||y>=11||vis[x][y]||mp[x][y]==0||mp[x][y]==1) return;
    vis[x][y]=true;
    flood(mp,vis,x+1,y); flood(mp,vis,x-1,y);
    flood(mp,vis,x,y+1); flood(mp,vis,x,y-1);
}

void gerarMapa(int mp[11][11], int fase) {
    int base[11][11]={
        {0,0,0,0,0,0,0,0,0,0,0},{0,2,2,2,2,2,2,2,2,2,0},{0,2,0,2,0,2,0,2,0,2,0},
        {0,2,2,2,2,2,2,2,2,2,0},{0,2,0,2,0,2,0,2,0,2,0},{0,2,2,2,2,2,2,2,2,2,0},
        {0,2,0,2,0,2,0,2,0,2,0},{0,2,2,2,2,2,2,2,2,2,0},{0,2,0,2,0,2,0,2,0,2,0},
        {0,2,2,2,2,2,2,2,2,2,0},{0,0,0,0,0,0,0,0,0,0,0}
    };
    for(int i=0;i<11;i++) for(int j=0;j<11;j++) mp[i][j]=base[i][j];
    int chance = 3+fase;
    for(int i=1;i<10;i++) for(int j=1;j<10;j++)
        if(mp[i][j]==2 && !(i<=2&&j<=2) && !(i<=2&&j>=8) && !(i>=8&&j<=2) && !(i>=8&&j>=8))
            if(rand()%chance==0) mp[i][j]=1;
}

// ============================================================
//  ITENS  (sobrecarga)
// ============================================================
void colocarItem(vector<Item>& its, int x, int y, int tipo) { its.push_back({x,y,tipo,true}); }
void colocarItem(vector<Item>& its, int x, int y)           { colocarItem(its,x,y,rand()%6+1); }

void aplicarItem(Jogador& j, int tipo) {
    const char* msg[]={"","Raio +1!","Bomba extra!","Vida extra!","Bomba Relogio!","Sobrevive bomba!","Passa blocos!"};
    switch(tipo){case 1:j.raio++;break;case 2:j.maxBombas++;break;case 3:j.vidas++;break;
                 case 5:j.sobreviveBomba=true;break;case 6:j.passaBlocos=true;break;}
    gotoxy(0,13); cor(11); cout<<"  [ITEM] "<<msg[tipo]<<"          "; cor(7);
}

void verificarItens(Jogador& j, vector<Item>& its, int mp[11][11]) {
    for(auto& it:its) if(it.ativo&&it.x==j.x&&it.y==j.y){it.ativo=false;mp[it.x][it.y]=2;aplicarItem(j,it.tipo);}
}

// ============================================================
//  EXPLOSAO (recursiva)
// ============================================================
void propagar(int mp[11][11], vector<Item>& its, int& caixas,
              int x, int y, int dx, int dy, int passos) {
    if(passos==0) return;
    int nx=x+dx, ny=y+dy;
    if(nx<0||nx>=11||ny<0||ny>=11||mp[nx][ny]==0) return;
    if(mp[nx][ny]==1){mp[nx][ny]=6;caixas++;if(rand()%3==0)colocarItem(its,nx,ny);return;}
    mp[nx][ny]=6;
    propagar(mp,its,caixas,nx,ny,dx,dy,passos-1);
}

void explodir(int mp[11][11], Bomba& b, Jogador& j, vector<Item>& its) {
    mp[b.x][b.y]=6;
    propagar(mp,its,j.caixasDestruidas,b.x,b.y,-1,0,j.raio);
    propagar(mp,its,j.caixasDestruidas,b.x,b.y, 1,0,j.raio);
    propagar(mp,its,j.caixasDestruidas,b.x,b.y,0,-1,j.raio);
    propagar(mp,its,j.caixasDestruidas,b.x,b.y,0, 1,j.raio);
}

void gerenciarBombas(int mp[11][11], vector<Bomba>& bs, Jogador& j, vector<Item>& its) {
    for(auto& b:bs){
        if(!b.ativa) continue;
        if(++b.cont==(b.relogio?7:15)){b.cont=0;b.contExp=1;b.ativa=false;explodir(mp,b,j,its);}
    }
    for(auto& b:bs){
        if(b.contExp<=0) continue;
        if(++b.contExp==6){
            b.contExp=0;
            for(int i=0;i<11;i++) for(int jj=0;jj<11;jj++)
                if(mp[i][jj]==6){
                    bool ti=false; for(auto& it:its)if(it.ativo&&it.x==i&&it.y==jj){ti=true;break;}
                    mp[i][jj]=ti?7:2;
                }
        }
    }
    bs.erase(remove_if(bs.begin(),bs.end(),[](const Bomba& b){return !b.ativa&&b.contExp==0;}),bs.end());
}

// ============================================================
//  JOGADOR
// ============================================================
void moverJogador(int mp[11][11], int tecla, Jogador& j, vector<Bomba>& bs, bool temRelogio=false) {
    auto temBmb=[&](int x,int y){for(auto& b:bs)if(b.ativa&&b.x==x&&b.y==y)return true;return false;};
    auto deixar=[&](int x,int y){mp[x][y]=temBmb(x,y)?5:2;};
    auto podeIr=[&](int nx,int ny){
        if(mp[nx][ny]==2||mp[nx][ny]==7||mp[nx][ny]==8)return true;
        if(j.passaBlocos&&mp[nx][ny]==1)return true;
        return temBmb(nx,ny);
    };

    if(tecla==224)tecla=getch();
    int nx=j.x,ny=j.y;
    if     (tecla==75||tecla=='a'||tecla=='A')ny--;
    else if(tecla==77||tecla=='d'||tecla=='D')ny++;
    else if(tecla==72||tecla=='w'||tecla=='W')nx--;
    else if(tecla==80||tecla=='s'||tecla=='S')nx++;
    else if(tecla=='z'||tecla=='Z'){
        int atv=0;for(auto& b:bs)if(b.ativa)atv++;
        if(atv<j.maxBombas){bs.push_back({j.x,j.y,0,0,true,temRelogio});mp[j.x][j.y]=5;j.bombasUsadas++;}
        return;
    }else return;

    if(nx==j.x&&ny==j.y)return;
    if(podeIr(nx,ny)){deixar(j.x,j.y);j.x=nx;j.y=ny;mp[j.x][j.y]=3;j.movimentos++;}
    else if(mp[nx][ny]==4)mp[j.x][j.y]=6;
}

// ============================================================
//  INIMIGOS
// ============================================================
void moverInimigo(int mp[11][11], Inimigo& in, Jogador& j, int dif) {
    int lim=(in.tipo==2)?2:3;
    if(++in.cont<lim)return;
    in.cont=0;

    int dx=0,dy=0;
    int chance=(in.tipo==2)?90:(dif==2?75:dif==1?50:0);
    bool perseguir=chance>0&&(rand()%100)<chance;

    if(perseguir){
        int dfx=j.x-in.x,dfy=j.y-in.y;
        if(abs(dfx)>=abs(dfy))dx=(dfx>0)?1:-1;else dy=(dfy>0)?1:-1;
    }else{
        int dirs[4][2]={{-1,0},{1,0},{0,-1},{0,1}};
        int d=rand()%4;dx=dirs[d][0];dy=dirs[d][1];
    }

    int nx=in.x+dx,ny=in.y+dy;
    if(nx<0||nx>=11||ny<0||ny>=11)return;
    if(mp[nx][ny]==2||mp[nx][ny]==7){mp[in.x][in.y]=2;in.x=nx;in.y=ny;mp[in.x][in.y]=4;}
    else if(mp[nx][ny]==3)mp[nx][ny]=6;
}

// ============================================================
//  MORTE
// ============================================================
void checaMorte(int mp[11][11], Jogador& j, bool& ativo, vector<Inimigo>& ins, bool& faseOk) {
    if(mp[j.x][j.y]==6){
        if(j.sobreviveBomba){j.sobreviveBomba=false;mp[j.x][j.y]=3;}
        else{j.vidas--;j.vivo=false;if(j.vidas<=0)ativo=false;}
    }
    for(auto& in:ins){
        if(!in.vivo)continue;
        if(mp[in.x][in.y]==6){
            if(in.tipo==2){in.vida--;if(in.vida>0){mp[in.x][in.y]=4;continue;}}
            in.vivo=false;j.inimigosAbatidos++;j.pontuacao+=200;
        }
    }
    bool todos=true;for(auto& in:ins)if(in.vivo){todos=false;break;}
    if(todos)faseOk=true;
}

// ============================================================
//  RENDERIZACAO
// ============================================================
void renderInimigo(const Inimigo& in) {
    switch(in.tipo){
        case 0: cor(12); cout<<"E "; break;  // vermelho: normal
        case 1: cor(14); cout<<"F "; break;  // amarelo: rapido
        case 2:
            if(in.vida==3){cor(13);cout<<"B3";}
            else if(in.vida==2){cor(4);cout<<"B2";}
            else{cor(12);cout<<"B1";}         // magenta/vermelho: boss
            break;
    }
}

void renderMapa(int mp[11][11], const vector<Inimigo>& ins) {
    gotoxy(0,0);
    for(int x=0;x<11;x++){
        cout<<"  ";
        for(int y=0;y<11;y++){
            bool ok=false;
            if(mp[x][y]==4){
                for(const auto& in:ins)if(in.vivo&&in.x==x&&in.y==y){renderInimigo(in);ok=true;break;}
                if(!ok){cor(12);cout<<"E ";}
                ok=true;
            }
            if(!ok) switch(mp[x][y]){
                case 0: corFundo(5); cout<<"  "; break;
                case 1: corFundo(9); cout<<"  "; break;
                case 2: corFundo(0); cout<<"  "; break;
                case 3: cor(10);     cout<<"@ "; break;
                case 5: cor(14);     cout<<"B "; break;
                case 6: cor(4);      cout<<"* "; break;
                case 7: cor(11);     cout<<"? "; break;
                case 8: cor(13);     cout<<"P "; break;
            }
            cor(7);
        }
        cout<<"\n";
    }
}

void renderHUD(const Jogador& j, int fase, int tempo, int dif, const vector<Inimigo>& ins) {
    auto lin=[&](int y,int lc,const char* lb,int vc,int val){
        gotoxy(26,y);cor(lc);cout<<lb;cor(vc);cout<<val<<"   ";
    };
    gotoxy(26,0);  cor(14); cout<<" BOMBERMAN v2  "; cor(7);
    gotoxy(26,1);  cor(8);  cout<<" ──────────────────────";
    lin(2, 14," Fase:    ",15,fase);
    gotoxy(26,3);  cor(11); cout<<" Tempo:   ";(tempo>15?cor(10):cor(12));cout<<tempo<<"s   ";
    lin(4, 13," Vidas:   ",15,j.vidas);
    lin(5, 14," Pontos:  ",15,j.pontuacao);
    lin(6, 11," Movs:    ",8, j.movimentos);
    lin(7, 12," Bombas:  ",8, j.bombasUsadas);
    lin(8, 10," Abatidos:",15,j.inimigosAbatidos);
    lin(9,  9," Caixas:  ",8, j.caixasDestruidas);
    gotoxy(26,10); cor(8); cout<<" ──────────────────────";
    gotoxy(26,11); cor(15);cout<<" Dif: ";
    switch(dif){case 0:cor(10);cout<<"FACIL   ";break;case 1:cor(14);cout<<"MEDIO   ";break;case 2:cor(12);cout<<"DIFICIL ";}
    lin(12,7," Raio:    ",15,j.raio);
    lin(13,7," MaxBmbs: ",15,j.maxBombas);
    gotoxy(26,14); cor(j.sobreviveBomba?10:8); cout<<" [SobrBmb]  ";
    gotoxy(26,15); cor(j.passaBlocos?10:8);    cout<<" [PassBlc]  ";
    gotoxy(26,16); cor(8); cout<<" ──────────────────────";
    int vivos=0;for(auto& in:ins)if(in.vivo)vivos++;
    gotoxy(26,17); cor(12);cout<<" Inimigos: ";cor(15);cout<<vivos<<"   ";
    gotoxy(26,18); cor(8); cout<<" ──────────────────────";
    gotoxy(26,19); cor(8); cout<<" [WASD/Setas] Mover    ";
    gotoxy(26,20); cor(8); cout<<" [Z] Bomba  [0] Salvar ";
    gotoxy(26,21); cor(8); cout<<" [ESC] Voltar ao menu  ";
    gotoxy(26,22); cor(8); cout<<" ──────────────────────";
    gotoxy(26,23); cor(12);cout<<" E";cor(8);cout<<"=Normal ";cor(14);cout<<"F";cor(8);cout<<"=Rapido ";
    gotoxy(26,24); cor(13);cout<<" B3";cor(8);cout<<"=Boss  ";cor(11);cout<<"?";cor(8);cout<<"=Item  ";cor(13);cout<<"P";cor(8);cout<<"=Portal";
    cor(7);
}

// ============================================================
//  MENU
// ============================================================
void banner(){
    cor(14);
    cout<<"  ██████╗  ██████╗ ███╗   ███╗██████╗ ███████╗██████╗ ███╗   ███╗\n";
    cout<<"  ██╔══██╗██╔═══██╗████╗ ████║██╔══██╗██╔════╝██╔══██╗████╗ ████║\n";
    cout<<"  ██████╔╝██║   ██║██╔████╔██║██████╔╝█████╗  ██████╔╝██╔████╔██║\n";
    cout<<"  ██╔══██╗██║   ██║██║╚██╔╝██║██╔══██╗██╔══╝  ██╔══██╗██║╚██╔╝██║\n";
    cout<<"  ██████╔╝╚██████╔╝██║ ╚═╝ ██║██████╔╝███████╗██║  ██║██║ ╚═╝ ██║\n";
    cout<<"  ╚═════╝  ╚═════╝ ╚═╝     ╚═╝╚═════╝ ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝\n";
    cor(7);
}

void menuPrincipal(){
    system("cls");banner();
    cor(8);cout<<"  ══════════════════════════════════════════════════════\n";cor(7);
    const char* labels[]={"JOGAR","CONTINUAR JOGO SALVO","INSTRUCOES","REGRAS",
                          "DIFICULDADES","PONTUACAO","RANK","SOBRE","SAIR"};
    int cores[]={10,11,13,14,9,11,13,14,12};
    for(int i=0;i<9;i++){cout<<"\n  ";cor(cores[i]);cout<<"[ "<<i+1<<" ]";cor(15);cout<<"  "<<labels[i]<<"\n";}
    cor(8);cout<<"\n  ══════════════════════════════════════════════════════\n";cor(7);
    cout<<"\n  Escolha: ";
}

void telaInstrucoes(){
    system("cls");cor(11);
    cout<<"\n  ╔══════════════════════════════════════╗\n  ║           INSTRUCOES                 ║\n  ╚══════════════════════════════════════╝\n\n";
    cor(7);
    cout<<"  WASD/setas → Mover   Z → Bomba   0 → Salvar   ESC → Menu\n\n";
    cor(15);cout<<"  SPRITES:\n";cor(7);
    cor(10);cout<<"  @ ";cor(7);cout<<"Jogador        ";
    cor(12);cout<<"E ";cor(7);cout<<"Inimigo normal\n";
    cor(14);cout<<"  F ";cor(7);cout<<"Inimigo rapido ";
    cor(13);cout<<"B3/B2/B1 ";cor(7);cout<<"Boss (3 vidas)\n";
    cor(14);cout<<"  B ";cor(7);cout<<"Bomba          ";
    cor(4); cout<<"* ";cor(7);cout<<"Explosao\n";
    cor(11);cout<<"  ? ";cor(7);cout<<"Item especial  ";
    cor(13);cout<<"P ";cor(7);cout<<"Portal prox. fase\n\n";
    cor(8);cout<<"  Pressione qualquer tecla...";cor(7);getch();
}

void telaRegras(){
    system("cls");cor(13);
    cout<<"\n  ╔══════════════════════════════════════╗\n  ║              REGRAS                  ║\n  ╚══════════════════════════════════════╝\n\n";
    cor(7);
    cout<<"  Elimine todos os inimigos e entre no portal (P)\n";
    cout<<"  Boss (B3) precisa de 3 explosoes para morrer\n";
    cout<<"  Caixas azuis destroidas revelam itens (?)\n";
    cout<<"  Contato com inimigo ou explosao = perde vida\n\n";
    cor(8);cout<<"  Pressione qualquer tecla...";cor(7);getch();
}

void telaDificuldades(){
    system("cls");cor(9);
    cout<<"\n  ╔══════════════════════════════════════╗\n  ║           DIFICULDADES               ║\n  ╚══════════════════════════════════════╝\n\n";
    cor(10);cout<<"  FACIL   ";cor(7);cout<<"3 inimigos (E), aleatorio, 90s\n";
    cor(14);cout<<"  MEDIO   ";cor(7);cout<<"5 inimigos (E+F), 50% perseguicao, 75s\n";
    cor(12);cout<<"  DIFICIL ";cor(7);cout<<"7 inimigos, 75% perseguicao, 60s\n";
    cor(13);cout<<"  BOSS    ";cor(7);cout<<"Fase 3: Boss com 3 vidas e 90% perseguicao\n\n";
    cor(8);cout<<"  Pressione qualquer tecla...";cor(7);getch();
}

void telaPontuacao(){
    system("cls");cor(11);
    cout<<"\n  ╔══════════════════════════════════════╗\n  ║         SISTEMA DE PONTUACAO         ║\n  ╚══════════════════════════════════════╝\n\n";
    cor(7);
    cout<<"  Inimigo abatido  +200   Caixa destruida   +50\n";
    cout<<"  Tempo restante    +5/s  Bonus fase  +100 x fase\n";
    cout<<"  Bomba usada       -10   Movimento          -2\n\n";
    cor(15);cout<<"  ITENS:\n";cor(7);
    cout<<"  1-Raio+1  2-Bomba+1  3-Vida  4-BombaRelogio\n";
    cout<<"  5-SobreviveBomba  6-PassaBlocos\n\n";
    cor(8);cout<<"  Pressione qualquer tecla...";cor(7);getch();
}

void telaRank(){
    system("cls");cor(13);
    cout<<"\n  ╔══════════════════════════════════════════════════════╗\n";
    cout<<"  ║                RANK DE JOGADORES                    ║\n";
    cout<<"  ╚══════════════════════════════════════════════════════╝\n\n";
    auto rank=carregarRank();bubbleSortRank(rank);
    if(rank.empty()){cor(8);cout<<"  Sem partidas registradas.\n";}
    else{
        cor(8);printf("  %-3s %-14s %7s %6s %7s %10s\n","#","NOME","PONTOS","MOVS","BOMBAS","DATA");
        cout<<"  ──────────────────────────────────────────────────────\n";
        for(int i=0;i<min((int)rank.size(),10);i++){
            if(i==0)cor(14);else if(i==1)cor(7);else if(i==2)cor(6);else cor(8);
            printf("  %-3d %-14s %7d %6d %7d %10s\n",
                i+1,rank[i].nome.c_str(),rank[i].pontuacao,rank[i].movimentos,rank[i].bombasUsadas,rank[i].data.c_str());
        }
    }
    cor(7);cout<<"\n";cor(8);cout<<"  Pressione qualquer tecla...";cor(7);getch();
}

void telaSobre(){
    system("cls");cor(14);
    cout<<"\n  ╔══════════════════════════════════════╗\n  ║               SOBRE                  ║\n  ╚══════════════════════════════════════╝\n\n";
    cor(7);cout<<"  Bomberman Console C++ - Trabalho M2\n  UNIVALI - Algoritmos e Programacao II\n\n";
    cor(10);cout<<"  Kamilli Correia | Melissa Melendez | Luiz Eduardo Oro\n\n";
    cor(15);cout<<"  Tecnicas: ";cor(7);
    cout<<"Struct, Template, Sobrecarga,\n  Parametro Default, Recursividade, Arquivo\n\n";
    cor(8);cout<<"  Pressione qualquer tecla...";cor(7);getch();
}

int selecionarDif(){
    system("cls");cor(11);cout<<"\n  Dificuldade:\n\n";
    cor(10);cout<<"  [1] FACIL\n";cor(14);cout<<"  [2] MEDIO\n";cor(12);cout<<"  [3] DIFICIL\n\n";
    cor(7);cout<<"  Escolha: ";int op;do{cin>>op;}while(op<1||op>3);return op-1;
}

// ============================================================
//  LOOP DA FASE
// ============================================================
int rodarFase(Jogador& jog, int fase, int dif){
    int mp[11][11];gerarMapa(mp,fase);
    jog.x=1;jog.y=1;jog.vivo=true;mp[1][1]=3;

    int nIni=dif==0?3:dif==1?5:7;
    int pos[][2]={{9,9},{9,1},{1,9},{9,5},{5,9},{5,1},{1,5}};
    vector<Inimigo> ins;
    for(int i=0;i<nIni&&i<7;i++){
        int tipo=0;
        if(fase>=2&&i>=2)tipo=1;
        if(fase==3&&i==nIni-1)tipo=2;
        ins.push_back({pos[i][0],pos[i][1],0,tipo,(tipo==2?3:1),true});
        mp[pos[i][0]][pos[i][1]]=4;
    }

    vector<Bomba> bs;vector<Item> its;
    bool ativo=true,faseOk=false,portalAberto=false;
    int tempoTotal=dif==0?90:dif==1?75:60,tempo=tempoTotal;
    DWORD tTick=GetTickCount(),tLogica=GetTickCount(),tMsg=0;
    bool msgVida=false;

    if(fase>1){jog.raio=1;jog.maxBombas=1;jog.sobreviveBomba=false;jog.passaBlocos=false;}

    CONSOLE_CURSOR_INFO ci={1,FALSE};SetConsoleCursorInfo(hCon,&ci);
    system("cls");

    while(ativo){
        DWORD agora=GetTickCount();
        if(agora-tTick>=1000){tTick=agora;if(--tempo<=0){jog.vidas--;if(jog.vidas<=0)ativo=false;tempo=tempoTotal;}}
        if(msgVida&&agora-tMsg>=1500){msgVida=false;gotoxy(0,13);cout<<"                                              ";}

        renderMapa(mp,ins);
        renderHUD(jog,fase,tempo,dif,ins);

        if(kbhit()){
            int t=getch();
            if(t==27)return 2;
            if(t=='0')salvarJogo(jog,fase,dif);
            else moverJogador(mp,t,jog,bs);
        }

        if(agora-tLogica>=250){
            tLogica=agora;
            for(auto& in:ins)if(in.vivo)moverInimigo(mp,in,jog,dif);
            gerenciarBombas(mp,bs,jog,its);
            verificarItens(jog,its,mp);
            checaMorte(mp,jog,ativo,ins,faseOk);

            if(faseOk&&!portalAberto&&fase<3){portalAberto=true;mp[9][9]=8;}
            if(portalAberto&&jog.x==9&&jog.y==9){jog.pontuacao+=calcPontos(jog,fase,tempo);return 1;}
            if(faseOk&&fase==3){jog.pontuacao+=calcPontos(jog,fase,tempo);return 1;}

            if(!jog.vivo&&jog.vidas>0){
                mp[jog.x][jog.y]=2;jog.x=1;jog.y=1;mp[1][1]=3;jog.vivo=true;
                gotoxy(0,13);cor(12);cout<<"  !! VOCE PERDEU UMA VIDA !! Restam: "<<jog.vidas<<"   ";cor(7);
                msgVida=true;tMsg=agora;
            }
        }
    }
    return 0;
}

// ============================================================
//  MAIN
// ============================================================
int main(){
    system("chcp 65001 > nul");
    hCon=GetStdHandle(STD_OUTPUT_HANDLE);
    srand(time(0));

    int op;bool sair=false;
    while(!sair){
        menuPrincipal();
        do{cin>>op;}while(op<1||op>9);

        if(op==9){sair=true;continue;}
        if(op==3){telaInstrucoes();continue;}
        if(op==4){telaRegras();continue;}
        if(op==5){telaDificuldades();continue;}
        if(op==6){telaPontuacao();continue;}
        if(op==7){telaRank();continue;}
        if(op==8){telaSobre();continue;}

        Jogador jog{"",1,1,3,1,1,0,0,0,0,0,true,false,false};
        int faseIni=1,dif=0;

        if(op==2){
            if(!carregarJogo(jog,faseIni,dif)){
                system("cls");cor(12);cout<<"\n  Nenhum save encontrado! Pressione qualquer tecla...";cor(7);getch();continue;
            }
            system("cls");cor(10);cout<<"\n  Bem-vindo de volta, "<<jog.nome<<"! Pressione qualquer tecla...";cor(7);getch();
        }else{
            dif=selecionarDif();
            system("cls");cor(7);cout<<"\n  Seu nome: ";cin>>jog.nome;
        }

        bool fimJogo=false,vitoria=false;
        for(int fase=faseIni;fase<=3&&!fimJogo;fase++){
            system("cls");gotoxy(10,5);cor(14);
            cout<<">>> FASE "<<fase<<" <<< - Pressione qualquer tecla...";cor(7);getch();
            int res=rodarFase(jog,fase,dif);
            if(res==0)fimJogo=true;
            else if(res==1&&fase==3)vitoria=true;
            else if(res==2){fimJogo=true;break;}
        }

        system("cls");
        if(vitoria){cor(14);cout<<"\n\n  ╔══════════════════════════════════════╗\n  ║      PARABENS! VOCE VENCEU!          ║\n  ╚══════════════════════════════════════╝\n";}
        else{cor(12);cout<<"\n\n  ╔══════════════════════════════════════╗\n  ║            GAME OVER!                ║\n  ╚══════════════════════════════════════╝\n";}
        cor(10);cout<<"\n  Pontuacao: "<<jog.pontuacao;
        cor(7); cout<<"\n  Inimigos: "<<jog.inimigosAbatidos<<"  Caixas: "<<jog.caixasDestruidas
                    <<"  Movs: "<<jog.movimentos<<"  Bombas: "<<jog.bombasUsadas<<"\n";
        RankEntry e{jog.nome,dataHoje(),jog.pontuacao,jog.movimentos,jog.bombasUsadas};
        salvarRank(e);cor(8);cout<<"\n  Partida salva no rank!\n";
        cor(7);cout<<"\n  Pressione qualquer tecla...";getch();getch();
    }
    return 0;
}
