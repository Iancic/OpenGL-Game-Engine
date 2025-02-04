#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec4 gl_FragCoord;

uniform sampler2D screenTexture;

uniform float iTime;
uniform vec2 iResolution;

vec2 CRTCurveUV( vec2 uv )
{
    uv = uv * 2.0 - 1.0;
    vec2 offset = abs( uv.yx ) / vec2( 6.0, 4.0 );
    uv = uv + uv * offset * offset;
    uv = uv * 0.5 + 0.5;
    return uv;
}

void DrawVignette( inout vec3 color, vec2 uv, float intensity )
{    
    float vignette = uv.x * uv.y * ( 1.0 - uv.x ) * ( 1.0 - uv.y );
    vignette = clamp( pow( 16.0 * vignette, intensity ), 0.0, 1.0 );
    color *= vignette;
}

void DrawScanline( inout vec3 color, vec2 uv )
{
    float scanline 	= clamp( 0.95 + 0.05 * cos( 3.14 * ( uv.y + 0.008 * iTime ) * 240.0 * 1.0 ), 0.0, 1.0 );
    float grille 	= 0.85 + 0.15 * clamp( 1.5 * cos( 3.14 * uv.x * 640.0 * 1.0 ), 0.0, 1.0 );    
    color *= scanline * grille * 1.2;
}


vec2 PincushionDistortion(in vec2 uv, float strength) 
{
	vec2 st = uv - 0.5;
    float uvA = atan(st.x, st.y);
    float uvD = dot(st, st);
    return 0.5 + vec2(sin(uvA), cos(uvA)) * sqrt(uvD) * (1.0 - strength * uvD);
}


vec3 ChromaticAbberation(sampler2D tex, in vec2 uv, float distortionAmount) 
{
	float rChannel = texture(tex, PincushionDistortion(uv, 0.3 * distortionAmount)).r;
    float gChannel = texture(tex, PincushionDistortion(uv, 0.15 * distortionAmount)).g;
    float bChannel = texture(tex, PincushionDistortion(uv, 0.075 * distortionAmount)).b;
    vec3 retColor = vec3(rChannel, gChannel, bChannel);
    return retColor;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec3 render = ChromaticAbberation(screenTexture, uv, 0.15f);

    vec2 crtUV = CRTCurveUV( uv );
    if ( crtUV.x < 0.0 || crtUV.x > 1.0 || crtUV.y < 0.0 || crtUV.y > 1.0 )
    {
        render = vec3( 0.0, 0.0, 0.0 );
    }

    DrawVignette( render, crtUV, 0.2f );
    DrawScanline( render, uv );

	FragColor = vec4(render, 1.0f);
}
