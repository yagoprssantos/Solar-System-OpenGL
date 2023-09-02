#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <stdio.h>
#include <glut.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define NUM_ESTRELAS 1000

// Vari�veis de controle de rota��o
float angle = 0.0; // �ngulo de rota��o da cena
float planetAngle = 0.0f, moonAngle = 0.0f; // �ngulo de rota��o do planeta e da lua
const float planetDistance = 3.0f, moonDistance = 0.7f; // Dist�ncia do planeta para o sol e dist�ncia da lua para o planeta

// Array para armazenar as coordenadas das estrelas
float estrelas[NUM_ESTRELAS][3];

// Vari�veis para armazenar as texturas
GLuint text_sol, text_terra, text_lua;

// Fun��o para carregar uma textura a partir de um arquivo BMP
GLuint carregarTextura(const char* filename)
{
    GLuint texture;          // Vari�vel para armazenar o identificador da textura
    GLint width, height;     // Vari�veis para armazenar a largura e altura da imagem
    unsigned char* data;     // Ponteiro para armazenar os dados da imagem

    // Abre o arquivo em modo bin�rio para leitura
    FILE* file;
    file = fopen(filename, "rb");

    if (file == NULL) return 0; // Retorna 0 se falhou em abrir o arquivo

    // Salta para o local onde est�o as informa��es de largura e altura da imagem
    fseek(file, 0x0012, SEEK_SET);
    // L� a largura e altura da imagem (cada um � um inteiro de 4 bytes)
    fread(&width, sizeof(int), 1, file);
    fread(&height, sizeof(int), 1, file);
    // Salta para o in�cio dos dados da imagem (ap�s o cabe�alho de 54 bytes)
    fseek(file, 54, SEEK_SET);

    // Aloca mem�ria para armazenar os dados da imagem (3 bytes por pixel)
    data = (unsigned char*)malloc(width * height * 3);
    // L� os dados da imagem para o ponteiro 'data'
    fread(data, width * height * 3, 1, file);
    // Fecha o arquivo ap�s a leitura
    fclose(file);

    // Ajusta a ordem das componentes de cor (de BGR para RGB)
    for (int i = 0; i < height * width; ++i)
    {
        int index = i * 3;
        unsigned char B, R;
        B = data[index];
        R = data[index + 2];
        data[index] = R;
        data[index + 2] = B;
    }

    // Gera um identificador de textura
    glGenTextures(1, &texture);
    // Vincula a textura rec�m-gerada para configura��o
    glBindTexture(GL_TEXTURE_2D, texture);
    // Define o modo de combina��o da textura (GL_MODULATE significa mistura com ilumina��o)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    // Define o filtro de interpola��o para redu��o da textura (GL_LINEAR_MIPMAP_NEAREST)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    // Define o filtro de interpola��o para amplia��o da textura (GL_LINEAR)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Define o modo de repeti��o da textura na dire��o S (horizontal)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // Define o modo de repeti��o da textura na dire��o T (vertical)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Cria os n�veis de mipmapping e especifica os par�metros de constru��o da textura
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    // Libera a mem�ria alocada para os dados da imagem
    free(data);

    return texture; // Retorna o identificador da textura gerada
}

// Fun��o para inicializar as texturas do sol, planeta e lua
void inicializarTexturas()
{
    text_sol = carregarTextura("Texturas/sol.bmp");
    text_terra = carregarTextura("Texturas/terra.bmp");
    text_lua = carregarTextura("Texturas/lua.bmp");
}

// Fun��o para gerar coordenadas aleat�rias para as estrelas
void coordenadasEstrelas() {
    srand(time(NULL)); // "Resetar" de acordo com o tempo atual
    for (int i = 0; i < NUM_ESTRELAS; i++) {
        estrelas[i][0] = (float)(rand() % 2000 - 1000) / 100.0f;
        estrelas[i][1] = (float)(rand() % 2000 - 1000) / 100.0f;
        estrelas[i][2] = (float)(rand() % 2000 - 1000) / 100.0f;
    }
}

