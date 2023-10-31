#include "EclipseMap.h"

using namespace std;

#define DEBUG false

void EclipseMap::Render(const char *coloredTexturePath, const char *greyTexturePath, const char *moonTexturePath) {
    // Open window
    GLFWwindow *window = openWindow(windowName, screenWidth, screenHeight);
    
    // Moon commands
    // Load shaders
    GLuint moonShaderID = initShaders("moonShader.vert", "moonShader.frag");

    initMoonColoredTexture(moonTexturePath, moonShaderID);

    // TODO: Set moonVertices
    initSphere(&moonVertices, &moonIndices, moonRadius);
    
    // TODO: Configure Buffers
    initBuffers(true);

    // World commands
    // Load shaders
    GLuint worldShaderID = initShaders("worldShader.vert", "worldShader.frag");

    initColoredTexture(coloredTexturePath, worldShaderID);
    initGreyTexture(greyTexturePath, worldShaderID);

    // TODO: Set worldVertices
    initSphere(&worldVertices, &worldIndices, radius);
        
    // TODO: Configure Buffers
    initBuffers(false);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Main rendering loop
    iter = 0;
    iterSelf = 0;
    iterMoonSelf = 0;
    textureOffset = 1/horizontalSplitCount;
    do {
        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        aspectRatio = (float)screenWidth/screenHeight;
        projectionAngle = 180*atan(aspectRatio)/PI;
        glViewport(0, 0, screenWidth, screenHeight);

        glClearStencil(0);
        glClearDepth(1.0f);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // TODO: Handle key presses
        if(DEBUG) handleKeyPressDebugMode(window);
        else handleKeyPress(window);

        // TODO: Manipulate rotation variables
        
        // TODO: Bind textures
        glUniform1i(glGetUniformLocation(moonShaderID, "MoonTexColor"),GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, moonTextureColor);
        glActiveTexture(GL_TEXTURE0);

        glUniform1i(glGetUniformLocation(worldShaderID, "TexColor"),GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureColor);
        glActiveTexture(GL_TEXTURE1);

        glUniform1i(glGetUniformLocation(worldShaderID, "TexGrey"),GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textureGrey);
        glActiveTexture(GL_TEXTURE2);
        
        // TODO: Use moonShaderID program
        glUseProgram(moonShaderID);

        // TODO: Update camera at every frame
        calculateCam(true);
        
        // TODO: Update uniform variables at every
        calculateUniforms(moonShaderID, true);
        
        // TODO: Bind moon vertex array   
        bindArrayObject(true);

        // TODO: Draw moon object
        glDrawElements(GL_TRIANGLES, moonIndices.size(), GL_UNSIGNED_INT, nullptr);
        
        /*************************/

        // TODO: Use worldShaderID program
        glUseProgram(worldShaderID);

        // TODO: Update camera at every frame
        calculateCam(false);

        // TODO: Update uniform variables at every frame
        calculateUniforms(worldShaderID, false);
        
        // TODO: Bind world vertex array
        bindArrayObject(false);

        // TODO: Draw world object
        glDrawElements(GL_TRIANGLES, worldIndices.size(), GL_UNSIGNED_INT, nullptr);     

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(window));

    // Delete buffers
    glDeleteBuffers(1, &moonVAO);
    glDeleteBuffers(1, &moonVBO);
    glDeleteBuffers(1, &moonEBO);

    // Delete buffers
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
   
    glDeleteProgram(moonShaderID);
    glDeleteProgram(worldShaderID);

    // Unbind VAO and Disable VAA
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    // Close window
    glfwTerminate();
}

void EclipseMap::handleKeyPress(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // P
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        if(displayFormat){
            displayFormat = displayFormatOptions::fullScreen;
            screenWidth = 1920;
            screenHeight = 1080;
        }
        else{
            displayFormat = displayFormatOptions::windowed;
            screenWidth = defaultScreenWidth;
            screenHeight = defaultScreenHeight;
        }
        glfwSetWindowMonitor(window, NULL, 1, 31, screenWidth, screenHeight, mode->refreshRate);
    }

    // I
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        pitch = startPitch;
        yaw = startYaw;
        speed = startSpeed;
        screenWidth = defaultScreenWidth;
        screenHeight = defaultScreenHeight;
        displayFormat = displayFormatOptions::windowed;
        heightFactor = 80;
        textureOffset = 0;
        orbitDegree = 0;
        cameraUp = cameraStartUp;
        cameraPosition = cameraStartPosition;
        cameraDirection = cameraStartDirection;
        selfRotation = 0;
        moonSelfRotation = 0;
        iter = 0;
        iterSelf = 0;
        iterMoonSelf = 0;
        projectionAngle = 45;
        aspectRatio = 1;
    }

    // F
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        heightFactor-=10;
    }

    // R
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        heightFactor+=10;
    }

    // A
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        yaw-=0.05;
    }

    // D
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        yaw+=0.05;
    }

    //S
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        pitch-=0.05;
    }

    // W
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        pitch+=0.05;
    }

    // H
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        speed-=0.01;
    }

    // Y
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        speed+=0.01;
    }

    // X
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        speed = 0;
    }

}

