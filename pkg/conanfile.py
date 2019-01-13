from conans import ConanFile, CMake, tools


class LibswocConan(ConanFile):
    name = "libswoc++"
    version = "1.0.3"
    license = "Apache License 2.0"
    author = "Alan M. Carroll <amc@apache.org>"
    url = "https://github.com/solidwallofcode/libswoc/pkg/conanfile.py"
    description = "Solid Wall of C++"
    topics = ("c++", "formatting", "memory")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=False"
    generators = "cmake"

    def source(self):
        self.run("git clone https://github.com/solidwallofcode/libswoc.git")
        self.run("cd libswoc && git checkout 1.0.3")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="libswoc")
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include/swoc", src="include/swoc")
        self.copy("*swoc++.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["swoc++"]

