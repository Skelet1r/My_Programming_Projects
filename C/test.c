// minicraft.c — минимальный «майнкрафт-подобный» воксельный сэндбокс на C
// Зависимости: SDL2, OpenGL (совместимый профиль, можно 2.1)
// Сборка (Linux/Arch):
//   sudo pacman -S sdl2
//   gcc minicraft.c -o minicraft -lSDL2 -lGL -lm
// Запуск:
//   ./minicraft
// Управление:
//   WASD — движение, SPACE — вверх, LCTRL — вниз
//   Мышь — поворот камеры
//   ЛКМ — сломать блок, ПКМ — поставить блок (в соседнюю клетку)
//   ESC — выход
// Замечание: код учебный, без шейдеров и VBO; рендер — через glBegin/glEnd

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CHUNK_X 64
#define CHUNK_Y 64
#define CHUNK_Z 32

#define FOV 70.0f
#define MOUSE_SENS 0.1f
#define MOVE_SPEED 8.0f
#define MAX_RANGE 6.0f

static unsigned char world[CHUNK_X][CHUNK_Y][CHUNK_Z];

static float clampf(float v, float a, float b){return v<a?a:(v>b?b:v);} 

// Простая целочисленная псевдошумовая функция (value noise)
static float hash2i(int x, int y){
    unsigned int n = (unsigned int)(x*73856093u) ^ (unsigned int)(y*19349663u);
    n ^= n<<13; n ^= n>>17; n ^= n<<5; 
    return (n & 0xFFFFFF)/16777215.0f; // [0,1]
}
static float lerp(float a, float b, float t){return a + t*(b-a);} 
static float smoothstepf(float t){return t*t*(3-2*t);} 
static float value_noise2f(float x, float y){
    int xi = (int)floorf(x), yi=(int)floorf(y);
    float xf = x - xi, yf = y - yi;
    float v00 = hash2i(xi, yi);
    float v10 = hash2i(xi+1, yi);
    float v01 = hash2i(xi, yi+1);
    float v11 = hash2i(xi+1, yi+1);
    float i1 = lerp(v00, v10, smoothstepf(xf));
    float i2 = lerp(v01, v11, smoothstepf(xf));
    return lerp(i1, i2, smoothstepf(yf));
}
static float fractal_noise2f(float x, float y){
    float total=0, amp=1, freq=1, maxv=0;
    for(int o=0;o<4;o++){ total += value_noise2f(x*freq, y*freq)*amp; maxv+=amp; amp*=0.5f; freq*=2.0f; }
    return total/maxv; // [0,1]
}

static void generate_world(unsigned int seed){
    srand(seed);
    for(int x=0;x<CHUNK_X;x++){
        for(int y=0;y<CHUNK_Y;y++){
            float n = fractal_noise2f(x*0.08f, y*0.08f); // 0..1
            int h = (int)(n*(CHUNK_Z-1)*0.8f) + 6; // базовая высота
            if(h>CHUNK_Z-1) h=CHUNK_Z-1;
            for(int z=0; z<CHUNK_Z; z++){
                world[x][y][z] = (z<=h)? 1 : 0; // 1 — земля, 0 — воздух
                if(z==h && n<0.4f) world[x][y][z]=2; // песок в низинах
                if(z==h && n>0.7f) world[x][y][z]=3; // камень на вершинах
            }
        }
    }
}

// Камера
typedef struct { float x,y,z; float yaw,pitch; } Camera;

// Вспомогательная математика
static void vec3_norm(float v[3]){ float l=sqrtf(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]); if(l>1e-6f){v[0]/=l;v[1]/=l;v[2]/=l;} }
static void vec3_cross(const float a[3], const float b[3], float r[3]){ r[0]=a[1]*b[2]-a[2]*b[1]; r[1]=a[2]*b[0]-a[0]*b[2]; r[2]=a[0]*b[1]-a[1]*b[0]; }