void EclipseMap::handleKeyPressDebugMode(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // P
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && availP) {
        availP = false;
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        if(displayFormat){
            displayFormat = displayFormatOptions::fullScreen;
            screenWidth = 1920;
            screenHeight = 1080;
            cout << "fullscreen" << endl;
        }
        else{
            displayFormat = displayFormatOptions::windowed;
            screenWidth = defaultScreenWidth;
            screenHeight = defaultScreenHeight;
            cout << "windowed" << endl;
        }
        glfwSetWindowMonitor(window, NULL, 1, 31, screenWidth, screenHeight, mode->refreshRate);
    }
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && !availP) availP = true;

    // I
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && availI) {
        availI = false;
        pitch = startPitch;
        yaw = startYaw;
        speed = startSpeed;
        screenWidth = defaultScreenWidth;
        screenHeight = defaultScreenHeight;
        displayFormat = displayFormatOptions::windowed;
        heightFactor = 80;
        textureOffset = 0;
        orbitDegree = 0;
        cameraUp = cameraStartUp;
        cameraPosition = cameraStartPosition;
        cameraDirection = cameraStartDirection;
        selfRotation = 0;
        moonSelfRotation = 0;
        iter = 0;
        iterSelf = 0;
        iterMoonSelf = 0;
        projectionAngle = 45;
        aspectRatio = 1;
        cout << "reset initials" << endl;
    }
    if(glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE && !availI) availI = true;

    // F
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && availF) {
        availF = false;
        heightFactor-=10;
        cout << "height factor -10" << endl;
    }
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE && !availF) availF = true;

    // R
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && availR) {
        availR = false;
        heightFactor+=10;
        cout << "height factor +10" << endl;
    }
    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE && !availR) availR = true;

    // A
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && availA) {
        availA = false;
        yaw-=0.05;
        cout << "yaw -0.05" << endl;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && !availA) availA = true;

    // D
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && availD) {
        availD = false;
        yaw+=0.05;
        cout << "yaw +0.05" << endl;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE && !availD) availD = true;

    //S
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && availS) {
        availS = false;
        pitch-=0.05;
        cout << "pitch -0.05" << endl;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE && !availS) availS = true;

    // W
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && availW) {
        availW = false;
        pitch+=0.05;
        cout << "pitch +0.05" << endl;
    }
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && !availW) availW = true;

    // H
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && availH) {
        availH = false;
        speed-=0.01;
        cout << "speed -0.01" << endl;
    }
    if(glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE && !availH) availH = true;

    // Y
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && availY) {
        availY = false;
        speed+=0.01;
        cout << "speed +0.01" << endl;
    }
    if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE && !availY) availY = true;

    // X
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && availX) {
        availX = false;
        speed = 0;
        cout << "stop" << endl;
    }
    if(glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE && !availX) availX = true;

    // Q - extra
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && availQ) {
        availQ = false;
        cout << "Pitch: " << pitch << endl;
        cout << "Yaw: " << yaw << endl;
        cout << "Speed: " << speed << endl;
        cout << "Display: " << screenWidth << "x" << screenHeight << " Windowed: " << displayFormat << " aspectRatio: " << aspectRatio << " FOV: " << projectionAngle << endl;
        cout << "Camera-> Position: {" <<  cameraPosition.x << "," << cameraPosition.y << "," << cameraPosition.z << "}"
        << " Direction: {" <<  cameraDirection.x << "," << cameraDirection.y << "," << cameraDirection.z << "}"
        << " Up: {" <<  cameraUp.x << "," << cameraUp.y << "," << cameraUp.z << "}" << endl;
        cout << "orbitDegree: " << orbitDegree << " selfRotation: " << selfRotation << " moonSelfRotation: " << moonSelfRotation << endl;
        cout << "heightFactor: " << heightFactor << " textureOffset: " << textureOffset << endl;
    }
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE && !availQ) availQ = true;

    // LEFT_ARROW
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        cameraPosition.x += 10.0f;
    }

    // RIGHT_ARROW
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        cameraPosition.x -= 10.0f;
    }

    // UP_ARROW
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        cameraPosition.y -= 10.0f;
    }

    // DOWN_ARROW
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        cameraPosition.y += 10.0f;
    }

    // M
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        cameraPosition.z -= 10.0f;
    }

    // N
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        cameraPosition.z += 10.0f;
    }

}

