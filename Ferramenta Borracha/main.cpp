//Autor: Raimundo Nonato Gomes Neto
// Bibliotecas utilizadas pelo OpenGL
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

// Definindo uma estrutura para representar um ponto
struct Point
{
	int x, y;
	Point(int _x, int _y) : x(_x), y(_y) {}
};

bool eraserMode = false; // Indica se a borracha está ativa ou não
float brushSize = 10.0f; // Tamanho da borracha

// Protótipo da função borracha
void borracha(int x, int y);

// Função para desenhar um pixel na coordenada (x, y)
void drawPixel(int x, int y)
{
	if (eraserMode)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 0.0f); // Cor transparente para apagar
	}
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

// Função para preencher um polígono com cor
void fillPolygon(std::vector<Point>& vertices)
{
	// Encontra as coordenadas y mínimas e máximas
	int minY = vertices[0].y;
	int maxY = vertices[0].y;
	for (size_t i = 1; i < vertices.size(); ++i)
	{
		if (vertices[i].y < minY) minY = vertices[i].y;
		if (vertices[i].y > maxY) maxY = vertices[i].y;
	}

	// Inicializa uma lista para armazenar as bordas do polígono
	std::vector<std::vector<int> > edges(maxY - minY + 1);

	// Encontra as interseções de cada aresta com as linhas horizontais e armazene as bordas
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		const Point& v1 = vertices[i];
		const Point& v2 = vertices[(i + 1) % vertices.size()];  // Próximo vértice
		// Calcula a diferença entre as coordenadas y
		int dy = v2.y - v1.y;
		if (dy != 0)
		{
			// Calcula a inclinação da aresta
			float m = static_cast<float>(v2.x - v1.x) / dy;
			int y = std::min(v1.y, v2.y);
			while (y < std::max(v1.y, v2.y))
			{
				edges[y - minY].push_back(v1.x + m * (y - v1.y));
				++y;
			}
		}
	}

	// Preenche os pixels entre as bordas do polígono para cada linha horizontal
	for (size_t i = 0; i < edges.size(); ++i)
	{
		std::sort(edges[i].begin(), edges[i].end());
		for (size_t j = 0; j < edges[i].size(); j += 2)
		{
			glBegin(GL_LINES);
			glVertex2i(edges[i][j], i + minY);
			glVertex2i(edges[i][j + 1], i + minY);
			glEnd();
		}
	}
}

// Função para desenhar e preencher as formas geométricas
void drawAndFillShapes()
{
	// Triângulo
	std::vector<Point> triangleVertices;
	triangleVertices.push_back(Point(150, 150));
	triangleVertices.push_back(Point(250, 250));
	triangleVertices.push_back(Point(50, 250));
	glColor3f(1.0f, 0.0f, 0.0f); // Cor vermelha
	fillPolygon(triangleVertices);

	// Quadrado
	std::vector<Point> squareVertices;
	squareVertices.push_back(Point(300, 150));
	squareVertices.push_back(Point(400, 150));
	squareVertices.push_back(Point(400, 250));
	squareVertices.push_back(Point(300, 250));
	glColor3f(0.0f, 1.0f, 0.0f); // Cor verde
	fillPolygon(squareVertices);

	// Hexágono
	std::vector<Point> hexagonVertices;
	hexagonVertices.push_back(Point(450, 150));
	hexagonVertices.push_back(Point(550, 200));
	hexagonVertices.push_back(Point(650, 150));
	hexagonVertices.push_back(Point(650, 250));
	hexagonVertices.push_back(Point(550, 300));
	hexagonVertices.push_back(Point(450, 250));
	glColor3f(0.0f, 0.0f, 1.0f); // Cor azul
	fillPolygon(hexagonVertices);
}

// Função de desenho
void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawAndFillShapes();
	glFlush();
}

// Função de inicialização
void init()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // Cor de fundo branca
	gluOrtho2D(0, 800, 0, 600);  // Configuração da janela de visualização
}

// Função para lidar com eventos de mouse
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// Troca o modo da borracha quando o botão esquerdo é pressionado
		eraserMode = !eraserMode;

		// Chama a função motion para desenhar ou apagar o ponto clicado
		borracha(x, y);
	}
}

// Função para lidar com movimentos do mouse
void borracha(int x, int y)
{
	// Define a cor branca para a borracha
	glColor3f(1.0f, 1.0f, 1.0f);

	// Desenha ou apaga pixels conforme a posição do mouse e o modo da borracha
	int xCenter = x;
	int yCenter = glutGet(GLUT_WINDOW_HEIGHT) - y; // Inverte o eixo y
	glBegin(GL_POLYGON);
	for (float angle = 0; angle < 360; angle += 1.0)
	{
		int xPixel = xCenter + brushSize * cos(angle * M_PI / 180);
		int yPixel = yCenter + brushSize * sin(angle * M_PI / 180);
		if (xPixel >= 0 && xPixel < glutGet(GLUT_WINDOW_WIDTH) && yPixel >= 0 && yPixel < glutGet(GLUT_WINDOW_HEIGHT))
		{
			glVertex2i(xPixel, yPixel);
		}
	}
	glEnd();
	glFlush();
}

// Função principal
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Teste: Borracha");
	init();
	glutDisplayFunc(draw);
	glutMouseFunc(mouse);
	glutMotionFunc(borracha);
	glutMainLoop();
	return 0;
}
