/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#include "T3DScriptCompiler.h"
#include "T3DScriptLexer.h"
#include "T3DScriptParser.h"
#include "T3DScriptError.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ScriptCompiler::ScriptCompiler()
        : mLexer(nullptr)
        , mParser(nullptr)
    {
        mLexer = new ScriptLexer();
        mParser = new ScriptParser(this);

        initWordMap();
    }

    //--------------------------------------------------------------------------

    ScriptCompiler::~ScriptCompiler()
    {
        delete mLexer;
        mLexer = nullptr;

        delete mParser;
        mParser = nullptr;
    }

    //--------------------------------------------------------------------------

    bool ScriptCompiler::compile(int32_t argc, const char *argv[])
    {
        bool ret = false;

        do 
        {
            Options opt;
            ret = parse(argc, argv, opt);
            if (!ret)
            {
                printUsage();
                break;
            }

            if (opt.shouldPrintUsage())
            {
                printUsage();
                break;
            }
            else if (opt.shouldPrintVersion())
            {
                printVersion();
            }
            else if (opt.hasOutputFile() && !opt.needLink())
            {
                // 指定了链接文件名，但是没有链接，报错
                printUsage();
            }
            else
            {
                ret = compile(opt);
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    void ScriptCompiler::printUsage()
    {
        T3D_LOG_INFO(LOG_TAG, "Usage : ");
        T3D_LOG_INFO(LOG_TAG, "  scc input_files [options]");
        T3D_LOG_INFO(LOG_TAG, "    Options : ");
        T3D_LOG_INFO(LOG_TAG, "      -v : Print version.");
        T3D_LOG_INFO(LOG_TAG, "      -h : Print help.");
        T3D_LOG_INFO(LOG_TAG, "      -p : Set the directory of the project. If this options is set, all input files in the list will be relative path.");
        T3D_LOG_INFO(LOG_TAG, "      -o : Set the output file name (*.tsc). Default file name is the same as input file. And if \'-d\' is not set, the directory of output is the same as input file directory.");
        T3D_LOG_INFO(LOG_TAG, "      -d : Set the directory of output files. If \'-p\' is set, this path will be relative to the project path.");
        T3D_LOG_INFO(LOG_TAG, "      -l : Link all script binary file (*.tsc) to one file. Default is not linking.");
    }

    //--------------------------------------------------------------------------

    void ScriptCompiler::printVersion()
    {
        T3D_LOG_INFO(LOG_TAG, "Version : %s", CURRENT_VERSION_STR);
    }

    //--------------------------------------------------------------------------

    bool ScriptCompiler::parse(int32_t argc, const char *argv[], Options &opt)
    {
        bool ret = true;

        opt.options = 0;

        do 
        {
            if (argc == 0)
            {
                // 没参数，只能直接打印帮助，让用户知道如何使用
                opt.options = Options::OPT_PRINT_HELP;
                break;
            }

            int i = 0;

            while (i < argc)
            {
                if (stricmp(argv[i], "-h") == 0)
                {
                    // 打印帮助信息
                    opt.options = Options::OPT_PRINT_HELP;
                }
                else if (stricmp(argv[i], "-v") == 0)
                {
                    // 打印版本号
                    opt.options = Options::OPT_PRINT_VERSION;
                }
                else if (stricmp(argv[i], "-p") == 0)
                {
                    // 设置工程目录
                    opt.options |= Options::OPT_PROJECT_DIR;

                    if (argc - 1 == i)
                    {
                        // 参数不够
                        T3D_LOG_ERROR(LOG_TAG, "Missing project directory !");
                        ret = false;
                        break;
                    }

                    ++i;
                    opt.projDir = argv[i];
                }
                else if (stricmp(argv[i], "-o") == 0)
                {
                    // 设置输出文件名
                    opt.options |= Options::OPT_OUTPUT_FILE;

                    if (argc - 1 == i)
                    {
                        // 参数不够
                        T3D_LOG_ERROR(LOG_TAG, "Missing output file name (*.tsc) !");
                        ret = false;
                        break;
                    }

                    ++i;
                    opt.outFile = argv[i];
                }
                else if (stricmp(argv[i], "-d") == 0)
                {
                    // 设置输出文件夹
                    opt.options |= Options::OPT_OUTPUT_DIR;

                    if (argc - 1 == i)
                    {
                        // 参数不够
                        T3D_LOG_ERROR(LOG_TAG, "Missing output directory for link file !");
                        ret = false;
                        break;
                    }

                    ++i;
                    opt.outDir = argv[i];
                }
                else
                {
                    // 输入文件列表
                    opt.inFiles.push_back(argv[i]);
                }
            }

            if (!ret)
            {
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ScriptCompiler::compile(const Options &opt)
    {
        bool ret = false;

        do 
        {
            String outDir;

            auto itr = opt.inFiles.begin();

            for (itr = opt.inFiles.begin(); itr != opt.inFiles.end(); ++itr)
            {
                // 构造输入文件完整路径
                String input;
                if (opt.hasProjectDir())
                {
                    input = opt.projDir + "/" + *itr;
                }
                else
                {
                    input = *itr;
                }

                // 
                String path;
                String filename;
                getFileName(input, path, filename);
                String title;
                String ext;
                getFileTitle(filename, title, ext);

                String output;
                if (opt.hasOutputDir())
                {
                    output = opt.outDir + "/" + title + ".tsc";
                    outDir = opt.outDir;
                }
                else
                {
                    output = path + "/" + title + ".tsc";
                    outDir = path;
                }

                ret = compile(input, output);
                if (!ret)
                {
                    // 失败了
                    break;
                }
            }

            if (!ret)
            {
                // 编译失败了
                break;
            }

            // 链接
            if (opt.needLink())
            {
                String output;
                if (opt.hasOutputFile())
                {
                    output = outDir + "/" + opt.outFile;
                }
                else
                {
                    output = outDir + "/" + "a.tsc";
                }

                ret = link(outDir, output);
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ScriptCompiler::compile(const String &input, const String &output)
    {
        bool ret = false;

        do
        {
            String content;
            ret = readInputFile(content, input);
            if (!ret)
            {
                break;
            }

            // 词法分析
            TokenListPtr tokens = ScriptLexer::getInstance().tokenize(content, input);

            // 语法分析
            AbstractNodeListPtr ast(new AbstractNodeList());
            ret = ScriptParser::getInstance().parse(tokens, ast);

        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ScriptCompiler::link(const String &outDir, const String &output)
    {
        bool ret = false;

        do 
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ScriptCompiler::readInputFile(String &content, const String &input)
    {
        bool ret = false;

        FileDataStream fs;
        uint8_t *data = nullptr;

        do 
        {
            if (!Dir::exists(input))
            {
                T3D_LOG_ERROR("Compiler", "%s does not exist !", input.c_str());
                ret = false;
                break;
            }

            // 打开文件
            if (!fs.open(input.c_str(), FileDataStream::E_MODE_READ_ONLY))
            {
                T3D_LOG_ERROR("Compiler", "Open %s failed !", input.c_str());
                ret = false;
                break;
            }

            // 读取文件
            size_t size = (size_t)fs.size();
            data = new uint8_t[size];
            size_t bytesOfRead = fs.read(data, size);
            if (size != bytesOfRead)
            {
                T3D_LOG_ERROR("Compiler", "Read %s data failed !", input.c_str());
                ret = false;
                break;
            }

            fs.close();

            content.assign((const char *)data, size);

            delete[]data;
            data = nullptr;

            ret = true;
        } while (0);


        if (fs.isOpened())
        {
            fs.close();
        }

        if (data != nullptr)
        {
            delete[]data;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ScriptCompiler::processImports(AbstractNodeList &nodes)
    {
//         // We only need to iterate over the top-level of nodes
//         AbstractNodeList::iterator i = nodes.begin();
//         while (i != nodes.end())
//         {
//             // We move to the next node here and save the current one.
//             // If any replacement happens, then we are still assured that
//             // i points to the node *after* the replaced nodes, no matter
//             // how many insertions and deletions may happen
//             AbstractNodeList::iterator cur = i++;
//             if ((*cur)->type == ANT_IMPORT)
//             {
//                 ImportAbstractNode *import = (ImportAbstractNode*)(*cur).get();
//                 // Only process if the file's contents haven't been loaded
//                 if (mImports.find(import->source) == mImports.end())
//                 {
//                     // Load the script
//                     AbstractNodeListPtr importedNodes = loadImportPath(import->source);
//                     if (importedNodes && !importedNodes->empty())
//                     {
//                         processImports(*importedNodes);
//                         processObjects(*importedNodes, *importedNodes);
//                     }
//                     if (importedNodes && !importedNodes->empty())
//                         mImports.insert(std::make_pair(import->source, importedNodes));
//                 }
// 
//                 // Handle the target request now
//                 // If it is a '*' import we remove all previous requests and just use the '*'
//                 // Otherwise, ensure '*' isn't already registered and register our request
//                 if (import->target == "*")
//                 {
//                     mImportRequests.erase(mImportRequests.lower_bound(import->source),
//                         mImportRequests.upper_bound(import->source));
//                     mImportRequests.insert(std::make_pair(import->source, "*"));
//                 }
//                 else
//                 {
//                     ImportRequestMap::iterator iter = mImportRequests.lower_bound(import->source),
//                         end = mImportRequests.upper_bound(import->source);
//                     if (iter == end || iter->second != "*")
//                     {
//                         mImportRequests.insert(std::make_pair(import->source, import->target));
//                     }
//                 }
// 
//                 nodes.erase(cur);
//             }
//         }
// 
//         // All import nodes are removed
//         // We have cached the code blocks from all the imported scripts
//         // We can process all import requests now
//         for (ImportCacheMap::iterator it = mImports.begin(); it != mImports.end(); ++it)
//         {
//             ImportRequestMap::iterator j = mImportRequests.lower_bound(it->first),
//                 end = mImportRequests.upper_bound(it->first);
//             if (j != end)
//             {
//                 if (j->second == "*")
//                 {
//                     // Insert the entire AST into the import table
//                     mImportTable.insert(mImportTable.begin(), it->second->begin(), it->second->end());
//                     continue; // Skip ahead to the next file
//                 }
//                 else
//                 {
//                     for (; j != end; ++j)
//                     {
//                         // Locate this target and insert it into the import table
//                         AbstractNodeList newNodes = locateTarget(*it->second, j->second);
//                         if (!newNodes.empty())
//                             mImportTable.insert(mImportTable.begin(), newNodes.begin(), newNodes.end());
//                     }
//                 }
//             }
//         }

        return true;
    }

    //--------------------------------------------------------------------------

    AbstractNodeListPtr ScriptCompiler::loadImportPath(const String &name)
    {
        AbstractNodeListPtr retval;
        ConcreteNodeListPtr nodes;


//         nodes = mParser->parse(mLexer->tokenize(stream->getAsString(), name));

        return retval;
    }

    //--------------------------------------------------------------------------

    bool ScriptCompiler::isNameExcluded(const ObjectAbstractNode& node, AbstractNode* parent)
    {
        // Run past the listener
        bool excludeName = false;

        // Process the built-in name exclusions
        if (node.id == ID_EMITTER || node.id == ID_AFFECTOR)
        {
            // emitters or affectors inside a particle_system are excluded
            while (parent && parent->type == ANT_OBJECT)
            {
                ObjectAbstractNode *obj = static_cast<ObjectAbstractNode*>(parent);
                if (obj->id == ID_PARTICLE_SYSTEM)
                    return true;
                parent = obj->parent;
            }
        }
        else if (node.id == ID_PASS)
        {
            // passes inside compositors are excluded
            while (parent && parent->type == ANT_OBJECT)
            {
                ObjectAbstractNode *obj = static_cast<ObjectAbstractNode*>(parent);
                if (obj->id == ID_COMPOSITOR)
                    return true;
                parent = obj->parent;
            }
        }
        else if (node.id == ID_TEXTURE_SOURCE)
        {
            // Parent must be texture_unit
            while (parent && parent->type == ANT_OBJECT)
            {
                ObjectAbstractNode *obj = static_cast<ObjectAbstractNode*>(parent);
                if (obj->id == ID_TEXTURE_UNIT)
                    return true;
                parent = obj->parent;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------

    void ScriptCompiler::initWordMap()
    {
        mIds["on"] = ID_ON;
        mIds["off"] = ID_OFF;
        mIds["true"] = ID_TRUE;
        mIds["false"] = ID_FALSE;
        mIds["yes"] = ID_YES;
        mIds["no"] = ID_NO;

        // Material ids
        mIds["material"] = ID_MATERIAL;
        mIds["vertex_program"] = ID_VERTEX_PROGRAM;
        mIds["geometry_program"] = ID_GEOMETRY_PROGRAM;
        mIds["fragment_program"] = ID_FRAGMENT_PROGRAM;
        mIds["tessellation_hull_program"] = ID_TESSELLATION_HULL_PROGRAM;
        mIds["tessellation_domain_program"] = ID_TESSELLATION_DOMAIN_PROGRAM;
        mIds["compute_program"] = ID_COMPUTE_PROGRAM;
        mIds["technique"] = ID_TECHNIQUE;
        mIds["pass"] = ID_PASS;
        mIds["texture_unit"] = ID_TEXTURE_UNIT;
        mIds["vertex_program_ref"] = ID_VERTEX_PROGRAM_REF;
        mIds["geometry_program_ref"] = ID_GEOMETRY_PROGRAM_REF;
        mIds["fragment_program_ref"] = ID_FRAGMENT_PROGRAM_REF;
        mIds["tessellation_hull_program_ref"] = ID_TESSELLATION_HULL_PROGRAM_REF;
        mIds["tessellation_domain_program_ref"] = ID_TESSELLATION_DOMAIN_PROGRAM_REF;
        mIds["compute_program_ref"] = ID_COMPUTE_PROGRAM_REF;
        mIds["shadow_caster_vertex_program_ref"] = ID_SHADOW_CASTER_VERTEX_PROGRAM_REF;
        mIds["shadow_caster_fragment_program_ref"] = ID_SHADOW_CASTER_FRAGMENT_PROGRAM_REF;
        mIds["shadow_receiver_vertex_program_ref"] = ID_SHADOW_RECEIVER_VERTEX_PROGRAM_REF;
        mIds["shadow_receiver_fragment_program_ref"] = ID_SHADOW_RECEIVER_FRAGMENT_PROGRAM_REF;

        mIds["lod_values"] = ID_LOD_VALUES;
        mIds["lod_strategy"] = ID_LOD_STRATEGY;
        mIds["lod_distances"] = ID_LOD_DISTANCES;
        mIds["receive_shadows"] = ID_RECEIVE_SHADOWS;
        mIds["transparency_casts_shadows"] = ID_TRANSPARENCY_CASTS_SHADOWS;
        mIds["set_texture_alias"] = ID_SET_TEXTURE_ALIAS;

        mIds["source"] = ID_SOURCE;
        mIds["syntax"] = ID_SYNTAX;
        mIds["default_params"] = ID_DEFAULT_PARAMS;
        mIds["param_indexed"] = ID_PARAM_INDEXED;
        mIds["param_named"] = ID_PARAM_NAMED;
        mIds["param_indexed_auto"] = ID_PARAM_INDEXED_AUTO;
        mIds["param_named_auto"] = ID_PARAM_NAMED_AUTO;

        mIds["scheme"] = ID_SCHEME;
        mIds["lod_index"] = ID_LOD_INDEX;
        mIds["shadow_caster_material"] = ID_SHADOW_CASTER_MATERIAL;
        mIds["shadow_receiver_material"] = ID_SHADOW_RECEIVER_MATERIAL;
        mIds["gpu_vendor_rule"] = ID_GPU_VENDOR_RULE;
        mIds["gpu_device_rule"] = ID_GPU_DEVICE_RULE;
        mIds["include"] = ID_INCLUDE;
        mIds["exclude"] = ID_EXCLUDE;

        mIds["ambient"] = ID_AMBIENT;
        mIds["diffuse"] = ID_DIFFUSE;
        mIds["specular"] = ID_SPECULAR;
        mIds["emissive"] = ID_EMISSIVE;
        mIds["vertexcolour"] = ID_VERTEXCOLOUR;
        mIds["scene_blend"] = ID_SCENE_BLEND;
        mIds["colour_blend"] = ID_COLOUR_BLEND;
        mIds["one"] = ID_ONE;
        mIds["zero"] = ID_ZERO;
        mIds["dest_colour"] = ID_DEST_COLOUR;
        mIds["src_colour"] = ID_SRC_COLOUR;
        mIds["one_minus_src_colour"] = ID_ONE_MINUS_SRC_COLOUR;
        mIds["one_minus_dest_colour"] = ID_ONE_MINUS_DEST_COLOUR;
        mIds["dest_alpha"] = ID_DEST_ALPHA;
        mIds["src_alpha"] = ID_SRC_ALPHA;
        mIds["one_minus_dest_alpha"] = ID_ONE_MINUS_DEST_ALPHA;
        mIds["one_minus_src_alpha"] = ID_ONE_MINUS_SRC_ALPHA;
        mIds["separate_scene_blend"] = ID_SEPARATE_SCENE_BLEND;
        mIds["scene_blend_op"] = ID_SCENE_BLEND_OP;
        mIds["reverse_subtract"] = ID_REVERSE_SUBTRACT;
        mIds["min"] = ID_MIN;
        mIds["max"] = ID_MAX;
        mIds["separate_scene_blend_op"] = ID_SEPARATE_SCENE_BLEND_OP;
        mIds["depth_check"] = ID_DEPTH_CHECK;
        mIds["depth_write"] = ID_DEPTH_WRITE;
        mIds["depth_func"] = ID_DEPTH_FUNC;
        mIds["depth_bias"] = ID_DEPTH_BIAS;
        mIds["iteration_depth_bias"] = ID_ITERATION_DEPTH_BIAS;
        mIds["always_fail"] = ID_ALWAYS_FAIL;
        mIds["always_pass"] = ID_ALWAYS_PASS;
        mIds["less_equal"] = ID_LESS_EQUAL;
        mIds["less"] = ID_LESS;
        mIds["equal"] = ID_EQUAL;
        mIds["not_equal"] = ID_NOT_EQUAL;
        mIds["greater_equal"] = ID_GREATER_EQUAL;
        mIds["greater"] = ID_GREATER;
        mIds["alpha_rejection"] = ID_ALPHA_REJECTION;
        mIds["alpha_to_coverage"] = ID_ALPHA_TO_COVERAGE;
        mIds["light_scissor"] = ID_LIGHT_SCISSOR;
        mIds["light_clip_planes"] = ID_LIGHT_CLIP_PLANES;
        mIds["transparent_sorting"] = ID_TRANSPARENT_SORTING;
        mIds["illumination_stage"] = ID_ILLUMINATION_STAGE;
        mIds["decal"] = ID_DECAL;
        mIds["cull_hardware"] = ID_CULL_HARDWARE;
        mIds["clockwise"] = ID_CLOCKWISE;
        mIds["anticlockwise"] = ID_ANTICLOCKWISE;
        mIds["cull_software"] = ID_CULL_SOFTWARE;
        mIds["back"] = ID_BACK;
        mIds["front"] = ID_FRONT;
        mIds["normalise_normals"] = ID_NORMALISE_NORMALS;
        mIds["lighting"] = ID_LIGHTING;
        mIds["shading"] = ID_SHADING;
        mIds["flat"] = ID_FLAT;
        mIds["gouraud"] = ID_GOURAUD;
        mIds["phong"] = ID_PHONG;
        mIds["polygon_mode"] = ID_POLYGON_MODE;
        mIds["solid"] = ID_SOLID;
        mIds["wireframe"] = ID_WIREFRAME;
        mIds["points"] = ID_POINTS;
        mIds["polygon_mode_overrideable"] = ID_POLYGON_MODE_OVERRIDEABLE;
        mIds["fog_override"] = ID_FOG_OVERRIDE;
        mIds["none"] = ID_NONE;
        mIds["linear"] = ID_LINEAR;
        mIds["exp"] = ID_EXP;
        mIds["exp2"] = ID_EXP2;
        mIds["colour_write"] = ID_COLOUR_WRITE;
        mIds["max_lights"] = ID_MAX_LIGHTS;
        mIds["start_light"] = ID_START_LIGHT;
        mIds["iteration"] = ID_ITERATION;
        mIds["once"] = ID_ONCE;
        mIds["once_per_light"] = ID_ONCE_PER_LIGHT;
        mIds["per_n_lights"] = ID_PER_N_LIGHTS;
        mIds["per_light"] = ID_PER_LIGHT;
        mIds["point"] = ID_POINT;
        mIds["spot"] = ID_SPOT;
        mIds["directional"] = ID_DIRECTIONAL;
        mIds["light_mask"] = ID_LIGHT_MASK;
        mIds["point_size"] = ID_POINT_SIZE;
        mIds["point_sprites"] = ID_POINT_SPRITES;
        mIds["point_size_min"] = ID_POINT_SIZE_MIN;
        mIds["point_size_max"] = ID_POINT_SIZE_MAX;
        mIds["point_size_attenuation"] = ID_POINT_SIZE_ATTENUATION;

        mIds["texture_alias"] = ID_TEXTURE_ALIAS;
        mIds["texture"] = ID_TEXTURE;
        mIds["1d"] = ID_1D;
        mIds["2d"] = ID_2D;
        mIds["3d"] = ID_3D;
        mIds["cubic"] = ID_CUBIC;
        mIds["unlimited"] = ID_UNLIMITED;
        mIds["2darray"] = ID_2DARRAY;
        mIds["alpha"] = ID_ALPHA;
        mIds["gamma"] = ID_GAMMA;
        mIds["anim_texture"] = ID_ANIM_TEXTURE;
        mIds["cubic_texture"] = ID_CUBIC_TEXTURE;
        mIds["separateUV"] = ID_SEPARATE_UV;
        mIds["combinedUVW"] = ID_COMBINED_UVW;
        mIds["tex_coord_set"] = ID_TEX_COORD_SET;
        mIds["tex_address_mode"] = ID_TEX_ADDRESS_MODE;
        mIds["wrap"] = ID_WRAP;
        mIds["clamp"] = ID_CLAMP;
        mIds["mirror"] = ID_MIRROR;
        mIds["border"] = ID_BORDER;
        mIds["tex_border_colour"] = ID_TEX_BORDER_COLOUR;
        mIds["filtering"] = ID_FILTERING;
        mIds["bilinear"] = ID_BILINEAR;
        mIds["trilinear"] = ID_TRILINEAR;
        mIds["anisotropic"] = ID_ANISOTROPIC;
        mIds["compare_test"] = ID_CMPTEST;
        mIds["compare_func"] = ID_CMPFUNC;
        mIds["max_anisotropy"] = ID_MAX_ANISOTROPY;
        mIds["mipmap_bias"] = ID_MIPMAP_BIAS;
        mIds["colour_op"] = ID_COLOUR_OP;
        mIds["replace"] = ID_REPLACE;
        mIds["add"] = ID_ADD;
        mIds["modulate"] = ID_MODULATE;
        mIds["alpha_blend"] = ID_ALPHA_BLEND;
        mIds["colour_op_ex"] = ID_COLOUR_OP_EX;
        mIds["source1"] = ID_SOURCE1;
        mIds["source2"] = ID_SOURCE2;
        mIds["modulate"] = ID_MODULATE;
        mIds["modulate_x2"] = ID_MODULATE_X2;
        mIds["modulate_x4"] = ID_MODULATE_X4;
        mIds["add"] = ID_ADD;
        mIds["add_signed"] = ID_ADD_SIGNED;
        mIds["add_smooth"] = ID_ADD_SMOOTH;
        mIds["subtract"] = ID_SUBTRACT;
        mIds["blend_diffuse_alpha"] = ID_BLEND_DIFFUSE_ALPHA;
        mIds["blend_texture_alpha"] = ID_BLEND_TEXTURE_ALPHA;
        mIds["blend_current_alpha"] = ID_BLEND_CURRENT_ALPHA;
        mIds["blend_manual"] = ID_BLEND_MANUAL;
        mIds["dotproduct"] = ID_DOT_PRODUCT;
        mIds["blend_diffuse_colour"] = ID_BLEND_DIFFUSE_COLOUR;
        mIds["src_current"] = ID_SRC_CURRENT;
        mIds["src_texture"] = ID_SRC_TEXTURE;
        mIds["src_diffuse"] = ID_SRC_DIFFUSE;
        mIds["src_specular"] = ID_SRC_SPECULAR;
        mIds["src_manual"] = ID_SRC_MANUAL;
        mIds["colour_op_multipass_fallback"] = ID_COLOUR_OP_MULTIPASS_FALLBACK;
        mIds["alpha_op_ex"] = ID_ALPHA_OP_EX;
        mIds["env_map"] = ID_ENV_MAP;
        mIds["spherical"] = ID_SPHERICAL;
        mIds["planar"] = ID_PLANAR;
        mIds["cubic_reflection"] = ID_CUBIC_REFLECTION;
        mIds["cubic_normal"] = ID_CUBIC_NORMAL;
        mIds["scroll"] = ID_SCROLL;
        mIds["scroll_anim"] = ID_SCROLL_ANIM;
        mIds["rotate"] = ID_ROTATE;
        mIds["rotate_anim"] = ID_ROTATE_ANIM;
        mIds["scale"] = ID_SCALE;
        mIds["wave_xform"] = ID_WAVE_XFORM;
        mIds["scroll_x"] = ID_SCROLL_X;
        mIds["scroll_y"] = ID_SCROLL_Y;
        mIds["scale_x"] = ID_SCALE_X;
        mIds["scale_y"] = ID_SCALE_Y;
        mIds["sine"] = ID_SINE;
        mIds["triangle"] = ID_TRIANGLE;
        mIds["sawtooth"] = ID_SAWTOOTH;
        mIds["square"] = ID_SQUARE;
        mIds["inverse_sawtooth"] = ID_INVERSE_SAWTOOTH;
        mIds["transform"] = ID_TRANSFORM;
        mIds["binding_type"] = ID_BINDING_TYPE;
        mIds["vertex"] = ID_VERTEX;
        mIds["fragment"] = ID_FRAGMENT;
        mIds["geometry"] = ID_GEOMETRY;
        mIds["tessellation_hull"] = ID_TESSELLATION_HULL;
        mIds["tessellation_domain"] = ID_TESSELLATION_DOMAIN;
        mIds["compute"] = ID_COMPUTE;
        mIds["content_type"] = ID_CONTENT_TYPE;
        mIds["named"] = ID_NAMED;
        mIds["shadow"] = ID_SHADOW;
        mIds["texture_source"] = ID_TEXTURE_SOURCE;
        mIds["shared_params"] = ID_SHARED_PARAMS;
        mIds["shared_param_named"] = ID_SHARED_PARAM_NAMED;
        mIds["shared_params_ref"] = ID_SHARED_PARAMS_REF;

        // Particle system
        mIds["particle_system"] = ID_PARTICLE_SYSTEM;
        mIds["emitter"] = ID_EMITTER;
        mIds["affector"] = ID_AFFECTOR;

        // Compositor
        mIds["compositor"] = ID_COMPOSITOR;
        mIds["target"] = ID_TARGET;
        mIds["target_output"] = ID_TARGET_OUTPUT;

        mIds["input"] = ID_INPUT;
        mIds["none"] = ID_NONE;
        mIds["previous"] = ID_PREVIOUS;
        mIds["target_width"] = ID_TARGET_WIDTH;
        mIds["target_height"] = ID_TARGET_HEIGHT;
        mIds["target_width_scaled"] = ID_TARGET_WIDTH_SCALED;
        mIds["target_height_scaled"] = ID_TARGET_HEIGHT_SCALED;
        mIds["pooled"] = ID_POOLED;
        //mIds["gamma"] = ID_GAMMA; - already registered
        mIds["no_fsaa"] = ID_NO_FSAA;
        mIds["depth_pool"] = ID_DEPTH_POOL;

        mIds["texture_ref"] = ID_TEXTURE_REF;
        mIds["local_scope"] = ID_SCOPE_LOCAL;
        mIds["chain_scope"] = ID_SCOPE_CHAIN;
        mIds["global_scope"] = ID_SCOPE_GLOBAL;
        mIds["compositor_logic"] = ID_COMPOSITOR_LOGIC;

        mIds["only_initial"] = ID_ONLY_INITIAL;
        mIds["visibility_mask"] = ID_VISIBILITY_MASK;
        mIds["lod_bias"] = ID_LOD_BIAS;
        mIds["material_scheme"] = ID_MATERIAL_SCHEME;
        mIds["shadows"] = ID_SHADOWS_ENABLED;

        mIds["clear"] = ID_CLEAR;
        mIds["stencil"] = ID_STENCIL;
        mIds["render_scene"] = ID_RENDER_SCENE;
        mIds["render_quad"] = ID_RENDER_QUAD;
        mIds["identifier"] = ID_IDENTIFIER;
        mIds["first_render_queue"] = ID_FIRST_RENDER_QUEUE;
        mIds["last_render_queue"] = ID_LAST_RENDER_QUEUE;
        mIds["quad_normals"] = ID_QUAD_NORMALS;
        mIds["camera_far_corners_view_space"] = ID_CAMERA_FAR_CORNERS_VIEW_SPACE;
        mIds["camera_far_corners_world_space"] = ID_CAMERA_FAR_CORNERS_WORLD_SPACE;

        mIds["buffers"] = ID_BUFFERS;
        mIds["colour"] = ID_COLOUR;
        mIds["depth"] = ID_DEPTH;
        mIds["colour_value"] = ID_COLOUR_VALUE;
        mIds["depth_value"] = ID_DEPTH_VALUE;
        mIds["stencil_value"] = ID_STENCIL_VALUE;

        mIds["check"] = ID_CHECK;
        mIds["comp_func"] = ID_COMP_FUNC;
        mIds["ref_value"] = ID_REF_VALUE;
        mIds["mask"] = ID_MASK;
        mIds["fail_op"] = ID_FAIL_OP;
        mIds["keep"] = ID_KEEP;
        mIds["increment"] = ID_INCREMENT;
        mIds["decrement"] = ID_DECREMENT;
        mIds["increment_wrap"] = ID_INCREMENT_WRAP;
        mIds["decrement_wrap"] = ID_DECREMENT_WRAP;
        mIds["invert"] = ID_INVERT;
        mIds["depth_fail_op"] = ID_DEPTH_FAIL_OP;
        mIds["pass_op"] = ID_PASS_OP;
        mIds["two_sided"] = ID_TWO_SIDED;
        mIds["read_back_as_texture"] = ID_READ_BACK_AS_TEXTURE;

        mIds["subroutine"] = ID_SUBROUTINE;

        mIds["line_width"] = ID_LINE_WIDTH;
        mIds["sampler"] = ID_SAMPLER;
        mIds["sampler_ref"] = ID_SAMPLER_REF;
        mIds["thread_groups"] = ID_THREAD_GROUPS;
        mIds["render_custom"] = ID_RENDER_CUSTOM;
    }

    void ScriptCompiler::getFileName(const String &filepath, String &path, String &name) const
    {
        String::size_type pos = filepath.find_last_of('/');

        if (pos == String::npos)
        {
            pos = filepath.find_last_of('\\');
        }

        if (pos == String::npos)
        {
            name = filepath;
            path = "";
        }
        else
        {
            path = filepath.substr(0, pos);
            name = filepath.substr(pos);
        }
    }

    void ScriptCompiler::getFileTitle(const String &filename, String &title, String &ext) const
    {
        String::size_type pos = filename.find_last_of('.');

        if (pos == String::npos)
        {
            title = filename;
            ext = "";
        }
        else
        {
            title = filename.substr(0, pos);
            ext = filename.substr(pos);
        }
    }
}

