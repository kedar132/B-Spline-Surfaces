//
//  main.cpp
//  dynamic model
//
//  Created by Hindupur Kedar on 5/17/15.
//  Copyright (c) 2015 Hindupur Kedar. All rights reserved.
//

//
//  main.cpp
//  hamma
//
//  Created by Hindupur Kedar on 11/19/14.
//  Copyright (c) 2014 Hindupur Kedar. All rights reserved.
//

#include <OpenGL/gl.h>//
#include <OpenGL/glu.h>//
#include <GLUT/glut.h>
#include<math.h>
#include<stdio.h>
#include <stdlib.h>

#define PI 2*acos(0)
#define MAX_CPTS  25
#define MAX_KNOT_VALUES MAX_CPTS+5
#define MAX_B_POINTS MAX_KNOT_VALUES*100
using namespace std;

int width = 500, height = 500;

struct point{
    GLfloat x,y,z;
};


struct triangle{
    point vector1, vector2, vector3;
    point normal;
};

int gridOn = 1;
int ctrlPointOn = 1;
int selectPointOn = -1;
int B_SplineCurveOn = 1;
int ctrlPolygonOn = 1;
int movePointOn = -1;
int BsplineOn = 1;
int wireframeOn = -1;
int textureMapOn = -1;
int shadeOn = -1;
int deletePointOn = -1;
GLfloat cpts[MAX_CPTS][3];
int ncpts = 0;
int numBpts;
int cpolygonOn = 1;
float B_points[MAX_B_POINTS][2];
float	knot[MAX_KNOT_VALUES];

int numCurves = 0;
point triangleMesh[MAX_B_POINTS][20];

int selectedPoint = -1;
float curPos[3];
float lastPos[3] = {0.0F, 0.0F, 0.0F};



GLfloat mat_specular1[] ={0.72941, 0.27453, 0.18953, 1.0};
GLfloat mat_ambient1[] ={0.34654, 0.29863, 0.11251, 1.0};
GLfloat mat_diffuse1[] ={0.89435, 0.78653, 0.37654, 1.0};
GLfloat mat_shininess1[] ={128.0 * 0.4};

GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_diffuse[] = {1.0, 0.0, 0.0, 1.0};
GLfloat mat_ambient[] = {0.0, 1.0, 0.0, 1.0};
GLfloat mat_shininess[]= {100.0};



void eventhandle(int key);
void materialparameters(GLfloat spec[], GLfloat amb[], GLfloat diff[], GLfloat shin[]);

void drawGrid()
{
    int i;
    float x,  y;
    
    glBegin(GL_LINES);
    
    glColor3f(0.2, 0.2, 0.2);
    for (i=0; i<9; i++) {
        y = -10.0 + i*2.5;
        glVertex3f(-9.9, y, 0.0);
        glVertex3f(9.9, y, 0.0);
    }
    
    for (i=0; i<9; i++) {
        x = -10.0 + i*2.5;
        glVertex3f(x, -9.9,  0.0);
        glVertex3f(x, 9.9,  0.0);
    }
    
    glEnd();
}

void drawCtrlPolygon()
{
    int i;
    glColor3f(0.0, 1.0, 0.5);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i < ncpts; i++)
        glVertex3fv(cpts[i]);
    glEnd();
}


/* This function implements the Cox deBoor algorithm.  */
float CoxdeBoor(int i, int p, float t)
{
    float	left, right;
    
    if (p==1) {
        if (knot[i] < knot[i+1] && knot[i] <= t && t < knot[i+1])
            return( 1.0 );
        else
            return( 0.0 );
    }
    else {
        if (knot[i+p-1] - knot[i] != 0.0)
            left = CoxdeBoor(i, p-1, t)*(t - knot[i])/
            (knot[i+p-1] - knot[i]);
        else
            left = 0.0;
        
        if (knot[i+p] - knot[i+1] != 0.0)
            right = CoxdeBoor(i+1, p-1, t)*(knot[i+p] - t)/
            (knot[i+p] - knot[i+1]);
        else
            right = 0.0;
        
        return( left + right );
    }
}



