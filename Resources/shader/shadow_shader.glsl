const char*  shadow_vs = R"(
attribute vec4 a_position;
attribute vec4 a_color;
attribute vec4 a_texCoord;
attribute vec4 _texCoord2;

varying vec2 edgeA;
varying vec2 edgeB;
varying vec2 pos;
#define PI 3.14159265358979323846264338327950288419716939937510

void main()
{
	gl_Position = CC_MVPMatrix * a_position; //CC_MVPMatrix * a_position;
	edgeA =  a_texCoord.xy;
	edgeB =  _texCoord2.xy;
	pos = a_position.xy;
}
)";

const char*  shadow_fs = R"(
#define PI 3.14159265358979323846264338327950288419716939937510
layout (location = 0) out vec4 FragColor;

varying vec2 edgeA;
varying vec2 edgeB;
varying vec2 pos;

uniform float _LightVolume;

vec2 rotate(vec2 v, float ang)
{
	vec2 r = vec2(cos(ang), sin(ang));
	return vec2(r.x * v.x - r.y * v.y, r.x * v.y + r.y *v.x);
}

float cross2(vec2 u, vec2 v)
{
	return cross(vec3(u,0), vec3(v,0)).z;
}

void main()
{
	float d = distance(vec2(0, 0), pos);
	float ang = asin(_LightVolume / d);
	vec2 left = normalize(rotate(-pos, ang));
	vec2 right = normalize(rotate(-pos, -ang));
	vec2 u = normalize(edgeA - pos);
	vec2 v = normalize(edgeB - pos);

	if(cross2(u, v) > 0)
	{
		vec2 t = u;
		u = v;
		v = t;
	}
	
	float leftLeak = clamp(sign(cross2(u, left)), 0 ,1)* acos(dot(left, u));
	float rightLeak = clamp(sign(cross2(right, v)), 0, 1) * acos(dot(right, v));
	float total = acos(dot(right, left));
	float rgb = (leftLeak + rightLeak + 0.01) / total;
	vec3 color = vec3(rgb, rgb, rgb);
	color = clamp(color.rgb, 0, 1);
	FragColor = vec4(1 - color, 1.0);
}
)";



