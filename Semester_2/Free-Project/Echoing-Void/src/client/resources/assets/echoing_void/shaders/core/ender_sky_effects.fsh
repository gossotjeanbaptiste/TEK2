#version 330

// Ported from Complementary Reimagined r2.3's lib/atmospherics/enderBeams.glsl and enderStars.glsl.
// Shaderpack-only globals (ambientColor, vlFactor, gbufferModelViewInverse, endSkyColor, noisetex)
// don't exist outside a shader pack, so they're replaced with fixed art-directed values and a
// noise texture bundled with the mod. HDR magic constants from the original are rescaled since
// vanilla's pipeline here has no HDR/exposure pass to tame them.

#moj_import <minecraft:dynamictransforms.glsl>
#moj_import <minecraft:globals.glsl>

uniform sampler2D Sampler0;

in vec3 localDir;

out vec4 fragColor;

const vec3 BEAM_PURPLE = vec3(0.55, 0.28, 0.85);
const vec3 BEAM_ORANGE = vec3(1.0, 0.35, 0.05);
const vec3 STAR_COLOR = vec3(0.92, 0.88, 1.0);
const float BEAM_SAMPLE_DISTANCE = 300.0;

float pow2(float x) {
    return x * x;
}

float enderStarHash(vec2 pos) {
    return fract(sin(dot(pos, vec2(12.9898, 4.1414))) * 43758.54953);
}

vec3 drawEnderStars(vec3 dir, float vDotUp) {
    vec3 starCoord = 0.65 * dir / (abs(dir.y) + length(dir.xz) + 1e-4);
    vec2 starCoord2 = starCoord.xz * 0.5;
    if (vDotUp < 0.0) starCoord2 += 100.0;

    // Slow wander of the whole star field: base frequencies (1, 2 cycles/day) scaled up by 15/13
    // to nudge the speed up a little while keeping the same 1:2 ratio between axes.
    const float TAU = 6.2831853;
    const float DRIFT_SPEEDUP = 15.0 / 13.0;
    vec2 drift = vec2(cos(GameTime * TAU * 1.0 * DRIFT_SPEEDUP), sin(GameTime * TAU * 2.0 * DRIFT_SPEEDUP)) * 0.02;
    starCoord2 += drift;

    // Higher grid resolution than the original -> each lit cell covers fewer screen pixels,
    // so stars read as fine pinpoints instead of the larger blobs a coarser grid produces here.
    const float starFactor = 2200.0;
    starCoord2 = floor(starCoord2 * starFactor) / starFactor;

    float raw = enderStarHash(starCoord2);
    raw *= enderStarHash(starCoord2 + 0.1);
    raw *= enderStarHash(starCoord2 + 0.23);
    // The original squares (raw - threshold), which is fine when a bloom pass amplifies the
    // result, but here it crushes every star to a practically invisible fraction (no bloom in
    // vanilla's pipeline). smoothstep keeps lit cells at a clearly visible brightness instead.
    float star = smoothstep(0.55, 0.98, raw);

    // GameTime is the time-of-day fraction (tick-of-day / 24000), not a free-running clock, so it
    // needs a large multiplier to read as a real oscillation; the per-cell phase (from a hash
    // uncorrelated with the visibility threshold above) keeps stars from twinkling in lockstep.
    float phase = enderStarHash(starCoord2 + 0.37) * 6.2831853;
    float twinkle = 0.6 + 0.4 * sin(GameTime * 800.0 + phase);
    star *= twinkle;

    vec3 stars = star * STAR_COLOR * 1.6;
    float vAbs = abs(vDotUp);
    stars *= 0.35 + 0.65 * vAbs;
    return stars;
}

float beamNoise(vec2 planeCoord) {
    float n = texture(Sampler0, planeCoord * 0.175).r;
    n += texture(Sampler0, planeCoord * 0.04375).r * 5.0;
    return n;
}

vec3 drawEnderBeams(vec3 dir, float vDotUp, vec3 cameraPos) {
    const int SAMPLES = 8;
    float vM = 1.0 - vDotUp * vDotUp;
    float vM2 = vM + smoothstep(0.0, 1.0, pow2(pow2(1.0 - abs(vDotUp)))) * 0.2;

    vec4 beams = vec4(0.0);
    float gradientMix = 1.0;
    vec2 worldXZ = dir.xz * BEAM_SAMPLE_DISTANCE + cameraPos.xz;

    for (int i = 0; i < SAMPLES; i++) {
        vec2 planeCoord = worldXZ * (1.0 + float(i) * 6.0 / float(SAMPLES)) * 0.0014;
        float n = beamNoise(planeCoord);
        n = max(0.75 - 1.0 / abs(n - (4.0 + vM * 2.0)), 0.0) * 3.0;

        if (n > 0.0) {
            n *= 0.65;
            float fireNoise = texture(Sampler0, abs(planeCoord * 0.2)).r;
            n *= 0.5 * fireNoise + 0.75;
            n = n * n * 3.0 / float(SAMPLES);
            n *= vM2;

            vec3 beamColor = BEAM_PURPLE + BEAM_ORANGE * pow2(pow2(fireNoise - 0.5));
            beamColor *= gradientMix / float(SAMPLES);

            n *= exp2(-6.0 * float(i) / float(SAMPLES));
            beams += vec4(n * beamColor, n);
        }
        gradientMix += 1.0;
    }

    beams.rgb *= beams.a * beams.a * beams.a * 3.5;
    return sqrt(max(beams.rgb, 0.0)) * 1.4;
}

void main() {
    vec3 dir = normalize(localDir);
    float vDotUp = dir.y;
    vec3 cameraPos = vec3(CameraBlockPos) + CameraOffset;

    vec3 color = drawEnderBeams(dir, vDotUp, cameraPos) + drawEnderStars(dir, vDotUp);

    fragColor = vec4(color, 1.0) * ColorModulator;
}
