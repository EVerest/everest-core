## Unit tests with GTest
A series of unit tests checks the implemented business logic of the controller. 
The unit tests require GoogleTest (GTest). This can be installed via
```bash
apt install libgtest-dev
```
Please do not forget to compile GTest library.

To run the tests, please issue compiler switch -DBUILD_TESTING=ON which will
enable EVEREST_CORE_BUILD_TESTING on which these unit tests relate.
