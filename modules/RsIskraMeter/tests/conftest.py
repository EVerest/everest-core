
def pytest_addoption(parser):
    parser.addoption(
        "--everest-prefix",
        action="store",
        default=".",
        help="everest prefix path",
    )
