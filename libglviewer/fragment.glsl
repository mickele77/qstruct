uniform lowp float outVertexUseTex;
varying lowp vec4 outVertexColor;
varying mediump vec4 outVertexTexCoord;

uniform sampler2D texture;

void main() {
    mediump vec2 flippedVertexCoord = vec2(outVertexTexCoord.x, 1.0 - outVertexTexCoord.y);
    gl_FragColor = mix( outVertexColor, texture2D(texture, flippedVertexCoord.st) * outVertexColor, outVertexUseTex );
    // gl_FragColor = outVertexColor;
}
