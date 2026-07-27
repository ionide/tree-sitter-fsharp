"""F# grammars for tree-sitter.

``language()`` is the grammar for implementation files (``.fs``, ``.fsx``);
``language_signature()`` is the grammar for signature files (``.fsi``).
"""

from importlib.resources import files as _files

from ._binding import language, language_signature


def _get_query(name, file):
    try:
        query = _files(f"{__package__}") / file
        globals()[name] = query.read_text()
    except FileNotFoundError:
        globals()[name] = None
    return globals()[name]


def __getattr__(name):
    if name == "HIGHLIGHTS_QUERY":
        return _get_query("HIGHLIGHTS_QUERY", "queries/highlights.scm")
    if name == "INJECTIONS_QUERY":
        return _get_query("INJECTIONS_QUERY", "queries/injections.scm")
    if name == "LOCALS_QUERY":
        return _get_query("LOCALS_QUERY", "queries/locals.scm")
    if name == "TAGS_QUERY":
        return _get_query("TAGS_QUERY", "queries/tags.scm")
    if name == "SIGNATURE_HIGHLIGHTS_QUERY":
        return _get_query(
            "SIGNATURE_HIGHLIGHTS_QUERY", "queries/signature/highlights.scm"
        )
    if name == "SIGNATURE_TAGS_QUERY":
        return _get_query("SIGNATURE_TAGS_QUERY", "queries/signature/tags.scm")

    raise AttributeError(f"module {__name__!r} has no attribute {name!r}")


__all__ = [
    "language",
    "language_signature",
    "HIGHLIGHTS_QUERY",
    "INJECTIONS_QUERY",
    "LOCALS_QUERY",
    "TAGS_QUERY",
    "SIGNATURE_HIGHLIGHTS_QUERY",
    "SIGNATURE_TAGS_QUERY",
]


def __dir__():
    return sorted(__all__ + [
        "__all__", "__builtins__", "__cached__", "__doc__", "__file__",
        "__loader__", "__name__", "__package__", "__path__", "__spec__",
    ])
