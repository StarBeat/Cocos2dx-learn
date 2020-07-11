const char*  bg_shader_vs = R"(
attribute vec4 a_position;
void main()
{
	gl_Position = a_position;
}
)";
const char* bg_shader_fs0=R"(
#ifdef GL_ES
precision mediump float;
#endif

// glslsandbox uniforms
uniform float time;
uniform vec2 resolution;

// shadertoy emulation
#define iTime time
#define iResolution resolution

// --------[ Original ShaderToy begins here ]---------- //
const int ITERATIONS = 110;
const float SPEED = 0.25;
const float PAN = 2.8;
const float MASK_VIS = 0.0; // Set between 0-1 to visualize masks

float vortex(vec2 uv, float dist, float seed, float bias, float offset)
{
    float ang = atan(uv.y, uv.x)+sin(dist+0.1*seed)*(1.2-offset)*2.0;
    ang += 3.14159*(0.01*seed);
    return clamp((sin((ang)*(3.0+offset*float(ITERATIONS)))+bias)/(1.0+bias), 0.0, 1.0);
}
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = (fragCoord-vec2(iResolution.xy*0.5))/iResolution.y;
    float texel = 1.0/iResolution.y;
    float dist = length(uv);
    vec3 col = vec3(0.0);
    for (int i=0; i<ITERATIONS; i++) 
    {
        float offset = float(i)/float(ITERATIONS);
        float seed = 1000.0*fract(3.1379136*floor(iTime*SPEED+offset));
        float time = fract(iTime*SPEED+offset);
        vec2 pan = vec2(0.0, time*PAN);
        float maskA = vortex(uv, dist, seed+100.0*float(i), -0.998+texel, offset);
        float dist2 = length(uv+pan);
        float maskB = vortex(uv+pan, dist2, seed+42.0*float(i), -0.99+texel, offset);
        float radius = pow(((maskA*maskA)+(maskB*maskB)), 2.0);
        float fade = time*(1.0-time);
        float mask = maskA*maskB*fade*radius;
        col = mix(col, vec3(1.50+2.0*dist-1.2*offset-1.1*radius, 0.75-0.5*offset-0.5*dist, 1.0-0.8*dist), mask);
        col = max(vec3(maskA*MASK_VIS, maskB*MASK_VIS+0.1, max(maskA*MASK_VIS*2.0, maskB*MASK_VIS)+0.12)*fade, col);
    }
    fragColor = vec4(col*3.0,0.5);
}
)";


const char* bg_shader_fs1=R"(
/* 030720N TIME TRAVEL
 * Original shader from: https://www.shadertoy.com/view/WtG3RD
 */

#ifdef GL_ES
precision mediump float;
#endif

// glslsandbox uniforms
uniform float time;
uniform vec2 resolution;

// shadertoy emulation
#define iTime time
#define iResolution resolution


// --------[ Original ShaderToy begins here ]---------- //
//CBS
//Parallax scrolling fractal galaxy.
//Inspired by JoshP's Simplicity shader: https://www.shadertoy.com/view/lslGWr

// http://www.fractalforums.com/new-theories-and-research/very-simple-formula-for-fractal-patterns/
float field(in vec3 p,float s) {
	float strength = 7. + .03 * log(1.e-6 + fract(sin(iTime) * 4373.11));
	float accum = s/4.;
	float prev = 0.;
	float tw = 0.;
	for (int i = 0; i < 26; ++i) {
		float mag = dot(p, p);
		p = abs(p) / mag + vec3(-.5, -.4, -1.5);
		float w = exp(-float(i) / 7.);
		accum += w * exp(-strength * pow(abs(mag - prev), 2.2));
		tw += w;
		prev = mag;
	}
	return max(0., 5. * accum / tw - .7);
}

// Less iterations for second layer
float field2(in vec3 p, float s) {
	float strength = 7. + .03 * log(1.e-6 + fract(sin(iTime) * 4373.11));
	float accum = s/4.;
	float prev = 0.;
	float tw = 0.;
	for (int i = 0; i < 18; ++i) {
		float mag = dot(p, p);
		p = abs(p) / mag + vec3(-.5, -.4, -1.5);
		float w = exp(-float(i) / 7.);
		accum += w * exp(-strength * pow(abs(mag - prev), 2.2));
		tw += w;
		prev = mag;
	}
	return max(0., 5. * accum / tw - .7);
}

vec3 nrand3( vec2 co )
{
	vec3 a = fract( cos( co.x*8.3e-3 + co.y )*vec3(1.3e5, 4.7e5, 2.9e5) );
	vec3 b = fract( sin( co.x*0.3e-3 + co.y )*vec3(8.1e5, 1.0e5, 0.1e5) );
	vec3 c = mix(a, b, 0.5);
	return c;
}


#define c_conj(a) vec2(a.x, -a.y)
#define c_exp(a) vec2(exp(a.x)*cos(a.y), exp(a.x)*sin(a.y))
#define c_sqr(a) vec2(a.x*a.x-a.y*a.y, 2.*a.x*a.y)
#define c_mul(a, b) vec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x)
#define c_div(a, b) vec2((a.x*b.x+a.y*b.y)/(b.x*b.x+b.y*b.y), (a.y*b.x-a.x*b.y)/(b.x*b.x+b.y*b.y))
#define c_sin(a) vec2(sin(a.x)*cosh(a.y), cos(a.x)*sinh(a.y))
#define c_cos(a) vec2(cos(a.x)*cosh(a.y), -sin(a.x)*sinh(a.y))
#define c_cartToPolar(a) vec2(length(a), atan(a.y, a.x))
#define c_polarToCart(a) a.x * vec2(cos(a.y), sin(a.y))


void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    vec2 uv = 2. * fragCoord.xy / iResolution.xy - 1.;
	
	vec2 uvs = uv * iResolution.xy / max(iResolution.x, iResolution.y);
	uvs = c_polarToCart(uvs);
	vec3 p = vec3(uvs / 4., 0) + vec3(1., -1.3, 0.);
	p += .2 * vec3(sin(iTime / 16.), sin(iTime / 12.),  sin(iTime / 128.));
	
	
	/*
	p.xy = c_exp(p.xy);
	p.xy = c_exp(p.xy);
	*/
	
	//uv = c_exp(-uv);
	
	float freqs[4];
	//Sound
	freqs[0] = 0.0;
	freqs[1] = 0.2;
	freqs[2] = 0.025;
	freqs[3] = 0.05;

	float t = field(p,freqs[2]);
	float v = (1. - exp((abs(uv.x) - 1.) * 6.)) * (1. - exp((abs(uv.y) - 1.) * 6.));
	
    //Second Layer
	vec3 p2 = vec3(uvs / (4.+sin(iTime*0.11)*0.2+0.2+sin(iTime*0.15)*0.3+0.4), 1.5) + vec3(2., -1.3, -1.);
	p2 += 0.25 * vec3(sin(iTime / 16.), sin(iTime / 12.),  sin(iTime / 128.));
	float t2 = field2(p2,freqs[3]);
	vec4 c2 = mix(.4, 1., v) * vec4(1.3 * t2 * t2 * t2 ,1.8  * t2 * t2 , t2* freqs[0], t2);
	
	
	//Let's add some stars
	//Thanks to http://glsl.heroku.com/e#6904.0
	vec2 seed = p.xy * 2.0;	
	seed = floor(seed * iResolution.x);
	vec3 rnd = nrand3( seed );
	vec4 starcolor = vec4(pow(rnd.y,40.0));
	
	//Second Layer
	vec2 seed2 = p2.xy * 2.0;
	seed2 = floor(seed2 * iResolution.x);
	vec3 rnd2 = nrand3( seed2 );
	starcolor += vec4(pow(rnd2.y,40.0));
	
	fragColor = mix(freqs[3]-.3, 1., v) * vec4(1.5*freqs[2] * t * t* t , 1.2*freqs[1] * t * t, freqs[3]*t, 1.0)+c2+starcolor;
}
)";

const char* bg_shader_fs2=R"(

const int NUM_STEPS = 8;
const float PI	 	= 3.141592;
const float EPSILON	= 1e-3;
#define EPSILON_NRM (0.1 / iResolution.x)
#define AA

// sea
const int ITER_GEOMETRY = 3;
const int ITER_FRAGMENT = 5;
const float SEA_HEIGHT = 0.6;
const float SEA_CHOPPY = 4.0;
const float SEA_SPEED = 0.8;
const float SEA_FREQ = 0.16;
const vec3 SEA_BASE = vec3(0.0,0.09,0.18);
const vec3 SEA_WATER_COLOR = vec3(0.8,0.9,0.6)*0.6;
#define SEA_TIME (1.0 + iTime * SEA_SPEED)
const mat2 octave_m = mat2(1.6,1.2,-1.2,1.6);

// math
mat3 fromEuler(vec3 ang) {
	vec2 a1 = vec2(sin(ang.x),cos(ang.x));
    vec2 a2 = vec2(sin(ang.y),cos(ang.y));
    vec2 a3 = vec2(sin(ang.z),cos(ang.z));
    mat3 m;
    m[0] = vec3(a1.y*a3.y+a1.x*a2.x*a3.x,a1.y*a2.x*a3.x+a3.y*a1.x,-a2.y*a3.x);
	m[1] = vec3(-a2.y*a1.x,a1.y*a2.y,a2.x);
	m[2] = vec3(a3.y*a1.x*a2.x+a1.y*a3.x,a1.x*a3.x-a1.y*a3.y*a2.x,a2.y*a3.y);
	return m;
}
float hash( vec2 p ) {
	float h = dot(p,vec2(127.1,311.7));	
    return fract(sin(h)*43758.5453123);
}
float noise( in vec2 p ) {
    vec2 i = floor( p );
    vec2 f = fract( p );	
	vec2 u = f*f*(3.0-2.0*f);
    return -1.0+2.0*mix( mix( hash( i + vec2(0.0,0.0) ), 
                     hash( i + vec2(1.0,0.0) ), u.x),
                mix( hash( i + vec2(0.0,1.0) ), 
                     hash( i + vec2(1.0,1.0) ), u.x), u.y);
}