// Fun��o para verificar se uma estrela est� oculta entre o objeto e a c�mera
// Par�metros:
//   - estrelaX, estrelaY, estrelaZ: coordenadas da estrela no espa�o tridimensional
//   - raioEsfera: raio da esfera que representa o objeto
//   - objetoX, objetoY, objetoZ: coordenadas do objeto no espa�o tridimensional (por exemplo, c�mera)
// Retorna:
//   - true se a estrela est� oculta atr�s do objeto em rela��o � c�mera, caso contr�rio, false
bool estrelaOculta(float estrelaX, float estrelaY, float estrelaZ, float raioEsfera, float objetoX, float objetoY, float objetoZ)
{
    // Calcula a dist�ncia entre a estrela e o objeto usando o Teorema de Pit�goras
    float distancia = sqrtf(powf(estrelaX - objetoX, 2) + powf(estrelaY - objetoY, 2) + powf(estrelaZ - objetoZ, 2));

    // Verifica se a dist�ncia � menor que o raio da esfera que representa o objeto
    // Se for, a estrela est� oculta atr�s do objeto em rela��o � c�mera
    return distancia < raioEsfera;
}

// Fun��o para desenhar o sol
void desenharSol() {

    // Configura��o do material e luz do sol
    GLfloat sol_emission[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat sol_especular1[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat sol_brilho = 0.0f;

    glMaterialfv(GL_FRONT, GL_EMISSION, sol_emission);
    glMaterialfv(GL_FRONT, GL_SPECULAR, sol_especular1);
    glMaterialf(GL_FRONT, GL_SHININESS, sol_brilho);

    GLfloat sol_posicao[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat sol_ambiente[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat sol_difusa[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat sol_especular2[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, sol_posicao);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sol_ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sol_difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sol_especular2);

    // Desenha o sol
    glRotatef(90.0f, -1.0f, 0.0f, 0.0f); // Rota��o para arrumar textura
    GLUquadricObj* esfera = NULL;
    esfera = gluNewQuadric();
    gluQuadricTexture(esfera, GL_TRUE);
    gluQuadricNormals(esfera, GLU_SMOOTH);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, text_sol);
    gluSphere(esfera, 1.0f, 50, 50);
    glDisable(GL_TEXTURE_2D);
}

// Fun��o para desenhar o planeta e a lua
void desenharPlanetaELua() {

    glPushMatrix();
    
    // Configura��o da luz e do material 
    GLfloat planeta_ambiente[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat planeta_difusa[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat planeta_especular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat planeta_emissao[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat planeta_brilho = 0.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, planeta_ambiente);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, planeta_difusa);
    glMaterialfv(GL_FRONT, GL_SPECULAR, planeta_especular);
    glMaterialfv(GL_FRONT, GL_EMISSION, planeta_emissao);
    glMaterialf(GL_FRONT, GL_SHININESS, planeta_brilho);

    glRotatef(planetAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(planetDistance, 0.0f, 0.0f);

    // Desenha o planeta
    GLUquadricObj* esfera = NULL;
    esfera = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricTexture(esfera, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D, text_terra);
    gluSphere(esfera, 0.25f, 50, 50);
    glDisable(GL_TEXTURE_2D);

    // Desenha a lua
    glRotatef(moonAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(moonDistance, 0.0f, 0.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, text_lua);
    gluSphere(esfera, 0.07f, 50, 50);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

// Fun��o para desenhar as estrelas, evitando que elas fiquem entre a c�mera e o sistema
void desenharEstrelas() {
    for (int i = 0; i < NUM_ESTRELAS; i++) {
        if (!estrelaOculta(estrelas[i][0], estrelas[i][1], estrelas[i][2], 5.5f, 0.0f, 0.0f, 0.0f)) {
            glPushMatrix();
            glTranslatef(estrelas[i][0], estrelas[i][1], estrelas[i][2] + 1.0f);
            GLfloat corEstrela[] = { 1.0f, 1.0f, 1.0f, 1.0f };

            // Define a cor da estrela
            glMaterialfv(GL_FRONT, GL_DIFFUSE, corEstrela);

            // Desenha a esfera que representa a estrela
            glutSolidSphere(0.01f, 10, 10);

            glPopMatrix();
        }
    }
}

// Fun��o para renderizar a cena
void renderizarCena() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa o buffer de cor e profundidade

    glMatrixMode(GL_MODELVIEW); // Seleciona a matriz de modelo e vis�o
    glLoadIdentity(); // Carrega a matriz identidade

    // Configura��o da ilumina��o
    GLfloat luz_posicao[] = { 2.0, 2.0, 7.0, 1.0 }; // Posi��o da fonte de luz
    GLfloat luz_branca[] = { 1.0, 1.0, 1.0, 1.0 }; // Cor da luz

    glEnable(GL_LIGHT0); // Habilita a fonte de luz 0
    glEnable(GL_LIGHTING); // Habilita a ilumina��o

    gluLookAt(2.0, 2.0, 7.0, // Define a posi��o da c�mera (olho)
        0.0, 0.0, 0.0, // Define o ponto para o qual a c�mera est� olhando (centro)
        0.0, 1.0, 0.0); // Define o vetor de orienta��o da c�mera (cima)

    desenharEstrelas(); // Chama a fun��o para desenhar as estrelas
    glRotatef(angle, 0.0f, 0.1f, 0.0f); // Aplica a rota��o ao objeto
    desenharSol(); // Chama a fun��o para desenhar o sol
    desenharPlanetaELua(); // Chama a fun��o para desenhar o planeta e a lua

    glutSwapBuffers(); // Troca os buffers (evita flickering)
}

// Fun��o de redimensionamento da janela
void redimensionarJanela(int width, int height) {
    if (height == 0) height = 1; // Evita divis�o por zero

    float aspectRatio = (float)width / (float)height; // Calcula a propor��o da janela

    glViewport(0, 0, width, height); // Configura a viewport

    glMatrixMode(GL_PROJECTION); // Seleciona a matriz de proje��o
    glLoadIdentity(); // Carrega a matriz identidade

    gluPerspective(45.0, aspectRatio, 0.1, 100.0); // Define a perspectiva
}

// Fun��o chamada a cada quadro de atualiza��o
void atualizar(int value) {
    angle += 2.0f; // Incrementa o �ngulo de rota��o
    if (angle >= 360.0f)
        angle -= 360.0f; // Mant�m o �ngulo dentro do intervalo [0, 360)

    planetAngle += 0.2f; // Incrementa o �ngulo de rota��o do planeta
    if (planetAngle >= 360.0f)
        planetAngle -= 360.0f; // Mant�m o �ngulo dentro do intervalo [0, 360)

    moonAngle += 4.0f; // Incrementa o �ngulo de rota��o da lua
    if (moonAngle >= 360.0f)
        moonAngle -= 360.0f; // Mant�m o �ngulo dentro do intervalo [0, 360)

    glutPostRedisplay(); // Marca a janela para redesenho

    glutTimerFunc(16, atualizar, 0); // Chama a fun��o novamente ap�s 16ms (aproximadamente 60 quadros por segundo)
}

int main(int argc, char** argv) {
    glutInit(&argc, argv); // Inicializa o GLUT
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); // Configura o modo de exibi��o
    glutInitWindowSize(800, 600); // Define o tamanho da janela
    glutCreateWindow("Sistema Solar"); // Cria a janela com o t�tulo "Sistema Solar"

    inicializarTexturas(); // Chama a fun��o para inicializar as texturas
    coordenadasEstrelas(); // Chama a fun��o para inicializar as coordenadas das estrelas

    glEnable(GL_DEPTH_TEST); // Habilita o teste de profundidade

    glutDisplayFunc(renderizarCena); // Define a fun��o de renderiza��o da cena
    glutReshapeFunc(redimensionarJanela); // Define a fun��o de redimensionamento da janela
    glutTimerFunc(16, atualizar, 0); // Define a fun��o de atualiza��o da cena

    glutMainLoop(); // Inicia o loop principal do GLUT

    return 0; // Retorna 0 ao finalizar o programa
}
