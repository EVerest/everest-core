# for code coverage (ensure gcovr, lcov and genhtml are installed):
#   meson setup build -Db_coverage=true
#   meson test -C build
#
#   ninja test -C build
#   ninja coverage-html -C build
# coverage report: build/meson-logs/coveragereport/index.html
#
# memcheck/valgrind
# meson test --wrap='valgrind --leak-check=full --error-exitcode=1' -C build
# or
# valgrind build/wifisetuptest

# for googletest (as root)
#   apt install googletest
#   cd /usr/src/googletest
#   mkdir build
#   cd build
#   cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_SHARED_LIBS=ON -DBUILD_GTEST=ON -DBUILD_GMOCK=ON -DINSTALL_GTEST=ON -DINSTALL_GMOCK=ON ../
#   make
#   make install
#   cd ..
#   rm -rf build

project('Setup_unittests', 'cpp')
gtestdep = dependency('gtest_main', version : '>=1.10.0')

wifisetup_src = ['WiFiSetup.cpp']
wifisetup_testsrc = ['test/RunApplicationStub.cpp', 'test/WiFiSetupTest.cpp']

wifisetuptest_exe = executable('wifisetuptest', wifisetup_src + wifisetup_testsrc, 
    include_directories : 'test',
    dependencies : gtestdep
)

test('wifisetup tests', wifisetuptest_exe)