// lighting
float diffuse(vec3 n,vec3 l,float p) {
    return pow(dot(n,l) * 0.4 + 0.6,p);
}
float specular(vec3 n,vec3 l,vec3 e,float s) {    
    float nrm = (s + 8.0) / (PI * 8.0);
    return pow(max(dot(reflect(e,n),l),0.0),s) * nrm;
}

// sky
vec3 getSkyColor(vec3 e) {
    e.y = (max(e.y,0.0)*0.8+0.2)*0.8;
    return vec3(pow(1.0-e.y,2.0), 1.0-e.y, 0.6+(1.0-e.y)*0.4) * 1.1;
}

// sea
float sea_octave(vec2 uv, float choppy) {
    uv += noise(uv);        
    vec2 wv = 1.0-abs(sin(uv));
    vec2 swv = abs(cos(uv));    
    wv = mix(wv,swv,wv);
    return pow(1.0-pow(wv.x * wv.y,0.65),choppy);
}

float map(vec3 p) {
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    vec2 uv = p.xz; uv.x *= 0.75;
    
    float d, h = 0.0;    
    for(int i = 0; i < ITER_GEOMETRY; i++) {        
    	d = sea_octave((uv+SEA_TIME)*freq,choppy);
    	d += sea_octave((uv-SEA_TIME)*freq,choppy);
        h += d * amp;        
    	uv *= octave_m; freq *= 1.9; amp *= 0.22;
        choppy = mix(choppy,1.0,0.2);
    }
    return p.y - h;
}

float map_detailed(vec3 p) {
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    vec2 uv = p.xz; uv.x *= 0.75;
    
    float d, h = 0.0;    
    for(int i = 0; i < ITER_FRAGMENT; i++) {        
    	d = sea_octave((uv+SEA_TIME)*freq,choppy);
    	d += sea_octave((uv-SEA_TIME)*freq,choppy);
        h += d * amp;        
    	uv *= octave_m; freq *= 1.9; amp *= 0.22;
        choppy = mix(choppy,1.0,0.2);
    }
    return p.y - h;
}

vec3 getSeaColor(vec3 p, vec3 n, vec3 l, vec3 eye, vec3 dist) {  
    float fresnel = clamp(1.0 - dot(n,-eye), 0.0, 1.0);
    fresnel = pow(fresnel,3.0) * 0.5;
        
    vec3 reflected = getSkyColor(reflect(eye,n));    
    vec3 refracted = SEA_BASE + diffuse(n,l,80.0) * SEA_WATER_COLOR * 0.12; 
    
    vec3 color = mix(refracted,reflected,fresnel);
    
    float atten = max(1.0 - dot(dist,dist) * 0.001, 0.0);
    color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;
    
    color += vec3(specular(n,l,eye,60.0));
    
    return color;
}

// tracing
vec3 getNormal(vec3 p, float eps) {
    vec3 n;
    n.y = map_detailed(p);    
    n.x = map_detailed(vec3(p.x+eps,p.y,p.z)) - n.y;
    n.z = map_detailed(vec3(p.x,p.y,p.z+eps)) - n.y;
    n.y = eps;
    return normalize(n);
}

float heightMapTracing(vec3 ori, vec3 dir, out vec3 p) {  
    float tm = 0.0;
    float tx = 1000.0;    
    float hx = map(ori + dir * tx);
    if(hx > 0.0) return tx;   
    float hm = map(ori + dir * tm);    
    float tmid = 0.0;
    for(int i = 0; i < NUM_STEPS; i++) {
        tmid = mix(tm,tx, hm/(hm-hx));                   
        p = ori + dir * tmid;                   
    	float hmid = map(p);
		if(hmid < 0.0) {
        	tx = tmid;
            hx = hmid;
        } else {
            tm = tmid;
            hm = hmid;
        }
    }
    return tmid;
}

vec3 getPixel(in vec2 coord, float time) {    
    vec2 uv = coord / iResolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;    
        
    // ray
    vec3 ang = vec3(sin(time*3.0)*0.1,sin(time)*0.2+0.3,time);    
    vec3 ori = vec3(0.0,3.5,time*5.0);
    vec3 dir = normalize(vec3(uv.xy,-2.0)); dir.z += length(uv) * 0.14;
    dir = normalize(dir) * fromEuler(ang);
    
    // tracing
    vec3 p;
    heightMapTracing(ori,dir,p);
    vec3 dist = p - ori;
    vec3 n = getNormal(p, dot(dist,dist) * EPSILON_NRM);
    vec3 light = normalize(vec3(0.0,1.0,0.8)); 
             
    // color
    return mix(
        getSkyColor(dir),
        getSeaColor(p,n,light,dir,dist),
    	pow(smoothstep(0.0,-0.02,dir.y),0.2));
}

// main
void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    float time = iTime * 0.3 + iMouse.x*0.01;
	
#ifdef AA
    vec3 color = vec3(0.0);
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
        	vec2 uv = fragCoord+vec2(i,j)/3.0;
    		color += getPixel(uv, time);
        }
    }
    color /= 9.0;
#else
    vec3 color = getPixel(fragCoord, time);
#endif
    
    // post
	fragColor = vec4(pow(color,vec3(0.65)), 1.0);
}
)";

const char* bg_shader_fs3=R"(
const float PI  = 3.14159265359;

void pR(inout vec2 p, float a) {
    p = cos(a)*p + sin(a)*vec2(p.y, -p.x);
}

float smin(float a, float b, float k){
    float f = clamp(0.5 + 0.5 * ((a - b) / k), 0., 1.);
    return (1. - f) * a + f  * b - f * (1. - f) * k;
}

float smax(float a, float b, float k) {
    return -smin(-a, -b, k);
}

float time;

vec4 leaf(vec3 p, vec2 uv) {
    float thick = clamp(uv.y, .7, 1.);
    thick = 1.;
    float th = thick * .16;
    pR(p.xz, -uv.x);
    float width = mix(.5, .1, min(uv.y, 1.));
    width = .75 / uv.y;
    width *= thick;
    vec3 n = normalize(vec3(1,0,width));
    float d = -dot(p, n);
    d = max(d, dot(p, n * vec3(1,1,-1)));
    float len = mix(PI / 1.2, PI / 2., pow(uv.y/2.9, 2.));
    len = max(len, 0.);
    pR(p.yz, PI / 2. - len);
    d = smax(d, p.y, thick);
    d = smax(d, abs(length(p) - uv.y) - thick * th, th);
    vec2 uuv = vec2(
        atan(p.y, p.z) / -len,
        p.x
   	);
    vec3 col = mix(vec3(0), vec3(.5,1,.7) * .05, 1.-smoothstep(.0, .5, uuv.x));
    col += vec3(.06,.0,.03) * max(1. - uv.y / 2., 0.);
    col = mix(col, col * .2, 1.-smoothstep(.0, .2, uuv.x));
    return vec4(d, col);
}

bool lightingPass;

vec4 opU(vec4 a, vec4 b) {
    return a.x < b.x ? a : b;
}

vec4 bloom(vec3 p) {

    float bound = length(p - vec3(0,-1.2,0)) - 3.3;
    bound = max(bound, p.y - 1.1);
    if (bound > .01 && ! lightingPass) {
        return vec4(bound, 0, 0, 0);
    }

    vec2 cc = vec2(5., 8.);
    if (iMouse.z > 0.) {
    	cc = floor(iMouse.xy / iResolution.xy * 10.);
    }
    float aa = atan(cc.x / cc.y);
    float r = (PI*2.) / sqrt(cc.x*cc.x + cc.y*cc.y);
    mat2 rot = mat2(cos(aa), -sin(aa), sin(aa), cos(aa));
    
    vec2 offset = vec2(1, 2) * time * r * rot;
    
    vec2 uv = vec2(
        atan(p.x, p.z),
        length(p)
    );

    uv -= offset;

    uv = rot * uv;
    vec2 cell = round(uv / r);

    vec4 d = vec4(1e12, vec3(0));

    d = opU(d, leaf(p, ((cell + vec2(-1, 0)) * rot * r) + offset));
    d = opU(d, leaf(p, ((cell + vec2(0, -1)) * rot * r) + offset));
    d = opU(d, leaf(p, ((cell + vec2(0, 0)) * rot * r) + offset));
    d = opU(d, leaf(p, ((cell + vec2(1, -1)) * rot * r) + offset));
    d = opU(d, leaf(p, ((cell + vec2(1, 0)) * rot * r) + offset));

    //*
    d = opU(d, leaf(p, ((cell + vec2(-1, -1)) * rot * r) + offset));
   	d = opU(d, leaf(p, ((cell + vec2(-1, 1)) * rot * r) + offset));
    d = opU(d, leaf(p, ((cell + vec2(0, 1)) * rot * r) + offset));
    d = opU(d, leaf(p, ((cell + vec2(1, 1)) * rot * r) + offset));
	//*/

    return d;
}

vec4 map(vec3 p) {
    return bloom(p);
}

vec3 calcNormal(vec3 pos){
    float eps = .0005;
    vec2 e = vec2(1.0,-1.0) * 0.5773;
    return normalize(
        e.xyy * map(pos + e.xyy * eps).x + 
		e.yyx * map(pos + e.yyx * eps).x + 
		e.yxy * map(pos + e.yxy * eps).x + 
		e.xxx * map(pos + e.xxx * eps).x
    );
}

// https://www.shadertoy.com/view/lsKcDD
float softshadow( in vec3 ro, in vec3 rd, in float mint, in float tmax )
{
    float res = 1.0;
    float t = mint;
    float ph = 1e10;
    
    for( int i=0; i<64; i++ )
    {
        float h = map( ro + rd*t ).x;
        res = min( res, 10.0*h/t );
        t += h;
        if( res<0.0001 || t>tmax ) break;
        
    }
    return clamp( res, 0.0, 1.0 );
}