void drawBsplineCurve()
{
    int		i;
    int		m = ncpts - 1;
    int		num_knots;
    float	t, B0, B1, B2, B3, x, y;
    
    
    // Compute the knot vector
    for (i=0; i<=3; i++)
        knot[i] = 0.0;
    
    for (i=4; i<=m; i++)
        knot[i] = i - 3.0;
    
    for (i=m+1;  i<=m+4; i++)
        knot[i] = m - 2.0;
    
    num_knots = m+4;
    printf("num_knots %d\n", num_knots);
    printf("knots: ");
    for (i=0;  i<=m+4; i++)
        printf("%.1f ", knot[i]);
    
    printf("\n");
    
    numBpts = -1;
    
    // Compute the store the points along the B-spline curve
    
    for (i=3; i < num_knots-3; i++) {
        for (t = knot[i]; t < knot[i+1]; t += 0.2) {
            B0 = CoxdeBoor(i, 4, t);
            B1 = CoxdeBoor(i-1, 4, t);
            B2 = CoxdeBoor(i-2, 4, t);
            B3 = CoxdeBoor(i-3, 4, t);
            
            x = cpts[i][0] * B0 +
            cpts[i-1][0] * B1 +
            cpts[i-2][0] * B2 +
            cpts[i-3][0] * B3;
            
            y = cpts[i][1] * B0 +
            cpts[i-1][1] * B1 +
            cpts[i-2][1] * B2 +
            cpts[i-3][1] * B3;
            
            numBpts++;
            B_points[ numBpts][0] = x;
            B_points[ numBpts][1] = y;
        }
    }
    
    // Store the last point of the B-spline curve
    numBpts++;
    B_points[numBpts][0] = cpts[ncpts-1][0];
    B_points[numBpts][1] = cpts[ncpts-1][1];
    
    glColor3f(1.0, 0.4, 0.4);
    
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= numBpts; i++) {
        glVertex3f(B_points[i][0],B_points[i][1], 0.0 );
    }
    glEnd();
}

// drawing a bsplinesurface

void bsplinesurface(){
    float theta = 0, a,b,c;
    numCurves = 0;
    
    while(theta < 360){
        glColor3f(1, 0, 1);
        glBegin(GL_LINE_STRIP);
        
        for (int i = 0; i <= numBpts; i++) {
            a = B_points[i][0] * cos(theta*PI/180);
            b = B_points[i][1];
            c = -B_points[i][0] * sin(theta*PI/180);
            glVertex3f(a, b, c);
            
            triangleMesh[i][numCurves].x = a;
            triangleMesh[i][numCurves].y = b;
            triangleMesh[i][numCurves].z = c;
        }
        glEnd();
        theta += 20;
        numCurves++;
    }
}

point computenormal(point vector1, point vector2, point vector3){
    float ax, ay, az, bx, by, bz, norm;
    point normal;
    
    ax = vector2.x - vector1.x;
    ay = vector2.y - vector1.y;
    az = vector2.z - vector1.z;
    bx = vector3.x - vector1.x;
    by = vector3.y - vector1.y;
    bz = vector3.z - vector1.z;
    
    normal.x = ay*bz - by*az;
    normal.y = bx*az - ax*bz;
    normal.z = ax*by - bx*ay;
    norm = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    
    if (norm != 0.0) {
        normal.x = normal.x/norm;
        normal.y = normal.y/norm;
        normal.z = normal.z/norm;
    }
    return normal;
}

