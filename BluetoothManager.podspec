Pod::Spec.new do |s|
  s.name         = "BluetoothManager"
  s.version      = "1.0.0"
  s.summary      = "BluetoothManager"
  s.homepage     = "BluetoothManager"
  s.license      = "MIT"
  s.authors      = "BluetoothManager"
  s.source       = { :path => "." }

  s.platforms = { :ios => "11.0", :osx => "10.9", :watchos => "2.0", :tvos => "9.0" }

  s.vendored_frameworks = "BluetoothManagerFramework.xcframework"

  s.swift_version = '6'

  s.source_files = [
    "ios/**/*.{h,m,mm,swift}",
  ]

  s.xcconfig = {
    "ENABLE_USER_SCRIPT_SANDBOXING" => "NO",
    "CLANG_CXX_LANGUAGE_STANDARD" => "c++20",
    "DEFINES_MODULE" => "YES",
    "CLANG_CXX_LIBRARY" => "libc++",
    "OTHER_CFLAGS" => "$(inherited)",
    "LD_RUNPATH_SEARCH_PATHS" => "$(inherited)"
  }

  s.pod_target_xcconfig = {
    "SWIFT_OBJC_INTEROP_MODE" => "objcxx"
  }

  s.requires_arc  = true

  s.libraries = "c++"
end