// https://www.shadertoy.com/view/Xds3zN
float calcAO( in vec3 pos, in vec3 nor )
{
    float occ = 0.0;
    float sca = 1.0;
    for( int i=0; i<5; i++ )
    {
        float hr = 0.01 + 0.12*float(i)/4.0;
        vec3 aopos =  nor * hr + pos;
        float dd = map( aopos ).x;
        occ += -(dd-hr)*sca;
        sca *= 0.95;
    }
    return clamp( 1.0 - 3.0*occ, 0.0, 1.0 );    
}

mat3 calcLookAtMatrix( in vec3 ro, in vec3 ta, in float roll )
{
    vec3 ww = normalize( ta - ro );
    vec3 uu = normalize( cross(ww,vec3(sin(roll),cos(roll),0.0) ) );
    vec3 vv = normalize( cross(uu,ww));
    return mat3( uu, vv, ww );
}

//#define AA 3

void mainImage(out vec4 fragColor, in vec2 fragCoord) {

    vec3 col;
    vec3 tot = vec3(0.0);

    float mTime = mod(iTime / 2., 1.);
    time = mTime;

    vec2 o = vec2(0);

    #ifdef AA
    for( int m=0; m<AA; m++ )
    for( int n=0; n<AA; n++ )
    {
    // pixel coordinates
    o = vec2(float(m),float(n)) / float(AA) - 0.5;
    // time coordinate (motion blurred, shutter=0.5)
    float d = 0.5*sin(fragCoord.x*147.0)*sin(fragCoord.y*131.0);
    time = mTime - 0.1*(1.0/24.0)*(float(m*AA+n)+d)/float(AA*AA-1);
    #endif
        lightingPass = false;

        vec2 p = (-iResolution.xy + 2.0*(fragCoord+o))/iResolution.y;

        vec3 camPos = vec3(0.5, 7.4, -8.7) * .9;
        mat3 camMat = calcLookAtMatrix( camPos, vec3(0,-1.4,0), -0.5);
        vec3 rd = normalize( camMat * vec3(p.xy,2.8) );

        vec3 pos = camPos;
        float rayLength = 0.;
        float dist = 0.;
        bool bg = false;
        vec4 res;

        for (int i = 0; i < 100; i++) {
            rayLength += dist;
            pos = camPos + rd * rayLength;
            res = map(pos);
            dist = res.x;

            if (abs(dist) < .001) {
                break;
            }
            
            if (rayLength > 16.) {
                bg = true;
                break;
            }
        }

        col = vec3(.19,.19,.22) * 1.9;
        col = vec3(.35,.24,.0) * 1.8;
		col = vec3(.5,1,.7) * .05;
        
        if ( ! bg) {
            
            lightingPass = true;
            
			vec3 nor = calcNormal(pos);
            float occ = calcAO( pos, nor );
            vec3  lig = normalize( vec3(-.2, 1.5, .3) );
            vec3  lba = normalize( vec3(.5, -1., -.5) );
            vec3  hal = normalize( lig - rd );
            float amb = sqrt(clamp( 0.5+0.5*nor.y, 0.0, 1.0 ));
            float dif = clamp( dot( nor, lig ), 0.0, 1.0 );
            float bac = clamp( dot( nor, lba ), 0.0, 1.0 )*clamp( 1.0-pos.y,0.0,1.0);
            float fre = pow( clamp(1.0+dot(nor,rd),0.0,1.0), 2.0 );

            occ = mix(1., occ, .8);
            
            dif *= softshadow( pos, lig, 0.001, .9 );

            float spe = pow( clamp( dot( nor, hal ), 0.0, 1.0 ),16.0)*
                        dif *
                        (0.04 + 0.96*pow( clamp(1.0+dot(hal,rd),0.0,1.0), 5.0 ));

            vec3 lin = vec3(0.0);
            lin += 2.80*dif*vec3(1.30,1.00,0.70);
            lin += 0.55*amb*vec3(0.40,0.60,1.15)*occ;
            lin += 1.55*bac*vec3(0.25,0.25,0.25)*occ*vec3(2,0,1);
            lin += 0.25*fre*vec3(1.00,1.00,1.00)*occ;

            col = res.yzw;
        	col = col*lin;
			col += 5.00*spe*vec3(1.10,0.90,0.70);

            //col = nor * .5 + .5;
            //col = max(dot(vec3(.1,1,-.2), nor), 0.) * vec3(.2);
        }

        tot += col;
    #ifdef AA
    }
    tot /= float(AA*AA);
    #endif

    col = tot;
    col *= 1.3;    
    col = pow( col, vec3(0.4545) );

    fragColor = vec4(col,1.0);
}

)";

const char* bg_shader_fs4=R"(
mat2 rot(in float a){float c = cos(a), s = sin(a);return mat2(c,s,-s,c);}
const mat3 m3 = mat3(0.33338, 0.56034, -0.71817, -0.87887, 0.32651, -0.15323, 0.15162, 0.69596, 0.61339)*1.93;
float mag2(vec2 p){return dot(p,p);}
float linstep(in float mn, in float mx, in float x){ return clamp((x - mn)/(mx - mn), 0., 1.); }
float prm1 = 0.;
vec2 bsMo = vec2(0);

vec2 disp(float t){ return vec2(sin(t*0.22)*1., cos(t*0.175)*1.)*2.; }

vec2 map(vec3 p)
{
    vec3 p2 = p;
    p2.xy -= disp(p.z).xy;
    p.xy *= rot(sin(p.z+iTime)*(0.1 + prm1*0.05) + iTime*0.09);
    float cl = mag2(p2.xy);
    float d = 0.;
    p *= .61;
    float z = 1.;
    float trk = 1.;
    float dspAmp = 0.1 + prm1*0.2;
    for(int i = 0; i < 5; i++)
    {
		p += sin(p.zxy*0.75*trk + iTime*trk*.8)*dspAmp;
        d -= abs(dot(cos(p), sin(p.yzx))*z);
        z *= 0.57;
        trk *= 1.4;
        p = p*m3;
    }
    d = abs(d + prm1*3.)+ prm1*.3 - 2.5 + bsMo.y;
    return vec2(d + cl*.2 + 0.25, cl);
}

vec4 render( in vec3 ro, in vec3 rd, float time )
{
	vec4 rez = vec4(0);
    const float ldst = 8.;
	vec3 lpos = vec3(disp(time + ldst)*0.5, time + ldst);
	float t = 1.5;
	float fogT = 0.;
	for(int i=0; i<130; i++)
	{
		if(rez.a > 0.99)break;

		vec3 pos = ro + t*rd;
        vec2 mpv = map(pos);
		float den = clamp(mpv.x-0.3,0.,1.)*1.12;
		float dn = clamp((mpv.x + 2.),0.,3.);
        
		vec4 col = vec4(0);
        if (mpv.x > 0.6)
        {
        
            col = vec4(sin(vec3(5.,0.4,0.2) + mpv.y*0.1 +sin(pos.z*0.4)*0.5 + 1.8)*0.5 + 0.5,0.08);
            col *= den*den*den;
			col.rgb *= linstep(4.,-2.5, mpv.x)*2.3;
            float dif =  clamp((den - map(pos+.8).x)/9., 0.001, 1. );
            dif += clamp((den - map(pos+.35).x)/2.5, 0.001, 1. );
            col.xyz *= den*(vec3(0.005,.045,.075) + 1.5*vec3(0.033,0.07,0.03)*dif);
        }
		
		float fogC = exp(t*0.2 - 2.2);
		col.rgba += vec4(0.06,0.11,0.11, 0.1)*clamp(fogC-fogT, 0., 1.);
		fogT = fogC;
		rez = rez + col*(1. - rez.a);
		t += clamp(0.5 - dn*dn*.05, 0.09, 0.3);
	}
	return clamp(rez, 0.0, 1.0);
}

float getsat(vec3 c)
{
    float mi = min(min(c.x, c.y), c.z);
    float ma = max(max(c.x, c.y), c.z);
    return (ma - mi)/(ma+ 1e-7);
}

//from my "Will it blend" shader (https://www.shadertoy.com/view/lsdGzN)
vec3 iLerp(in vec3 a, in vec3 b, in float x)
{
    vec3 ic = mix(a, b, x) + vec3(1e-6,0.,0.);
    float sd = abs(getsat(ic) - mix(getsat(a), getsat(b), x));
    vec3 dir = normalize(vec3(2.*ic.x - ic.y - ic.z, 2.*ic.y - ic.x - ic.z, 2.*ic.z - ic.y - ic.x));
    float lgt = dot(vec3(1.0), ic);
    float ff = dot(dir, normalize(ic));
    ic += 1.5*dir*sd*ff*lgt;
    return clamp(ic,0.,1.);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{	
	vec2 q = fragCoord.xy/iResolution.xy;
    vec2 p = (gl_FragCoord.xy - 0.5*iResolution.xy)/iResolution.y;
    bsMo = (iMouse.xy - 0.5*iResolution.xy)/iResolution.y;
    
    float time = iTime*3.;
    vec3 ro = vec3(0,0,time);
    
    ro += vec3(sin(iTime)*0.5,sin(iTime*1.)*0.,0);
        
    float dspAmp = .85;
    ro.xy += disp(ro.z)*dspAmp;
    float tgtDst = 3.5;
    
    vec3 target = normalize(ro - vec3(disp(time + tgtDst)*dspAmp, time + tgtDst));
    ro.x -= bsMo.x*2.;
    vec3 rightdir = normalize(cross(target, vec3(0,1,0)));
    vec3 updir = normalize(cross(rightdir, target));
    rightdir = normalize(cross(updir, target));
	vec3 rd=normalize((p.x*rightdir + p.y*updir)*1. - target);
    rd.xy *= rot(-disp(time + 3.5).x*0.2 + bsMo.x);
    prm1 = smoothstep(-0.4, 0.4,sin(iTime*0.3));
	vec4 scn = render(ro, rd, time);
		
    vec3 col = scn.rgb;
    col = iLerp(col.bgr, col.rgb, clamp(1.-prm1,0.05,1.));
    
    col = pow(col, vec3(.55,0.65,0.6))*vec3(1.,.97,.9);

    col *= pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.12)*0.7+0.3; //Vign
    
	fragColor = vec4( col, 1.0 );
})";
const char* bg_shader_fs5=R"(
float hash(vec3 p) {
    p = fract(p * 0.3183099 + 0.1);
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

float noise(vec3 x) {
    vec3 i = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix(hash(i + vec3(0, 0, 0)), 
                       hash(i + vec3(1, 0, 0)), f.x),
                   mix(hash(i + vec3(0, 1, 0)), 
                       hash(i + vec3(1, 1, 0)), f.x), f.y),
               mix(mix(hash(i + vec3(0, 0, 1)), 
                       hash(i + vec3(1, 0, 1)), f.x),
                   mix(hash(i + vec3(0, 1, 1)), 
                       hash(i + vec3(1, 1, 1)), f.x), f.y), f.z);
}

