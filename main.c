
#include <raylib.h>


#include <stdlib.h>         // Required for: malloc(), free()
#include <math.h>           // Required for: sinf()
#include <string.h>         // Required for: memcpy()



#define MAX_SAMPLES               512
#define MAX_SAMPLES_PER_UPDATE   4096

#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include "extras/raygui.h"                 // Required for GUI controls

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [audio] example - raw audio streaming");

    InitAudioDevice();              // Initialize audio device

    SetAudioStreamBufferSizeDefault(MAX_SAMPLES_PER_UPDATE);

    // Init raw audio stream (sample rate: 22050, sample size: 16bit-short, channels: 1-mono)
    AudioStream stream = LoadAudioStream(44100, 16, 1);

    // Buffer for the single cycle waveform we are synthesizing
    // Size in bytes. 2bytes  * 512 = 1024bytes Unsigned long
    short *data = (short *)malloc(sizeof(short)*MAX_SAMPLES);

    // Frame buffer, describing the waveform when repeated over the course of a frame
    short *writeBuf = (short *)malloc(sizeof(short)*MAX_SAMPLES_PER_UPDATE);

    PlayAudioStream(stream);        // Start processing stream buffer (no data loaded currently)

    // Position read in to determine next frequency

    // POSIBLEMENTE QUITAR
    // Vector2 mousePosition = { -100.0f, -100.0f };

    // Cycles per second (hz)
    float frequency = 0.0f;

    // Previous value, used to test if sine needs to be rewritten, and to smoothly modulate frequency
    float oldFrequency = 1.0f;

    // Cursor to read and copy the samples of the sine wave buffer
    int readCursor = 0;

    // Computed size in samples of the sine wave
    int waveLength = 1;

    // Vector2 position = { 0, 0 };

   

    float perilla1= 50.0f;
    // float perilla2= 50.0f;
    // float perilla3= 50.0f;
    // float perilla4= 50.0f;
    // float perilla5= 50.0f;



    // Vector2 center = {(GetScreenWidth() - 300)/2.0f, GetScreenHeight()/2.0f };

    float innerRadius = 80.0f;
    float outerRadius = 190.0f;


    // Pausar
     bool play = true;

    //  bucle de sonido infinito
    bool loopInfinito  = false;

    // Funcion original de seno
    bool originalFunction = true;\


    
    SetTargetFPS(30);               // Set our game to run at 30 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
    

    //    if (IsKeyPressed(KEY_P))
    //         play = !play;
    //     if(IsKeyPressed(KEY_L))
    //         loopInfinito = !loopInfinito;
    //     if(IsKeyPressed(KEY_Q))
    //         originalFunction = !originalFunction;


        // if (!loopInfinito) frequency = 0;

        if (play==true) PlayAudioStream(stream);
        
        

        // if (play==true) PauseAudioStream(stream);
        //     else PlayAudioStream(stream);

         // Frecuencia para tecla DO
            if (IsKeyDown(KEY_Z)) {
               
                frequency = 261.6f;
            }
            // Frecuencia para tecla DO #
            if (IsKeyDown(KEY_S)) {
                frequency = 277.2f;
            }
            //Frecuencia para tecla RE
            if (IsKeyDown(KEY_X)) {
                frequency = 293.6f;
            }
            //Frecuencia para tecla RE#
            if (IsKeyDown(KEY_D))  {
                frequency = 311.1f;
            }
            //Frecuencia para tecla MI
            if (IsKeyDown(KEY_C)) {
                frequency = 329.6f;
            }
            //Frecuencia para tecla FA
            if (IsKeyDown(KEY_V)) {
                frequency = 349.2f;
            }
            //Frecuencia para tecla FA#
            if (IsKeyDown(KEY_G)) {
                frequency = 370.0f;
            }
            //Frecuencia para tecla SOL
            if (IsKeyDown(KEY_B)) {
                frequency = 392.0f;
            }
            //Frecuencia para tecla SOL#
            if (IsKeyDown(KEY_H)) {
                frequency = 415.3f;
            }
            //Frecuencia para tecla LA
            if (IsKeyDown(KEY_N)) {
                frequency = 440.0f;
            }
            //Frecuencia para tecla LA#
            if (IsKeyDown(KEY_J)) {
                frequency = 466.2f;
            }
            //Frecuencia para tecla SI
            if (IsKeyDown(KEY_M)) {
                frequency = 493.2f;
            }
            

        // Rewrite the sine wave.
        // Compute two cycles to allow the buffer padding, simplifying any modulation, resampling, etc.
        if (frequency != oldFrequency)
        {
            // Compute wavelength. Limit size in both directions.
            int oldWavelength = waveLength;
            waveLength = (int)(22050/frequency);
            if (waveLength > MAX_SAMPLES/2) waveLength = MAX_SAMPLES/2;
            if (waveLength < 1) waveLength = 1;

            // Write sine wave.
            for (int i = 0; i < waveLength*2; i++)
            {
                if(originalFunction){
                    data[i] = (short)(sinf(((2*PI*(float)i/waveLength)))*32000);
                }else {

                data[i] = (short)(sinf(((2*PI*(float)i/waveLength)))*32000)+(short)(sinf(((2*PI*(float)i/waveLength)))*32000);

                 }

            }

            // Scale read cursor's position to minimize transition artifacts
            readCursor = (int)(readCursor * ((float)waveLength / (float)oldWavelength));
            oldFrequency = frequency;
        }

        // Refill audio stream if required
        if (IsAudioStreamProcessed(stream))
        {
            // Synthesize a buffer that is exactly the requested size
            int writeCursor = 0;

            while (writeCursor < MAX_SAMPLES_PER_UPDATE)
            {
                // Start by trying to write the whole chunk at once
                int writeLength = MAX_SAMPLES_PER_UPDATE-writeCursor;

                // Limit to the maximum readable size
                int readLength = waveLength-readCursor;

                if (writeLength > readLength) writeLength = readLength;

                // Write the slice
                memcpy(writeBuf + writeCursor, data + readCursor, writeLength*sizeof(short));

                // Update cursors and loop audio
                readCursor = (readCursor + writeLength) % waveLength;

                writeCursor += writeLength;
            }

            // Copy finished frame to audio stream
            UpdateAudioStream(stream, writeBuf, MAX_SAMPLES_PER_UPDATE);
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText(TextFormat("Frecuencia del seno: %i",(int)frequency), GetScreenWidth() - 300, 10, 20, DARKGRAY);
            DrawText("Presionar de z a m escala normal.", 10, 10, 20, DARKGRAY);
            DrawText("Presionar q para cambiar de funcion.", 10, 30, 20, DARKGRAY);
            DrawText("Presionar p para pausa.", 10, 50, 20, DARKGRAY);
            DrawText("Presionar l para bucle de sonido.", 10, 70, 20, DARKGRAY);

            // perilla1 = GuiSliderBar((Rectangle){ 600, 140, 120, 20 }, "Perilla1", NULL, perilla1, 0, 100);



        

            // DrawLine(500, 0, 500, GetScreenHeight(), Fade(LIGHTGRAY, 0.6f));
            // DrawRectangle(500, 0, GetScreenWidth() - 500, GetScreenHeight(), Fade(LIGHTGRAY, 0.3f));

            




        // Draw GUI controls
            //------------------------------------------------------------------------------
            

            innerRadius = GuiSliderBar((Rectangle){ 600, 140, 120, 20 }, "InnerRadius", NULL, innerRadius, 0, 100);
            outerRadius = GuiSliderBar((Rectangle){ 600, 170, 120, 20 }, "OuterRadius", NULL, outerRadius, 0, 200);


            // play = GuiCheckBox((Rectangle){ 600, 320, 20, 20 }, "Play", play);

            // loopInfinito = GuiCheckBox((Rectangle){ 600, 350, 20, 20 }, "Musica Continua", loopInfinito);
            // originalFunction = GuiCheckBox((Rectangle){ 600, 380, 20, 20 }, "Sin Original", originalFunction);
            //------------------------------------------------------------------------------

            

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    free(data);                 // Unload sine wave data
    free(writeBuf);             // Unload write buffer

    UnloadAudioStream(stream);   // Close raw audio stream and delete buffers from RAM
    CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}