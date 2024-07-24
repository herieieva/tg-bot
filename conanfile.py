from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.build import check_min_cppstd


class ExampleRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "tgbot/[>=1.7.2]", "boost/[>=1.83.0]"
    build_policy = "missing"

    def validate(self):
        check_min_cppstd(self, "20")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        CMakeToolchain(self).generate()
        CMakeDeps(self).generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
