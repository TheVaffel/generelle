#include <memory>

namespace generelle {
    class GeometricExpression;

    class InnerGeometricExpression;

    typedef GeometricExpression GE;
    typedef std::shared_ptr<InnerGeometricExpression> IGE;
};
