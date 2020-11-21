// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: FileScriptObject.proto

#include "FileScriptObject.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
extern PROTOBUF_INTERNAL_EXPORT_FileScriptObject_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_FileHeader_FileScriptObject_2eproto;
extern PROTOBUF_INTERNAL_EXPORT_MaterialScriptObject_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<8> scc_info_Material_MaterialScriptObject_2eproto;
extern PROTOBUF_INTERNAL_EXPORT_MetaScriptObject_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_MetaData_MetaScriptObject_2eproto;
namespace Tiny3D {
namespace Script {
namespace FileFormat {
class FileHeaderDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<FileHeader> _instance;
} _FileHeader_default_instance_;
class FileMaterialDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<FileMaterial> _instance;
} _FileMaterial_default_instance_;
class FileMetaDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<FileMeta> _instance;
} _FileMeta_default_instance_;
}  // namespace FileFormat
}  // namespace Script
}  // namespace Tiny3D
static void InitDefaultsscc_info_FileHeader_FileScriptObject_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::Tiny3D::Script::FileFormat::_FileHeader_default_instance_;
    new (ptr) ::Tiny3D::Script::FileFormat::FileHeader();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Tiny3D::Script::FileFormat::FileHeader::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_FileHeader_FileScriptObject_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_FileHeader_FileScriptObject_2eproto}, {}};

static void InitDefaultsscc_info_FileMaterial_FileScriptObject_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::Tiny3D::Script::FileFormat::_FileMaterial_default_instance_;
    new (ptr) ::Tiny3D::Script::FileFormat::FileMaterial();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Tiny3D::Script::FileFormat::FileMaterial::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<2> scc_info_FileMaterial_FileScriptObject_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 2, 0, InitDefaultsscc_info_FileMaterial_FileScriptObject_2eproto}, {
      &scc_info_FileHeader_FileScriptObject_2eproto.base,
      &scc_info_Material_MaterialScriptObject_2eproto.base,}};

static void InitDefaultsscc_info_FileMeta_FileScriptObject_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::Tiny3D::Script::FileFormat::_FileMeta_default_instance_;
    new (ptr) ::Tiny3D::Script::FileFormat::FileMeta();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Tiny3D::Script::FileFormat::FileMeta::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<2> scc_info_FileMeta_FileScriptObject_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 2, 0, InitDefaultsscc_info_FileMeta_FileScriptObject_2eproto}, {
      &scc_info_FileHeader_FileScriptObject_2eproto.base,
      &scc_info_MetaData_MetaScriptObject_2eproto.base,}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_FileScriptObject_2eproto[3];
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_FileScriptObject_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_FileScriptObject_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_FileScriptObject_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Tiny3D::Script::FileFormat::FileHeader, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Tiny3D::Script::FileFormat::FileHeader, magic_),
  PROTOBUF_FIELD_OFFSET(::Tiny3D::Script::FileFormat::FileHeader, type_),
  PROTOBUF_FIELD_OFFSET(::Tiny3D::Script::FileFormat::FileHeader, version_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Tiny3D::Script::FileFormat::FileMaterial, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Tiny3D::Script::FileFormat::FileMaterial, header_),
  PROTOBUF_FIELD_OFFSET(::Tiny3D::Script::FileFormat::FileMaterial, material_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Tiny3D::Script::FileFormat::FileMeta, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Tiny3D::Script::FileFormat::FileMeta, header_),
  PROTOBUF_FIELD_OFFSET(::Tiny3D::Script::FileFormat::FileMeta, meta_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::Tiny3D::Script::FileFormat::FileHeader)},
  { 8, -1, sizeof(::Tiny3D::Script::FileFormat::FileMaterial)},
  { 15, -1, sizeof(::Tiny3D::Script::FileFormat::FileMeta)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::Tiny3D::Script::FileFormat::_FileHeader_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::Tiny3D::Script::FileFormat::_FileMaterial_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::Tiny3D::Script::FileFormat::_FileMeta_default_instance_),
};