static void apply_perspective(float fov_deg, float aspect, float znear, float zfar){
    float f = 1.0f / tanf((fov_deg * 0.5f) * (float)M_PI/180.0f);
    float m[16] = {0};
    m[0]=f/aspect; m[5]=f; m[10]=(zfar+znear)/(znear-zfar); m[11]=-1; m[14]=(2*zfar*znear)/(znear-zfar);
    glMatrixMode(GL_PROJECTION); glLoadMatrixf(m);
    glMatrixMode(GL_MODELVIEW);
}

static void lookAt(Camera *c){
    float cyaw = cosf(c->yaw*(float)M_PI/180.0f);
    float syaw = sinf(c->yaw*(float)M_PI/180.0f);
    float cp   = cosf(c->pitch*(float)M_PI/180.0f);
    float sp   = sinf(c->pitch*(float)M_PI/180.0f);
    float fwd[3] = { cyaw*cp, sp, -syaw*cp };
    float center[3] = { c->x+fwd[0], c->y+fwd[1], c->z+fwd[2] };

    // Простая матрица вида (gluLookAt эквивалент)
    float up[3]={0,1,0};
    float zaxis[3]={ c->x-center[0], c->y-center[1], c->z-center[2] }; vec3_norm(zaxis);
    float xaxis[3]; vec3_cross(up, zaxis, xaxis); vec3_norm(xaxis);
    float yaxis[3]; vec3_cross(zaxis, xaxis, yaxis);

    float m[16] = {
        xaxis[0], yaxis[0], zaxis[0], 0,
        xaxis[1], yaxis[1], zaxis[1], 0,
        xaxis[2], yaxis[2], zaxis[2], 0,
        - (xaxis[0]*c->x + xaxis[1]*c->y + xaxis[2]*c->z),
        - (yaxis[0]*c->x + yaxis[1]*c->y + yaxis[2]*c->z),
        - (zaxis[0]*c->x + zaxis[1]*c->y + zaxis[2]*c->z),
        1
    };
    glLoadMatrixf(m);
}

static int in_bounds(int x,int y,int z){ return x>=0 && y>=0 && z>=0 && x<CHUNK_X && y<CHUNK_Y && z<CHUNK_Z; }

// Рисуем один квадратик (квад) — вспомогательно
static void draw_face(float x,float y,float z, float nx,float ny,float nz){
    float s=1.0f; // размер блока = 1
    float x0=x, x1=x+1, y0=y, y1=y+1, z0=z, z1=z+1;
    glNormal3f(nx,ny,nz);
    glBegin(GL_QUADS);
    if(nz==1){ glVertex3f(x0,y0,z1); glVertex3f(x1,y0,z1); glVertex3f(x1,y1,z1); glVertex3f(x0,y1,z1);} // front
    else if(nz==-1){ glVertex3f(x1,y0,z0); glVertex3f(x0,y0,z0); glVertex3f(x0,y1,z0); glVertex3f(x1,y1,z0);} // back
    else if(nx==1){ glVertex3f(x1,y0,z1); glVertex3f(x1,y0,z0); glVertex3f(x1,y1,z0); glVertex3f(x1,y1,z1);} // right
    else if(nx==-1){ glVertex3f(x0,y0,z0); glVertex3f(x0,y0,z1); glVertex3f(x0,y1,z1); glVertex3f(x0,y1,z0);} // left
    else if(ny==1){ glVertex3f(x0,y1,z1); glVertex3f(x1,y1,z1); glVertex3f(x1,y1,z0); glVertex3f(x0,y1,z0);} // top
    else if(ny==-1){ glVertex3f(x0,y0,z0); glVertex3f(x1,y0,z0); glVertex3f(x1,y0,z1); glVertex3f(x0,y0,z1);} // bottom
    glEnd();
}

static void set_block_color(unsigned char id, int z){
    switch(id){
        case 1: glColor3f(0.2f, 0.7f, 0.2f); break; // трава
        case 2: glColor3f(0.85f,0.75f,0.5f); break; // песок
        case 3: glColor3f(0.6f,0.6f,0.6f); break;  // камень
        default: glColor3f(1,1,1); break;
    }
}

