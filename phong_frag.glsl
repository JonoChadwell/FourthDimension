#version 330 core
in vec3 fragNor;
in vec3 fragWorld;
in vec3 viewDir;
out vec4 color;

void main(){
   vec3 MatAmb = vec3(0.2,0.2,0.2);
   vec3 MatDif = vec3(0.5, 0.5, 0.5);
   vec3 MatSpec = vec3(0.5, 0.5, 0.5);
   float shine = 2;
   vec3 sunDir = vec3(0,-1,0);
   
   vec3 lightColor = vec3(1, 1, 1);
   vec3 normLight = normalize(-sunDir);
   float ambient = .2;
   vec3 diffShade = MatDif * max(0, dot(normLight, fragNor));
   vec3 ambShade = ambient * MatAmb;
   vec3 reflect = -1 * normLight + 2 * (dot( normLight, fragNor) ) * fragNor;
   vec3 specShade = pow(max(0, dot(normalize(viewDir), normalize(reflect))), shine)
                    * MatSpec * lightColor;
   color = vec4(ambShade + diffShade + specShade, 1.0);
}
