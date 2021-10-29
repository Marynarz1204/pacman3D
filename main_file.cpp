/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "constants.h"
#include "lodepng.h"
#include "object.h"
#include "pacman.h"
#include "wall.h"
#include "ghost.h"
#include "shaderprogram.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

bool turn_right;
bool turn_left;
bool forward;

pacman Pacman("pacman.fbx");
ghost Ghost_blue("ghost.fbx",4,5);
ghost Ghost_pink("ghost.fbx", 5, 5);
ghost Ghost_orange("ghost.fbx", 5, 4);
ghost Ghost_red("ghost.fbx", 4, 4);
object O_floor("floor.fbx");
object star[4];
glm::mat4 M_ghost[4];
wall wallH[13][10];
bool wallH_binary[13][10] =
{
{1,1,1,1,1,1,1,1,1,1},
{0,1,0,0,0,0,0,0,1,0},
{0,1,0,1,0,0,1,0,1,0},
{0,0,1,0,1,1,0,1,0,0},
{0,1,0,0,0,0,0,0,1,0},
{0,1,0,0,0,0,0,0,1,0},
{0,1,0,0,1,1,0,0,1,0},
{1,0,0,0,1,1,0,0,0,1},
{0,1,0,1,0,0,1,0,1,0},
{0,1,0,0,1,1,0,0,1,0},
{0,0,1,0,0,0,0,1,0,0},
{0,1,1,0,1,1,0,1,1,0},
{1,1,1,1,1,1,1,1,1,1},
};
wall wallV[12][11];
bool wallV_binary[12][11] =
{
{1,0,0,0,1,0,1,0,0,0,1},
{1,0,0,1,0,1,0,1,0,0,1},
{1,1,0,0,0,1,0,0,0,1,1},
{1,0,0,0,0,0,0,0,0,0,1},
{1,1,1,1,1,0,1,1,1,1,1},
{1,0,0,1,1,0,1,1,0,0,1},
{1,0,1,1,0,0,0,1,1,0,1},
{1,0,1,0,0,1,0,0,1,0,1},
{1,0,0,1,0,0,0,1,0,0,1},
{1,0,0,1,0,1,0,1,0,0,1},
{1,1,0,0,1,0,1,0,0,1,1},
{1,0,0,0,0,0,0,0,0,0,1},
};




//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void key_callback(GLFWwindow* window,int key,int scancode,int action,int mod
) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) {
			turn_left = 1;
			turn_right = 0;
		}
		if (key == GLFW_KEY_RIGHT) {
			turn_right = 1;
			turn_left = 0;
		}
		if (key == GLFW_KEY_UP) {
		}
		if (key == GLFW_KEY_DOWN) {
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
		}
		if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
		}
	}
}