mat2 rotate(float a) {
    float s = sin(a);
    float c = cos(a);
    return mat2(c, -s, s, c);
}

float sphere(vec4 s) {
    return length(s.xyz) - s.w;
}

vec4 getGlow(float minPDist) {
    float mainGlow = minPDist * 1.2;
    mainGlow = pow(mainGlow, 32.0);
    mainGlow = clamp(mainGlow, 0.0, 1.0);
    float outerGlow = minPDist * 0.4;
    outerGlow = pow(outerGlow, 2.0);
    outerGlow = clamp(outerGlow, 0.0, 1.0);
    vec4 glow = vec4(10, 5, 3, mainGlow);
    glow += vec4(0, 0, 0, outerGlow);
    glow.a = min(glow.a, 1.0);
    return glow;
}

float getDist(vec3 p) {
    vec3 diskPos = -p;
    float diskDist = sphere(vec4(diskPos, 5.0));
    diskDist = max(diskDist, diskPos.y - 0.01);
    diskDist = max(diskDist, -diskPos.y - 0.01);
    diskDist = max(diskDist, -sphere(vec4(-p, 1.5) * 10.0));
    if(diskDist < 2.0)
    {
        vec3 c = vec3(length(diskPos), diskPos.y, atan(diskPos.z + 1.0, diskPos.x + 1.0) * 0.5);
        c *= 10.0;
        diskDist += noise(c) * 0.4;
        diskDist += noise(c * 2.5) * 0.2;
    }
    return diskDist;
}

vec4 raymarch(vec3 ro, vec3 rd) {
    vec3 p = ro;
    float glow = 0.0;
    for (int i = 0; i < 700; i++) {
        float dS = getDist(p);
        glow = max(glow, 1.0 / (dS + 1.0));
        vec3 bdir = normalize(-p);
        float bdist = length(p);
        dS = min(dS, bdist) * 0.04;
        if(dS > 30.0) break;
        if(bdist < 1.0) {
            vec4 gcol = getGlow(glow);
            vec4 c = vec4(0.0, 0.0, 0.0, 1.0);
            c.rgb = mix(c.rgb, gcol.rgb, gcol.a);
            return c;
        }
        bdist = pow(bdist + 1.0, 2.0);
        bdist = dS * 1.0 / bdist;
        rd = mix(rd, bdir, bdist);
        p += rd * max(dS, 0.01);
    }
    vec4 c = vec4(texture(iChannel0, rd.xy).rgb * 0.2, 1.0);
    vec4 gcol = getGlow(glow);
    c.rgb = mix(c.rgb, gcol.rgb, gcol.a);
    return c;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = (fragCoord - 0.5 * iResolution.xy) / iResolution.y;
    vec3 ro = vec3(0, cos(iTime * 0.5) * 10.0, sin(iTime * 0.5) * 10.0);
    vec3 rd = normalize(vec3(uv.x, uv.y, 1));
    rd.yz *= rotate(iTime * 0.5 + 1.57);
    vec4 c = raymarch(ro, rd);
    fragColor = vec4(c.rgb, 1.0);
})";
const char* bg_shader_fs6=R"(
#define MAX_DISTANCE 80.0
#define MAX_STEP 120
#define EPSILON 0.0001
#define PI 3.1415
#define RIM_COLOR vec3(0.1,0.05,0.2)
#define LIGHT_COLOR vec3(0.03,0.2,0.3)
#define LIGHT_DIR vec3(cos(iTime),1.0,sin(iTime))
#define SPHERE_COLOR vec3(0.2, 0.6, 1.0)
#define GLOW vec3(0.1, 0.8, 1.0)

//http://iquilezles.org/www/articles/smin/smin.htm
vec3 sdMin (vec3 d1, vec3 d2)
{
    return d1.x < d2.x ? d1 : d2;
}

vec3 sdMax (vec3 d1, vec3 d2)
{
    return d1.x > d2.x ? d1 : d2;
}

float sdSmoothMin( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h); 
}


float smin(in float a, in float b, in float k)
{
    float h = max( k - abs(a-b), 0.0);
    return min(a,b) - h*h*0.25/k;
}

float smax(in float a, in float b, in float k)
{
    float h = max( k - abs(a-b), 0.0);
    return max(a,b) + h*h*0.25/k;
}

float sphereSDF(vec3 p, float r) 
{
    return length(p) - r;
}

float sdVerticalCapsule( vec3 p, float h, float r )
{
  p.y -= clamp( p.y, 0.0, h );
  return length( p ) - r;
}

float disp (vec3 p) 
{
    float n = texture(iChannel2, p.xy+p.yz+p.xz).r;
    return  (sin(p.z * 25.2 + 10.0*n) + sin(p.x * 28.9 + 10.0*n));
}

float disp1 (vec3 p)
{
    //return (sin(p.x) + sin(p.z)) * sin(p.y);
    return texture(iChannel1, p.xz*0.1).r;
}

float leg(vec3 p, float h, float r) 
{
    vec3 q = p-vec3(0.0,-1.0,0.0) ;
    q.xz += 0.5*sin(q.y*0.3);
    float scale = mix(1.0, 2.2, smoothstep(h-10.0,h, q.y));
    r *= scale;
    float d = sdVerticalCapsule(q, h, r);
    d += 0.008*disp(q*0.5);
    return d;
}

float shell(vec3 p, float r) 
{
    float t = iTime*0.1;
    float scale = mix(0.6+0.2*sin(t), 2.2+0.8*sin(t), smoothstep(-1.2,2.5, -0.6*p.y));
    r *= scale;
    float s1 = sphereSDF(p , r);
    s1 = abs(s1)-0.1;
    
    s1 += 0.18*disp1(p*2.3)*(0.5+0.5*sin(t)); // holes
    s1 *= 0.5;
    
    float plane = dot(p, normalize(vec3(0.0,-1.0,0.0)))-3.65;
    
    //plane -= 0.08*disp1(p*8.0);;
    float d = s1 > plane ? s1 : plane;
    
    return d;
}

mat3 getCamera( in vec3 ro, in vec3 ta) 
{
	vec3 cw = normalize(ta-ro);
	vec3 cp = vec3(0.0, 1.0, 0.0);
	vec3 cu = normalize( cross(cw,cp) );
	vec3 cv =          ( cross(cu,cw) );
    return mat3( cu, cv, cw );
}


float noise(in vec2 uv) {
    return texture(iChannel1, uv/64.0).r;
}

float smoothNoise(in vec2 uv) {
    vec2 luv = fract(uv); //range from 0.0 to 1.0
    vec2 id = floor(uv); //the integer part of uv, 0, 1, 2
    luv = luv*luv*(3.0 - 2.0*luv); //similar to smoothstep
    
    //get values from the cordinates of a square
    float bl = noise(id);
    float br = noise(id + vec2(1.0, 0.0));
    float tl = noise(id + vec2(0.0, 1.0));
    float tr = noise(id + vec2(1.0, 1.0));
    
    float b = mix(bl, br, luv.x); //interpolate between bl and br
    float t = mix(tl, tr, luv.x); //interpolate between tl and tr
    
    return mix(b, t, luv.y);
}

float hash21(vec2 p) {
    p = fract(p*vec2(133.7, 337.1));
    p += dot(p, p+vec2(37.1,17.33));
    return fract(p.x*p.y);
}

vec3 offset = vec3(0.0);
vec4 sceneSDF(vec3 p) 
{
    
    vec3 q = p; // org pos
    
    //head
    float d1 = shell(q, 3.5);
    
    //leg
    float h = 15.0;
    float r = 0.9;
    vec3 q1 = q - vec3(0.0, -h, 0.0);
    float d2 = leg(q1, h, r);
    
    //ground
    float d3 = q1.y + 2.0*texture(iChannel1, q1.xz*0.01).r;
    
    float d = sdSmoothMin(d1, d2, 0.5);
    d = sdSmoothMin(d, d3, 2.0);
    vec3 rst = vec3(d, 1.0, 0.0);
    
    //glow
    float v = 0.2;
    vec3 npos = v*(p-vec3(0.0, -h-1.0, 0.0));
    vec2 nid = floor(vec2(npos.x+0.5, npos.z+0.5));
    vec3 fid = vec3(fract(npos.x+0.5)-0.5, npos.y, fract(npos.z+0.5)-0.5);
    float nn = hash21(nid*3.0);
    vec3 fpos = fid + 0.2*vec3(sin(nn*112.33), 0.0, cos(nn*171.3));
    float rr = pow(nn,3.0);
    fpos.y += 0.08*sin(nid.x * nid.y + iTime*5.0);
    float s2 = sphereSDF(fpos, 0.02+0.2*rr)/v;
    rst = sdMin(rst, vec3(s2, 2.0, 0.0));
    return vec4(rst, s2);
}

