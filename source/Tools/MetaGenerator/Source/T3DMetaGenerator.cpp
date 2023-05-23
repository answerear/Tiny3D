/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "T3DMetaGenerator.h"
#include "T3DGeneratorCommand.h"
#include "T3DGeneratorOptions.h"
//#include "FileScriptObject.pb.h"
//#include <google/protobuf/util/json_util.h>


namespace Tiny3D
{
    #define T3D_FILE_MAGIC              "T3D"
    #define T3D_FILE_VERSION_0_0_1      0x00000100
    #define T3D_FILE_CURRENT_VERSION    T3D_FILE_VERSION_0_0_1

    MetaGenerator::MetaGenerator()
    {

    }

    MetaGenerator::~MetaGenerator()
    {

    }

    TResult MetaGenerator::execute(int32_t argc, char* argv[])
    {
        TResult ret = T3D_OK;

        do 
        {
            GeneratorOptions opts;
            GeneratorCommand cmd;

            bool rt = cmd.parse(argc, argv, opts);
            if (!rt)
            {
                ret = T3D_ERR_FAIL;
                break;
            }

            ret = process(opts.mDir, opts.mRemove);
        } while (0);

        return ret;
    }

    TResult MetaGenerator::process(const String& path, bool isClear /* = false */)
    {
        TResult ret = T3D_OK;

        String fullpath = path + Dir::getNativeSeparator() + "*.*";
        Dir dir;
        bool working = dir.findFile(fullpath);

        while (working)
        {
            working = dir.findNextFile();

            if (dir.isDots())
            {
                // . or ..
                continue;
            }
            else if (dir.isDirectory())
            {
                // directory
                if (!isClear)
                {
                    const String &filepath = dir.getFilePath();
                    const String &name = dir.getFileName();
                    generate(filepath, name, true);
                }

                process(dir.getFilePath(), isClear);
            }
            else
            {
                // file
                const String &filepath = dir.getFilePath();
                const String &name = dir.getFileName();
                
                if (isClear)
                {
                    // delete *.meta
                    if (isMetaFile(name))
                    {
                        clear(filepath);
                    }
                }
                else
                {
                    // generate *.meta
                    if (!isMetaFile(name))
                    {
                        generate(filepath, name, false);
                    }
                    else
                    {
                        // 有元文件，如果没有资源文件，则删掉元文件
                        const String& title = dir.getFileTitle();
                        String filepath = dir.getRoot() + title;
                        if (!Dir::exists(filepath))
                        {
                            Dir::remove(dir.getFilePath());
                        }
                    }
                }
            }
        }

        dir.close();

        return ret;
    }

    bool MetaGenerator::isMetaFile(const String& name) const
    {
        bool ret = false;

        int32_t pos = name.find_last_of('.');

        if (pos != String::npos)
        {
            String ext = name.substr(pos);

            if (ext == ".meta")
            {
                ret = true;
            }
        }

        return ret;
    }

    void MetaGenerator::generate(const String &path, const String &name, bool isDir)
    {
        String fullpath = path + ".meta";

        if (!Dir::exists(fullpath))
        {
#if 1
            MetaPtr meta = Meta::create();
            meta->uuid = UUID::generate();
            // meta->uuid.TestObj->setValue(100);
            // meta->uuid.ArrayData.reserve(5);
            // for (size_t i = 0; i < 5; i++)
            // {
            //     meta->uuid.ArrayData.push_back(10+i*10);
            //     meta->uuid.ListData.push_back(1.0f+i*1.0f);
            //     UUID::Item item;
            //     item.key = (int32_t)i;
            //     item.value = (int32_t)i * 100;
            //     meta->uuid.ItemData.push_back(item);
            //     meta->uuid.MapData.insert(TMap<int,int>::value_type(i, 1000*i));
            //     UUID::Data data;
            //     data.ival = i * 100;
            //     data.fval = i * 10.0f;
            //     std::stringstream ss;
            //     ss << i;
            //     String key;
            //     key = ss.str();
            //     ss.str() = "";
            //     meta->uuid.MapItemData.insert(TMap<String, UUID::Data>::value_type(key, data));
            // }
            // meta->type = isDir ? Meta::FileType::kDir : Meta::FileType::kFile;

            if (isDir)
            {
                meta->type = Meta::FileType::kDir;
            }
            else
            {
                meta->type = Meta::FileType::kFile;
                // *.t3d
                    //
                // *.Shader
                // *.vs
                // *.ps
                // *.gs
                // *.hs
                // *.ds
                // *.cs
            }
            
            FileDataStream fs;

            if (fs.open(fullpath.c_str(), FileDataStream::E_MODE_TEXT | FileDataStream::E_MODE_WRITE_ONLY))
            {
                auto serializer = SerializerManager::create();
                serializer->setFileMode(SerializerManager::FileMode::kText);
                serializer->serialize(fs, meta);
                fs.close();
            }

            // meta = Meta::create();
            if (fs.open(fullpath.c_str(), FileDataStream::E_MODE_TEXT | FileDataStream::E_MODE_READ_ONLY))
            {
                auto serializer = SerializerManager::create();
                serializer->setFileMode(SerializerManager::FileMode::kText);
                meta = serializer->deserialize<Meta>(fs);
                fs.close();
            }
#else
            Script::FileFormat::FileMeta file;
            Script::FileFormat::FileHeader *header = file.mutable_header();
            header->set_magic(T3D_FILE_MAGIC);
            header->set_type(Script::FileFormat::FileHeader_FileType_Meta);
            header->set_version(T3D_FILE_CURRENT_VERSION);

            //String uuid = UUID::generate();
            UUID uuid = UUID::generate();
            String suid = uuid.toString();
            Script::MetaSystem::MetaData* meta = file.mutable_meta();
            meta->set_uuid(suid.c_str());
            meta->set_name(name.c_str());

            if (isDir)
            {
                meta->set_type(Script::MetaSystem::MetaData_FileType_Dir);
            }
            else
            {
                meta->set_type(Script::MetaSystem::MetaData_FileType_File);
            }

            FileDataStream fs;

            if (fs.open(fullpath.c_str(), FileDataStream::E_MODE_TEXT | FileDataStream::E_MODE_WRITE_ONLY))
            {
                String content;
                google::protobuf::util::JsonOptions opts;
                opts.add_whitespace = true;
                opts.always_print_enums_as_ints = false;
                opts.always_print_primitive_fields = true;
                google::protobuf::util::Status status = google::protobuf::util::MessageToJsonString(file, &content, opts);

                fs.write((void*)content.c_str(), content.length());
                fs.close();
            }
#endif
        }
    }

    void MetaGenerator::clear(const String& path)
    {
        if (Dir::exists(path))
        {
            Dir::remove(path);
        }
    }
}