const char descriptor_table_protodef_FileScriptObject_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\026FileScriptObject.proto\022\030Tiny3D.Script."
  "FileFormat\032\030CommonScriptObject.proto\032\032Ma"
  "terialScriptObject.proto\032\027ModelScriptObj"
  "ect.proto\032\027SceneScriptObject.proto\032\026Meta"
  "ScriptObject.proto\"\243\001\n\nFileHeader\022\r\n\005mag"
  "ic\030\001 \001(\t\022;\n\004type\030\002 \001(\0162-.Tiny3D.Script.F"
  "ileFormat.FileHeader.FileType\022\017\n\007version"
  "\030\003 \001(\007\"8\n\010FileType\022\014\n\010Material\020\000\022\t\n\005Mode"
  "l\020\001\022\t\n\005Scene\020\002\022\010\n\004Meta\020\003\"~\n\014FileMaterial"
  "\0224\n\006header\030\001 \001(\0132$.Tiny3D.Script.FileFor"
  "mat.FileHeader\0228\n\010material\030\002 \001(\0132&.Tiny3"
  "D.Script.MaterialSystem.Material\"r\n\010File"
  "Meta\0224\n\006header\030\001 \001(\0132$.Tiny3D.Script.Fil"
  "eFormat.FileHeader\0220\n\004meta\030\002 \001(\0132\".Tiny3"
  "D.Script.MetaSystem.MetaDatab\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_FileScriptObject_2eproto_deps[5] = {
  &::descriptor_table_CommonScriptObject_2eproto,
  &::descriptor_table_MaterialScriptObject_2eproto,
  &::descriptor_table_MetaScriptObject_2eproto,
  &::descriptor_table_ModelScriptObject_2eproto,
  &::descriptor_table_SceneScriptObject_2eproto,
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_FileScriptObject_2eproto_sccs[3] = {
  &scc_info_FileHeader_FileScriptObject_2eproto.base,
  &scc_info_FileMaterial_FileScriptObject_2eproto.base,
  &scc_info_FileMeta_FileScriptObject_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_FileScriptObject_2eproto_once;
static bool descriptor_table_FileScriptObject_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_FileScriptObject_2eproto = {
  &descriptor_table_FileScriptObject_2eproto_initialized, descriptor_table_protodef_FileScriptObject_2eproto, "FileScriptObject.proto", 596,
  &descriptor_table_FileScriptObject_2eproto_once, descriptor_table_FileScriptObject_2eproto_sccs, descriptor_table_FileScriptObject_2eproto_deps, 3, 5,
  schemas, file_default_instances, TableStruct_FileScriptObject_2eproto::offsets,
  file_level_metadata_FileScriptObject_2eproto, 3, file_level_enum_descriptors_FileScriptObject_2eproto, file_level_service_descriptors_FileScriptObject_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_FileScriptObject_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_FileScriptObject_2eproto), true);
namespace Tiny3D {
namespace Script {
namespace FileFormat {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* FileHeader_FileType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_FileScriptObject_2eproto);
  return file_level_enum_descriptors_FileScriptObject_2eproto[0];
}
bool FileHeader_FileType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

#if (__cplusplus < 201703) && (!defined(_MSC_VER) || _MSC_VER >= 1900)
constexpr FileHeader_FileType FileHeader::Material;
constexpr FileHeader_FileType FileHeader::Model;
constexpr FileHeader_FileType FileHeader::Scene;
constexpr FileHeader_FileType FileHeader::Meta;
constexpr FileHeader_FileType FileHeader::FileType_MIN;
constexpr FileHeader_FileType FileHeader::FileType_MAX;
constexpr int FileHeader::FileType_ARRAYSIZE;
#endif  // (__cplusplus < 201703) && (!defined(_MSC_VER) || _MSC_VER >= 1900)

// ===================================================================

void FileHeader::InitAsDefaultInstance() {
}
class FileHeader::_Internal {
 public:
};

FileHeader::FileHeader()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Tiny3D.Script.FileFormat.FileHeader)
}
FileHeader::FileHeader(const FileHeader& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  magic_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_magic().empty()) {
    magic_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.magic_);
  }
  ::memcpy(&type_, &from.type_,
    static_cast<size_t>(reinterpret_cast<char*>(&version_) -
    reinterpret_cast<char*>(&type_)) + sizeof(version_));
  // @@protoc_insertion_point(copy_constructor:Tiny3D.Script.FileFormat.FileHeader)
}

