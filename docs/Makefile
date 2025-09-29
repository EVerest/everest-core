# Makefile for Sphinx documentation
#

# You can set these variables from the command line.
SPHINXOPTS    ?=
SPHINXBUILD   ?= $(PYTHON) -m sphinx.cmd.build
SPHINXAUTOBUILD ?= $(PYTHON) -m sphinx_autobuild
SOURCEDIR     = source
BUILDDIR      = _build

# Path to the requirements file
REQUIREMENTS  = requirements.txt
PYTHON        = python3

# Put it first so that "make" without an argument is like "make help".
help:
	@echo "Please use \`make <target>' where <target> is one of"
	@echo "  setup      to install the required Python dependencies"
	@echo "  html       to make standalone HTML files"
	@echo "  livehtml   to build and automatically reload the page on changes"
	@echo "  clean      to remove all build files"

.PHONY: help html livehtml clean

# Build the HTML documentation
html: setup
	@$(SPHINXBUILD) -b html "$(SOURCEDIR)" "$(BUILDDIR)/html" $(SPHINXOPTS)
	@echo
	@echo "Build finished. The HTML pages are in $(BUILDDIR)/html."

# Run the live-reloading server for development
livehtml: setup
	@$(SPHINXAUTOBUILD) "$(SOURCEDIR)" "$(BUILDDIR)/html" $(SPHINXOPTS)

# Remove the build directory
clean:
	@rm -rf "$(BUILDDIR)"
	@echo "Clean finished."

# Install Python dependencies
setup:
	@echo "Checking and installing Python dependencies..."
	@$(PYTHON) -m pip install -r $(REQUIREMENTS)
	@echo "Dependencies are up to date."

.PHONY: help setup html livehtml clean