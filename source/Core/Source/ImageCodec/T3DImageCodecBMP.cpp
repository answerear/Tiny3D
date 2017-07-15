// 
// 
// #include "ImageCodec/T3DImageCodecBMP.h"
// 
// 
// namespace Tiny3D
// {
//     ImageCodecBMPPtr ImageCodecBMP::create()
//     {
//         ImageCodecBMPPtr codec = new ImageCodecBMP();
//         codec->release();
//         return codec;
//     }
// 
//     ImageCodecBMP::ImageCodecBMP()
//     {
// 
//     }
// 
//     ImageCodecBMP::~ImageCodecBMP()
//     {
// 
//     }
// 
//     ImageCodecBase::FileType ImageCodecBMP::getFileType() const
//     {
//         return E_TYPE_BMP;
//     }
// 
//     bool ImageCodecBMP::encode(const String &name, const Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecBMP::encode(DataStream &stream, const Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecBMP::encode(uint8_t *&data, size_t &size, const Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecBMP::decode(const String &name, Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecBMP::decode(DataStream &stream, Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecBMP::decode(const uint8_t *data, size_t size, Image &image)
//     {
//         return true;
//     }
// }
