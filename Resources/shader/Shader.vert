


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



