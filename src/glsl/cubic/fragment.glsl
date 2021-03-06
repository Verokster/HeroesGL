/*
	MIT License

	Copyright (c) 2020 Oleksiy Ryabchun

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

uniform sampler2D tex01;
uniform vec2 texSize;
#if defined(LEV_IN_RGB) || defined(LEV_IN_A)
#define LEV_IN
#endif
#if defined(LEV_GAMMA_RGB) || defined(LEV_GAMMA_A)
#define LEV_GAMMA
#endif
#if defined(LEV_OUT_RGB) || defined(LEV_OUT_A)
#define LEV_OUT
#endif
#if defined(LEV_IN) || defined(LEV_GAMMA) || defined(LEV_OUT)
#define LEVELS
#endif
#if defined(LEV_HUE_L) || defined(LEV_HUE_R)
#define LEV_HUE
#endif
#if defined(LEV_HUE) || defined(LEV_SAT)
#define SATHUE
uniform vec2 satHue;
#endif
#ifdef LEV_IN
uniform vec4 in_left;
uniform vec4 in_right;
#endif
#ifdef LEV_GAMMA
uniform vec4 gamma;
#endif
#ifdef LEV_OUT
uniform vec4 out_left;
uniform vec4 out_right;
#endif

#if __VERSION__ >= 130
	#define COMPAT_IN in
	#define COMPAT_TEXTURE texture
	out vec4 FRAG_COLOR;
#else
	#define COMPAT_IN varying 
	#define COMPAT_TEXTURE texture2D
	#define FRAG_COLOR gl_FragColor
#endif

COMPAT_IN vec2 fTex;

vec3 cubic(sampler2D tex, vec2 coord) {
	const vec4 p0 = vec4(0.0, -0.5,  1.0, -0.5);
	const vec4 p1 = vec4(1.0,  0.0, -2.5,  1.5);
	const vec4 p2 = vec4(0.0,  0.5,  2.0, -1.5);
	const vec4 p3 = vec4(0.0,  0.0, -0.5,  0.5);

	vec2 uv = coord * texSize - 0.5;
	vec2 texel = floor(uv) - 0.5;
	vec2 t = fract(uv);

	vec2 t2 = t * t;
	vec2 t3 = t2 * t;

	vec4 x = vec4(1.0, t.x, t2.x, t3.x);
	x = vec4(dot(x, p0), dot(x, p1), dot(x, p2), dot(x, p3));
	
	vec4 y = vec4(1.0, t.y, t2.y, t3.y);
	y = vec4(dot(y, p0), dot(y, p1), dot(y, p2), dot(y, p3));

	#define TEX(a, b) COMPAT_TEXTURE(tex, (texel + vec2(a, b)) / texSize).rgb

	return
		(TEX(0.0, 0.0) * x.r + TEX(1.0, 0.0) * x.g + TEX(2.0, 0.0) * x.b + TEX(3.0, 0.0) * x.a) * y.r +
		(TEX(0.0, 1.0) * x.r + TEX(1.0, 1.0) * x.g + TEX(2.0, 1.0) * x.b + TEX(3.0, 1.0) * x.a) * y.g +
		(TEX(0.0, 2.0) * x.r + TEX(1.0, 2.0) * x.g + TEX(2.0, 2.0) * x.b + TEX(3.0, 2.0) * x.a) * y.b +
		(TEX(0.0, 3.0) * x.r + TEX(1.0, 3.0) * x.g + TEX(2.0, 3.0) * x.b + TEX(3.0, 3.0) * x.a) * y.a;
}

#ifdef SATHUE
vec3 saturate(vec3 color) {
#ifdef LEV_HUE
#ifdef LEV_HUE_L
	color = color.brg + 0.5 * satHue.y * (color - color.gbr + satHue.y * (color.gbr - 5.0 * color.brg + 4.0 * color + 3.0 * satHue.y * (color.brg - color)));
#else
	color = color + 0.5 * satHue.y * (color.gbr - color.brg + satHue.y * (color.brg - 5.0 * color + 4.0 * color.gbr + 3.0 * satHue.y * (color - color.gbr)));
#endif
#endif
#ifdef LEV_SAT
	float s = dot(color, vec3(1.0)) / 3.0;
	color = (color - s) * satHue.x + s;
#endif
	return color;
}
#endif

#ifdef LEVELS
vec3 levels(vec3 color) {
#ifdef LEV_IN_RGB
	color = clamp((color - in_left.rgb) / (in_right.rgb - in_left.rgb), 0.0, 1.0);
#endif
#ifdef LEV_GAMMA_RGB
	color = pow(color, gamma.rgb);
#endif
#ifdef LEV_OUT_RGB
	color = clamp(color * (out_right.rgb - out_left.rgb) + out_left.rgb, 0.0, 1.0);
#endif
#ifdef LEV_IN_A
	color = clamp((color - in_left.a) / (in_right.a - in_left.a), 0.0, 1.0);
#endif
#ifdef LEV_GAMMA_A
	color = pow(color, gamma.aaa);
#endif
#ifdef LEV_OUT_A
	color = clamp(color * (out_right.a - out_left.a) + out_left.a, 0.0, 1.0);
#endif
	return color;
}
#endif

void main() {
	vec3 color = cubic(tex01, fTex);

#ifdef SATHUE
	color = saturate(color);
#endif
#ifdef LEVELS
	color = levels(color);
#endif

	FRAG_COLOR = vec4(color, 1.0);
} 