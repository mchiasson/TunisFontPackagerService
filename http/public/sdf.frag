/*
 * MIT License
 *
 * Copyright (c) 2017-2018 Mathieu-Andre Chiasson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if defined(GL_OES_standard_derivatives)
    #extension GL_OES_standard_derivatives : enable
#endif

precision mediump float;

uniform sampler2D u_texture;
uniform vec4 u_color;
uniform float u_outlineThickness;
uniform vec4 u_outlineColor;
uniform vec2 u_shadowOffset;
uniform vec4 u_shadowColor;
uniform float u_debug;
uniform float u_mode;

varying vec2 v_texcoord;

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

float getDistance(in vec2 texcoord)
{
    if (u_mode > 0.5)
    {
        vec3 msdf = texture2D(u_texture, texcoord).rgb;
        return median(msdf.r, msdf.g, msdf.b);
    }
    else
    {
        return texture2D(u_texture, texcoord).a;
    }
}

float calculateSmoothingFactor(in float distance)
{
    // Stefan Gustavson's fwidth
    return (0.707106781 * length(vec2(dFdx(distance), dFdy(distance))));
}

float stepAA(in float threshold, in float smoothing, in float distance)
{
    return smoothstep(threshold - smoothing, threshold + smoothing, distance);
}

void main() {
    if (u_debug > 0.0)
    {
        if(u_mode > 1.5)
        {
            gl_FragColor = texture2D(u_texture, v_texcoord);
        }
        else if(u_mode > 0.5)
        {
            gl_FragColor = vec4(texture2D(u_texture, v_texcoord).rgb, 1);
        }
        else
        {
            gl_FragColor = vec4(texture2D(u_texture, v_texcoord).aaa, 1);
        }
    }
    else
    {
        float distance = getDistance(v_texcoord);
        float smoothing = calculateSmoothingFactor(distance);
        float alphaMask = stepAA(0.5, smoothing, distance);
        vec4 finalColor;

        float outlineThreshold = max(0.1, 0.5 - u_outlineThickness);
        float outlineMask = stepAA(outlineThreshold, smoothing, distance);
        if (outlineThreshold < 0.5)
        {
            finalColor = mix(u_outlineColor, u_color, alphaMask);
            finalColor = vec4(finalColor.rgb, finalColor.a * outlineMask);
        }
        else
        {
            finalColor = mix(u_outlineColor, u_color, outlineMask);
            finalColor = vec4(finalColor.rgb, finalColor.a * alphaMask);
        }

        vec2 shadowTexCoord = v_texcoord - u_shadowOffset;
        float shadowDistance = u_mode > 1.5 ? texture2D(u_texture, shadowTexCoord).a : getDistance(shadowTexCoord);
        float shadowSmoothing = calculateSmoothingFactor(shadowDistance);
        float shadowMask = stepAA(0.0, shadowSmoothing, shadowDistance);
        vec4 shadowColor = vec4(u_shadowColor.rgb, u_shadowColor.a * shadowMask);
        shadowColor.a *= smoothstep(0.1, u_outlineThickness > 0.0 ? 0.75 - u_outlineThickness : 0.75, shadowDistance);
        finalColor = mix(shadowColor, finalColor, finalColor.a);

        gl_FragColor = finalColor;
    }
}