vec4 marching( in vec3 ro, in vec3 rd )
{
    vec4 rst = vec4(0.0);
    float t = 0.01;
    float minDist = MAX_DISTANCE;
    for ( int i = 0; i < MAX_STEP; ++i )
    {
        vec3 p = ro + t * rd;
        vec4 dist = sceneSDF(p);
        minDist = min(minDist, dist.w/t);
        rst = vec4(t, dist.y, minDist, dist.w);
        if ( abs(dist.x)< EPSILON || t>MAX_DISTANCE) break;
        t += dist.x; 
    }
    
    if ( t>MAX_DISTANCE )
    {
        rst = vec4(MAX_DISTANCE, -1.0, minDist, MAX_DISTANCE);
    }
    
    return rst;
}

vec3 getNormal(vec3 p) 
{
    return normalize(
            vec3(
                sceneSDF(vec3(p.x + EPSILON, p.y, p.z)).x - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)).x,
                sceneSDF(vec3(p.x, p.y + EPSILON, p.z)).x - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)).x,
                sceneSDF(vec3(p.x, p.y, p.z  + EPSILON)).x - sceneSDF(vec3(p.x, p.y, p.z - EPSILON)).x
            )
    	);
}

vec3 testSurf(vec2 p)
{
    float f0 = mod(floor(p.x*2.0) + floor(p.y*2.0), 4.0);
    float f1 = mod(floor(p.x*4.0) + floor(p.y*4.0), 2.0);
    vec3 col = mix(vec3(0.8, 0.5, 0.4), vec3(0.5, 0.3, 0.7), f0);
    col = mix(col, vec3(0.2, 0.4, 0.3), f1);
    
    return col;
}

vec3 shading(vec4 hit, vec3 ro, vec3 rd) 
{
    vec3 p = ro + hit.x * rd;
    vec3 nor = getNormal(p);
    
    vec3 col = vec3(0.0);
    vec3 surfCol = vec3(0.0);
    vec3 coeff = vec3(0.04, 1.0, 1.0); //ambient, diffuse, specular
    vec3 p1 = p - offset;
    float n = texture(iChannel2, p.xz*0.2).r;
    float nf = texture(iChannel2, floor(p1.xz)).r;
    vec2 polar = vec2(atan(p1.z, p1.x), 0.5+0.1*n);
    float detail = texture(iChannel1,  polar).r;
    vec3 w0 = nor * nor;
    vec3 p2 = 0.2*p1;
    vec3 noiseTex = w0.xxx * texture(iChannel2, p2.yz).rgb 
        			+ w0.yyy * texture(iChannel2, p2.xz).rgb
        			+ w0.zzz * texture(iChannel2, p2.xy).rgb;
    float rimPow = 8.0;
    float t = iTime;
    if (hit.y < 1.5)
    {
        float h = 0.5+0.5*sin(p.y*0.5+1.3); 
        surfCol = mix(vec3(0.1,0.58,0.85), vec3(0.0), h);
        surfCol *= detail;
        float tk = 0.5+0.5*(sin(length(p)+t*3.0)*cos(length(p)+t*2.0));
        surfCol += pow(n,64.0)*vec3(0.0, 100.0,100.0)*tk;
        surfCol *= smoothstep(-10.0, 0.0, p1.y);   
        
        vec3 lightDir = normalize(LIGHT_DIR); 
        vec3 viewDir = normalize(-rd);
        vec3 reflectDir = normalize(reflect(-viewDir, nor));
        float spec = pow(max(dot(reflectDir, viewDir), 0.0), 32.0); 
        float diff = max( dot(nor, lightDir), 0.0);

        float rim = 1.0-max(dot(nor, viewDir), 0.0);
        float rimS = pow(rim, rimPow);
        vec3 rimCol = RIM_COLOR*rimS;
        vec3 refCol = texture(iChannel0, reflectDir.xy).rgb;

        surfCol = coeff.x*surfCol + (coeff.y*surfCol*diff + coeff.z*spec)*LIGHT_COLOR;
        surfCol += rimCol;
    } 
    else if (hit.y < 2.5)
    {
        surfCol = vec3(0.01);
    }
    return surfCol;
}

