
const char* light2d_analytic_vs=R"(
attribute vec4 a_position;
attribute vec4 a_color;
attribute vec4 a_texCoord;

varying vec4 color;
varying vec2 texcoord;
varying vec4 worldPos;
varying vec4 shadowUV;
varying vec4 Color;

uniform vec4 _Color;
//uniform mat4 _MMatrix;
inline vec4 _ComputeGrabScreenPos (vec4 pos) 
{
      #if UNITY_UV_STARTS_AT_TOP
      float scale = -1.0;
      #else
     float scale = 1.0;
     #endif
     vec4 o = pos * 0.5f;
     o.xy = vec2(o.x, o.y*scale) + o.w;
     o.zw = pos.zw;
     return o;
}
void main()
{
	gl_Position = CC_MVPMatrix * a_position;
	texcoord = a_texCoord.xy;
	color = a_color * _Color;
	worldPos = CC_MVMatrix * a_position;//model mat
	shadowUV = _ComputeGrabScreenPos(CC_MVPMatrix * a_position);
	Color = _Color;
}
)";
const char* light2d_analytic_fs=R"(
layout (location = 0) out vec4 FragColor;

varying vec4 color;
varying vec2 texcoord;
varying vec4 worldPos;
varying vec4 shadowUV;
varying vec4 Color;


uniform sampler2D _ShadowMap;
uniform float _Attenuation;
uniform float _Intensity;

void main()
{
	float dist = distance(texcoord, vec2(0.5, 0.5));
	dist/= 0.5;
	dist = clamp(dist, 0, 1);
	float illum = 0;

	if(_Attenuation <= -1)
	{
		illum = 0;
	}
	else if (_Attenuation <= 0) // (-1, 0]
    {
	    float t = 1 / (_Attenuation + 1) - 1;
        illum = exp(-dist * t) - exp(-t) * dist;
    }
    else if (_Attenuation < 1) // (0, 1)
    {
	    float t = 1 / (1 - _Attenuation) - 1;
        dist = 1 - dist;
        illum = 1 - (exp(-dist * t) - exp(-t) * dist);
    }
    else
    {
        illum = dist >= 1 ? 0 : 1;
    }

    vec3 color1 = (illum * _Intensity * Color).rgb;
	vec2 uv = shadowUV.xy / shadowUV.w;
    
    color1 = color1 * (1 - texture2D(_ShadowMap, uv).r);
    FragColor = vec4(color1, 1);
}
)";
//	Lighting Off
//		ZWrite Off
//		Blend One One

//const char* light2d_texture_vs=R"(
//attribute vec4 a_position;
//attribute vec4 a_color;
//attribute vec4 a_texCoord;
//
//varying vec4 vertex;
//varying vec4 color;
//varying vec4 Color;
//varying vec2 texcoord;
//varying vec4 worldPos;
//varying vec4 shadowUV;
//
//uniform sampler2D _LightCookie;
//uniform vec4 _Color;
//uniform float _Attenuation;
//uniform float _Intensity;
//uniform sampler2D _ShadowMap;
//
//inline vec4 ComputeGrabScreenPos (vec4 pos) 
//{
//      #if UNITY_UV_STARTS_AT_TOP
//      float scale = -1.0;
//      #else
//     float scale = 1.0;
//     #endif
//     vec4 o = pos * 0.5f;
//     o.xy = vec2(o.x, o.y*scale) + o.w;
//     o.zw = pos.zw;
//     return o;
//    }
//
//void main()
//{
//	vertex = CC_MVPMatrix * a_position;
//	texcoord = a_texCoord;
//	worldPos = CC_MVMatrix * a_position;
//	shadowUV = ComputeGrabScreenPos(vertex);
//    Color = _Color;
//    color = _Color * a_color;
//}
//)";
//
//const char* light2d_texture_fs=R"(
//varying vec4 vertex;
//varying vec4 color;
//varying vec4 Color;
//varying vec2 texcoord;
//varying vec4 worldPos;
//varying vec4 shadowUV;
//
//uniform sampler2D _LightCookie;
//uniform vec4 _Color;
//uniform float _Attenuation;
//uniform float _Intensity;
//uniform sampler2D _ShadowMap;
//
//void main()
//{
//	vec3 color1 = texture2D(_LightCookie, texcoord.xy) * _Intensity * Color;
//	shadowUV.xy /= shadowUV.w;
//	color1 = color1 * (1 - texture2D(_ShadowMap, shadowUV).r);
//	gl_FragColor = vec4(color1, 1);
//}
//)";
//