
#include <iostream>

#include <generelle/modelling.hpp>
#include <generelle/visualization.hpp>

#include <OpenImageIO/imageio.h>

namespace gn = generelle;

void drawModel(const gn::GE model, int width, int height, const std::string& file_name) {

    unsigned char* ch = gn::Visualization::visualize(model, width, height);

    std::unique_ptr<OIIO::ImageOutput> outfile = OIIO::ImageOutput::create(file_name);
    if (!outfile) {
        std::cerr << "Cannot open output file " << file_name << ", exiting" << std::endl;
        exit(-1);
    }

    OIIO::ImageSpec spec(width, height, 4, OIIO::TypeDesc::UINT8);
    outfile->open(file_name, spec);
    outfile->write_image(OIIO::TypeDesc::UINT8, ch);
    outfile->close();

    gn::Visualization::destroyBuffer(ch);

    std::cout << "Wrote image to " << file_name << std::endl;
}

int main() {
    const int width = 800;
    const int height = 600;

    /* gn::GE ge1 = gn::makeSphere(falg::Vec3(0.0f, -1.0f, 0.0f), 1.5f);
    gn::GE ge2 = gn::makeSphere(falg::Vec3(0.0f, 1.0f, 0.0f), 1.5f);

    gn::GE sum = ge1.add(ge2); */

    gn::GE box = gn::makeBox(falg::Vec3(2.0f, 1.0f, 1.0f));
    gn::GE sphere = gn::makeSphere(0.5f).translate(falg::Vec3(0.0f, 1.5f, 0.0f));

    gn::GE sum = box.smoothAdd(sphere, 0.5f).pad(1.0f).subtract(gn::makeSphere(2.f).translate(falg::Vec3(0.5f, 1.5f, 0.0f)));

    // gn::GE tot = sum.scale(0.6f);

    // gn::GE tot = gn::makeSphere(0.5f).scale(0.9f).translate(falg::Vec3(0.0f, 0.0f, 0.0f));
    // gn::GE tot2 = gn::makeSphere(0.5f).scale(0.9f).translate(falg::Vec3(0.0f, 1.0f, 2.0f));

    gn::GE tot = gn::makeBox(falg::Vec3(1.0f, 1.0f, 1.0f)).scale(1.0f);
    gn::GE tot2 = gn::makeBox(falg::Vec3(1.0f, 1.0f, 1.0f)).scale(0.8f);

    drawModel(tot, width, height, "out.png");
    drawModel(tot2, width, height, "out2.png");
}