GLFWwindow *EclipseMap::openWindow(const char *windowName, int width, int height) {
    if (!glfwInit()) {
        getchar();
        return 0;
    }

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(width, height, windowName, NULL, NULL);
    glfwSetWindowMonitor(window, NULL, 1, 31, screenWidth, screenHeight, mode->refreshRate);

    if (window == NULL) {
        getchar();
        glfwTerminate();
        return 0;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        getchar();
        glfwTerminate();
        return 0;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(0, 0, 0, 0);

    return window;
}


void EclipseMap::initColoredTexture(const char *filename, GLuint shader) {
    int width, height;
    glGenTextures(1, &textureColor);
    cout << shader << endl;
    glBindTexture(GL_TEXTURE_2D, textureColor);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *raw_image = NULL;
    int bytes_per_pixel = 3;   /* or 1 for GRACYSCALE images */
    int color_space = JCS_RGB; /* or JCS_GRAYSCALE for grayscale images */

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen(filename, "rb");
    unsigned long location = 0;
    int i = 0, j = 0;

    if (!infile) {
        printf("Error opening jpeg file %s\n!", filename);
        return;
    }
    printf("Texture filename = %s\n", filename);

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error(&jerr);
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress(&cinfo);
    /* this makes the library read from infile */
    jpeg_stdio_src(&cinfo, infile);
    /* reading the image header which contains image information */
    jpeg_read_header(&cinfo, TRUE);
    /* Start decompression jpeg here */
    jpeg_start_decompress(&cinfo);

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char *) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *) malloc(cinfo.output_width * cinfo.num_components);
    /* read one scan line at a time */
    while (cinfo.output_scanline < cinfo.image_height) {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        for (i = 0; i < cinfo.image_width * cinfo.num_components; i++)
            raw_image[location++] = row_pointer[0][i];
    }

    height = cinfo.image_height;
    width = cinfo.image_width;


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);
   

    imageWidth = width;
    imageHeight = height;

    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(shader); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:

    glUniform1i(glGetUniformLocation(shader, "TexColor"), 0);
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row_pointer[0]);
    free(raw_image);
    fclose(infile);

}

void EclipseMap::initGreyTexture(const char *filename, GLuint shader) {

    glGenTextures(1, &textureGrey);
    glBindTexture(GL_TEXTURE_2D, textureGrey);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height;

    unsigned char *raw_image = NULL;
    int bytes_per_pixel = 3;   /* or 1 for GRACYSCALE images */
    int color_space = JCS_RGB; /* or JCS_GRAYSCALE for grayscale images */

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen(filename, "rb");
    unsigned long location = 0;
    int i = 0, j = 0;

    if (!infile) {
        printf("Error opening jpeg file %s\n!", filename);
        return;
    }
    printf("Texture filename = %s\n", filename);

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error(&jerr);
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress(&cinfo);
    /* this makes the library read from infile */
    jpeg_stdio_src(&cinfo, infile);
    /* reading the image header which contains image information */
    jpeg_read_header(&cinfo, TRUE);
    /* Start decompression jpeg here */
    jpeg_start_decompress(&cinfo);

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char *) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *) malloc(cinfo.output_width * cinfo.num_components);
    /* read one scan line at a time */
    while (cinfo.output_scanline < cinfo.image_height) {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        for (i = 0; i < cinfo.image_width * cinfo.num_components; i++)
            raw_image[location++] = row_pointer[0][i];
    }

    height = cinfo.image_height;
    width = cinfo.image_width;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);
  



    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(shader); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:

    glUniform1i(glGetUniformLocation(shader, "TexGrey"), 1);
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row_pointer[0]);
    free(raw_image);
    fclose(infile);

}

