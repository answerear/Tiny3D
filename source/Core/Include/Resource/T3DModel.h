

#ifndef __T3D_MODEL_H__
#define __T3D_MODEL_H__


#include "Resource/T3DResource.h"
#include "Render/T3DHardwareVertexBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Model : public Resource
    {
        friend class XMLModelSerializer;
        friend class BinModelSerializer;

    public:
        enum FileType
        {
            E_FILETYPE_UNKNOWN = 0,     /// 未知模型文件格式
            E_FILETYPE_T3B,             /// 二进制模型文件格式
            E_FILETYPE_T3T              /// XML模型文件格式
        };

        /**
         * @brief 创建模型对象实例.
         */
        static ModelPtr create(const String &name);

        /**
         * @brief Destructor
         */
        virtual ~Model();

        /**
         * @brief 获取资源类型枚举，从Resource继承下来的方法.
         */
        virtual Type getType() const override;

        /**
         * @brief 获取模型相关所有数据.
         */
        const ObjectPtr &getModelData() const
        {
            return mModelData;
        }

    protected:
        Model(const String &name);

        /**
         * @brief 加载模型资源，从Resource继承下来的方法.
         */
        virtual bool load() override;

        /**
         * @brief 卸载模型资源，从Resource继承下来的方法.
         */
        virtual void unload() override;
        
        /**
         * @brief 克隆一份模型资源，这里的克隆是做深拷贝，模型数据完全是另外一份.
         */
        virtual ResourcePtr clone() const override;

        /**
         * @brief 根据模型扩展名，获取对应的模型文件格式枚举值.
         */
        FileType parseFileType(const String &name) const;

    protected:
        ObjectPtr           mModelData;         /// 模型相关数据
    };
}

#endif  /*__T3D_MODEL_H__*/
