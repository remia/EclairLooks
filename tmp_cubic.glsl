

#ifdef GL_ES
precision mediump float;
#endif

#extension GL_OES_standard_derivatives : enable

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
uniform sampler2D backbuffer;


// http://polycu.be/edit/?h=cpcFVB
// http://www.pouet.net/topic.php?which=11100

// #template shader

// Signed distance to quadratic bezier with parametrization.
// Tom'2017
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License

// Based on:
// Signed Distance to a Quadratic Bezier Curve
// - Adam Simmons (@adamjsimmons) 2015
// https://www.shadertoy.com/view/ltXSDB

// Do we care about endpoints? 0 = no
#define ENDPOINTS 1

// Solve cubic equation for roots
vec3 solveCubic(float a, float b, float c)
{
   float p = b - a*a / 3.0, p3 = p*p*p;
   float q = a * (2.0*a*a - 9.0*b) / 27.0 + c;
   float d = q*q + 4.0*p3 / 27.0;
   float offset = -a / 3.0;
   if(d >= 0.0) {
      float z = sqrt(d);
      vec2 x = (vec2(z, -z) - q) / 2.0;
      vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));
      return vec3(offset + uv.x + uv.y);
   }
   float v = acos(-sqrt(-27.0 / p3) * q / 2.0) / 3.0;
   float m = cos(v), n = sin(v)*1.732050808;
   return vec3(m + m, -n - m, n - m) * sqrt(-p / 3.0) + offset;
}

// Signed distance to quadratic bezier with parametrization.
// Tom'2017
// returns vec4(
//  unsigned distance to clamped curve,
//  signed distance to extended curve,
//  extended t )
vec3 sdBezier(vec2 A, vec2 B, vec2 C, vec2 p)
{
   B = mix(B + vec2(1e-4), B, abs(sign(B * 2.0 - A - C)));
   vec2 a = B - A, b = A - B * 2.0 + C, c = a * 2.0, d = A - p;
   vec3 k = vec3(3.*dot(a,b),2.*dot(a,a)+dot(d,b),dot(d,a)) / dot(b,b);

   vec2 t = clamp(solveCubic(k.x, k.y, k.z).xy, 0.0, 1.0);
   vec2 dp1 = d + (c + b*t.x)*t.x;
   float d1 = dot(dp1, dp1);
   vec2 dp2 = d + (c + b*t.y)*t.y;
   float d2 = dot(dp2, dp2);

   // note: 3rd root is actually never closest, we can just ignore it

#if ENDPOINTS == 1

   // Find closest distance and t
   vec2 r = (d1 < d2) ? vec2(d1, t.x) : vec2(d2, t.y);

   // Find on which side (t=0 or t=1) is extension
   vec2 e = vec2(step(0.,-r.y),step(1.,r.y));

   // Calc. gradient
   vec2 g = 2.*b*r.y + c;

   // Calc. extension to t
   float et = (e.x*dot(-d,g) + e.y*dot(p-C,g))/dot(g,g);

   // Find closest point on curve with extension
   vec2 dp = d + (c + b*r.y)*r.y + et*g;

   // Sign is just cross product with gradient
   float s =  sign(g.x*dp.y - g.y*dp.x);

   return vec3(sqrt(r.x), s*length(dp), r.y + et);

#else

   // If we don't care about endpoint extension, it's simpler.

   vec4 r = (d1 < d2) ? vec4(d1, t.x, dp1) : vec4(d2, t.y, dp2);

   // Sign is just cross product with gradient
   vec2 g = 2.*b*r.y + c;
   float s =  sign(g.x*r.w - g.y*r.z);

   float dist = sqrt(r.x);
   return vec3(dist, s*dist, r.y);

#endif
}


void main( void ) {

   const float sc = 2.;
   vec2 p = (2.0*gl_FragCoord.xy-resolution.xy)*sc/resolution.y;
//   vec2 m = mix((2.0*mouse.xy-resolution.xy)*sc/resolution.y,
//      vec2(cos(time * 1.2) * 0.8, -0.6), step(mouse.z, 0.0)); wtf is mouse.z
  float t = time * 3.;
  mat2 rot2d = mat2(cos(t), sin(t), -sin(t), cos(t));
  vec2 m = (mouse * sin(time) - resolution / resolution.x) * rot2d;

   // Define the control points of our curve
   vec2 A = vec2(0.0, -1.2), C = vec2(0.0, 0.0), B = (4.0 * m - A - C) / 2.0;
   vec2 D = (2.0 * C - B), E = vec2(1.0, 1.4);

   // Render the control points
   float d = distance(p, m);
   if (d < 0.06) { gl_FragColor = vec4(1.0 - smoothstep(0.045, 0.054, d), 0,0,1); return; }
   d = min(distance(p, B),distance(p, C));
   d = min(distance(p, A),d);
   d = min(distance(p, D),d);
   d = min(distance(p, E),d);
   if (d < 0.04) { gl_FragColor = vec4(1.0 - smoothstep(0.025, 0.034, d)); return; }

   // Get the signed distance with parametrization to closest bezier curve
   // If second one is closer, add 1 to "t"
   vec3 r1 = sdBezier(A, B, C, p);
   vec3 r2 = sdBezier(C, D, E, p);
   vec3 r = (r1.x < r2.x) ? r1 : vec3(r2.xy,r2.z+1.);
   vec2 uv = r.yz;

   // Visualize the distance field
   gl_FragColor = // texture2D(backbuffer, uv*3.)*(smoothstep(.31,.3,r.x)*.7+.3)
	   texture2D(backbuffer, (((gl_FragCoord.xy - resolution.xy) / resolution.xy)) * .9 + 0.95) * .9 // *(smoothstep(.31,.3,r.x)*.7+.3)
	     + vec4((smoothstep(.11,.1,r.x)*.7+.3)) * .1;
   // gl_FragColor = vec4((smoothstep(.11,.1,r.x)*.7+.3));
}