void FileHeader::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_FileHeader_FileScriptObject_2eproto.base);
  magic_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(&type_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&version_) -
      reinterpret_cast<char*>(&type_)) + sizeof(version_));
}

FileHeader::~FileHeader() {
  // @@protoc_insertion_point(destructor:Tiny3D.Script.FileFormat.FileHeader)
  SharedDtor();
}

void FileHeader::SharedDtor() {
  magic_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void FileHeader::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const FileHeader& FileHeader::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_FileHeader_FileScriptObject_2eproto.base);
  return *internal_default_instance();
}


void FileHeader::Clear() {
// @@protoc_insertion_point(message_clear_start:Tiny3D.Script.FileFormat.FileHeader)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  magic_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(&type_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&version_) -
      reinterpret_cast<char*>(&type_)) + sizeof(version_));
  _internal_metadata_.Clear();
}

const char* FileHeader::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // string magic = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_magic();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "Tiny3D.Script.FileFormat.FileHeader.magic"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // .Tiny3D.Script.FileFormat.FileHeader.FileType type = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
          _internal_set_type(static_cast<::Tiny3D::Script::FileFormat::FileHeader_FileType>(val));
        } else goto handle_unusual;
        continue;
      // fixed32 version = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 29)) {
          version_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint32>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint32);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* FileHeader::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Tiny3D.Script.FileFormat.FileHeader)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string magic = 1;
  if (this->magic().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_magic().data(), static_cast<int>(this->_internal_magic().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "Tiny3D.Script.FileFormat.FileHeader.magic");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_magic(), target);
  }

  // .Tiny3D.Script.FileFormat.FileHeader.FileType type = 2;
  if (this->type() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      2, this->_internal_type(), target);
  }

  // fixed32 version = 3;
  if (this->version() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed32ToArray(3, this->_internal_version(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Tiny3D.Script.FileFormat.FileHeader)
  return target;
}

size_t FileHeader::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Tiny3D.Script.FileFormat.FileHeader)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string magic = 1;
  if (this->magic().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_magic());
  }

  // .Tiny3D.Script.FileFormat.FileHeader.FileType type = 2;
  if (this->type() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_type());
  }

  // fixed32 version = 3;
  if (this->version() != 0) {
    total_size += 1 + 4;
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void FileHeader::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Tiny3D.Script.FileFormat.FileHeader)
  GOOGLE_DCHECK_NE(&from, this);
  const FileHeader* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<FileHeader>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Tiny3D.Script.FileFormat.FileHeader)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Tiny3D.Script.FileFormat.FileHeader)
    MergeFrom(*source);
  }
}

void FileHeader::MergeFrom(const FileHeader& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Tiny3D.Script.FileFormat.FileHeader)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.magic().size() > 0) {

    magic_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.magic_);
  }
  if (from.type() != 0) {
    _internal_set_type(from._internal_type());
  }
  if (from.version() != 0) {
    _internal_set_version(from._internal_version());
  }
}

void FileHeader::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Tiny3D.Script.FileFormat.FileHeader)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void FileHeader::CopyFrom(const FileHeader& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Tiny3D.Script.FileFormat.FileHeader)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool FileHeader::IsInitialized() const {
  return true;
}

