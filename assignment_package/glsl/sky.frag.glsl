#version 150

uniform mat4 u_ViewProj;    // We're actually passing the inverse of the viewproj
                            // from our CPU, but it's named u_ViewProj so we don't
                            // have to bother rewriting our ShaderProgram class

uniform ivec2 u_Dimensions; // Screen dimensions

uniform vec3 u_Eye; // Camera pos

uniform float u_Time;

out vec4 outColor;

const float PI = 3.14159265359;
const float TWO_PI = 6.28318530718;

// Sunset palette
const vec3 sunset[5] = vec3[](vec3(255, 229, 119) / 255.0,
                               vec3(254, 192, 81) / 255.0,
                               vec3(255, 137, 103) / 255.0,
                               vec3(253, 96, 81) / 255.0,
                               vec3(57, 32, 51) / 255.0);
// Dusk palette
const vec3 dusk[5] = vec3[](vec3(144, 96, 144) / 255.0,
                            vec3(96, 72, 120) / 255.0,
                            vec3(72, 48, 120) / 255.0,
                            vec3(48, 24, 96) / 255.0,
                            vec3(0, 24, 72) / 255.0);

//midday palette
const vec3 midday[5] = vec3[](vec3(208, 221, 246) / 255.0,
                               vec3(51, 181, 255) / 255.0,
                               vec3(1, 163, 255) / 255.0,
                                vec3(8, 97,148) / 255.0,
                                vec3(44, 49, 65) / 255.0);

//full sun color
const vec3 sunColor = vec3(255, 255, 190) / 255.0;
const vec3 cloudColor = sunset[3];

vec2 sphereToUV(vec3 p) {
    float phi = atan(p.z, p.x);
    if(phi < 0) {
        phi += TWO_PI;
    }
    float theta = acos(p.y);
    return vec2(1 - phi / TWO_PI, 1 - theta / PI);
}

//divided into intervals, it gives the right color mix depending on the uv.y
vec3 uvToSunset(vec2 uv) {
    if(uv.y < 0.5) {
        return sunset[0];
    }
    else if(uv.y < 0.55) {
        return mix(sunset[0], sunset[1], (uv.y - 0.5) / 0.05);
    }
    else if(uv.y < 0.6) {
        return mix(sunset[1], sunset[2], (uv.y - 0.55) / 0.05);
    }
    else if(uv.y < 0.65) {
        return mix(sunset[2], sunset[3], (uv.y - 0.6) / 0.05);
    }
    else if(uv.y < 0.75) {
        return mix(sunset[3], sunset[4], (uv.y - 0.65) / 0.1);
    }
    return sunset[4];
}

//divided into intervals, it gives the right color mix depending on the uv.y
vec3 uvToDusk(vec2 uv) {
    if(uv.y < 0.5) {
        return dusk[0];
    }
    else if(uv.y < 0.55) {
        return mix(dusk[0], dusk[1], (uv.y - 0.5) / 0.05);
    }
    else if(uv.y < 0.6) {
        return mix(dusk[1], dusk[2], (uv.y - 0.55) / 0.05);
    }
    else if(uv.y < 0.65) {
        return mix(dusk[2], dusk[3], (uv.y - 0.6) / 0.05);
    }
    else if(uv.y < 0.75) {
        return mix(dusk[3], dusk[4], (uv.y - 0.65) / 0.1);
    }
    return dusk[4];
}

//divided into intervals, it gives the right color mix depending on the uv.y
vec3 uvToMidday(vec2 uv) {
    if(uv.y < 0.5) {
        return midday[0];
    }
    else if(uv.y < 0.55) {
        return mix(midday[0], midday[1], (uv.y - 0.5) / 0.05);
    }
    else if(uv.y < 0.6) {
        return mix(midday[1], midday[2], (uv.y - 0.55) / 0.05);
    }
    else if(uv.y < 0.65) {
        return mix(midday[2], midday[3], (uv.y - 0.6) / 0.05);
    }
    else if(uv.y < 0.75) {
        return mix(midday[3], midday[4], (uv.y - 0.65) / 0.1);
    }
    return midday[4];
}

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

vec3 random3( vec3 p ) {
    return fract(sin(vec3(dot(p,vec3(127.1, 311.7, 191.999)),
                          dot(p,vec3(269.5, 183.3, 765.54)),
                          dot(p, vec3(420.69, 631.2,109.21))))
                 *43758.5453);
}

//make the sun rotate around x axis depending on the u_Time
vec3 rotateX(vec3 p, float a) {
    return vec3(p.x, cos(a) * p.y + -sin(a) *p.z, sin(a) * p.y +cos(a) * p.z);
}

float WorleyNoise3D(vec3 p)
{
    // Tile the space
    vec3 pointInt = floor(p);
    vec3 pointFract = fract(p);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int z = -1; z <= 1; z++)
    {
        for(int y = -1; y <= 1; y++)
        {
            for(int x = -1; x <= 1; x++)
            {
                vec3 neighbor = vec3(float(x), float(y), float(z));

                // Random point inside current neighboring cell
                vec3 point = random3(pointInt + neighbor);

                // Animate the point
                point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

                // Compute the distance b/t the point and the fragment
                // Store the min dist thus far
                vec3 diff = neighbor + point - pointFract;
                float dist = length(diff);
                minDist = min(minDist, dist);
            }
        }
    }
    return minDist;
}

