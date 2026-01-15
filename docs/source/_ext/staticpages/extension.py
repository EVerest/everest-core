import docutils
import os
import sphinx
import warnings

from sphinx.environment.collectors import EnvironmentCollector
from sphinx.errors import ExtensionError

from . import __version__
from .utils import replace_uris


class BaseURIError(ExtensionError):
    """Exception for malformed base URI."""
    pass


# https://www.sphinx-doc.org/en/stable/extdev/appapi.html#event-html-collect-pages
def html_collect_pages(app):
    """
    Create for each ``<pagename>`` a html page.

    Uses ``<template>`` as a template to be rendered with
    ``<context>`` for its context. The resulting file generated is
    ``<pagename>.html``.

    If the user already defined a page with pagename title
    ``<pagename>``, we don't generate this page.

    :param app: Sphinx Application
    :type app: sphinx.application.Sphinx
    """
    result = []
    for page in app.config.staticpages_pages:
        if page['pagename'] in app.env.titles:
            # There is already a ``<pagename>.rst`` file rendered.
            # Skip generating our default one.
            continue
        result.append((
            page['pagename'],
            page['context'],
            page['template'],
        ))

    return result


# https://www.sphinx-doc.org/en/stable/extdev/appapi.html#event-html-page-context
def finalize_media(app, pagename, templatename, context, doctree):
    """
    Point media files at our media server.

    Generate absolute URLs for resources (js, images, css, etc) to point to the
    right URL. For example, if a URL in the page is ``_static/js/custom.js`` it will
    be replaced by ``<urls_prefix>/_static/js/custom.js``.

    Also, all the links from the sidebar (toctree) are replaced with their
    absolute version. For example, ``../section/pagename.html`` will be replaced
    by ``/section/pagename.html``.

    It handles a special case for Read the Docs and URLs starting with ``/_/``.
    These URLs have a special meaning under Read the Docs and don't have to be changed.
    (e.g. ``/_/static/javascript/readthedocs-doc-embed.js``)

    :param app: Sphinx Application
    :type app: sphinx.application.Sphinx

    :param pagename: name of the page being rendered
    :type pagename: str

    :param templatename: template used to render the page
    :type templatename: str

    :param context: context used to render the page
    :type context: dict

    :param doctree: doctree of the page being rendered
    :type doctree: docutils.nodes.document
    """

    # https://github.com/sphinx-doc/sphinx/blob/7138d03ba033e384f1e7740f639849ba5f2cc71d/sphinx/builders/html.py#L1054-L1065
    def pathto(otheruri, resource=False, baseuri=None):
        """
        Hack pathto to display absolute URL's.

        Instead of calling ``relative_url`` function, we call
        ``app.builder.get_target_uri`` to get the absolute URL.

        .. note::

            If ``otheruri`` is a external ``resource`` it does not modify it.
            If ``otheruri`` is a static file on Read the Docs it does not modify it.
        """
        current_page = {}
        for page in app.config.staticpages_pages:
            if page['pagename'] == pagename:
                current_page = page
                break
        READTHEDOCS = os.environ.get('READTHEDOCS', False) == 'True'

        if resource and '://' in otheruri:
            # allow non-local resources given by scheme
            return otheruri

        if READTHEDOCS and otheruri.startswith('/_/'):
            # special case on Read the Docs
            return otheruri

        if not resource:
            otheruri = app.builder.get_target_uri(otheruri)

        if baseuri is None:
            if current_page['urls_prefix'] is None:
                baseuri = '/'
            else:
                baseuri = '{prefix}'.format(
                    prefix=current_page['urls_prefix'] or '/',
                )

        if not baseuri.startswith('/'):
            raise BaseURIError('"baseuri" must be absolute')

        if otheruri and not otheruri.startswith('/'):
            otheruri = '/' + otheruri

        if otheruri:
            if baseuri.endswith('/'):
                baseuri = baseuri[:-1]
            otheruri = baseuri + otheruri

        uri = otheruri or '#'
        return uri

    # https://github.com/sphinx-doc/sphinx/blob/2adeb68af1763be46359d5e808dae59d708661b1/sphinx/builders/html.py#L1081
    def toctree(*args, **kwargs):
        current_page = {}
        for page in app.config.staticpages_pages:
            if page['pagename'] == pagename:
                current_page = page
                break
        try:
            # Sphinx >= 1.6
            from sphinx.environment.adapters.toctree import TocTree
            get_toctree_for = TocTree(app.env).get_toctree_for
        except ImportError:
            # Sphinx < 1.6
            get_toctree_for = app.env.get_toctree_for

        toc = get_toctree_for(
            current_page['pagename'],
            app.builder,
            collapse=kwargs.pop('collapse', False),
            includehidden=kwargs.pop('includehidden', False),
            **kwargs  # not using trailing comma here makes this compatible with
                      # Python2 syntax
        )

        # If no TOC is found, just return ``None`` instead of failing here
        if not toc:
            return None

        replace_uris(app, toc, docutils.nodes.reference, 'refuri')
        return app.builder.render_partial(toc)['fragment']

    # Borrowed from Sphinx<4.x to backward compatibility
    # https://github.com/sphinx-doc/sphinx/blob/v3.5.4/sphinx/builders/html/__init__.py#L1003-L1010
    def css_tag(css):
        attrs = []
        for key in sorted(css.attributes):
            value = css.attributes[key]
            if value is not None:
                if sphinx.version_info < (2, 0):
                    # https://github.com/sphinx-doc/sphinx/blob/v1.8.5/sphinx/builders/html.py#L1144
                    from sphinx.util.pycompat import htmlescape
                    attrs.append('%s="%s"' % (key, htmlescape(value, True)))
                else:
                    import html
                    attrs.append('%s="%s"' % (key, html.escape(value, True)))
        attrs.append('href="%s"' % pathto(css.filename, resource=True))
        return '<link %s />' % ' '.join(attrs)

    # Apply our custom manipulation for each <pagename>.html page only
    pagenames = []
    pagenames.extend(page['pagename'] for page in app.config.staticpages_pages)
    if pagename in pagenames:
        # Override the ``pathto`` helper function from the context to use a custom one
        # https://www.sphinx-doc.org/en/master/templating.html#pathto
        context['pathto'] = pathto

        # Override the ``toctree`` helper function from context to use a custom
        # one and generate valid links on not found page.
        # https://www.sphinx-doc.org/en/master/templating.html#toctree
        # NOTE: not used on ``singlehtml`` builder for RTD Sphinx theme
        context['toctree'] = toctree

        if sphinx.version_info < (4, 0) or sphinx.version_info >= (7, 0):
            context['css_tag'] = css_tag


