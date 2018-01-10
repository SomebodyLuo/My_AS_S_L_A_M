attribute vec4 position;  //顶点坐标值（x,y,z）
attribute vec2 texCoord0;
attribute vec4 normal;
varying vec2 v_texCoord;
varying vec4 v_normal;
uniform mat4 matProjViewModel;
uniform mat4 matNormal;
attribute vec4 aColor;  //顶点的颜色（R,G,B,A）
varying vec4 vColor;   //传递给片段着色器的颜色值,varying声明的变量都是要传递给fragment的
void main()
{
	v_texCoord = texCoord0;
    v_normal = matNormal * normal;
    vColor = aColor;//将顶点颜色值传递给fragment
	gl_Position = matProjViewModel * position;
	gl_PointSize = 10.0;
}