void FileHeader::InternalSwap(FileHeader* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  magic_.Swap(&other->magic_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(type_, other->type_);
  swap(version_, other->version_);
}

::PROTOBUF_NAMESPACE_ID::Metadata FileHeader::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void FileMaterial::InitAsDefaultInstance() {
  ::Tiny3D::Script::FileFormat::_FileMaterial_default_instance_._instance.get_mutable()->header_ = const_cast< ::Tiny3D::Script::FileFormat::FileHeader*>(
      ::Tiny3D::Script::FileFormat::FileHeader::internal_default_instance());
  ::Tiny3D::Script::FileFormat::_FileMaterial_default_instance_._instance.get_mutable()->material_ = const_cast< ::Tiny3D::Script::MaterialSystem::Material*>(
      ::Tiny3D::Script::MaterialSystem::Material::internal_default_instance());
}
class FileMaterial::_Internal {
 public:
  static const ::Tiny3D::Script::FileFormat::FileHeader& header(const FileMaterial* msg);
  static const ::Tiny3D::Script::MaterialSystem::Material& material(const FileMaterial* msg);
};

const ::Tiny3D::Script::FileFormat::FileHeader&
FileMaterial::_Internal::header(const FileMaterial* msg) {
  return *msg->header_;
}
const ::Tiny3D::Script::MaterialSystem::Material&
FileMaterial::_Internal::material(const FileMaterial* msg) {
  return *msg->material_;
}
void FileMaterial::clear_material() {
  if (GetArenaNoVirtual() == nullptr && material_ != nullptr) {
    delete material_;
  }
  material_ = nullptr;
}
FileMaterial::FileMaterial()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Tiny3D.Script.FileFormat.FileMaterial)
}
FileMaterial::FileMaterial(const FileMaterial& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  if (from._internal_has_header()) {
    header_ = new ::Tiny3D::Script::FileFormat::FileHeader(*from.header_);
  } else {
    header_ = nullptr;
  }
  if (from._internal_has_material()) {
    material_ = new ::Tiny3D::Script::MaterialSystem::Material(*from.material_);
  } else {
    material_ = nullptr;
  }
  // @@protoc_insertion_point(copy_constructor:Tiny3D.Script.FileFormat.FileMaterial)
}

void FileMaterial::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_FileMaterial_FileScriptObject_2eproto.base);
  ::memset(&header_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&material_) -
      reinterpret_cast<char*>(&header_)) + sizeof(material_));
}

FileMaterial::~FileMaterial() {
  // @@protoc_insertion_point(destructor:Tiny3D.Script.FileFormat.FileMaterial)
  SharedDtor();
}

void FileMaterial::SharedDtor() {
  if (this != internal_default_instance()) delete header_;
  if (this != internal_default_instance()) delete material_;
}

void FileMaterial::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const FileMaterial& FileMaterial::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_FileMaterial_FileScriptObject_2eproto.base);
  return *internal_default_instance();
}


void FileMaterial::Clear() {
// @@protoc_insertion_point(message_clear_start:Tiny3D.Script.FileFormat.FileMaterial)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  if (GetArenaNoVirtual() == nullptr && header_ != nullptr) {
    delete header_;
  }
  header_ = nullptr;
  if (GetArenaNoVirtual() == nullptr && material_ != nullptr) {
    delete material_;
  }
  material_ = nullptr;
  _internal_metadata_.Clear();
}

const char* FileMaterial::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // .Tiny3D.Script.FileFormat.FileHeader header = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          ptr = ctx->ParseMessage(_internal_mutable_header(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // .Tiny3D.Script.MaterialSystem.Material material = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          ptr = ctx->ParseMessage(_internal_mutable_material(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* FileMaterial::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Tiny3D.Script.FileFormat.FileMaterial)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // .Tiny3D.Script.FileFormat.FileHeader header = 1;
  if (this->has_header()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        1, _Internal::header(this), target, stream);
  }

  // .Tiny3D.Script.MaterialSystem.Material material = 2;
  if (this->has_material()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        2, _Internal::material(this), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Tiny3D.Script.FileFormat.FileMaterial)
  return target;
}

