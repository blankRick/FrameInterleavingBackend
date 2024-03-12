#include <iostream>
#include <cstdlib>
#include <cmath>

int main(int argc, char* argv [])
{
    int width = 2400;
    int height = 3600;
    unsigned char* image = new unsigned char[width*height*3];
    int printer_dpi = 1200;
    float lpi = atof(argv[1]);
    float ppl = (float)printer_dpi/(float)lpi;
    int frames = atoi(argv[2]);
    float white_ppl = ppl/frames;
    std::cout<<"white_ppl : "<<white_ppl<<" ppl : "<<ppl<<"\n";

    int white_line_offset = atoi(argv[3]); //should be <= 0
    for(int f = 0; f < frames ; f++)
    {
        for(int j=0 ; j < height; j++)
        {
            white_line_offset = atoi(argv[3]);
            for(int i=0; i < width; i++)
            {
                if(i >= white_line_offset && i < int(white_ppl+(float)white_line_offset))
                {
                    image[(j*width+i)*3+0] = 255;
                    image[(j*width+i)*3+1] = 255;
                    image[(j*width+i)*3+2] = 255;
                }
                else
                {
                    if(i>=white_line_offset) white_line_offset = round((float)white_line_offset+ppl);
                    image[(j*width + i)*3+0] = 0;
                    image[(j*width + i)*3+1] = 0;
                    image[(j*width + i)*3+2] = 0;
                }
            }
        }
        white_line_offset = atoi(argv[3]);

        std::string fname = std::string("pattern_")+std::to_string(f)+"_"+std::to_string(lpi)+"_"+std::to_string(frames)+"_"+std::to_string(white_line_offset);
        FILE*fp = fopen((fname+"_"+std::to_string(width)+"x"+std::to_string(height)+".raw").c_str(), "w");
        fwrite(image, sizeof(char) * width*3, height, fp);
        fclose(fp);
        std::string cmd = std::string("python3 -c \"import numpy as np; import cv2; cv2.imwrite(\'"+(fname+".png")+"\', "+"np.fromfile(\'"+(fname+"_"+std::to_string(width)+"x"+std::to_string(height)+".raw")+"\', dtype=np.uint8).reshape(("+std::to_string(height)+", "+std::to_string(width)+",3)))\"");
        std::cout<<cmd<<"\n";
        system(cmd.c_str());
    }
}