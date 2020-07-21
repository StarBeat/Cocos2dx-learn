static const GLchar* pudding_shader_vs = R"(
attribute vec4 a_position;
attribute vec2 a_texCoord;
uniform float u_size;

varying vec2 v_texCoord;
void main()
{
    gl_Position = CC_MVPMatrix * a_position;
    gl_PointSize = u_size;
    v_texCoord = a_texCoord;
}
)";

// Fragment shader
static const GLchar* pudding_shader_fs = R"(

#ifdef GL_ES
precision lowp float;
#endif
varying vec2 v_texCoord;
uniform sampler2D _texture;

void main()
{
    vec4 color = texture2D(_texture, v_texCoord);
    gl_FragColor = color;// texture2D(_texture, v_texCoord);
}
)";

//
// --------- Custom Shaders ---------
//           Render Texture
//

// Vertex shader
static const GLchar* pudding_rt_shader_vs = R"(
attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

void main()
{
    gl_Position = a_position;
    v_fragmentColor = a_color;
    v_texCoord = a_texCoord;
}
)";

// Fragment shader
static const GLchar* pudding_rt_shader_fs = R"(
#ifdef GL_ES
precision lowp float;
#endif
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform float u_threshold_discard;
uniform float u_threshold_border;

void main()
{
    vec4 color = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);
    if( color.a < u_threshold_discard)
        // black or discard
        color = vec4(0,0,0,0);
    else if( color.a < u_threshold_border)
        // blue for the border
        color = vec4(0.2,0.2,0.9,1);
    gl_FragColor = color;
})";
