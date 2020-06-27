
const char* primitive_vs=R"(
attribute vec4 a_position;
attribute vec4 a_color;
attribute vec4 a_texCoord;

varying vec4 v_fragmentColor;

void main()
{
    gl_Position = CC_MVPMatrix * a_position;
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