static void render_world(){
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    for(int x=0;x<CHUNK_X;x++){
        for(int y=0;y<CHUNK_Y;y++){
            for(int z=0;z<CHUNK_Z;z++){
                unsigned char id = world[x][y][z];
                if(!id) continue;
                set_block_color(id, z);
                // рисуем только видимые грани
                if(!in_bounds(x,y,z+1) || !world[x][y][z+1]) draw_face((float)x,(float)z,(float)y, 0,0,1);
                if(!in_bounds(x,y,z-1) || !world[x][y][z-1]) draw_face((float)x,(float)z,(float)y, 0,0,-1);
                if(!in_bounds(x+1,y,z) || !world[x+1][y][z]) draw_face((float)x,(float)z,(float)y, 1,0,0);
                if(!in_bounds(x-1,y,z) || !world[x-1][y][z]) draw_face((float)x,(float)z,(float)y,-1,0,0);
                if(!in_bounds(x,y+1,z) || !world[x][y+1][z]) draw_face((float)x,(float)z,(float)y, 0,1,0);
                if(!in_bounds(x,y-1,z) || !world[x][y-1][z]) draw_face((float)x,(float)z,(float)y, 0,-1,0);
            }
        }
    }
}

// DDA-рейкаст для выбора блока
typedef struct { int hit; int bx,by,bz; int nx,ny,nz; } Hit;
static Hit raycast(Camera *c){
    Hit h={0};
    float cyaw = cosf(c->yaw*(float)M_PI/180.0f);
    float syaw = sinf(c->yaw*(float)M_PI/180.0f);
    float cp   = cosf(c->pitch*(float)M_PI/180.0f);
    float sp   = sinf(c->pitch*(float)M_PI/180.0f);
    float dir[3] = { cyaw*cp, sp, -syaw*cp };

    float x=c->x, y=c->y, z=c->z;
    float step=0.02f, dist=0.0f;
    int lastnx=0,lastny=0,lastnz=0;
    while(dist<MAX_RANGE){
        int bx=(int)floorf(x), by=(int)floorf(z), bz=(int)floorf(y); // заметим: у нас ось Y мира — вверх, но храним как [x][y][z]; для рендера использовали (x,z,y)
        if(in_bounds(bx,by,bz) && world[bx][by][bz]){ h.hit=1; h.bx=bx; h.by=by; h.bz=bz; h.nx=lastnx; h.ny=lastny; h.nz=lastnz; return h; }
        // шаг с отслеживанием какой нормали мы пересекли приблизительно
        float nx = dir[0]>0?1:-1; float ny = dir[1]>0?1:-1; float nz = dir[2]>0?1:-1;
        lastnx=(fabsf(dir[0])>fabsf(dir[1]) && fabsf(dir[0])>fabsf(dir[2]))?(int)nx:0;
        lastny=(fabsf(dir[1])>fabsf(dir[0]) && fabsf(dir[1])>fabsf(dir[2]))?(int)ny:0;
        lastnz=(fabsf(dir[2])>fabsf(dir[0]) && fabsf(dir[2])>fabsf(dir[1]))?(int)nz:0;
        x+=dir[0]*step; y+=dir[1]*step; z+=dir[2]*step; dist+=step;
    }
    return h;
}