size_t FileMaterial::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Tiny3D.Script.FileFormat.FileMaterial)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // .Tiny3D.Script.FileFormat.FileHeader header = 1;
  if (this->has_header()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *header_);
  }

  // .Tiny3D.Script.MaterialSystem.Material material = 2;
  if (this->has_material()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *material_);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void FileMaterial::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Tiny3D.Script.FileFormat.FileMaterial)
  GOOGLE_DCHECK_NE(&from, this);
  const FileMaterial* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<FileMaterial>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Tiny3D.Script.FileFormat.FileMaterial)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Tiny3D.Script.FileFormat.FileMaterial)
    MergeFrom(*source);
  }
}

void FileMaterial::MergeFrom(const FileMaterial& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Tiny3D.Script.FileFormat.FileMaterial)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.has_header()) {
    _internal_mutable_header()->::Tiny3D::Script::FileFormat::FileHeader::MergeFrom(from._internal_header());
  }
  if (from.has_material()) {
    _internal_mutable_material()->::Tiny3D::Script::MaterialSystem::Material::MergeFrom(from._internal_material());
  }
}

void FileMaterial::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Tiny3D.Script.FileFormat.FileMaterial)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void FileMaterial::CopyFrom(const FileMaterial& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Tiny3D.Script.FileFormat.FileMaterial)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool FileMaterial::IsInitialized() const {
  return true;
}

void FileMaterial::InternalSwap(FileMaterial* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(header_, other->header_);
  swap(material_, other->material_);
}

::PROTOBUF_NAMESPACE_ID::Metadata FileMaterial::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void FileMeta::InitAsDefaultInstance() {
  ::Tiny3D::Script::FileFormat::_FileMeta_default_instance_._instance.get_mutable()->header_ = const_cast< ::Tiny3D::Script::FileFormat::FileHeader*>(
      ::Tiny3D::Script::FileFormat::FileHeader::internal_default_instance());
  ::Tiny3D::Script::FileFormat::_FileMeta_default_instance_._instance.get_mutable()->meta_ = const_cast< ::Tiny3D::Script::MetaSystem::MetaData*>(
      ::Tiny3D::Script::MetaSystem::MetaData::internal_default_instance());
}
class FileMeta::_Internal {
 public:
  static const ::Tiny3D::Script::FileFormat::FileHeader& header(const FileMeta* msg);
  static const ::Tiny3D::Script::MetaSystem::MetaData& meta(const FileMeta* msg);
};

const ::Tiny3D::Script::FileFormat::FileHeader&
FileMeta::_Internal::header(const FileMeta* msg) {
  return *msg->header_;
}
const ::Tiny3D::Script::MetaSystem::MetaData&
FileMeta::_Internal::meta(const FileMeta* msg) {
  return *msg->meta_;
}
void FileMeta::clear_meta() {
  if (GetArenaNoVirtual() == nullptr && meta_ != nullptr) {
    delete meta_;
  }
  meta_ = nullptr;
}
FileMeta::FileMeta()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Tiny3D.Script.FileFormat.FileMeta)
}
FileMeta::FileMeta(const FileMeta& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  if (from._internal_has_header()) {
    header_ = new ::Tiny3D::Script::FileFormat::FileHeader(*from.header_);
  } else {
    header_ = nullptr;
  }
  if (from._internal_has_meta()) {
    meta_ = new ::Tiny3D::Script::MetaSystem::MetaData(*from.meta_);
  } else {
    meta_ = nullptr;
  }
  // @@protoc_insertion_point(copy_constructor:Tiny3D.Script.FileFormat.FileMeta)
}

void FileMeta::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_FileMeta_FileScriptObject_2eproto.base);
  ::memset(&header_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&meta_) -
      reinterpret_cast<char*>(&header_)) + sizeof(meta_));
}

FileMeta::~FileMeta() {
  // @@protoc_insertion_point(destructor:Tiny3D.Script.FileFormat.FileMeta)
  SharedDtor();
}

void FileMeta::SharedDtor() {
  if (this != internal_default_instance()) delete header_;
  if (this != internal_default_instance()) delete meta_;
}

