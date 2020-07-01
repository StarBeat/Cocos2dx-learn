
#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform vec2 resolution;
uniform sampler2D backbuffer;


#define PI 3.14159265359
#define T (time / .99)



mat4 rotationMatrix(vec3 axis, float angle)
{
	axis = normalize(axis);
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;

	return mat4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0,
		oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0,
		oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

vec3 rotate(vec3 v, vec3 axis, float angle)
{
	mat4 m = rotationMatrix(axis, angle);
	return (m * vec4(v, 1.0)).xyz;
}



const float aoinParam1 = 0.8;

float snow(vec2 uv,float scale)
{
	float w=smoothstep(9.,0.,-uv.y*(scale/10.));if(w<.1)return 0.;
	uv+=(time*aoinParam1)/scale;uv.y+=time*0./scale;uv.x+=sin(uv.y+time*.05)/scale;
	uv*=scale;vec2 s=floor(uv),f=fract(uv),p;float k=3.,d;
	p=.5+.35*sin(11.*fract(sin((s+p+scale)*mat2(7,3,6,5))*2.))-f;d=length(p);k=min(d,k);
	k=smoothstep(0.,k,sin(f.x+f.y)*0.01);
    	return k*w;
}

void main( void ) {

	vec2 position = (( gl_FragCoord.xy / resolution.xy ) - 0.5);	
	position.x *= resolution.x / resolution.y;
		
	vec3 color = vec3(0.);
	
	vec2 uv=(gl_FragCoord.xy*2.-resolution.xy)/min(resolution.x,resolution.y); 
	
	// uv = rotate(vec3(position, 0.), vec3(1.,0.,1.), 0.1*3.14159*sin(time)).xy;
	

	float c=.5;
	for (float i=-10.;i<10.;i+=1.)
		c+=snow(uv, i) * (1.0 + 1.0*abs(sin(time)));

	c += uv.y*sin(time);
	c += uv.x*sin(time);	
	// if ((uv.x*uv.x + uv.y*uv.y) > 0.5)
	//	c = .0;

	
	vec3 finalColor = rotate(vec3(c), vec3(1.,0.,1.), 0.1*3.14159*sin(time));

	//vec4 me =texture2D(backbuffer, uv);
	//finalColor -= me.rgb;
	
	gl_FragColor = (vec4( color, 1.0 ) + vec4(finalColor,1)) / vec4(2, 2, 2, 1);

}


const char*  shadow_vs = R"(
attribute vec4 a_position;
attribute vec4 a_color;
attribute vec4 a_texCoord;
attribute vec4 _texCoord2;

#define PI 3.14159265358979323846264338327950288419716939937510
varying vec2 A;
varying vec2 B;
varying vec2 E;
void main()
{
	gl_Position = CC_MVPMatrix * a_position; //CC_MVPMatrix * a_position;
	A = (CC_MVMatrix * a_texCoord).xy;
	B = (CC_MVMatrix * _texCoord2).xy;
	E = (CC_MVMatrix * a_position).xy;
}
)";

const char*  shadow_fs = R"(
#define PI 3.14159265358979323846264338327950288419716939937510
layout (location = 0) out vec4 FragColor;
varying vec2 A;
varying vec2 B;
varying vec2 E;

uniform vec2 _LightPos;
uniform	float _LightVolume;

float dirAngle(vec2 v)
{
	float angle = atan(v.y, v.x);
	return angle;
}
float normAngle(float angle)
{
	angle = angle - step(180, angle) * 360;
	angle = angle + step(angle, -180) * 360;
	return angle;
}
float dirBetweenAngle(vec2 v1, vec2 v2) {
	return normAngle(dirAngle(v1) - dirAngle(v2));
}

void main()
{
	vec2 CE = E - _LightPos;
	vec2 CEnorm = normalize(vec2(-CE.y, CE.x) * _LightVolume);
	vec2 dirF = (_LightPos - CEnorm) - E;
	vec2 dirG = (_LightPos + CEnorm) -E;
	vec2 dirA = A -E;
	vec2 dirB = B -E;
	float full = dirBetweenAngle(dirF, dirG);

	float ABiggerThanB =step(0, dirBetweenAngle(dirA, dirB));
	vec2 dirCW = ABiggerThanB * (dirA - dirB)+dirB;
	vec2 dirCCW = dirA + dirB -dirCW;
	float crossG = step(0, dirBetweenAngle(dirG, dirCCW)) * step(0, dirBetweenAngle(dirCW, dirG));
	float sign = crossG * 2 - 1;
	vec2 startingEdge = dirF + (dirG - dirF) * crossG;
	float valueCW = clamp(sign * dirBetweenAngle(dirCW, startingEdge) / full, 0, 1);
	float valueCCW = clamp(sign * dirBetweenAngle(dirCCW, startingEdge) / full, 0, 1);
	float occlusion = abs(valueCW - valueCCW);
	FragColor = vec4(occlusion,occlusion,occlusion, 1); 
}
)";



