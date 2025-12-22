# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import os
import sys
sys.path.append(os.path.abspath("./_ext"))

import yaml

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here.
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'EVerest Documentation'
copyright = '2021-2025, EVerest community'
author = 'EVerest community'

# The full version, including alpha/beta/rc tags, taken from CMakeLists.txt
release = '2025.9.0'
# The short X.Y version.
version = '.'.join(release.split('.')[:2])


# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'sphinx.ext.autodoc',      # Core library for autodoc
    'sphinx.ext.napoleon',     # Support for Google and NumPy style docstrings
    'sphinx.ext.viewcode',     # Add links to highlighted source code
    'sphinx_copybutton',       # Add a "copy" button to code blocks
    'sphinx_design',           # For advanced design elements like cards, grids
    'sphinx.ext.mathjax',      # For rendering math equations
    'staticpages.extension',   # For static pages like versions_index.html
]

templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = [
    '_build',
    'Thumbs.db',
    '.DS_Store',
    'venv', # Exclude the virtual environment directory
    'README.md',
]


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

# The theme to use for HTML and HTML Help pages.
# See the documentation for a list of builtin themes.
html_theme = 'furo'

# A shorter title for the navigation bar.
html_short_title = f"EVerest Manual {release}"

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

metadata_yaml_path = os.getenv('EVEREST_METADATA_YAML_PATH', "metadata_everest.yaml")
with open(metadata_yaml_path, 'r') as f:
    metadata = yaml.safe_load(f)

deployed_versions = metadata.get('versions', [])
versionsindex_page = {
    'pagename': 'versions_index',
    'urls_prefix': '/latest/',
    'template': 'versions_index.html',
    'context': {
        'versions': deployed_versions,
    },
}
notfound_page = {
    'pagename': '404',
    'urls_prefix': '/latest/',
}

staticpages_pages = [
    versionsindex_page,
    notfound_page,
]
staticpages_urls_prefix = '/latest/'