static GLfloat theta[] = {0.0,0.0,0.0};
void draw_style_triangle(){
    int i,j;
    
    
    materialparameters(mat_specular, mat_diffuse, mat_ambient, mat_shininess);
    
    GLfloat texInd_x = 0.0, texInd_y=0.0, diff_x, diff_y;
    diff_x = 1.0 / numCurves;
    diff_y = 1.0 / numBpts;
    point normal;
    
    
    for(j=0; j<numCurves; j++){
        for(i=0; i<numBpts; i++){
            
            normal = computenormal(triangleMesh[i][j], triangleMesh[i+1][j],triangleMesh[i][(j+1)%numCurves]);
            if(shadeOn == -1 )glBegin(GL_LINE_STRIP);
            else glBegin(GL_TRIANGLES);
            
            glNormal3f(normal.x, normal.y, normal.z);
            glTexCoord2f(texInd_x, texInd_y);
            glVertex3f(triangleMesh[i][j].x, triangleMesh[i][j].y, triangleMesh[i][j].z);
            
            glNormal3f(normal.x, normal.y, normal.z);
            glTexCoord2f(texInd_x, texInd_y + diff_y);
            glVertex3f(triangleMesh[i+1][j].x, triangleMesh[i+1][j].y, triangleMesh[i+1][j].z);
            
            glNormal3f(normal.x, normal.y, normal.z);
            glTexCoord2f(texInd_x + diff_x, texInd_y);
            glVertex3f(triangleMesh[i][(j+1)%numCurves].x, triangleMesh[i][(j+1)%numCurves].y, triangleMesh[i][(j+1)%numCurves].z);
            glEnd();
            
            
            normal = computenormal(triangleMesh[i][(j+1)%numCurves], triangleMesh[i+1][j],triangleMesh[i+1][(j+1)%numCurves]);
            
            if(shadeOn == -1 )glBegin(GL_LINE_STRIP);
            else glBegin(GL_TRIANGLES);
            
            glNormal3f(normal.x, normal.y, normal.z);
            
            glVertex3f(triangleMesh[i][(j+1)%numCurves].x, triangleMesh[i][(j+1)%numCurves].y, triangleMesh[i][(j+1)%numCurves].z);
            
            glNormal3f(normal.x, normal.y, normal.z);
            
            glVertex3f(triangleMesh[i+1][j].x, triangleMesh[i+1][j].y, triangleMesh[i+1][j].z);
            
            glNormal3f(normal.x, normal.y, normal.z);
            glTexCoord2f(texInd_x + diff_x, texInd_y + diff_y);
            glVertex3f(triangleMesh[i+1][(j+1)%numCurves].x, triangleMesh[i+1][(j+1)%numCurves].y, triangleMesh[i+1][(j+1)%numCurves].z);
            glEnd();
            
            texInd_y += diff_y;
            
        }
        texInd_y= 0.0;
        texInd_x += diff_x;
    }
    
}




void mouseMotion(int x, int y)
{
    float  dx;
    
    float wx = (20.0 * x) / (float)(width - 1) - 10.0;
    float wy = (20.0 * (height - 1 - y)) / (float)(height - 1) - 10.0;
    lastPos[0] = curPos[0];
    lastPos[1] = curPos[1];
    lastPos[2] = curPos[2];
    
    curPos[0] = wx;
    curPos[1] = wy;
    curPos[2] = 0;
    
    if(selectPointOn == 1 && movePointOn == 1){
        cpts[selectedPoint][0] += (curPos[0] - lastPos[0]);
        cpts[selectedPoint][1] += (curPos[1] - lastPos[1]);
        cpts[selectedPoint][2] += (curPos[2] - lastPos[2]);
        glutPostRedisplay();
    }
}

static void mouse(int button, int state, int x, int y)
{
    float wx, wy;
    
    /* We are only interested in left clicks */
    
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
        return;
    
    /* Translate back to our coordinate system */
    wx = (20.0 * x) / (float)(width - 1) - 10.0;
    wy = (20.0 * (height - 1 - y)) / (float)(height - 1) - 10.0;
    curPos[0] = wx;
    curPos[1] = wy;
    curPos[2] = 0;
    
    if(selectPointOn == 1){
        for(int i = 0; i < ncpts; i++){
            if(fabs(cpts[i][0] - wx) < 0.5 && fabs(cpts[i][1] - wy) < 0.5){
                selectedPoint = i;
            }
        }
        glutPostRedisplay();
    }
    /* See if we have room for any more control points */
    if (ctrlPointOn == 1 && ncpts != MAX_CPTS ){
        /* Save the point */
        cpts[ncpts][0] = wx;
        cpts[ncpts][1] = wy;
        cpts[ncpts][2] = 0.0;
        ncpts++;
        glutPostRedisplay();
    }
}





void lighting(){
    
    GLfloat lightpos[] = {5, 5, 10, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
    glEnable ( GL_COLOR_MATERIAL ) ;
    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
}

void display(void)
{
    int i;
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    glRotatef(theta[0], 1.0, 0.0, 0.0);
    glRotatef(theta[1], 0.0, 1.0, 0.0);
    glRotatef(theta[2], 0.0, 0.0, 1.0);
    
    
    // Draw the grid if needed
    if (gridOn)
        drawGrid();
    
    // Draw the control polygon if needed
    if (ctrlPolygonOn == 1) {
        //printf("cpolygon %d\n", cpolygonOn);
        drawCtrlPolygon();
    }
    
    // Draw the B-spline curve if needed
    if (B_SplineCurveOn == 1 && ncpts>3 )
        drawBsplineCurve();
    
    if(wireframeOn == 1){
        bsplinesurface();
        draw_style_triangle();
    }
    
    // Draw the control points
    glPointSize(5.0);
    glBegin(GL_POINTS);
    for (i = 0; i < ncpts; i++){
        if(i == selectedPoint) glColor3f(0.0, 0.0, 1.0);
        else glColor3f(0.0, 0.0, 0.0);
        glVertex3fv(cpts[i]);
    }
    glEnd();
    
    glFlush();
    glutSwapBuffers();
}


void materialparameters(GLfloat spec[], GLfloat amb[], GLfloat diff[], GLfloat shin[]){
    glShadeModel(GL_SMOOTH);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, shin);
    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
}


void keyboard(unsigned char key, int x, int y)
{
    
    switch (key)
    {
        case 'q': case 'Q':
            exit(0);
            break;
        case 'c': case 'C':
            ncpts = 0;
            glutPostRedisplay();
            break;
            
    }
}

/* set texture enabled or disabled*/
void parameters(){
    if(textureMapOn == 1){
        glEnable(GL_TEXTURE_2D);
    }
    else {
        glDisable(GL_TEXTURE_2D);
    }
}



// Menus
typedef struct menuEntryStruct {
    char *label;
    int key;
} menuEntryStruct;

static menuEntryStruct mainMenu[] = {
    "CONTROL POINTS ", 		    '0',
    "CONTROL POLYGON", 		    '1',
    "BSPLINE CURVE", 	        '2',
    "SELECTION OF POINT",       'a',
    "DELETION OF POINT",    	'd',
    "MOVING",                   'm',
    "SAVING", 	   	            's',
    "LOADING",                  'l',
    "CLEAR",                	'c',
    "WIREFRAME BSPLINE",   	    'w',
    "SHADED SURFACE",           'p',
    "TEXTURED SURFACE",          't',
    
    "EXIT", 			         27,
};

int mainMenuEntries = sizeof(mainMenu)/sizeof(menuEntryStruct);

void selectMain(int choice)
{
    eventhandle(mainMenu[choice].key);
}

void setMenuEntries(bool init)
{
    int i, sub;
    
    if (init) {
        glutCreateMenu(selectMain);
        for (i=0; i < mainMenuEntries; i++) {
            glutAddMenuEntry(mainMenu[i].label, i);
        }
        glutAttachMenu(GLUT_RIGHT_BUTTON);
    }
}

void eventhandle(int key) {
    switch(key) {
        case '0':
            ctrlPointOn = -ctrlPointOn;
            //selectPoint = -selectPoint;
            break;
        case '1':
            ctrlPolygonOn = -ctrlPolygonOn;
            break;
        case '2':
            B_SplineCurveOn = -B_SplineCurveOn;
            break;
        case 'a':
            selectPointOn = -selectPointOn;
            ctrlPointOn = -1;
            break;
        case 'd':
            deletePointOn = -deletePointOn;
            if(selectPointOn == 1 && selectedPoint > -1){
                for(int i = selectedPoint; i < ncpts - 1; i++){
                    cpts[i][0] = cpts[i+1][0];
                    cpts[i][1] = cpts[i+1][1];
                    cpts[i][2] = cpts[i+1][2];
                }
                selectedPoint = -1;
            }
            break;
        case 'm':
            movePointOn = -movePointOn;
            break;
        case 's':
            //save the control points
            freopen ("bspline.txt","w",stdout);
            for(int i = 0; i < ncpts; i++){
                //     cout<<cpts[i][0]<<" "<<cpts[i][1]<<" "<<cpts[i][2]<< ;
            }
            fclose(stdout);
            break;
        case 'r':
            // retrieve the control points
            freopen ("bspline.txt","r",stdin);
            float a,b,c;
            ncpts = 0;
            //while(cin>>a>>b>>c){
            cpts[ncpts][0] = a;
            cpts[ncpts][1] = b;
            cpts[ncpts][2] = c;
            ncpts++;
            
            fclose(stdin);
            break;
        case 'c':
            ncpts = 0;
            numBpts = 0;
            break;
        case 'w':
            
            wireframeOn = -wireframeOn;
            ctrlPointOn = -1;
            break;
        case 'p':
            
            shadeOn = -shadeOn;
            break;
        case 't':
            
            textureMapOn = - textureMapOn;
            break;
        case 27:
            exit(0);
        default:
            break;
    }
    parameters();
    glutPostRedisplay();
}