vec3 render(in vec2 fragCoord) 
{
    vec2 uv = fragCoord/iResolution.xy;
    uv -= 0.5; 
    uv.x *= iResolution.x/iResolution.y; 
    vec2 mo = vec2(0.01) + iMouse.xy  / iResolution.xy ;
    mo = -1.0 + 2.0 * mo;
    
    vec3 ro = vec3(25.0 * cos(mo.x * 2.0 * PI), 0.0, 25.0 * sin(mo.x * 2.0 * PI));
    vec3 ta = vec3(0.0, -5.0, 0.0);
    mat3 cam = getCamera(ro, ta);
    vec3 rd = normalize(cam * vec3(uv, 1.0));
    vec4 hit = marching(ro, rd);
    vec3 col = vec3(0.0,0.001,0.003);
    if (hit.x < MAX_DISTANCE) 
    {
       col = shading(hit, ro, rd);
    }
   
    //glow
    vec3 p = ro + rd * hit.x;
    hit.z = clamp(hit.z, 0.0, 3.0);
    float glow0 = exp(-180.0*hit.z);
    float glow1 = min(pow(0.0013/hit.z, 32.0), 1.0);
    vec3 glowCol = vec3(0.0);
    
    float v = 0.2;
    vec3 npos = v*p;
    vec3 nid = floor(npos+0.5);
    float vc = hash21(nid.xz);
    vec3 vCol = vec3(0.0,1.0,4.0)*vec3(0.0,1.0,4.0);
    vCol.r += 3.0*vc;
    vCol.g += 1.0*fract(vc*111.77);
    glowCol += 0.4*vCol*glow0;  
    glowCol += vCol*glow1; 
    float t = sin(iTime+vc*5.0);
    float y = 3.0*t*(1.0-t); 
    glowCol *= 0.5+0.5*y;
    col += glowCol;
    // fog
    col = mix( col, vec3(0.0), 1.0-exp( -0.001*hit.x*hit.x ) );
    return col;
    
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec3 col = render(fragCoord);
    
    col = pow(col, vec3(1.0/2.2));  

    // Output to screen
    fragColor = vec4(col,1.0);
})";
const char* bg_shader_fs7[]={R"(
#define AA 1
// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdSphere( in vec3 p, in vec4 s )
{
    return length(p-s.xyz) - s.w;
}
// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdEllipsoid( in vec3 p, in vec3 c, in vec3 r )
{
    return (length( (p-c)/r ) - 1.0) * min(min(r.x,r.y),r.z);
}

// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdCircle( in vec2 p, in vec2 c, in float r )
{
    return length(p-c) - r;
}

// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdTorus( vec3 p, vec2 t )
{
    return length( vec2(length(p.xz)-t.x,p.y) )-t.y;
}

// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdCapsule( vec3 p, vec3 a, vec3 b, float r )
{
	vec3 pa = p-a, ba = b-a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	return length( pa - ba*h ) - r;
}

// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
vec2 udSegment( vec3 p, vec3 a, vec3 b )
{
	vec3 pa = p-a, ba = b-a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	return vec2( length( pa - ba*h ), h );
}

// http://research.microsoft.com/en-us/um/people/hoppe/ravg.pdf
float det( vec2 a, vec2 b ) { return a.x*b.y-b.x*a.y; }
vec3 getClosest( vec2 b0, vec2 b1, vec2 b2 ) 
{
    float a =     det(b0,b2);
    float b = 2.0*det(b1,b0);
    float d = 2.0*det(b2,b1);
    float f = b*d - a*a;
    vec2  d21 = b2-b1;
    vec2  d10 = b1-b0;
    vec2  d20 = b2-b0;
    vec2  gf = 2.0*(b*d21+d*d10+a*d20); gf = vec2(gf.y,-gf.x);
    vec2  pp = -f*gf/dot(gf,gf);
    vec2  d0p = b0-pp;
    float ap = det(d0p,d20);
    float bp = 2.0*det(d10,d0p);
    float t = clamp( (ap+bp)/(2.0*a+b+d), 0.0 ,1.0 );
    return vec3( mix(mix(b0,b1,t), mix(b1,b2,t),t), t );
}

vec4 sdBezier( vec3 a, vec3 b, vec3 c, vec3 p )
{
	vec3 w = normalize( cross( c-b, a-b ) );
	vec3 u = normalize( c-b );
	vec3 v =          ( cross( w, u ) );

	vec2 a2 = vec2( dot(a-b,u), dot(a-b,v) );
	vec2 b2 = vec2( 0.0 );
	vec2 c2 = vec2( dot(c-b,u), dot(c-b,v) );
	vec3 p3 = vec3( dot(p-b,u), dot(p-b,v), dot(p-b,w) );

	vec3 cp = getClosest( a2-p3.xy, b2-p3.xy, c2-p3.xy );

	return vec4( sqrt(dot(cp.xy,cp.xy)+p3.z*p3.z), cp.z, length(cp.xy), p3.z );
}

// http://iquilezles.org/www/articles/smin/smin.htm
float smin( float a, float b, float k )
{
	float h = clamp( 0.5 + 0.5*(b-a)/k, 0.0, 1.0 );
	return mix( b, a, h ) - k*h*(1.0-h);
}

// http://iquilezles.org/www/articles/smin/smin.htm
float smax( float a, float b, float k )
{
	float h = clamp( 0.5 + 0.5*(b-a)/k, 0.0, 1.0 );
	return mix( a, b, h ) + k*h*(1.0-h);
}

// http://iquilezles.org/www/articles/smin/smin.htm
vec3 smax( vec3 a, vec3 b, float k )
{
	vec3 h = clamp( 0.5 + 0.5*(b-a)/k, 0.0, 1.0 );
	return mix( a, b, h ) + k*h*(1.0-h);
}

//---------------------------------------------------------------------------

float hash1( float n )
{
    return fract(sin(n)*43758.5453123);
}

vec3 hash3( float n )
{
    return fract(sin(n+vec3(0.0,13.1,31.3))*158.5453123);
}

vec3 forwardSF( float i, float n) 
{
    const float PI  = 3.141592653589793238;
    const float PHI = 1.618033988749894848;
    float phi = 2.0*PI*fract(i/PHI);
    float zi = 1.0 - (2.0*i+1.0)/n;
    float sinTheta = sqrt( 1.0 - zi*zi);
    return vec3( cos(phi)*sinTheta, sin(phi)*sinTheta, zi);
}

//---------------------------------------------------------------------------

#define ZERO (min(iFrame,0))

const float pi = 3.1415927;

//---------------------------------------------------------------------------

float mapShell( in vec3 p, out vec4 matInfo ) 
{
    
    const float sc = 1.0/1.0;
    p -= vec3(0.05,0.12,-0.09);    

    p *= sc;

    vec3 q = mat3(-0.6333234236, -0.7332753384, 0.2474039592,
                   0.7738444477, -0.6034162289, 0.1924931824,
                   0.0081370606,  0.3133626215, 0.9495986813) * p;

    const float b = 0.1759;
    
    float r = length( q.xy );
    float t = atan( q.y, q.x );
 
    // https://swiftcoder.wordpress.com/2010/06/21/logarithmic-spiral-distance-field/
    float n = (log(r)/b - t)/(2.0*pi);

    float nm = (log(0.11)/b-t)/(2.0*pi);

    n = min(n,nm);
    
    float ni = floor( n );
    
    float r1 = exp( b * (t + 2.0*pi*ni));
    float r2 = r1 * 3.019863;
    
    //-------

    float h1 = q.z + 1.5*r1 - 0.5; float d1 = sqrt((r1-r)*(r1-r)+h1*h1) - r1;
    float h2 = q.z + 1.5*r2 - 0.5; float d2 = sqrt((r2-r)*(r2-r)+h2*h2) - r2;
    
    float d, dx, dy;
    if( d1<d2 ) { d = d1; dx=r1-r; dy=h1; }
    else        { d = d2; dx=r2-r; dy=h2; }


    float di = textureLod( iChannel2, vec2(t+r,0.5), 0. ).x;
    d += 0.002*di;
    
    matInfo = vec4(dx,dy,r/0.4,t/pi);

    vec3 s = q;
    q = q - vec3(0.34,-0.1,0.03);
    q.xy = mat2(0.8,0.6,-0.6,0.8)*q.xy;
    d = smin( d, sdTorus( q, vec2(0.28,0.05) ), 0.06);
    d = smax( d, -sdEllipsoid(q,vec3(0.0,0.0,0.0),vec3(0.24,0.36,0.24) ), 0.03 );

    d = smax( d, -sdEllipsoid(s,vec3(0.52,-0.0,0.0),vec3(0.42,0.23,0.5) ), 0.05 );
    
    return d/sc;
}

vec2 mapSnail( vec3 p, out vec4 matInfo )
{
    vec3 head = vec3(-0.76,0.6,-0.3);
    
    vec3 q = p - head;

    // body
#if 1
    vec4 b1 = sdBezier( vec3(-0.13,-0.65,0.0), vec3(0.24,0.9+0.1,0.0), head+vec3(0.04,0.01,0.0), p );
    float d1 = b1.x;
    d1 -= smoothstep(0.0,0.2,b1.y)*(0.16 - 0.07*smoothstep(0.5,1.0,b1.y));
    b1 = sdBezier( vec3(-0.085,0.0,0.0), vec3(-0.1,0.9-0.05,0.0), head+vec3(0.06,-0.08,0.0), p );
    float d2 = b1.x;
    d2 -= 0.1 - 0.06*b1.y;
    d1 = smin( d1, d2, 0.03 );
    matInfo.xyz = b1.yzw;
#else
    vec4 b1 = sdBezier( vec3(-0.13,-0.65,0.0), vec3(0.24,0.9+0.11,0.0), head+vec3(0.05,0.01-0.02,0.0), p );
    float d1 = b1.x;
    d1 -= smoothstep(0.0,0.2,b1.y)*(0.16 - 0.75*0.07*smoothstep(0.5,1.0,b1.y));
    matInfo.xyz = b1.yzw;
    float d2;
#endif
    d2 = sdSphere( q, vec4(0.0,-0.06,0.0,0.085) );
    d1 = smin( d1, d2, 0.03 );
    
    d1 = smin( d1, sdSphere(p,vec4(0.05,0.52,0.0,0.13)), 0.07 );
    
    q.xz = mat2(0.8,0.6,-0.6,0.8)*q.xz;

    vec3 sq = vec3( q.xy, abs(q.z) );
    
    // top antenas
    vec3 af = 0.05*sin(0.5*iTime+vec3(0.0,1.0,3.0) + vec3(2.0,1.0,0.0)*sign(q.z) );
    vec4 b2 = sdBezier( vec3(0.0), vec3(-0.1,0.2,0.2), vec3(-0.3,0.2,0.3)+af, sq );
    float d3 = b2.x;
    d3 -= 0.03 - 0.025*b2.y;
    d1 = smin( d1, d3, 0.04 );
    d3 = sdSphere( sq, vec4(-0.3,0.2,0.3,0.016) + vec4(af,0.0) );
    d1 = smin( d1, d3, 0.01 );    
    
    // bottom antenas
    vec3 bf = 0.02*sin(0.3*iTime+vec3(4.0,1.0,2.0) + vec3(3.0,0.0,1.0)*sign(q.z) );
    vec2 b3 = udSegment( sq, vec3(0.06,-0.05,0.0), vec3(-0.04,-0.2,0.18)+bf );
    d3 = b3.x;
    d3 -= 0.025 - 0.02*b3.y;
    d1 = smin( d1, d3, 0.06 );
    d3 = sdSphere( sq, vec4(-0.04,-0.2,0.18,0.008)+vec4(bf,0.0) );
    d1 = smin( d1, d3, 0.02 );
    
    // bottom
    vec3 pp = p-vec3(-0.17,0.15,0.0);
    float co = 0.988771078;
    float si = 0.149438132;
    pp.xy = mat2(co,-si,si,co)*pp.xy;
    d1 = smin( d1, sdEllipsoid( pp, vec3(0.0,0.0,0.0), vec3(0.084,0.3,0.15) ), 0.05 );
    d1 = smax( d1, -sdEllipsoid( pp, vec3(-0.08,-0.0,0.0), vec3(0.06,0.55,0.1) ), 0.02 );
    
    // disp
    float dis = textureLod( iChannel1, 5.0*p.xy, 0. ).x;
    float dx = 0.5 + 0.5*(1.0-smoothstep(0.5,1.0,b1.y));
    d1 -= 0.005*dis*dx*0.5;
        
    return vec2(d1,1.0);
}
    
float mapDrop( in vec3 p )
{
    p -= vec3(-0.26,0.25,-0.02);
    p.x -= 2.5*p.y*p.y;
    return sdCapsule( p, vec3(0.0,-0.06,0.0), vec3(0.014,0.06,0.0), 0.037 );
}

float mapLeaf( in vec3 p )
{
    p -= vec3(-1.8,0.6,-0.75);
    
    p = mat3(0.671212, 0.366685, -0.644218,
            -0.479426, 0.877583,  0.000000,
             0.565354, 0.308854,  0.764842)*p;
 
    p.y += 0.2*exp(-abs(2.0*p.z) );
    
    
    float ph = 0.25*50.0*p.x - 0.25*75.0*abs(p.z);// + 1.0*sin(5.0*p.x)*sin(5.0*p.z);
    float rr = sin( ph );
    rr = rr*rr;    
    rr = rr*rr;    
    p.y += 0.005*rr;
    
    float r = clamp((p.x+2.0)/4.0,0.0,1.0);
    r = 0.0001 + r*(1.0-r)*(1.0-r)*6.0;
    
    rr = sin( ph*2.0 );
    rr = rr*rr;    
    rr *= 0.5+0.5*sin( p.x*12.0 );

    float ri = 0.035*rr;
    
    float d = sdEllipsoid( p, vec3(0.0), vec3(2.0,0.25*r,r+ri) );

    float d2 = p.y-0.02;
    d = smax( d, -d2, 0.02 );
    
    return d;
}

vec2 mapOpaque( vec3 p, out vec4 matInfo )
{
    matInfo = vec4(0.0);
    
   	//--------------
    vec2 res = mapSnail( p, matInfo );
    
    //---------------
    vec4 tmpMatInfo;
    float d4 = mapShell( p, tmpMatInfo );    
    if( d4<res.x  ) { res = vec2(d4,2.0); matInfo = tmpMatInfo; }

    //---------------
    
    // plant
    vec4 b3 = sdBezier( vec3(-0.15,-1.5,0.0), vec3(-0.1,0.5,0.0), vec3(-0.6,1.5,0.0), p );
    d4 = b3.x;
    d4 -= 0.04 - 0.02*b3.y;
    if( d4<res.x  ) { res = vec2(d4,3.0); }
	
	//----------------------------
    
    float d5 = mapLeaf( p );
    if( d5<res.x ) res = vec2(d5,4.0);
        
    return res;
}

// http://iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
vec3 calcNormalOpaque( in vec3 pos, in float eps )
{
    vec4 kk;
#if 0
    vec2 e = vec2(1.0,-1.0)*0.5773*eps;
    return normalize( e.xyy*mapOpaque( pos + e.xyy, kk ).x + 
					  e.yyx*mapOpaque( pos + e.yyx, kk ).x + 
					  e.yxy*mapOpaque( pos + e.yxy, kk ).x + 
					  e.xxx*mapOpaque( pos + e.xxx, kk ).x );
#else
    // inspired by tdhooper and klems - a way to prevent the compiler from inlining map() 4 times
    vec3 n = vec3(0.0);
    for( int i=ZERO; i<4; i++ )
    {
        vec3 e = 0.5773*(2.0*vec3((((i+3)>>1)&1),((i>>1)&1),(i&1))-1.0);
        n += e*mapOpaque(pos+eps*e,kk).x;
    }
    return normalize(n);
#endif    
}

//=========================================================================

float mapLeafWaterDrops( in vec3 p )
{
    p -= vec3(-1.8,0.6,-0.75);
    vec3 s = p;
    p = mat3(0.671212, 0.366685, -0.644218,
            -0.479426, 0.877583,  0.000000,
             0.565354, 0.308854,  0.764842)*p;
  
    vec3 q = p;
    p.y += 0.2*exp(-abs(2.0*p.z) );
    
    //---------------
    
    float r = clamp((p.x+2.0)/4.0,0.0,1.0);
    r = r*(1.0-r)*(1.0-r)*6.0;
    float d0 = sdEllipsoid( p, vec3(0.0), vec3(2.0,0.25*r,r) );
    float d1 = sdEllipsoid( q, vec3(0.5,0.0,0.2), 1.0*vec3(0.15,0.13,0.15) );
    float d2 = sdEllipsoid( q, vec3(0.8,-0.07,-0.15), 0.5*vec3(0.15,0.13,0.15) );
    float d3 = sdEllipsoid( s, vec3(0.76,-0.8,0.6), 0.5*vec3(0.15,0.2,0.15) );
    float d4 = sdEllipsoid( q, vec3(-0.5,0.09,-0.2), vec3(0.04,0.03,0.04) );

    d3 = max( d3, p.y-0.01);
    
    return min( min(d1,d4), min(d2,d3) );
}

vec2 mapTransparent( vec3 p, out vec4 matInfo )
{
    matInfo = vec4(0.0);
    
    float d5 = mapDrop( p );
    vec2  res = vec2(d5,4.0);

    float d6 = mapLeafWaterDrops( p );
    res.x = min( res.x, d6 );

    return res;
}

// http://iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
vec3 calcNormalTransparent( in vec3 pos, in float eps )
{
    vec4 kk;
    vec2 e = vec2(1.0,-1.0)*0.5773*eps;
    return normalize( e.xyy*mapTransparent( pos + e.xyy, kk ).x + 
					  e.yyx*mapTransparent( pos + e.yyx, kk ).x + 
					  e.yxy*mapTransparent( pos + e.yxy, kk ).x + 
					  e.xxx*mapTransparent( pos + e.xxx, kk ).x );
}

//=========================================================================

float calcAO( in vec3 pos, in vec3 nor )
{
    vec4 kk;
	float ao = 0.0;
    for( int i=ZERO; i<32; i++ )
    {
        vec3 ap = forwardSF( float(i), 32.0 );
        float h = hash1(float(i));
		ap *= sign( dot(ap,nor) ) * h*0.1;
        ao += clamp( mapOpaque( pos + nor*0.01 + ap, kk ).x*3.0, 0.0, 1.0 );
    }
	ao /= 32.0;
	
    return clamp( ao*6.0, 0.0, 1.0 );
}

float calcSSS( in vec3 pos, in vec3 nor )
{
    vec4 kk;
	float occ = 0.0;
    for( int i=ZERO; i<8; i++ )
    {
        float h = 0.002 + 0.11*float(i)/7.0;
        vec3 dir = normalize( sin( float(i)*13.0 + vec3(0.0,2.1,4.2) ) );
        dir *= sign(dot(dir,nor));
        occ += (h-mapOpaque(pos-h*dir, kk).x);
    }
    occ = clamp( 1.0 - 11.0*occ/8.0, 0.0, 1.0 );    
    return occ*occ;
}

// http://iquilezles.org/www/articles/rmshadows/rmshadows.htm
float calcSoftShadow( in vec3 ro, in vec3 rd, float k )
{
    vec4 kk;    
    float res = 1.0;
    float t = 0.01;
    for( int i=ZERO; i<32; i++ )
    {
        float h = mapOpaque(ro + rd*t, kk ).x;
        res = min( res, smoothstep(0.0,1.0,k*h/t) );
        t += clamp( h, 0.04, 0.1 );
		if( res<0.01 ) break;
    }
    return clamp(res,0.0,1.0);
}

const vec3 sunDir = normalize( vec3(0.2,0.1,0.02) );
)",
R"(
vec3 shadeOpaque( in vec3 ro, in vec3 rd, in float t, in float m, in vec4 matInfo )
{
    float eps = 0.002;
    
    vec3 pos = ro + t*rd;
    vec3 nor = calcNormalOpaque( pos, eps );

    vec3 mateD = vec3(0.0);
    vec3 mateS = vec3(0.0);
    vec2 mateK = vec2(0.0);
    vec3 mateE = vec3(0.0);

    float focc = 1.0;
    float fsha = 1.0;

    if( m<1.5 ) // snail body
    {
        float dis = texture( iChannel1, 5.0*pos.xy ).x;

        float be = sdEllipsoid( pos, vec3(-0.3,-0.5,-0.1), vec3(0.2,1.0,0.5) );
        be = 1.0-smoothstep( -0.01, 0.01, be );        
        
        float ff = abs(matInfo.x-0.20);
        
        mateS = 6.0*mix( 0.7*vec3(2.0,1.2,0.2), vec3(2.5,1.8,0.9), ff );
        mateS += 2.0*dis;
        mateS *= 1.5;
        mateS *= 1.0 + 0.5*ff*ff;
        mateS *= 1.0-0.5*be;
        
        mateD = vec3(1.0,0.8,0.4);
        mateD *= dis;
        mateD *= 0.015;
        mateD += vec3(0.8,0.4,0.3)*0.15*be;
        
        mateK = vec2( 60.0, 0.7 + 2.0*dis );
        
        float f = clamp( dot( -rd, nor ), 0.0, 1.0 );
        f = 1.0-pow( f, 8.0 );
        f = 1.0 - (1.0-f)*(1.0-texture( iChannel2, 0.3*pos.xy ).x);
        mateS *= vec3(0.5,0.1,0.0) + f*vec3(0.5,0.9,1.0);
        
        float b = 1.0-smoothstep( 0.25,0.55,abs(pos.y));
        focc = 0.2 + 0.8*smoothstep( 0.0, 0.15, sdSphere(pos,vec4(0.05,0.52,0.0,0.13)) );
    }
	else if( m<2.5 ) // shell
    {
        mateK = vec2(0.0);
        
        float tip = 1.0-smoothstep(0.05,0.4, length(pos-vec3(0.17,0.2,0.35)) );
        mateD = mix( 0.7*vec3(0.2,0.21,0.22), 0.2*vec3(0.15,0.1,0.0), tip );
        
        vec2 uv = vec2( .5*atan(matInfo.x,matInfo.y)/3.1416, 1.5*matInfo.w );
        
        vec3 ral = texture( iChannel1, vec2(2.0*matInfo.w+matInfo.z*0.5,0.5) ).xxx;
        mateD *= 0.25 + 0.75*ral;
        
        float pa = smoothstep(-0.2,0.2, 0.3+sin(2.0+40.0*uv.x + 3.0*sin(11.0*uv.x)) );
        float bar = mix(pa,1.0,smoothstep(0.7,1.0,tip));
        bar *= (matInfo.z<0.6) ? 1.0 : smoothstep( 0.17, 0.21, abs(matInfo.w)  );
        mateD *= vec3(0.06,0.03,0.0)+vec3(0.94,0.97,1.0)*bar;
        
        mateK = vec2( 64.0, 0.2 );
        mateS = 1.5*vec3(1.0,0.65,0.6) * (1.0-tip);//*0.5;
    }
    else if( m<3.5 ) // plant
    {
        mateD = vec3(0.05,0.1,0.0)*0.2;
        mateS = vec3(0.1,0.2,0.02)*25.0;
        mateK = vec2(5.0,1.0);
        
        float fre = clamp(1.0+dot(nor,rd), 0.0, 1.0 );
        mateD += 0.2*fre*vec3(1.0,0.5,0.1);
        
        vec3 te = texture( iChannel2, pos.xy*0.2 ).xyz;
        mateS *= 0.5 + 1.5*te;
        mateE = 0.5*vec3(0.1,0.1,0.03)*(0.2+0.8*te.x);
    }
    else //if( m<4.5 ) // leave
    {
        vec3 p = pos - vec3(-1.8,0.6,-0.75);
        vec3 s = p;
        p = mat3(0.671212, 0.366685, -0.644218,
                -0.479426, 0.877583,  0.000000,
                 0.565354, 0.308854,  0.764842)*p;

        vec3 q = p;
        p.y += 0.2*exp(-abs(2.0*p.z) );

        float v = smoothstep( 0.01, 0.02, abs(p.z));
        
        float rr = sin( 4.0*0.25*50.0*p.x - 4.0*0.25*75.0*abs(p.z) );

        vec3 te = texture( iChannel2, p.xz*0.35 ).xyz;

        float r = clamp((p.x+2.0)/4.0,0.0,1.0);
        r = r*(1.0-r)*(1.0-r)*6.0;
        float ff = length(p.xz/vec2(2.0,r));

        mateD = mix( vec3(0.07,0.1,0.0), vec3(0.05,0.2,0.01)*0.25, v );
        mateD = mix( mateD, vec3(0.16,0.2,0.01)*0.25, ff );
        mateD *= 1.0 + 0.25*te;
        mateD *= 0.8;
        
        mateS = vec3(0.15,0.2,0.02)*0.8;
        mateS *= 1.0 + 0.2*rr;
        mateS *= 0.8;

        mateK = vec2(64.0,0.25);
        
        //---------------------
        
        nor.xz += v*0.15*(-1.0+2.0*texture( iChannel3, 1.0*p.xz ).xy);
        nor = normalize( nor );

        float d1 = sdEllipsoid( q, vec3( 0.5-0.07, 0.0,  0.20), 1.0*vec3(1.4*0.15,0.13,0.15) );
        float d2 = sdEllipsoid( q, vec3( 0.8-0.05,-0.07,-0.15), 0.5*vec3(1.3*0.15,0.13,0.15) );
        float d4 = sdEllipsoid( q, vec3(-0.5-0.07, 0.09,-0.20), 1.0*vec3(1.4*0.04,0.03,0.04) );
        float dd = min(d1,min(d2,d4));
        fsha = 0.05 + 0.95*smoothstep(0.0,0.05,dd);
        
        d1 = abs( sdCircle( q.xz, vec2( 0.5, 0.20), 1.0*0.15 ));
        d2 = abs( sdCircle( q.xz, vec2( 0.8,-0.15), 0.5*0.15 ));
        d4 = abs( sdCircle( q.xz, vec2(-0.5,-0.20), 1.0*0.04 ));
        dd = min(d1,min(d2,d4));
        focc *= 0.55 + 0.45*smoothstep(0.0,0.08,dd);
        
        d1 = distance( q.xz, vec2( 0.5-0.07, 0.20) );
        d2 = distance( q.xz, vec2( 0.8-0.03,-0.15) );
        fsha += (1.0-smoothstep(0.0,0.10,d1))*1.5;
        fsha += (1.0-smoothstep(0.0,0.05,d2))*1.5;    
    }
    
  
    vec3 hal = normalize( sunDir-rd );
    float fre = clamp(1.0+dot(nor,rd), 0.0, 1.0 );
    float occ = calcAO( pos, nor )*focc;
    float sss = calcSSS( pos, nor );
    sss = sss*occ + fre*occ + (0.5+0.5*fre)*pow(abs(matInfo.x-0.2),1.0)*occ;
    
    float dif1 = clamp( dot(nor,sunDir), 0.0, 1.0 );
    float sha = calcSoftShadow( pos, sunDir, 20.0 ); 
    dif1 *= sha*fsha;
    float spe1 = clamp( dot(nor,hal), 0.0, 1.0 );

    float bou = clamp( 0.3-0.7*nor.y, 0.0, 1.0 );

    // illumination
    
    vec3 col = vec3(0.0);
    col += 7.0*vec3(1.7,1.2,0.6)*dif1*2.0;           // sun
    col += 4.0*vec3(0.2,1.2,1.6)*occ*(0.5+0.5*nor.y);    // sky
    col += 1.8*vec3(0.1,2.0,0.1)*bou*occ;                // bounce

    col *= mateD;

    col += 0.4*sss*(vec3(0.15,0.1,0.05)+vec3(0.85,0.9,0.95)*dif1)*(0.05+0.95*occ)*mateS; // sss
    col = pow(col,vec3(0.6,0.8,1.0));
    
    col += vec3(1.0,1.0,1.0)*0.2*pow( spe1, 1.0+mateK.x )*dif1*(0.04+0.96*pow(fre,4.0))*mateK.x*mateK.y;   // sun lobe1
    col += vec3(1.0,1.0,1.0)*0.1*pow( spe1, 1.0+mateK.x/3.0 )*dif1*(0.1+0.9*pow(fre,4.0))*mateK.x*mateK.y; // sun lobe2
	col += 0.1*vec3(1.0,max(1.5-0.7*col.y,0.0),2.0)*occ*occ*smoothstep( 0.0, 0.3, reflect( rd, nor ).y )*mateK.x*mateK.y*(0.04+0.96*pow(fre,5.0)); // sky

    col += mateE;

    return col;        
}

