#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <array>
#include <memory>
#include <vector>

using namespace std;

bool is_image_file(std::string input)
{
    std::string extn = input.substr(input.find_last_of("."));
    if(
        extn == ".bmp" ||
        extn == ".png" ||
        extn == ".jpg" ||
        extn == ".jpeg" ||
        extn == ".gif" 
    )
    {
        return true;
    }
    return false;
}

std::string run_command(const std::string& command)
{
    std::array<char, 12800000> buffer;
    std::string output;
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()))
    {
        output += buffer.data();
    }
    return output;
}

int main(int argc, char* argv [])
{
    int width = 3600; //to be set configurable
    int height = 2000; //to be set configurable
    int printer_dpi = 1200; //to be set configurable
    float lpi = atof(argv[1]);
    float ppl = (float)printer_dpi/(float)lpi;
    int frames = atoi(argv[2]);
    float white_ppl = ppl/frames;
    std::cout<<"white_ppl : "<<white_ppl<<" ppl : "<<ppl<<"\n";

    int white_line_offset = atoi(argv[3]); //should be <= 0
    string output_prefix = argv[4];

    //store the images now
    int in_frames = 0;
    int arg_offset = 5;
    std::string input = argv[arg_offset + in_frames];
    std::vector<string> input_raws;
    while(arg_offset + in_frames < argc) 
    {
        
        if(is_image_file(input)) {
            auto input_raw_name = (input+"_"+to_string(width)+"x"+to_string(height)+".raw");
            std::string cmd = std::string("python3 -c \"import numpy as np; import cv2; x = cv2.imread(\'"+(input)+"\'); x = cv2.resize(x, ("+to_string(width)+","+to_string(height)+"),interpolation=cv2.INTER_CUBIC); cv2.convertScaleAbs(x).tofile(\'"+input_raw_name+"\')\"");
            std::cout<<cmd<<"\n";
            if(system(cmd.c_str()))
            {
                std::cout<<"Incorrect image / couldn't resize\n";
                exit(-1);
            }
            // std::cout<<output<<"\n";
            // if(output.find("Error") == output.size())
            // {
            //     std::cout<<"Incorrect image / couldn't resize\n";
            //     exit(-1);
            // }
            input_raws.push_back(input_raw_name);
            std::cout<<"input file processed : "<<input_raws[input_raws.size()-1]<<"\n";
            in_frames++;
            if(arg_offset + in_frames >= argc) break;
            input = argv[arg_offset + in_frames];
        }
    }
    unsigned char* image = new unsigned char[width*height*3]();
    
    unsigned char* frame_image = new unsigned char[width*height*3]();
    std::cout<<"allocated\n";
    for(int f = 0; f < frames ; f++)
    {
        std::cout<<"reading\n";
        FILE*fp = fopen(input_raws[f].c_str(), "r");
        if(fp) 
        {
            fread(frame_image, sizeof(uint8_t)*width*3, height, fp);
            fclose(fp);
        }
        else 
        {
            std::cout<<"file not present : "<<input_raws[f]<<"\n";
            exit(-1);
        }
        std::cout<<"read\n";
        for(int j=0 ; j < height; j++)
        {
            white_line_offset = atoi(argv[3]) + f*white_ppl;
            for(int i=0; i < width; i++)
            {
                if(i >= white_line_offset && i < int(white_ppl+(float)white_line_offset))
                {
                    image[(j*width+i)*3+0] = frame_image[(j*width+i)*3+0];
                    image[(j*width+i)*3+1] = frame_image[(j*width+i)*3+1];
                    image[(j*width+i)*3+2] = frame_image[(j*width+i)*3+2];
                }
                else
                {
                    if(i>=white_line_offset) white_line_offset = round((float)white_line_offset+ppl);
                }
            }
        }

    }
    white_line_offset = atoi(argv[3]);
    std::string fname = output_prefix+std::to_string(lpi)+"_"+std::to_string(frames)+"_"+std::to_string(white_line_offset);
    FILE*fp = fopen((fname+"_"+std::to_string(width)+"x"+std::to_string(height)+".raw").c_str(), "w");
    fwrite(image, sizeof(char) * width*3, height, fp);
    fclose(fp);
    std::string cmd = std::string("python3 -c \"import numpy as np; import cv2; cv2.imwrite(\'"+(fname+".png")+"\', "+"np.fromfile(\'"+(fname+"_"+std::to_string(width)+"x"+std::to_string(height)+".raw")+"\', dtype=np.uint8).reshape(("+std::to_string(height)+", "+std::to_string(width)+",3)))\"");
    system(cmd.c_str());
    delete [] frame_image;
    delete [] image;
    return 0;
}