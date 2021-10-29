#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;


//Atrybuty
layout (location=0) in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
layout (location=1) in vec4 normal; //wektor normalny w wierzcholku
layout (location=2) in vec2 texCoord; //wspó³rzêdne teksturowania


//Zmienne interpolowane
out vec2 i_tc;

out vec4 n;
out vec4 v;

out vec4 slight;
out vec4 tlight;
out vec4 flight;
out vec4 ilight;


void main(void) {

    vec4 i = vec4(4,0,4,0);
    ilight = normalize(V*i - V*M*vertex);

    vec4 s = vec4(4,0,4,0);
    slight = normalize(V*s - V*M*vertex);

    vec4 t = vec4(-4,0,-4,0);
    tlight = normalize(V*t - V*M*vertex);

    vec4 f = vec4(-4,0,-4,0);
    flight = normalize(V*f - V*M*vertex);

    
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex); //wektor do obserwatora w przestrzeni oka
    n = normalize(V*M*normal);
    i_tc = texCoord;
  
    gl_Position=P*V*M*vertex;
}