void drawObject(object O , glm::mat4 M)
{
	glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(M));
	glEnableVertexAttribArray(spLambertTextured->a("vertex"));
	glVertexAttribPointer(spTextured->a("vertex"), 4, GL_FLOAT, false, 0, O.verts.data());
	glEnableVertexAttribArray(spLambertTextured->a("normal"));
	glVertexAttribPointer(spLambertTextured->a("normal"), 4, GL_FLOAT, false, 0, O.norms.data());
	glEnableVertexAttribArray(spLambertTextured->a("texCoord"));
	glVertexAttribPointer(spLambertTextured->a("texCoord"), 2, GL_FLOAT, false, 0, O.texCoords.data());
	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, O.tex);
	glUniform1i(spTextured->u("tex"), 0);
	glDrawElements(GL_TRIANGLES, O.indices.size(), GL_UNSIGNED_INT, O.indices.data());
	glDisableVertexAttribArray(spLambertTextured->a("vertex"));
	glDisableVertexAttribArray(spLambertTextured->a("normal"));
	glDisableVertexAttribArray(spLambertTextured->a("texCoord"));
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
    initShaders();
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0, 0, 0, 1); //Ustaw kolor czyszczenia bufora kolorów
	glEnable(GL_DEPTH_TEST); //Włącz test głębokości na pikselach
	glfwSetKeyCallback(window, key_callback);
	Ghost_orange.loadModel(Ghost_orange.model);
	Ghost_orange.readTexture("orange.png");
	Ghost_pink.loadModel(Ghost_pink.model);
	Ghost_pink.readTexture("pink.png");
	Ghost_red.loadModel(Ghost_red.model);
	Ghost_red.readTexture("red.png");
	Ghost_blue.loadModel(Ghost_blue.model);
	Ghost_blue.readTexture("blue.png");
	Pacman.loadModel(Pacman.model);
	Pacman.readTexture("pacman.png");
	O_floor.loadModel("floor.fbx");
	O_floor.readTexture("floor.png");
	for (int i = 0; i < 4; i++)
	{
		star[i].loadModel("star.fbx");
		star[i].readTexture("star.png");
	}
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (wallH_binary[i][j])
			{
				wallH[i][j].loadModel("floor.fbx");
				wallH[i][j].readTexture("wall.png");
			}	
			wallH[i][j].exists = wallH_binary[i][j];
		}
	}
	for (int i = 0; i < 12; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			if (wallV_binary[i][j])
			{
				wallV[i][j].loadModel("floor.fbx");
				wallV[i][j].readTexture("wall.png");
			}
			wallV[i][j].exists = wallV_binary[i][j];
		}
	}
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    freeShaders();
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
	glDeleteTextures(1, &(Pacman.tex));
	glDeleteTextures(1, &(O_floor.tex));
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (wallH_binary[i][j])
				glDeleteTextures(1, &(wallH[i][j].tex));
		}
	}
	for (int i = 0; i < 12; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			if (wallV_binary[i][j])
				glDeleteTextures(1, &(wallV[i][j].tex));
		}
	}
}

void drawWalls(wall wallsH[13][10], wall wallsV[12][11])
{
	glm::mat4 M = glm::mat4(1.0f);
	M = glm::scale(M, glm::vec3(0.2f, 2.0f, 1.2f));
	M = glm::translate(M, glm::vec3(-140.0f, -10.0f,  0.0f));
	for (int i = 0; i < 13; i++)
	{
		M = glm::translate(M, glm::vec3(20.0f, 0.0f, 0.0f));
		for (int j = 0; j < 10; j++)
		{
			M = glm::translate(M, glm::vec3(0.0f, 2.0f, 0.0f));
			if (wallsH[i][j].exists == 1)
			{
				drawObject(wallsH[i][j], M);
			}
		}
		M = glm::translate(M, glm::vec3( 0.0f, -20.0f, 0.0f));
	}
	M = glm::mat4(1.0f);
	M = glm::scale(M, glm::vec3(2.0f, 0.2f, 1.2f));
	M = glm::translate(M, glm::vec3(-13.0f, -111.0f, 0.0f));
	for (int i = 0; i < 12; i++)
	{
		M = glm::translate(M, glm::vec3(2.0f, 0.0f, 0.0f));
		for (int j = 0; j < 11; j++)
		{
			M = glm::translate(M, glm::vec3(0.0f, 20.0f, 0.0f));
			if (wallsV[i][j].exists == 1)
			{
				drawObject(wallsV[i][j], M);
			}
		}
		M = glm::translate(M, glm::vec3(0.0f, -220.0f, 0.0f));
	}

}

