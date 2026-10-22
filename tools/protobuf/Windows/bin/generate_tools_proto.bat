@rem generate ImageConverter proto buffer source files.
@set input_path=..\..\proto\tools
@set output_path=..\..\source
@set target_dir=..\..\..\..\source\Tools\Common\ImgconvIPC\proto\

if not exist "%output_path%" mkdir "%output_path%"
if not exist "%target_dir%" mkdir "%target_dir%"

"%~dp0protoc.exe" --proto_path="%input_path%" --cpp_out="%output_path%" ImageConverter.proto

@xcopy /Y "%output_path%\ImageConverter.pb.*" "%target_dir%"
