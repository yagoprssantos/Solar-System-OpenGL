#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <stdio.h>
#include <glut.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define NUM_STARS 1000

// Rotation control variables
float angle = 0.0; // Scene rotation angle
float planetAngle = 0.0f, moonAngle = 0.0f; // Planet and moon rotation angles
const float planetDistance = 3.0f, moonDistance = 0.7f; // Distance from planet to the sun and moon distance to the planet

// Array to store star coordinates
float stars[NUM_STARS][3];

// Variables to store textures
GLuint tex_sun, tex_earth, tex_moon;

// Function to load a texture from a BMP file
GLuint loadTexture(const char* filename)
{
    GLuint texture;          // Texture identifier variable
    GLint width, height;     // Variables to store image width and height
    unsigned char* data;     // Pointer to store image data

    // Open the file in binary mode for reading
    FILE* file;
    file = fopen(filename, "rb");

    if (file == NULL) return 0; // Return 0 if failed to open the file

    // Seek to the location where image width and height information is stored
    fseek(file, 0x0012, SEEK_SET);
    // Read the image width and height (each is a 4-byte integer)
    fread(&width, sizeof(int), 1, file);
    fread(&height, sizeof(int), 1, file);
    // Seek to the start of image data (after the 54-byte header)
    fseek(file, 54, SEEK_SET);

    // Allocate memory to store image data (3 bytes per pixel)
    data = (unsigned char*)malloc(width * height * 3);
    // Read image data into the 'data' pointer
    fread(data, width * height * 3, 1, file);
    // Close the file after reading
    fclose(file);

    // Adjust the order of color components (from BGR to RGB)
    for (int i = 0; i < height * width; ++i)
    {
        int index = i * 3;
        unsigned char B, R;
        B = data[index];
        R = data[index + 2];
        data[index] = R;
        data[index + 2] = B;
    }

    // Generate a texture identifier
    glGenTextures(1, &texture);
    // Bind the newly generated texture for configuration
    glBindTexture(GL_TEXTURE_2D, texture);
    // Set the texture blending mode (GL_MODULATE means blend with lighting)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    // Set the interpolation filter for texture reduction (GL_LINEAR_MIPMAP_NEAREST)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    // Set the interpolation filter for texture magnification (GL_LINEAR)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Set the texture wrap mode in the S direction (horizontal)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // Set the texture wrap mode in the T direction (vertical)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Create mipmaps and specify texture construction parameters
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    // Free the allocated memory for image data
    free(data);

    return texture; // Return the generated texture identifier
}

// Function to initialize the textures for the sun, planet, and moon
void initializeTextures()
{
    tex_sun = loadTexture("Textures/sun.bmp");
    tex_earth = loadTexture("Textures/earth.bmp");
    tex_moon = loadTexture("Textures/moon.bmp");
}

// Function to generate random coordinates for stars
void starCoordinates() {
    srand(time(NULL)); // "Reset" based on the current time
    for (int i = 0; i < NUM_STARS; i++) {
        stars[i][0] = (float)(rand() % 2000 - 1000) / 100.0f;
        stars[i][1] = (float)(rand() % 2000 - 1000) / 100.0f;
        stars[i][2] = (float)(rand() % 2000 - 1000) / 100.0f;
    }
}

// Function to check if a star is hidden between the object and the camera
// Parameters:
//   - starX, starY, starZ: star coordinates in three-dimensional space
//   - sphereRadius: radius of the sphere representing the object
//   - objectX, objectY, objectZ: object coordinates in three-dimensional space (e.g., camera)
// Returns:
//   - true if the star is hidden behind the object relative to the camera, otherwise false
bool starHidden(float starX, float starY, float starZ, float sphereRadius, float objectX, float objectY, float objectZ)
{
    // Calculate the distance between the star and the object using the Pythagorean theorem
    float distance = sqrtf(powf(starX - objectX, 2) + powf(starY - objectY, 2) + powf(starZ - objectZ, 2));

    // Check if the distance is less than the radius of the sphere representing the object
    // If it is, the star is hidden behind the object relative to the camera
    return distance < sphereRadius;
}

