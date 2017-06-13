

#include "SG/Visual/T3DSGIndicator.h"
#include "SG/Renderable/T3DSGAxis.h"
#include "Bound/T3DAabbBound.h"


namespace Tiny3D
{
    SGIndicatorPtr SGIndicator::create(Real fAxisX, Real fAxisY, Real fAxisZ, uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGIndicatorPtr indicator = new SGIndicator();

        if (indicator != nullptr && indicator->init(fAxisX, fAxisY, fAxisZ))
        {
            indicator->release();
        }
        else
        {
            T3D_SAFE_RELEASE(indicator);
        }

        return indicator;
    }

    SGIndicator::SGIndicator(uint32_t uID /* = E_NID_AUTOMATIC */)
    {

    }

    SGIndicator::~SGIndicator()
    {

    }

    Node::Type SGIndicator::getNodeType() const
    {
        return E_NT_INDICATOR;
    }

    NodePtr SGIndicator::clone() const
    {
        SGIndicatorPtr indicator = new SGIndicator();

        if (indicator != nullptr)
        {
            cloneProperties(indicator);
            indicator->release();
        }

        return indicator;
    }

    bool SGIndicator::init(Real fAxisX, Real fAxisY, Real fAxisZ)
    {
        bool ret = false;

        SGAxisPtr axis = SGAxis::create(fAxisX, fAxisY, fAxisZ);
        if (axis != nullptr)
        {
            addChild(axis);
            ret = true;

//             AabbBoundPtr bound = AabbBound::create(getNodeID(), this);
//             if (bound != nullptr)
//             {
//                 bound->setParam(0, fAxisX, 0, fAxisY, 0, fAxisZ);
//                 mBound = bound;
//             }
        }

        return ret;
    }

    void SGIndicator::addChild(const NodePtr &node)
    {
        T3D_ASSERT(node->getNodeType() == Node::E_NT_AXIS);
        SGVisual::addChild(node);
    }
}