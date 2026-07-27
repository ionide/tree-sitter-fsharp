from typing import Final
from typing_extensions import CapsuleType

HIGHLIGHTS_QUERY: Final[str] | None
"""The syntax highlighting query for the implementation grammar."""

INJECTIONS_QUERY: Final[str] | None
"""The language injection query for the implementation grammar."""

LOCALS_QUERY: Final[str] | None
"""The local variable query for the implementation grammar."""

TAGS_QUERY: Final[str] | None
"""The symbol tagging query for the implementation grammar."""

SIGNATURE_HIGHLIGHTS_QUERY: Final[str] | None
"""The syntax highlighting query for the signature grammar."""

SIGNATURE_TAGS_QUERY: Final[str] | None
"""The symbol tagging query for the signature grammar."""

def language() -> CapsuleType:
    """The tree-sitter language function for F# implementation files (.fs, .fsx)."""

def language_signature() -> CapsuleType:
    """The tree-sitter language function for F# signature files (.fsi)."""
