
#ifndef __MCONV_CONVERTER_H__
#define __MCONV_CONVERTER_H__


#include "mconv_prerequisites.h"
#include "mconv_settings.h"


namespace mconv
{
    class ConverterImpl;

    class Converter
    {
    public:
        Converter();
        virtual ~Converter();

        bool execute(int argc, char *argv[]);

    protected:
        ConverterImpl   *mConverter;
    };

    class ConverterImpl
    {
    public:
        ConverterImpl(const Settings &settings);
        virtual ~ConverterImpl();

        virtual bool convert() = 0;

    protected:
        const Settings    &mSettings;
    };
}


#endif  /*__MCONV_CONVERTER_H__*/
