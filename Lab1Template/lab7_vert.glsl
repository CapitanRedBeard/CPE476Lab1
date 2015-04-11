attribute vec4 vertPos; // in object space
attribute vec3 vertNor; // in object space
uniform mat4 P;
uniform mat4 MV;

varying vec3 color; // Pass to fragment shader
varying vec4 pos;
varying vec3 normal;	

void main()
{
	vec4 norm = MV * vec4(vertNor, 0.0);
	normal = norm.xyz;
	pos = MV * vertPos;
	gl_Position = P * MV * vertPos;
	//color = vec3(0.5, 0.5, 0.5);

}
