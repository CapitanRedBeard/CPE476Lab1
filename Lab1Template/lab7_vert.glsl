attribute vec4 vertPos; // in object space
attribute vec3 vertNor; // in object space
uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

varying vec3 color; // Pass to fragment shader
varying vec4 pos;
varying vec3 normal;	

void main()
{
	vec4 norm = (uViewMatrix * uModelMatrix) * vec4(vertNor, 0.0);
	normal = norm.xyz;
	pos = uViewMatrix * uModelMatrix * vertPos;
	gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vertPos;
	//color = vec3(0.5, 0.5, 0.5);

}