// Function to draw the sun
void drawSun() {

    // Sun material and light configuration
    GLfloat sun_emission[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat sun_specular1[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat sun_shininess = 0.0f;

    glMaterialfv(GL_FRONT, GL_EMISSION, sun_emission);
    glMaterialfv(GL_FRONT, GL_SPECULAR, sun_specular1);
    glMaterialf(GL_FRONT, GL_SHININESS, sun_shininess);

    GLfloat sun_position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat sun_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat sun_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat sun_specular2[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, sun_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sun_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_specular2);

    // Draw the sun
    glRotatef(90.0f, -1.0f, 0.0f, 0.0f); // Rotation to fix texture
    GLUquadricObj* sphere = NULL;
    sphere = gluNewQuadric();
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_sun);
    gluSphere(sphere, 1.0f, 50, 50);
    glDisable(GL_TEXTURE_2D);
}

// Function to draw the planet and the moon
void drawPlanetAndMoon() {

    glPushMatrix();

    // Planet and moon material and light configuration
    GLfloat planet_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat planet_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat planet_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat planet_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat planet_shininess = 0.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, planet_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, planet_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, planet_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, planet_emission);
    glMaterialf(GL_FRONT, GL_SHININESS, planet_shininess);

    glRotatef(planetAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(planetDistance, 0.0f, 0.0f);

    // Draw the planet
    GLUquadricObj* sphere = NULL;
    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricTexture(sphere, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D, tex_earth);
    gluSphere(sphere, 0.25f, 50, 50);
    glDisable(GL_TEXTURE_2D);

    // Draw the moon
    glRotatef(moonAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(moonDistance, 0.0f, 0.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_moon);
    gluSphere(sphere, 0.07f, 50, 50);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

// Function to draw stars, avoiding them being between the camera and the system
void drawStars() {
    for (int i = 0; i < NUM_STARS; i++) {
        if (!starHidden(stars[i][0], stars[i][1], stars[i][2], 5.5f, 0.0f, 0.0f, 0.0f)) {
            glPushMatrix();
            glTranslatef(stars[i][0], stars[i][1], stars[i][2] + 1.0f);
            GLfloat starColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

            // Set the star color
            glMaterialfv(GL_FRONT, GL_DIFFUSE, starColor);

            // Draw the sphere representing the star
            glutSolidSphere(0.01f, 10, 10);

            glPopMatrix();
        }
    }
}

// Function to render the scene
void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color and depth buffer

    glMatrixMode(GL_MODELVIEW); // Select the model-view matrix
    glLoadIdentity(); // Load the identity matrix

    // Lighting configuration
    GLfloat light_position[] = { 2.0, 2.0, 7.0, 1.0 }; // Light source position
    GLfloat light_white[] = { 1.0, 1.0, 1.0, 1.0 }; // Light color

    glEnable(GL_LIGHT0); // Enable light source 0
    glEnable(GL_LIGHTING); // Enable lighting

    gluLookAt(2.0, 2.0, 7.0, // Set camera position (eye)
        0.0, 0.0, 0.0, // Set the point the camera is looking at (center)
        0.0, 1.0, 0.0); // Set the camera's orientation vector (up)

    drawStars(); // Call the function to draw the stars
    glRotatef(angle, 0.0f, 0.1f, 0.0f); // Apply rotation to the object
    drawSun(); // Call the function to draw the sun
    drawPlanetAndMoon(); // Call the function to draw the planet and the moon

    glutSwapBuffers(); // Swap the buffers (avoid flickering)
}

// Function for window resizing
void resizeWindow(int width, int height) {
    if (height == 0) height = 1; // Avoid division by zero

    float aspectRatio = (float)width / (float)height; // Calculate the window's aspect ratio

    glViewport(0, 0, width, height); // Set the viewport

    glMatrixMode(GL_PROJECTION); // Select the projection matrix
    glLoadIdentity(); // Load the identity matrix

    gluPerspective(45.0, aspectRatio, 0.1, 100.0); // Define the perspective
}

// Function called every frame update
void update(int value) {
    angle += 2.0f; // Increment the rotation angle
    if (angle >= 360.0f)
        angle -= 360.0f; // Keep the angle within [0, 360)

    planetAngle += 0.2f; // Increment planet rotation angle
    if (planetAngle >= 360.0f)
        planetAngle -= 360.0f; // Keep the angle within [0, 360)

    moonAngle += 4.0f; // Increment moon rotation angle
    if (moonAngle >= 360.0f)
        moonAngle -= 360.0f; // Keep the angle within [0, 360)

    glutPostRedisplay(); // Mark the window for redraw

    glutTimerFunc(16, update, 0); // Call the function again after 16ms (approximately 60 frames per second)
}

int main(int argc, char** argv) {
    glutInit(&argc, argv); // Initialize GLUT
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); // Configure the display mode
    glutInitWindowSize(800, 600); // Set the window size
    glutCreateWindow("Solar System"); // Create the window with the title "Solar System"

    initializeTextures(); // Call the function to initialize textures
    starCoordinates(); // Call the function to initialize star coordinates

    glEnable(GL_DEPTH_TEST); // Enable depth testing

    glutDisplayFunc(renderScene); // Define the scene rendering function
    glutReshapeFunc(resizeWindow); // Define the window resizing function
    glutTimerFunc(16, update, 0); // Define the scene update function

    glutMainLoop(); // Start the main GLUT loop

    return 0; // Return 0 when the program finishes
}
