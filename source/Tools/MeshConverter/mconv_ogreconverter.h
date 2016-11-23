

#ifndef __MCONV_OGRE_CONVERTER_H__
#define __MCONV_OGRE_CONVERTER_H__


#include "mconv_converter.h"


namespace mconv
{
    class OgreConverter : public ConverterImpl
    {
    public:
        OgreConverter(const Settings &settings);
        virtual ~OgreConverter();

        virtual bool convert() override;

    protected:
    };
}


#endif  /*__MCONV_OGRE_CONVERTER_H__*/