void FileMeta::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const FileMeta& FileMeta::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_FileMeta_FileScriptObject_2eproto.base);
  return *internal_default_instance();
}


void FileMeta::Clear() {
// @@protoc_insertion_point(message_clear_start:Tiny3D.Script.FileFormat.FileMeta)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  if (GetArenaNoVirtual() == nullptr && header_ != nullptr) {
    delete header_;
  }
  header_ = nullptr;
  if (GetArenaNoVirtual() == nullptr && meta_ != nullptr) {
    delete meta_;
  }
  meta_ = nullptr;
  _internal_metadata_.Clear();
}

const char* FileMeta::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // .Tiny3D.Script.FileFormat.FileHeader header = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          ptr = ctx->ParseMessage(_internal_mutable_header(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // .Tiny3D.Script.MetaSystem.MetaData meta = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          ptr = ctx->ParseMessage(_internal_mutable_meta(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* FileMeta::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Tiny3D.Script.FileFormat.FileMeta)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // .Tiny3D.Script.FileFormat.FileHeader header = 1;
  if (this->has_header()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        1, _Internal::header(this), target, stream);
  }

  // .Tiny3D.Script.MetaSystem.MetaData meta = 2;
  if (this->has_meta()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        2, _Internal::meta(this), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Tiny3D.Script.FileFormat.FileMeta)
  return target;
}

size_t FileMeta::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Tiny3D.Script.FileFormat.FileMeta)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // .Tiny3D.Script.FileFormat.FileHeader header = 1;
  if (this->has_header()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *header_);
  }

  // .Tiny3D.Script.MetaSystem.MetaData meta = 2;
  if (this->has_meta()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *meta_);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void FileMeta::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Tiny3D.Script.FileFormat.FileMeta)
  GOOGLE_DCHECK_NE(&from, this);
  const FileMeta* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<FileMeta>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Tiny3D.Script.FileFormat.FileMeta)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Tiny3D.Script.FileFormat.FileMeta)
    MergeFrom(*source);
  }
}

void FileMeta::MergeFrom(const FileMeta& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Tiny3D.Script.FileFormat.FileMeta)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.has_header()) {
    _internal_mutable_header()->::Tiny3D::Script::FileFormat::FileHeader::MergeFrom(from._internal_header());
  }
  if (from.has_meta()) {
    _internal_mutable_meta()->::Tiny3D::Script::MetaSystem::MetaData::MergeFrom(from._internal_meta());
  }
}

void FileMeta::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Tiny3D.Script.FileFormat.FileMeta)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void FileMeta::CopyFrom(const FileMeta& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Tiny3D.Script.FileFormat.FileMeta)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool FileMeta::IsInitialized() const {
  return true;
}

void FileMeta::InternalSwap(FileMeta* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(header_, other->header_);
  swap(meta_, other->meta_);
}

::PROTOBUF_NAMESPACE_ID::Metadata FileMeta::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace FileFormat
}  // namespace Script
}  // namespace Tiny3D
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Tiny3D::Script::FileFormat::FileHeader* Arena::CreateMaybeMessage< ::Tiny3D::Script::FileFormat::FileHeader >(Arena* arena) {
  return Arena::CreateInternal< ::Tiny3D::Script::FileFormat::FileHeader >(arena);
}
template<> PROTOBUF_NOINLINE ::Tiny3D::Script::FileFormat::FileMaterial* Arena::CreateMaybeMessage< ::Tiny3D::Script::FileFormat::FileMaterial >(Arena* arena) {
  return Arena::CreateInternal< ::Tiny3D::Script::FileFormat::FileMaterial >(arena);
}
template<> PROTOBUF_NOINLINE ::Tiny3D::Script::FileFormat::FileMeta* Arena::CreateMaybeMessage< ::Tiny3D::Script::FileFormat::FileMeta >(Arena* arena) {
  return Arena::CreateInternal< ::Tiny3D::Script::FileFormat::FileMeta >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
