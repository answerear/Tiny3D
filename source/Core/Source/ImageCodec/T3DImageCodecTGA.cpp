// 
// 
// #include "ImageCodec/T3DImageCodecTGA.h"
// 
// 
// namespace Tiny3D
// {
//     ImageCodecTGAPtr ImageCodecTGA::create()
//     {
//         ImageCodecTGAPtr codec = new ImageCodecTGA();
//         codec->release();
//         return codec;
//     }
// 
//     ImageCodecTGA::ImageCodecTGA()
//     {
// 
//     }
// 
//     ImageCodecTGA::~ImageCodecTGA()
//     {
// 
//     }
// 
//     ImageCodecBase::FileType ImageCodecTGA::getFileType() const
//     {
//         return E_TYPE_TGA;
//     }
// 
//     bool ImageCodecTGA::encode(const String &name, const Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecTGA::encode(DataStream &stream, const Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecTGA::encode(uint8_t *&data, size_t &size, const Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecTGA::decode(const String &name, Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecTGA::decode(DataStream &stream, Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecTGA::decode(const uint8_t *data, size_t size, Image &image)
//     {
//         return true;
//     }
// }
