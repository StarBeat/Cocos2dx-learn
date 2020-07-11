const char*  deferred_light_vs = R"(
attribute vec4 a_position;
attribute vec4 a_texCoord;

varying	 vec4 texcoord;
void main()
{
	gl_Position = a_position;
	texcoord = a_texCoord;
}
)";

const char*  deferred_light_fs = R"(

varying	 vec4 texcoord;

uniform sampler2D _LightMap;
uniform sampler2D _ColorMap;
uniform float _ExposureLimit;
uniform vec3 _GlobalLight;

void main()
{
	vec3 ambient = _GlobalLight;
	vec3 light = ambient + texture2D(_LightMap, texcoord.xy).rgb;
	vec3 color = texture2D(_ColorMap, texcoord.xy).rgb;
	if(color.r >0 || color.g >0 || color.b>0)
		light *= color;
	if(_ExposureLimit > 0)
	{
		light = clamp(light, 0, _ExposureLimit);
	}
	gl_FragColor = vec4(light, 1);
}
)";