void EclipseMap::initMoonColoredTexture(const char *filename, GLuint shader) {
    int width, height;
    glGenTextures(1, &moonTextureColor);
    cout << shader << endl;
    glBindTexture(GL_TEXTURE_2D, moonTextureColor);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *raw_image = NULL;
    int bytes_per_pixel = 3;   /* or 1 for GRACYSCALE images */
    int color_space = JCS_RGB; /* or JCS_GRAYSCALE for grayscale images */

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen(filename, "rb");
    unsigned long location = 0;
    int i = 0, j = 0;

    if (!infile) {
        printf("Error opening jpeg file %s\n!", filename);
        return;
    }
    printf("Texture filename = %s\n", filename);

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error(&jerr);
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress(&cinfo);
    /* this makes the library read from infile */
    jpeg_stdio_src(&cinfo, infile);
    /* reading the image header which contains image information */
    jpeg_read_header(&cinfo, TRUE);
    /* Start decompression jpeg here */
    jpeg_start_decompress(&cinfo);

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char *) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *) malloc(cinfo.output_width * cinfo.num_components);
    /* read one scan line at a time */
    while (cinfo.output_scanline < cinfo.image_height) {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        for (i = 0; i < cinfo.image_width * cinfo.num_components; i++)
            raw_image[location++] = row_pointer[0][i];
    }

    height = cinfo.image_height;
    width = cinfo.image_width;


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);
   

    imageWidth = width;
    imageHeight = height;

    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(shader); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:

    glUniform1i(glGetUniformLocation(shader, "MoonTexColor"), 2);
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row_pointer[0]);
    free(raw_image);
    fclose(infile);

}

void EclipseMap::initSphere(vector<vertex>* sphereVertices, vector<unsigned int>* sphereIndices, float radius){
    float alpha, beta, nx, ny, nz, m, n, coffA, coffB;
    vertex tmp;
    coffA = 2*PI/horizontalSplitCount;
    coffB = PI/verticalSplitCount;
    for(int i=0;i<=horizontalSplitCount;i++){
        beta=coffB*i;
        nz = cos(beta);
        m = i*(horizontalSplitCount+1);
        n = (i+1)*(horizontalSplitCount+1);
        for(int j=0;j<=2*verticalSplitCount;j++,m++,n++){
            alpha=coffA*j;
            nx = sin(beta)*cos(alpha);
            ny = sin(beta)*sin(alpha);
            tmp.position = glm::vec3(nx*radius, ny*radius, nz*radius);
            tmp.normal = glm::vec3(nx, ny, nz);
            tmp.texture = glm::vec2((float)j/horizontalSplitCount,(float)i/verticalSplitCount);
            
            sphereVertices->push_back(tmp);

            if(i<verticalSplitCount){
                if(i>0){
                    sphereIndices->push_back(m+1);
                    sphereIndices->push_back(m);
                    sphereIndices->push_back(n);
                }
                if(i<(verticalSplitCount-1)){
                    sphereIndices->push_back(m+1);
                    sphereIndices->push_back(n);
                    sphereIndices->push_back(n+1);
                }
            }
        }
    }
}

void EclipseMap::initBuffers(bool moon){
    if(moon){
        glGenVertexArrays(1, &moonVAO);
        glGenBuffers(1, &moonVBO);
        glGenBuffers(1, &moonEBO);
        
        glBindVertexArray(moonVAO);
        glBindBuffer(GL_ARRAY_BUFFER, moonVBO);
        glBufferData(GL_ARRAY_BUFFER, moonVertices.size()*sizeof(vertex), moonVertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, moonEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, moonIndices.size()*sizeof(unsigned int), moonIndices.data(), GL_STATIC_DRAW);
    }
    else{
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, worldVertices.size()*sizeof(vertex), worldVertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, worldIndices.size()*sizeof(unsigned int), worldIndices.data(), GL_STATIC_DRAW);
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *) (sizeof(glm::vec3)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *) (sizeof(glm::vec3)*2));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
}

void EclipseMap::calculateCam(bool moon){
    if(moon){
        moonSelfRotation = (++iterMoonSelf)*(rotation);
        if(moonSelfRotation>=360){
            moonSelfRotation = 0;
            iterMoonSelf=0;
        }
        orbitDegree = ((++iter)*0.02);
        if(orbitDegree>=2*PI){
            orbitDegree = 0;
            iter=0;
        }
        mMod = glm::rotate(mIdent, moonSelfRotation, glm::vec3(0,0,-1));
        mTra = glm::translate(moonPosition);
        mRot = glm::rotate(mIdent, orbitDegree, glm::vec3(0,0,-1));
        mMod = mRot*mTra*mMod;
        
    }
    else{
        selfRotation = (++iterSelf)*(rotation);
        if(selfRotation>=360){
            selfRotation = 0;
            iterSelf=0;
        }
        mRot = glm::rotate(mIdent, selfRotation, glm::vec3(0,0,1));
        mMod = mRot;
    }
    cameraDirection.x = cos(yaw)*cos(pitch);
    cameraDirection.y = sin(pitch);
    cameraDirection.z = sin(yaw)*cos(pitch);
    cameraPosition+=cameraDirection*speed;
    // cameraDirection = glm::normalize(cameraDirection);
    mCam = glm::lookAt(cameraPosition, cameraPosition+cameraDirection, cameraStartUp);
    radian = projectionAngle*PI/180;
    mPro = glm::perspective(radian, aspectRatio, near, far);
    mMix = mPro*mCam*mMod;
}

