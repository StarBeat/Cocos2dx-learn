
const char* primitive_vs=R"(
attribute vec4 a_position;
attribute vec4 a_color;
attribute vec4 a_texCoord;
layout (location = 3) in mat4 _instanceMatrix;

varying vec4 v_fragmentColor;
void main()
{
    gl_Position = _instanceMatrix * a_position;
    v_fragmentColor = a_color;
}
)";
const char* primitive_fs=R"(
varying vec4 v_fragmentColor;

void main()
{
    gl_FragColor = v_fragmentColor;
}
)";