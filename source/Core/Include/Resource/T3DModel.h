

#ifndef __T3D_MODEL_H__
#define __T3D_MODEL_H__


#include "Resource/T3DResource.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DRenderer.h"


namespace tinyxml2
{
    class XMLElement;
}


namespace Tiny3D
{
    class T3D_ENGINE_API Model : public Resource
    {
    public:
        typedef std::vector<ObjectPtr>              GeometryDataList;
        typedef GeometryDataList::iterator          GeometryDataListItr;
        typedef GeometryDataList::const_iterator    GeometryDataListConstItr;

        typedef std::vector<ObjectPtr>              SkinDataList;
        typedef SkinDataList::iterator              SkinDataListItr;
        typedef SkinDataList::const_iterator        SkinDataListConstItr;

        typedef std::map<String, ObjectPtr>         AnimationData;
        typedef AnimationData::iterator             AnimationItr;
        typedef AnimationData::const_iterator       AnimationConstItr;
        typedef std::pair<String, ObjectPtr>        AnimationValue;

        enum FileType
        {
            E_FILETYPE_UNKNOWN = 0,     /// 未知模型文件格式
            E_FILETYPE_TMB,             /// 二进制模型文件格式
            E_FILETYPE_TMT              /// XML模型文件格式
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
         * @brief 获取模型拥有的几何体数据列表.
         */
        const GeometryDataList &getGeometryDataList() const
        {
            return mGeometryData;
        }

        /**
         * @brief 获取模型的蒙皮信息数据列表.
         */
        const SkinDataList &getSkinData() const
        {
            return mSkinData;
        }

        /**
         * @brief 获取骨骼数据
         */
        const ObjectPtr &getSkeletonData() const
        {
            return mSkeletonData;
        }

        /**
         * @brief 获取动画数据
         */
        const AnimationData &getAnimationData() const
        {
            return mAnimationData;
        }

        /**
         * @brief 模型几何体数据是否共用顶点模式.
         */
        bool isSharedVertex() const
        {
            return mIsVertexShared;
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

        /**
         * @brief 从XML文件加载模型资源.
         * @param [in] stream : 模型资源输入内存数据流对象
         * @return 加载成功，返回true，否则返回false.
         */
        bool loadFromXML(MemoryDataStream &stream);

        /**
         * @brief 
         */
        size_t parseVertexValue(const String &text, size_t &start, const VertexElement &attribute, void *value);
        size_t parseIndexValue(const String &text, size_t &start, bool is16bits, void *value);

        VertexElement::Semantic parseVertexSemantic(const String &name);
        VertexElement::Type parseVertexType(VertexElement::Semantic semantic, const String &name, size_t valueCount);
        Renderer::PrimitiveType parsePrimitiveType(const String &name);

        bool parseMesh(tinyxml2::XMLElement *pMeshElement);
        bool parseVertexBuffers(tinyxml2::XMLElement *pBuffersElement, void *attributes, void *buffers);
        bool parseVertexBuffer(tinyxml2::XMLElement *pBufferElement, void *attribute, void *buffer, size_t index);

        bool parseSubMeshes(tinyxml2::XMLElement *pMeshElement, void *attributes, void *buffers);
        bool parseSubMesh(tinyxml2::XMLElement *pSubMeshElement, void *attributes, void *buffers);

        bool parseSkin(tinyxml2::XMLElement *pSkinElement);
        bool parseBone(tinyxml2::XMLElement *pBoneElement, const ObjectPtr &bone);

        bool parseSkeleton(tinyxml2::XMLElement *pSkelElement);
        bool parseSkeleton(tinyxml2::XMLElement *pBoneElement, const ObjectPtr &parent);

        bool parseAnimation(tinyxml2::XMLElement *pAnimElement);
        bool parseAction(tinyxml2::XMLElement *pActionElement);
        bool parseKeyframe(tinyxml2::XMLElement *pKeyframeElement, const String &actionType, const String &boneName, size_t frameCount, const ObjectPtr &actionData);

        int32_t parseActionType(const String &type);

        bool loadFromBinary(DataStream &stream);

    protected:
        bool                mIsVertexShared;    /// 是否共享顶点数据
        GeometryDataList    mGeometryData;      /// 渲染几何数据
        SkinDataList        mSkinData;          /// 蒙皮数据
        ObjectPtr           mSkeletonData;      /// 骨骼数据
        AnimationData       mAnimationData;     /// 动画数据
    };
}

#endif  /*__T3D_MODEL_H__*/
