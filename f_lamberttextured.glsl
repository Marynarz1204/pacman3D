#version 330


uniform sampler2D tex;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

//Zmienne interpolowane
in vec2 i_tc; //wspó³rzêdne teksturowania

in vec4 n;
in vec4 v;
in vec4 slight;
in vec4 tlight;
in vec4 flight;
in vec4 ilight;

void main(void) {
	
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);

	vec4 msl = normalize(slight);
	vec4 mfl = normalize(flight);
	vec4 mtl = normalize(tlight);
	vec4 mil = normalize(ilight);

    vec4 kd = texture(tex, i_tc);
	vec4 ks = vec4(1,1,1,1);

	float ity = 0.3;
	float nl =  ity * clamp(dot(mn,msl),0,1) + ity * clamp(dot(mn,mfl),0,1) + ity * clamp(dot(mn,mtl),0,1) +  ity *  clamp(dot(mn,mil),0,1);
	pixelColor = vec4(kd.rgb * nl, kd.a);

}