vec3 shadeTransparent( in vec3 ro, in vec3 rd, in float t, in float m, in vec4 matInfo, in vec3 col, in float depth )
{
    vec3 oriCol = col;
    
    float dz = depth - t;
    float ao = clamp(dz*50.0,0.0,1.0);
    vec3  pos = ro + t*rd;
    vec3  nor = calcNormalTransparent( pos, 0.002 );
    float fre = clamp( 1.0 + dot( rd, nor ), 0.0, 1.0 );
    vec3  hal = normalize( sunDir-rd );
    vec3  ref = reflect( -rd, nor );
    float spe1 = clamp( dot(nor,hal), 0.0, 1.0 );
    float spe2 = clamp( dot(ref,sunDir), 0.0, 1.0 );


    float ds = 1.6 - col.y;
    
    col *= mix( vec3(0.0,0.0,0.0), vec3(0.4,0.6,0.4), ao );

    col += ds*1.5*vec3(1.0,0.9,0.8)*pow( spe1, 80.0 );
    col += ds*0.2*vec3(0.9,1.0,1.0)*smoothstep(0.4,0.8,fre);
    col += ds*0.9*vec3(0.6,0.7,1.0)*smoothstep( -0.5, 0.5, -reflect( rd, nor ).y )*smoothstep(0.2,0.4,fre);    
    col += ds*0.5*vec3(1.0,0.9,0.8)*pow( spe2, 80.0 );
    col += ds*0.5*vec3(1.0,0.9,0.8)*pow( spe2, 16.0 );
    col += vec3(0.8,1.0,0.8)*0.5*smoothstep(0.3,0.6,texture( iChannel1, 0.8*nor.xy ).x)*(0.1+0.9*fre*fre);
    
    // hide aliasing a bit
    return mix( col, oriCol, smoothstep(0.6,1.0,fre) ); 
}

