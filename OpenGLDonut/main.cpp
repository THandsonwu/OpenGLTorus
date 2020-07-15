//
//  main.cpp
//  OpenGLDonut
//
//  Created by tanzhiwu on 2020/7/14.
//  Copyright © 2020 tanzhiwu. All rights reserved.
//

//为了演示OpenGL背面剔除，深度测试，和多边形模式,这些库我们上个demo已经注释过是什么用途
#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#pragma mark --- 设置类和属性
////设置角色帧，作为相机
GLFrame             cameraFrame;
//使用GLFrustum类来设置透视投影
GLFrustum           viewFrustum;
GLTriangleBatch     torusBatch;
GLMatrixStack       modelViewMatix;
GLMatrixStack       projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager     shaderManager;

//标记：背面剔除、深度测试
int iCull = 0;
int iDepth = 0;

#pragma mark --- 设置注册函数(类似回调)

void RenderScene()
{
    //1.清除窗口和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    //开启/关闭背面剔除功能
    if (iCull) {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
    
    //根据设置iDepth标记来判断是否开启深度测试
    if (iDepth) {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    
    //2.把摄像机📹矩阵压入模型矩阵中
    modelViewMatix.PushMatrix(cameraFrame);
    
    //3.设置绘图颜色,天天圈的屎黄
    GLfloat vTorus[] = {0.87f, 0.60f, 0.29f, 1.0f};
//    GLfloat vTorus[] = {0.87f, 0.36f, 0.196f, 1.0f};
    
    //4.使用默认光源着色器
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vTorus);
    
    //5.绘制
    torusBatch.Draw();
    
    //6. 出栈,绘制完成恢复原来现状
    modelViewMatix.PopMatrix();
    
    //7.交换缓存区
    glutSwapBuffers();

    
}

void SetupRC()
{
    //1.设置背景颜色
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    
    //2.初始化着色管理器
    shaderManager.InitializeStockShaders();
    
    //3.将相机视角往后移动8个单元:肉眼到物体之间的距离
    cameraFrame.MoveForward(8.0);
    
    //4.来个甜甜圈🍩
    /*
     参数1:GLTriangleBatch 容易帮助类
     参数2:外边缘半径
     参数3:内边缘半径
     参数4、5:主半径和从半径的细分单元数量
     
     void gltMakeTorus(GLTriangleBatch& torusBatch, GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor);
    */
    gltMakeTorus(torusBatch, 1.0f, 0.4f, 56, 28);
    
    //5.设置点的大小(方便点填充时,肉眼观察)
    glPointSize(5.0f);
     
}

void SpecialKeys(int key, int x, int y)
{
    //1.判断方向
    if (key == GLUT_KEY_UP) {
        //2.根据方向调整观察者位置
        cameraFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_DOWN) {
           //2.根据方向调整观察者位置
        cameraFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_LEFT) {
           //2.根据方向调整观察者位置
        cameraFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    }
    if (key == GLUT_KEY_RIGHT) {
           //2.根据方向调整观察者位置
        cameraFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    
    //3.重新刷新
    glutPostRedisplay();
    
}

void changeSize(int w, int h)
{
    
    //1.防止高度清0
    if (h == 0) {
        h = 1;
    }
    
    //2.设置视口窗口尺寸
    glViewport(0, 0, w, h);
    
    //3.setPrespective函数的参数是一个从顶点方向看去的视场角度(用角度值表示)
    //设置透视模式,初始化其透视矩阵
    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);
    
    //4.把透视矩阵加载到透视矩阵堆阵中
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    //5.初始化渲染管线
    transformPipeline.SetMatrixStacks(modelViewMatix, projectionMatrix);
}

void ProcessMenu(int value)
{
    switch (value) {
        case 1:
            iDepth = !iDepth;
            break;
        case 2:
            iCull = !iCull;
            break;
        case 3:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case 4:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);;
            break;
        case 5:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);;
            break;
            
        default:
            break;
    }
    glutPostRedisplay();
}


#pragma mark --- main 函数

int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(900, 600);
    glutCreateWindow("爱的魔力转圈圈");
    glutReshapeFunc(changeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    
    
    //添加右击菜单栏
    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("开启深度测试", 1);
    glutAddMenuEntry("开启 裁剪", 2);
    glutAddMenuEntry("设置填满模式", 3);
    glutAddMenuEntry("设置线模式", 4);
    glutAddMenuEntry("设置点模式", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return -999;
    }
    SetupRC();
    glutMainLoop();
    return 0;
    
    
}