bool visited_star[4];

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, glm::mat4 M_pacman, glm::mat4 M_ghost[4], glm::mat4 M_star, float angle) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyść bufor koloru i bufor głębokości
	 
	//-----------
	spLambertTextured->use();
	
	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku
	glm::mat4 P = glm::perspective(glm::radians(100.0f), 1.0f, 1.0f, 50.0f); //Wylicz macierz rzutowania

	drawObject(Pacman, M_pacman);
	drawObject(Ghost_orange, M_ghost[3]);
	drawObject(Ghost_pink, M_ghost[2]);
	drawObject(Ghost_red, M_ghost[1]);
	drawObject(Ghost_blue, M_ghost[0]);
	glm::mat4 M_star1 = glm::translate(M_star, glm::vec3(-36.0f, 0.0f, 0.0f));
	M_star1 = glm::rotate(M_star1, angle, glm::vec3(1.0f, 1.0f, 2.0f));
	glm::mat4 M_star2 = glm::translate(M_star, glm::vec3(20.0f, 16.0f, 0.0f));
	M_star2 = glm::rotate(M_star2, angle, glm::vec3(1.0f, 1.0f, 2.0f));
	glm::mat4 M_star3 = glm::translate(M_star, glm::vec3(12.0f, -32.0f, 0.0f));
	M_star3 = glm::rotate(M_star3, angle, glm::vec3(1.0f, 1.0f, 2.0f));
	glm::mat4 M_star4 = glm::translate(M_star, glm::vec3(-36.0f, 32.0f, 0.0f));
	M_star4 = glm::rotate(M_star4, angle, glm::vec3(1.0f, 1.0f, 2.0f));
	if(!visited_star[0])
		drawObject(star[0], M_star1);
	if (!visited_star[1])
		drawObject(star[1], M_star2);
	if (!visited_star[2])
		drawObject(star[2], M_star3);
	if (!visited_star[3])
		drawObject(star[3], M_star4);

	if (Pacman.x == 0 && Pacman.y == 7)
		visited_star[2] = 1;
	if (Pacman.x == 6 && Pacman.y == 8)
		visited_star[1] = 1;
	if (Pacman.x == 8 && Pacman.y == 1)
		visited_star[3] = 1;
	if (Pacman.x == 4 && Pacman.y == 1)
		visited_star[0] = 1;

	glm::mat4 M_floor = glm::mat4(1.0f);
	M_floor = glm::translate(M_floor, glm::vec3(0.0f, 0.0f, -1.0f));
	M_floor = glm::scale(M_floor, glm::vec3(27.0f, 22.0f, 0.1f));
	drawObject(O_floor, M_floor);
	drawWalls(wallH, wallV);
	

	glUniformMatrix4fv(spLambertTextured->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spLambertTextured->u("V"), 1, false, glm::value_ptr(V));
	//-----------
	glfwSwapBuffers(window); //Skopiuj bufor tylny do bufora przedniego
}


