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

#define M_PI 3.1415926535897932384626433832795

vec3 weight(float a) {
	vec3 s = max(abs(2.0 * M_PI * vec3(a - 1.5, a - 0.5, a + 0.5)), 1e-5);
	return sin(s) * sin(s / 3.0) / (s * s);
}

vec3 lum(sampler2D tex, vec3 x1, vec3 x2, float y, vec3 y1, vec3 y2)
{
	#define TEX(a) COMPAT_TEXTURE(tex, vec2(a, y)).rgb

	return
		mat3(TEX(x1.r), TEX(x1.g), TEX(x1.b)) * y1 +
		mat3(TEX(x2.r), TEX(x2.g), TEX(x2.b)) * y2;
}

vec3 lanczos(sampler2D tex, vec2 coord) {
	vec2 stp = 1.0 / texSize;
	vec2 uv = coord + stp * 0.5;
	vec2 f = fract(uv / stp);

	vec3 y1 = weight(0.5 - f.x * 0.5);
	vec3 y2 = weight(1.0 - f.x * 0.5);
	vec3 x1 = weight(0.5 - f.y * 0.5);
	vec3 x2 = weight(1.0 - f.y * 0.5);

	const vec3 one = vec3(1.0);
	float xsum = dot(x1, one) + dot(x2, one);
	float ysum = dot(y1, one) + dot(y2, one);
	
	x1 /= xsum;
	x2 /= xsum;
	y1 /= ysum;
	y2 /= ysum;
	
	vec2 pos = (-0.5 - f) * stp + uv;
	
	vec3 px1 = vec3(pos.x - stp.x * 2.0, pos.x, pos.x + stp.x * 2.0);
	vec3 px2 = vec3(pos.x - stp.x, pos.x + stp.x, pos.x + stp.x * 3.0);

	vec3 py1 = vec3(pos.y - stp.y * 2.0, pos.y, pos.y + stp.y * 2.0);
	vec3 py2 = vec3(pos.y - stp.y, pos.y + stp.y, pos.y + stp.y * 3.0);

	#define LUM(a) lum(tex, px1, px2, a, y1, y2)

	return
		LUM(py1.r) * x1.r +
		LUM(py1.g) * x1.g +
		LUM(py1.b) * x1.b +
		LUM(py2.r) * x2.r +
		LUM(py2.g) * x2.g +
		LUM(py2.b) * x2.b;
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
	vec3 color = lanczos(tex01, fTex);

#ifdef SATHUE
	color = saturate(color);
#endif
#ifdef LEVELS
	color = levels(color);
#endif
	
	FRAG_COLOR = vec4(color, 1.0);
}