# https://www.sphinx-doc.org/en/stable/extdev/appapi.html#event-doctree-resolved
def doctree_resolved(app, doctree, docname):
    """
    Generate and override URLs for ``.. image::`` Sphinx directive.

    When ``.. image::`` is used in the ``<pagename>.rst`` file, this function will
    override the URLs to point to the right place.

    :param app: Sphinx Application
    :type app: sphinx.application.Sphinx
    :param doctree: doctree representing the document
    :type doctree: docutils.nodes.document
    :param docname: name of the document
    :type docname: str
    """

    pagenames = []
    pagenames.extend(item['pagename'] for item in app.config.staticpages_pages)
    if docname in pagenames:
        # Replace image ``uri`` to its absolute version
        replace_uris(app, doctree, docutils.nodes.image, 'uri')


class OrphanMetadataCollector(EnvironmentCollector):
    """
    Force the staicpages pages to be ``orphan``.

    This way we remove the WARNING that Sphinx raises saying the page is not
    included in any toctree.

    This collector has the same effect than ``:orphan:`` at the top of the page.
    """

    def clear_doc(self, app, env, docname):
        return None

    def process_doc(self, app, doctree):
        for page in app.config.staticpages_pages:
            metadata = app.env.metadata[page['pagename']]
            metadata.update({'orphan': True})
            if sphinx.version_info >= (3, 0, 0):
                metadata.update({'nosearch': True})

    def merge_other(self, app, env, docnames, other):
        """Merge in specified data regarding docnames from a different `BuildEnvironment`
        object which coming from a subprocess in parallel builds."""
        # TODO: find an example about why this is strictly required for parallel read
        # https://github.com/readthedocs/sphinx-notfound-page/pull/112/files#r498219556
        env.metadata.update(other.metadata)

def validate_configs(app, *args, **kwargs):
    """
    Validate configs.

    Shows a warning if one of the configs is not valid.
    """
    for page in app.config.staticpages_pages:
        if 'pagename' not in page:
            message = 'pagename is required for each page'
            warnings.warn(message, UserWarning, stacklevel=2)
        if 'template' not in page:
            page.update({'template': 'page.html'})
            message = 'template is required for each page, using default'
            warnings.warn(message, UserWarning, stacklevel=2)
        if 'context' not in page:
            page.update({'context': {
                'title': 'Page Title',
                'body': "<h1>Page Title</h1>\n\nThis is a placeholder page.",
            }})
            message = 'context is required for each page, using default'
            warnings.warn(message, UserWarning, stacklevel=2)
        if 'urls_prefix' not in page:
            message = 'urls_prefix is required for each page'
            warnings.warn(message, UserWarning, stacklevel=2)

def setup(app):
    default_pages = [{
        'template': 'page.html',
        'context': {
            'title': 'Versions Index',
            'body': "<h1>Versions Index</h1>\n\nThis is a placeholder page for versions index.",
        },
        'pagename': 'versions_index',
        'urls_prefix': '/en/latest/',
    }]
    app.add_config_value('staticpages_pages', default_pages, 'html')
    # FIXME this config shouldn't exist, see _ext/staticpages/utils.py
    app.add_config_value('staticpages_urls_prefix', None, 'html')

    if sphinx.version_info > (1, 8, 0):
        app.connect('config-inited', validate_configs)
    else:
        app.connect('builder-inited', validate_configs)

    app.connect('html-collect-pages', html_collect_pages)

    if sphinx.version_info >= (3, 0, 0):
        # Use ``priority=400`` argument here because we want to execute our function
        # *before* Sphinx's ``setup_resource_paths`` where the ``logo_url`` and
        # ``favicon_url`` are resolved.
        # See https://github.com/readthedocs/sphinx-notfound-page/issues/180#issuecomment-959506037
        app.connect('html-page-context', finalize_media, priority=400)
    else:
        app.connect('html-page-context', finalize_media)

    app.connect('doctree-resolved', doctree_resolved)

    # Sphinx injects some javascript files using ``add_js_file``. The path for
    # this file is rendered in the template using ``js_tag`` instead of
    # ``pathto``. The ``js_tag`` uses ``pathto`` internally to resolve these
    # paths, we call again the setup function for this tag *after* the context
    # was overridden by our extension with the patched ``pathto`` function.
    if sphinx.version_info < (7, 0):
        if sphinx.version_info >= (1, 8):
            from sphinx.builders.html import setup_js_tag_helper
            app.connect('html-page-context', setup_js_tag_helper)

        if sphinx.version_info >= (4, 0):
            # CSS are now added via a ``css_tag``
            # https://github.com/sphinx-doc/sphinx/pull/8643
            from sphinx.builders.html import setup_css_tag_helper
            app.connect('html-page-context', setup_css_tag_helper)

    app.add_env_collector(OrphanMetadataCollector)

    return {
        'version': __version__,
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }
