#include <GL/freeglut.h>

void display() {
    // Clear the screen with a black background
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw a triangle
    glBegin(GL_TRIANGLES);
        glColor3f(1.0, 0.0, 0.0); // Red
        glVertex2f(-0.5, -0.5);
        
        glColor3f(0.0, 1.0, 0.0); // Green
        glVertex2f(0.5, -0.5);
        
        glColor3f(0.0, 0.0, 1.0); // Blue
        glVertex2f(0.0, 0.5);
    glEnd();

    // Force execution of OpenGL commands
    glFlush();
}

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    
    // Set window size and create it
    glutInitWindowSize(500, 500);
    glutCreateWindow("My First OpenGL Triangle");
    
    // Register the display callback function
    glutDisplayFunc(display);
    
    // Enter the infinite event-processing loop
    glutMainLoop();
    return 0;
}