
#include <iostream>

#include <generelle/modelling.hpp>
#include <generelle/visualization.hpp>

#include <OpenImageIO/imageio.h>

namespace gn = generelle;

int main() {
    const int width = 800;
    const int height = 600;

    gn::GE ge1 = gn::makeSphere(falg::Vec3(0.0f, -1.0f, 0.0f), 1.5f);
    gn::GE ge2 = gn::makeSphere(falg::Vec3(0.0f, 1.0f, 0.0f), 1.5f);

    gn::GE sum = ge1.add(ge2);

    unsigned char* ch = gn::Visualization::visualize(sum, width, height);

    std::string output_file_name = "out2.png";
    
    OIIO::ImageOutput *outfile = OIIO::ImageOutput::create(output_file_name);
    if (!outfile) {
        std::cerr << "Cannot open output file " << output_file_name << ", exiting" << std::endl;
        exit(-1);
    }

    OIIO::ImageSpec spec(width, height, 4, OIIO::TypeDesc::UINT8);
    outfile->open(output_file_name, spec);
    outfile->write_image(OIIO::TypeDesc::UINT8, ch);
    outfile->close();
    
    gn::Visualization::destroyBuffer(ch);

    std::cout << "Wrote image to " << output_file_name << std::endl;
}
