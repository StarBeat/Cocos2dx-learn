static const GLchar* particle_shader_vs = R"(
attribute vec4 a_position;
attribute vec2 a_texCoord;

varying vec2 v_texCoord;
void main()
{
    gl_Position = a_position;
    v_texCoord = a_texCoord;
}
)";

// Fragment shader
static const GLchar* particle_shader_fs = R"(
#ifdef GL_ES
precision lowp float;
#endif
varying vec2 v_texCoord;
uniform sampler2D _ColorMap;
uniform sampler2D _ParticleColorMap;
float COLOR_TRESHHOLD=0.2; 
void main()
{
    vec4 src = texture2D(_ColorMap, v_texCoord);
    vec4 color = texture2D(_ParticleColorMap, v_texCoord);
	vec4 finalColor= color;					

    if(color.r>0.3){ // This is for the lava effect, easiest one!
		finalColor=vec4(color.r,color.r/9.5,0.0,color.a);
		finalColor=floor(finalColor/0.05)*0.5; //The floor function for the metaball effect play around the 2 values for different effects (use the color preview at the scene!)		    
	}
	else if(color.g>0.3){ //This is for the gas effect		
	    finalColor=vec4(color.g,color.g,color.g,0.5f);// All channels white with some transparency
	    finalColor=floor(finalColor/0.05)*0.3;  //Blend it in
	     finalColor.a=0.5f; // To add some transparency		    
	}	    
	else if(color.b>0.3){	//This is for the water effect
			finalColor=vec4(0.0,color.b/2.0,color.b,0.5);
			finalColor.b=floor((finalColor.b/0.1)*0.5);
	}	 
	
    gl_FragColor = finalColor + src;
}
)";