int main(int argc, char** argv){
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){ fprintf(stderr, "SDL init error: %s\n", SDL_GetError()); return 1; }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* win = SDL_CreateWindow("MiniMinecraft (C + SDL2)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if(!win){ fprintf(stderr, "Window error: %s\n", SDL_GetError()); return 1; }

    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    SDL_GL_SetSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);

    generate_world((unsigned)time(NULL));

    Camera cam = { CHUNK_X/2.0f, CHUNK_Z+3.0f, CHUNK_Y/2.0f, -90.0f, -20.0f };

    int running=1; Uint64 prev = SDL_GetPerformanceCounter();
    SDL_SetRelativeMouseMode(SDL_TRUE);

    int width=1200, height=800;
    glViewport(0,0,width,height);
    apply_perspective(FOV, (float)width/height, 0.1f, 200.0f);

    while(running){
        // DT
        Uint64 now = SDL_GetPerformanceCounter();
        double dt = (double)(now - prev) / SDL_GetPerformanceFrequency();
        prev = now;

        SDL_Event e; 
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT) running=0;
            if(e.type==SDL_WINDOWEVENT && e.window.event==SDL_WINDOWEVENT_SIZE_CHANGED){
                width=e.window.data1; height=e.window.data2; glViewport(0,0,width,height); apply_perspective(FOV,(float)width/height,0.1f,200.0f);
            }
            if(e.type==SDL_KEYDOWN && e.key.keysym.sym==SDLK_ESCAPE) running=0;
            if(e.type==SDL_MOUSEMOTION){
                cam.yaw   += e.motion.xrel * MOUSE_SENS;
                cam.pitch -= e.motion.yrel * MOUSE_SENS;
                cam.pitch = clampf(cam.pitch, -89.0f, 89.0f);
            }
            if(e.type==SDL_MOUSEBUTTONDOWN){
                Hit h = raycast(&cam);
                if(h.hit){
                    if(e.button.button==SDL_BUTTON_LEFT){
                        world[h.bx][h.by][h.bz]=0; // ломаем
                    } else if(e.button.button==SDL_BUTTON_RIGHT){
                        int nx=h.bx + (h.nx);
                        int ny=h.by + (h.nz); // внимание: в рендере оси Y/Z менялись местами, здесь учитываем нормаль по z как "мировой y"
                        int nz=h.bz + (h.ny);
                        if(in_bounds(nx,ny,nz) && !world[nx][ny][nz]) world[nx][ny][nz]=1; // ставим траву
                    }
                }
            }
        }

        const Uint8* ks = SDL_GetKeyboardState(NULL);
        float cyaw = cosf(cam.yaw*(float)M_PI/180.0f);
        float syaw = sinf(cam.yaw*(float)M_PI/180.0f);
        float cp   = cosf(cam.pitch*(float)M_PI/180.0f);
        float sp   = sinf(cam.pitch*(float)M_PI/180.0f);
        float forward[3] = { cyaw*cp, 0, -syaw*cp }; // горизонтальная проекция
        vec3_norm(forward);
        float right[3] = { -forward[2], 0, forward[0] };
        float speed = MOVE_SPEED * (float)dt * (ks[SDL_SCANCODE_LSHIFT]?2.0f:1.0f);
        if(ks[SDL_SCANCODE_W]){ cam.x += forward[0]*speed; cam.z += forward[2]*speed; }
        if(ks[SDL_SCANCODE_S]){ cam.x -= forward[0]*speed; cam.z -= forward[2]*speed; }
        if(ks[SDL_SCANCODE_A]){ cam.x -= right[0]*speed;   cam.z -= right[2]*speed; }
        if(ks[SDL_SCANCODE_D]){ cam.x += right[0]*speed;   cam.z += right[2]*speed; }
        if(ks[SDL_SCANCODE_SPACE]) cam.y += speed;
        if(ks[SDL_SCANCODE_LCTRL]) cam.y -= speed;
        cam.y = clampf(cam.y, 1.0f, CHUNK_Z+20.0f);

        glClearColor(0.53f,0.81f,0.92f,1.0f); // небо
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        lookAt(&cam);

        // простое directional освещение
        glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
        float light_dir[4]={-0.5f, -1.0f, -0.3f, 0.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, light_dir);
        float diffuse[4]={0.9f,0.9f,0.9f,1.0f}; glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        float ambient[4]={0.3f,0.3f,0.35f,1.0f}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

        render_world();

        glDisable(GL_LIGHTING);

        SDL_GL_SwapWindow(win);
    }

    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
