#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTex;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 NormalMatrix;
uniform mat4 MVP;

uniform sampler2D TexColor;
uniform sampler2D TexGrey;
uniform float textureOffset;
uniform float orbitDegree;

uniform float heightFactor;
uniform float imageWidth;
uniform float imageHeight;

out Data
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
} data;


out vec3 LightVector;// Vector from Vertex to Light;
out vec3 CameraVector;// Vector from Vertex to Camera;


void main()
{
    // get orbitDegree value, compute new x, y coordinates
    // there won't be height in moon shader
    vec2 textureCoordinate = vec2(VertexTex.x+textureOffset, VertexTex.y);
    data.TexCoord = textureCoordinate;

    data.Normal = normalize(NormalMatrix*vec4(VertexNormal,0.0f)).xyz;
    LightVector = normalize(lightPosition.xyz-VertexPosition);
    CameraVector = normalize(cameraPosition.xyz-VertexPosition);

 
   // set gl_Position variable correctly to give the transformed vertex position


    gl_Position = MVP* vec4(VertexPosition,1.0f); // this is a placeholder. It does not correctly set the position

}