//--------------------------------------------

vec2 intersectOpaque( in vec3 ro, in vec3 rd, const float mindist, const float maxdist, out vec4 matInfo )
{
    vec2 res = vec2(-1.0);
    
    float t = mindist;
    for( int i=ZERO; i<64; i++ )
    {
        vec3 p = ro + t*rd;
        vec2 h = mapOpaque( p, matInfo );
        res = vec2(t,h.y);

        if( h.x<(0.001*t) ||  t>maxdist ) break;
        
        t += h.x*0.9;
    }
	return res;
}

vec2 intersectTransparent( in vec3 ro, in vec3 rd, const float mindist, const float maxdist, out vec4 matInfo )
{
    vec2 res = vec2(-1.0);
    
    float t = mindist;
    for( int i=ZERO; i<64; i++ )
    {
        vec3 p = ro + t*rd;
        vec2 h = mapTransparent( p, matInfo );
        res = vec2(t,h.y);

        if( h.x<(0.001*t) ||  t>maxdist ) break;
        
        t += h.x;
    }
	return res;
}

vec3 background( in vec3 d )
{
    // cheap cubemap
    vec3 n = abs(d);
    vec2 uv = (n.x>n.y && n.x>n.z) ? d.yz/d.x: 
              (n.y>n.x && n.y>n.z) ? d.zx/d.y:
                                     d.xy/d.z;
    
    // fancy blur
    vec3  col = vec3( 0.0 );
    for( int i=ZERO; i<200; i++ )
    {
        float h = float(i)/200.0;
        float an = 31.0*6.2831*h;
        vec2  of = vec2( cos(an), sin(an) ) * h;

        vec3 tmp = texture( iChannel2, uv*0.25 + 0.0075*of, 4.0 ).yxz;
        col = smax( col, tmp, 0.5 );
    }
    
    return pow(col,vec3(3.5,3.0,6.0))*0.2;
}


vec3 render( in vec3 ro, in vec3 rd, in vec2 q )
{
    //-----------------------------

    vec3 col = background( rd );
    
    //-----------------------------
    
    float mindist = 1.0;
    float maxdist = 4.0;

    vec4 matInfo;
    vec2 tm = intersectOpaque( ro, rd, mindist, maxdist, matInfo );
    if( tm.y>-0.5 && tm.x < maxdist )
    {
        col = shadeOpaque( ro, rd, tm.x, tm.y, matInfo );
        maxdist = tm.x;
    }

    //-----------------------------
    
    tm = intersectTransparent( ro, rd, mindist, maxdist, matInfo );
    if( tm.y>-0.5 && tm.x < maxdist )
    {
        col = shadeTransparent( ro, rd, tm.x, tm.y, matInfo, col, maxdist );
    }

    //-----------------------------
    
    float sun = clamp(dot(rd,sunDir),0.0,1.0);
    col += 1.0*vec3(1.5,0.8,0.7)*pow(sun,4.0);

    //-----------------------------

    col = pow( col, vec3(0.45) );
    
    col = vec3(1.05,1.0,1.0)*col*(0.7+0.3*col*max(3.0-2.0*col,0.0)) + vec3(0.0,0.0,0.04);

    col *= 0.3 + 0.7*pow(16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y),0.1);

    return clamp( col, 0.0, 1.0 );
}

mat3 setCamera( in vec3 ro, in vec3 rt )
{
    vec3 w = normalize(ro-rt);
    
    float m = sqrt(1.0-w.y*w.y);
    
    return mat3( w.z,     0.0, -w.x, 
                 0.0,     m*m, -w.z*w.y,
                 w.x*m, w.y*m,  w.z*m );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{	

    #if AA<2
    
        vec2  p = (2.0*fragCoord-iResolution.xy)/iResolution.y;
        vec2  q = fragCoord/iResolution.xy;
        float an = 1.87 - 0.04*(1.0-cos(0.5*iTime));

        vec3  ro = vec3(-0.4,0.2,0.0) + 2.2*vec3(cos(an),0.0,sin(an));
        vec3  ta = vec3(-0.6,0.2,0.0);
        mat3  ca = setCamera( ro, ta );
        vec3  rd = normalize( ca * vec3(p,-2.8) );

        vec3 col = render( ro, rd, q );
    
    #else

        vec3 col = vec3(0.0);
        for( int m=ZERO; m<AA; m++ )
        for( int n=ZERO; n<AA; n++ )
        {
            vec2 rr = vec2(float(m),float(n))/float(AA);

            vec2 p = (2.0*(fragCoord+rr)-iResolution.xy)/iResolution.y;
            float an = 1.87 - 0.04*(1.0-cos(0.5*iTime));
            vec2 q = (fragCoord+rr)/iResolution.xy;

            vec3 ro = vec3(-0.4,0.2,0.0) + 2.2*vec3(cos(an),0.0,sin(an));
            vec3 ta = vec3(-0.6,0.2,0.0);
            mat3 ca = setCamera( ro, ta );
            vec3 rd = normalize( ca * vec3(p,-2.8) );

		    col += render( ro, rd, q );
        }    
        col /= float(AA*AA);
    #endif
                

    fragColor = vec4( col, 1.0 );
})"};

const char* bg_shader_fs8=R"()";
const char* bg_shader_fs9=R"()";
const char* bg_shader_fs10=R"()";