/* This routine handles window resizes */
void reshape(int w, int h)
{
    width = w;
    height = h;
    
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);
}



void init(){
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glMatrixMode(GL_MODELVIEW);
    setMenuEntries(true);
    glOrtho(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0);
}

#define BPP 4

GLuint tgaToTexture( char* filename, int* outWidth, int* outHeight ) {
    
    // open the file
    FILE* file = fopen( filename, "rb" );
    if( file == NULL ) {
        fprintf( stderr, "Could not open file %s for reading.\n", filename );
        return 0;
    }
    
    
    fseek( file, 2, SEEK_CUR );
    
    
    unsigned char imageTypeCode;
    fread( &imageTypeCode, 1, 1, file );
    if( imageTypeCode != 2 && imageTypeCode != 3 ) {
        fclose( file );
        fprintf( stderr, "File %s is an unsupported TGA type: %d\n", filename, imageTypeCode );
        return 0;
    }
    
    
    fseek( file, 9, SEEK_CUR );
    
    
    int imageWidth = 0;
    int imageHeight = 0;
    int bitCount = 0;
    fread( &imageWidth, sizeof( short ), 1, file );
    fread( &imageHeight, sizeof( short ), 1, file );
    fread( &bitCount, sizeof( unsigned char ), 1, file );
    unsigned char* bytes = (unsigned char*) malloc( (size_t) (imageWidth * imageHeight * BPP) );
    
    if( bitCount == 32 ) {
        for( int i = 0; i != imageWidth * imageHeight; ++i ) {
            bytes[ i * BPP + 3 ] = fgetc( file );
            bytes[ i * BPP + 2 ] = fgetc( file );
            bytes[ i * BPP + 1 ] = fgetc( file );
            bytes[ i * BPP + 0 ] = fgetc( file );
        }
    } else {
        for( int i = 0; i != imageWidth * imageHeight; ++i ) {
            bytes[ i * BPP + 0 ] = fgetc( file );
            bytes[ i * BPP + 1 ] = fgetc( file );
            bytes[ i * BPP + 2 ] = fgetc( file );
            bytes[ i * BPP + 3 ] = 255;
        }
    }
    
    fclose( file );
    
    
    GLuint tex;
    glGenTextures( 1, &tex );
    
    glBindTexture( GL_TEXTURE_2D, tex );
    gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, imageWidth, imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, bytes );
    
    
    if( outWidth ) {
        *outWidth = imageWidth;
    }
    if( outHeight ) {
        *outHeight = imageHeight;
    }
    printf("File %s (%d x %d) loaded\n", filename, imageWidth, imageHeight);
    return tex;
}


void textureMap(){
    
    
    GLuint txtMap;
    int txtWidth, txtHeight;
    
    
    glEnable(GL_DEPTH_TEST);
    
    txtMap = tgaToTexture("khindupur.tga", &txtWidth, &txtHeight);
    
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
    // Replace the shaded object's surface with the texture pattern
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    
}

int main(int argc, char **argv)
{
    /* Intialize the program */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("B-spline Surface");
    
    /* Register the callbacks */
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    textureMap();
    init();
    lighting();
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glutMainLoop();
}