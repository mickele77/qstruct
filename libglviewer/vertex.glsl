attribute highp vec4 vertexPosition;
attribute lowp vec4 vertexColor;
attribute lowp float vertexUseTex;
attribute mediump vec4 vertexTexCoord;

uniform highp mat4 matrix;

varying lowp vec4 outVertexColor;
// varying lowp float outVertexUseTex;
varying mediump vec4 outVertexTexCoord;

void main() {
    outVertexColor = vertexColor;
    // outVertexUseTex = vertexUseTex;
    outVertexTexCoord = vertexTexCoord;

    gl_PointSize = 8.0;
    gl_Position = matrix * vertexPosition;
}