float WorleyNoise(vec2 uv)
{
    // Tile the space
    vec2 uvInt = floor(uv);
    vec2 uvFract = fract(uv);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x <= 1; x++)
        {
            vec2 neighbor = vec2(float(x), float(y));

            // Random point inside current neighboring cell
            vec2 point = random2(uvInt + neighbor);

            // Animate the point
            point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

            // Compute the distance b/t the point and the fragment
            // Store the min dist thus far
            vec2 diff = neighbor + point - uvFract;
            float dist = length(diff);
            minDist = min(minDist, dist);
        }
    }
    return minDist;
}

float worleyFBM(vec3 uv) {
    float sum = 0;
    float freq = 4;
    float amp = 0.5;
    for(int i = 0; i < 8; i++) {
        sum += WorleyNoise3D(uv * freq) * amp;
        freq *= 2;
        amp *= 0.5;
    }
    return sum;
}

//#define RAY_AS_COLOR
//#define SPHERE_UV_AS_COLOR
#define WORLEY_OFFSET

void main()
{
    vec2 ndc = (gl_FragCoord.xy / vec2(u_Dimensions)) * 2.0 - 1.0; // -1 to 1 NDC

    vec4 p = vec4(ndc.xy, 1, 1); // Pixel at the far clip plane
    p *= 1000.0; // Times far clip plane value
    p = /*Inverse of*/ u_ViewProj * p; // Convert from unhomogenized screen to world

    vec3 rayDir = normalize(p.xyz - u_Eye);

    //make an illusion that the quad is spherical shape
    vec2 uv = sphereToUV(rayDir);


    //make offset for noise in the sky
    vec2 offset = vec2(0.0);
#ifdef WORLEY_OFFSET
    // Get a noise value in the range [-1, 1]
    // by using Worley noise as the noise basis of FBM
    offset = vec2(worleyFBM(rayDir));
    offset *= 2.0;
    offset -= vec2(1.0);
#endif

    // Compute a gradient from the bottom of the sky-sphere to the top
    vec3 sunsetColor = uvToSunset(uv + offset * 0.1);
    vec3 duskColor = uvToDusk(uv + offset * 0.1);
    vec3 middayColor = uvToMidday(uv + offset * 0.1);

    vec3 xAxis = vec3(1, 0, 0);
    vec3 yAxis = vec3(0, 1, 0);
    vec3 zAxis = vec3(0, 0, 1);

    // Add a glowing sun in the sky
    vec3 sunDir = normalize(rotateX(normalize(vec3(0, 0, -1.0)), u_Time *0.01));

    float sunSize = 30.0;
    float angle = (acos(dot(rayDir, sunDir)) * 360.0 / PI);

    //find the dot product between sundirection and yaxis
    float floorAngle = dot(sunDir, yAxis);

    //compare the dot product to create color change depending on how far up (y) the sun is
    float floorSunDot  = floorAngle;

#define SUNSET_THRESHOLD 0.0
//#define SUNSET_BETWEEN_MIDDAY 0.6
#define MIDDAY_THRESHOLD 0.5
//#define DUSK_BETWEEN_MIDDAY 0.2
#define DUSK_THRESHOLD -0.5


    //blue sky midday
    if (floorSunDot > MIDDAY_THRESHOLD) {
        if(angle < sunSize) {
            // Full center of sun
            if(angle < 7.5) {
                outColor = vec4(sunColor, 1);
            }
            // Corona of sun, mix with the current sky color
            else {
                outColor = vec4(mix(sunColor, middayColor, (angle - 7.5) / 22.5), 1);
            }
            //if not within sun angle, we set the color to midday

        } else {
            outColor = vec4(middayColor, 1);
        }

        //between sunset and midday
    } else if (floorSunDot > SUNSET_THRESHOLD) {
        if(angle < sunSize) {
            // Full center of sun
            if(angle < 7.5) {
                outColor = vec4(sunColor, 1);
            }
            // Corona of sun, mix with the current sky color
            else {
                float t = (floorSunDot - SUNSET_THRESHOLD) / (MIDDAY_THRESHOLD - SUNSET_THRESHOLD);
                outColor = vec4(mix(sunColor, mix(sunsetColor, middayColor, t), (angle - 7.5) / 22.5), 1);
            }
            //if not within sun angle, we set the color to midday & sunset
        } else {
            float t = (floorSunDot - SUNSET_THRESHOLD) / (MIDDAY_THRESHOLD - SUNSET_THRESHOLD);
            outColor = vec4(mix(sunsetColor, middayColor, t), 1);
        }

        //between dusk and sunset
    } else if (floorSunDot > DUSK_THRESHOLD) {
        if(angle < sunSize) {
            // Full center of sun
            if(angle < 7.5) {
                outColor = vec4(sunColor, 1);
            }
            // Corona of sun, mix with the current sky color
            else {
                float t = (floorSunDot - SUNSET_THRESHOLD) / (DUSK_THRESHOLD - SUNSET_THRESHOLD);
                outColor = vec4(mix(sunColor, mix(sunsetColor, duskColor, t), (angle - 7.5) / 22.5), 1);
            }
            //if not within sun angle, we set the color to sunset & dusk
        } else {
            float t = (floorSunDot - SUNSET_THRESHOLD) / (DUSK_THRESHOLD - SUNSET_THRESHOLD);
            outColor = vec4(mix(sunsetColor, duskColor, t), 1);
        }

        //full night/dusk
    } else {
        //no sun exists when complete night/dusk
        float t = (floorSunDot - SUNSET_THRESHOLD) / (DUSK_THRESHOLD - SUNSET_THRESHOLD);
        outColor = vec4(mix(sunsetColor, duskColor, t), 1);
 //       outColor = vec4(duskColor, 1);
    }
}
