@rem generate proto buffer source files.
@set input_file=MaterialScriptObject.proto
.\protoc.exe --proto_path="../../proto" --cpp_out="../../source" %input_file%

@rem copy all generated files to target directory.
@set tools_dir=..\..\..\..\source\step09\Tools\ScriptCompiler\protobuf\
@set core_dir=..\..\..\..\source\step09\Core\protobuf\

@del /Q %tools_dir%
@xcopy /Y ..\..\source\*.pb.* %tools_dir%
@del /Q %core_dir%
@xcopy /Y ..\..\source\*.pb.* %core_dir%
@pause
