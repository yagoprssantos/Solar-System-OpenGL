#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <stdio.h>
#include <glut.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define NUM_ESTRELAS 1000

// Variáveis de controle de rotação
float angle = 0.0; // Ângulo de rotação da cena
float planetAngle = 0.0f, moonAngle = 0.0f; // Ângulo de rotação do planeta e da lua
const float planetDistance = 3.0f, moonDistance = 0.7f; // Distância do planeta para o sol e distância da lua para o planeta

// Array para armazenar as coordenadas das estrelas
float estrelas[NUM_ESTRELAS][3];

// Variáveis para armazenar as texturas
GLuint text_sol, text_terra, text_lua;

// Função para carregar uma textura a partir de um arquivo BMP
GLuint carregarTextura(const char* filename)
{
    GLuint texture;          // Variável para armazenar o identificador da textura
    GLint width, height;     // Variáveis para armazenar a largura e altura da imagem
    unsigned char* data;     // Ponteiro para armazenar os dados da imagem

    // Abre o arquivo em modo binário para leitura
    FILE* file;
    file = fopen(filename, "rb");

    if (file == NULL) return 0; // Retorna 0 se falhou em abrir o arquivo

    // Salta para o local onde estão as informações de largura e altura da imagem
    fseek(file, 0x0012, SEEK_SET);
    // Lê a largura e altura da imagem (cada um é um inteiro de 4 bytes)
    fread(&width, sizeof(int), 1, file);
    fread(&height, sizeof(int), 1, file);
    // Salta para o início dos dados da imagem (após o cabeçalho de 54 bytes)
    fseek(file, 54, SEEK_SET);

    // Aloca memória para armazenar os dados da imagem (3 bytes por pixel)
    data = (unsigned char*)malloc(width * height * 3);
    // Lê os dados da imagem para o ponteiro 'data'
    fread(data, width * height * 3, 1, file);
    // Fecha o arquivo após a leitura
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
    // Vincula a textura recém-gerada para configuração
    glBindTexture(GL_TEXTURE_2D, texture);
    // Define o modo de combinação da textura (GL_MODULATE significa mistura com iluminação)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    // Define o filtro de interpolação para redução da textura (GL_LINEAR_MIPMAP_NEAREST)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    // Define o filtro de interpolação para ampliação da textura (GL_LINEAR)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Define o modo de repetição da textura na direção S (horizontal)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // Define o modo de repetição da textura na direção T (vertical)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Cria os níveis de mipmapping e especifica os parâmetros de construção da textura
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    // Libera a memória alocada para os dados da imagem
    free(data);

    return texture; // Retorna o identificador da textura gerada
}

// Função para inicializar as texturas do sol, planeta e lua
void inicializarTexturas()
{
    text_sol = carregarTextura("Texturas/sol.bmp");
    text_terra = carregarTextura("Texturas/terra.bmp");
    text_lua = carregarTextura("Texturas/lua.bmp");
}

// Função para gerar coordenadas aleatórias para as estrelas
void coordenadasEstrelas() {
    srand(time(NULL)); // "Resetar" de acordo com o tempo atual
    for (int i = 0; i < NUM_ESTRELAS; i++) {
        estrelas[i][0] = (float)(rand() % 2000 - 1000) / 100.0f;
        estrelas[i][1] = (float)(rand() % 2000 - 1000) / 100.0f;
        estrelas[i][2] = (float)(rand() % 2000 - 1000) / 100.0f;
    }
}

// Função para verificar se uma estrela está oculta entre o objeto e a câmera
// Parâmetros:
//   - estrelaX, estrelaY, estrelaZ: coordenadas da estrela no espaço tridimensional
//   - raioEsfera: raio da esfera que representa o objeto
//   - objetoX, objetoY, objetoZ: coordenadas do objeto no espaço tridimensional (por exemplo, câmera)
// Retorna:
//   - true se a estrela está oculta atrás do objeto em relação à câmera, caso contrário, false
bool estrelaOculta(float estrelaX, float estrelaY, float estrelaZ, float raioEsfera, float objetoX, float objetoY, float objetoZ)
{
    // Calcula a distância entre a estrela e o objeto usando o Teorema de Pitágoras
    float distancia = sqrtf(powf(estrelaX - objetoX, 2) + powf(estrelaY - objetoY, 2) + powf(estrelaZ - objetoZ, 2));

    // Verifica se a distância é menor que o raio da esfera que representa o objeto
    // Se for, a estrela está oculta atrás do objeto em relação à câmera
    return distancia < raioEsfera;
}

