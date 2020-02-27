@set input_file=MaterialScriptObject.proto
@set output_dir=..\..\..\..\source\step09\Tools\ScriptCompiler\protobuf\
del /Q %output_dir%
.\protoc.exe --proto_path="../../proto" --cpp_out="../../source" %input_file%
xcopy /Y ..\..\source\*.pb.* %output_dir%
pause