void GhostMove(glm::mat4 &M_ghost, ghost &Ghost)
{
	int ran = rand() % 3;
	switch (Ghost.rotation)
	{
	case 0:
		if (ran==2 && Ghost.counter % 40 == 0 && wallH[Ghost.y + 1][Ghost.x].exists == 0)
		{
			M_ghost = glm::rotate(M_ghost, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Ghost.y++;
			Ghost.rotation = 3;
		}
		else if (ran == 1 && Ghost.counter % 40 == 0 && wallH[Ghost.y][Ghost.x].exists == 0)
		{
			M_ghost = glm::rotate(M_ghost, -90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Ghost.y--;
			Ghost.rotation = 1;
		}
		else if (Ghost.counter % 40 == 0 && wallV[Ghost.y][Ghost.x].exists == 1)
		{
			Ghost.counter--;
			M_ghost = glm::translate(M_ghost, glm::vec3(0.0f, 0.0f, 0.1f));
		}
		else if (Ghost.counter % 40 == 0)
		{
			Ghost.x--;
		}
		break;
	case 1:
		if (ran == 2 && Ghost.counter % 40 == 0 && wallV[Ghost.y][Ghost.x].exists == 0)
		{
			M_ghost = glm::rotate(M_ghost, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Ghost.x--;
			Ghost.rotation = 0;
		}
		else if (ran == 1 && Ghost.counter % 40 == 0 && wallV[Ghost.y][Ghost.x + 1].exists == 0)
		{
			M_ghost = glm::rotate(M_ghost, -90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Ghost.x++;
			Ghost.rotation = 2;
		}
		else if (Ghost.counter % 40 == 0 && wallH[Ghost.y][Ghost.x].exists == 1)
		{
			Ghost.counter--;
			M_ghost = glm::translate(M_ghost, glm::vec3(0.0f, 0.0f, 0.1f));
		}
		else if (Ghost.counter % 40 == 0)
		{
			Ghost.y--;
		}
		break;
	case 2:
		if (ran == 2 && Ghost.counter % 40 == 0 && wallH[Ghost.y][Ghost.x].exists == 0)
		{
			M_ghost = glm::rotate(M_ghost, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Ghost.y--;
			Ghost.rotation = 1;
		}
		else if (ran == 1 && Ghost.counter % 40 == 0 && wallH[Ghost.y + 1][Ghost.x].exists == 0)
		{
			M_ghost = glm::rotate(M_ghost, -90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Ghost.y++;
			Ghost.rotation = 3;
		}
		else if (Ghost.counter % 40 == 0 && wallV[Ghost.y][Ghost.x + 1].exists == 1)
		{
			Ghost.counter--;
			M_ghost = glm::translate(M_ghost, glm::vec3(0.0f, 0.0f, 0.1f));
		}
		else if (Ghost.counter % 40 == 0)
		{
			Ghost.x++;
		}
		break;
	case 3:
		if (ran == 2 && Ghost.counter % 40 == 0 && wallV[Ghost.y][Ghost.x + 1].exists == 0)
		{
			M_ghost = glm::rotate(M_ghost, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Ghost.x++;
			Ghost.rotation = 2;
		}
		else if (ran == 1 && Ghost.counter % 40 == 0 && wallV[Ghost.y][Ghost.x].exists == 0)
		{
			M_ghost = glm::rotate(M_ghost, -90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Ghost.x--;
			Ghost.rotation = 0;
		}
		else if (Ghost.counter % 40 == 0 && wallH[Ghost.y + 1][Ghost.x].exists == 1)
		{
			Ghost.counter--;
			M_ghost = glm::translate(M_ghost, glm::vec3(0.0f, 0.0f, 0.1f));
		}
		else if (Ghost.counter % 40 == 0)
		{
			Ghost.y++;
		}
		break;
	}
	M_ghost = glm::translate(M_ghost, glm::vec3(0.0f, 0.0f, -0.1f));
	Ghost.counter++;
	Ghost.counter %= 40;
}

void GhostInit(glm::mat4 M_ghost[4])
{
	M_ghost[0] = glm::mat4(1.0f);
	M_ghost[1] = glm::mat4(1.0f);
	M_ghost[2] = glm::mat4(1.0f);
	M_ghost[3] = glm::mat4(1.0f);
	M_ghost[0] = glm::rotate(M_ghost[0], 90.0f * PI / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	M_ghost[0] = glm::rotate(M_ghost[0], 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	M_ghost[0] = glm::translate(M_ghost[0], glm::vec3(0.0f, 0.0f, -2.0f));
	M_ghost[1] = glm::rotate(M_ghost[1], 90.0f * PI / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	M_ghost[1] = glm::rotate(M_ghost[1], 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	M_ghost[1] = glm::translate(M_ghost[1], glm::vec3(0.0f, 0.0f, -6.0f));
	M_ghost[2] = glm::rotate(M_ghost[2], 90.0f * PI / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	M_ghost[2] = glm::rotate(M_ghost[2], 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	M_ghost[2] = glm::translate(M_ghost[2], glm::vec3(4.0f, 0.0f, -2.0f));
	M_ghost[3] = glm::rotate(M_ghost[3], 90.0f * PI / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	M_ghost[3] = glm::rotate(M_ghost[3], 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	M_ghost[3] = glm::translate(M_ghost[3], glm::vec3(4.0f, 0.0f, -6.0f));
}


int main(void)
{
	srand(time(NULL));
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1000, 1000, "OpenGL", NULL, NULL);  //Utwórz okno 1000x1000 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	glm::mat4 M_pacman = glm::mat4(1.0f);
	glm::mat4 M_star = glm::mat4(1.0f);
	M_star = glm::scale(M_star, glm::vec3(0.5f, 0.5f, 0.5f));
	M_pacman = glm::translate(M_pacman, glm::vec3(2.0f, 0.0f, 0.0f));
	GhostInit(M_ghost);
	int counter = 0;
	float angle = 0.0f;
	glfwSetTime(0); //Wyzeruj licznik czasu
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		switch (Pacman.rotation)
		{
		case 0:
			if (turn_left && counter % 40 == 0 && wallH[Pacman.y + 1][Pacman.x].exists==0)
			{
				M_pacman = glm::rotate(M_pacman, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
				turn_left = 0;
				Pacman.y++;
				Pacman.rotation = 3;
			}
			else if (turn_right && counter % 40 == 0 && wallH[Pacman.y][Pacman.x].exists == 0)
			{
				M_pacman = glm::rotate(M_pacman, -90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
				turn_right = 0;
				Pacman.y--;
				Pacman.rotation = 1;
			}
			else if (counter % 40 == 0 && wallV[Pacman.y][Pacman.x].exists == 1)
			{
				counter--;
				M_pacman = glm::translate(M_pacman, glm::vec3(-0.1f, 0.0f, 0.0f));
			}
			else if (counter % 40 == 0)
			{
				Pacman.x--;
			}
			break;
		case 1:
			if (turn_left && counter % 40 == 0 && wallV[Pacman.y][Pacman.x].exists == 0)
			{
				M_pacman = glm::rotate(M_pacman, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
				turn_left = 0;
				Pacman.x--;
				Pacman.rotation = 0;
			}
			else if (turn_right && counter % 40 == 0 && wallV[Pacman.y][Pacman.x+1].exists == 0)
			{
				M_pacman = glm::rotate(M_pacman, -90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
				turn_right = 0;
				Pacman.x++;
				Pacman.rotation = 2;
			}
			else if (counter % 40 == 0 && wallH[Pacman.y][Pacman.x].exists == 1)
			{
				counter--;
				M_pacman = glm::translate(M_pacman, glm::vec3(-0.1f, 0.0f, 0.0f));
			}
			else if (counter % 40 == 0)
			{
				Pacman.y--;
			}
			break;
		case 2:
			if (turn_left && counter % 40 == 0 && wallH[Pacman.y][Pacman.x].exists == 0)
			{
				M_pacman = glm::rotate(M_pacman, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
				turn_left = 0;
				Pacman.y--;
				Pacman.rotation = 1;
			}
			else if (turn_right && counter % 40 == 0 && wallH[Pacman.y + 1][Pacman.x].exists == 0)
			{
				M_pacman = glm::rotate(M_pacman, -90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
				turn_right = 0;
				Pacman.y++;
				Pacman.rotation = 3;
			}
			else if (counter % 40 == 0 && wallV[Pacman.y][Pacman.x + 1].exists == 1)
			{
				counter--;
				M_pacman = glm::translate(M_pacman, glm::vec3(-0.1f, 0.0f, 0.0f));
			}
			else if (counter % 40 == 0)
			{
				Pacman.x++;
			}
			break;
		case 3: 
			if (turn_left && counter % 40 == 0 && wallV[Pacman.y][Pacman.x+1].exists == 0)
			{
				M_pacman = glm::rotate(M_pacman, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
				turn_left = 0;
				Pacman.x++;
				Pacman.rotation = 2;
			}
			else if (turn_right && counter % 40 == 0 && wallV[Pacman.y][Pacman.x].exists == 0)
			{
				M_pacman = glm::rotate(M_pacman, -90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
				turn_right = 0;
				Pacman.x--;
				Pacman.rotation = 0;
			}
			else if (counter % 40 == 0 && wallH[Pacman.y + 1][Pacman.x].exists == 1)
			{
				counter--;
				M_pacman = glm::translate(M_pacman, glm::vec3(-0.1f, 0.0f, 0.0f));
			}
			else if (counter % 40 == 0)
			{
				Pacman.y++;
			}
			break;
		}
		M_pacman = glm::translate(M_pacman, glm::vec3(0.1f, 0.0f, 0.0f));
		GhostMove(M_ghost[0], Ghost_blue);
		GhostMove(M_ghost[1], Ghost_red);
		GhostMove(M_ghost[2], Ghost_pink);
		GhostMove(M_ghost[3], Ghost_orange);
		glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window,M_pacman, M_ghost, M_star, angle); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
		counter++;
		counter %= 40;
		angle += 0.1f;
		if (Pacman.x == Ghost_red.x && Pacman.y == Ghost_red.y)
			break;
		if (Pacman.x == Ghost_pink.x && Pacman.y == Ghost_pink.y)
			break;
		if (Pacman.x == Ghost_orange.x && Pacman.y == Ghost_orange.y)
			break;
		if (Pacman.x == Ghost_blue.x && Pacman.y == Ghost_blue.y)
			break;
	}

	for (int i = 0; i < 100; i++)
	{
		cout << "GAME OVER" << endl;
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