// Função para desenhar o sol
void desenharSol() {

    // Configuração do material e luz do sol
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
    glRotatef(90.0f, -1.0f, 0.0f, 0.0f); // Rotação para arrumar textura
    GLUquadricObj* esfera = NULL;
    esfera = gluNewQuadric();
    gluQuadricTexture(esfera, GL_TRUE);
    gluQuadricNormals(esfera, GLU_SMOOTH);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, text_sol);
    gluSphere(esfera, 1.0f, 50, 50);
    glDisable(GL_TEXTURE_2D);
}

// Função para desenhar o planeta e a lua
void desenharPlanetaELua() {

    glPushMatrix();
    
    // Configuração da luz e do material 
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

// Função para desenhar as estrelas, evitando que elas fiquem entre a câmera e o sistema
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

// Função para renderizar a cena
void renderizarCena() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa o buffer de cor e profundidade

    glMatrixMode(GL_MODELVIEW); // Seleciona a matriz de modelo e visão
    glLoadIdentity(); // Carrega a matriz identidade

    // Configuração da iluminação
    GLfloat luz_posicao[] = { 2.0, 2.0, 7.0, 1.0 }; // Posição da fonte de luz
    GLfloat luz_branca[] = { 1.0, 1.0, 1.0, 1.0 }; // Cor da luz

    glEnable(GL_LIGHT0); // Habilita a fonte de luz 0
    glEnable(GL_LIGHTING); // Habilita a iluminação

    gluLookAt(2.0, 2.0, 7.0, // Define a posição da câmera (olho)
        0.0, 0.0, 0.0, // Define o ponto para o qual a câmera está olhando (centro)
        0.0, 1.0, 0.0); // Define o vetor de orientação da câmera (cima)

    desenharEstrelas(); // Chama a função para desenhar as estrelas
    glRotatef(angle, 0.0f, 0.1f, 0.0f); // Aplica a rotação ao objeto
    desenharSol(); // Chama a função para desenhar o sol
    desenharPlanetaELua(); // Chama a função para desenhar o planeta e a lua

    glutSwapBuffers(); // Troca os buffers (evita flickering)
}

// Função de redimensionamento da janela
void redimensionarJanela(int width, int height) {
    if (height == 0) height = 1; // Evita divisão por zero

    float aspectRatio = (float)width / (float)height; // Calcula a proporção da janela

    glViewport(0, 0, width, height); // Configura a viewport

    glMatrixMode(GL_PROJECTION); // Seleciona a matriz de projeção
    glLoadIdentity(); // Carrega a matriz identidade

    gluPerspective(45.0, aspectRatio, 0.1, 100.0); // Define a perspectiva
}

// Função chamada a cada quadro de atualização
void atualizar(int value) {
    angle += 2.0f; // Incrementa o ângulo de rotação
    if (angle >= 360.0f)
        angle -= 360.0f; // Mantém o ângulo dentro do intervalo [0, 360)

    planetAngle += 0.2f; // Incrementa o ângulo de rotação do planeta
    if (planetAngle >= 360.0f)
        planetAngle -= 360.0f; // Mantém o ângulo dentro do intervalo [0, 360)

    moonAngle += 4.0f; // Incrementa o ângulo de rotação da lua
    if (moonAngle >= 360.0f)
        moonAngle -= 360.0f; // Mantém o ângulo dentro do intervalo [0, 360)

    glutPostRedisplay(); // Marca a janela para redesenho

    glutTimerFunc(16, atualizar, 0); // Chama a função novamente após 16ms (aproximadamente 60 quadros por segundo)
}

int main(int argc, char** argv) {
    glutInit(&argc, argv); // Inicializa o GLUT
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); // Configura o modo de exibição
    glutInitWindowSize(800, 600); // Define o tamanho da janela
    glutCreateWindow("Sistema Solar"); // Cria a janela com o título "Sistema Solar"

    inicializarTexturas(); // Chama a função para inicializar as texturas
    coordenadasEstrelas(); // Chama a função para inicializar as coordenadas das estrelas

    glEnable(GL_DEPTH_TEST); // Habilita o teste de profundidade

    glutDisplayFunc(renderizarCena); // Define a função de renderização da cena
    glutReshapeFunc(redimensionarJanela); // Define a função de redimensionamento da janela
    glutTimerFunc(16, atualizar, 0); // Define a função de atualização da cena

    glutMainLoop(); // Inicia o loop principal do GLUT

    return 0; // Retorna 0 ao finalizar o programa
}
