const char* metaball_vs = R"(
attribute vec4 a_position;
attribute vec2 a_texCoord;

uniform vec2 _center;

varying vec2 center;
varying vec2 v_texCoord;
void main()
{
	center =_center;// (CC_MVPMatrix * vec4(_center.x, _center.y, 1, 1)).xy;
    v_texCoord = a_texCoord.xy;
    gl_Position = CC_PMatrix * a_position;
}
)";


const char* metaball_fs = R"(
uniform vec2 resolution;
uniform vec2 mouse;
uniform	float radius=0.1;

varying vec2 v_texCoord;
varying vec2 center;

float MetaBall(vec2 position){
	return 1.0/(pow(position.x,2.0)+pow(position.y,2.0));
}

float Blob(vec2 position,vec2 point, float radius){
	float temp=pow(position.x-point.x,2.0)+pow(position.y-point.y,2.0);
	float result=0.0;
	if( temp<pow(radius,2.0)){		
		float distance=sqrt(pow(position.x-point.x,2.0)+pow(position.y-point.y,2.0))/radius;		
		result=pow((1.0-pow(distance,2.0)),2.0);		
	}
	return result;
}

void main() {
	vec2 position =  gl_FragCoord.xy / resolution.xy ;

	float blobValue=0.0;
	blobValue+=Blob(position,center,radius);
	blobValue+=Blob(position,mouse,radius);
	
    vec4 color = texture2D(CC_Texture0, v_texCoord);
	//blobValue *= color.a;
	//color=vec4(0.0,blobValue/2.0,blobValue, 1);
	//color=floor(color/0.1)*0.5;
	if(blobValue > 0)
	{
		color = texture2D(CC_Texture0, vec2(0.5,0.5));// * vec4(1,blobValue/2.0,blobValue, 1);
	}
	//color = color + color * blobValue;

	gl_FragColor = color;
}
)";

//
//uniform vec4 uColor;
//uniform vec2 uTouch;
//in vec4 vPosition;
//out vec4 fragColor;
//const float radius = 0.1;
//
//float energy(float R, vec2 fragPos, vec2 energyPos) {
//    return (R * R) / ((fragPos.x - energyPos.x) * (fragPos.x - energyPos.x) + (fragPos.y - energyPos.y) * (fragPos.y - energyPos.y));
//}
//
//void main() {
//    float centerDist = length(vPosition.xy);
//    if(centerDist > radius) {
//        float touchDist = length(vPosition.xy - uTouch);
//        if (touchDist < radius) {
//            fragColor = uColor;
//        } else {
//            float fragEnergy = energy(radius, vPosition.xy, vec2(0.0)) + energy(radius, vPosition.xy, uTouch);
//            if (fragEnergy > 1.2) {
//                fragColor = uColor;
//            } else {
//                discard;
//            }
//        }
//    } else {
//        fragColor = uColor;
//    }
//}
//