void EclipseMap::calculateUniforms(GLuint shaderId, bool moon){
    glUniform3fv(glGetUniformLocation(shaderId, "lightPosition"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderId, "cameraPosition"), 1, glm::value_ptr(cameraPosition));
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(mPro));
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(mCam));
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(mMod));
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "MVP"), 1, GL_FALSE, glm::value_ptr(mMix));
    glUniform1f(glGetUniformLocation(shaderId, "heightFactor"), heightFactor);
    glUniform1f(glGetUniformLocation(shaderId, "imageWidth"), screenWidth);
    glUniform1f(glGetUniformLocation(shaderId, "imageHeight"), screenHeight);
    glUniform1f(glGetUniformLocation(shaderId, "textureOffset"), textureOffset);
    if(moon){ glUniform1f(glGetUniformLocation(shaderId, "orbitDegree"), orbitDegree);}

    //cout << "calculateUniforms " << glGetError() << endl;
    
}

void EclipseMap::bindArrayObject(bool moon){
    if(moon) glBindVertexArray(moonVAO);
    else glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2); 
}

void EclipseMap::printVectors(vector<vertex>* sphereVertices, vector<unsigned int>* sphereIndices, bool VP){
    cout << sphereVertices->size() << endl;
    if(!VP){
        for(int i=0;i<sphereVertices->size();i+=3){
            cout << (*sphereVertices)[i].position.x << "," << (*sphereVertices)[i].position.y << "," << (*sphereVertices)[i].position.z  << endl;
        }
        for(int i=0;i<sphereIndices->size();i+=3){
            cout << (*sphereIndices)[i] << ": " << (*sphereVertices)[(*sphereIndices)[i]].position.x << "," << (*sphereVertices)[(*sphereIndices)[i]].position.y << "," << (*sphereVertices)[(*sphereIndices)[i]].position.z << " | "
            << (*sphereIndices)[i+1] << ": " << (*sphereVertices)[(*sphereIndices)[i+1]].position.x << "," << (*sphereVertices)[(*sphereIndices)[i+1]].position.y << "," << (*sphereVertices)[(*sphereIndices)[i+1]].position.z << " | "
            << (*sphereIndices)[i+2] << ": " << (*sphereVertices)[(*sphereIndices)[i+2]].position.x << "," << (*sphereVertices)[(*sphereIndices)[i+2]].position.y << "," << (*sphereVertices)[(*sphereIndices)[i+2]].position.z << " | " << endl;
        }
    }
    else{
        for(int i=0;i<sphereIndices->size();i+=3){
            glm::vec4 tmp1 = mMix*glm::vec4((*sphereVertices)[(*sphereIndices)[i]].position.x,(*sphereVertices)[(*sphereIndices)[i]].position.y,(*sphereVertices)[(*sphereIndices)[i]].position.z,1.0f);
            glm::vec4 tmp2 = mMix*glm::vec4((*sphereVertices)[(*sphereIndices)[i+1]].position.x,(*sphereVertices)[(*sphereIndices)[i+1]].position.y,(*sphereVertices)[(*sphereIndices)[i+1]].position.z,1.0f);
            glm::vec4 tmp3 = mMix*glm::vec4((*sphereVertices)[(*sphereIndices)[i+2]].position.x,(*sphereVertices)[(*sphereIndices)[i+2]].position.y,(*sphereVertices)[(*sphereIndices)[i+2]].position.z,1.0f);

            cout << (*sphereIndices)[i] << ": " << tmp1.x/tmp1.w << "," << tmp1.y/tmp1.w << "," << tmp1.z/tmp1.w << " | "
            << (*sphereIndices)[i+1] << ": " << tmp2.x/tmp2.w << "," << tmp2.y/tmp2.w << "," << tmp2.z/tmp2.w << " | "
            << (*sphereIndices)[i+2] << ": " << tmp3.x/tmp3.w << "," << tmp3.y/tmp3.w << "," << tmp3.z/tmp3.w << " | " << endl;
        }
    }
}